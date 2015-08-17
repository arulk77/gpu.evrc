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
/*  Module:     macro_fx.h -- defining constants                        */
/*----------------------------------------------------------------------*/

#ifndef  _MACRO_FX_H_
#define  _MACRO_FX_H_

#include "typedef_fx.h"
#include <stdio.h>


#define DYNAMIC_COMPLEXITY_REDUC

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
/*======================================================================*/
/* Define constants for general use                                     */
/*======================================================================*/
#define YES 1
#define NO 0
#define ORDER	      10	/* LPC order                             */
#define LPCORDER ORDER
#define LOOKAHEAD_LEN            80
#define SPEECH_BUFFER_LEN        160
#define BITSTREAM_BUFFER_LEN      12       /*  data + 1 word for rate  */
#define FrameSize   160		/* CELP frame size */
#define FSIZE FrameSize
#define NoOfSubFrames 3		/* Number of sub frames in one frame     */
#define SubFrameSize  54
#define GUARD          80	/* Guard buffers for RCELP          */
#define ACBMemSize    128	/* Size of adaptive c.b. memory          */
#define EXTRA         10	/* Extra samples calc. in exc.      */
#define FALSE 0
#define TRUE 1
#define PACKWDSNUM    11	/* Without one word for frame erasure signaling */
#define LPC_ANALYSIS_WINDOW_LENGTH      320
#define     NUM_EQ_BITS     6

/*======================================================================*/
/* Define constants for mode decision                                   */
/*======================================================================*/
//#define USE_VADSNR_INSTEAD_OF_NSSNR
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
#define SNR_THLD 30*512  // Threshold to distinguish between clean and noisy speech, Q9
#else
#define SNR_THLD 25*512
#endif
#define SFNUM 10
#define INACTIVE 0
#define ACTIVE 1
#define SILENCE 1
#define UNVOICED 2
#define VOICED 3
#define TRANSIENT 4
#define DOWN_TRANSIENT 5
#define UP_TRANSIENT 6

typedef struct {
    Word16 mode;
    Word16 curr_voiced;
    Word16 speech_type;
}MODE_FX;

/*======================================================================*/
/* Define constants for NACF at pitch computation                       */
/*======================================================================*/

#define DMIN          20	/* Minimum delay                    */
#define DMAX         120	/* Maximum delay                    */
#define SUBSAMP      2

#define LPFRMEM_LEN 61          /* (DMAX+2)/SUBSAMP                 */
#define D_RANGE 53              /* ((DMAX+2)-(DMIN-4))/SUBSAMP      */ 
#define RLPF_ORDER 12



/*======================================================================*/
/* Define constants factorial packing/unpacking                         */
/*======================================================================*/
#define L_MAXSIZE 55
#define L_VECTOR  54
#define NUM_PULSES 7



/*======================================================================*/
/* Define constants for lsp_vq (all rates)                              */
/*======================================================================*/


#define M3_N_LSP_SEG0_28		2	/* For 28-bit quantizer */
#define M3_N_LSP_SEG1_28		2
#define M3_N_LSP_SEG2_28		3
#define M3_N_LSP_SEG3_28		3

#define   M3_L_LSP_SEG0_28	64
#define   M3_L_LSP_SEG1_28	64
#define   M3_L_LSP_SEG2_28	512
#define   M3_L_LSP_SEG3_28	128


#define   M3_B_LSP_SEG1_28	2
#define   M3_B_LSP_SEG2_28	4
#define   M3_B_LSP_SEG3_28	7

#ifdef DYNAMIC_COMPLEXITY_REDUC

//dont define but use these params as variables

#else //DYNAMIC_COMPLEXITY_REDUC


#define	PRE_LSP_NUM0	4	/* Total # of joint search: 6 + 5 + 4 + 3= 18 times */
#define	PRE_LSP_NUM2	6

#endif //DYNAMIC_COMPLEXITY_REDUC

#define M3_N_LSP_SEG0_22		3	/* For 22-bit quantizer */
#define M3_N_LSP_SEG1_22		3
#define M3_N_LSP_SEG2_22		4

#define   M3_L_LSP_SEG0_22	128
#define   M3_L_LSP_SEG1_22	128
#define   M3_L_LSP_SEG2_22	256


#define   M3_B_LSP_SEG1_22	3
#define   M3_B_LSP_SEG2_22	6


#define M3_N_LSP_SEG0_16		5	/* For 16-bit quantizer */
#define M3_N_LSP_SEG1_16		5

#define   M3_L_LSP_SEG0_16	256
#define   M3_L_LSP_SEG1_16	256

#define   M3_B_LSP_SEG1_16	5


 /* 8-bit quantizer from EVRC */

#define MIN_LSP_SEP   261       /* (64Hz=0.05/(2*PI)*32767 Min. LSP Separation */

#define ALP 16384               /* Weight adjustment factor, ALP=0  no weight
                                   ALP > 0 input is weighted */             

