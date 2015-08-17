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

/*===========================================================================*/
/*         ..Includes.                                                       */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "agc.h"
#include "check.h"
#include "dtmf_det.h"
#include "dtmf.h"
#include "goertzel.h"
#include "tables.h"

/*===========================================================================*/
/*         ..Defines.                                                        */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*         ..Globals.                                                        */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*         ..EVRC 20ms DTMF Detector Task (called every 160 samples)         */
/*---------------------------------------------------------------------------*/
/* buffer[] (simulates EVRC data buffering)
 *
 *                                       new data read in from file
 *                                        (new_dataP) (160 points)
 *                                   |--------------------------------|
 *                  |<------A------->|<------B------>|<------C------->|
 * |----------------|--------------------------------|----------------|
 *      lookback     data to be processed by vocoder     lookahead
 *     (80 points)      (cur_dataP) (160 points)        (80 points)
 *
 *
 *   Muting Logic:
 *   -------------
 *   if (block B AND block C contain "possible DTMF"),
 *          then mute blocks A and B;
 *   if  ( (DtmfDetectStatLast == 1) // Valid DTMF tone in last DTMF block? 
 *          AND (80pt DTMF block A contains "possible DTMF")  ),
 *             then mute blocks A, B, and C;
 */

short  Dtmf20ms (
    DTMF_DETECTOR* D,      /* (i) - pointer to detector structure */
    short*         buffer, /* (i) - EVRC data buffer (see comments above) */
    short*         digit   /* (i) - DTMF digit or -1 if no DTMF detect*/
)
{
    /*....(static) variables....*/
    static long   dtmfblock10ms = 0; /* 10ms DTMF block counter */
                                     /* (Not needed in DSP implementation!!) */

    /*....(local) variables....*/
    register  int j;
    short         DtmfDetectStatLast;
    short         digit1,digit2;
    short         possible_tone_block_A;
    short         possible_tone_block_B;
    short         possible_tone_block_C;
    short*        dataP;
    short*        cur_dataP;
    short*        new_dataP;
    short         dtmf_buf[N];
    short         mute = 0;            /* =1 mute current 160, =2 mute current+
 look_ahead */

    /*....execute....*/
    dataP = &buffer[0];          /* start of look-back */
    cur_dataP = &buffer[80];     /* current 160pt frame to be processed */
    new_dataP = &buffer[160];    /* start of data read in from file  */

    possible_tone_block_A = D->PossibleDtmfLast;

    /* Move data to local buffer since DtmfDetector() overwrites data...*/
    for (j=0; j<N; j++) {
        dtmf_buf[j] = new_dataP[j];
    }

    /* Process first N samples (second half of current speech frame) */
    possible_tone_block_B = DtmfDetector(D,dtmf_buf,dtmfblock10ms);
    DtmfDetectStatLast = D->DtmfDetectStat;
    digit1= -1;
    if (D->DtmfDetectStat ==1 )
        digit1 = D->DtmfDigit; 
    dtmfblock10ms++;

    /* Process second N samples (lookahead) */
    for (j=0; j<N; j++) {
        dtmf_buf[j] = new_dataP[N+j]; /* move data to local buffer */
    }

    possible_tone_block_C = DtmfDetector(D,dtmf_buf,dtmfblock10ms);
    digit2= -1;
    if (D->DtmfDetectStat ==1 )
        digit2 =D->DtmfDigit; 
    dtmfblock10ms++;

    /* Decide whether or not to mute */
    if ( possible_tone_block_B && possible_tone_block_C )
    {
        mute=1;
        for (j=0; j<160; j++) {
            cur_dataP[j] = 0;
        }
    }

    if ( (possible_tone_block_A) && (DtmfDetectStatLast == 1) )
    {
        mute=2;
        for (j=0; j<240; j++) {
            cur_dataP[j] = 0;
        }
    }

    D->PossibleDtmfLast = possible_tone_block_C;

    *digit = -1;
    if (digit1> -1 )   *digit=digit1;
    if (digit2> -1 )   *digit=digit2;
    return mute;
}
