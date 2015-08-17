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
/*  globs_fx.h -- Extern global variable definition                     */
/*======================================================================*/

#ifndef  _GLOBS_FX_H_
#define  _GLOBS_FX_H_


#include "typedef_fx.h"
#include "macro_fx.h"
#include "proto_fx.h"
#include "SmartBlanking.h"

/*======================================================================*/
/* General purpose variables                                            */
/*======================================================================*/
extern Word16 lsp_fx[ORDER];	  /* quantized lsp vector  (Q15) */
extern Word16 lspi_fx[ORDER];	  /* Interpolation of correlation coeff      */
extern Word16 pci_fx[ORDER]; /* Interpolated prediction coefficients, Q12   */
extern Word16 prev_pci_fx[ORDER];

extern Word16 Scratch_fx[SubFrameSize + 6];
extern Word16 SScratch_fx[6];		/* scratch short memory */
extern Word16   FCBGainSize;   /* Current Fixed Codebook Gain Size */

extern Word16  bit_rate; /* Current coding scheme for encoder/decoder */
extern Word16  PACKET_RATE;       /* Current bitrate for encoder/decoder */
extern Word16  MODE_BIT;   /* Used to distinguish between CELP(0) and FPPP(1),   NELP(0) and QPPP(1) */
extern Word16	PktPtr[2];
extern struct PACKET data_packet;

/*======================================================================*/
/* Encoder variables                                                    */
/*======================================================================*/
extern Word16 stoporder15, stoporder30;
extern Word16 beta_fx, beta1_fx;  // periodicity, Q15
extern Word16 delay_fx, delay1_fx; // open loop pitch, Q0
extern Word16 pdelay_fx;

extern Word16   idxppg;	/* Pitch gain c.b. - selected gain index   */
extern Word16   idxcb;	/* Shape c.b. - selected shape index       */
extern Word16   idxcbg;		/* FCB gain c.b. - selected gain index     */

extern Word16 residual_fx[GUARD + FrameSize + LOOKAHEAD_LEN];
extern Word16 lsp_nq_fx[ORDER];  /* Un-quantized Correlation coefficients (Q15) */
extern Word16 wpci_gma1_fx[ORDER];  /* LPC coefficients weighted with gamma1 (0.9), Q12 */
extern Word16 wpci_gma2_fx[ORDER];  /* LPC coefficients weighted with gamma2 (0.5), Q12 */
extern Word16 lspi_nq_fx[ORDER];  /* Interpolation of correlation coeff, Q15 */

extern Word16 pci_nq_fx[ORDER];  /* unquantized LPC coefficients, Q12  */
extern Word16 HPspeech_fx[GUARD+FrameSize+LOOKAHEAD_LEN];
extern Word16 ConstHPspeech_fx[GUARD];	/* temporary buffer to store HPspech  */

extern Word16 H_fx[Hlength+1];    /* Impulse response [Hlength] (Q11) */
extern Word16 TARGETw_fx[SubFrameSize+1];
extern Word16 fcbGain_fx;            /* ACELP fixed codebook gain,Q0 */

extern Word16 OldlspE_fx[ORDER];  /* Last frame quantized lsp, Q15*/

extern Word16 Oldlsp_nq_fx[ORDER];  /* Last frame un-quantized lsp, Q15 */

extern Word16 OldOldlspE_fx[ORDER];

extern Word16 origm_fx[SubFrameSize];
extern Word16  *worigm_fx;	/* shared weighted original memory */

extern Word32  SynMemoryM_fx[ORDER];	/* weighted speech synthesis filter memory */

extern Word16   WFmemFIR_fx[ORDER];   /* Weighting filter memory            */
extern Word32   WFmemIIR_fx[ORDER];   /* Weighting filter memory            */

extern Word16 FIRmem_fx[ORDER]; /* prediction filter memory */

extern Word16 wspeech_fx[FrameSize]; /* RCELP output, modified weighted speech */
extern Word16 mspeech_fx[FrameSize]; /* Modified speech  */

extern Word16 bufferm_fx[ACBMemSize+SubFrameSize+EXTRA]; // Modified residual target
extern Word16 accshift_fx; // Accumulated shift, Q8
extern Word16 residualm_fx[SubFrameSize + EXTRA];  // modified residual for one subframe
extern Word16 Excitation_fx[ACBMemSize+SubFrameSize+EXTRA];

extern Word16 zir_fx[SubFrameSize]; /* Zero Input Response (can share memory w/ HtH)*/
extern Word16 wpci_fx[ORDER];
extern Word16 TARGET_fx[SubFrameSize+1];
extern Word16 fcbIndexVector_fx[10];     /* ACELP fixed codebook index vector */
extern Word16 *gnvq_fx;
extern Word16 *ExconvH_fx;

extern Word16 cbprev_E_fx[ORDER];
extern Word16 cbprevprev_E_fx[ORDER];

extern Word16 PPP_BUMPUP; //should be changed to Word16 later
extern Word16 pppcountE;
extern Word16 rcelp_half_rateE, prev_dim_and_burstE, dim_and_burstE;


extern Word16 PPP_MODE_E_fx; // 0=QPPP, 1=FPPP
extern Word16   lastLgainE_fx;    /* Q11, Previous gain value for the low band*/
extern Word16   lastHgainE_fx;    /* Q11, Previous gain value for the high band*/
extern Word16   lasterbE_fx[NUM_ERB];  /* Q13, Previous Amplitude spectrum (ERB)*/

#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
extern Word16 VAD_SNR_fx;
#else
extern Word16 NS_SNR_fx; //Q9
#endif

