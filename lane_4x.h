/* date = October 27th 2021 0:44 pm */

#ifndef LANE_4X_H
#define LANE_4X_H

struct lane_f32
{
    __m128 v;
    
    lane_f32 &operator=(f32 A);
};

struct lane_u32
{
    __m128i v;
    lane_u32 &operator=(u32 A);
};

internal lane_u32
operator<(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmplt_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator<=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmple_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator>(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpgt_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator>=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpge_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator==(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpeq_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator!=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm_castps_si128(_mm_cmpneq_ps(a.v, b.v));
    
    return(result);
}

internal lane_u32
operator!=(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    // TODO: What's the most efficient way to invert this comparison?
    result.v = _mm_xor_si128(_mm_cmpeq_epi32(a.v, b.v), _mm_set1_epi32(0xFFFFFFFF));
    
    return(result);
}

internal lane_u32
operator^(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_xor_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator&(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_and_si128(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator&(lane_u32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_and_ps(_mm_castsi128_ps(a.v), b.v);
    
    return(result);
}

internal lane_u32
and_not(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_andnot_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator|(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_or_si128(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator<<(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm_slli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_u32
operator>>(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm_srli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_f32
operator+(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_add_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator+(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm_add_epi32(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator-(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_sub_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator*(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_mul_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator/(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_div_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3, 
                  u32, u32, u32, u32,
                  u32, u32, u32, u32,
                  u32, u32, u32, u32)
{
    lane_u32 result;
    
    result.v = _mm_setr_epi32(r0, r1, r2, r3);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result;
    
    result.v = _mm_cvtepi32_ps(a.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 a)
{
    lane_u32 result;
    
    result.v = _mm_set1_epi32(a);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(u32 a)
{
    lane_f32 result;
    
    result.v = _mm_set1_ps((f32)a);
    
    return(result);
}

internal lane_f32
lane_f32_from_f32(f32 a)
{
    lane_f32 result;
    
    result.v = _mm_set1_ps(a);
    
    return(result);
}

internal lane_f32
square_root(lane_f32 a)
{
    lane_f32 result;
    
    result.v = _mm_sqrt_ps(a.v);
    
    return(result);
}

internal void
conditional_assign(lane_f32 *dest, lane_u32 mask, lane_f32 src)
{
    __m128 mask_ps = _mm_castsi128_ps(mask.v);
    dest->v = _mm_or_ps(_mm_andnot_ps(mask_ps, dest->v), 
                        _mm_and_ps(mask_ps, src.v));
}

inline lane_f32
fmin(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_min_ps(a.v, b.v);
    
    return(result);
}

inline lane_f32
fmax(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm_max_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
gather_f32_(void *base_ptr, u32 stride, lane_u32 indices)
{
    u32 *v = (u32 *)&indices.v;
    lane_f32 result;
    result.v = _mm_setr_ps(*(f32 *)((u8 *)base_ptr + v[0]*stride),
                           *(f32 *)((u8 *)base_ptr + v[1]*stride),
                           *(f32 *)((u8 *)base_ptr + v[2]*stride),
                           *(f32 *)((u8 *)base_ptr + v[3]*stride));
    
    return result;
}

internal b32x
mask_is_zeroed(lane_u32 a)
{
    int mask = _mm_movemask_epi8(a.v);
    
    return mask == 0;
}

internal u64
horizontal_add(lane_u32 a)
{
    u32 *v = (u32 *)&a.v;
    u64 result = (u64)v[0] + (u64)v[1] + (u64)v[2] + (u64)v[3];
    
    return result;
}

internal f32
horizontal_add(lane_f32 a)
{
    f32 *v = (f32 *)&a.v;
    f32 result = v[0] + v[1] + v[2] + v[3];
    
    return result;
}

#endif //LANE_4X_H
