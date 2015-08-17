/**********************************************************************
Each of the companies; Qualcomm, and Lucent, and Motorola (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the EVRC-B Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the EVRC-B be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC-B or the field of use of practice of the 
EVRC-B Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for           */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#ifndef  _PROTO_FX_H_
#define  _PROTO_FX_H_

#include "typedef_fx.h"
#include "macro_fx.h"

#if 0
#include "struct.h"
#endif


class DTFS_fx {

public:
  Word16 a[MAXLAG_WI];
  Word16 b[MAXLAG_WI];
  Word16 lag;
  Word16 Q;

  DTFS_fx(); //constructor
  DTFS_fx operator=(DTFS_fx X2);
  DTFS_fx operator+(DTFS_fx X2);
  DTFS_fx operator-(DTFS_fx X2);
  DTFS_fx operator*(Word16 );
  void zeroPadd_fx(Word16 );
  void zeroInsert_fx(Word16 I);
  void transform_fx(DTFS_fx X2, Word16 *phase_fx, Word16 *out_fx, Word16 M);
  void to_fs_fx(Word16 *x, Word16 M, Word16 *S_fx, Word16 *C_fx);
  Word16 fs_inv_fx(Word16 *x, Word16 N, Word32 ph0, Word16 *cos_table);
  void phaseShift_fx(Word16 ph, Word16 Lag, Word16 *S_fx, Word16 *C_fx);
  void Q2phaseShift_fx(Word16 ph, Word16 Lag, Word16 *S_fx, Word16 *C_fx);
  void DTFS_fx::phaseShift_band_fx(Word16 ph, Word16 lband, Word16 hband, Word16 *S_fx, Word16 *C_fx);
  Word16 alignment_fine_fx(DTFS_fx, Word16 *S_fx, Word16 *C_fx);
  Word16 alignment_full_fx(DTFS_fx, Word16,Word16 *S_fx, Word16 *C_fx);
  void zeroFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx);
  void poleFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx);
  void poleFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_norm);
  void peaktoaverage_fx(Word32 *pos_fx, Word16 *Qpos, Word32 *neg_fx, Word16 *Qneg);
  Word16 alignment_weight_fx(DTFS_fx X2, Word16 Eshift, Word16 *LPC1, Word16 *LPC2, Word16 *S_fx, Word16 *C_fx, Word16 *cos_table, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2);
  Word16 alignment_extract_fx(DTFS_fx X2, Word16 *LPC2, Word16 *S_fx, Word16 *C_fx);
  Word16 alignment_band_fx(DTFS_fx X2, Word16 lband, Word16 hband, Word16 diff, Word16 *S_fx, Word16 *C_fx);
  Word16 glbl_alignment_FPPP_fx(DTFS_fx X2, Word16 num_steps, Word16 *cos_table);
  Word32 freq_corr_fx(DTFS_fx X2, Word16 lband, Word16 hband, Word16 *Qout);
  void adjustLag_fx(Word16 M);
  Word40 getEngy_fx();
  Word40 getEngy_fx(Word16 lband, Word16 hband);
  Word40 setEngy_fx(Word40 en2);
  Word32 setEngyHarm_fx(Word16 f1, Word16 f2, Word16 g1, Word16 g2, Word32 en2, Word16 Qen2, Word16 *Qa);

  void to_erb_fx(Word16 *erb_out);
  void erb_inv_fx(Word16 *erb_in, Word16 *slot, Word16 *mfreq);
  void car2pol_fx();
  void pol2car_fx();
  DTFS_fx copy_phase_fx(DTFS_fx X2);
  Word32 getSpEngyFromResAmp_fx(Word16 lband, Word16 hband,
				       Word16 *curr_lpc, Word16 *sin_tab, 
				       Word16 *cos_tab);
  void dequant_cw_fx(Word16 pl, Word16 p_idx, Word16 *a_idx, Word16 *lasterb, Word16& Lgain, Word16& Hgain, Word16 *curr_erb);
  void rshiftHarmBand_fx(Word16 lband, Word16 hband, Word16 shift);
  void dequant_cw_memless_fx(Word16 p_idx, Word16 *a_idx);
  Word16 quant_cw_fx(Word16 pl, Word16 *curr_lpc, Word16& p_idx, Word16 *a_idx,
    Word16 *sin_tab, Word16 *cos_tab);
  void DTFS_fx::quant_target(Word16 *curr_lpc, Word16 *w, Word16 *target,
    Word16 *sin_tab, Word16 *cos_tab);
  void DTFS_fx::quant_cw_memless_fx(Word16 *curr_lpc,
    Word16& p_idx, Word16 *a_idx, Word16 *sin_tab, Word16 *cos_tab);

  Word16 glbl_alignment_FPPP_fx(DTFS_fx X2);
  void fast_fs_inv_fx(Word16 *out, Word16 N, Word16 LOG2N);

};

