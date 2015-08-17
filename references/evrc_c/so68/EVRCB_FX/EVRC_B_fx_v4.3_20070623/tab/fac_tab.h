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
/*  fac_tab.h  -- factorial look-up table                               */
/*----------------------------------------------------------------------*/
static Word32 fact_table[245]={
0x0014,   0x0000,   0x0000,   0x0000,   0x0000,
0x0023,   0x0000,   0x0000,   0x0000,   0x0000,   
0x0038,   0x0046,   0x0000,   0x0000,   0x0000,    
0x0054,   0x007e,   0x0000,   0x0000,   0x0000,
0x0078,   0x00d2,   0x00fc,   0x0000,   0x0000,   
0x00a5,   0x014a,   0x01ce,   0x0000,   0x0000,   
0x00dc,   0x01ef,   0x0318,   0x039c,   0x0000,   
0x011e,   0x02cb,   0x0507,   0x06b4,   0x0000,   
0x016c,   0x03e9,   0x07d2,   0x0bbb,   0x0d68,   
0x01c7,   0x0555,   0x0bbb,   0x138d,   0x1923,   
0x0230,   0x071c,   0x1110,   0x1f48,   0x2cb0,   
0x02a8,   0x094c,   0x182c,   0x3058,   0x4bf8,   
0x0330,   0x0bf4,   0x2178,   0x4884,   0x7c50,   
0x03c9,   0x0f24,   0x2d6c,   0x69fc,   0xc4d4,    
0x0474,   0x012ed,   0x03c90,   0x09768,   0x012ed0,   
0x0532,   0x01761,   0x04f7d,   0x0d3f8,   0x01c638,   
0x0604,   0x01c93,   0x066de,   0x012375,   0x029a30,   
0x06eb,   0x02297,   0x08371,   0x018a53,   0x03bda5,   
0x07e8,   0x02982,   0x0a608,   0x020dc4,   0x0547f8,   
0x08fc,   0x0316a,   0x0cf8a,   0x02b3cc,   0x0755bc,   
0x0a28,   0x03a66,   0x0100f4,   0x038356,   0x0a0988,   
0x0b6d,   0x0448e,   0x013b5a,   0x04844a,   0x0d8cde,   
0x0ccc,   0x04ffb,   0x017fe8,   0x05bfa4,   0x0121128,   
0x0e46,   0x05cc7,   0x01cfe3,   0x073f8c,   0x017d0cc,   
0x0fdc,   0x06b0d,   0x022caa,   0x090f6f,   0x01f1058,   
0x0118f,   0x07ae9,   0x0297b7,   0x0b3c19,   0x0281fc7,   
0x01360,   0x08c78,   0x0312a0,   0x0dd3d0,   0x0335be0,   
0x01550,   0x09fd8,   0x039f18,   0x010e670,   0x0412fb0,   
0x01760,   0x0b528,   0x043ef0,   0x0148588,   0x0521620,   
0x01991,   0x0cc88,   0x04f418,   0x018c478,   0x0669ba8,   
0x01be4,   0x0e619,   0x05c0a0,   0x01db890,   0x07f6020,   
0x01e5a,   0x0101fd,   0x06a6b9,   0x0237930,   0x09d18b0,   
0x020f4,   0x012057,   0x07a8b6,   0x02a1fe9,   0x0c091e0,   
0x023b3,   0x01414b,   0x08c90d,   0x031c89f,   0x0eab1c9,   
0x02698,   0x0164fe,   0x0a0a58,   0x03a91ac,   0x011c7a68,   
0x029a4,   0x018b96,   0x0b6f56,   0x0449c04,   0x01570c14,   
0x02cd8,   0x01b53a,   0x0cfaec,   0x0500b5a,   0x019ba818,   
0x03035,   0x01e212,   0x0eb026,   0x05d0646,   0x01ebb372,   
0x033bc,   0x021247,   0x0109238,   0x06bb66c,   0x0248b9b8,   
0x0376e,   0x024603,   0x012a47f,   0x07c48a4,   0x02b47024,   
0x03b4c,   0x027d71,   0x014ea82,   0x08eed23,   0x0330b8c8,   
0x03f57,   0x02b8bd,   0x01767f3,   0x0a3d7a5,   0x03bfa5eb,   
0x04390,   0x02f814,   0x01a20b0,   0x0bb3f98,   0x04637d90,   
0x047f8,   0x033ba4,   0x01d18c4,   0x0d56048,   0x051ebd28,   
0x04c90,   0x03839c,   0x0205468,   0x0f2790c,   0x05f41d70,   
0x05159,   0x03d02c,   0x023d804,   0x0112cd74,   0x06e6967c,   
0x05654,   0x042185,   0x027a830,   0x0136a578,   0x07f963f0,   
0x05b82,   0x0477d9,   0x02bc9b5,   0x015e4da8,   0x09300968,   
0x060e4,   0x04d35b,   0x030418e,   0x018a175d,   0x0a8e5710};