extern Word16     LAST_PACKET_RATE_E;
extern Word16     LAST_MODE_BIT_E;

extern Word16   shiftSTATE;
extern Word16   lastrateE;		/* last bitrate used for encoder*/
extern Word16   dpm;
extern Word16	TxPkt[PACKWDSNUM];

extern long    encode_fcnt;	/* Frame counter */

/*======================================================================*/
/* Decoder variables                                                    */
/*======================================================================*/
#ifdef VOIP_DORA
extern Word16 DECspeech_fx[SubFrameSize*2];/* Output decoder speech           */
extern Word16 DECspeechPF_fx[SubFrameSize*2];/* Output decoder speech with post filter */
extern Word16 PitchMemoryD_fx[ACBMemSize + SubFrameSize*2+ EXTRA];
#else
extern Word16 DECspeech_fx[SubFrameSize];/* Output decoder speech           */
extern Word16 DECspeechPF_fx[SubFrameSize];/* Output decoder speech with post filter */
extern Word16 PitchMemoryD_fx[ACBMemSize + SubFrameSize+ EXTRA];
#endif

extern Word16 OldlspD_fx[ORDER];		/* Last frame quantized lsp, Q15*/
extern Word32 SynMemory_fx[ORDER];	/* Synthesis filter's memory             */

extern Word16 ave_fcb_gain_fx; //Q0
extern Word16 ave_acb_gain_fx; //Q14

extern Word16 pdelayD_fx;

extern Word16 FadeScale_fx; //Q14

/* Memory needed for the moving average LPC Quantization */
extern Word16 cbprev_D_fx[ORDER];
extern Word16 cbprevprev_D_fx[ORDER];

extern Word16 pdelayD2_fx, pdelayD3_fx;
extern Word16 ave_acb_gain_back_fx;
extern Word16 PitchMemoryD2_fx[ACBMemSize], PitchMemoryD3_fx[ACBMemSize];

extern Word16 last_valid_rate;   /* last valid decoder rate of operation */
extern Word16 erasureFlag;
extern Word16   errorFlag;
extern Word16   fer_counter;
extern Word16   lastrateD;
extern Word16 RxPkt[PACKWDSNUM];

extern Word16   lastLgainD_fx;    /* Q11, Previous gain value for the low band*/
extern Word16   lastHgainD_fx;    /* Q11, Previous gain value for the high band*/
extern Word16   lasterbD_fx[NUM_ERB];  /* Q13, Previous Amplitude spectrum (ERB)*/

extern Word16 PPP_MODE_D_fx, LAST_PPP_MODE_D_fx; // 0=QPPP, 1=FPPP,  2=BUMP -UP 
extern Word16 pdeltaD_fx;  // Delta delay for QPPP
extern Word16 OldOldlspD_fx[ORDER]; /* Last last frame quantized lsp  */

extern Word16     LAST_PACKET_RATE_D;
extern Word16     LAST_MODE_BIT_D;
extern Word16   prev_frame_error;

extern Word16 rcelp_half_rateD;
extern Word16 prev_rcelp_half;

extern long    decode_fcnt;	/* Frame counter */

extern Word16 zrbit[2];

extern Word16 BAD_RATE;

extern Word16 data_buf[DATA_SIZE+5];

extern Word16 SPL_HCELP;

extern Word16 SPL_HPPP;

extern Word16 SPL_HNELP;

extern Word16 N_consec_ers;

extern Word16 prev_prev_frame_error;

/* WI_fx.cc globals */
extern Word16 C_fx[481], S_fx[481]; 

/* main.cc globals */
extern EvrcArgs*  Eargs;

/* voiced.cc globals */
extern Word16 ph_offset_E_fx; 
// PPP Specific Stuff
extern DTFS_fx currp_nq_fx, prev_cw_E_fx ;

#ifdef DYNAMIC_COMPLEXITY_REDUC
//dynamic complexity reduction
//dynamic complexity reduction
extern Word16 fcb_N0;	/* # of candidate positions for pulse 1 */
extern Word16 fcb_N1;	/* # of candidate positions for pulse 2 */	
extern Word16 fcb_N2;	/* # of candidate positions for pulse 3 */
extern Word16 fcb_N3;

extern Word16 fcb_N_PRE; //10	/* # of candidate combinations for pulse 1, 2 and 3 */

extern Word16 PRE_LSP_NUM0;
extern Word16 PRE_LSP_NUM2;
#endif//DYNAMIC_COMPLEXITY_REDUC


#ifdef VOIP_DORA
extern Word16	go_back_input;
#endif

/* smart blanking and LSP jittering related */
extern SBEncoder SB_enc;
extern SBDecoder SB_dec; 
#define DTX
//#define GAIN_PRINT


/* lsp jitter, lsp interp related */
extern Word16 lsp_arry[8][10];
extern short ER_count;
extern short SID;
extern short SILFrame;
extern short prev_SILFrame;
extern short last_last_last_rateD;
extern short last_last_rateD;
extern short INTERP;
extern short N_inter;
extern Word16 final_lsp[ORDER];
extern Word16 init_lsp[ORDER];

/*gain smoothing related */
extern Word16 init_gain;
extern Word16 final_gain;
extern short NG_inter;
extern short G_INTERP;
extern short noSID;
extern short lpcgflag;

#define FCB_ATTN_FLAG
extern Word16 ones_dec_cnt;


extern Word32 lpcg;
extern Word16 lastlastrateE;

extern Word16 LAST_LAST_MODE_BIT_E;

extern Word16 TONE_DETECTED;


#endif