void poleFilter_setup_fx(Word16 *LPC, Word16 N, Word16 lag, Word16 *S_fx, Word16 *C_fx, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_norm);


/*======================================================================*/
/*         ..OLP definition                                             */
/*----------------------------------------------------------------------*/
extern void fndppf_fx(Word16 *delay_fx, Word16 *beta_fx, Word16 *buf, Word16 dmin, Word16 dmax, Word16 length);
extern Word16 JCELP_Config_fx(Word16 lag, Word16 gain, Word16 l_subfr, UNS_Word16 pos[], Word16 *offset,Word16 *sign, Word16 *ratio);

/*======================================================================*/
/* Factorial packing and unpacking                                      */
/*======================================================================*/
extern void factorial_pack_fx(Word16 *vector_fx, Word16 *pw35);
extern void factorial_unpack_fx(Word16 *pw35, Word16 *vector_fx);

/*======================================================================*/
/* NACF functions                                                       */
/*======================================================================*/
//extern Word16 get_nacf_at_pitch_fx(Word16 resid[], Word16 pitch[], Word16 nacf[]);
extern void get_nacf_at_pitch_fx(Word16 resid[], Word16 pitch[], Word16 nacf[]);
/*======================================================================*/
/* Filter functions                                                     */
/*======================================================================*/
extern void fir_filter_fx (Word16 *, Word16 *, Word16 *,Word16, Word16, Word16);

extern void pz_filter_fx (Word16 *, Word16 *, Word16 *, Word16 *,
			 Word16 *, Word16, Word16, Word16);

extern void pz_filter_dp_fx (Word16 b [], Word16 a [], Word16 x [], Word16 y [],
			     Word32 buf [], Word16 P, Word16 N, Word16 Qa);

/*======================================================================*/
/* Mode decision                                                        */
/*======================================================================*/
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
MODE_FX new_mode_decision_fx(Word16 va, Word16 prev_voiced,
			  Word16 prev_mode, Word16 *nacf_ap_fx, 
			  Word16 *curr_snr_fx, Word16 *in_fx);
#else
MODE_FX new_mode_decision_fx(Word16 va, Word16 prev_voiced,
			  Word16 prev_mode, Word16 *nacf_ap_fx, Word16 *curr_ns_snr_fx,
			  Word16 *curr_snr_fx, Word16 *in_fx);
#endif
/*======================================================================*/
/* lsp_vq  (all rates)                                                  */
/*======================================================================*/

extern void enc_lsp_vq_28_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]);
 
extern void dec_lsp_vq_28_fx( Word16 codes[], Word16 lsp[]);

extern void enc_lsp_vq_22_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]);

extern void dec_lsp_vq_22_fx( Word16 codes[], Word16 lsp[]);  

extern void enc_lsp_vq_16_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]);

extern void dec_lsp_vq_16_fx( Word16 codes[], Word16 lsp[]);

extern void lspmaq_fx1(Word16 *x,Word16 *nsub,Word16 *nsiz,Word16 *w,Word16 *y, Word16 *index,Word16 *lsptab8_fx1);
// for voiced

extern void lspmaq_fx8(Word16 *x,Word16 *nsub,Word16 *nsiz,Word16 *w,Word16 *y, Word16 *index,Word16 *lsptab8_fx1);
// for 1/8th rate

extern void weight_lsp8_fx(Word16 *x,Word16 *w, Word16 *shft_fctr);

extern void weight_lsp_fx(Word16 lsp_nq_fx[],Word16 w[]);

extern void lspmaq_dec_fx(short *nsub, short *nsiz,short *y, short *index,short *lsptab_fx);
//for voiced

extern void lspmaq_dec_fx8(short *nsub, short *nsiz, short *y, short *index,short *lsptab_fx);
//for 1/8th rate

/*======================================================================*/
/* LPC utility functions, such prediction filter, systhesis filter etc. */
/*======================================================================*/