/*======================================================================*/
/* Define constants for HALF-RATE & FULL RATE FCB SEARCH                */
/*======================================================================*/

#define Hlength       SubFrameSize	/* Length of impulse response       */
#define ACBGainSize   8		/* Size of acb scalar gain               */
#define L_CODE    55            /* for dec3_10jcelp_fx.cc                 */

#define L_SUBFR  54
#define L_SUBFR2 55
#define N_PULSES	7	/* Number of unit magnitude pulses to use. */
#define MAX_TLEN L_CODE

/*======================================================================*/
/* Define constants for FULL-RATE FCB SEARCH                            */
/*======================================================================*/
#ifdef DYNAMIC_COMPLEXITY_REDUC
//dont define but use these params as variables

#else //DYNAMIC_COMPLEXITY_REDUC

#define	fcb_N0	4	/* # of candidate positions for pulse 1 */
#define	fcb_N1	8	/* # of candidate positions for pulse 2 */	
#define	fcb_N2	12	/* # of candidate positions for pulse 3 */
#define fcb_N3	16

#define fcb_N_PRE	12 //10	/* # of candidate combinations for pulse 1, 2 and 3 */

#endif//DYNAMIC_COMPLEXITY_REDUC

#define	N_POST	7 //21

/*======================================================================*/
/* Define constants for JCELP_Config_fx                                 */
/*======================================================================*/

#define VCM_UuU_OVERSTEP_FX		2	/* glottal side pulse */
#define VCM_UDU_PIT_FX			66	/* roughly (1 + 1/4) * subframesize */
#define VCM_UD_PIT_FX			95	/* roughly (1 + 3/4) * subframesize */
#define VCM_FAC_FX			590 // 0.9 / (120 - VCM_UD_PIT) in Q14
#define PIT_THRSH_FX			4915     // 0.3 in Q14
#define VCM_L0                          55


/*======================================================================*/
/* Define constants for RCELP                                           */
/*======================================================================*/
#define RSHIFT         3	/* Search boundary                  */

/*======================================================================*/
/* Define constants for voice activity detection (VAD)                  */
/*======================================================================*/
#define FILTERORDER           17
#define FREQBANDS              2
#define FULLRATE_VOICED        4
#define HALFRATE_VOICED        3
#define EIGHTH                 1

//TIME WARPRING SUPOORT FOR VOIP
#define VOIP_DORA
#define VOIP_DORA_VOICE
#define VOIP_DORA_NELP
#define VOIP_DORA_CELP

//#define VOIP_PARSE                /* To Read in Parameter file for decoder  in VOIP Case only*/

#define INC_FACTOR 16876 /* 1.03 in Q14 */
#define SNR_MAP_THRESHOLD 3
#define IS96_INC  16474 /* 1.00547 in Q14 */

#define STATVOICED 5
#define SCALE_DOWN_ENERGY 31785 /* 0.97 in Q15 */

#define TLEVELS 8

#define SMSNR  39322  /* 0.6 in Q16, unsigned  */

#define ADP 8
#define NACF_ADAP_BGN_THR 9830 /* threshold signifying frame does            */
                               /* not have any voiced speech in it, Q15      */
                               /* so we might start to adapt thresholds      */
#define NACF_SOLID_VOICED 16384 /* threshold above which we are pretty sure  */
                               /* speech is present and thus SNRs can be     */
                               /* adjusted accordingly                       */

#define HIGH_THRESH_LIM  5059644*16

#define FULL_THRESH 2 // 1 changed 2/17/00 JPA   /* the number of full rates in a row before */

typedef struct {
    Word16  last_rate; /* rate decisision after 2nd stage select_mode2() */
    Word16  num_full_frames;
    Word16  hangover, hangover_in_progress;
    Word16  band_rate[FREQBANDS];
    Word32  signal_energy[FREQBANDS];
    Word32  frame_energy_sm[FREQBANDS];
    Word32  band_noise_sm[FREQBANDS];
    Word32  band_power[FREQBANDS];
    Word16  adaptcount;
    Word16  pitchrun;
    Word16  snr_stat_once;
    Word16  snr_map[FREQBANDS];
    Word16  frame_num;
} ENCODER_MEM_fx;


/*======================================================================*/
/* Define constants 1/8th rate coder                                    */
/*======================================================================*/
#define MIN_LOG_ENERGY_FX 1638  // 0.2 in Q13
#define MAX_LOG_ENERGY_FX 21299 // 2.6 in Q13
#define LOG_ZERO_ENERGY_FX -16384 // -2.0 in Q13
#define LOG_ENERGY_STEP_FX 307  // (2.6-0.2)/64 in Q13
#define NUM_Q_LEVELS    64


#define SCALE 1

