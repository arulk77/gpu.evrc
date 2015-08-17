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
/*  LPC analysis filter, synthesis filter, impulse response etc         */
/*======================================================================*/

#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

/*===================================================================*/
/* FUNCTION      :  prediction_filter_fx ()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  FIR-filter input speech by the LPC filter        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) x   : input signal  (Qn).                         */
/*   _ (Word16 []) b   : filter coefficients (Qc).                   */
/*   _ (Word16)    P   : filter order.                               */
/*   _ (Word16)    N   : number of input samples.                    */
/*   _ (Word16)    Qa  : Q factor compensation (Qa=16-Qc)            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) y : output signal  (Qn)                           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*   _ (Word16 []) buf : filter memory (Qn).                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/*-------------------------------------------------------------------*/
/*      y(n)=x(n)-sum(i=10 to 1){x(n-i)*lpcCoeff(i)}                 */
/*===================================================================*/

void prediction_filter_fx(Word16 b[], Word16 x[], Word16 y[], Word16 buf[],
		 Word16 P, Word16 N, Word16 Qa)
{
  Word16 i,j;
  Word32 Ltemp;
  Word40 Lacc;

  for (i=0; i<N; i++) {
#ifdef WMOPS_FX
    move32();
#endif
    Lacc=0;
    for (j=P-1; j>0; j--) {
      Lacc=L_msu40(Lacc,b[j],buf[j]);
#ifdef WMOPS_FX
    move16();
#endif
      buf[j]=buf[j-1];
    }
    Lacc=L_msu40(Lacc,b[0],buf[0]);
#ifdef WMOPS_FX
    move16();
#endif
    buf[0]=x[i];
    Lacc=L_shr40(Lacc,1);  // Get rid of the L_mult left shift 1
                           // Lacc in Qc=Q(16-Qa)
    
    Ltemp=L_deposit_h(x[i]); // Ltemp=x[i] in Q16
    Ltemp=L_shr(Ltemp,Qa);   // Ltemp=x[i] in Q(16-Qa)

    Lacc=L_add40(Lacc,Ltemp); // Lacc=output in Q(16-Qa)
    Lacc=L_add40(L_shl40(Lacc,Qa),0x08000); // shift & round

    y[i]=extract_h((Word32)L_sat32_40(Lacc));

  }

}

/*===================================================================*/
/* FUNCTION      :  synthesis_filter_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  IIR-filter residual by the LPC sysnthesis filter */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) x   : residual input  (Qn).                       */
/*   _ (Word16 []) b   : filter coefficients (Qc).                   */
/*   _ (Word16)    P   : filter order.                               */
/*   _ (Word16)    N   : number of input samples.                    */
/*   _ (Word16)    Qa  : Q factor compensation (Qa=15-Qc)            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) y : output speech  (Qn)                           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*   _ (Word32 []) buf : filter memory (Q16+Qn)                      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/*      y(n)=sum(i=1 to 10){                                         */
/*              rxLpcCoeff(i)*rxFormSynthMem(n-i)                    */
/*              }                                                    */
/*           +fres(n)                                                */
/*===================================================================*/
void synthesis_filter_fx (Word16 b[], Word16 x[], Word16 y[], Word32 buf[],
		 Word16 P, Word16 N, Word16 Qa)
{
  Word16 i, j;
  Word16 mh, ml;
  Word32 Ltemp1;
  Word40 Lacc;

  for (i=0; i<N; i++) {
#ifdef WMOPS_FX
    move32();
#endif
    Lacc=0;
    for (j=P-1; j>=0; j--) {
      ml=extract_l(buf[j]);
      Lacc=L_mac40_su(Lacc,b[j],ml);
    }
    Ltemp1=(Word32)L_shr40(Lacc,15);
#ifdef WMOPS_FX
    move32();
#endif
    Lacc=0;
    for (j=P-1; j>0; j--) {
      mh=extract_h(buf[j]);
      Lacc=L_mac40(Lacc,b[j],mh);
#ifdef WMOPS_FX
    move16();
#endif
      buf[j]=buf[j-1];
    }
    mh=extract_h(buf[0]);
    Lacc=L_mac40(Lacc,b[0],mh);
    Lacc=L_add40(Lacc,Ltemp1); // combine MSW and LSW

    Ltemp1=L_deposit_h(x[i]);
    Lacc=L_add40(L_shl40(Lacc,Qa),Ltemp1); // Lacc = output in Q16
    buf[0]=(Word32)L_sat32_40(Lacc);
    
    y[i]=round32_16(buf[0]);
  }
}

/*===================================================================*/
/* FUNCTION      :  weight_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Apply weighting coefficients to LPCs             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) coef_orig  : Original coefficients                */
/*   _ (Word16 []) w   : weighting coefficients (Q15)                */
/*   _ (Word16)    N   : # of coefficients                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) coef_w  : Weighted coefficients, output           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/*      for (i=0;i<N;i++) {                                          */
/*         coef_w[i]=coef_orig[i]*w[i];                              */
/*      }                                                            */
/*===================================================================*/
void weight_fx(Word16 coef_orig[], Word16 w[], Word16 coef_w[], Word16 N)
{
  Word16 i;
    
  for (i=0; i<N; i++) {
    coef_w[i]=mult_r(coef_orig[i],w[i]);
  }
}

