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
        // TODO: Where should the depth check take place?
#if 0
        if(buffer->depth_check_enabled)
        {
            // TODO: Compute depth
            
            f32 depth_at_pixel = *(buffer->depth + offset);
            
            if(!depth_at_pixel)
            {
                *(buffer->depth + offset) = 1;
                *((v3 *)buffer->pixels + (p.y * buffer->width + p.x)) = color;
            }
        }
        else
        {
            
        }
#endif
        
        *((v3 *)buffer->pixels + (p.y * buffer->width + p.x)) = color;
    }
}

inline void
translate(v3 *point, v3 *translation)
{
    *point += *translation;
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

internal void
project_vertices(attribute_buffer *buffer, u32 offset, u32 count, projection_data *proj)
{
    f32 inv_viewport_d = 1.0f / proj->viewport.z;
    
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

internal void
clear(pixel_buffer_f32 buffer, u32 options)
{
    if(options & COLOR_BUFFER)
    {
        memset(buffer.pixels, 0, buffer.total_size_in_bytes);
    }
    
    if(options & DEPTH_BUFFER)
    {
        memset(buffer.depth, 0, buffer.total_size_in_bytes);
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

internal void
render_instance(pixel_buffer_f32 *frame_buffer, model_instance *instance, projection_data *proj, u32 render_options)
{
    struct triangle_indices
    {
        // NOTE: indices of the three vertices that make a triangle, assumed to be in this
        // format 
        u32 a, b, c;
    };
    
    Assert(instance->triangle_count);
    Assert(instance->index_count);
    Assert(instance->model_count);
    
    triangle_indices *triangles = (triangle_indices *)instance->indices;
    u32 triangle_count = instance->triangle_count;
    
    vertex_attributes *attribs = instance->attributes;
    vertex_attributes triangle_attributes[3];
    
    v3 origin;
    v3 translation;
    v3 rotation;
    v3 camera_origin = proj->camera_origin;
    triangle_vertices tri;
    for(u32 model_index = 0; model_index < instance->model_count; ++model_index)
    {
        origin = instance->origin[model_index];
        translation = instance->translation[model_index];
        rotation = instance->rotation[model_index];
        
        for(triangle_indices *t = triangles; t < triangles + triangle_count; ++t)
        {
            tri.v0 = instance->attributes[t->a].vertex;
            tri.v1 = instance->attributes[t->b].vertex;
            tri.v2 = instance->attributes[t->c].vertex;
            
            triangle_attributes[0] = instance->attributes[t->a];
            triangle_attributes[1] = instance->attributes[t->b];
            triangle_attributes[2] = instance->attributes[t->c];
            
            rotate(&triangle_attributes[0].vertex, rotation);
            rotate(&triangle_attributes[1].vertex, rotation);
            rotate(&triangle_attributes[2].vertex, rotation);
            
            triangle_attributes[0].vertex += origin;
            triangle_attributes[1].vertex += origin;
            triangle_attributes[2].vertex += origin;
            
            triangle_attributes[0].vertex += translation;
            triangle_attributes[1].vertex += translation;
            triangle_attributes[2].vertex += translation;
            
            triangle_attributes[0].vertex -= camera_origin;
            triangle_attributes[1].vertex -= camera_origin;
            triangle_attributes[2].vertex -= camera_origin;
            
            project(&triangle_attributes[0], proj, 3);
            
            triangle(frame_buffer, &triangle_attributes[0], render_options);
        }
    }
}