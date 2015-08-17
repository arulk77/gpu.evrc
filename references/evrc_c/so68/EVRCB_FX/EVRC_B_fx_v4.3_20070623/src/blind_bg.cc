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
/******************************************************************************
*                                                                            *
*     M O T O R O L A   C O N F I D E N T I A L   I N F O R M A T I O N      *
*                                                                            *
*                   Copyright 1994 - 2005, Motorola, Inc.                    *
*                                                                            *
*            This software has been provided to QUALCOMM under a             *
*           Source Code Loan Agreement effective December 5, 2002.           *
*                                                                            *
*****************************************************************************/

/* ##### Package date: Wed Sep 14 16:50:00 CDT 2005 ##### */

#include <math.h>
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "globs_fx.h"
#include "typedef_fx.h"
#include "lpcana_tab.h"		/* For gamma994, etc.*/

#if MOT_SUPPRESSED_EIGHTH_RATE
//#include "lspq_fx.dat"
extern Word16 lsptab8_fx[160];	/* Instead of including lspq_fx.dat */

#define MOT_ADJUST_BG_ESTIMATE 1



#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

#ifndef PI
#define PI              3.14159265358979323846
#endif
#ifndef SPEECH_BUFFER_LEN
#define SPEECH_BUFFER_LEN 160
#endif


/*======================================================================*/
/*         ..For use when eighth rate frames are dropped.               */
/*----------------------------------------------------------------------*/
#define BBG_WINDOW_LEN 104
#define BBG_M 80
#define BBG_DELAY 24
#define BBG_FFTLEN 128

#define BBG_ALPHA_FIX 0x68F6
//#define BBG_ALPHA_FIX 0x7333
#define BBG_BETA1_FIX 0x4CCD
#define BBG_BETA1_MINUS1_FIX 0xCCCD
#define BBG_BETA2_FIX 0x7F0A
#define BBG_BETA2_MINUS1_FIX 0xFF0A
#define BBG_BETA2r_FIX 0x65A2
#define BBG_BETA2r_MINUS1_FIX 0xE5A2
#define BBG_MIN_LEVEL_FIX -1920


extern Word16 ran0_fx(Word16 &seed);
extern Word16 cos64_fx(Word16);
extern Word16 sin64_fx(Word16);

void r_fft(Word16 *, Word16);		/* Defined in r_fft.c */

extern Word16 BBG_window_FIX[BBG_WINDOW_LEN];


#define BBG_NUM_CHAN 16
#define BBG_LO_CHAN 0
#define BBG_HI_CHAN 15

static Word16 E_bgn_FIX[BBG_NUM_CHAN+2];
/* FOR SMOOTHING E_BGN_BEFORE RE-GENERATING COMFORT NOISE */
static Word16 E_bgn_smooth_FIX[BBG_NUM_CHAN+2];
static Word16 BBG_frmSkipCnt = 0;
static Word16 BBG_frmSkipCnt2 = 0;

extern Word16 ch_tbl[BBG_NUM_CHAN][2];
extern Word16 ch_tbl_sh[BBG_NUM_CHAN][2];

/*
 * Updates the background noise envelope shape
 */

