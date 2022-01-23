#include "draw.h"

internal void
clear(pixel_buffer_f32 buffer, u32 options)
{
    u32 bytes = sizeof(buffer.width * buffer.height * buffer.bytes_per_pixel);
    if(options & COLOR_BUFFER)
    {
        memset(buffer.pixels, 0, bytes);
    }
    
    if(options & DEPTH_BUFFER)
    {
        memset(buffer.depth, 0, bytes);
    }
}

internal v2
project_to_canvas(v3 point, v3 viewport, u32 canvas_width, u32 canvas_height)
{
    v2 result;
    
    // Project 3D point to viewplane through perspective divide
    result.x = (point.x * viewport.z) / point.z;
    result.y = (point.y * viewport.z) / point.z;
    
    // Project 3D point on viewplane to 2D point on canvas
    result.x = (result.x * canvas_width) / viewport.x;
    result.y = (result.y * canvas_height) / viewport.y;
    
    return result;
}

internal v2
project_to_canvas(v3 point, v3 viewport, f32 canvas_width, f32 canvas_height)
{
    v2 result;
    
    // Project 3D point to viewplane through perspective divide
    result.x = (point.x * viewport.z) / point.z;
    result.y = (point.y * viewport.z) / point.z;
    
    // Project 3D point on viewplane to 2D point on canvas
    result.x = (result.x * canvas_width) / viewport.x;
    result.y = (result.y * canvas_height) / viewport.y;
    
    return result;
}

internal v2
viewport_to_canvas(v2 point, v3 viewport, u32 canvas_width, u32 canvas_height)
{
    v2 result;
    
    result.x = point.x * canvas_width / viewport.x;
    result.y = point.y * canvas_height / viewport.y;
    
    return result;
}

internal v2
project(v3 point, v3 viewport, u32 canvas_width, u32 canvas_height)
{
    
    v2 point_on_viewport = V2(point.x * viewport.z / point.z, point.y * viewport.z / point.z);
    v2 result = viewport_to_canvas(point_on_viewport, viewport, canvas_width, canvas_height);
    
    return result;
}

internal f32
interpolate_slope(f32 x0, f32 y0, f32 t, f32 x1, f32 slope)
{
    if(x0 == x1)
    {
        return y0;
    }
    
    f32 result = y0 + t * slope;
    
    return result;
}

inline void
swap(v2 *p0, v2 *p1)
{
    v2 temp = *p0;
    *p0 = *p1;
    *p1 = temp;
}

inline void
swap(vertex_attributes *a, vertex_attributes *b)
{
    vertex_attributes temp = *a;
    *a = *b;
    *b = temp;
}

internal void
line(pixel_buffer_f32 *buffer, vertex_attributes a0, vertex_attributes a1)
{
    // TODO: Explore MSAA, FXAA, SSAA, TAA
    
    if(fabs(a1.vertex.x - a0.vertex.x) > fabs(a1.vertex.y - a0.vertex.y))
    {
        // horizontal
        
        if(a0.vertex.x > a1.vertex.x)
        {
            swap(&a0, &a1);
        }
        
        f32 slope = (a1.vertex.y - a0.vertex.y) / (a1.vertex.x - a0.vertex.x);
        v3 color;
        for(f32 x = a0.vertex.x; x <= a1.vertex.x; ++x)
        {
            f32 t = x / a1.vertex.x;
            color = lerp(a0.color, t, a1.color);
            set_pixel(buffer, x, interpolate_slope(a0.vertex.x, a0.vertex.y, x - a0.vertex.x, a1.vertex.x, slope), color);
        }
        
    }
    else
    {
        // vertical
        if(a0.vertex.y > a1.vertex.y)
        {
            swap(&a0, &a1);
        }
        
        f32 slope = (a1.vertex.x - a0.vertex.x) / (a1.vertex.y - a0.vertex.y);
        v3 color;
        for(f32 y = a0.vertex.y; y <= a1.vertex.y; ++y)
        {
            f32 t = y / a1.vertex.y;
            color = lerp(a0.color, t, a1.color);
            f32 x = interpolate_slope(a0.vertex.y, a0.vertex.x, y - a0.vertex.y, a1.vertex.y, slope);
            set_pixel(buffer, x, y, color);
        }
    }
}

