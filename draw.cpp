#include "draw.h"

v2i pixel_coordinates(f32 x, f32 y, u32 canvas_width, u32 canvas_height)
{
    // x : [-cw / 2, cw / 2]
    // y : [-ch / 2, ch / 2]
    // origin at [0, 0]
    v2i result;
    result.x = (s32)((canvas_width * 0.5f) + x);
    result.y = (s32)((canvas_height * 0.5f) + y);
    
    return result;
}

internal void
set_pixel(pixel_buffer_f32 *buffer, f32 x, f32 y, v3 color)
{
    v2i p = pixel_coordinates(x, y, buffer->width, buffer->height);
    u32 offset = p.y * buffer->width + p.x;
    
    if((p.x >= 0 && p.x < (s32)buffer->width) && (p.y >= 0 && p.y < (s32)buffer->height))
    {
        if(buffer->depth_check_enabled && buffer->depth[offset] == 0)
        {
            *((v3 *)buffer->pixels + offset) = color;
            
            // TODO: Compute actual depth
            buffer->depth[offset] = 1.0f;
        }
    }
}

inline void
project(vertex_attributes *attribs, projection_data *proj, u32 count)
{
    f32 inv_vp_x = 1.0f / proj->viewport.x;
    f32 inv_vp_y = 1.0f / proj->viewport.y;
    
    for(u32 point_index = 0; point_index < count; ++point_index)
    {
        attribs[point_index].vertex.x = (attribs[point_index].vertex.x * proj->viewport.z) / attribs[point_index].vertex.z;
        attribs[point_index].vertex.y = (attribs[point_index].vertex.y * proj->viewport.z) / attribs[point_index].vertex.z;
        
        // Convert from 3D world coordinates to canvas coordinates
        attribs[point_index].vertex.x = (attribs[point_index].vertex.x * proj->canvas_width) * inv_vp_x;
        attribs[point_index].vertex.y = (attribs[point_index].vertex.y * proj->canvas_height) * inv_vp_y;
    }
}

inline void
project_triangle(triangle_vertices *t, projection_data *proj)
{
    f32 vp_z = proj->viewport.z;
    
    f32 inv_vp_x = (1 / proj->viewport.x) * proj->canvas_width;
    f32 inv_vp_y = (1 / proj->viewport.y) * proj->canvas_height;
    
    f32 v0_inv_z = (1 / t->v0.z) * vp_z;
    f32 v1_inv_z = (1 / t->v1.z) * vp_z;
    f32 v2_inv_z = (1 / t->v2.z) * vp_z;
    
    t->v0.x = t->v0.x * v0_inv_z * inv_vp_x;
    t->v0.y = t->v0.y * v0_inv_z * inv_vp_y;
    
    t->v1.x = t->v1.x * v1_inv_z * inv_vp_x;
    t->v1.y = t->v1.y * v1_inv_z * inv_vp_y;
    
    t->v2.x = t->v2.x * v2_inv_z * inv_vp_x;
    t->v2.y = t->v2.y * v2_inv_z * inv_vp_y;
}

internal void
project_vertices(attribute_buffer *buffer, u32 offset, u32 count, projection_data *proj)
{
    // TODO: Does copying all these attrbutes to a seperate buffer make sense?
    for(vertex_attributes *a = buffer->data + offset; 
        a < buffer->data + offset + count; 
        ++a)
    {
        // TODO: SIMD?
        
        // Project 3D point to viewplane through
        a->vertex.x = (a->vertex.x * proj->viewport.z) / a->vertex.z;
        a->vertex.y = (a->vertex.y * proj->viewport.z) / a->vertex.z;
        
        // Convert from 3D world coordinates to canvas coordinates
        a->vertex.x = (a->vertex.x * proj->canvas_width) / proj->viewport.x;
        a->vertex.y = (a->vertex.y * proj->canvas_height) / proj->viewport.y;
    }
}