static void update_bgestimate(Word16 *buf16, int rate, Word16 walpha_FIX)
  {
  static int firstTime = 1;
  Word16 s_m_FIX[BBG_FFTLEN];
  static Word16 window_overlap_FIX[BBG_DELAY];
  static Word16 E_ch_mem_FIX[BBG_NUM_CHAN+2];
  Word16 E_ch_FIX[BBG_NUM_CHAN+2];
  Word16 *s_m_FIX_ptr;
  Word16 *win_FIX_ptr;
  int indx;
  Word32 tmp32;
  Word40 tmp40;
  Word16 tmp16;

  if (firstTime)
    {
    /* Move these to init_bbg() */
    firstTime = 0;
    for (indx=0; indx<BBG_DELAY; indx++)
      {
      window_overlap_FIX[indx] = 0;
      }
    for (indx=0; indx<BBG_NUM_CHAN+2; indx++)
      {
      E_bgn_FIX[indx] = 0xFB00;			/* -10.0 in Q7 */
      E_ch_mem_FIX[indx] = 0;
/* FOR SMOOTHING E_BGN BEFORE RE-GENERATING COMFORT NOISE */
      E_bgn_smooth_FIX[indx] = 0xFB00;		/* -10.0 in Q7 */
      }
    }

  /* Do actual update in this section */
  BBG_frmSkipCnt = add(BBG_frmSkipCnt, 1);

  /* Process first "subframe" of 80 samples */
  s_m_FIX_ptr = &s_m_FIX[0];
  win_FIX_ptr = &BBG_window_FIX[0];
  for (indx=0; indx<BBG_DELAY; indx++)
    {
    (*s_m_FIX_ptr++) = mult_r(window_overlap_FIX[indx], (*win_FIX_ptr++));
    }
  for (indx=0; indx<BBG_M; indx++)
    {
    (*s_m_FIX_ptr++) = mult_r(buf16[indx], (*win_FIX_ptr++));
    }
  for (indx=0; indx<BBG_FFTLEN-(BBG_M+BBG_DELAY); indx++)
    {
    (*s_m_FIX_ptr++) = 0;
    }
  for (indx=0; indx<BBG_DELAY; indx++)
    {
    window_overlap_FIX[indx] = buf16[BBG_M - BBG_DELAY + indx];
    }
/* MOVED HERE FROM update_bbg_estimate() */
  if (rate == 1)
    {
    s_m_FIX_ptr = &s_m_FIX[0];
    for (indx=0; indx<BBG_M; indx++)
      {
      (*s_m_FIX_ptr++) = shl(*s_m_FIX_ptr, 1);
    }
  for (indx=0; indx<BBG_DELAY; indx++)
    {
      window_overlap_FIX[indx] = shl(window_overlap_FIX[indx], 1);
      }
    }

  /* Compute fft of real signal */
  r_fft(s_m_FIX, +1);

  /* Now compute magnitude in dB */
  for (indx=0; indx<BBG_FFTLEN/2; indx++)
    {
    tmp40 = L_mac40(0, s_m_FIX[2*indx], s_m_FIX[2*indx]);
    tmp40 = L_mac40(tmp40, s_m_FIX[2*indx+1], s_m_FIX[2*indx+1]);
    /* tmp40 in Q1, convert to Q0 */
    tmp40 = L_shr40(tmp40, 1);
    s_m_FIX[indx] = round32_16(log10_lut(tmp40));
    if (s_m_FIX[indx]<-5120)
      {
      s_m_FIX[indx] = -5120;
      }
    }
  if (BBG_frmSkipCnt < 3)
    {
    int chan;
    int j1, j2;
    j1 = ch_tbl[0][0];
    for (indx=0; indx<j1; indx++)
      {
      E_ch_FIX[indx] = s_m_FIX[indx];
      }
    for (chan=0; chan<BBG_NUM_CHAN; chan++)
      {
      j1 = ch_tbl[chan][0];
      j2 = ch_tbl[chan][1];
      tmp40 = 0;
      for (indx=j1; indx<=j2; indx++)
	{
	tmp40 = L_msu40(tmp40, 0x8000, s_m_FIX[indx]);
	}
      if (ch_tbl_sh[chan][0] == TRUE)
	{
	tmp32 = (Word32)L_sat32_40(L_shr40(tmp40, ch_tbl_sh[chan][1]));
	tmp16 = round32_16(tmp32);
	E_ch_FIX[chan+2] = tmp16;
	}
      else
	{
	Word16 norm_shift;
	norm_shift = norm32_l40(tmp40);
	tmp16 = extract_h((Word32)L_shl40(tmp40, norm_shift));
	tmp32 = L_mult(tmp16, ch_tbl_sh[chan][1]);
	tmp16 = round32_16(L_shr(tmp32, norm_shift));
	E_ch_FIX[chan+2] = tmp16;
      }
      }
    for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
      {
      E_bgn_FIX[indx] = E_ch_FIX[indx];
      if (E_bgn_FIX[indx]<BBG_MIN_LEVEL_FIX)
	{
	E_bgn_FIX[indx] = BBG_MIN_LEVEL_FIX;
	E_ch_FIX[indx] = E_bgn_FIX[indx];
	}
#if 1
      /* Initialization was done without bin grouping. Fixed here.*/
      E_ch_mem_FIX[indx] = E_ch_FIX[indx];
#endif
      }
    }
  else
    {
    int chan;
    int j1, j2;
    j1 = ch_tbl[0][0];
    for (indx=0; indx<j1; indx++)
      {
      E_ch_FIX[indx] = s_m_FIX[indx];
      }
    for (chan=0; chan<BBG_NUM_CHAN; chan++)
      {
      j1 = ch_tbl[chan][0];
      j2 = ch_tbl[chan][1];
      tmp32 = 0;
      tmp16 = 0;
      tmp40 = 0;
      for (indx=j1; indx<=j2; indx++)
	{
	tmp40 = L_msu40(tmp40, 0x8000, s_m_FIX[indx]);
	}
      if (ch_tbl_sh[chan][0] == TRUE)
	{
	tmp32 = (Word32)L_sat32_40(L_shr40(tmp40, ch_tbl_sh[chan][1]));
	tmp16 = round32_16(tmp32);
	E_ch_FIX[chan+2] = tmp16;
	}
      else
	{
	Word16 norm_shift;
	norm_shift = norm32_l40(tmp40);
	tmp16 = extract_h((Word32)L_shl40(tmp40, norm_shift));
	tmp32 = L_mult(tmp16, ch_tbl_sh[chan][1]);
	tmp16 = round32_16(L_shr(tmp32, norm_shift));
	E_ch_FIX[chan+2] = tmp16;
      }

      }
    walpha_FIX = mult_r(walpha_FIX, BBG_ALPHA_FIX);
    if (rate == 1)
      {
#if 1
      walpha_FIX = mult_r(walpha_FIX, 0x4CCD);
      tmp16 = add(walpha_FIX, 0x8000);
#endif
      for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
	{
	tmp32 = L_mult(walpha_FIX, E_ch_mem_FIX[indx]);
       	tmp32 = L_msu(tmp32, tmp16, E_ch_FIX[indx]);
       	E_ch_mem_FIX[indx] =  round32_16(tmp32);
	}
      }
    else
      {
      tmp16 = add(walpha_FIX, 0x8000);
      for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
	{
	tmp32 = L_mult(walpha_FIX, E_ch_mem_FIX[indx]);
// add 20log10(1.5) in Q7 format
       	tmp32 = L_msu(tmp32, tmp16, add(E_ch_FIX[indx], 0x01C3));
	E_ch_mem_FIX[indx] = round32_16(tmp32);
	}
      }
      }
  if (rate == 1)
    {
    for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
      {
      tmp32 = L_mac(0, BBG_BETA1_FIX, E_ch_mem_FIX[indx]);
      tmp32 = L_msu(tmp32, BBG_BETA1_MINUS1_FIX, E_ch_FIX[indx]);
      E_bgn_FIX[indx] = round32_16(tmp32);
      }
    }
  else
    {
    for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
      {
      if (E_ch_mem_FIX[indx]<E_bgn_FIX[indx])
	{
#if 1
	if (E_ch_mem_FIX[indx]>BBG_MIN_LEVEL_FIX)
	  {
	  E_bgn_FIX[indx] = E_ch_mem_FIX[indx];
	  }
	else
	  {
	  E_bgn_FIX[indx] = BBG_MIN_LEVEL_FIX;
	  E_ch_mem_FIX[indx] = E_bgn_FIX[indx];
	  }
#else
	E_bgn_FIX[indx] = E_ch_mem_FIX[indx];
#endif
	}
      else
	{
	/* Prevent noise floor from rising too fast when signal is suspected
	 * to be high energy speech.
	 */
	  if (sub(sub(E_ch_mem_FIX[indx], E_bgn_FIX[indx]), 1536) > 0)
//	  if ((E_ch_mem[indx]-E_bgn[indx])>12)
	    {
	    E_bgn_FIX[indx] = add(E_bgn_FIX[indx], 0x0001);	// 0.01 in Q7
	    }
	  else
	    {
	    E_bgn_FIX[indx] = add(E_bgn_FIX[indx], 0x0003);	// 0.02 in Q7
	    }
	}
      }
    }

  }


