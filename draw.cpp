#include "draw.h"

internal f32
interpolate_slope(f32 x0, f32 y0, f32 t, f32 x1, f32 y1, f32 slope)
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

internal void
line(pixel_buffer_f32 *buffer, v2 p0, v2 p1, v3 color)
{
    // TODO: Explore MSAA, FXAA, SSAA, TAA
    
    if(fabs(p1.x - p0.x) > fabs(p1.y - p0.y))
    {
        // horizontal
        
        if(p0.x > p1.x)
        {
            swap(&p0, &p1);
        }
        
        f32 slope = (p1.y - p0.y) / (p1.x - p0.x);
        for(f32 x = p0.x; x <= p1.x; ++x)
        {
            set_pixel(buffer, x, interpolate_slope(p0.x, p0.y, x - p0.x, p1.x, p1.y, slope), color);
        }
        
    }
    else
    {
        // vertical
        if(p0.y > p1.y)
        {
            swap(&p0, &p1);
        }
        
        f32 slope = (p1.x - p0.x) / (p1.y - p0.y);
        for(f32 y = p0.y; y <= p1.y; ++y)
        {
            f32 x = interpolate_slope(p0.y, p0.x, y - p0.y, p1.y, p1.x, slope);
            set_pixel(buffer, x, y, color);
        }
    }
}

internal void
barycentric_triangle_fill(pixel_buffer_f32 *buffer, vertex_attributes *attributes)
{
    f32 max_x = max(attributes[0].vertices.x, max(attributes[1].vertices.x, attributes[2].vertices.x));
    f32 min_x = min(attributes[0].vertices.x, min(attributes[1].vertices.x, attributes[2].vertices.x));
    f32 max_y = max(attributes[0].vertices.y, max(attributes[1].vertices.y, attributes[2].vertices.y));
    f32 min_y = min(attributes[0].vertices.y, min(attributes[1].vertices.y, attributes[2].vertices.y));
    
    v2 v0v1 = attributes[1].vertices - attributes[0].vertices;
    v2 v0v2 = attributes[2].vertices - attributes[0].vertices;
    
    f32 d00 = inner(v0v1, v0v1);
    f32 d01 = inner(v0v1, v0v2);
    f32 d11 = inner(v0v2, v0v2);
    f32 inv_denom = 1 / (d00 * d11 - d01 * d01);
    
    f32 u, v, w;
    for(f32 x = min_x; x <= max_x; ++x)
    {
        for(f32 y = min_y; y <= max_y; ++y)
        {
            v2 p = V2(x, y);
            
            v2 vp0 = p - attributes[0].vertices;
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
triangle(pixel_buffer_f32 *buffer, vertex_attributes *attributes, raster_option option)
{
    // TODO: explore draw command buffer
    if(option == WIREFRAME)
    {
        line(buffer, attributes[0].vertices, attributes[1].vertices, attributes[0].color);
        line(buffer, attributes[1].vertices, attributes[2].vertices, attributes[0].color);
        line(buffer, attributes[2].vertices, attributes[0].vertices, attributes[0].color);
    }
    else if(option == FILL)
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