void prediction_filter_fx(Word16 b[], Word16 x[], Word16 y[], Word16 buf[],
		 Word16 P, Word16 N, Word16 Qa);
void synthesis_filter_fx (Word16 b[], Word16 x[], Word16 y[], Word32 buf[],
		 Word16 P, Word16 N, Word16 Qa);

void weight_fx(Word16 coef_orig[], Word16 w[], Word16 coef_w[], Word16 N);

void lpc_weighted_filtering_fx(Word16 input[], Word16 acoef[],
			       Word16 acoef_n[], Word16 acoef_d[],
			       Word32 mem1[], Word16 mem2[], Word32 mem3[],
			       Word16 output[], Word16 N);
void ImpulseRzp_fx(Word16 acoef[], Word16 acoef_n[], Word16 acoef_d[],
		   Word16 ir[], Word16 N);

void ZeroInput_fx(Word16 *input, Word16 *acoef, Word16 *acoef_n,
		  Word16 *acoef_d, Word16 *output, Word16 len, Word16 type);

    void ConvolveImpulseR_fx(Word16 input[], Word16 ir[], Word16 output[],
			 Word16 L, Word16 N);

Word16 lpcanalys_fx(Word32 *pc, Word16 *input, Word16 order, Word16 len,
		  Word32 *R);

Word16 lpc2lsp_fx(Word32 *a, Word16 *freq, Word16 *old_freq);

void lsp2lpc_fx(Word16 *freq, Word16 *prev_a, Word16 *a);

void  Interpol_fx(Word16 *lsp_old, Word16 *lsp_curr, Word16 *lsp_out,
		  Word16 k);

/*======================================================================*/
/* HALF-RATE FCB SEARCH                                                 */
/*======================================================================*/

void corr_xh_fx(Word16 *x, Word16 *dn, Word16 *h, Word16 l, Word16 *n,Word16 ns);
void cod3_10jcelp_fx( Word16 xn[], Word16 h[], Word16 pit_lag, Word16 pit_gain, Word16 l_subfr,
                      Word16 cod[], Word16 *gain_code, Word16 *indices);
void dec3_10jcelp_fx(Word16 pit_lag,Word16 pit_gain, Word16 l_subfr, Word16 *indices,
                    Word16 cod[]);

/*======================================================================*/
/* FULL-RATE FCB SEARCH                                                 */
/*======================================================================*/
void cod7_35_fx(Word16 *xw, Word16 *x, Word16 l_subfr, Word16 *h,
                Word16 *ck, Word16 *gain, Word16 *codeword);
void ACELP_Code_fx(
  Word16 xn[],         /* (i)     :Target signal for codebook.       */
  Word16 res2[],       /* (i)     :Residual after pitch contribution */
  Word16 h1[],         /* (i)     :Impulse response of filters.      */
  int   T0,           /* (i)     :Pitch lag.                        */
  Word16 pitch_gain,   /* (i)     :Pitch gain.                       */
  int   l_subfr,      /* (i)     :Subframe lenght.                  */
  Word16 code[],       /* (o)     :Innovative vector.                */
  Word16 *gain_code,   /* (o)     :Innovative vector gain.           */
  Word16  *index,        /* (o)     :Index of codebook + signs         */
  int  choice         /* (i)     :Choice of innovative codebook     */
                      /*          0 -> 10 bits                      */
                      /*          1 -> 35 bits                      */
);
void pit_shrp_fx(Word16 *x,	int pit_lag, Word16 sharp, int L_subfr);

/*======================================================================*/
/* RCELP and ACB related functions                                      */
/*======================================================================*/
void Interpol_delay_fx(Word16 delay_old, Word16 delay_curr,
		       Word32 *delay_out, Word16 k);
void getgain_fx(Word16 *excit, Word16 *mresid, Word16 *h,
		Word16 sflen, Word16& lambda, Word16& g_idx);
void ComputeACB_fx(Word16 *residualm, Word32 *delay3,
		   Word16 *residual, Word16 guard, Word16& dpm,
		   Word16& accshift, Word16 beta, Word16 length);
void acb_excitation_fx(Word16 *excit, Word32 *delay3, Word16 gain, Word16 sflen);
void rclp_block_interp_fx(Word16 *resid, Word32 *delay, Word16 *intrp_coef,
			  Word16 N, Word16 len);
void delay_contour_fx(Word32 *delay3, Word16 len, Word32 *delay_out);

