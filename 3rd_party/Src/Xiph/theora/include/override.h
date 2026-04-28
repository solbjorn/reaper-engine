/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggTheora SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE Theora SOURCE CODE IS COPYRIGHT (C) 2002-2009                *
 * by the Xiph.Org Foundation and contributors                      *
 * https://www.xiph.org/                                            *
 *                                                                  *
 ********************************************************************

  function:

 ********************************************************************/

#define __GNUC_PREREQ(x, y) 1

#if !defined(_x86_vc_x86int_H)
# define _x86_vc_x86int_H (1)
# include "../lib/internal.h"

# if defined(OC_X86_ASM)
#  define oc_state_accel_init oc_state_accel_init_x86
#  if defined(OC_X86_64_ASM)
/*x86-64 guarantees SIMD support up through at least SSE2.
  If the best routine we have available only needs SSE2 (which at the moment
   covers all of them), then we can avoid runtime detection and the indirect
   call.*/
#   define oc_frag_copy(_state,_dst,_src,_ystride) \
  oc_frag_copy_mmx(_dst,_src,_ystride)
#   define oc_frag_copy_list(_state,_dst_frame,_src_frame,_ystride, \
 _fragis,_nfragis,_frag_buf_offs) \
  oc_frag_copy_list_mmx(_dst_frame,_src_frame,_ystride, \
   _fragis,_nfragis,_frag_buf_offs)
#   define oc_frag_recon_intra(_state,_dst,_ystride,_residue) \
  oc_frag_recon_intra_mmx(_dst,_ystride,_residue)
#   define oc_frag_recon_inter(_state,_dst,_src,_ystride,_residue) \
  oc_frag_recon_inter_mmx(_dst,_src,_ystride,_residue)
#   define oc_frag_recon_inter2(_state,_dst,_src1,_src2,_ystride,_residue) \
  oc_frag_recon_inter2_mmx(_dst,_src1,_src2,_ystride,_residue)
#   define oc_idct8x8(_state,_y,_x,_last_zzi) \
  oc_idct8x8_sse2(_y,_x,_last_zzi)
#   define oc_state_frag_recon oc_state_frag_recon_mmx
#   define oc_loop_filter_init(_state,_bv,_flimit) \
  oc_loop_filter_init_mmxext(_bv,_flimit)
#   define oc_state_loop_filter_frag_rows oc_state_loop_filter_frag_rows_mmxext
#   define oc_restore_fpu(_state) \
  oc_restore_fpu_mmx()
#  else
#   define OC_STATE_USE_VTABLE (1)
#  endif
# endif

# include "../lib/state.h"
# include "../lib/x86/x86cpu.h"

/*Converts the expression in the argument to a string.*/
#define OC_M2STR(_s) #_s

/*Memory operands do not always include an offset.
  To avoid warnings, we force an offset with %H (which adds 8).*/
# if __GNUC_PREREQ(4,0)
#  define OC_MEM_OFFS(_offs,_name) \
  OC_M2STR(_offs-8+%H[_name])
# endif
/*If your gcc version doesn't support %H, then you get to suffer the warnings.
  Note that Apple's gas breaks on things like _offs+(%esp): it throws away the
   whole offset, instead of substituting in 0 for the missing operand to +.*/
# if !defined(OC_MEM_OFFS)
#  define OC_MEM_OFFS(_offs,_name) \
  OC_M2STR(_offs+%[_name])
# endif

/*Declare an array operand with an exact size.
  This tells gcc we're going to clobber this memory region, without having to
   clobber all of "memory" and lets us access local buffers directly using the
   stack pointer, without allocating a separate register to point to them.*/
#define OC_ARRAY_OPERAND(_type,_ptr,_size) \
  (*({ \
    struct{_type array_value__[(_size)];} *array_addr__=(void *)(_ptr); \
    array_addr__; \
  }))

/*Declare an array operand with an exact size.
  This tells gcc we're going to clobber this memory region, without having to
   clobber all of "memory" and lets us access local buffers directly using the
   stack pointer, without allocating a separate register to point to them.*/
#define OC_CONST_ARRAY_OPERAND(_type,_ptr,_size) \
  (*({ \
    const struct{_type array_value__[(_size)];} *array_addr__= \
     (const void *)(_ptr); \
    array_addr__; \
  }))

extern const unsigned short __attribute__((aligned(16))) OC_IDCT_CONSTS[64];