inline void
rotate_triangle(triangle_vertices *t, v3 rotation)
{
    triangle_vertices tri_result = *t;
    triangle_vertices temp = tri_result;
    
    if(!zero_vector(rotation))
    {
        if(rotation.z)
        {
            f32 radians = rotation.z * RADIANS_PER_DEGREE;
            
            v3 rot_x = V3(cosf(radians), sinf(radians), 0);
            v3 rot_y = V3(-sinf(radians), cosf(radians), 0);
            
            tri_result.v0 = V3(inner(temp.v0, rot_x), inner(temp.v0, rot_y), temp.v0.z);
            tri_result.v1 = V3(inner(temp.v1, rot_x), inner(temp.v1, rot_y), temp.v1.z);
            tri_result.v2 = V3(inner(temp.v2, rot_x), inner(temp.v2, rot_y), temp.v2.z);
            
            temp = tri_result;
        }
        
        if(rotation.y)
        {
            f32 radians = rotation.y * RADIANS_PER_DEGREE;
            
            v3 rot_x = V3(cosf(radians), 0, -sinf(radians));
            v3 rot_z = V3(sinf(radians), 0, cosf(radians));
            
            tri_result.v0 = V3(inner(temp.v0, rot_x), temp.v0.y, inner(temp.v0, rot_z));
            tri_result.v1 = V3(inner(temp.v1, rot_x), temp.v1.y, inner(temp.v1, rot_z));
            tri_result.v2 = V3(inner(temp.v2, rot_x), temp.v2.y, inner(temp.v2, rot_z));
            
            temp = tri_result;
        }
        
        if(rotation.x)
        {
            f32 radians = rotation.x * RADIANS_PER_DEGREE;
            
            v3 rot_y = V3(0, cosf(radians), sinf(radians));
            v3 rot_z = V3(0, -sinf(radians), cosf(radians));
            
            tri_result.v0 = V3(temp.v0.x, inner(temp.v0, rot_y), inner(temp.v0, rot_z));
            tri_result.v1 = V3(temp.v1.x, inner(temp.v1, rot_y), inner(temp.v1, rot_z));
            tri_result.v2 = V3(temp.v2.x, inner(temp.v2, rot_y), inner(temp.v2, rot_z));
        }
    }
    
    *t = tri_result;
}

internal void
clear(pixel_buffer_f32 buffer, u32 options)
{
    if(options & COLOR_BUFFER)
    {
        memset(buffer.pixels, 0, buffer.frame_buffer_size_in_bytes);
    }
    
    if(options & DEPTH_BUFFER)
    {
        memset(buffer.depth, 0, buffer.depth_buffer_size_in_bytes);
    }
}

