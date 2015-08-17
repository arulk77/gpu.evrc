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
/*  globs_fx.cc -- Global variable definition                           */
/*======================================================================*/

#include "typedef_fx.h"
#include "macro_fx.h"
#include "proto_fx.h"
#include "SmartBlanking.h"

/*======================================================================*/
/* General purpose variables                                            */
/*======================================================================*/
Word16 lsp_fx[ORDER];	  /* quantized lsp vector  (Q15) */
Word16 lspi_fx[ORDER];	  /* Interpolation of correlation coeff      */
Word16 pci_fx[ORDER]; /* Interpolated prediction coefficients, Q12   */
Word16 prev_pci_fx[ORDER];

Word16 Scratch_fx[SubFrameSize + 6];
Word16 SScratch_fx[6];		/* scratch short memory */
Word16   FCBGainSize;   /* Current Fixed Codebook Gain Size */

Word16     bit_rate;		/* Current coding scheme for encoder/decoder */
Word16     PACKET_RATE;            /* Current bitrate for encoder/decoder */
Word16     MODE_BIT;               /* Used to distinguish between CELP(0) and FPPP(1),   NELP(0) and QPPP(1) */
Word16	PktPtr[2];
struct PACKET data_packet;

/*======================================================================*/
/* Encoder variables                                                    */
/*======================================================================*/
Word16 stoporder15, stoporder30;
Word16 beta_fx, beta1_fx;  // periodicity, Q15
Word16 delay_fx, delay1_fx; // open loop pitch, Q0
Word16 pdelay_fx;

Word16   idxppg;	/* Pitch gain c.b. - selected gain index   */
Word16   idxcb;			/* Shape c.b. - selected shape index       */
Word16   idxcbg;		/* FCB gain c.b. - selected gain index     */

Word16 residual_fx[GUARD + FrameSize + LOOKAHEAD_LEN];
Word16 lsp_nq_fx[ORDER];  /* Un-quantized Correlation coefficients (Q15) */
Word16 wpci_gma1_fx[ORDER];  /* LPC coefficients weighted with gamma1 (0.9), Q12 */
Word16 wpci_gma2_fx[ORDER];  /* LPC coefficients weighted with gamma2 (0.5), Q12 */
Word16 lspi_nq_fx[ORDER];  /* Interpolation of correlation coeff, Q15      */

Word16 pci_nq_fx[ORDER];  /* unquantized LPC coefficients, Q12  */
Word16 HPspeech_fx[GUARD+FrameSize+LOOKAHEAD_LEN];
Word16 ConstHPspeech_fx[GUARD];	/* temporary buffer to store HPspech  */

Word16 H_fx[Hlength+1];        	/* Impulse response [Hlength] (Q11) */
Word16 TARGETw_fx[SubFrameSize+1];
Word16 fcbGain_fx;                /* ACELP fixed codebook gain,Q0 */

Word16 OldlspE_fx[ORDER];		/* Last frame quantized lsp, Q15*/

Word16 Oldlsp_nq_fx[ORDER];        /* Last frame un-quantized lsp, Q15 */

Word16 OldOldlspE_fx[ORDER];

Word16 origm_fx[SubFrameSize];
Word16  *worigm_fx = origm_fx;		/* shared weighted original memory */

Word32  SynMemoryM_fx[ORDER];	/* weighted speech synthesis filter memory */

Word16   WFmemFIR_fx[ORDER];   /* Weighting filter memory                 */
Word32   WFmemIIR_fx[ORDER];   /* Weighting filter memory                 */

Word16 FIRmem_fx[ORDER]; /* prediction filter memory */

Word16 wspeech_fx[FrameSize]; /* RCELP output, modified weighted speech */
Word16 mspeech_fx[FrameSize]; /* Modified speech  */

Word16 bufferm_fx[ACBMemSize+SubFrameSize+EXTRA]; // Modified residual target
Word16 accshift_fx; // Accumulated shift, Q8
Word16 residualm_fx[SubFrameSize + EXTRA];  // modified residual for one subframe
Word16 Excitation_fx[ACBMemSize+SubFrameSize+EXTRA];

Word16 zir_fx[SubFrameSize]; /* Zero Input Response (can share memory w/ HtH)*/
Word16 wpci_fx[ORDER];
Word16 TARGET_fx[SubFrameSize+1];
Word16 fcbIndexVector_fx[10];     /* ACELP fixed codebook index vector */
Word16 *gnvq_fx;
Word16 *ExconvH_fx=Scratch_fx;

Word16 cbprev_E_fx[ORDER];
Word16 cbprevprev_E_fx[ORDER];

Word16 PPP_BUMPUP; //should be changed to Word16 later
Word16 pppcountE;
Word16 rcelp_half_rateE, prev_dim_and_burstE, dim_and_burstE;


Word16 PPP_MODE_E_fx; // 0=QPPP, 1=FPPP
Word16   lastLgainE_fx;    /* Q11, Previous gain value for the low band*/
Word16   lastHgainE_fx;    /* Q11, Previous gain value for the high band*/
Word16   lasterbE_fx[NUM_ERB];  /* Q13, Previous Amplitude spectrum (ERB)*/

#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
Word16 VAD_SNR_fx; //Q9
#else
Word16 NS_SNR_fx; //Q9
#endif