void oc_state_accel_init_x86(oc_theora_state *_state);

void oc_frag_copy_mmx(unsigned char *_dst,
 const unsigned char *_src,int _ystride);
void oc_frag_copy_list_mmx(unsigned char *_dst_frame,
 const unsigned char *_src_frame,int _ystride,
 const ptrdiff_t *_fragis,ptrdiff_t _nfragis,const ptrdiff_t *_frag_buf_offs);
void oc_frag_recon_intra_mmx(unsigned char *_dst,int _ystride,
 const ogg_int16_t *_residue);
void oc_frag_recon_inter_mmx(unsigned char *_dst,
 const unsigned char *_src,int _ystride,const ogg_int16_t *_residue);
void oc_frag_recon_inter2_mmx(unsigned char *_dst,const unsigned char *_src1,
 const unsigned char *_src2,int _ystride,const ogg_int16_t *_residue);
void oc_idct8x8_mmx(ogg_int16_t _y[64],ogg_int16_t _x[64],int _last_zzi);
void oc_idct8x8_sse2(ogg_int16_t _y[64],ogg_int16_t _x[64],int _last_zzi);
void oc_state_frag_recon_mmx(const oc_theora_state *_state,ptrdiff_t _fragi,
 int _pli,ogg_int16_t _dct_coeffs[128],int _last_zzi,ogg_uint16_t _dc_quant);
void oc_loop_filter_init_mmx(signed char _bv[256],int _flimit);
void oc_loop_filter_init_mmxext(signed char _bv[256],int _flimit);
void oc_state_loop_filter_frag_rows_mmx(const oc_theora_state *_state,
 signed char _bv[256],int _refi,int _pli,int _fragy0,int _fragy_end);
void oc_state_loop_filter_frag_rows_mmxext(const oc_theora_state *_state,
 signed char _bv[256],int _refi,int _pli,int _fragy0,int _fragy_end);
void oc_restore_fpu_mmx(void);

#endif

#if !defined(_x86_vc_x86enc_H)
# define _x86_vc_x86enc_H (1)
# include "../lib/x86/x86int.h"

# if defined(OC_X86_ASM)
#  define oc_enc_accel_init oc_enc_accel_init_x86
#  if defined(OC_X86_64_ASM)
/*x86-64 guarantees SIMD support up through at least SSE2.
  If the best routine we have available only needs SSE2 (which at the moment
   covers all of them), then we can avoid runtime detection and the indirect
   call.*/
#   define oc_enc_frag_sub(_enc,_diff,_x,_y,_stride) \
  oc_enc_frag_sub_mmx(_diff,_x,_y,_stride)
#   define oc_enc_frag_sub_128(_enc,_diff,_x,_stride) \
  oc_enc_frag_sub_128_mmx(_diff,_x,_stride)
#   define oc_enc_frag_sad(_enc,_src,_ref,_ystride) \
  oc_enc_frag_sad_mmxext(_src,_ref,_ystride)
#   define oc_enc_frag_sad_thresh(_enc,_src,_ref,_ystride,_thresh) \
  oc_enc_frag_sad_thresh_mmxext(_src,_ref,_ystride,_thresh)
#   define oc_enc_frag_sad2_thresh(_enc,_src,_ref1,_ref2,_ystride,_thresh) \
  oc_enc_frag_sad2_thresh_mmxext(_src,_ref1,_ref2,_ystride,_thresh)
#   define oc_enc_frag_satd(_enc,_dc,_src,_ref,_ystride) \
  oc_enc_frag_satd_sse2(_dc,_src,_ref,_ystride)
#   define oc_enc_frag_satd2(_enc,_dc,_src,_ref1,_ref2,_ystride) \
  oc_enc_frag_satd2_sse2(_dc,_src,_ref1,_ref2,_ystride)
#   define oc_enc_frag_intra_satd(_enc,_dc,_src,_ystride) \
  oc_enc_frag_intra_satd_sse2(_dc,_src,_ystride)
#   define oc_enc_frag_ssd(_enc,_src,_ref,_ystride) \
  oc_enc_frag_ssd_sse2(_src,_ref,_ystride)
#   define oc_enc_frag_border_ssd(_enc,_src,_ref,_ystride,_mask) \
  oc_enc_frag_border_ssd_sse2(_src,_ref,_ystride,_mask)
