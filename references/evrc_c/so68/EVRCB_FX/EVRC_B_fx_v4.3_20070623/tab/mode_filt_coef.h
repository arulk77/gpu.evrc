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
/*  mode_filt_coef.h  -- mode decision filter coefficients              */
/*----------------------------------------------------------------------*/
#if 0
static Word16 lpf_num_coef_fx[13]={212, 1270, 4211, 9674, 16848, 23195,
    25753, 23195, 16848, 9674, 4211, 1270, 212};
static Word16 lpf_den_coef_fx[13]={8192, 9709, 23417, 22785, 26209,
    19636, 13973, 7558, 3473, 1228, 329, 59, 5};
static Word16 hpf_num_coef_fx[13]={212, -1270, 4211, -9674, 16848,
    -23195, 25753, -23195, 16848, -9674, 4211, -1270, 212};
static Word16 hpf_den_coef_fx[13]={8192, -9709, 23417, -22785, 26209,
    -19636, 13973, -7558, 3473, -1228, 329, -59, 5};
#else
static Word16 lpf_num_coef_fx[13]={106, 635, 2105, 4837, 8424, 11598,
    12876, 11598, 8424, 4837, 2105, 635, 106};
static Word16 lpf_den_coef_fx[13]={4096, 4854, 11708, 11392, 13104, 9818,
    6987, 3779, 1736, 614, 165, 30, 3};
static Word16 hpf_num_coef_fx[13]={106, -635, 2105, -4837, 8424, -11598,
    12876, -11598, 8424, -4837, 2105, -635, 106};
static Word16 hpf_den_coef_fx[13]={4096, -4854, 11708, -11392, 13104, -9818,
    6987, -3779, 1736, -614, 165, -30, 3};
    
#endif
