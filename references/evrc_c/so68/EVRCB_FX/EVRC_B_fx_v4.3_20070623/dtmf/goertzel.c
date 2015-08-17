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
/*         ..Defines.                                                        */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*         ..Includes.                                                       */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include  "basic_op40.h"
#include  "basic_op.h"
#include "dtmf.h"
#include "tables.h"

/*===========================================================================*/
/* ..Goertzel filtering and energy computation function                      */
/*   for the eight DTMF fundamental frequencies.                             */
/*---------------------------------------------------------------------------*/
void  goertzel (
    DTMF_DETECTOR* D,    /* (i) - pointer to detector structure */
    short*         data  /* (i)   - input data buffer */
)
{
    /* ..(local) variables..*/
    register int j,k;
    short FiltMem[16];
    short temp;
    short *dP;
    short *vkP;
    short *vkP1;
    short *vkP2;
    short *eP;
    short *coefP;
    Acc   accA;
    Acc   accB;


    /* ..execute..*/

    /* Initialize filter memories to zero for each data frame */
    /* to be filtered. */
    vkP = FiltMem;
    for (j=0; j<16; j++)
    {
        *vkP++ = 0;
    }

    /* Goertzel filtering */
    vkP  = &FiltMem[15];
    vkP1 = &FiltMem[14]; 
    coefP = COEF1st;           /* coefP points to coefficients */
    eP = D->DtmfEnergy;        /* eP points to energy template */
    /*for (k=0; k<7; k++) */       /* loop over 7 filters (4th col not needed)*/
    for (k=0; k<8; k++)        /* loop over 8 filters */
    {
        /* Compute vk(n) = 2*coef*vk(n-1) - vk(n-2) + x(n) for 7 fundamental freqs. */
        dP = data;        /* dP points to input data */
        for (j=0;j<N;j++) 
        {

			accA= (*dP++)<<16;             /* A(31:16) = x(n) */
            accB = L_sub40(accA,(*vkP<<16));   /* B = -vk(n-2)+x(n) */
            accB = L_mac40(accB,*vkP1,*coefP); /* B = coef*vk(n-1)-vk(n-2)+x(n) */
            accB = L_mac40(accB,*vkP1,*coefP); /* B = 2*coef*vk(n-1)-vk(n-2)+x(n) */
            *vkP = *vkP1;                       /* delay: vk(n-2) = vk(n-1) */
			accB = L_shr40(accB,16);
            *vkP1 = extract_l40(accB);             /* vk(n-1) = B */

        }
        /* Energy computation for 7 fundamental freqs. */
        /* y(n)y*(n) = vk(n)*vk(n) - 2*coef*vk(n)*vk(n-1) + vk(n-1)*vk(n-1) */
        temp = *vkP;                       /* T = vk(n-1) */
        accB = L_mult(*vkP,*vkP);        /* B = vk(n-1)*vk(n-1) */
        accA = L_mult(temp,*vkP1);       /* A = vk(n-1) * vk(n) */
		accA = L_shr40(accA,16);
        temp = extract_l40(accA);  
        accA = L_mult(*coefP,temp);       /* A = coef * vk(n-1) * vk(n) */
        accA = L_shl40(accA,1);            /* A = A*2 */
        accB =  (accB-accA);               /* B = vk(n-1)*vk(n-1)-2*coef*vk(n-1)*vk(n) */
        accB = L_mac40(accB,*vkP1,*vkP1);  /* B += vk(n)*vk(n)-2*coef*vk(n-1)*vk(n) */
        accB = L_shr40(accB,16);
        *eP++ = extract_l40(accB);          /* store result into energy template */

        /* Update pointers for next filter */
        coefP++;
        vkP-=2;
        vkP1-=2;
    }
    /* Save filter taps for 1st row frequency for next 160 pt goertzel */
    vkP  = &FiltMem[15];
    vkP1 = &D->DtmfSaveMem1[1];
    vkP2 = &D->DtmfSaveMemLast1[1];
    for (j=0; j<2; j++)
    {
        *(vkP2--) = (*( vkP1 ))>>1;
        *(vkP1--) = *(vkP--);
    }
    /* Save filter taps for 4th row frequency for next 160 pt goertzel */
    vkP  = &FiltMem[9];
    vkP1 = &D->DtmfSaveMem4[1];
    vkP2 = &D->DtmfSaveMemLast4[1];
    for (j=0; j<2; j++)
    {
        *(vkP2--) = (*( vkP1 ))>>1;
        *(vkP1--) = *(vkP--);
    }

    /* Exit */
    return;
}


/*===========================================================================*/
/* ..Goertzel filtering and energy computation function                      */
/*   for 2nd harmonic checks and 160pt check                                 */
/*---------------------------------------------------------------------------*/
Acc  goertzel2 (              /* (o) - goertzel energy */
    short* InputData,         /* (i) - data sequence to be filtered */
    short* StateVariables,    /* (i) - filter state variables (memory) */
    short* FilterCoef,        /* (i) - filter coefficient */
    short* EnergyTemplate,    /* (i) - goertzel energy template */
    short  InputShift         /* (i) - shift factor for input data */
)
{
    /* ..(local) variables..*/
    register int  j;
    short  *dP;
    short  *vkP;
    short  *vkP1;
    short  *coefP;
    short  *eP;
    short  temp;
    short  s;
    Acc    accA;
    Acc    accB;
   
    /* ..execute..*/
    dP = InputData;
    vkP = StateVariables;
    coefP = FilterCoef;
    eP = EnergyTemplate;
    s = InputShift;

    vkP1 = vkP;
    vkP1--;
    for (j=0; j<N; j++)       /* loop over N inputs */
    {
        /* Goertzel filtering */
			accA= (*dP++)<<16;             /* A(31:16) = x(n) */
			accA = L_shr40(accA,s);
            accB = L_sub40(accA,(*vkP<<16));   /* B = -vk(n-2)+x(n) */
            accB = L_mac40(accB,*vkP1,*coefP); /* B = coef*vk(n-1)-vk(n-2)+x(n) */
            accB = L_mac40(accB,*vkP1,*coefP); /* B = 2*coef*vk(n-1)-vk(n-2)+x(n) */
            *vkP = *vkP1;                       /* delay: vk(n-2) = vk(n-1) */
			accB = L_shr40(accB,16);
            *vkP1 = extract_l40(accB);             /* vk(n-1) = B */
    }


    /* Energy computation */
    /* y(n)y*(n) = vk(n)*vk(n) - 2*coef*vk(n)*vk(n-1) + vk(n-1)*vk(n-1) */
        temp = *vkP;                      /* T = vk(n-1) */
	    accB = L_mult(*vkP,*vkP);        /* B = vk(n-1)*vk(n-1) */
		vkP--;
        accA = L_mult(temp,*vkP );       /* A = vk(n-1) * vk(n) */
		accA = L_shr40(accA,16);
        temp = extract_l40(accA);  
        accA = L_mult(*coefP,temp);       /* A = coef * vk(n-1) * vk(n) */
        accA = L_shl40(accA,1);            /* A = A*2 */
        accB =  (accB-accA);               /* B = vk(n-1)*vk(n-1)-2*coef*vk(n-1)*vk(n) */
        accB = L_mac40(accB,*vkP ,*vkP );  /* B += vk(n)*vk(n)-2*coef*vk(n-1)*vk(n) */
        accA = L_shr40(accB,16);
        *eP  = extract_l40(accA);          /* store result into energy template */
        return (accB);
}	
