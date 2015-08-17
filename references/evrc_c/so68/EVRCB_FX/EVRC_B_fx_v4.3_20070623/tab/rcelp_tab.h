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
/*  Tables for RCELP and ACB related functions                          */
/*======================================================================*/
static Word16 quad_int_tab1[8]={12288,8448,5120,2304,0,-1792,-3072,-3840}; //Q15
static Word16 quad_int_tab2[8]={24576,28160,30720,32256,32767,32256,30720,28160}; // Q15
static Word16 quad_int_tab3[8]={-4096,-3840,-3072,-1792,0,2304,5120,8448}; // Q15
static Word16 delay_int_tab[5]={0x8000, 0x5598, 0x2B33, 0x0000, 0x0000};
                                // Q15 unsigned for delay interpolation
static Word32 inv_sflen_tab[2]={0x4D4873ED, 0x4BDA12F7};
                                // 1/53, 1/54 in Q36
static Word16 intrp_tab3[56]={  // interpolation table, 7 point, Q15
0xfd15,
0x0c56,
0x36ff,
0x36ff,
0x0c56,
0xfd15,
0xff57,
0xfd3b,
0x083d,
0x3262,
0x3ace,
0x110c,
0xfd30,
0xff2b,
0xfd88,
0x04d0,
0x2d27,
0x3da6,
0x1646,
0xfda4,
0xfef1,
0xfde8,
0x0212,
0x2784,
0x3f68,
0x1be0,
0xfe8c,
0xfea6,
0xfe4b,
0x0000,
0x21b0,
0x4000,
0x21b0,
0x0000,
0xfe4b,
0xfea6,
0xfe8c,
0x1be0,
0x3f68,
0x2784,
0x0212,
0xfde8,
0xfef1,
0xfda4,
0x1646,
0x3da6,
0x2d27,
0x04d0,
0xfd88,
0xff2b,
0xfd30,
0x110c,
0x3ace,
0x3262,
0x083d,
0xfd3b};

static Word16 intrp_tab8[136]={  // interpolation table, 17 point, Q15
0x006d,
0xff6e,
0x0063,
0x00b4,
0xfc7b,
0x096b,
0xe977,
0x4fdb,
0x4fdb,
0xe977,
0x096b,
0xfc7b,
0x00b4,
0x0063,
0xff6e,
0x006d,
0xffa9,
0x003b,
0xffde,
0xff8e,
0x020d,
0xfaad,
0x0b47,
0xe91b,
0x3f28,
0x5e61,
0xed0e,
0x05fd,
0xff07,
0xff0f,
0x0150,
0xfefe,
0x0099,
0xff9c,
0x000a,
0x003e,
0xfee6,
0x02f9,
0xf9bb,
0x0b8d,
0xeb77,
0x2d82,
0x69a3,
0xf421,
0x0135,
0x0211,
0xfd4c,
0x0235,
0xfe9d,
0x00b8,
0xff9b,
0xffdf,
0x0087,
0xfe7a,
0x0369,
0xf9a9,
0x0a6b,
0xefd9,
0x1c24,
0x70c3,
0xfea2,
0xfb7f,
0x0542,
0xfba6,
0x02f0,
0xfe5d,
0x00c3,
0xffa6,
0xffbd,
0x00b3,
0xfe4e,
0x0360,
0xfa5c,
0x0830,
0xf577,
0x0c33,
0x7333,
0x0c33,
0xf577,
0x0830,
0xfa5c,
0x0360,
0xfe4e,
0x00b3,
0xffbd,
0xffa6,
0x00c3,
0xfe5d,
0x02f0,
0xfba6,
0x0542,
0xfb7f,
0xfea2,
0x70c3,
0x1c24,
0xefd9,
0x0a6b,
0xf9a9,
0x0369,
0xfe7a,
0x0087,
0xffdf,
0xff9b,
0x00b8,
0xfe9d,
0x0235,
0xfd4c,
0x0211,
0x0135,
0xf421,
0x69a3,
0x2d82,
0xeb77,
0x0b8d,
0xf9bb,
0x02f9,
0xfee6,
0x003e,
0x000a,
0xff9c,
0x0099,
0xfefe,
0x0150,
0xff0f,
0xff07,
0x05fd,
0xed0e,
0x5e61,
0x3f28,
0xe91b,
0x0b47,
0xfaad,
0x020d,
0xff8e,
0xffde,
0x003b};