Word16     LAST_PACKET_RATE_E;
Word16     LAST_MODE_BIT_E;

Word16   shiftSTATE;
Word16   lastrateE;		/* last bitrate used for encoder*/
Word16   dpm;
Word16	TxPkt[PACKWDSNUM];

long    encode_fcnt;		/* Frame counter */

/*======================================================================*/
/* Decoder variables                                                    */
/*======================================================================*/
#ifdef VOIP_DORA
Word16 DECspeech_fx[SubFrameSize*2];/* Output decoder speech           */
Word16 DECspeechPF_fx[SubFrameSize*2];/* Output decoder speech with post filter */
Word16 PitchMemoryD_fx[ACBMemSize + SubFrameSize *2 + EXTRA];
#else
Word16 DECspeech_fx[SubFrameSize];/* Output decoder speech           */
Word16 DECspeechPF_fx[SubFrameSize];/* Output decoder speech with post filter */
Word16 PitchMemoryD_fx[ACBMemSize + SubFrameSize  + EXTRA];
#endif

Word16 OldlspD_fx[ORDER];		/* Last frame quantized lsp, Q15*/
Word32 SynMemory_fx[ORDER];	/* Synthesis filter's memory             */

Word16 ave_fcb_gain_fx; //Q0
Word16 ave_acb_gain_fx; //Q14

Word16 pdelayD_fx;

Word16 FadeScale_fx; //Q14

/* Memory needed for the moving average LPC Quantization */
Word16 cbprev_D_fx[ORDER];
Word16 cbprevprev_D_fx[ORDER];

Word16 pdelayD2_fx, pdelayD3_fx;
Word16 ave_acb_gain_back_fx;
Word16 PitchMemoryD2_fx[ACBMemSize], PitchMemoryD3_fx[ACBMemSize];

Word16 last_valid_rate;   /* last valid decoder rate of operation */
Word16 erasureFlag;
Word16   errorFlag;
Word16   fer_counter;
Word16   lastrateD;
Word16 RxPkt[PACKWDSNUM];

Word16   lastLgainD_fx;    /* Q11, Previous gain value for the low band*/
Word16   lastHgainD_fx;    /* Q11, Previous gain value for the high band*/
Word16   lasterbD_fx[NUM_ERB];  /* Q13, Previous Amplitude spectrum (ERB)*/


Word16 PPP_MODE_D_fx, LAST_PPP_MODE_D_fx; // 0=QPPP, 1=FPPP,  2=BUMP -UP 
Word16 pdeltaD_fx;  // Delta delay for QPPP
Word16 OldOldlspD_fx[ORDER]; /* Last last frame quantized lsp  */

Word16     LAST_PACKET_RATE_D;
Word16     LAST_MODE_BIT_D;
Word16   prev_frame_error;

Word16 rcelp_half_rateD;
Word16 prev_rcelp_half;

long    decode_fcnt;		/* Frame counter */

#ifdef VOIP_DORA
Word16		go_back_input = 0;
#endif

/*==================================================*/
/*      BAD-RATE CHECK variables                    */
/*==================================================*/
Word16 zrbit[2];

Word16 BAD_RATE;

/*==================================================*/
/*      TTY variables                               */
/*==================================================*/

Word16 data_buf[DATA_SIZE+5];
Word16 SPL_HCELP;
Word16 SPL_HPPP;
Word16 SPL_HNELP;
Word16 N_consec_ers;

Word16 prev_prev_frame_error;

/* WI_fx.cc globals */
Word16 C_fx[481], S_fx[481];

/* main.cc globals */
EvrcArgs*  Eargs;

/* voiced.cc globals */
Word16 ph_offset_E_fx; 

// PPP Specific Stuff
DTFS_fx currp_nq_fx;
DTFS_fx prev_cw_E_fx;

#ifdef DYNAMIC_COMPLEXITY_REDUC
//dynamic complexity reduction
Word16 fcb_N0;	/* # of candidate positions for pulse 1 */
Word16 fcb_N1;	/* # of candidate positions for pulse 2 */	
Word16 fcb_N2;	/* # of candidate positions for pulse 3 */
Word16 fcb_N3;
Word16 fcb_N_PRE; //10	/* # of candidate combinations for pulse 1, 2 and 3 */

Word16 PRE_LSP_NUM0;
Word16 PRE_LSP_NUM2;
#endif//DYNAMIC_COMPLEXITY_REDUC
/* smart blanking related */
SBEncoder SB_enc;
SBDecoder SB_dec; 


/* lsp jitter, lsp interp related */
Word16 lsp_arry[8][10];
short ER_count;
short SID;
short SILFrame;
short prev_SILFrame;
short last_last_last_rateD;
short last_last_rateD;
short INTERP;
short N_inter;
Word16 final_lsp[ORDER];
Word16 init_lsp[ORDER];

/*gain smoothing related */
Word16 init_gain;
Word16 final_gain;
short NG_inter;
short G_INTERP;
short noSID;
short lpcgflag;
Word16 ones_dec_cnt;
Word32 lpcg;
Word16 lastlastrateE;
Word16 LAST_LAST_MODE_BIT_E;
Word16 TONE_DETECTED;



