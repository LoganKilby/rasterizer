#include "preview.cpp"
#include "math_lib.h"

struct pixel_buffer_f32
{
    f32 *pixels;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
};

internal void
clear(pixel_buffer_f32 buffer, u32 bytes)
{
    memset(buffer.pixels, 0, bytes);
}

internal void
set_pixel(pixel_buffer_f32 *buffer, f32 x, f32 y, v3 color)
{
    x = (buffer->width / 2) + x;
    y = (buffer->height / 2) + y;
    
    if((x >= 0 && x < buffer->width) && (y >= 0 && y < buffer->height))
    {
        *((v3 *)buffer->pixels + ((u32)y * buffer->width + (u32)x)) = color;
    }
}

#include "draw.cpp"

int main()
{
    pixel_buffer_f32 buffer = {};
    buffer.width = 1280;
    buffer.height = 720;
    buffer.bytes_per_pixel = sizeof(v3); // RGB
    u32 total_buffer_bytes = buffer.width * buffer.height * buffer.bytes_per_pixel;
    buffer.pixels = (f32 *)malloc(total_buffer_bytes);
    
    preview_context context = setup_preview_window(buffer.width, buffer.height);
    
    f32 canvas_left = -(buffer.width * 0.5f);
    f32 canvas_right = buffer.width * 0.5f;
    f32 canvas_top = buffer.height * 0.5f;
    f32 canvas_bottom = -(buffer.height * 0.5f);
    
    vertex_attributes attributes[] = 
    {
        { V2(0, 100), V3(1, 0, 0) },
        { V2(-100, -100), V3(0, 1, 0) },
        { V2(100, -100), V3(0, 0, 1) }
    };
    
    while(context.active)
    {
        clear(buffer, total_buffer_bytes);
        
        // draw
        triangle(&buffer, &attributes[0], FILL);
        
        update_preview(&context, buffer.pixels);
    }
    
    return 0;
}