internal void
line(pixel_buffer_f32 *buffer, v2 a0, v2 a1, v3 color)
{
    // TODO: Explore MSAA, FXAA, SSAA, TAA
    
    if(fabs(a1.x - a0.x) > fabs(a1.y - a0.y))
    {
        // horizontal
        
        if(a0.x > a1.x)
        {
            swap(&a0, &a1);
        }
        
        f32 slope = (a1.y - a0.y) / (a1.x - a0.x);
        for(f32 x = a0.x; x <= a1.x; ++x)
        {
            f32 y = interpolate_slope(a0.x, a0.y, x - a0.x, a1.x, slope);
            set_pixel(buffer, x, y, color);
        }
        
    }
    else
    {
        // vertical
        if(a0.y > a1.y)
        {
            swap(&a0, &a1);
        }
        
        f32 slope = (a1.x - a0.x) / (a1.y - a0.y);
        for(f32 y = a0.y; y <= a1.y; ++y)
        {
            f32 x = interpolate_slope(a0.y, a0.x, y - a0.y, a1.y, slope);
            set_pixel(buffer, x, y, color);
        }
    }
}

internal void
barycentric_triangle_fill(pixel_buffer_f32 *buffer, vertex_attributes *attributes)
{
    f32 max_x = max(attributes[0].vertex.x, max(attributes[1].vertex.x, attributes[2].vertex.x));
    f32 min_x = min(attributes[0].vertex.x, min(attributes[1].vertex.x, attributes[2].vertex.x));
    f32 max_y = max(attributes[0].vertex.y, max(attributes[1].vertex.y, attributes[2].vertex.y));
    f32 min_y = min(attributes[0].vertex.y, min(attributes[1].vertex.y, attributes[2].vertex.y));
    
    v3 v0v1 = attributes[1].vertex - attributes[0].vertex;
    v3 v0v2 = attributes[2].vertex - attributes[0].vertex;
    
    f32 d00 = inner(v0v1, v0v1);
    f32 d01 = inner(v0v1, v0v2);
    f32 d11 = inner(v0v2, v0v2);
    f32 inv_denom = 1 / (d00 * d11 - d01 * d01);
    
    f32 u, v, w;
    for(f32 x = min_x; x <= max_x; ++x)
    {
        for(f32 y = min_y; y <= max_y; ++y)
        {
            v3 p = V3(x, y, 1);
            
            v3 vp0 = p - attributes[0].vertex;
            f32 d20 = inner(vp0, v0v1);
            f32 d21 = inner(vp0, v0v2);
            
            v = (d11 * d20 - d01 * d21) * inv_denom;
            w = (d00 * d21 - d01 * d20) * inv_denom;
            
            if((v >= 0) && (w >= 0) && (w + v <= 1))
            {
                u = 1 - v - w;
                
                set_pixel(buffer, x, y, attributes[0].color * u +  attributes[1].color * v +  attributes[2].color * w);
            }
        }
    }
}

internal void
triangle(pixel_buffer_f32 *buffer, vertex_attributes *attributes, u32 option)
{
    // TODO: explore draw command buffer
    if(option & WIREFRAME)
    {
        line(buffer, attributes[0], attributes[1]);
        line(buffer, attributes[1], attributes[2]);
        line(buffer, attributes[2], attributes[0]);
    }
    else if(option & FILL)
    {
        // TODO: Explore other triangle filling techniques
        barycentric_triangle_fill(buffer, attributes);
    }
}

internal void
triangle_vertex_sort(v2 *v)
{
    // v[0] is now the lowest, v[2] is the highest
    if(v[1].y < v[0].y)
    {
        swap(&v[1], &v[0]);
    }
    
    if(v[2].y < v[0].y)
    {
        swap(&v[2], &v[0]);
    }
    
    if(v[2].y < v[1].y)
    {
        swap(&v[2], &v[1]);
    }
}