internal v2
project_to_canvas(v3 point, v3 viewport, u32 canvas_width, u32 canvas_height)
{
    v2 result;
    
    // Project 3D point onto viewplane 
    result.x = (point.x * viewport.z) / point.z;
    result.y = (point.y * viewport.z) / point.z;
    
    // Convert 3D point on viewplane surface to 2D point on canvas
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
swap(v3 *p0, v3 *p1)
{
    v3 temp = *p0;
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
    f32 canvas_left = -(s32)buffer->width * 0.5f;
    f32 canvas_right = buffer->width * 0.5f;
    f32 canvas_top = -(s32)buffer->height * 0.5f;
    f32 canvas_bottom = (s32)buffer->height * 0.5f;
    
    a0.vertex.x = clamp(buffer->left, a0.vertex.x, buffer->right);
    a0.vertex.y = clamp(buffer->top, a0.vertex.y, buffer->bottom);
    a1.vertex.x = clamp(buffer->left, a1.vertex.x, buffer->right);
    a1.vertex.y = clamp(buffer->top, a1.vertex.y, buffer->bottom);
    
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
            f32 y = interpolate_slope(a0.vertex.x, a0.vertex.y, x - a0.vertex.x, a1.vertex.x, slope);
            color = lerp(a0.color, t, a1.color);
            set_pixel(buffer, x, y, color);
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
line(pixel_buffer_f32 *buffer, v3 a0, v3 a1, v3 color)
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
line(pixel_buffer_f32 *buffer, v3 a0, v3 a1, v3 color0, v3 color1)
{
    // TODO: Explore MSAA, FXAA, SSAA, TAA
    if(fabs(a1.x - a0.x) > fabs(a1.y - a0.y))
    {
        // horizontal
        
        if(a0.x > a1.x)
        {
            swap(&a0, &a1);
            swap(&color0, &color1);
        }
        
        f32 slope = (a1.y - a0.y) / (a1.x - a0.x);
        for(f32 x = a0.x; x <= a1.x; ++x)
        {
            f32 t = x / a1.x;
            f32 y = interpolate_slope(a0.x, a0.y, x - a0.x, a1.x, slope);
            set_pixel(buffer, x, y, lerp(color0, t, color1));
        }
        
    }
    else
    {
        // vertical
        if(a0.y > a1.y)
        {
            swap(&a0, &a1);
            swap(&color0, &color1);
        }
        
        f32 slope = (a1.x - a0.x) / (a1.y - a0.y);
        for(f32 y = a0.y; y <= a1.y; ++y)
        {
            f32 t = y / a1.y;
            f32 x = interpolate_slope(a0.y, a0.x, y - a0.y, a1.y, slope);
            set_pixel(buffer, x, y, lerp(color0, t, color1));
        }
    }
}

internal void
barycentric_triangle_fill(pixel_buffer_f32 *buffer, vertex_attributes *attributes)
{
#if 0
    attributes[0].vertex.x = clamp(buffer->left, attributes[0].vertex.x, buffer->right);
    attributes[0].vertex.y = clamp(buffer->top, attributes[0].vertex.y, buffer->bottom);
    attributes[1].vertex.x = clamp(buffer->left, attributes[1].vertex.x, buffer->right);
    attributes[1].vertex.y = clamp(buffer->top, attributes[1].vertex.y, buffer->bottom);
    attributes[2].vertex.x = clamp(buffer->left, attributes[2].vertex.x, buffer->right);
    attributes[2].vertex.y = clamp(buffer->top, attributes[2].vertex.y, buffer->bottom);
#endif
    
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
barycentric_triangle_fill(pixel_buffer_f32 *buffer, triangle_vertices *tri, v3 c0, v3 c1, v3 c2)
{
#if 0
    tri->v0.x = clamp(buffer->left, tri->v0.x, buffer->right);
    tri->v0.y = clamp(buffer->top, tri->v0.y, buffer->bottom);
    tri->v1.x = clamp(buffer->left, tri->v1.x, buffer->right);
    tri->v1.y = clamp(buffer->top, tri->v1.y, buffer->bottom);
    tri->v2.x = clamp(buffer->left, tri->v2.x, buffer->right);
    tri->v2.y = clamp(buffer->top, tri->v2.y, buffer->bottom);
#endif
    
    f32 max_x = max(tri->v0.x, max(tri->v1.x, tri->v2.x));
    f32 min_x = min(tri->v0.x, min(tri->v1.x, tri->v2.x));
    f32 max_y = max(tri->v0.y, max(tri->v1.y, tri->v2.y));
    f32 min_y = min(tri->v0.y, min(tri->v1.y, tri->v2.y));
    
    v3 v0v1 = tri->v1 - tri->v0;
    v3 v0v2 = tri->v2 - tri->v0;
    
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
            
            v3 vp0 = p - tri->v0;
            f32 d20 = inner(vp0, v0v1);
            f32 d21 = inner(vp0, v0v2);
            
            v = (d11 * d20 - d01 * d21) * inv_denom;
            w = (d00 * d21 - d01 * d20) * inv_denom;
            
            if((v >= 0) && (w >= 0) && (w + v <= 1))
            {
                u = 1 - v - w;
                
                set_pixel(buffer, x, y, c0 * u +  c1 * v +  c2 * w);
            }
        }
    }
}

internal void
barycentric_triangle_fill(pixel_buffer_f32 *buffer, vertex_attributes *a, vertex_attributes *b, vertex_attributes *c)
{
    f32 max_x = max(a->vertex.x, max(b->vertex.x, c->vertex.x));
    f32 min_x = min(a->vertex.x, min(b->vertex.x, c->vertex.x));
    f32 max_y = max(a->vertex.y, max(b->vertex.y, c->vertex.y));
    f32 min_y = min(a->vertex.y, min(b->vertex.y, c->vertex.y));
    
    v3 v0v1 = b->vertex - a->vertex;
    v3 v0v2 = c->vertex - a->vertex;
    
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
            
            v3 vp0 = p - a->vertex;
            f32 d20 = inner(vp0, v0v1);
            f32 d21 = inner(vp0, v0v2);
            
            v = (d11 * d20 - d01 * d21) * inv_denom;
            w = (d00 * d21 - d01 * d20) * inv_denom;
            
            if((v >= 0) && (w >= 0) && (w + v <= 1))
            {
                u = 1 - v - w;
                
                set_pixel(buffer, x, y, a->color * u +  b->color * v +  c->color * w);
            }
        }
    }
}

