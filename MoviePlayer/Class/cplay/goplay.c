//
//  goplay.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#include "goplay.h"

void g_player_on_message(GMessageQueue *q, GMessage *msg);

GPlayer *g_player_alloc(void)
{
    GPlayer *p = (GPlayer *)gmalloc(sizeof(GPlayer));
    if (p) {
        avformat_network_init();
        p->lock = g_mutex_alloc();
        p->video_param = (struct GVideoParamter){
            .aqueue_max = 20,
            .aqueue_min = 10,
            .vqueue_max = 20,
            .vqueue_min = 10,
            .sample_fmt = AV_SAMPLE_FMT_S16,
            .pixel_fmt = AV_PIX_FMT_YUV420P,
            .channels = 2
        };
        p->msg_queue = g_msg_queue_alloc("g_player_msg", p);
        p->msg_queue->on_message = g_player_on_message;
    }
    return p;
}

void g_player_release(GPlayer *p)
{
    if (p) {
        g_player_close_input(p);
        g_msg_queue_release(p->msg_queue);
        avformat_network_deinit();
        g_mutex_release(p->lock);
    }
    gfree(p);
}

void g_player_counting_wait(GPlayer *p)
{
    if (p->abort)
        return;
    
    if (
        !p->packet_thread->waiting &&
        ((!p->audio_queue || p->audio_queue->size > p->video_param.aqueue_max) ||
         (!p->video_queue || p->video_queue->size > p->video_param.vqueue_max))
        )
    {
        g_thread_local_wait(p->packet_thread);
    }
    else if (
             p->packet_thread->waiting &&
             ((!p->audio_queue || p->audio_queue->size < p->video_param.aqueue_min) ||
              (!p->video_queue || p->video_queue->size < p->video_param.vqueue_min))
             )
    {
        g_thread_local_signal(p->packet_thread);
    }
}

void g_player_video_thread(GThread *t, GPointer data)
{
    GPlayer *player = data;
    while (1)
    {
    retry:
        if (player->abort || !player->video_thread || player->video_thread->exit) {
            goto clean; break;
        }
        if (!player->playing) {
            g_thread_local_wait(player->video_thread); continue;
        }
                            
        GQueueTask *task = g_queue_dequeue(player->video_queue);
        if (!task) {
            g_thread_local_wait(player->video_thread); continue;
        }
        int64_t pts = g_frame_get_pts(task->data);
//        int64_t avg_display = g_avg_display(player->video_param.avg_fps);
//        int64_t display_diff = GMIN(g_frame_getv_duration(task->data, player->video_clk), avg_display);
        int64_t display_diff = g_frame_getv_duration(task->data, player->video_clk);
        int64_t sync_diff = g_clock_diff(player->video_clk, player->audio_clk);
        //drop frame
        if (sync_diff > GPlayerDropDuration) {
            gprintf("g_player_video_thread sync will drop video frame (diff:%td)", sync_diff);
            g_clock_tick(&player->video_clk, pts, av_gettime_relative()/1000000.0);
            g_queue_task_release(task);
            goto retry;
        }
        int64_t delay = 0;
        if (sync_diff > GPlayerSyncDuration) {
            gprintf("g_player_video_thread sync will show direct video frame (diff:%td)", sync_diff);
            goto display;
        }
        if (sync_diff > 0)
        {
            delay = GMAX(display_diff - sync_diff, 0);//音频快, 视频加速，追赶音频
        }
        else
        {
            delay = GMAX((llabs(sync_diff) - display_diff), display_diff);//视频快, 视频减速，等待音频
        }
        
    display:
        GFnCall(player->video_render.render_frame, player->video_render, task, task->data);
        g_clock_tick(&player->video_clk, pts, av_gettime_relative()/1000000.0);
        g_player_counting_wait(player);
        gusleep((int)delay);
    }
    
clean:
    gprintf("g_player_video_thread clean");
    g_queue_release(player->video_queue);
    g_thread_local_release(&player->video_thread);
}

