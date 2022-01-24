#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#include "preview.cpp" // OpenGL + GLFW (for blitting our buffer to the screen)
#include "math_lib.h"

#include "draw.cpp"

int main()
{
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
    
    preview_context context = setup_preview_window(frame_buffer.width, frame_buffer.height);
    
    model_properties p[] = 
    {
        { V3(1, 0, 5.0f), {}, {} },
        { V3(1, 5, 13.0f), {}, {} },
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
        
        //u32 offset = copy_attributes(&projection_buffer, &cube_verts[0], array_count(cube_verts));
        //translate_vertices(&projection_buffer, offset, array_count(cube_verts), p.translation);
        //project_vertices(&projection_buffer, offset, array_count(cube_verts), &proj);
        
        //render_triangle_buffer(&frame_buffer, &projection_buffer, offset, &cube_vert_indices[0], 12, WIREFRAME);
        
        //projection_buffer.count = 0;
        
        render_instance(&frame_buffer, &cube_instance, &proj, WIREFRAME);
        
        update_preview(&context, frame_buffer.pixels);
    }
    
    return 0;
}