/*======================================================================*/
/* Post-filter                                                          */
/*======================================================================*/
void Post_Filter_fx(Word16 *syn, Word16 *Lsp, Word16 *Az, Word16 *synpf,
		    Word32 delayi, Word16 agc_fact, Word16 l_subfr);


/*======================================================================*/
/* QUARTER-RATE NELP                                                    */
/*======================================================================*/

Word16  quantize_uvg_fx(Word16 *G, Word16& iG1, Word16 *iG2, Word16 *quantG);

void dequantize_uvg_fx(Word16 iG1, Word16 *iG2, Word16 *G);

void nelp_encoder_fx();

#ifdef VOIP_DORA_NELP
void nelp_decoder_fx(Word16 outFbuf_fx[], Word16 post_filter,Word16 erasure,short time_warp_fraction, short* obuf_len);
#else
void nelp_decoder_fx(Word16 outFbuf_fx[], Word16 post_filter,Word16 erasure);
#endif


/*======================================================================*/
/* CELP ENCODER AND DECODER                                             */
/*======================================================================*/
void InitEncoder_fx();
void  encode_fx (Word16& rate, Word16 *codeBuf);
void InitDecoder_fx();

#ifdef VOIP_DORA
void  decode_fx (short*  codeBuf,short  rate,short  post_filter,short*  outFbuf,short run_length, short phase_offset,short time_warp_fraction,short* obuf_len);
void fer_processing_fx(short *out,short pf,short prev_rate, Word16 run_length, Word16 phase_offset, short time_warp_fraction,short* obuf_len);
#else
void  decode_fx (short*  codeBuf,short  rate,short  post_filter,short*  outFbuf);
void fer_processing_fx(short *out,short pf,short prev_rate);
#endif

extern void Bitpack(short in, unsigned short *TrWords, short NoOfBits, short *ptr);
extern void BitUnpack(short *out, unsigned short *RecWords, short NoOfBits, short *ptr);



#ifdef VOIP_DORA_CELP
void Synthesis_of_decsph_fx(Word16 LOldlspD_fx[], Word16 Llsp_fx[], Word16 LpdelayD_fx, Word16 Ldelay_fx,
Word16 LPitchMemoryD_frame_fx[], Word32 LSynMemory_fx[],Word16 LoutFbuf_fx[],
Word16 erasure,Word16 post_filter, Word16 num_samples, Word16 temp_samples_count);
void celp_erasure_decoder_fx(Word16 outFbuf_fx[],Word16 post_filter,short phase_offset,short run_length,short time_warp_fraction,short* obuf_len );
#else
void celp_erasure_decoder_fx(Word16 outFbuf_fx[],Word16 post_filter);
void Synthesis_of_decsph_fx(Word16 LOldlspD_fx[], Word16 Llsp_fx[], Word16 LpdelayD_fx, Word16 Ldelay_fx,
Word16 LPitchMemoryD_frame_fx[], Word32 LSynMemory_fx[],Word16 LoutFbuf_fx[],
Word16 erasure,Word16 post_filter);


#endif




/*======================================================================*/
/* VAD and pre_encode                                                   */
/*======================================================================*/
Word16 pre_encode_fx (Word16 *inFbuf, Word32 *Rs, Word16& r_scale);
Word16 voice_activity_detection_fx(Word32 *Rs, ENCODER_MEM_fx *e_mem,
				   Word16 norm_r, Word16 beta, Word16 mode);

/*======================================================================*/
/* 1/8th rate related functions                                         */
/*======================================================================*/
Word16 ran_g_fx(Word16& seed0, Word16& extra_flag, Word16& extra_ran);
void silence_encoder_fx();
void silence_decoder_fx(Word16 *outFbuf, Word16 post_filter, Word16 fer_flag);

/*======================================================================*/
/* FULL-RATE CELP                                                    */
/*======================================================================*/
void celp_encoder_fx(int bit_rate);

#ifdef VOIP_DORA_CELP
short celp_decoder_fx(Word16 *outFbuf,short post_filter,int bit_rate,Word16 phase_offset, Word16 run_length, Word16 time_warp_fraction, short*     obuf_len);
#else
short celp_decoder_fx(Word16 *outFbuf,short post_filter,int bit_rate);
#endif

void Weight2Res_fx(
     Word16 *input,
     Word16 *output,
     Word16 *pci_fx,
     Word16 *wpci_gma1_fx,
     Word16 *wpci_gma2_fx,
     short order,
     short length
);

