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
/*======================================================================*/

/*===================================================================*/
/* PROTOTYPE FILE : lib_wmp_fx.h                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE: Calculate the WMOPS for the floating point code.         */
/*===================================================================*/

/*----------------------------------------------------------------------------*/
/*-------------------------------- FUNCTIONS ---------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef WMOPS_FX

#define NB_FUNC_MAX 1024

typedef struct
{
   Word32 add;       /* Complexity Weight of 1 */
   Word32 sub;
   Word32 abs_s;
   Word32 shl;
   Word32 shr;
   Word32 extract_h;
   Word32 extract_l;
   Word32 mult;
   Word32 L_mult;
   Word32 negate;
   Word32 round;
   Word32 L_mac;
   Word32 L_msu;
   Word32 L_add;      /* Complexity Weight of 2 */
   Word32 L_sub;
   Word32 L_add_c;
   Word32 L_sub_c;
   Word32 L_negate;
   Word32 L_shl;
   Word32 L_shr;
   Word32 mult_r;
   Word32 shr_r;
   Word32 shift_r;
   Word32 mac_r;
   Word32 msu_r;
   Word32 L_deposit_h;
   Word32 L_deposit_l;
   Word32 L_shr_r;    /* Complexity Weight of 3 */
   Word32 L_shift_r;
   Word32 L_abs;
   Word32 norm_s;     /* Complexity Weight of 15 */
   Word32 divide_s;   /* Complexity Weight of 18 */
   Word32 norm_l;     /* Complexity Weight of 30 */
   Word32 DataMove16; /* Complexity Weight of 1 */
   Word32 DataMove32; /* Complexity Weight of 2 */
   Word32 Logic16;    /* Complexity Weight of 1 */
   Word32 Logic32;    /* Complexity Weight of 2 */
   Word32 Test;       /* Complexity Weight of 2 */
   Word32 clearOverflow;/* Complexity Weight of 1 */
   Word32 L_saturate; /* Complexity Weight of 4 */ 	   
   Word32 L_add40;
   Word32 L_sub40;
   Word32 L_mac40;
   Word32 L_msu40;
   Word32 L_shl40;
   Word32 L_shr40;
   Word32 norm32_l40;
   Word32 L_sat32_40;
   Word32 extract_l40;   /* Complexity Weight of 1 */
   Word32 L_deposit_l40; 
   Word32 L_mpy_ll;      /* Complexity Weight of 6 */
   Word32 L_mpy_ls;      /* Complexity Weight of 3 */
} BASIC_OP_FX;

extern BASIC_OP_FX counter_fx;
extern BASIC_OP_FX op_weight_fx;

extern char   wc_fn_fx [NB_FUNC_MAX][255];
extern Word32 glob_wc_fx, wc_fx[NB_FUNC_MAX], fnc_comp_fx [NB_FUNC_MAX];
extern Word32 wc_frame_fx;
extern Word32 nbframe_fx;
extern Word32 LastWOper_fx;
extern Word40 total_wmops_fx;

void	WMP_init_lib_fx	(void);

void    move16  (void);
void    move32  (void);
void    logic16 (void);
void    logic32 (void);
void    test    (void);
void    Nmove16  (Word16);
void    Nmove32  (Word16);
void    Nlogic16 (Word16);
void    Nlogic32 (Word16);
void    Ntest    (Word16);

Word32	WMP_calcul_total_WMOPS_fx	(void);
void	WMP_reset_counter_WMOPS_fx	(void);
Word32	WMP_calcul_delta_WMOPS_fx	(void);

Word16	WMP_hash_fx (char []);
Word32	WMP_fwc_fx  (char []);

Word40	WMP_calcul_stat_fx (Word32);

void 	WMP_swap_fx  (Word32 [], char [NB_FUNC_MAX][255], Word16, Word16);
void	WMP_qsort_fx (Word32 [], char [NB_FUNC_MAX][255], Word16, Word16);

void	WMP_print_stat_fx	(void);

#endif

/*============================================================================*/
/*------------------------------------- END ----------------------------------*/
/*============================================================================*/
