/* date = January 20th 2022 3:25 pm */

#ifndef DRAW_H
#define DRAW_H

enum raster_option
{
    WIREFRAME,
    FILL
};

struct vertex_attributes
{
    v2 vertex;
    v3 color;
};

#endif //DRAW_H
