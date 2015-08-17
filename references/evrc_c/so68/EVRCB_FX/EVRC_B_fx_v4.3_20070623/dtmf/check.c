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
#include "basic_op.h"
#include "dtmf.h"
#include "goertzel.h"
#include "tables.h"

/*===========================================================================*/
/*         ..Defines.                                                        */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*         ..Globals.                                                        */
/*---------------------------------------------------------------------------*/
void  terminate_checks(DTMF_DETECTOR*);
 
/*===========================================================================*/
/*         ..Determine if DTMF is present.                                   */
/*---------------------------------------------------------------------------*/
void  check (
    DTMF_DETECTOR* D,    /* (i) - pointer to detector structure */
    short*         data  /* (i) - input data buffer */
)
{
    /* ..(local) variables..*/
    register int k;
    short dtmf_mem[16]; /* local scratch array */
    short *dP;
    short *vkP;
    short *eP;
    short *coefP;
    short *rmaxP;  /* points to max. row energy */
    short *cmaxP;  /* points to max. col. energy */
    short rmax_index;
    short cmax_index;
    Acc   accA;
    Acc   accB;

    /*..execute..*/

    /* DetectStat flag can be set to 1 only if */
    /* all conditions for a valid digit are met. */
    D->DtmfDetectStat = 0;



    /* Find maximum row energy */
    eP = D->DtmfEnergy;
    rmaxP = eP;
    rmax_index = 0;
    eP++;
    for (k=1; k<4; k++)
    {
        if (*eP > *rmaxP) 
        {
            rmaxP = eP;
            rmax_index = k;
        }
        eP++;
    }

    /* Find maximum column energy */
    eP = &D->DtmfEnergy[4];
    cmaxP = eP;
    cmax_index = 4;
    eP++;
    for (k=1; k<4; k++)
    {
        if (*eP > *cmaxP) 
        {
            cmaxP = eP;
            cmax_index = k+4;
        }
        eP++;
    }

    /* Pause test */
    if ( Max(*rmaxP,*cmaxP) < THR_PAU)
    {
        D->DtmfPauseLatch += 1;
        if (D->DtmfPauseLatch >= 16384)  D->DtmfPauseLatch = 16384;

        terminate_checks(D);
        return;
    }

    /* Signal strength check: compare sum of rowmax and colmax to threshold */
    if ( (*rmaxP + *cmaxP) <= thr_sig2[rmax_index])
    {
        terminate_checks(D);
        return;
    }

    /* Twist Check */
    if (*rmaxP <= *cmaxP)  /* standard twist */
    {
     /*   if ( (A_set_hi(*rmaxP)) <= (A_mul_ss(THR_STDTWI,*cmaxP)) )*/
		 if ( ( (*rmaxP)<<16) <= (L_mult(THR_STDTWI,*cmaxP)) )
        {
            terminate_checks(D);
            return;
        }
    } else   /* reverse twist */
    {
        if ( ( (*cmaxP)<<16) <= (L_mult(THR_REVTWI,*rmaxP)) )
        {
            terminate_checks(D);
            return;
        }
    }

    /* Copy goertzel energies to local array */
    for (k=0; k<8; k++)
    {
        dtmf_mem[k] = D->DtmfEnergy[k];
    }

    /* Set highest row and col energies in local mem to -1 */
    /* in order to compute second highest row and col energies. */
    dtmf_mem[rmax_index] = -1;
    dtmf_mem[cmax_index] = -1;

    /* Compare max row energy to second highest row energy */
    eP = dtmf_mem;
    accB =  (-1)<<16;    /* hold second highest row energy in accB */
    for (k=0; k<4; k++)
    {
       accA =  (*eP)<<16;
       accB = Max(accB,accA); 
       eP++;
    }

    if ( accB >= L_mult(thr_rowrel[rmax_index],*rmaxP) )
    {
        terminate_checks(D);
        return;
    }

    /* Compare max col energy to second highest col energy */
    accB =  (-1)<<16;    /* hold second highest col energy in accB */
    for (k=0; k<4; k++)
    {
       accA = (*eP)<<16;
       accB = Max(accB,accA); 
       eP++;
    }

    if ( accB >=L_mult(THR_COLREL,*cmaxP) )
    {
        terminate_checks(D);
        return;
    }

    /* 2nd Harmonic check for row */
    coefP = &COEF2nd[rmax_index];
    dP = data;                /* dP points to input data */
    dtmf_mem[0] = 0;          /* init goertzel filter memory to 0 */
    dtmf_mem[1] = 0;
    vkP = &dtmf_mem[1];       /* vkP points filter memory for row's 2nd harmonic */
    eP = &D->DtmfEnergy[8];   /* pointer to row's 2nd harmonic in energy template */

    /* Goertzel filtering and energy computation */
    accB = goertzel2(dP,vkP,coefP,eP,0);

    /* Check threshold conditions for row's 2nd harmonic */
    if (accB >= L_mult(THR_ROW2nd,*rmaxP)) 
    {
        terminate_checks(D);
        return;
    }

    /* 2nd Harmonic check for column */
    coefP = &COEF2nd[cmax_index];
    dP = data;                /* dP points to input data */
    dtmf_mem[0] = 0;          /* init goertzel filter memory to 0 */
    dtmf_mem[1] = 0;
    vkP = &dtmf_mem[1];       /* vkP points filter memory for col's 2nd harmonic */
    eP = &D->DtmfEnergy[9];   /* pointer to column's 2nd harmonic in energy template */

    /* Goertzel filtering and energy computation */
    accB = goertzel2(dP,vkP,coefP,eP,0);

    /* Check threshold conditions for column's 2nd harmonic */
    if (accB >= L_mult(THR_COL2nd,*cmaxP)) 
    {
        terminate_checks(D);
        return;
    }

    /* If code reaches this point, all validity tests have passed (except 3.5% test).*/
    k = (((rmax_index)&0x3)<<2) | ((cmax_index-4)&0x3);
    D->DtmfDigit = dtmf_lookup_table[k];

    /* Cannot change DtmfDuration from 0 to 1 unless a pause had been */
    /* detected 3 previous  and the last digit was considered not valid */
    if ( (D->DtmfDigitLast == (short) 0x8000) && (D->DtmfPauseLatch >= 3) )

    {
        D->DtmfDuration = 1;
        D->DtmfDigitLast = D->DtmfDigit;
        D->DtmfDigit = 0xffff;
        return;
    }

    /* Current digit must = last digit to increment duration counter */
    if ( (D->DtmfDuration > 0) && (D->DtmfDigit == D->DtmfDigitLast) )
    {
        D->DtmfDuration += 1;
    }
    else
    {
        /* if (D->DtmfDuration == 0) || (D->DtmfDigit != D->DtmfDigitLast) */
        D->DtmfDuration = 0;
        D->DtmfPauseLatch = 0;  /* must now wait for a pause */
    }

    /* Check for 3.5% error in lowest and highest row  freq using 160 pt Goertzel */
    if ( (D->DtmfDuration == THR_DURATION) && ((rmax_index==0)||(rmax_index==3)) )
    {
        coefP = &COEF1st[rmax_index];
        dP = data;                    /* dP points to input data */
        if (rmax_index==0)
            vkP = &D->DtmfSaveMemLast1[1]; /* vkP points to saved filter memory for 1st freq row's max */
        if (rmax_index==3)
            vkP = &D->DtmfSaveMemLast4[1]; /* vkP points to saved filter memory for 4th freq row's max */

        eP = &D->DtmfEnergy[10];  /* pointer to rowmax 160 pt energy template */

        /* Goertzel filtering and energy computation */
        accB = goertzel2(dP,vkP,coefP,eP,1);

        /* Check threshold conditions for row's160 pt Goertzel */
        accA = L_mult(THR_ROW160,*rmaxP);
        if   ((accB-accA) <= 0)  /* accB still holds *eP */
        {
            terminate_checks(D);
            return;
        }
    }

    D->DtmfDigitLast = D->DtmfDigit;
    if (D->DtmfDuration >= THR_DURATION)
    {
        D->DtmfPauseLatch = 0;
        D->DtmfDetectStat = 1;  /* valid DTMF signal present */
    } else {
        D->DtmfDigit=0xffff;
    }

    /* Exit */
    return;
}

void  terminate_checks (
    DTMF_DETECTOR* D  /* (i) - pointer to current channel detector structure */
)
{
    D->DtmfDigitLast = (short) 0x8000; 
    D->DtmfDuration = 0;
    D->DtmfDigit = 0xffff;
}