/*
 * Generates noise using estimated background noise shape
 */
static void gen_bg_fromestimate(Word16 *buf16)
  {
  Word16 indx;
  static Word16 seed0_fx = 19267;
  Word16 tmp16;
  Word32 tmp32;
  Word40 tmp40;
  static Word16 X_dec_ph_FIX[2];
  static Word16 X_dec_overlap_FIX[BBG_DELAY];
  Word16 X_dec_FIX[BBG_FFTLEN];
  Word16 chan;
  Word16 j1, j2;

#if 1
  /* SMOOTH E_BGN BEFORE RE-GENERATING COMFORT NOISE */
  if (BBG_frmSkipCnt2 < 10) // Wait for first true estimate
    {
    BBG_frmSkipCnt2 = add(BBG_frmSkipCnt2, 1);
    for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
      {
      tmp32 = L_mac(0, BBG_BETA2r_FIX, E_bgn_smooth_FIX[indx]);
      tmp32 = L_msu(tmp32, BBG_BETA2r_MINUS1_FIX, E_bgn_FIX[indx]);
      E_bgn_smooth_FIX[indx] = round32_16(tmp32);
      }
    }
  else
    {
    for (indx=0; indx<(BBG_NUM_CHAN+2); indx++)
      {
      tmp32 = L_mac(0, BBG_BETA2_FIX, E_bgn_smooth_FIX[indx]);
      tmp32 = L_msu(tmp32, BBG_BETA2_MINUS1_FIX, E_bgn_FIX[indx]);
      E_bgn_smooth_FIX[indx] = round32_16(tmp32);
      }
    }
#endif
  X_dec_FIX[0] = 0;
  X_dec_FIX[1] = 0;
  tmp16 =  ran0_fx(seed0_fx);
  X_dec_ph_FIX[0] = cos64_fx(tmp16);
  X_dec_ph_FIX[1] = sin64_fx(tmp16);
  /* Divide by 20, shift left 3 to convert Q23 to Q26 */
/* USE SMOOTHED E_BGN FOR RE-GENERATING COMFORT NOISE */
  tmp32 = L_mult(E_bgn_smooth_FIX[1], 0x3333);
  tmp40 = dsp_pow10(tmp32);	/* tmp40 in Q15 */
  tmp32 = (Word32)L_sat32_40(L_shl40(tmp40, 1));
  tmp16 = round32_16(tmp32);
  X_dec_FIX[2] = mult_r(tmp16, X_dec_ph_FIX[0]);
  X_dec_FIX[2+1] = mult_r(tmp16, X_dec_ph_FIX[1]);

  for (chan=0; chan<(BBG_NUM_CHAN); chan++)
    {
    j1 = ch_tbl[chan][0];
    j2 = ch_tbl[chan][1];
    for (indx=j1; indx<=j2; indx++)
  {
      tmp16 = ran0_fx(seed0_fx);
      X_dec_ph_FIX[0] = cos64_fx(tmp16);
      X_dec_ph_FIX[1] = sin64_fx(tmp16);
      /* Divide by 20, shift left 3 to convert Q23 to Q26 */
      /* USE SMOOTHED E_BGN FOR RE-GENERATING COMFORT NOISE */
      tmp32 = L_mult(E_bgn_smooth_FIX[chan+2], 0x3333);
      tmp40 = dsp_pow10(tmp32);	/* tmp40 in Q15 */
      tmp32 = (Word32)L_sat32_40(L_shl40(tmp40, 1));
      tmp16 = round32_16(tmp32);
      X_dec_FIX[2*indx] = mult_r(tmp16, X_dec_ph_FIX[0]);
      X_dec_FIX[2*indx+1] = mult_r(tmp16, X_dec_ph_FIX[1]);
      }
    }
  r_fft(X_dec_FIX, -1);

  if (BBG_frmSkipCnt < 1)	// Wait for first true estimate
    {
    for (indx=0; indx<(BBG_M+BBG_DELAY); indx++)
      {
      /* X_dec_FIX in Q0 */
      X_dec_FIX[indx] = mult_r(sub(ran0_fx(seed0_fx), 0x4000), 0x0006);
      }
    }
  for (indx=0; indx<BBG_M+BBG_DELAY; indx++)
    {
    X_dec_FIX[indx] = mult_r(X_dec_FIX[indx], BBG_window_FIX[indx]);
    }
  for (indx=0; indx<BBG_DELAY; indx++)
    {
    X_dec_FIX[indx] = add(X_dec_FIX[indx], X_dec_overlap_FIX[indx]);
    X_dec_overlap_FIX[indx] = X_dec_FIX[indx+BBG_M];
    }

  for (indx=0; indx<BBG_M; indx++)
    {
    buf16[indx] = X_dec_FIX[indx];
    }

  }


