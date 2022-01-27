#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#include "math_lib.h"
#include "preview.cpp" // OpenGL + GLFW (for blitting our buffer to the screen)
#include "camera.cpp"
#include "draw.cpp"

int main()
{
    pixel_buffer_f32 frame_buffer = {};
    frame_buffer.width = 1280;
    frame_buffer.height = 720;
    frame_buffer.bytes_per_pixel = sizeof(v3); // RGB
    frame_buffer.frame_buffer_size_in_bytes = frame_buffer.width * frame_buffer.height * frame_buffer.bytes_per_pixel;
    frame_buffer.pixels = (f32 *)malloc(frame_buffer.frame_buffer_size_in_bytes);
    frame_buffer.depth_buffer_size_in_bytes = frame_buffer.width * frame_buffer.height * sizeof(f32);
    frame_buffer.depth = (f32 *)malloc(frame_buffer.depth_buffer_size_in_bytes);
    frame_buffer.depth_check_enabled = 1;
    frame_buffer.left = -(s32)frame_buffer.width * 0.5f;
    frame_buffer.right = frame_buffer.width * 0.5f;
    frame_buffer.top = -(s32)frame_buffer.height * 0.5f;
    frame_buffer.bottom = (s32)frame_buffer.height * 0.5f;
    
    projection_data proj = {};
    proj.viewport = V3(1, 1, 1);
    proj.canvas_width = frame_buffer.width;
    proj.canvas_height = frame_buffer.height;
    proj.camera.origin = {};
    proj.camera.rotation = {};
    proj.camera.pan_speed = 13.0f;
    proj.camera.turn_speed = 13.0f;
    
    f32 inv_sqrt2 = 1 / square_root(2);
    proj.clip.near = V3(0, 0, 1);
    proj.clip.left = V3(inv_sqrt2, 0, inv_sqrt2);
    proj.clip.right = V3(-inv_sqrt2, 0, inv_sqrt2);
    proj.clip.bottom = V3(0, inv_sqrt2, inv_sqrt2);
    proj.clip.top = V3(0, -inv_sqrt2, inv_sqrt2);
    
    f32 rotation_speed = 50.0f;
    f32 fps_timeout = 0.0f;
    
    preview_context context = setup_preview_window(frame_buffer.width, frame_buffer.height);
    
    model_properties p[] = 
    {
        { V3(0, 0, 5), V3(-1.5f, 0, 7), {}, {} },
        { V3(1, 2, 3), V3(1.5f, 0, 7), {}, {} },
        
    };
    
    model_instance cube_instance;
    cube_instance.attributes = &cube_verts[0];
    cube_instance.indices = &cube_vert_indices[0];
    cube_instance.vertex_count = array_count(cube_verts);
    cube_instance.index_count = array_count(cube_vert_indices);
    cube_instance.triangle_count = cube_instance.index_count / 3;
    cube_instance.model_count = 0;
    cube_instance.bounding_sphere = min_bounding_sphere(cube_instance.attributes, cube_instance.vertex_count);
    
    push_models_to_instance(&cube_instance, &p[0], array_count(p));
    
    while(context.active)
    {
        clear(frame_buffer, COLOR_BUFFER | DEPTH_BUFFER);
        start_frame(&context);
        
        poll_update_camera_position(&context, &proj.camera);
        
        if(get_key(&context, GLFW_KEY_Q, GLFW_PRESS))
        {
            cube_instance.rotation[0].z += rotation_speed * context.step;
        }
        
        if(get_key(&context, GLFW_KEY_E, GLFW_PRESS))
        {
            cube_instance.rotation[0].z -= rotation_speed * context.step;
        }
        
        if(get_key(&context, GLFW_KEY_Z, GLFW_PRESS))
        {
            cube_instance.rotation[0].x += rotation_speed * context.step;
        }
        
        if(get_key(&context, GLFW_KEY_C, GLFW_PRESS))
        {
            cube_instance.rotation[0].x -= rotation_speed * context.step;
        }
        
        if(get_key(&context, GLFW_KEY_R, GLFW_PRESS))
        {
            cube_instance.rotation[0].y += rotation_speed * context.step;
        }
        
        if(get_key(&context, GLFW_KEY_T, GLFW_PRESS))
        {
            cube_instance.rotation[0].y -= rotation_speed * context.step;
        }
        
        // clip the model
        // -> clip each intersecting triangle
        //    -> generate new vertices
        
        render_instance(&frame_buffer, &cube_instance, &proj, FILL);
        draw_fps_timeout(&frame_buffer, context.step, &fps_timeout, 5, 5, 5, V3(1.0f, 1.0f, 1.0f));
        end_frame(&context, frame_buffer.pixels);
    }
    
    return 0;
}