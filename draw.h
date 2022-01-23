/* date = January 20th 2022 3:25 pm */

#ifndef DRAW_H
#define DRAW_H

#define WHITE {1, 1, 1}
#define BLUE {0, 0, 1}
#define RED {1, 0, 0}
#define GREEN {0, 1, 0}

#define COLOR_BUFFER 1
#define DEPTH_BUFFER 2

#define WIREFRAME 1
#define FILL 2

struct vertex_attributes
{
    v3 vertex;
    v3 color;
};

#endif //DRAW_H
