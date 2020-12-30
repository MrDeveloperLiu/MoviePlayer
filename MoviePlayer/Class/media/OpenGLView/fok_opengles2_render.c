//
//  fok_opengles2_render.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/25.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_opengles2_render.h"

void fok_opengl_gen_draw_buffer(GLuint *fbuf, GLuint *rbuf)
{
    glGenFramebuffers(1, fbuf);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbuf);
    
    glGenRenderbuffers(1, rbuf);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbuf);
}

void fok_opengl_delete_draw_buffer(GLuint *fbuf, GLuint *rbuf)
{
    if (fbuf && *fbuf) {
        glDeleteFramebuffers(1, fbuf);
    }
    if (rbuf && *rbuf) {
        glDeleteRenderbuffers(1, rbuf);
    }
}

int fok_opengl_bind_draw_buffer(GLuint rbuf)
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbuf);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return -1;    
    return 0;
}



void fok_opengl_gen_texture(GLenum texture, GLuint *texture_handle)
{
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, *texture_handle);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void fok_opengl_gen_i420_texture(GLuint *textures)
{
    glGenTextures(3, textures);
    fok_opengl_gen_texture(GL_TEXTURE0, &textures[0]);
    fok_opengl_gen_texture(GL_TEXTURE1, &textures[1]);
    fok_opengl_gen_texture(GL_TEXTURE2, &textures[2]);
}

void fok_opengl_delete_i420_texture(GLuint *textures)
{
    if (textures) {
        glDeleteTextures(3, textures);
    }
}

int fok_opengl_complie_shader(const GLchar *shader_code, GLint shader_len, GLenum shader_type, GLuint *shader_handle)
{
    GLuint s = glCreateShader(shader_type);
    glShaderSource(s, 1, &shader_code, &shader_len);
    glCompileShader(s);
    GLint state;
    glGetShaderiv(s, GL_COMPILE_STATUS, &state);
    if (state == GL_FALSE) {
        char messages[256];
        glGetShaderInfoLog(s, sizeof(messages), 0, &messages[0]);
        fok_log("glCompileShader error: %s", messages);
        return -1;
    }
    if (shader_handle) {
        *shader_handle = s;
    }
    return 0;
}

int fok_opengl_create_program(GLuint *program_handle, GLuint vsh, const GLchar *vn, GLuint vidx, GLuint fsh, const GLchar *fn, GLuint fidx)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vsh);
    glBindAttribLocation(program, vidx, vn);
    glAttachShader(program, fsh);
    glBindAttribLocation(program, fidx, fn);
    glLinkProgram(program);
    GLint state;
    glGetProgramiv(program, GL_LINK_STATUS, &state);
    if (state == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(program, sizeof(messages), 0, &messages[0]);
        fok_log("glLinkProgram error: %s", messages);
        return -1;
    }
    if (vsh)
        glDeleteShader(vsh);
    if (fsh)
        glDeleteShader(fsh);
    
    if (program_handle) {
        *program_handle = program;
    }
    glUseProgram(program);
    return 0;
}

void fok_opengl_calculate_size(size_t *sample_size, int *sample_width, int *sample_height, int vw, int vh)
{
    sample_width[0] = vw;
    sample_width[1] = vw >> 1;
    sample_width[2] = vw >> 1;
    
    sample_height[0] = vh;
    sample_height[1] = vh >> 1;
    sample_height[2] = vh >> 1;

    sample_size[0] = sample_width[0] * sample_height[0];
    sample_size[1] = sample_width[1] * sample_height[1];
    sample_size[2] = sample_width[2] * sample_height[2];
}


void fok_opengl_fill_i420_buffer(uint8_t *data, int width, int height, GLuint ty, GLuint tu, GLuint tv)
{
    size_t sampleSize[3] = {0};
    int sampleWidth[3] = {0};
    int sampleHeight[3] = {0};    
    fok_opengl_calculate_size(sampleSize, sampleWidth, sampleHeight, width, height);
    //luma
    glBindTexture(GL_TEXTURE_2D, ty);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT,
                 sampleWidth[0], sampleHeight[0], 0, GL_RED_EXT, GL_UNSIGNED_BYTE,
                 data);
    //chromaB
    glBindTexture(GL_TEXTURE_2D, tu);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT,
                 sampleWidth[1], sampleHeight[1], 0, GL_RED_EXT, GL_UNSIGNED_BYTE,
                 data + sampleSize[0]);
    //chromaR
    glBindTexture(GL_TEXTURE_2D, tv);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT,
                 sampleWidth[2], sampleHeight[2], 0, GL_RED_EXT, GL_UNSIGNED_BYTE,
                 data + (sampleSize[0] + sampleSize[1]));
}

void fok_opengl_draw(GLuint vidx, const GLvoid *vao, GLuint tidx, const GLvoid *tao)
{
    glVertexAttribPointer(vidx, 2, GL_FLOAT, 0, 0, vao);
    glEnableVertexAttribArray(vidx);
    
    glVertexAttribPointer(tidx, 2, GL_FLOAT, 0, 0, tao);
    glEnableVertexAttribArray(tidx);
        
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void fok_opengl_clear_color(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}
