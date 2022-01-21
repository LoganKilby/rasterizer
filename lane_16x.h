/* date = October 28th 2021 1:32 pm */

#ifndef LANE_16X_H
#define LANE_16X_H

// TODO: Check if the blend operation is really the most sensible thing to do for converting __mmask16 to _mm512

struct lane_f32
{
    __m512 v;
    
    lane_f32 &operator=(f32 A);
};

struct lane_u32
{
    __m512i v;
    lane_u32 &operator=(u32 A);
};

internal lane_u32
operator<(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    __mmask16 comp_mask = _mm512_cmplt_ps_mask(a.v, b.v);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    return(result);
}

internal lane_u32
operator<=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmple_ps_mask(a.v, b.v);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator>(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmp_ps_mask(a.v, b.v, _CMP_GT_OQ);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator>=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmp_ps_mask(a.v, b.v, _CMP_GE_OQ);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator==(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmp_ps_mask(a.v, b.v, _CMP_EQ_OQ);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator!=(lane_f32 a, lane_f32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmp_ps_mask(a.v, b.v, _CMP_NEQ_OQ);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator!=(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    __mmask16 comp_mask = _mm512_cmpneq_epi32_mask(a.v, b.v);
    result.v = _mm512_mask_blend_epi32(comp_mask, _mm512_setzero_si512(), _mm512_set1_epi32(1));
    
    return(result);
}

internal lane_u32
operator^(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm512_xor_si512(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator&(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm512_and_epi32(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator&(lane_u32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_and_ps(_mm512_castsi512_ps(a.v), b.v);
    
    return(result);
}

internal lane_u32
and_not(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm512_andnot_si512(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator|(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm512_or_si512(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator<<(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm512_slli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_u32
operator>>(lane_u32 a, u32 Shift)
{
    lane_u32 result;
    
    result.v = _mm512_srli_epi32(a.v, Shift);
    
    return(result);
}

internal lane_f32
operator+(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_add_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
operator+(lane_u32 a, lane_u32 b)
{
    lane_u32 result;
    
    result.v = _mm512_add_epi32(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator-(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_sub_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator*(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_mul_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
operator/(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_div_ps(a.v, b.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3, 
                  u32 r4, u32 r5, u32 r6, u32 r7,
                  u32 r8, u32 r9, u32 r10, u32 r11,
                  u32 r12, u32 r13, u32 r14, u32 r15)
{
    lane_u32 result;
    
    result.v = _mm512_set_epi32(r0, r1, r2, r3,
                                r4, r5, r6, r7,
                                r8, r9, r10, r11,
                                r12, r13, r14, r15);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result;
    
    result.v = _mm512_cvtepi32_ps(a.v);
    
    return(result);
}

internal lane_u32
lane_u32_from_u32(u32 a)
{
    lane_u32 result;
    
    result.v = _mm512_set1_epi32(a);
    
    return(result);
}

internal lane_f32
lane_f32_from_u32(u32 a)
{
    lane_f32 result;
    
    result.v = _mm512_set1_ps((f32)a);
    
    return(result);
}

internal lane_f32
lane_f32_from_f32(f32 a)
{
    lane_f32 result;
    
    result.v = _mm512_set1_ps(a);
    
    return(result);
}

internal lane_f32
square_root(lane_f32 a)
{
    lane_f32 result;
    
    result.v = _mm512_sqrt_ps(a.v);
    
    return(result);
}

internal void
conditional_assign(lane_f32 *dest, lane_u32 mask, lane_f32 src)
{
    // TODO: I think there's instructions that would condense lane_u32 mask to a __mmask16
    __m512 mask_ps = _mm512_castsi512_ps(mask.v);
    dest->v = _mm512_or_ps(_mm512_andnot_ps(mask_ps, dest->v), 
                           _mm512_and_ps(mask_ps, src.v));
}

inline lane_f32
fmin(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_min_ps(a.v, b.v);
    
    return(result);
}

inline lane_f32
fmax(lane_f32 a, lane_f32 b)
{
    lane_f32 result;
    
    result.v = _mm512_max_ps(a.v, b.v);
    
    return(result);
}

internal lane_f32
gather_f32_(void *base_ptr, u32 stride, lane_u32 indices)
{
    u32 *v = (u32 *)&indices.v;
    lane_f32 result;
    result.v = _mm512_setr_ps(*(f32 *)((u8 *)base_ptr + v[0]*stride),
                              *(f32 *)((u8 *)base_ptr + v[1]*stride),
                              *(f32 *)((u8 *)base_ptr + v[2]*stride),
                              *(f32 *)((u8 *)base_ptr + v[3]*stride),
                              *(f32 *)((u8 *)base_ptr + v[4]*stride),
                              *(f32 *)((u8 *)base_ptr + v[5]*stride),
                              *(f32 *)((u8 *)base_ptr + v[6]*stride),
                              *(f32 *)((u8 *)base_ptr + v[7]*stride),
                              *(f32 *)((u8 *)base_ptr + v[8]*stride),
                              *(f32 *)((u8 *)base_ptr + v[9]*stride),
                              *(f32 *)((u8 *)base_ptr + v[10]*stride),
                              *(f32 *)((u8 *)base_ptr + v[11]*stride),
                              *(f32 *)((u8 *)base_ptr + v[12]*stride),
                              *(f32 *)((u8 *)base_ptr + v[13]*stride),
                              *(f32 *)((u8 *)base_ptr + v[14]*stride),
                              *(f32 *)((u8 *)base_ptr + v[15]*stride));
    
    return result;
}

internal b32x
mask_is_zeroed(lane_u32 a)
{
    __mmask16 lane_mask = _mm512_movepi32_mask(a.v);
    int mask = _mm512_mask2int(lane_mask);
    
    return mask == 0;
}

internal u64
horizontal_add(lane_u32 a)
{
    u32 *v = (u32 *)&a.v;
    u64 result = (u64)v[0] + (u64)v[1] + (u64)v[2] + (u64)v[3] +
        (u64)v[4] + (u64)v[5] + (u64)v[6] + (u64)v[7] +
        (u64)v[8] + (u64)v[9] + (u64)v[10] + (u64)v[11] + 
        (u64)v[12] + (u64)v[13] + (u64)v[14] + (u64)v[15];
    
    return result;
}

internal f32
horizontal_add(lane_f32 a)
{
    f32 *v = (f32 *)&a.v;
    f32 result = v[0] + v[1] + v[2] + v[3] + 
        v[4] + v[5] + v[6] + v[7] + 
        v[0] + v[1] + v[2] + v[3] + 
        v[4] + v[5] + v[6] + v[7] +
        v[8] + v[9] + v[10] + v[11] +
        v[12] + v[13] + v[14] + v[15];
    
    return result;
}

#endif //LANE_16X_H
