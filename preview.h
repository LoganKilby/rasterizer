/* date = October 26th 2021 2:57 pm */

#ifndef PREVIEW_H
#define PREVIEW_H

#define AssertUniformLoc(Integer) if(Integer == -1) {*(int *)0 = 0;}

struct preview_context
{
    u32 screen_quad_vao;
    u32 shader_id;
    u32 screen_texture;
    u32 texture_width;
    u32 texture_height;
    
    GLFWwindow *window;
    
    GLsync fence;
    s32 fence_status = GL_SIGNALED;
};

#endif //PREVIEW_H