/*===================================================================*/
/* FUNCTION      :  lpc_weighted_filtering_fx ()                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute output of 3-stage LPC filters            */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) input  : input to filters                         */
/*   _ (Word16 []) acoef  : 1st stage synthesis filter coeffs, Q12   */
/*   _ (Word16 []) acoef_n: 2nd stage prediction filter coeffs, Q12  */
/*   _ (Word16 []) acoef_d: 3rd stage synthesis filter coeffs, Q12   */
/*   _ (Word16)    N   : length of output                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) output : Output after 3-stage LPC filters         */
/*                          Q factor same as input                   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word32 []) mem1:    1st stage filter memory, 10th order      */
/*   _ (Word16 []) mem2:    2nd stage filter memory, 10th order      */
/*   _ (Word32 []) mem3:    3rd stage filter memory, 10th order      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/*      Hwq(z) = 1 / Aq(z) * A(z/gamma1) / A(z/gamma2)               */
/*      Aq(z)=1-sum(aq(k)*z^-k) for k=1 to 10                        */
/*      A(z/gamma(i))=1-sum(ag(k,i)*z^-k) for k=1 to 10, i=1,2       */
/*===================================================================*/
/* NOTE: This function does not corrupt original filter memory       */
/*       unless t_mem? also points to the same original memory       */
/*===================================================================*/
void lpc_weighted_filtering_fx(Word16 input[], Word16 acoef[],
			       Word16 acoef_n[], Word16 acoef_d[],
			       Word32 mem1[], Word16 mem2[], Word32 mem3[],
			       Word16 output[], Word16 N)
{
  
  synthesis_filter_fx (acoef, input, output, mem1,
		       ORDER, N, 3);

  prediction_filter_fx(acoef_n, output, output, mem2,
		       ORDER, N, 4);
  
  synthesis_filter_fx (acoef_d, output, output, mem3,
		       ORDER, N, 3);
}


/*===================================================================*/
/* FUNCTION      :  ImpulseRzp_fx ()                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute LPC impulse response                     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) acoef  : 1st stage synthesis filter coeffs, Q12   */
/*   _ (Word16 []) acoef_n: 2nd stage prediction filter coeffs, Q12  */
/*   _ (Word16 []) acoef_d: 3rd stage synthesis filter coeffs, Q12   */
/*   _ (Word16)    N   : length of impulse response                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) ir  : Impulse response output, Q11                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/*      Hwq(z) = 1 / Aq(z) * A(z/gamma1) / A(z/gamma2)               */
/*      Aq(z)=1-sum(aq(k)*z^-k) for k=1 to 10                        */
/*      A(z/gamma(i))=1-sum(ag(k,i)*z^-k) for k=1 to 10, i=1,2       */
/*===================================================================*/
void ImpulseRzp_fx(Word16 acoef[], Word16 acoef_n[], Word16 acoef_d[],
		   Word16 ir[], Word16 N)
{
  Word16 i;
  Word16 temp_in[SubFrameSize];
  Word16 mem2[ORDER];
  Word32 mem1[ORDER], mem3[ORDER];
  
#ifdef WMOPS_FX
    move16();
#endif
  temp_in[0]=0x0800;   // one impulse 1.0 in Q11
  for (i=1; i<N; i++) temp_in[i]=0;

  for (i=0; i<ORDER; i++) {
#ifdef WMOPS_FX
    move32();
	move32();
	move32();
#endif
    mem1[i]=0;
    mem2[i]=0;
    mem3[i]=0;
  }
  lpc_weighted_filtering_fx(temp_in, acoef,  acoef_n, acoef_d, mem1, mem2, mem3, ir, N);
}


/*===================================================================*/
/* FUNCTION      :  ConvolveImpulseR_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convolve input signal with impulse response      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) input  : input signal, Q0                         */
/*   _ (Word16 []) ir  : impulse response vector, Q11                */
/*   _ (Word16)    L : length of impulse response                    */
/*   _ (Word16)    N   : length of input                             */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) output  : output of convolution, Q0               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: Length of impulse response is fixed at 54 (i.e. L=54)       */
/*===================================================================*/

void ConvolveImpulseR_fx(Word16 input[], Word16 ir[], Word16 output[],
			 Word16 L, Word16 N)
{
  Word16 i,j;
  Word40 Lacc;

  for (i = 0; i < N; i++) {
#ifdef WMOPS_FX
    move32();
#endif
    Lacc=0;
    for (j = 0; j <= MIN_FX(i, L-1); j++)
      Lacc = L_mac40(Lacc, input[i - j], ir[j]);
    Lacc=L_shl40(Lacc,4);  // Lacc in Q(11+1+4)
    Lacc=L_add40(Lacc,0x08000);
    Lacc=L_sat32_40(Lacc);
    output[i] = extract_h((Word32)Lacc);
  }
}

