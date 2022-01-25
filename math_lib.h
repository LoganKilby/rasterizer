/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef float f32;
typedef double f64;
typedef int32_t s32;
typedef int64_t s64;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned char u8;

typedef int b32;
typedef b32 b32x;

#define U32_MAX ((u32)-1)

#define _USE_MATH_DEFINES
#include "math.h"
#include <stdlib.h>
#include <float.h>

#define PI (float)M_PI
#define PI32 3.14159265359f
#define TWO_PI 2*PI32
#define RADIANS_PER_DEGREE 0.0174533f
#define EPSILON 10e-6

#define float_equal(x, y) (fabs(x - y) < EPSILON)
#define float_zero(x) (fabs(x) < EPSILON)
#define zero_vector(v) (!v.x && !v.y && !v.z)

struct v2i
{
    s32 x;
    s32 y;
};

//
// v3
// 

union v3
{
    struct
    {
        f32 x, y, z;
    };
    
    struct
    {
        f32 r, g, b;
    };
};

union v4
{
    struct
    {
        f32 x, y, z, w;
    };
    
    struct
    {
        f32 r, g, b, a;
    };
};

internal v3
V3(f32 a, f32 b, f32 c)
{
    v3 result = {a, b, c};
    
    return result;
}

internal v4
V4(f32 a, f32 b, f32 c, f32 d)
{
    v4 result = { a, b, c, d };
    
    return result;
}

inline v4
operator*(f32 a, v4 b)
{
    v4 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;
    result.w = a*b.w;
    
    return result;
}

inline v4
operator*(v4 b, f32 a)
{
    v4 result = a*b;
    
    return result;
}

inline v4 &
operator*=(v4 &b, f32 a)
{
    b = a * b;
    
    return b;
}

inline v4
operator/(v4 b, f32 a)
{
    v4 result = (1.0f/a)*b;
    
    return result;
}

inline v4 &
operator/=(v4 &b, f32 a)
{
    b = b / a;
    
    return b;
}

inline v4
operator-(v4 a)
{
    v4 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    
    return result;
}

inline v4
operator+(v4 a, v4 b)
{
    v4 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    
    return result;
}

inline v4 &
operator+=(v4 &a, v4 b)
{
    a = a + b;
    
    return(a);
}

inline v4
operator-(v4 a, v4 b)
{
    v4 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
    
    return result;
}

inline v4 &
operator-=(v4 &a, v4 b)
{
    a = a - b;
    
    return a;
}

//
// v3
// 

inline v3
operator*(f32 a, v3 b)
{
    v3 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;
    
    return result;
}

inline v3
operator*(v3 b, f32 a)
{
    v3 result = a*b;
    
    return result;
}

inline v3 &
operator*=(v3 &b, f32 a)
{
    b= a * b;
    
    return b;
}

inline v3
operator/(v3 b, f32 a)
{
    v3 result = (1.0f/a)*b;
    
    return result;
}

inline v3 &
operator/=(v3 &b, f32 a)
{
    b= b / a;
    
    return b;
}

inline v3
operator-(v3 a)
{
    v3 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    
    return result;
}

