/* date = January 20th 2022 3:25 pm */

#ifndef DRAW_H
#define DRAW_H

#define WHITE {1, 1, 1}
#define BLUE {0, 0, 1}
#define RED {1, 0, 0}
#define GREEN {0, 1, 0}
#define CYAN {0, 1, 1}
#define YELLOW {1, 1, 0}

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

struct clipping_planes
{
    v3 near; // (0, 0, 1) - d = 0
    v3 left; // (1, 0, 1), legnth == sqrt(2), so normalized -> (1/sqrt(2), 0, 1 / sqrt(2));
    v3 right; // (-1 / sqrt(2), 0, 1 / sqrt(2))
    v3 bottom; // (0, 1 / sqrt(2), 1 / sqrt(2))
    v3 top; // (0, -1 / sqrt(2), 1 / sqrt(2)
};

struct projection_data
{
    v3 viewport;
    clipping_planes clip;
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
