//
//  fokplayer.cpp
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fokplayer.h"

void _FokTaskFreePkt(void *i)
{
    AVPacket *pkt = i;
    av_packet_free(&pkt);
}

int64_t fok_player_fill_audio(FokPlayer *player)
{
    if (player->abort) {
        return 0;
    }
    FokQueueTask *task = fok_audio_decoderpar_dequeue(player->audioeng);
    if (!task) {
        return FokNullPacketDuration;
    }
    FokFrame *frame = task->data;
    fok_frame_render_receive(player->audio_render, frame);
    fok_audio_decoderpar_next(player->audioeng);
    fok_clock_display(&player->aclock, av_gettime_relative() / 1000, frame->pts);
    int64_t display = frame->duration ?: player->audioeng->frenquency;
//    fok_log("fok_player_fill_audio ->display:%lld", display);
    return display;
}

int64_t fok_player_fill_video(FokPlayer *player)
{
retry:
    if (player->abort) {
        return 0;
    }
    FokQueueTask *task = fok_video_decoderpar_dequeue(player->videoeng);
    if (!task) { //其实这里应该等待信号
        return FokNullPacketDuration;
    }
    FokFrame *frame = task->data;
    int64_t avg_display = frame->duration ?: (1 / player->videoeng->fps) * 1000;
    int64_t sync_diff = player->aclock.time.num - player->vclock.time.num;
    if (sync_diff > FokFrameSyncDiff) {//drop frame
        fok_video_decoderpar_next(player->videoeng);
        fok_clock_display(&player->vclock, av_gettime_relative() / 1000, frame->pts);
        fok_log("fok_player_fill_video ->diff:%lld drop the video frame", sync_diff);
        goto retry;
    }
    int64_t display = 0;
    if (sync_diff > 0) { //音频快, 视频加速，追赶音频
        display = FokMAX(avg_display - sync_diff, 0);
    } else { //视频快, 视频减速，等待音频
        display = FokMAX((llabs(sync_diff) - avg_display), avg_display);
    }
    fok_frame_render_receive(player->video_render, frame);
    fok_video_decoderpar_next(player->videoeng);
    fok_clock_display(&player->vclock, av_gettime_relative() / 1000, frame->pts);
    fok_log("fok_player_fill_video ->diff:%lld ->display:%lld", sync_diff, display);
    return display;
}

FokPlayer *fok_player_init(void)
{
    FokPlayer *player = fok_malloc(sizeof(FokPlayer));
    player->vpk_queue = fok_queue_alloc(FokQueueFIFO);
    player->apk_queue = fok_queue_alloc(FokQueueFIFO);
    pthread_mutex_init(&player->lock, NULL);
    pthread_mutex_init(&player->vpk_lock, NULL);
    pthread_mutex_init(&player->apk_lock, NULL);
    return player;
}

void fok_player_parse_avpacket(FokPlayer *player, AVPacket *pkt)
{
    /*
    int64_t diff = pkt->pts - player->aclock.time.num;
    if (diff < 0) { 
        av_packet_free(&pkt);
        fok_log("AVPacket drop with diff %lld", diff);
        return;
    }
     */
    //video pkt
    if (fok_decoder_pair_packet(player->videoeng->decoder, pkt))
    {
        pthread_mutex_lock(&player->vpk_lock);
        fok_queue_enqueue_func(player->vpk_queue, pkt, pkt->size, NULL, &_FokTaskFreePkt);
        pthread_mutex_unlock(&player->vpk_lock);
        
        if (player->vrefresh_thread->waiting == 1) {
            fok_thread_singal(player->vrefresh_thread);
        }
    }
    //audio pkt
    else if (fok_decoder_pair_packet(player->audioeng->decoder, pkt))
    {
        pthread_mutex_lock(&player->apk_lock);
        fok_queue_enqueue_func(player->apk_queue, pkt, pkt->size, NULL, &_FokTaskFreePkt);
        pthread_mutex_unlock(&player->apk_lock);
        
        if (player->arefresh_thread->waiting == 1) {
            fok_thread_singal(player->arefresh_thread);
        }
    }
    //other unsupport
    else
    {
        av_packet_free(&pkt);
    }
}

void fok_runable_read(FokThread *thread, void *info)
{
    FokPlayer *player = info;
    while (1)
    {
        if (player->abort || thread->abort)
        {
            goto clean;
            break;
        }
        if (!player->play) {
            fok_thread_wait(thread);
        }else{
            if (player->vpk_queue->count > FokPacketQueueMaxSize || player->apk_queue->count > FokPacketQueueMaxSize)
            {
                fok_thread_wait(thread);
            }
            else
            {
                //codec
                AVPacket *pkt = av_packet_alloc();
                int ret = av_read_frame(player->ctx, pkt);
                if (ret < 0)//error or eof
                {
                    av_packet_free(&pkt);
                    if (ret == AVERROR_EOF) {
                        goto eof;
                        break;
                    }
                    fok_thread_wait_timeout(thread, 10);
                    continue;
                }
                                
                fok_player_parse_avpacket(player, pkt);
            }
        }
    }
    
clean:
    fok_log("fok_runable_read thread exit");
    
eof:
    fok_player_pause(player);
}

