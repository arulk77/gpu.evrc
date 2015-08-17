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
/*===================================================================*/
/* Conexant Systems Inc., Ericsson Inc., Lucent Technologies Inc.,   */
/* Motorola Inc., Nokia Inc, Nortel Networks Corporation, and        */
/* Qualcomm Inc. do hereby grant a free, irrevocable license to      */
/* 3GPP2 and its Organizational Partners (OPs) to incorporate text   */
/* or other copyrightable material contained in the above-referenced */
/* document and any modifications thereof in the creation of 3GPP2   */
/* publications; to copyright and sell in Organizational Partner's   */
/* name, any Organizational Partner's standards publication even     */
/* though it may include portions of the text or other copyrightable */
/* material contained in such document; and at the Organizational    */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such text or other copyrightable material or the       */
/* resulting Organizational Partner's standards publication.         */
/* Conexant Systems Inc., Ericsson Inc, Lucent Technologies Inc.,    */
/* Motorola Inc., Nokia Inc, Nortel Networks Corporation, and        */
/* Qualcomm Inc. do hereby agree that it will grant licenses under   */
/* any copyrights of such undersigned party to third parties on      */
/* reasonable, non-discriminatory terms and conditions.              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel or otherwise, any license or right under    */
/* any existing or later issuing patent, whether or not the use of   */ 
/* information in the document necessarily employs an invention of   */
/* any existing or later issued patent, or copyright, but Conexant   */
/* Systems Inc., Ericsson Inc., Lucent Technologies Inc., Motorola   */
/* Inc., Nokia Inc, Nortel Networks Corporation, and Qualcomm Inc.   */
/* will make a copyright license available as hereinabove previously */
/* stated.                                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*===================================================================*/

short  thr_rowrel[4] = {10485, 11469, 7864, 6554};
short  thr_sig2[4] = {6900, 6850, 6650, 6400};

/*******************************************************
 * The following table holds the coefficients for      *
 * the 16 Goertzel filters, computed as follows        *
 *                                                     *
 *           cos(2pi*k/N)*32768    //coefficient  *
 *                                                     *
 * with  N = 102  (1st and 2nd Harmonics)              *
 *       k =  9, 10, 11, 12, 15, 17, 19, 21  (1. Harm) *
 *       k = 18, 20, 22, 24, 31, 34, 38, 42  (2. Harm) *
 *                                                     *
 ******************************************************/

short  COEF1st[8] = {  /* 1st Harmonics */
        27980, 
        26956,
        25701,
        24219,
        19073,
        16325,
        13085,
         9315
};

short  COEF2nd[8] = {  /* 2nd Harmonics */
         14739,
         11414,
          7549,
          3032,
        -10565,
        -16503,
        -22318,
        -27472
};

/* DTMF Digit Lookup Table: */
/*    4-bit digit array index = ((row<<2) | col) */
/*    Digits 'A', 'B', 'C', 'D' (3rd col) */
/*    are treated as 'invalid' and are set to 0xffff. */
short  dtmf_lookup_table[16] = {
    0x0001,      /* '1' : row = 0, col = 0 */
    0x0002,      /* '2' : row = 0, col = 1 */
    0x0003,      /* '3' : row = 0, col = 2 */
    0x000d,      /* 'A' : row = 0, col = 3 */
    0x0004,      /* '4' : row = 1, col = 0 */
    0x0005,      /* '5' : row = 1, col = 1 */
    0x0006,      /* '6' : row = 1, col = 2 */
    0x000e,      /* 'B' : row = 1, col = 3 */
    0x0007,      /* '7' : row = 2, col = 0 */
    0x0008,      /* '8' : row = 2, col = 1 */
    0x0009,      /* '9' : row = 2, col = 2 */
    0x000f,      /* 'C' : row = 2, col = 3 */
    0x000b,      /* '*' : row = 3, col = 0 */
    0x000a,      /* '0' : row = 3, col = 1 */
    0x000c,      /* '#' : row = 3, col = 2 */
    0x0010       /* 'D' : row = 3, col = 3 */
};

short   dtmf_low_freq[17] = {
           0,           /* 0 */
        27980, 		/* 697 Hz Digit = 1 */
        27980, 		/* 697 Hz Digit = 2 */
        27980, 		/* 697 Hz Digit = 3 */
        26956,		/* 770 Hz Digit = 4 */
        26956,		/* 770 Hz Digit = 5 */
        26956,		/* 770 Hz Digit = 6 */
        25701,		/* 852 Hz Digit = 7 */
        25701,		/* 852 Hz Digit = 8 */
        25701,		/* 852 Hz Digit = 9 */
        24219,		/* 941 Hz Digit = 0 */
        24219,		/* 941 Hz Digit = * */
        24219,		/* 941 Hz Digit = # */
        27980, 		/* 697 Hz Digit = A */
        26956,		/* 770 Hz Digit = B */
        25701,		/* 852 Hz Digit = C */
        24219		/* 941 Hz Digit = D */
};

short   dtmf_high_freq[17] = {
         0,             /* 0 */
        19073,		/* 1209 Hz Digit = 1 */
        16325,		/* 1336 Hz Digit = 2 */
        13085,		/* 1477 Hz Digit = 3 */
        19073,		/* 1209 Hz Digit = 4 */
        16325,		/* 1336 Hz Digit = 5 */
        13085,		/* 1477 Hz Digit = 6 */
        19073,		/* 1209 Hz Digit = 7 */
        16325,		/* 1336 Hz Digit = 8 */
        13085,		/* 1477 Hz Digit = 9 */
        16325,		/* 1336 Hz Digit = 0 */
        19073,		/* 1209 Hz Digit = * */
        13085,		/* 1477 Hz Digit = # */
         9315,		/* 1633 Hz Digit = A */
         9315,		/* 1633 Hz Digit = B */
         9315,		/* 1633 Hz Digit = C */
         9315		/* 1633 Hz Digit = D */
};