inline v3
operator+(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline v3 &
operator+=(v3 &a, v3 b)
{
    a = a + b;
    
    return(a);
}

inline v3
operator-(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline v3 &
operator-=(v3 &a, v3 b)
{
    a = a - b;
    
    return a;
}

//
// v2
//

struct v2
{
    f32 x, y;
};

internal v2
V2(f32 a, f32 b)
{
    v2 result = { a, b};
    
    return result;
}

internal v3
V3(v2 a, f32 b)
{
    v3 result = {a.x, a.y, b};
    
    return result;
}

inline v2
operator*(f32 a, v2 b)
{
    v2 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    
    return result;
}

inline v2
operator*(v2 b, f32 a)
{
    v2 result = a*b;
    
    return result;
}

inline v2 &
operator*=(v2 &b, f32 a)
{
    b = a * b;
    
    return b;
}

inline v2
operator/(v2 b, f32 a)
{
    v2 result = (1.0f/a)*b;
    
    return result;
}

inline v2 &
operator/=(v2 &b, f32 a)
{
    b = b / a;
    
    return b;
}

inline v2
operator-(v2 a)
{
    v2 result;
    
    result.x = -a.x;
    result.y = -a.y;
    
    return result;
}

inline v2
operator+(v2 a, v2 b)
{
    v2 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline v2 &
operator+=(v2 &a, v2 b)
{
    a = a + b;
    
    return(a);
}

inline v2
operator-(v2 a, v2 b)
{
    v2 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline v2 &
operator-=(v2 &a, v2 b)
{
    a = a - b;
    
    return a;
}

inline v3
hadamard(v3 a, v3 b)
{
    v3 result = {a.x*b.x, a.y*b.y, a.z*b.z};
    
    return result;
}

inline f32
inner(v4 a, v4 b)
{
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z + a.w * b.w;
    return result;
}

inline f32
inner(v3 a, v3 b)
{
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}

inline f32
inner(v2 a, v2 b)
{
    f32 result = a.x*b.x + a.y*b.y;
    return result;
}

inline v3
cross(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    
    return result;
}

inline f32
cross(v2 a, v2 b)
{
    f32 result = a.x * b.y - a.y * b.x;
    
    return result;
}

inline f32
length_squared(v3 a)
{
    f32 result = inner(a, a);
    
    return result;
}

inline f32
square(f32 a)
{
    f32 result = a*a;
    
    return result;
}

inline f32 
square_root(f32 a)
{
    f32 result = (f32)sqrt(a);
    return(result);
}

inline f32
length(v3 a)
{
    f32 result = square_root(length_squared(a));
    return result;
}

inline v3
noz(v3 a)
{
    v3 result = {};
    
    f32 len_sq = length_squared(a);
    if(len_sq > square(0.0001f))
    {
        result = a * (1.0f / square_root(len_sq));
    }
    
    return result;
}

inline v3
normalize(v3 a)
{
    v3 result = a * (1.0f / length(a));
    
    return result;
}

inline v3
lerp(v3 a, f32 t, v3 b)
{
    v3 result = (1.0f - t)*a + t*b;
    
    return result;
}

inline v2
lerp(v2 a, f32 t, v2 b)
{
    v2 result = (1.0f - t)*a + t*b;
    
    return result;
}

inline f32
lerp(f32 a, f32 t, f32 b)
{
    f32 result = (1.0f - t)*a + t*b;
    
    return result;
}

inline f32
fpow(f32 A, f32 B)
{
    f32 result = (f32)pow(A, B);
    return(result);
}

inline u32
round_f32_to_u32(f32 F)
{
    // TODO(casey): Replace with SSE
    u32 result = (u32)(F + 0.5f);
    return(result);
}

inline s32
clamp(s32 min, s32 value, s32 max)
{
    s32 result = value;
    
    if(result < min)
    {
        result = min;
    }
    else if(result > max)
    {
        result = max;
    }
    
    return result;
}

inline f32
clamp(f32 min, f32 value, f32 max)
{
    f32 result = value;
    
    if(result < min)
    {
        result = min;
    }
    else if(result > max)
    {
        result = max;
    }
    
    return result;
}

inline f32
clamp01(f32 value)
{
    f32 result = clamp(0.0f, value, 1.0f);
    
    return result;
}

inline v3
clamp01(v3 v)
{
    v3 result;
    
    result.x = clamp01(v.x);
    result.y = clamp01(v.y);
    result.z = clamp01(v.z);
    
    return result;
}

inline f32
clamp01_map_to_range(f32 min, f32 t, f32 max)
{
    f32 result = 0.0f;
    
    f32 range = max - min;
    if(range != 0.0f)
    {
        result = clamp01((t - min) / range);
    }
    
    return result;
}

inline f32
clamp_above_zero(f32 value)
{
    f32 result = (value < 0) ? 0.0f : value;
    return result;
}

inline f32
max(f32 a, f32 b)
{
    f32 result = (a > b) ? a : b;
    
    return result;
}

inline f32
min(f32 a, f32 b)
{
    f32 result = (a < b) ? a : b;
    
    return result;
}

union mat4
{
    // column-major
    
    struct
    {
        f32 m[4][4];
    };
    
    struct
    {
        v4 c0;
        v4 c1;
        v4 c2;
        v4 c3;
    };
};

union mat3
{
    // column-major
    
    struct
    {
        f32 m[3][3];
    };
    
    struct
    {
        v3 c0;
        v3 c1;
        v3 c2;
    };
};

inline mat3
mat3_identity()
{
    mat3 result = {};
    result.c0.x = 1;
    result.c1.y = 1;
    result.c2.z = 1;
    
    return result;
}

inline v3
mat3_multiply(mat3& m, v3& v)
{
    v3 result;
    
    result.x = inner(v, m.c0);
    result.y = inner(v, m.c1);
    result.z = inner(v, m.c2);
    
    return result;
}

inline mat4
mat4_identity()
{
    mat4 result = {};
    
    result.c0.x = 1;
    result.c1.y = 1;
    result.c2.z = 1;
    result.c3.w = 1;
    
    return result;
}

inline mat4
mat4_scale(f32 a, f32 b, f32 c)
{
    mat4 result = {};
    
    result.c0.x = a;
    result.c1.y = b;
    result.c2.z = c;
    result.c3.w = 1;
    
    return result;
}

inline mat4
mat4_translate(v3 translation)
{
    mat4 result = mat4_identity();
    result.c0.w = translation.x;
    result.c1.w = translation.y;
    result.c2.w = translation.z;
    
    return result;
}
inline mat4
mat4_translate(f32 x, f32 y, f32 z)
{
    mat4 result = mat4_identity();
    result.c0.w = x;
    result.c1.w = y;
    result.c2.w = z;
    
    return result;
}

inline mat4
mat4_transpose(mat4& a)
{
    mat4 result;
    
    result.c0 = V4(a.m[0][0], a.m[1][0], a.m[2][0], a.m[3][0]);
    result.c1 = V4(a.m[0][1], a.m[1][1], a.m[2][1], a.m[3][1]);
    result.c2 = V4(a.m[0][2], a.m[1][2], a.m[2][2], a.m[3][2]);
    result.c3 = V4(a.m[0][3], a.m[1][3], a.m[2][3], a.m[3][3]);
    
    return result;
}

inline v4 
mat4_multiply(mat4& m, v4& v)
{
    v4 result;
    
    result.x = inner(v, m.c0);
    result.y = inner(v, m.c1);
    result.z = inner(v, m.c2);
    result.w = inner(v, m.c3);
    
    return result;
}

inline mat4
mat4_multiply(mat4& a, mat4& b)
{
    // TODO: SIMD this? Get rid of transpose
    mat4 c = mat4_transpose(b);
    
    mat4 result;
    
    result.c0 = V4(inner(a.c0, c.c0), inner(a.c0, c.c1), inner(a.c0, c.c2), inner(a.c0, c.c3));
    result.c1 = V4(inner(a.c1, c.c0), inner(a.c1, c.c1), inner(a.c1, c.c2), inner(a.c1, c.c3));
    result.c2 = V4(inner(a.c2, c.c0), inner(a.c2, c.c1), inner(a.c2, c.c2), inner(a.c2, c.c3));
    result.c3 = V4(inner(a.c3, c.c0), inner(a.c3, c.c1), inner(a.c3, c.c2), inner(a.c3, c.c3));
    
    return result;
}

inline void
rotate(v3 *origin, v3 rotation)
{
    v3 result = *origin;
    v3 temp = *origin;
    
    if(!zero_vector(rotation))
    {
        if(rotation.z)
        {
            f32 radians = rotation.z * RADIANS_PER_DEGREE;
            result.x = inner(temp, V3(cosf(radians), sinf(radians), 0));
            result.y = inner(temp, V3(-sinf(radians), cosf(radians), 0));
            result.z = temp.z;
            temp = result;
        }
        
        if(rotation.y)
        {
            f32 radians = rotation.y * RADIANS_PER_DEGREE;
            result.x = inner(temp, V3(cosf(radians), 0, -sinf(radians)));
            result.y = temp.y;
            result.z = inner(temp, V3(sinf(radians), 0, cosf(radians)));
            temp = result;
        }
        
        if(rotation.x)
        {
            f32 radians = rotation.x * RADIANS_PER_DEGREE;
            result.x = temp.x;
            result.y = inner(temp, V3(0, cosf(radians), sinf(radians)));
            result.z = inner(temp, V3(0, -sinf(radians), cosf(radians)));
        }
    }
    
    *origin = result;
};

#endif //MATH_LIB_H
