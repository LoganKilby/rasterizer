#include "preview.cpp"
#include "math_lib.h"

struct pixel_buffer_f32
{
    f32 *pixels;
    f32 *depth;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
    u32 total_size_in_bytes;
};

v2i pixel_coordinates(f32 x, f32 y, u32 canvas_width, u32 canvas_height)
{
    // x : [-cw / 2, cw / 2]
    // y : [-ch / 2, ch / 2]
    // origin at [0, 0]
    v2i result;
    result.x = (s32)((canvas_width * 0.5f) + x);
    result.y = (s32)((canvas_height * 0.5f) - y);
    
    return result;
}

internal void
set_pixel(pixel_buffer_f32 *buffer, f32 x, f32 y, v3 color)
{
    v2i p = pixel_coordinates(x, y, buffer->width, buffer->height);
    u32 offset = p.y * buffer->width + p.x;
    
    if((p.x >= 0 && p.x < (s32)buffer->width) && (p.y >= 0 && p.y < (s32)buffer->height))
    {
        // TODO: Where should the depth check take place?
        bool depth_hit = *(buffer->depth + offset);
        
        if(!depth_hit)
        {
            *((v3 *)buffer->pixels + (p.y * buffer->width + p.x)) = color;
            *(buffer->depth + offset) = 1;
        }
    }
}

#include "draw.cpp"

int main()
{
    pixel_buffer_f32 buffer = {};
    buffer.width = 1280;
    buffer.height = 720;
    buffer.bytes_per_pixel = sizeof(v3); // RGB
    buffer.total_size_in_bytes = buffer.width * buffer.height * buffer.bytes_per_pixel;
    buffer.pixels = (f32 *)malloc(buffer.total_size_in_bytes);
    buffer.depth = (f32 *)malloc(buffer.total_size_in_bytes);
    
    
    // look down +z, y up
    v3 camera_origin = {};
    
    v3 viewport = V3(1, 1, 0.1f);
    v3 point = {5, 5, 5};
    v3 white = V3(1, 1, 1);
    
    v2i test = pixel_coordinates(0, 0, buffer.width, buffer.height);
    
    v3 test_point = V3(-1, 1, 1);
    test_point *= viewport.z;
    test_point /= test_point.z;
    
    v2 vp_to_canvas = V2(buffer.width / viewport.x, buffer.height / viewport.y);
    v2i p = pixel_coordinates(test_point.x * vp_to_canvas.x, test_point.y * vp_to_canvas.y, buffer.width, buffer.height);
    
    preview_context context = setup_preview_window(buffer.width, buffer.height);
    
    f32 canvas_left = -(buffer.width * 0.5f);
    f32 canvas_right = buffer.width * 0.5f;
    f32 canvas_top = buffer.height * 0.5f;
    f32 canvas_bottom = -(buffer.height * 0.5f);
    
    vertex_attributes triangle_verts[] = 
    {
        { V3(0, 100, 1), V3(1, 0, 0) },
        { V3(-100, -100, 1), V3(0, 1, 0) },
        { V3(100, -100, 1), V3(0, 0, 1) }
    };
    
    vertex_attributes line_verts[] = 
    {
        { V3(0, 0, 1), V3(1, 0, 0)},
        { V3(100, 0, 1), V3(0, 0, 1)}
    };
    
    vertex_attributes cube_verts[] = 
    {
        { V3(-1, 1, 1), V3(1, 1, 1) },
        { V3(1, 1, 1), V3(1, 1, 1) },
        { V3(1, -1, 1), V3(1, 1, 1) },
        { V3(-1, -1, 1), V3(1, 1, 1) },
        
        { V3(-1, 1, 2), V3(1, 1, 1) },
        { V3(1, 1, 2), V3(1, 1, 1) },
        { V3(1, -1, 2), V3(1, 1, 1) },
        { V3(-1, -1, 2), V3(1, 1, 1) }
    };
    
    while(context.active)
    {
        clear(buffer, COLOR_BUFFER | DEPTH_BUFFER);
        
        // draw
        //triangle(&buffer, &triangle_verts[0], FILL);
        
        line(&buffer, 
             project_to_canvas(cube_verts[0].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[1].vertex, viewport, buffer.width, buffer.height),
             BLUE);
        line(&buffer, 
             project_to_canvas(cube_verts[1].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[2].vertex, viewport, buffer.width, buffer.height),
             BLUE);
        line(&buffer, 
             project_to_canvas(cube_verts[2].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[3].vertex, viewport, buffer.width, buffer.height),
             BLUE);
        line(&buffer, 
             project_to_canvas(cube_verts[3].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[0].vertex, viewport, buffer.width, buffer.height),
             BLUE);
        
        line(&buffer, 
             project_to_canvas(cube_verts[4].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[5].vertex, viewport, buffer.width, buffer.height),
             RED);
        line(&buffer, 
             project_to_canvas(cube_verts[5].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[6].vertex, viewport, buffer.width, buffer.height),
             RED);
        line(&buffer, 
             project_to_canvas(cube_verts[6].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[7].vertex, viewport, buffer.width, buffer.height),
             RED);
        line(&buffer, 
             project_to_canvas(cube_verts[7].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[4].vertex, viewport, buffer.width, buffer.height),
             RED);
        
        line(&buffer, 
             project_to_canvas(cube_verts[0].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[4].vertex, viewport, buffer.width, buffer.height),
             GREEN);
        line(&buffer, 
             project_to_canvas(cube_verts[1].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[5].vertex, viewport, buffer.width, buffer.height),
             GREEN);
        line(&buffer, 
             project_to_canvas(cube_verts[2].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[6].vertex, viewport, buffer.width, buffer.height),
             GREEN);
        line(&buffer, 
             project_to_canvas(cube_verts[3].vertex, viewport, buffer.width, buffer.height),
             project_to_canvas(cube_verts[7].vertex, viewport, buffer.width, buffer.height),
             GREEN);
        
        update_preview(&context, buffer.pixels);
    }
    
    return 0;
}