void fok_runable_video(FokThread *thread, void *info)
{
    FokPlayer *player = info;
    while (1)
    {
        if (player->abort || thread->abort)
        {
            goto clean;
            break;
        }
        
        //解码
        pthread_mutex_lock(&player->vpk_lock);
        FokQueueTask *task = fok_queue_task_dequeue(player->vpk_queue);
        if (task) {
            fok_video_decode_packet(player->videoeng, task->data);
            fok_queue_task_next(player->vpk_queue);
        }
        pthread_mutex_unlock(&player->vpk_lock);
        //填充
        if (!player->play) {
            fok_thread_wait(thread);
        } else {
            if (!player->abort &&
                (player->vpk_queue->count <= FokPacketQueueMinSize || player->apk_queue->count <= FokPacketQueueMinSize) &&
                player->read_thread->waiting == 1)
            {
                fok_thread_singal(player->read_thread);
            }
            int64_t usleep = fok_player_fill_video(player);
            if (usleep > 0) {
                fok_usleep((fok_usec)usleep);
            }
        }
                
    }
    
clean:
    fok_log("fok_runable_video thread exit");
    
    fok_queue_abort(player->vpk_queue);
    fok_queue_free_all_task(player->vpk_queue);
    fok_queue_free(player->vpk_queue);
    
    fok_queue_abort(player->videoeng->queue);
    fok_queue_free_all_task(player->videoeng->queue);
    fok_video_decoderpar_free(player->videoeng);
}

void fok_runable_audio(FokThread *thread, void *info)
{
    FokPlayer *player = info;
    while (1)
    {
        if (player->abort || thread->abort)
        {
            goto clean;
            break;
        }
        //解码
        pthread_mutex_lock(&player->apk_lock);
        FokQueueTask *task = fok_queue_task_dequeue(player->apk_queue);
        if (task) {
            fok_audio_decode_packet(player->audioeng, task->data);
            fok_queue_task_next(player->apk_queue);
        }
        pthread_mutex_unlock(&player->apk_lock);
        //填充
        if (!player->play) {
            fok_thread_wait(thread);
        } else {
            if (!player->abort &&
                player->vpk_queue->count <= FokPacketQueueMinSize &&
                player->apk_queue->count <= FokPacketQueueMinSize &&
                player->read_thread->waiting == 1)
            {
                fok_thread_singal(player->read_thread);
            }
            int64_t usleep = fok_player_fill_audio(player);
            if (usleep > 0) {
                fok_usleep((fok_usec)usleep);
            }
        }
        
    }
    
clean:
    fok_log("fok_runable_audio thread exit");
    
    fok_queue_abort(player->apk_queue);
    fok_queue_free_all_task(player->apk_queue);
    fok_queue_free(player->apk_queue);
    
    fok_queue_abort(player->audioeng->queue);
    fok_queue_free_all_task(player->audioeng->queue);
    fok_audio_decoderpar_free(player->audioeng);
}

void fok_player_free(FokPlayer *player)
{
    fok_frame_render_free(player->video_render);
    fok_frame_render_free(player->audio_render);
    pthread_mutex_destroy(&player->lock);
    pthread_mutex_destroy(&player->vpk_lock);
    pthread_mutex_destroy(&player->apk_lock);
    fok_free(player);
}

FokStatus fok_player_open_input(FokPlayer *player, const char *url)
{
    if (!player || !url) {
        return FokStatusError;
    }
    //input
    if (avformat_open_input(&player->ctx, url, NULL, NULL) < 0) {
        return FokStatusError;
    }
    //stream
    if (avformat_find_stream_info(player->ctx, NULL) < 0) {
        return FokStatusError;
    }
#if DEBUG
    av_dump_format(player->ctx, -1, NULL, 0);
#endif
    //codec
    AVStream *v_st = fok_av_get_stream(player->ctx, AVMEDIA_TYPE_VIDEO);
    if (v_st)
        player->videoeng = fok_video_decoderpar_alloc(player, v_st);

    AVStream *a_st = fok_av_get_stream(player->ctx, AVMEDIA_TYPE_AUDIO);
    if (a_st)
        player->audioeng = fok_audio_decoderpar_alloc(player, a_st);

    player->aclock = fok_clock_make();
    player->vclock = fok_clock_make();
    
    player->arefresh_thread = fok_thread_alloc(fok_runable_audio, player, "fok_audio");
    player->vrefresh_thread = fok_thread_alloc(fok_runable_video, player, "fok_video");
    player->read_thread = fok_thread_alloc(fok_runable_read, player, "fok_read_frame");
    
    return FokStatusOK;
}

void fok_player_close_input(FokPlayer *player)
{
    if (!player) {
        return;
    }
    pthread_mutex_lock(&player->lock);
    player->abort = 1;
    pthread_mutex_unlock(&player->lock);
        
    //destory threads
    fok_thread_destory(player->read_thread);
    fok_thread_destory(player->vrefresh_thread);
    fok_thread_destory(player->arefresh_thread);
    
    fok_thread_join(player->read_thread);
    //close input
    avformat_close_input(&player->ctx);

    fok_thread_free(player->arefresh_thread);
    fok_thread_free(player->vrefresh_thread);
    fok_thread_free(player->read_thread);
}


void fok_player_play(FokPlayer *player)
{
    if (!player || player->play == 1) {
        return;
    }
    pthread_mutex_lock(&player->lock);
    player->play = 1;
    pthread_mutex_unlock(&player->lock);
    
    fok_thread_singal(player->read_thread);
}

void fok_player_pause(FokPlayer *player)
{
    if (!player || !player->play) {
        return;
    }
    
    pthread_mutex_lock(&player->lock);
    player->play = 0;
    pthread_mutex_unlock(&player->lock);
}

void fok_player_set_video_render(FokPlayer *player, void *render, FokRenderCallback callback)
{
    if (!player) {
        return;
    }
    fok_frame_render_free(player->video_render);
    if (render) {
        player->video_render = fok_frame_render_alloc(render, FokTypeVideo, callback);
    }
}

void fok_player_set_audio_render(FokPlayer *player, void *render, FokRenderCallback callback)
{
    if (!player) {
        return;
    }
    fok_frame_render_free(player->audio_render);
    if (render) {
        player->audio_render = fok_frame_render_alloc(render, FokTypeAudio, callback);
    }
}