#if 1
static Word16 speechBufHist_fx[GUARD+LOOKAHEAD_LEN];
#else
static Word16 speechBufHist_fx[GUARD];
#endif


static void bbg_eighth_rate_encode(Word16 inBuf[SPEECH_BUFFER_LEN], Word16 *outBuf16)
  {
    /*....(local) variables....*/
  register int i, j;	/* counters */
  
  static Word16 last_delay;
  Word16 k, k0;
  Word16 d1h, d1l, offset;
  Word32 Ltemp;
  Word40 Lacc;

  Word32 pci_dp[ORDER];  // LPC coefficients (output of LPC analysis), Q27
  Word16 speechBuf_fx[GUARD+FrameSize+LOOKAHEAD_LEN];
  Word32 bbg_Rs;
  Word16 lsp_noise_fx[ORDER];
  Word16 lsp_noiseW_fx[ORDER];
  Word16 lspQ_noise_fx[ORDER];
  Word16 lspi_fx[ORDER];
  static Word16 lpci_fx[ORDER];
  Word16 lresidual_fx[SubFrameSize];
  Word16 H_noise_fx[Hlength];
  Word16 sfSize;
  Word16 r_scale;
  Word16 nsub8[2]={5,5};
  Word16 nsize8[2]={16,16};
  Word16 sf;
  Word16 indx;
  Word16 maxEnergy_fx;
  Word32 minErr_fx;
  Word16 D;
  int best;
  Word40 tmp40;
  Word32 tmp32;
  Word16 tmp16;
  Word32 scale_fx;
  Word16 Sum_fx[3];
  static short first_time=0;

  if (first_time==0)
    {
      first_time=1;
      for(k=0;k<ORDER;k++)
	lpci_fx[k]=pci_init_tab[k];

    }
  
  /*....execute....*/
  
  /* Re-initialize PackWdsPtr */

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  PktPtr[0]=16;PktPtr[1]=0;
  for (i=0; i<PACKWDSNUM; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  RxPkt[i]=0;
  }

#if 1
  /* Process Data */
  for (j = 0; j < FrameSize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
    speechBuf_fx[j + GUARD + LOOKAHEAD_LEN] = inBuf[j];
    }
  
  /* Make sure HPspeech - HPspeech+2*GUARD have the right memory */
  for (j=0; j<GUARD+LOOKAHEAD_LEN; j++) {
	  speechBuf_fx[j] = speechBufHist_fx[j];
  }

  /* Update ConstHPspeech for next frame */
  for (j=0; j<GUARD+LOOKAHEAD_LEN; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  speechBufHist_fx[j] = speechBuf_fx[j+GUARD+LOOKAHEAD_LEN];
  }

#else

  /* Process Data */
  for (j = 0; j < FrameSize + LOOKAHEAD_LEN; j++) {
#ifdef WMOPS_FX
  move16();
#endif
    speechBuf_fx[j + GUARD] = inBuf[j];
  }
  
  /* Make sure HPspeech - HPspeech+2*GUARD have the right memory */
  for (j=0; j<GUARD; j++) {
	  speechBuf_fx[j] = speechBufHist_fx[j];
  }

  /* Update ConstHPspeech for next frame */
  for (j=0; j<GUARD; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  speechBufHist_fx[j] = speechBuf_fx[j + FrameSize];
  }
#endif

  /* Calculate prediction coefficients */
  /* reflection coef. not needed-returned in Scratch */
  r_scale=lpcanalys_fx(pci_dp, speechBuf_fx, ORDER, LPC_ANALYSIS_WINDOW_LENGTH,
			&bbg_Rs);

  /* Bandwidth expansion */
  for (i=0;i<ORDER;i++) {
    d1h=extract_h(pci_dp[i]);
    d1l=extract_l(pci_dp[i]);
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp=0x08000;  // for rounding
    Lacc=L_mult_su(gamma994_tab[i],d1l);  // gamma994_tab in Q15
    Lacc=L_add40(L_shl40(Lacc,1),Ltemp);
    pci_dp[i]=(Word32)L_add40(L_shr40(Lacc,16),L_mult(d1h,gamma994_tab[i])); // Q27
  }

  /* What to do with Oldlsp_nq_fx?  Use decoder's.*/
  lpc2lsp_fx(pci_dp, lsp_noise_fx, OldlspD_fx);

  /* Quantize lsp parameters */
  weight_lsp_fx(lsp_noise_fx, lsp_noiseW_fx);
  lspmaq_fx8(lsp_noise_fx, nsub8, nsize8, lsp_noiseW_fx, lspQ_noise_fx, Scratch_fx, lsptab8_fx);

  /* Pack LSP bits */
  Bitpack(Scratch_fx[0],(unsigned short*)RxPkt,4,PktPtr);
  Bitpack(Scratch_fx[1],(unsigned short*)RxPkt,4,PktPtr);

  
  /* Get residual signal */
  for (j=0; j<ORDER; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  Scratch_fx[j]=0; /* Scratch is used as filter memory */
  }

  lsp2lpc_fx(OldlspD_fx, lpci_fx, lpci_fx);

#if 0  
  prediction_filter_fx(lpci_fx,speechBuf_fx,lresidual_fx, Scratch_fx,ORDER, GUARD ,4);
#endif
  
  for (sf=0; sf<NoOfSubFrames; sf++) {
    
    Interpol_fx(OldlspD_fx, lsp_noise_fx, lspi_fx, sf);
    lsp2lpc_fx(lspi_fx, lpci_fx, lpci_fx); //Q12    
    offset= add(shr(extract_l(L_mult(sf,SubFrameSize-1)),1),GUARD);
#ifdef WMOPS_FX
  test();
#endif
    if (sf < 2)
      sfSize = SubFrameSize-1;
    else
      sfSize = SubFrameSize;
    prediction_filter_fx(lpci_fx,speechBuf_fx+offset,lresidual_fx, Scratch_fx,ORDER, SubFrameSize ,4); 
    Interpol_fx(OldlspD_fx, lspQ_noise_fx, lspi_fx, sf);
    /* Convert to lpcs */
    lsp2lpc_fx(lspi_fx, lpci_fx, lpci_fx); //Q12    
    /* Get lpc gain */
    /* Generate impulse response */
    ImpulseRzp_fx(lpci_fx, lpci_fx, lpci_fx, H_noise_fx, Hlength);

    /* Get energy of H */
    tmp40 = 0;
    for (indx=0; indx<sfSize; indx++)
      {
      /* tmp40 in Q23 */
      tmp40 = L_mac40(tmp40, H_noise_fx[indx], H_noise_fx[indx]);
      }
    tmp32 = dsp_sqrt_lut((Word32)L_shr40(tmp40, 15), 0);
    /* tmp32 in Q20 */
    scale_fx = tmp32;

    tmp32 = 0;
    for (indx=0; indx<sfSize; indx++)
      {
      /* tmp32 in Q9*Q0 = Q10 format */
      tmp32 = L_mac(tmp32, 0x0200, abs_s(lresidual_fx[indx]));
      }
    if (L_sub(tmp32, 0x00000400L) < 0)
      {
      tmp32 = 0x00000400L;
      }
    /* log10((2/sfSize)*sum/scale) = log10(sum_fx)-log10(scale_fx)+11*log10(2)-log10(sfSize) */
    tmp32 = log10_lut(tmp32);
    /* Add 11*log10(2) - log10(sfSize) */
    if (sfSize == 54)
      {
      tmp32 = L_add(tmp32, 0x07E509D0L);
      }
    else
      {
      tmp32 = L_add(tmp32, 0x07EF6DE2L);
      }
    tmp32 = L_sub(tmp32, log10_lut(scale_fx));
    /* Divide by 10 and convert to Q13 */
    tmp16 = round32_16(L_shl(tmp32, 2));	/* tmp16 in Q9 */
    tmp32 = L_mult(tmp16, 0x6667);	/* Q9*Q18 = Q28 */
    Sum_fx[sf] = round32_16(L_shl(tmp32, 1));	/* Convert to Q13 */
  }

  /* QUANTIZE AND PACK GAIN */

  maxEnergy_fx = Sum_fx[0];
  for (sf=1; sf<NoOfSubFrames; sf++)
    {
    if (sub(Sum_fx[sf], maxEnergy_fx) > 0)
      {
      maxEnergy_fx = Sum_fx[sf];
      }
    }
//  maxEnergy_fx = (Word32)(maxEnergy*8192.0);	// In Q13 format
  minErr_fx = LW_MAX;
  D=sub(maxEnergy_fx,MIN_LOG_ENERGY_FX); // Q13
  for (i = 0; i < NUM_Q_LEVELS; i++)
    {
    Ltemp=L_mult(D,D);
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub(Ltemp,minErr_fx)<0)
      {
      best=i;
      minErr_fx=Ltemp;
      }
    D=sub(D,LOG_ENERGY_STEP_FX);
    }

  Bitpack(best,(unsigned short*)RxPkt,NUM_EQ_BITS,PktPtr);

  for (i=0; i<PACKWDSNUM; i++)
    {
    outBuf16[i] = RxPkt[i];
    }
  
  }



