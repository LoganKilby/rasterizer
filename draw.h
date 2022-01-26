/* date = January 20th 2022 3:25 pm */

#ifndef DRAW_H
#define DRAW_H

#define WHITE {1, 1, 1}
#define BLUE {0, 0, 1}
#define RED {1, 0, 0}
#define GREEN {0, 1, 0}
#define CYAN {0, 1, 1}
#define YELLOW {1, 1, 0}

#define MAX_PROJECTED_VERTEX_BUFFER 10000
#define MAX_INSTANCE_COUNT 1000

#define COLOR_BUFFER 1
#define DEPTH_BUFFER 2

#define WIREFRAME 1
#define FILL 2

struct pixel_buffer_f32
{
    f32 *pixels;
    f32 *depth;
    u32 width;
    u32 height;
    u32 bytes_per_pixel;
    u32 total_size_in_bytes;
    u32 depth_check_enabled;
};

struct vertex_attributes
{
    v3 vertex;
    v3 color;
    // v3 texture_coordinates;
};

struct attribute_buffer
{
    vertex_attributes *data;
    u32 count;
    u32 max;
};

struct projection_data
{
    v3 viewport;
    u32 canvas_width;
    u32 canvas_height;
    camera camera;
};

struct model_properties
{
    v3 origin;
    v3 translation;
    v3 rotation;
    v3 scale;
};

struct model_instance
{
    // contains data about objects with the same vertices, but different properties
    vertex_attributes *attributes; // pointer to object vertices
    u32 *indices; // pointer to the objects triangle indices
    attribute_buffer *transformed_vertex_buffer; // buffer to store the transformed data for rendering
    u32 vertex_count;
    u32 index_count; // NOTE: divide by 3 to get triangle count
    u32 triangle_count;
    
    u32 model_count;
    v3 origin[MAX_INSTANCE_COUNT];
    v3 translation[MAX_INSTANCE_COUNT];
    v3 rotation[MAX_INSTANCE_COUNT];
    v3 scale[MAX_INSTANCE_COUNT];
};

struct triangle_vertices
{
    v3 v0;
    v3 v1;
    v3 v2;
};

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


global_variable vertex_attributes cube_verts[] = 
{
    { V3(1, 1, 1), RED },
    { V3(-1, 1, 1), RED },
    { V3(-1, -1, 1), GREEN },
    { V3(1, -1, 1), GREEN },
    
    { V3(1, 1, -1), YELLOW },
    { V3(-1, 1, -1), YELLOW },
    { V3(-1, -1, -1), CYAN },
    { V3(1, -1, -1), CYAN },
};

global_variable u32 cube_vert_indices[] = 
{
    0, 1, 2,
    0, 2, 3,
    4, 0, 3,
    4, 3, 7,
    5, 4, 7,
    5, 7, 6,
    1, 5, 6,
    1, 6, 2,
    4, 5, 1,
    4, 1, 0,
    2, 6, 7,
    2, 7, 3
};

#endif //DRAW_H