internal void
triangle_fill(pixel_buffer_f32 *buffer, triangle_vertices *t, v3 color)
{
    f32 max_x = max(t->v0.x, max(t->v1.x, t->v2.x));
    f32 min_x = min(t->v0.x, min(t->v1.x, t->v2.x));
    f32 max_y = max(t->v0.y, max(t->v1.y, t->v2.y));
    f32 min_y = min(t->v0.y, min(t->v1.y, t->v2.y));
    
    for(f32 x = min_x; x <= max_x; ++x)
    {
        for(f32 y = min_y; y <= max_y; ++y)
        {
            set_pixel(buffer, x, y, color);
        }
    }
}

internal void
triangle_fill(pixel_buffer_f32 *buffer, v3 a, v3 b, v3 c, v3 color)
{
    f32 max_x = max(a.x, max(b.x, c.x));
    f32 min_x = min(a.x, min(b.x, c.x));
    f32 max_y = max(a.y, max(b.y, c.y));
    f32 min_y = min(a.y, min(b.y, c.y));
    
    for(f32 x = min_x; x <= max_x; ++x)
    {
        for(f32 y = min_y; y <= max_y; ++y)
        {
            set_pixel(buffer, x, y, color);
        }
    }
}

internal void
triangle(pixel_buffer_f32 *buffer, vertex_attributes *attributes, u32 option)
{
    // TODO: explore draw command buffer
    if(option & FILL)
    {
        // TODO: Explore other triangle filling techniques
        barycentric_triangle_fill(buffer, attributes);
    }
    else if(option & WIREFRAME)
    {
        line(buffer, attributes[0], attributes[1]);
        line(buffer, attributes[1], attributes[2]);
        line(buffer, attributes[2], attributes[0]);
    }
}

internal void
triangle(pixel_buffer_f32 *buffer, vertex_attributes *a, vertex_attributes *b, vertex_attributes *c, u32 option)
{
    // TODO: explore draw command buffer
    if(option & FILL)
    {
        // TODO: Explore other triangle filling techniques
        barycentric_triangle_fill(buffer, a, b, c);
    }
    else if(option & WIREFRAME)
    {
        line(buffer, *a, *b);
        line(buffer, *b, *c);
        line(buffer, *c, *a);
    }
    
}

internal void
triangle(pixel_buffer_f32 *buffer, triangle_vertices *tri, v3 c0, v3 c1, v3 c2, u32 option)
{
    // TODO: explore draw command buffer
    if(option & FILL)
    {
        // TODO: Explore other triangle filling techniques
        barycentric_triangle_fill(buffer, tri, c0, c1, c2);
    }
    else if(option & WIREFRAME)
    {
        line(buffer, tri->v0, tri->v1, c0, c1);
        line(buffer, tri->v1, tri->v2, c1, c2);
        line(buffer, tri->v2, tri->v0, c2, c0);
    }
    
}

internal void
triangle(pixel_buffer_f32 *buffer, v3 a, v3 b, v3 c, v3 color, u32 option)
{
    if(option & FILL)
    {
        triangle_fill(buffer, a, b, c, color);
    }
    else if(option & WIREFRAME)
    {
        line(buffer, a, b, color);
        line(buffer, b, c, color);
        line(buffer, c, a, color);
    }
}

