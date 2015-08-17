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
//included for the spectral tilt algo ( in floating point)

#include "DTX_floating_point_functions.h"


void weight(float *awght,float *a,float gamma,short order)
{
  register int i;
  double  fac;
  
  /* For the simulation: prepare a multiplication table */
  fac = gamma;
  
  /* Now calculate the exponential weighting */
  for (i=0; i<order; i++) {
    *awght++ = *a++ * fac;
    fac *= gamma;
  }
}

void ImpulseRzp(float *output,float *coef_uq,float *coef,float gamma1,
		float gamma2,short order,short length)
{
  register short i, j;
  float   SUM;
  float   memory[ORDER];
  float   wcoef[ORDER];
  
  /* ImpulseR of 1/A(Z) */
  output[0] = 1.0;
  memory[0] = 1.0;
  
  for (i=1; i<order; i++) memory[i] = 0;
  for (i=1; i<length; i++) {
    for (j=order-1,SUM=0; j>0; j--) {
      SUM -= coef[j] * memory[j];
      memory[j] = memory[j - 1];
    }
    SUM -= coef[0] * memory[0];
    *memory = SUM;
    output[i] = SUM;
  }

  /* Cascade with ImpulseR of A(Z/gamma1) */
  weight(wcoef, coef_uq, gamma1, order);
  
  memory[0] = 1.0;
  for (i=1; i<order; i++) memory[i] = 0;
  for (i=1; i<length; i++) {
    for (j=order-1,SUM=output[i]; j>0; j--) {
      SUM += wcoef[j] * memory[j];
      memory[j] = memory[j - 1];
    }
    SUM += wcoef[0] * memory[0];
    *memory = output[i];
    output[i] = SUM;
  }

  /* Cascade with ImpulseR of 1/A(Z/gamma2) */
  weight(wcoef, coef_uq, gamma2, order);
  
  memory[0] = 1.0;
  for (i=1; i<order; i++) memory[i] = 0;
  for (i=1; i<length; i++) {
    for (j=order-1,SUM=output[i]; j>0; j--) {
      SUM -= wcoef[j] * memory[j];
      memory[j] = memory[j - 1];
    }
    SUM -= wcoef[0] * memory[0];
    *memory = SUM;
    output[i] = SUM;
  }
}

/*===================================================================*/
/* FUNCTION      :  LPC_pred2refl ().                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function calculate the PARCOR coefficients  */
/*                  using  the prediction coeff.                     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*             _ (float []) a         : output LP filter coeff.    */
/*             _ (int    ) LPC_ORDER : LPC order.                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*             _ (float []) refl      : output reflection coeff.   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/

void LPC_pred2refl (float a [], float refl [], int LPC_ORDER)
	{
	 /*-------------------------------------------------------------------*/
	 
#ifdef	WIN32
		float* f = new float[LPC_ORDER];
#else
		float	f[LPC_ORDER];
#endif

	 int	 m, j, n;
	 float km, denom, x;

	 /*-------------------------------------------------------------------*/



	 /*-------------------------------------------------------------------*/
	 /*                          Initialisation                           */
	 /*-------------------------------------------------------------------*/
#ifdef WMOPS
	 WMP_cnt_negate (LPC_ORDER);
