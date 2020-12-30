//
//  fok_opengles2_render.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/25.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_opengles2_render_h
#define fok_opengles2_render_h

#include <stdio.h>

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

//创建framebuf与renderbuf
void fok_opengl_gen_draw_buffer(GLuint *fbuf, GLuint *rbuf);

//销毁framebuf与renderbuf
void fok_opengl_delete_draw_buffer(GLuint *fbuf, GLuint *rbuf);

//绑定renaderbuf
int fok_opengl_bind_draw_buffer(GLuint rbuf);

//创建纹理
void fok_opengl_gen_texture(GLenum texture, GLuint *texture_handle);

//创建yuv纹理
void fok_opengl_gen_i420_texture(GLuint *textures);

//销毁yuv纹理
void fok_opengl_delete_i420_texture(GLuint *textures);

//编译着色器
int fok_opengl_complie_shader(const GLchar *shader_code, GLint shader_len, GLenum shader_type, GLuint *shader_handle);

//创建opengl程序
int fok_opengl_create_program(GLuint *program_handle, GLuint vsh, const GLchar *vn, GLuint vidx, GLuint fsh, const GLchar *fn, GLuint fidx);

//填充i420数据
void fok_opengl_fill_i420_buffer(uint8_t *data, int width, int height, GLuint ty, GLuint tu, GLuint tv);

//绘制
void fok_opengl_draw(GLuint vidx, const GLvoid *vao, GLuint tidx, const GLvoid *tao);

//清除
void fok_opengl_clear_color(void);

#endif /* fok_opengles2_render_h */
