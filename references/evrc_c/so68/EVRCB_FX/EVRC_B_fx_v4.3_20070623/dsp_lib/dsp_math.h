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
/* This file contains Qualcomm math library functions as implemented    */
/* in DSP. It can be considered an extension to the fixed point library */
/* functions.                                                           */
/*----------------------------------------------------------------------*/

#ifndef _dspmath_h
#define _dspmath_h

#include "typedef_fx.h"

#define MAX_FX(x,y) (((x)>(y))?(x):(y))
#define MIN_FX(x,y) (((x)<(y))?(x):(y))

Word40 L_negate40(Word40 L_var1);
/* Basic op extension for unsigned multiplication   */
/* Note that there is no left-shift by 1            */
Word40 L_mult_su(Word16 var1, UNS_Word16 var2);  
Word40 L_mac40_su(Word40 L_var3, Word16 var1, UNS_Word16 var2);
Word40 L_mult_uu(UNS_Word16 var1, UNS_Word16 var2);
Word40 L_mult_ll(Word32 var1, Word32 var2);
Word40 L_add40_40(Word40 L_var1, Word40 L_var2);
Word40 L_sub40_40(Word40 L_var1, Word40 L_var2);

/* Specialty DSP functions                          */
Word32 dsp_sqrt_lut(Word32 Linput, Word32 Lround);
Word40 divide_dp(Word40 Lnom, Word40 Ldenom, Word16 Qadj);
Word32 sqrt_divide_dp(Word40 x, Word40 y, Word16 Qdif, Word16 *Qout);
Word32 log10_lut(Word40 Linput);
Word40 log2_lut(Word40 Linput);
Word32 invert_lut(Word32 Linput, Word16 *Qout);
Word32 invert_dp(Word40 Linput, Word16 n, Word16 *Qout);
Word40 dsp_pow10(Word32 x);
Word16 div_int_sp(Word16 n, Word16 m, Word16 &r);
Word32 div_int_dp(Word32 n, Word32 m, Word32 &r);

#endif
