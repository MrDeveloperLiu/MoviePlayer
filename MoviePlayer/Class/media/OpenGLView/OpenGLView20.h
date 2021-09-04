//
//  OpenGLView20.h
//  MyTest
//
//  Created by smy  on 12/20/11.
//  Copyright (c) 2011 ZY.SYM. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#include <sys/time.h>


@interface OpenGLView20 : UIView
{
    GLuint                  _framebuffer;
    GLuint                  _renderBuffer;
    GLuint                  _program;
    GLuint                  _videoW;
    GLuint                  _videoH;
	       
    GLuint                  _textureYUV[3];
#ifdef DEBUG
    struct timeval          _time;
    NSInteger               _frameRate;
#endif
}

@property (nonatomic, strong) UILabel *floatView;

@property (nonatomic, strong) EAGLContext             *glContext;
@property (nonatomic, strong) NSLock                  *lock;

#pragma mark - 接口
- (void)displayYUV420pData:(uint8_t *)data width:(GLsizei)w height:(GLsizei)h;
- (void)setVideoSize:(GLsizei)width height:(GLsizei)height;
@end


static inline const char * opengl20_shader_code(NSString *name, int *len)
{
    NSString *shaderPath = [NSBundle.mainBundle pathForResource:name ofType:nil];
    NSString *shaderString = [NSString stringWithContentsOfFile:shaderPath encoding:NSUTF8StringEncoding error:nil];
    if (len) {
        *len = (int)shaderString.length;
    }
    return shaderString.UTF8String;
}