#endif
	 for (m = 0; m < LPC_ORDER;  m++)
	   f[m] = -a[m]; 

	 /*-------------------------------------------------------------------*/
	
	 for (m = LPC_ORDER-1; m >= 0; m--)
	   {
#ifdef WMOPS
	     WMP_cnt_move (1);
	     WMP_cnt_test (2);
#endif
	     km = f[m];

	     if (km <= -1.0 || km >= 1.0)
	       { 
#ifdef VERBOSE
		 printf("Nonstable reflection coeffs !!!\n"); 
#endif
		 return; 
	       }
	     
#ifdef WMOPS
	     WMP_cnt_negate	(1);
	     WMP_cnt_move	(1);
	     WMP_cnt_mac	(1);
	     WMP_cnt_div	(1);
#endif
	     refl [m] = -km;
	     denom = 1.0/(1.0 - km * km);
	     
	     for (j = 0; j < m/2; j++) 
	       {
#ifdef WMOPS
		 WMP_cnt_mult (4);
		 WMP_cnt_move (2);
		 WMP_cnt_mac  (2);
#endif
		 n    = m - 1 - j;
		 x    = denom * f[j] + km * denom * f[n];
		 f[n] = denom * f[n] + km * denom * f[j];
		 f[j] = x;
	       }
#ifdef WMOPS
	     WMP_cnt_test (1);
	     if (m & 1)
	       {
		 WMP_cnt_mult (2);
			 WMP_cnt_mac  (1);
	       }
#endif
	     if (m & 1)
	       f[j] = denom * f[j] + km * denom * f[j];
	   }   
	 
	 /*-------------------------------------------------------------------*/
#ifdef	WIN32
	   delete[] f;
#endif

	 return;

	 /*-------------------------------------------------------------------*/
	}

/*----------------------------------------------------------------------------*/
/*======================================================================*/
/*  Lucent Technologies Network Wireless Systems                        */
/*  EVRC Floating-point C Simulation.                                   */
/*                                                                      */
/*  Copyright (C) 1996 Lucent Technologies Incorporated. All rights     */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
/*  Module:     lsp2a                                                   */
/*----------------------------------------------------------------------*/
/*  History:                                                            */
/*     01/01/95  Written By Dror Nahumi, AT&T                           */
/*----------------------------------------------------------------------*/

/************************************************************************
* Routine name: lsptopc.                                                *
* Function: Convert lsp to pc.                                          *
* Inputs: freq - Quantized line spectral frequencies.                   *
* Outputs: pc - prediction coefficients.                                *
************************************************************************/

void lsp2a(float *pc,float *freq)
{
  float   p[ORDER / 2], q[ORDER / 2];
  float   a[ORDER / 2 + 1], a1[ORDER / 2 + 1], a2[ORDER / 2 + 1];
  float   b[ORDER / 2 + 1], b1[ORDER / 2 + 1], b2[ORDER / 2 + 1];
  float   xx, xf;
  int     i, k;
  int     lspflag;
  
  lspflag = 1;
  /*	*** check input for ill-conditioned cases */
  if ((freq[0] <= 0.0) || (freq[ORDER - 1] >= 0.5)) {
    lspflag = 0;
    if (freq[0] <= 0) freq[0] = 0.022;
    if (freq[ORDER - 1] >= 0.5)	freq[ORDER - 1] = 0.499;
  }

  if (!lspflag) {
    xx = (freq[ORDER - 1] - freq[0]) / (float) ORDER;
    for (i=1; i<ORDER; i++)	freq[i] = freq[i - 1] + xx;
  }

  for (i=0; i<=ORDER/2; i++) {
    a[i] = 0.;
    a1[i] = 0.;
    a2[i] = 0.;
    b[i] = 0.;
    b1[i] = 0.;
    b2[i] = 0.;
  }
  
  for (i=0; i<ORDER/2; i++) {
    p[i] = cos(6.2832 * freq[2 * i]);
    q[i] = cos(6.2832 * freq[2 * i + 1]);
  }
  
  xf = 0.;
  xx = .25;
  
  for (k=0; k<=ORDER; k++) {
    a[0] = xx + xf;
    b[0] = xx - xf;
    xf = xx;
    xx = 0;
    
    for (i=0; i<ORDER/2; i++) {
      a[i + 1] = a[i] - 2 * p[i] * a1[i] + a2[i];
      b[i + 1] = b[i] - 2 * q[i] * b1[i] + b2[i];
      a2[i] = a1[i];
      a1[i] = a[i];
      b2[i] = b1[i];
      b1[i] = b[i];
    }
    if (k != 0) {
      pc[k - 1] = 2 * (a[ORDER / 2] + b[ORDER / 2]);
    }
    //printf("LSP2A %f\n",pc[k-1]);
  }
}



/* above are some floating point fns */