void g_player_audio_buffer(void *buffer, int32_t size, void **buf, void *userdata)
{
    GPlayer *player = userdata;
    if (*buf) {
        g_queue_task_release((GQueueTask *)*buf);
        *buf = NULL;
    }    
    GQueueTask *task = g_queue_dequeue(player->audio_queue);
    if (task) {
        *buf = task;
        int64_t pts = g_frame_get_pts(task->data);
        GFrame *f = task->data;
        memcpy(buffer, f->buf[0], f->linesize[0]);
        g_clock_tick(&player->audio_clk, pts, av_gettime_relative()/1000000.0);
        g_player_counting_wait(player);        
        gusleep(20);
    }else{
        memset(buffer, 0, size);
    }
}

void g_player_packet_thread(GThread *t, GPointer data)
{
    GPlayer *player = data;
    
    while (1)
    {
        if (player->abort || player->packet_thread->exit) {
            goto clean; break;
        }        
        if (!player->playing) {
            g_thread_local_wait(player->packet_thread); continue;
        }
        //codec
        AVPacket pkt;
        int ret = av_read_frame(player->ctx, &pkt);
        if (ret < 0)//error or eof
        {
            av_packet_unref(&pkt);
            if (ret == AVERROR_EOF){
                goto eof; break;
            }
            if (ret == AVERROR_EXIT) {
                goto eof; break;
            }
            
            g_thread_local_wait_timeout(player->packet_thread, 10);
            continue;
        }
                
        //video pkt
        if (pkt.stream_index == player->video_codec->stream->index)
        {
            AVCodecContext *ctx = player->video_codec->ctx;
            if (avcodec_send_packet(ctx , &pkt) == 0) {
                GFrame *f = g_frame_alloc();
                f->frame = av_frame_alloc();
                f->type = GFrameType_Video;
                
                if (avcodec_receive_frame(ctx , f->frame) == 0){
                    
                    GQueueTask *task = g_queue_task_alloc();
                    g_frame_fill_video_buf(f, player);
                    g_queue_task_ref(task, NULL, (GQueueTaskRelease)g_frame_release, f);
                    g_queue_enqueue_task(player->video_queue, task);
                    g_player_counting_wait(player);

                }else{
                    g_frame_release(f);
                }
            }
        }
        //audio pkt
        else if (pkt.stream_index == player->audio_codec->stream->index)
        {
            AVCodecContext *ctx = player->audio_codec->ctx;
            if (avcodec_send_packet(ctx , &pkt) == 0) {
                GFrame *f = g_frame_alloc();
                f->frame = av_frame_alloc();
                f->type = GFrameType_Audio;
                if (avcodec_receive_frame(ctx , f->frame) == 0){

                    GQueueTask *task = g_queue_task_alloc();
                    g_frame_fill_audio_buf(f, player);
                    g_queue_task_ref(task, NULL, (GQueueTaskRelease)g_frame_release, f);
                    g_queue_enqueue_task(player->audio_queue, task);
                    
                    g_player_counting_wait(player);

                }else{
                    g_frame_release(f);
                }
            }
        }
        /* else {} */ //unsupport
        av_packet_unref(&pkt);
        
        if (player->video_thread->waiting)
            g_thread_local_signal(player->video_thread);
                
    }
    
        
clean:
    gprintf("g_player_packet_thread clean");
    g_thread_local_release(&player->packet_thread);
    
eof:
    gprintf("g_player_packet_thread eof");
    g_player_pause(player);
}

void g_player_open_video_stream(GPlayer *p, AVStream *vst)
{
    if (!p)
        return;
    
    if (vst) {
        p->video_codec = g_codec_alloc(vst);
        p->video_queue = g_queue_alloc(0);
        AVCodecContext *ctx = p->video_codec->ctx;
        p->sws = sws_getContext(ctx->width,
                                ctx->height,
                                ctx->pix_fmt,
                                ctx->width,
                                ctx->height,
                                p->video_param.pixel_fmt,
                                SWS_FAST_BILINEAR,
                                NULL,
                                NULL,
                                NULL);
        p->video_thread = g_thread_local_alloc("g_video_thread", g_player_video_thread, p);
        p->video_clk = g_clock_make();
        g_av_stream_timebase_get(vst, ctx, 0.04, &p->video_param.avg_fps);
    }
}

