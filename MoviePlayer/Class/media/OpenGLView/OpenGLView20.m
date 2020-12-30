//
//  OpenGLView.m
//  MyTest
//
//  Created by smy on 12/20/11.
//  Copyright (c) 2011 ZY.SYM. All rights reserved.
//

#import "OpenGLView20.h"
#import "OpenGLView20+Render.h"
#include "fok_opengles2_render.h"

#define TEXY 0
#define TEXU 1
#define TEXV 2

#define ATTRIB_VERTEX 0
#define ATTRIB_TEXTURE 1

//包含1-4象限内
static const GLfloat gl20Vertices[] = {
    -1.0, -1.0,
    1.0, -1.0,
    -1.0,  1.0,
    1.0,  1.0,
};

//1象限内
static const GLfloat gl20Textures[] = {
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
};

@implementation OpenGLView20

- (void)dealloc{
    fok_opengl_delete_i420_texture(_textureYUV);
    fok_opengl_delete_draw_buffer(&_framebuffer, &_renderBuffer);
}

- (id)initWithCoder:(NSCoder *)aDecoder{
    self = [super initWithCoder:aDecoder];
    [self doInit];
    return self;
}

- (id)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    [self doInit];
    return self;
}

- (instancetype)init{
    return [self initWithFrame:CGRectZero];
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (BOOL)doInit{
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = @{
        kEAGLDrawablePropertyRetainedBacking : @(NO),
        kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8
    };
    self.contentScaleFactor = [UIScreen mainScreen].scale;
    _glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
    _lock = [[NSLock alloc] init];

    if (![EAGLContext setCurrentContext:_glContext]){
        return NO;
    }
    [self setOpenGL];
    return YES;
}

- (void)setOpenGL{
    fok_opengl_gen_i420_texture(_textureYUV);
    int vshl;
    GLuint vsh_handle;
    const char *vsh = opengl20_shader_code(@"Vsh.glsl", &vshl);
    fok_opengl_complie_shader(vsh, vshl, GL_VERTEX_SHADER, &vsh_handle);
    
    int fshl;
    GLuint fsh_handle;
    const char *fsh = opengl20_shader_code(@"Fsh.glsl", &fshl);
    fok_opengl_complie_shader(fsh, fshl, GL_FRAGMENT_SHADER, &fsh_handle);
    
    fok_opengl_create_program(&_program, vsh_handle, "position", ATTRIB_VERTEX, fsh_handle, "TexCoordIn", ATTRIB_TEXTURE);
    
    glUniform1i(glGetUniformLocation(_program, "SamplerY"), TEXY);
    glUniform1i(glGetUniformLocation(_program, "SamplerU"), TEXU);
    glUniform1i(glGetUniformLocation(_program, "SamplerV"), TEXV);
}

- (BOOL)createFrameAndRenderBuffer:(CAEAGLLayer *)layer{
    fok_opengl_delete_draw_buffer(&_framebuffer, &_renderBuffer);
    fok_opengl_gen_draw_buffer(&_framebuffer, &_renderBuffer);
    if (![_glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer]) {
        return NO;
    }
    if (fok_opengl_bind_draw_buffer(_framebuffer) != 0) {
        return NO;
    }
    return YES;
}

#pragma mark - 接口
- (void)displayYUV420pData:(uint8_t *)data width:(GLsizei)w height:(GLsizei)h{
    if (!self.window){
        return;
    }
    [_lock lock];
        [self setVideoSize:w height:h];
        CGRect glViewRect = [self glViewRect];
        [EAGLContext setCurrentContext:_glContext];
        fok_opengl_fill_i420_buffer(data, w, h, _textureYUV[TEXY], _textureYUV[TEXU], _textureYUV[TEXV]);
        glViewport(glViewRect.origin.x, glViewRect.origin.y, glViewRect.size.width, glViewRect.size.height);
        fok_opengl_draw(ATTRIB_VERTEX, gl20Vertices, ATTRIB_TEXTURE, gl20Textures);
        [_glContext presentRenderbuffer:GL_RENDERBUFFER];
    [_lock unlock];
    [self debugFramerate];
}

- (void)layoutSubviews {
    CAEAGLLayer *layer = (CAEAGLLayer *)self.layer;
    CGRect glViewRect = [self glViewRect];
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        [self.lock lock];
            [EAGLContext setCurrentContext:self.glContext];
            [self createFrameAndRenderBuffer:layer];
        [self.lock unlock];
        glViewport(glViewRect.origin.x, glViewRect.origin.y, glViewRect.size.width, glViewRect.size.height);
    });
}


- (void)setVideoSize:(GLsizei)width height:(GLsizei)height{
    if (_videoW != width)
        _videoW = width;
    if (_videoH != height)
        _videoH = height;
}

- (void)debugFramerate{
#ifdef DEBUG
    GLenum err = glGetError();
    if (err != GL_NO_ERROR){
        printf("GL_ERROR=======>%d\n", err);
    }
    struct timeval nowtime;
    gettimeofday(&nowtime, NULL);
    if (nowtime.tv_sec != _time.tv_sec){
        printf("帧率:   %td\n", _frameRate);
        memcpy(&_time, &nowtime, sizeof(struct timeval));
        _frameRate = 0;
    }else{
        _frameRate++;
    }
#endif
}

@end