internal void
triangle(pixel_buffer_f32 *buffer, triangle_vertices *t, v3 color, u32 option)
{
    if(option & FILL)
    {
        triangle_fill(buffer, t, color);
    }
    else if(option & WIREFRAME)
    {
        line(buffer, t->v0, t->v1, color);
        line(buffer, t->v1, t->v2, color);
        line(buffer, t->v2, t->v0, color);
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

internal void
push_attribute(attribute_buffer *buffer, vertex_attributes *new_vertex)
{
    Assert(buffer->count < buffer->max)
    {
        buffer->data[buffer->count++] = *new_vertex;
    }
}

internal void
translate_vertices(attribute_buffer *buffer, u32 offset, u32 count, v3 translation)
{
    for(vertex_attributes *a = buffer->data + offset; 
        a < buffer->data + offset + count; 
        ++a)
    {
        a->vertex += translation;
    }
}

internal u32
copy_attributes(attribute_buffer *dest, vertex_attributes *attrib, u32 count)
{
    u32 offset = dest->count;
    
    // TODO: Does copying all these attrbutes to a seperate buffer make sense?
    for(vertex_attributes *a = attrib; 
        a < attrib + count; 
        ++a)
    {
        push_attribute(dest, a);
    }
    
    return offset;
}

internal void
render_triangle_buffer(pixel_buffer_f32 *frame_buffer, attribute_buffer *vertices, u32 attribute_offset, u32 *indices, u32 triangle_count, u32 options)
{
    struct triangle_indices
    {
        // NOTE: indices of the three vertices that make a triangle, assumed to be in this
        // format 
        u32 a, b, c;
    };
    
    triangle_indices *triangles = (triangle_indices *)indices;
    vertex_attributes *attribs = &vertices->data[attribute_offset];
    
    for(triangle_indices *t = triangles; t < triangles + triangle_count; ++t)
    {
        triangle(frame_buffer, &attribs[t->a], &attribs[t->b], &attribs[t->c], options);
    }
}

internal void
push_models_to_instance(model_instance *instance, model_properties *models, u32 models_to_add)
{
    u32 model_count = instance->model_count;
    u32 new_model_count = model_count + models_to_add;
    Assert(new_model_count < MAX_INSTANCE_COUNT)
    {
        for(u32 model_index = 0; model_index < new_model_count; ++model_index)
        {
            instance->origin[model_count] = models[model_index].origin;
            instance->translation[model_count] = models[model_index].translation;
            instance->rotation[model_count] = models[model_index].rotation;
            instance->scale[model_count] = models[model_index].scale;
            
            model_count++;
        }
        
        instance->model_count = new_model_count;
    }
}

internal v4
min_bounding_sphere(triangle_vertices *t)
{
    // TODO: Implement something more robust
    
    v3 center = (1 / 3.0f) * (t->v0 + t->v1 + t->v2);
    
    f32 v0_length = length(t->v0 - center);
    f32 v1_length = length(t->v1 - center);
    f32 v2_length = length(t->v2 - center);
    
    f32 radius = max(v0_length, max(v1_length, v2_length));
    
    v4 result = V4(center.x, center.y, center.z, radius);
    
    return result;
}

internal v4
min_bounding_sphere(vertex_attributes *a, u32 count)
{
    // TODO: Implement something more robust
    v3 vertex_sum = {};
    for(u32 vi = 0; vi < count; ++vi)
    {
        vertex_sum += a[vi].vertex;
    }
    
    v3 center = (1.0f / count) * (vertex_sum);
    
    f32 radius = 0; // max vertex length
    f32 len;
    for(u32 vi = 0; vi < count; ++vi)
    {
        len = length(a[vi].vertex - center);
        
        if(len > radius) 
            radius = len;
    }
    
    v4 result = V4(center.x, center.y, center.z, radius);
    
    return result;
}

internal u32
clip_sphere(v4 sphere, clipping_planes *cp)
{
    u32 result = 0;
    
    // Should I copy the sphere instead of having to cast to v3?
    v3 sphere_center = V3(sphere.x, sphere.y, sphere.z);
    
    // Sphere behind plane
    result |= (inner(sphere_center, cp->near) < -sphere.w) << 0;
    result |= (inner(sphere_center, cp->left) < -sphere.w) << 1;
    result |= (inner(sphere_center, cp->right) < -sphere.w) << 2;
    result |= (inner(sphere_center, cp->bottom) < -sphere.w) << 3;
    result |= (inner(sphere_center, cp->top) < -sphere.w) << 4;
    
    return result;
}

internal void
render_instance(pixel_buffer_f32 *frame_buffer, model_instance *instance, projection_data *proj, u32 render_options)
{
    struct indices
    {
        // NOTE: indices of the three vertices that make a triangle, assumed to be in this
        // format 
        u32 a, b, c;
    };
    
    Assert(instance->triangle_count);
    Assert(instance->index_count);
    Assert(instance->model_count);
    
    indices *triangles = (indices *)instance->indices;
    u32 triangle_count = instance->triangle_count;
    
    vertex_attributes *attribs = instance->attributes;
    vertex_attributes triangle_attributes[3];
    
    triangle_vertices tri;
    
    v3 near_plane_normal = proj->clip.near;
    
    for(u32 model_index = 0; model_index < instance->model_count; ++model_index)
    {
        v3 total_translation = instance->origin[model_index] + instance->translation[model_index] - proj->camera.origin;
        
        v4 model_bounding_sphere = instance->bounding_sphere;
        *(v3 *)&model_bounding_sphere += total_translation;
        
        f32 sd = inner(*(v3 *)&model_bounding_sphere, near_plane_normal);
        u32 sphere_behind_near = sd < -model_bounding_sphere.w;
        
        u32 model_clip_mask = clip_sphere(model_bounding_sphere, &proj->clip);
        if(!model_clip_mask)
        {
            for(indices *i = triangles; i < triangles + triangle_count; ++i)
            {
                triangle_attributes[0] = instance->attributes[i->a];
                triangle_attributes[1] = instance->attributes[i->b];
                triangle_attributes[2] = instance->attributes[i->c];
                
                tri.v0 = instance->attributes[i->a].vertex;
                tri.v1 = instance->attributes[i->b].vertex;
                tri.v2 = instance->attributes[i->c].vertex;
                
                rotate_triangle(&tri, instance->rotation[model_index]);
                tri.v0 += total_translation;
                tri.v1 += total_translation;
                tri.v2 += total_translation;
                rotate_triangle(&tri, proj->camera.rotation);
                
                // TODO: Rotate the frustum by the camera's rotation angle to clip properly
                
                v4 bounding_sphere = min_bounding_sphere(&tri);
                f32 signed_dist_to_plane = inner(*(v3 *)&bounding_sphere, near_plane_normal);
                // if the vertex is not totally behind the plane
                if(!(signed_dist_to_plane < -bounding_sphere.w))
                {
                    v3 c0 = triangle_attributes[0].color;
                    v3 c1 = triangle_attributes[1].color;
                    v3 c2 = triangle_attributes[2].color;
                    
                    // sphere intersecting plane
                    if(f32abs(signed_dist_to_plane) < bounding_sphere.w)
                    {
                        f32 sd_v0 = inner(tri.v0, near_plane_normal);
                        f32 sd_v1 = inner(tri.v1, near_plane_normal);
                        f32 sd_v2 = inner(tri.v2, near_plane_normal);
                        
                        u32 v0_visible = sd_v0 > 0;
                        u32 v1_visible = sd_v1 > 0;
                        u32 v2_visible = sd_v2 > 0;
                        u32 visible_count = v0_visible + v1_visible + v2_visible;
                        
#if 1
                        // NOTE: We would need to check the other planes as well.
                        // I think that's why we want to do a bounding sphere test first, otherwise we will
                        // have to test every vertex on every plane of the view volume
                        if(visible_count == 1)
                        {
                            // one in front
                            if(!v0_visible)
                            {
                                f32 numerator = -sd_v0 - sd_v0;
                                v3 v0v1 = tri.v1 - tri.v0;
                                v3 v0v2 = tri.v2 - tri.v0;
                                f32 t_v0v1 = (numerator) / inner(near_plane_normal, v0v1);
                                f32 t_v0v2 = (numerator) / inner(near_plane_normal, v0v2);
                                tri.v1 = tri.v0 + t_v0v1 * v0v1;
                                tri.v2 = tri.v0 + t_v0v2 * v0v2;
                                c1 = lerp(c1, t_v0v1, c0);
                                c2 = lerp(c2, t_v0v2, c0);
                            }
                            else if(!v1_visible)
                            {
                                f32 numerator = -sd_v1 - sd_v1;
                                v3 v1v0 = tri.v0 - tri.v1;
                                v3 v1v2 = tri.v2 - tri.v1;
                                f32 t_v1v0 = (numerator) / inner(near_plane_normal, v1v0);
                                f32 t_v1v2 = (numerator) / inner(near_plane_normal, v1v2);
                                tri.v0 = tri.v1 + t_v1v0 * v1v0;
                                tri.v2 = tri.v1 + t_v1v2 * v1v2;
                                c0 = lerp(c0, t_v1v0, c1);
                                c2 = lerp(c2, t_v1v2, c1);
                            }
                            else //if(!v2_visible)
                            {
                                f32 numerator = -sd_v2 - sd_v2;
                                v3 v2v1 = tri.v1 - tri.v2;
                                v3 v2v0 = tri.v0 - tri.v2;
                                f32 t_v2v0 = (numerator) / inner(near_plane_normal, v2v0);
                                f32 t_v2v1 = (numerator) / inner(near_plane_normal, v2v1);
                                tri.v0 = tri.v2 + t_v2v0 * v2v0;
                                tri.v1 = tri.v2 + t_v2v1 * v2v1;
                                c0 = lerp(c0, t_v2v0, c2);
                                c1 = lerp(c1, t_v2v1, c2);
                            }
                        }
                        else if(visible_count == 2)
                        {
                            // two in front
                            
                            triangle_vertices new_tri;
                            if(!v2_visible)
                            {
                                // v0_visible && v1_visible
                                
                                // alter: v0, v1, v2v1
                                f32 numerator = -sd_v2 - sd_v2;
                                v3 v2v1 = tri.v1 - tri.v2;
                                v3 v2v0 = tri.v0 - tri.v2;
                                f32 t_v2v0 = (numerator) / inner(near_plane_normal, v2v0);
                                f32 t_v2v1 = (numerator) / inner(near_plane_normal, v2v1);
                                v3 new_v2 = tri.v1 + t_v2v1 * v2v1;
                                tri.v2 = new_v2;
                                c2 = lerp(c2, t_v2v1, c1);
                                
                                // create: v0, v2v1, v2v0
                                new_tri.v0 = tri.v0;
                                new_tri.v1 = new_v2;
                                new_tri.v2 = tri.v0 + t_v2v0 * v2v0;
                                v3 new_c2 = lerp(c2, t_v2v0, c0);
                                
                                project_triangle(&new_tri, proj);
                                triangle(frame_buffer, &new_tri, c0, c2, new_c2, render_options);
                            }
                            else if(!v0_visible)
                            {
                                // v1_visible && v2_visible
                                
                                // alter: v0v2, v1, v2
                                f32 numerator = -sd_v0 - sd_v0;
                                v3 v0v1 = tri.v1 - tri.v0;
                                v3 v0v2 = tri.v2 - tri.v0;
                                f32 t_v0v1 = (numerator) / inner(near_plane_normal, v0v1);
                                f32 t_v0v2 = (numerator) / inner(near_plane_normal, v0v2);
                                v3 new_v0 = tri.v0 + t_v0v2 * v0v2;
                                tri.v0 = new_v0;
                                c0 = lerp(c0, t_v0v2, c2);
                                
                                // create: v0v2, v0v1, v1
                                v3 new_v1 = tri.v0 + t_v0v1 * v0v1;
                                v3 new_c1 = lerp(c1, t_v0v1, c0);
                                new_tri.v0 = new_v0;
                                new_tri.v1 = new_v1;
                                new_tri.v2 = tri.v1;
                                
                                project_triangle(&new_tri, proj);
                                triangle(frame_buffer, &new_tri, c0, new_c1, c2, render_options);
                            }
                            else //if(!v1_visible)
                            {
                                // v0_visible && v2_visible
                                
                                // alter: v0, v1v0, v2
                                v3 v1v0 = tri.v0 - tri.v1;
                                v3 v1v2 = tri.v2 - tri.v1;
                                f32 numerator = -sd_v1 - sd_v1;
                                f32 t_v1v0 = (numerator) / inner(near_plane_normal, v1v0);
                                f32 t_v1v2 = (numerator) / inner(near_plane_normal, v1v2);
                                v3 new_v1 = tri.v1 + t_v1v0 * (v1v0);
                                tri.v1 = new_v1;
                                c1 = lerp(c1, t_v1v0, c0);
                                
                                // create: v1v0, v1v2, v2
                                new_tri.v0 = new_v1;
                                new_tri.v1 = tri.v1 + t_v1v2 * (v1v2);
                                new_tri.v2 = tri.v2;
                                v3 c1_new = lerp(c1, t_v1v2, c2);
                                
                                project_triangle(&new_tri, proj);
                                triangle(frame_buffer, &new_tri, c1, c1_new, c2, render_options);
                            }
                        }
#endif
                    }
                    
                    project_triangle(&tri, proj);
                    triangle(frame_buffer, &tri, c0, c1, c2, render_options);
                }
                else
                {
                    // triangle sphere behind plane
                }
            }
        }
        else
        {
            // model sphere behind plane
            printf("clipped model sphere\n");
        }
    }
}

internal void 
draw_string(pixel_buffer_f32 *frame_buffer, float x, float y, float scale, char *text, v3 color)
{
    static char buffer[99999]; // ~500 chars
    int num_quads;
    
    struct quad_attributes
    {
        v3 vertex;
        uint8_t color[4];
    };
    
    struct quad
    {
        quad_attributes a0;
        quad_attributes a1;
        quad_attributes a2;
        quad_attributes a3;
    };
    
    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));
    quad *quads = (quad *)buffer;
    
    vertex_attributes va0;
    vertex_attributes va1;
    vertex_attributes va2;
    vertex_attributes va3;
    
    f32 screen_origin_x = -(frame_buffer->width / 2.0f);
    f32 screen_origin_y = (frame_buffer->height / 2.0f);
    
    quad q;
    for(int quad_index = 0; quad_index < num_quads; ++quad_index)
    {
        q = quads[quad_index];
        
        va0 = { q.a0.vertex * scale, color };
        va1 = { q.a1.vertex * scale, color };
        va2 = { q.a2.vertex * scale, color };
        va3 = { q.a3.vertex * scale, color };
        
        va0.vertex.y = -va0.vertex.y + screen_origin_y;
        va1.vertex.y = -va1.vertex.y + screen_origin_y;
        va2.vertex.y = -va2.vertex.y + screen_origin_y;
        va3.vertex.y = -va3.vertex.y + screen_origin_y;
        
        va0.vertex.x = va0.vertex.x + screen_origin_x;
        va1.vertex.x = va1.vertex.x + screen_origin_x;
        va2.vertex.x = va2.vertex.x + screen_origin_x;
        va3.vertex.x = va3.vertex.x + screen_origin_x;
        
        triangle(frame_buffer, &va0, &va1, &va2, FILL);
        triangle(frame_buffer, &va0, &va2, &va3, FILL);
    }
}

internal void
draw_fps_timeout(pixel_buffer_f32 *frame_buffer, f32 step, f32 *timeout, float x, float y, float scale, v3 color)
{
    local_persist char fps_buf[10] = {};
    
    if(*timeout <= 0)
    {
        memset(fps_buf, 0, sizeof(fps_buf));
        
        f32 this_frame_fps = 1 / step;
        sprintf(fps_buf, "%.3f", this_frame_fps);
        *timeout = 0.5f;
    }
    
    draw_string(frame_buffer, x, y, scale, fps_buf, color);
    *timeout -= step;
}
