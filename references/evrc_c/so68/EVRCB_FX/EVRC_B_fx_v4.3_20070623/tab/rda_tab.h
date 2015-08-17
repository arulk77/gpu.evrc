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
/*  Tables for voice activity detection                                 */
/*======================================================================*/

static Word16 hangover0[TLEVELS] = {5,5,4,3,0,0,0,0}; 
static Word16 hangover1[TLEVELS] = {4,4,2,2,0,0,0,0}; 
static Word16 hangover2[TLEVELS] = {4,3,2,1,0,0,0,0}; 

static Word16 THRESH_SNR0_fx[FREQBANDS][TLEVELS][2]={ // Q5
{{ 224, 288 }, 
{ 224, 403 }, 
{ 256, 544 }, 
{ 275, 592 }, 
{ 285, 621 }, 
{ 301, 669 }, 
{ 704, 1632 }, 
{ 2022, 5094} },

{{ 224, 288 }, 
{ 224, 403 }, 
{ 256, 544 }, 
{ 275, 592 }, 
{ 285, 621 }, 
{ 301, 669 }, 
{ 704, 1632 }, 
{ 2022, 5094} }
};
static Word16 THRESH_SNR1_fx[FREQBANDS][TLEVELS][2]={ // Q5
{{224, 288 }, 
{224, 403 }, 
{256, 544 }, 
{275, 592 }, 
{285, 621 }, 
{361, 803 }, 
{1056, 2448 }, 
{4045, 10189 }}, 

{{224, 288 }, 
{224, 403 }, 
{256, 544 }, 
{275, 592 }, 
{285, 621 }, 
{361, 803 }, 
{1056, 2448 }, 
{4045, 10189 }}};

static Word16 THRESH_SNR2_fx[FREQBANDS][TLEVELS][2]={ // Q5
{{ 224, 288 }, 
{ 278, 506 }, 
{ 288, 592 }, 
{ 320, 640 }, 
{ 339, 730 }, 
{ 442, 1011 }, 
{ 1606, 5072 }, 
{ 6400, 20192 }}, 

{{ 224, 288 }, 
{ 278, 506 }, 
{ 288, 592 }, 
{ 320, 640 }, 
{ 339, 730 }, 
{ 442, 1011 }, 
{ 1606, 5072 }, 
{ 6400, 20192 }}, 
};

static Word32 LOWEST_LEVEL[FREQBANDS]={160, 80};
static Word32 voice_initial_fx[FREQBANDS]={50118723,5011872};

static Word16 rate_filt_auto_fx[FREQBANDS][FILTERORDER]={ // Q15
{   /* .2-2K bandpass filter, cut low end for noise immunity */
    0x34d8, 0x43c1, 0xfbb7, 0xd860,
    0xe8b6, 0x0060, 0xfc6f, 0xea96,
    0xe88a, 0xf880, 0x05fb, 0x061a, 
    0x00e4, 0xffb7, 0x01a1, 0x020e,
    0x00ca},
    {	/* 2-4k HPF */
    0x3cce, 0xae95, 0x06da, 0x1aad,
    0xf876, 0xf0b7, 0x08d1, 0x09d7,
    0xf28e, 0x0359, 0x0334, 0xfea9,
    0xfeb8, 0x00ac, 0x006a, 0xffb9,
    0x000a}
};