/*======================================================================*/
/* 80Hz high pass filter                                                */
/*======================================================================*/
void hpf80_fx(Word16 *input, Word16 *output, Word16 n);

/*======================================================================*/
/* EVRC noise suppressor                                                */
/*======================================================================*/
void noise_suprs_fx(Word16 * farray_ptr, Word16 *ns_snr);

/*======================================================================*/
/* WI-related function prototypes                                       */
/*======================================================================*/
Word40 Mpy_32_16(Word32 Lx, Word16 x);
void WIsyn_fx(DTFS_fx prevCW_fx, DTFS_fx *curr_cw_fx, Word16 *prev_lsp_fx, Word16 *curr_lsp_fx, Word16 *ph_offset_fx, Word16 *out_fx, Word16 N, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2);

Word16 alignment_extract_td_fx(Word16 *x1, Word16 *x2, Word16 lag);
void ppp_extract_pitch_period_fx(Word16 *in, Word16 *out, Word16 l);
void cubicPhase_fx(Word16 ph1, Word16 ph2, Word16 L1, Word16 L2,
		   Word16 N, Word16 *phOut);
void erb_slot_fx(Word16 lag, Word16 *out, Word16 *mfreq);
void LPCPowSpect_fx(Word16 *freq, Word16 *LPC, Word16 Nf, Word16 Np,
		    Word16 *out);
void GetSinCosTab_fx(Word16 L, Word16 *sinTab, Word16 *cosTab);
void erb_add_fx(Word16 pl, Word16 l, Word16 *index, Word16 *prev_erb,
		Word16 *curr_erb);
void erb_diff_fx(Word16 pl, Word16 l, Word16 *prev_erb, Word16 *curr_erb,
		 Word16 *curr_lpc, Word16 *index);
void erb_dequant_fx(Word16 l, Word16 *index, Word16 *curr_erb);
void erb_quant_fx(Word16 l, Word16 *curr_erb, Word16 *curr_lpc,
		  Word16 *index);
//void ppp_quarter_decoder_fx(DTFS_fx *currCW_q_fx, Word16 pl, Word16 *curr_lsp_fx,Word16 *S_fx, Word16 *C_fx);
void ppp_quarter_decoder_fx(Word16 pidx,Word16 *aidx,DTFS_fx *currCW_q_fx, Word16 pl, Word16 *curr_lsp_fx,Word16 *S_fx, Word16 *C_fx);
void ppp_full_decoder_fx(Word16 p_idx, Word16 *a_idx, Word16 rot_idx,
			 Word16 *band_idx, Word16 *curr_lpc,
			 DTFS_fx *CWout, Word16 *sin_tab, Word16 *cos_tab);
void ppp_full_encoder_fx(DTFS_fx *CWout, DTFS_fx CWin, Word16 *curr_lpc,
			 Word16& p_idx, Word16 *a_idx, Word16& rot_idx,
			 Word16 *band_idx, Word16 *sin_tab, Word16 *cos_tab);

Word16  ppp_quarter_encoder_fx(DTFS_fx *currCW_q_fx, Word16 pl, DTFS_fx currCW_fx,
			 Word16 *curr_lsp_fx, DTFS_fx *targetCW_fx,Word16 *S_fx, Word16 *C_fx,Word16& pidx, Word16 *aidx);

void voiced_encoder_fx(Word16 *out_fx, Word16& rate);

#ifdef VOIP_DORA_VOICE
void voiced_decoder_fx(Word16 *outFbuf,Word16 post_filter, Word16 fer_flag,Word16 phase_offset, Word16 run_length, Word16 time_warp_function, short* obuf_len);
#else
void voiced_decoder_fx(Word16 *outFbuf,Word16 post_filter, Word16 fer_flag);
#endif

Word16 bad_rate_check(Word16 opt,Word16*);

void Init_Post_Filter_fx(void);
void Init_ZeroInput_fx(void);

void TTY_DTMF_pack (Word16 *data_buf, Word16 TxPkt [], Word16 PktPtr[]);
Word16 TTY_DTMF_unpack(Word16 RxPkt [], Word16 PktPtr [], Word16* data_buf);

void Init_globs_fx();
void lsp_spread(Word16 *lsp_fx);
void LPC_pred2refl_fx(Word16 *p_fx, Word16 *k_fx,Word16 NPTS);
#endif