#   define oc_enc_frag_copy2(_enc,_dst,_src1,_src2,_ystride) \
  oc_int_frag_copy2_mmxext(_dst,_ystride,_src1,_src2,_ystride)
#   define oc_enc_enquant_table_init(_enc,_enquant,_dequant) \
  oc_enc_enquant_table_init_x86(_enquant,_dequant)
#   define oc_enc_enquant_table_fixup(_enc,_enquant,_nqis) \
  oc_enc_enquant_table_fixup_x86(_enquant,_nqis)
#  define oc_enc_quantize(_enc,_qdct,_dct,_dequant,_enquant) \
  oc_enc_quantize_sse2(_qdct,_dct,_dequant,_enquant)
#   define oc_enc_frag_recon_intra(_enc,_dst,_ystride,_residue) \
  oc_frag_recon_intra_mmx(_dst,_ystride,_residue)
#   define oc_enc_frag_recon_inter(_enc,_dst,_src,_ystride,_residue) \
  oc_frag_recon_inter_mmx(_dst,_src,_ystride,_residue)
#   define oc_enc_fdct8x8(_enc,_y,_x) \
  oc_enc_fdct8x8_x86_64sse2(_y,_x)
#  else
#   define OC_ENC_USE_VTABLE (1)
#  endif
# endif

# include "../lib/encint.h"

void oc_enc_accel_init_x86(oc_enc_ctx *_enc);

void oc_enc_frag_sub_mmx(ogg_int16_t _diff[64],
 const unsigned char *_x,const unsigned char *_y,int _stride);
void oc_enc_frag_sub_128_mmx(ogg_int16_t _diff[64],
 const unsigned char *_x,int _stride);
unsigned oc_enc_frag_sad_mmxext(const unsigned char *_src,
 const unsigned char *_ref,int _ystride);
unsigned oc_enc_frag_sad_thresh_mmxext(const unsigned char *_src,
 const unsigned char *_ref,int _ystride,unsigned _thresh);
unsigned oc_enc_frag_sad2_thresh_mmxext(const unsigned char *_src,
 const unsigned char *_ref1,const unsigned char *_ref2,int _ystride,
 unsigned _thresh);
unsigned oc_enc_frag_satd_mmxext(int *_dc,const unsigned char *_src,
 const unsigned char *_ref,int _ystride);
unsigned oc_enc_frag_satd_sse2(int *_dc,const unsigned char *_src,
 const unsigned char *_ref,int _ystride);
unsigned oc_enc_frag_satd2_mmxext(int *_dc,const unsigned char *_src,
 const unsigned char *_ref1,const unsigned char *_ref2,int _ystride);
unsigned oc_enc_frag_satd2_sse2(int *_dc,const unsigned char *_src,
 const unsigned char *_ref1,const unsigned char *_ref2,int _ystride);
unsigned oc_enc_frag_intra_satd_mmxext(int *_dc,
 const unsigned char *_src,int _ystride);
unsigned oc_enc_frag_intra_satd_sse2(int *_dc,
 const unsigned char *_src,int _ystride);
unsigned oc_enc_frag_ssd_sse2(const unsigned char *_src,
 const unsigned char *_ref,int _ystride);
unsigned oc_enc_frag_border_ssd_sse2(const unsigned char *_src,
 const unsigned char *_ref,int _ystride,ogg_int64_t _mask);
void oc_int_frag_copy2_mmxext(unsigned char *_dst,int _dst_ystride,
 const unsigned char *_src1,const unsigned char *_src2,int _src_ystride);
void oc_enc_frag_copy2_mmxext(unsigned char *_dst,
 const unsigned char *_src1,const unsigned char *_src2,int _ystride);
void oc_enc_enquant_table_init_x86(void *_enquant,
 const ogg_uint16_t _dequant[64]);
void oc_enc_enquant_table_fixup_x86(void *_enquant[3][3][2],int _nqis);
int oc_enc_quantize_sse2(ogg_int16_t _qdct[64],const ogg_int16_t _dct[64],
 const ogg_uint16_t _dequant[64],const void *_enquant);
void oc_enc_fdct8x8_mmxext(ogg_int16_t _y[64],const ogg_int16_t _x[64]);

# if defined(OC_X86_64_ASM)
void oc_enc_fdct8x8_x86_64sse2(ogg_int16_t _y[64],const ogg_int16_t _x[64]);
# endif

#endif