/*======================================================================*/
/* Define constants QPPP LSP_VQ                                         */
/*======================================================================*/
#define LSP_SPREAD_FACTOR_FX 262 //Q15
#define LSP_SPREAD_FACTOR_FX_QPPP 328 //Q15
/*======================================================================*/
/* Define constants for WI functions                                    */
/*======================================================================*/
#define NUM_ERB 22
#define ERB_CBSIZE1 64 
#define ERB_CBSIZE2 64 
#define A_ERB_CBSIZE0 64
#define A_ERB_CBSIZE1 64
#define A_ERB_CBSIZE2 256
#define P_CBSIZE 64
#define A_P_CBSIZE 256
#define NUM_FIXED_BAND 17 /* Num of bands for multiband alignment in FPPP */
#define MAXLAG 120
#define MAXLAG_WI 61 /* Need to allocate more space to handle pitch doubling/tripling*/


struct PACKET {
    short PACKET_RATE; //4-Full;3-Half;2-Quarter;1-Eighth
    short MODE_BIT;//for the bit rate
    short LSP_IDX[4];
    short DELAY_IDX;
    short DELTA_DELAY_IDX;

    short ACBG_IDX[3];
    short FCB_PULSE_IDX[3][4];//3=NoOfSubFrames
    short FCBG_IDX[3];
    short NELP_GAIN_IDX[2][2];
    short NELP_FID;
    short SILENCE_GAIN;
    
    short Q_delta_lag; /*Delta pitch for QPPP*/
    short POWER_IDX;     /*Codebook index for the power quantization for QPPP*/
    short AMP_IDX[2];    /*Codebook index for the Amplitude quantization for QPPP*/
    short F_ROT_IDX; /*Codebook index for the global alignment for FPPP*/
    short A_POWER_IDX;/*Codebook index for the absolute power quantization for FPPP*/
    short A_AMP_IDX[3]; /*Codebook index for the absolute amplitude quantization for FPPP*/
    short F_ALIGN_IDX[17]; /*Codebook index for the alignment shifts for FPPP*/
};

typedef long   int32;
typedef short  int16;

typedef struct {

  char*   input_filename;
  char*   output_filename;
  int16   encode_only;
  int16   decode_only;
  int16   max_rate;             int16  max_rate_default;
  int16   min_rate;             int16  min_rate_default;
  int16   post_filter;          int16  post_filter_default;
  int16   noise_suppression;    int16  noise_suppression_default;
  int16   ibuf_len;
  int16   obuf_len;

  int16   highpass_filter;
  int16   olr_calibration;
 
  int16 unquantized_lsp;
  int16 partial_file_processing;
  int16 starting_frame_num;
  int16 num_frames;
  int16 form_res_out;
  int16 qform_res_out;
  int16 mform_res_out;
  int16 target_speech_out;
  int16 rate_out;
  int16 lag_out;
  int16 nacf_out;
  int16 unquantized_prototype;
  int16 unquantized_zero_rate;
  int16 unquantized_quarter_rate;
  int16 unquantized_half_rate;
  int16 unquantized_full_rate;
  FILE *rfileP;
  char* rate_filename;
  int16 packet_out;
  int16 vad_out;
  int16 forced_count;
  int16 forced_rate[10];
  int16 forced_frame[10];
  int16 accshift_out;
  int16 avg_rate_control;
  float PPP_to_CELP_threshold;
  int16 avg_rate_target;
  int16 ratewin;
  int operating_point;
  char pattern[10];
  FILE* erasure_file;
  FILE* signalling_file;
  int16 verbose;
#ifdef VOIP_DORA
  //Added for Support of Phase Matching/Warping
  int16 phase_matching;
  int16 double_erasures_pm;
  int16 time_warping;
  
#ifdef VOIP_PARSE
  char*   dec_parfile;
#endif

//End:Added for Support of Phase Matching/Warping

#endif
  int dtx;

  FILE* tar_adr;
  
} EvrcArgs;


/*======================================================================*/
/* Define constants for TTY functions                                    */
/*======================================================================*/
#define  DATA_SIZE          5    /* number of 16-bit data words*/
#define  TTY_TYPE_Baudot    1    /* indicates the type of TTY (1=Baudot)*/
#define  TTY_HEADER         2    /* header to indicate data */
#define DTMF_HEADER         3    

/*======================================================================*/
/* Define constants for update_average_rate                             */
/*======================================================================*/

#define CONST1 (Word16) 6000 //Eargs->ratewin*10
#define CONST2 (Word16) ((2-0.02)*CONST1) 
#define CONST3 (Word32) (7.5*CONST1*1000*2)
#define CONST4 (Word32) 238609 //1/(10*Eargs->ratewin*(9.0-7.5)) in Q31
#define CONST5 (Word32) (6.6*CONST1*1000*2)
#define CONST6 (Word32) 397682 //1/(10*Eargs->ratewin*(7.5-6.6)) in Q31
#define CONST7 (Word32) (5.75*CONST1*1000*2)
#define CONST8 (Word32) 421075 //1/(10*Eargs->ratewin*(6.6-5.75)) in Q31

//lsp_spreading function
#define LSP_SPRD

#endif
 




