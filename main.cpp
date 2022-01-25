#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#include "math_lib.h"
#include "preview.cpp" // OpenGL + GLFW (for blitting our buffer to the screen)
#include "draw.cpp"
#include "camera.cpp"


int main()
{
    mat4 translation = mat4_translate(1, 2, 3);
    mat4 scale = mat4_scale(3, 3, 3);
    mat4 m2 = mat4_multiply(translation, scale);
    mat4 m3 = mat4_multiply(scale, translation);
    v4 v = V4(1, 1, 1, 1);
    v = mat4_multiply(m2, v);
    
    pixel_buffer_f32 frame_buffer = {};
    frame_buffer.width = 1280;
    frame_buffer.height = 720;
    frame_buffer.bytes_per_pixel = sizeof(v3); // RGB
    frame_buffer.total_size_in_bytes = frame_buffer.width * frame_buffer.height * frame_buffer.bytes_per_pixel;
    frame_buffer.pixels = (f32 *)malloc(frame_buffer.total_size_in_bytes);
    frame_buffer.depth = (f32 *)malloc(frame_buffer.total_size_in_bytes);
    frame_buffer.depth_check_enabled = 1;
    
    attribute_buffer projection_buffer = {};
    projection_buffer.data = (vertex_attributes *)malloc(sizeof(vertex_attributes) * MAX_PROJECTED_VERTEX_BUFFER);
    projection_buffer.max = MAX_PROJECTED_VERTEX_BUFFER;
    
    // look down +z, y up
    v3 camera_origin = {};
    
    projection_data proj = {};
    proj.viewport = V3(1, 1, 1);
    proj.canvas_width = frame_buffer.width;
    proj.canvas_height = frame_buffer.height;
    proj.camera_origin = V3(0, 0, 0);
    
    f32 camera_pan_speed = 3.0f;
    
    preview_context context = setup_preview_window(frame_buffer.width, frame_buffer.height);
    
    model_properties p[] = 
    {
        { V3(0, 0, 5), V3(-1.5f, 0, 7), {}, {} },
        { V3(1, 2, 3), V3(1.5f, 0, 7), {}, {} },
        
    };
    
    model_instance cube_instance;
    cube_instance.attributes = &cube_verts[0];
    cube_instance.indices = &cube_vert_indices[0];
    cube_instance.transformed_vertex_buffer = &projection_buffer;
    cube_instance.vertex_count = array_count(cube_verts);
    cube_instance.index_count = array_count(cube_vert_indices);
    cube_instance.triangle_count = cube_instance.index_count / 3;
    cube_instance.model_count = 0;
    
    push_models_to_instance(&cube_instance, &p[0], array_count(p));
    
    while(context.active)
    {
        clear(frame_buffer, COLOR_BUFFER | DEPTH_BUFFER);
        start_frame(&context);
        
        move_camera(&context, &proj.camera_origin, camera_pan_speed);
        
        render_instance(&frame_buffer, &cube_instance, &proj, WIREFRAME);
        end_frame(&context, frame_buffer.pixels);
    }
    
    return 0;
}