/*
 * Updates the background noise envelope shape
 */
void update_bbg_estimate(Word16 *buf16, int rate, Word16 walpha_fx)
  {
  register int i;

#if 0
  if (rate == 1)
    {
    /* There seems to be a 4-6dB drop in energy when an eighth rate
     * packet is decoded, reencoded, and then decoded again.
     * Compensate for that before reenconding. */
    for (i=0; i<SPEECH_BUFFER_LEN; i++)
      {
      buf16[i] = 2*buf16[i];
      }
    }
#endif

  update_bgestimate(buf16, rate, walpha_fx);
  update_bgestimate(&buf16[SPEECH_BUFFER_LEN/2], rate, walpha_fx);
  }



void gen_encode_bbg(Word16 *buf16)
  {
  Word16 tmpBuf[SPEECH_BUFFER_LEN];

  gen_bg_fromestimate(tmpBuf);
  gen_bg_fromestimate(&tmpBuf[80]);
  bbg_eighth_rate_encode(tmpBuf, buf16);
  }


void init_bbg()
  {
  unsigned int indx;
  /* Clear out memory for bbg_eighth_rate_encode */
    for (indx=0; indx<GUARD; indx++)
      {
      speechBufHist_fx[indx] = 0;
      }
  }

#endif //MOT_SUPPRESSED_EIGHTH_RATE