void g_player_open_audio_stream(GPlayer *p, AVStream *ast)
{
    if (!p)
        return;

    if (ast) {
        p->audio_codec = g_codec_alloc(ast);
        p->audio_queue = g_queue_alloc(0);
        AVCodecContext *ctx = p->audio_codec->ctx;
        GFnCall(p->audio_render.open_engine, p->audio_render, ctx);
        p->swr = swr_alloc_set_opts(NULL,
                                    av_get_default_channel_layout(2),
                                    p->video_param.sample_fmt,
                                    ctx->sample_rate,
                                    av_get_default_channel_layout(ctx->channels),
                                    ctx->sample_fmt,
                                    ctx->sample_rate,
                                    0,
                                    NULL);
        swr_init(p->swr);
        p->audio_clk = g_clock_make();
        p->video_param.avf_frenqucy = g_av_audio_frenquency(ctx);
        GFnCall(p->audio_render.start, p->audio_render);
    }
}

void g_player_format_ctx_infos(AVDictionary *fmt_dict)
{
    gprintf("[Meta]:");
    AVDictionaryEntry *e = NULL;
    while ( (e = av_dict_get(fmt_dict, "", e, AV_DICT_IGNORE_SUFFIX)) )    
        gprintf("%s : %s", e->key, e->value);
}

void g_player_open_input(GPlayer *p, const char *url)
{
    if (!p)
        return;
    
    GMessage *msg = g_msg_alloc();
    msg->msgtype = GPlayerMsgType_OpenInput;
    msg->argv = (void *)url;
    g_msg_queue_push(p->msg_queue, msg);
}

void g_player_close_input(GPlayer *p)
{
    if (!p)
        return;
    if (p->abort) {
        return;
    }
    p->abort = 1;
    //停止音频队列
    GFnCall(p->audio_render.stop, p->audio_render);
    g_queue_release(p->audio_queue);
    //退出视频线程
    g_thread_local_exit(p->video_thread);
    //停止读取
    g_thread_local_exit(p->packet_thread);
    g_thread_local_join(p->packet_thread);

    g_codec_release(p->video_codec);
    g_codec_release(p->audio_codec);
    if (p->swr) {
        swr_free(&p->swr);
    }
    if (p->sws) {
        sws_freeContext(p->sws);
    }
    avformat_close_input(&p->ctx);
}

void g_player_play(GPlayer *p)
{
    if (!p)
        return;
    GMessage *msg = g_msg_alloc();
    msg->msgtype = GPlayerMsgType_Play;
    g_msg_queue_push(p->msg_queue, msg);
}

void g_player_pause(GPlayer *p)
{
    if (!p)
        return;    
    GMessage *msg = g_msg_alloc();
    msg->msgtype = GPlayerMsgType_Pause;
    g_msg_queue_push(p->msg_queue, msg);
}


void g_player_on_message(GMessageQueue *q, GMessage *msg)
{
    gprintf("g_player_on_message %d", msg->msgtype);
    GPlayer *p = q->ctx;
    if (msg->msgtype == GPlayerMsgType_Play)
    {
        if (p->playing)
            return;
        
        g_mutex_lock(p->lock);
        p->playing = 1;
        g_mutex_unlock(p->lock);
        g_thread_local_signal(p->packet_thread);
        gprintf("g_player_play");
    }
    else if (msg->msgtype == GPlayerMsgType_Pause)
    {
        if (!p->playing)
            return;

        g_mutex_lock(p->lock);
        p->playing = 0;
        g_mutex_unlock(p->lock);
        gprintf("g_player_pause");
    }
    else if (msg->msgtype == GPlayerMsgType_OpenInput)
    {
        const char *url = (const char *)msg->argv;
        //input
        if (avformat_open_input(&p->ctx, url, NULL, NULL) < 0) {
            return;
        }
        //stream
        if (avformat_find_stream_info(p->ctx, NULL) < 0) {
            return;
        }
    #if DEBUG
        g_player_format_ctx_infos(p->ctx->metadata);
    #endif
        if (p->ctx->nb_streams <= 0) {
            gprintf("Err: (url:%s) has no streams!", url);
            g_player_close_input(p);
            return;
        }
        //video
        g_player_open_video_stream(p, g_av_get_stream(p->ctx, AVMEDIA_TYPE_VIDEO));
        //audio
        g_player_open_audio_stream(p, g_av_get_stream(p->ctx, AVMEDIA_TYPE_AUDIO));
        //packet read
        p->packet_thread = g_thread_local_alloc("g_packet_thread", g_player_packet_thread, p);

    }
}
