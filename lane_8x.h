/* date = October 27th 2021 0:44 pm */

#ifndef LANE_8X_H
#define LANE_8X_H

struct lane_f32
{
    __m256 v;
    
    lane_f32 &operator=(f32 A);
};

struct lane_u32
{
    __m256i v;
    lane_u32 &operator=(u32 A);
};

internal lane_u32
operator<(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_LT_OQ));
    
    return(result);
}

internal lane_u32
operator<=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_LE_OQ));
    
    return(result);
}

internal lane_u32
operator>(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_GT_OQ));
    
    return(result);
}

internal lane_u32
operator>=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_GE_OQ));
    
    return(result);
}

internal lane_u32
operator==(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_EQ_OQ));
    
    return(result);
}

internal lane_u32
operator!=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    result.v = _mm256_castps_si256(_mm256_cmp_ps(a.v, b.v, _CMP_NEQ_OQ));
    
    return(result);
}

internal lane_u32
operator!=(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    // TODO: What's the most efficient way to invert this comparison?
    result.v = _mm256_xor_si256(_mm256_cmpeq_epi32(a.v, b.v), _mm256_set1_epi32(0xFFFFFFFF));
    
    return(result);
}

internal lane_u32
operator^(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm256_xor_si256(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator&(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm256_and_si256(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator&(lane_u32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_and_ps(_mm256_castsi256_ps(a.v), b.v);
    
    return(result);
}

internal lane_u32
and_not(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm256_andnot_si256(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator|(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm256_or_si256(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator<<(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm256_slli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_u32
operator>>(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm256_srli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_f32
operator+(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_add_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator+(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm256_add_epi32(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator-(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_sub_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator*(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_mul_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator/(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_div_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3, 
                  u32 r4, u32 r5, u32 r6, u32 r7,
                  u32, u32, u32, u32,
                  u32, u32, u32, u32)
{
    lane_u32 result;
    
    result.v = _mm256_setr_epi32(r0, r1, r2, r3, r4, r5, r6, r7);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result;
    
    result.v = _mm256_cvtepi32_ps(a.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 a)
{
    lane_u32 result;
    
    result.v = _mm256_set1_epi32(a);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(u32 a)
{
    lane_f32 result;
    
    result.v = _mm256_set1_ps((f32)a);
    
    return(result);
}

internal lane_f32
lane_f32_from_f32(f32 a)
{
    lane_f32 result;
    
    result.v = _mm256_set1_ps(a);
    
    return(result);
}

internal lane_f32
square_root(lane_f32 a)
{
    lane_f32 result;
    
    result.v = _mm256_sqrt_ps(a.v);
    
    return(result);
}

internal void
conditional_assign(lane_f32 *dest, lane_u32 mask, lane_f32 src)
{
    __m256 mask_ps = _mm256_castsi256_ps(mask.v);
    dest->v = _mm256_or_ps(_mm256_andnot_ps(mask_ps, dest->v), 
                           _mm256_and_ps(mask_ps, src.v));
}

inline lane_f32
fmin(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_min_ps(a.v, b.v);
    
    return(result);
}

inline lane_f32
fmax(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm256_max_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
gather_f32_(void *base_ptr, u32 stride, lane_u32 indices)
{
    u32 *v = (u32 *)&indices.v;
    lane_f32 result;
    result.v = _mm256_setr_ps(*(f32 *)((u8 *)base_ptr + v[0]*stride),
                              *(f32 *)((u8 *)base_ptr + v[1]*stride),
                              *(f32 *)((u8 *)base_ptr + v[2]*stride),
                              *(f32 *)((u8 *)base_ptr + v[3]*stride),
                              *(f32 *)((u8 *)base_ptr + v[4]*stride),
                              *(f32 *)((u8 *)base_ptr + v[5]*stride),
                              *(f32 *)((u8 *)base_ptr + v[6]*stride),
                              *(f32 *)((u8 *)base_ptr + v[7]*stride));
    
    return result;
}

internal b32x
mask_is_zeroed(lane_u32 a)
{
    int mask = _mm256_movemask_epi8(a.v);
    
    return mask == 0;
}

internal u64
horizontal_add(lane_u32 a)
{
    u32 *v = (u32 *)&a.v;
    u64 result = (u64)v[0] + (u64)v[1] + (u64)v[2] + (u64)v[3] + (u64)v[4] + (u64)v[5] + (u64)v[6] + (u64)v[7];
    
    return result;
}

internal f32
horizontal_add(lane_f32 a)
{
    f32 *v = (f32 *)&a.v;
    f32 result = v[0] + v[1] + v[2] + v[3] + v[4] + v[5] + v[6] + v[7];
    
    return result;
}

#endif //LANE_8X_H
