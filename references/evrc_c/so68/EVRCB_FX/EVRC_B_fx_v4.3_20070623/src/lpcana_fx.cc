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
/*  LPC analysis: windowing, durbin, lpc2lsp, lsp2lpc etc               */
/*======================================================================*/

#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "lpcana_tab.h"
#include "cos_tab.dat"
#include "globs_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


#define LPCGAINFIX //lpc g

/*===================================================================*/
/* FUNCTION      :  autocorrelation_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Window the input speech and calculate            */
/*                  autocorrelation                                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) input : input speech  (Q0)                        */
/*   _ (Word16)  len   : length of window                            */
/*   _ (Word16)  order : number of autocorr                          */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32 []) r : autocorr vector, normalized                   */
/*   _ (Word16) *r_scale : norm factor of autocorr                   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void autocorrelation_fx(Word16 input[], Word16 len, Word16 order,
			Word32 r[], Word16 *r_scale)
{

  Word16 k, i;
  Word16 winput[320];
  Word32 Ltemp, max_s;
  Word16 n1, n2, d1h, d1l, d2h, d2l;
  Word40 Lacc, Lacc2;
  
  /* First find out the block normalization factor for windowed samples */
  for (k=0, max_s=0;k<len;k++) {
    Ltemp=(Word32) L_mult_su(input[k],lpc_window[k]);  // lpc_window[] in Q16
#ifdef WMOPS_FX
    test();
#endif
    if (Ltemp<0) Ltemp=~Ltemp;
    max_s=MAX_FX(max_s,Ltemp);  // max_s has the sample with least sign bits
  }
  n1=norm_l(max_s);  // n1 is normalization factor for windowed samples
                     // If input are all 0s, n1 will be 0 !!!
  /* Window the input and normalize windowed samples  */
  for (k = 0; k < len; k++) {
    Ltemp=(Word32) L_mult_su(input[k],lpc_window[k]);
    winput[k]=round32_16(L_shl(Ltemp,n1));
  }

  /* Calculate autocorrelation coefficients and store them as long integers. */

  /* First calculate r[0], the largest value for norm factor   */
  for (i=0, Lacc=0; i<len; i++) {
    Lacc=L_mac40(Lacc,winput[i], winput[i]);
    // Accumulate 320 samples
  }
  Lacc=L_shr40(Lacc,1); // remove the extra left shift
  /* Do a triple by double multiplication: Lacc=Lacc+Lacc*coef[0] */
  Ltemp=(Word32) L_shr40(Lacc,16); // Ltemp high word has MSW of Lacc
  d1h=extract_h(w_corr[0]);   // w_corr[0] is actual coef - 1.0
  d1l=extract_l(w_corr[0]);
  d2l=extract_l40(Lacc);
  d2h=extract_l(Ltemp);  // This is actually the middle of Lacc, unsigned number
  Lacc2=L_mult_uu(d1h,d2l);
  Lacc2=L_add40_40(Lacc2,L_mult_uu(d2h,d1l));
  Lacc2=L_add40(Lacc,(Word32)L_shr40(Lacc2,15)); // Lacc2=R[0]+LSW(coef[0]*R[0])
  Lacc=L_mult_uu(d2h,d1h);
  Lacc=L_add40_40(Lacc,L_mult_uu(d1l,extract_h(Ltemp))); // Lacc should be in 32 bits because w_corr[0] is small
  Lacc2=L_add40_40(Lacc2,L_shl40(Lacc,1));
  Lacc=L_mult_uu(d1h,extract_h(Ltemp));  // In this case, Lacc=0 because d1h=0
  Lacc=L_add40_40(Lacc2,L_shl40(Lacc,17)); 

  n2=norm32_l40(Lacc);  // If input are all 0s, n2 will be 0 !!!
  
  r[0] = (Word32)L_shl40(Lacc,n2);

  for (k = 1; k < order; k++) {
    for (i = 0, Lacc = 0; i < len - k; i++) {
      Lacc=L_mac40(Lacc,winput[i],winput[i+k]);
    }
    Lacc=L_shr40(Lacc,1);
    r[k] = (Word32)L_shl40(Lacc, n2); // block normalize r[k] according to r[0]
  /* Spectral smoothing of autocorrelation coefficients */
    d1h=extract_h(w_corr[k]); // double by double multiplication
    d1l=extract_l(w_corr[k]); // w_corr[k]<1, in Q31
    d2h=extract_h(r[k]);
    d2l=extract_l(r[k]);
    Lacc=L_mult_su(d1h,d2l);
    Lacc=L_mac40_su(Lacc,d2h,d1l);
    Ltemp=L_mult(d1h,d2h);
    r[k]=(Word32)L_add40(L_shr40(Lacc,15),Ltemp);  // r[k]*=w_corr[k], w_corr[k] in Q31
    
  }
  *r_scale=add(n2,shl(n1,1));  // final norm factor =n2+2*n1
}

/*===================================================================*/
/* FUNCTION      :  durbin_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Durbin recursion to find lpc coefficients        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word32 [])  R: Auto-correlation coefficients, normalized     */
/*   _ (Word16)  order : order of LPC prediction, usually 10         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32 []) spc: LPC coefficients, Q27                        */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void durbin_fx(Word32 *R, Word32 *spc, Word16 order)
{
  Word16 i, j, k,stop15=0,stop30=0;
  Word16 d1h, d1l, d2h, d2l;
  Word32 pc[ORDER];
  Word32 pc_scratch[ORDER];
  //  Word16 shift;
  Word32 Ltemp, E_long;
  Word32 E_min;
  Word40 Lacc, Lacc2;

 
  
#define T15 0x3F3F //10^1.5 in Q9
#define T30 0x7D00 //10^3.0 in Q5


  stoporder15 = 10;
  stoporder30  = 10;

#ifdef WMOPS_FX
  test();
#endif
  if (*R ==0) {
    /* if R(0)<=0, set LPC coeff. to zero */
    for (i = 0; i < order; i++)	{
#ifdef WMOPS_FX
	  move16();
#endif
      spc[i]=0;
    }

  }
  else { 
     
#ifdef WMOPS_FX
	  move16();
#endif
    E_long = R[0]; /* maintain 32-bit normalized pred. error */

#ifdef LPCGAINFIX    
    E_min = L_shr(E_long,25); /* 2**-15 = -45dB was 2**-10 = -30dB */
#else
    E_min = L_shr(E_long,10); /* 2**-15 = -45dB was 2**-10 = -30dB */
#endif    
    for(i=0; i < order; i++) {
      
#ifdef WMOPS_FX
  test();
#endif
      if( E_long < E_min ) {
#ifdef WMOPS_FX
	  move32();
#endif
	pc[i] = 0;   /* reflection coef = 0 */
      }
      else { 
	Lacc=L_deposit_l(0x0400); // Lacc loaded with rounding bit (Q11)
#ifdef WMOPS_FX
	  move32();
#endif
	Lacc2=0;
	for (j=0;j<i;j++) {
	  /* pc in Q27  */
	  d1h=extract_h(R[i-j]);
	  d1l=extract_l(R[i-j]);
	  d2h=extract_h(pc[j]);
	  d2l=extract_l(pc[j]);
	  Lacc=L_mac40_su(Lacc,d1h,d2l);
	  Lacc=L_mac40_su(Lacc,d2h,d1l);
	  Lacc2=L_mac40(Lacc2,d1h,d2h);
	}
	/* Lacc in Q11, Lacc2 in Q-4 (treat R & E as Q0)  */
	Ltemp=(Word32)L_shr40(Lacc,11);
	Lacc=L_add40(L_shl40(Lacc2,4),Ltemp); // Lacc in Q0
	Lacc=L_negate40(L_sub40(Lacc,R[i+1]));
	Ltemp=E_long;  // E_long is previous prediction error in Q0
	Lacc=divide_dp(Lacc,Ltemp,2); // Lacc is pc[i] in Q31
	pc[i]=(Word32)L_sat32_40(Lacc);
	
      } /* end else */

      /* pc[i] is now Q31 */
      d1h=extract_h(pc[i]);
      d1l=extract_l(pc[i]);
      pc[i]=L_shr(pc[i],4);  // pc[i] now in Q27

      for (j=0;j<i;j++) {
	/* pc_scratch[j] = pc[j] - pc[i] * pc[i-j-1]; */
	d2h=extract_h(pc[i-j-1]);
	d2l=extract_l(pc[i-j-1]);
	Lacc=L_deposit_l(0x4000);  // Rounding
	Lacc=L_mac40_su(Lacc,d1h,d2l);
	Lacc=L_mac40_su(Lacc,d2h,d1l);
	Lacc=L_shr40(Lacc,15);
	Lacc=L_mac40(Lacc,d1h,d2h); // Lacc in Q27
	pc_scratch[j]=(Word32)L_sat32_40(L_negate40(L_sub40(Lacc,pc[j])));
      }
      
#ifdef WMOPS_FX
  test();
#endif
  if (i != order - 1) {
    for(k = 0; k < j; k++) {
      pc[k] = pc_scratch[k];
    }
  }  /* end if */
  
      /* E = (1 - pc[i]*pc[i]) * E; */

      Lacc=L_deposit_l(0x4000); // rounding
      Lacc=L_mac40_su(Lacc,d1h,d1l);
      Lacc=L_add40(L_shr40(Lacc,15),L_shr(L_mult(d1h,d1h),1));
      // Lacc is pc[i]^2 in Q30
#ifdef WMOPS_FX
  move32();
#endif
      Ltemp=0x40000000; // Ltemp=1.0 in Q30
      Ltemp=L_sub(Ltemp,(Word32)Lacc); // Ltemp=1-pc[i]^2, Q30
      d1h=extract_h(Ltemp);
      d1l=extract_l(Ltemp);
      d2h=extract_h(E_long);
      d2l=extract_l(E_long);
      
      /* E = (1 - pc[i]*pc[i]) * E; */
      Lacc=0x2000; // rounding
      Lacc=L_mac40_su(Lacc,d1h,d2l);
      Lacc=L_mac40_su(Lacc,d2h,d1l);
      Lacc=L_add40(L_shr40(Lacc,14),L_shl(L_mult(d1h,d2h),1));

      E_long=(Word32) L_sat32_40(Lacc);

      
	  //if (E_long*T15 > R[0]) S15 = i+1;
	  Lacc=0;
	  d1h=extract_h(E_long);
	  d1l=extract_l(E_long);  
	  Lacc=L_shl40(L_mult_su(T15,d1l),1);  //T15 in Q9
	  Ltemp=(Word32)L_add40(L_shr40(Lacc,16),L_mult(d1h,T15));
	  if (L_sub(Ltemp,L_shr(R[0],6)) > 0L) {stoporder15=add(i,1);stop15=1;}
	 
	  //if (E_long*T30 > R[0]) S30 = i+1;
	
	  Lacc=0;
	  d1h=extract_h(E_long);
	  d1l=extract_l(E_long);  
	  Lacc=L_shl40(L_mult_su(T30,d1l),1);  //T30 in Q5
	  Ltemp=(Word32)L_add40(L_shr40(Lacc,16),L_mult(d1h,T30));
	  if (L_sub(Ltemp,L_shr(R[0],10)) > 0L) {stoporder30=add(i,1);stop30=1;}

	  
	  
	  
	  
    } /*end for i */

    lpcg = (Word32) L_sat32_40(divide_dp(R[0],E_long,-10)); //Q19, global var used inside pre_encode_fx for tone detection

#if 0  
    if (stop15==0) stoporder15=0;
    if (stop30==0) stoporder30=0;
#endif    
    
    /* Round predictor coefficients to 16 bits (Q12) and
       negate them for compatibility with other modules */
    spc[i-1]=pc[i-1];
	for(k=0; k < order-1; k++) {
#ifdef WMOPS_FX
	  move32();
#endif
      spc[k] = pc_scratch[k]; 
	}
    
    
  }
  /* end else */

}

/*===================================================================*/
/* FUNCTION      :  lpcanalys_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  LPC analysis -- windowing, autocorr and durbin   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) input : input speech  (Q0)                        */
/*   _ (Word16)  len   : length of window                            */
/*   _ (Word16)  order : order of LPC prediction, usually 10         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32 []) spc: LPC coefficients, Q27                        */
/*   _ (Word32 []) R:   autocorrelation of 17th order                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) : block norm factor for autocorrelation              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
Word16 lpcanalys_fx(Word32 *pc, Word16 *input, Word16 order, Word16 len,
		  Word32 *R)
{
  Word16 R_scale;

  /* autocorrelation(R, input, len, order + 1); */
  autocorrelation_fx(input, len, order + 7, R, &R_scale);
  /*...add 7 for rate determination... */

  durbin_fx(R, pc, order);
  /*
  for (i = 0; i < order + 7; i++)
    R[i] = L_shr(R[i], R_scale);
  */
  return (R_scale);
}

/*===================================================================*/
/* FUNCTION      :  polynomial_eval_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Evaluate P or Q polynomial at given frequency    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16)  f : frequency used as index for cosine table lookup */
/*                   Q9                                              */
/*   _ (Word32 [])  coef: polynomial coefficients, Q26               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word32)  polynomial value at given frequency f, Q25          */
/*===================================================================*/
/* Note: This function uses 512 entry cosine table cos_table[], Q15  */
/*===================================================================*/
/* P(w)=cos(5w)+p[1]*cos(4w)+p[2]*cos(3w)+...+p[4]*cos(w)+p[5]/2     */
/* Q(w)=cos(5w)+q[1]*cos(4w)+q[2]*cos(3w)+...+q[4]*cos(w)+q[5]/2     */
/*===================================================================*/
Word32 polynomial_eval_fx(Word16 f, Word32 *coef)
{
  Word16 i, idx;
  Word16 dh, dl, coslut;
  Word32 Ltemp;
  Word40 Lacc;

  idx=f;
  dh=extract_h(coef[5]);
  dl=extract_l(coef[5]);

#ifdef WMOPS_FX
	  move16();
#endif
  coslut=0x4000;  // coslut=0.5 in Q15
  Ltemp=L_shr(L_mult(coslut,dh),1); // Ltemp=MSW, coef[5]/2 in Q25
  Lacc=L_mult_su(coslut,dl);  // Lacc=LSW, coef[5]/2 in Q41
  
  for (i=4;i>0;i--) {
#ifdef WMOPS_FX
	  move16();
#endif
    coslut=cos_table[idx];  // coslut=cos((5-i)f), Q15
    dh=extract_h(coef[i]);
    dl=extract_l(coef[i]);

    Ltemp=L_add(Ltemp,L_shr(L_mult(dh,coslut),1));
    Lacc=L_mac40_su(Lacc,coslut,dl);
    idx+=f;
#ifdef WMOPS_FX
  test();
#endif
    if (idx>=512) idx-=512;   // modulo of 512
  }
#ifdef WMOPS_FX
	  move16();
#endif
  coslut=cos_table[idx];  // coslut=cos(5f), Q15
  Ltemp=L_add(Ltemp,L_shr(L_mult(0x400,coslut),1));  // coef[0]=1.0, Q26

  return(L_add(Ltemp,(Word32)L_shr40(Lacc,16)));
}

/*===================================================================*/
/* FUNCTION      :  root_search_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Search root of the P or Q polynomial in a given  */
/*                  interval using binary search                     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16)  low  : Low index of the interval, Q9 (0-511)        */
/*   _ (Word16)  high : High index of the interval, Q9 (0-511)       */
/*   _ (Word32 [])  coef: polynomial coefficients, Q26               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word32)  v_low: Polynomial value at low index, Q25           */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16)  root in Q15, or 0xffff if no root is found          */
/*===================================================================*/
Word16 root_search_fx(Word16 low, Word16 high, Word32 &v_low, Word32 *coef)
{
  Word16 f;
  Word32 v_high, vh;
  Word32 Ltemp;
  Word40 Lacc;

  v_high=polynomial_eval_fx(high, coef); // v_high has the value at high index

#ifdef WMOPS_FX
  test();
#endif
  if (v_high==0) {  // find the exact root
    v_low=v_high;   // Set low value for next root search call
    return(shl(high,6));  // return exact root at high in Q15
  }
  else { // No exact root found
#ifdef WMOPS_FX
  test();
  logic16();
#endif
    if ((v_high^v_low)>=0) { // No root in this interval (low high]
      v_low=v_high;
      return(-1);
    }
    else { // there is a root in this interval
      /* Do binary search for root   */
      vh=v_high;
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
      while (sub(sub(high,low),2)>=0) { // high-low>=2
	f=shr(add(high,low),1);  // f=(high+low)/2
	Ltemp=polynomial_eval_fx(f, coef);
#ifdef WMOPS_FX
  test();
#endif
	if (Ltemp==0) {
	  v_low=v_high;  // Set low value for next root search call
	  return (shl(f,6));
	}
	else {
#ifdef WMOPS_FX
  test();
  logic16();
#endif
	  if ((Ltemp^v_low)<0) { // root between f & low
	    high=f;
	    vh=Ltemp;
	  }
	  else {
	    v_low=Ltemp;
	    low=f;
	  }
	  
	}
      } // end while

      /* do interpolation for root in between high and low */
      Lacc=divide_dp(v_low, L_sub(v_low,vh),2); // Lacc in Q31
      Ltemp=(Word32)L_shr40(Lacc,9); // Ltemp =quotient*1.0 in Q31
      
      v_low=v_high;
      return(add(round32_16(Ltemp),shl(low,6)));
      
    } // end else (root in interval)
  } // end else (exact root at high)
    

}


/*===================================================================*/
/* FUNCTION      :  lpc2lsp_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convert LPC coefficients to LSP coefficients     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word32 []) a : LPC coefficients, Q27                         */
/*   _ (Word16 []) old_freq: Previous frame LSP coefficients, Q15    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) freq: LSP coefficients, Q15                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16)  flag: 1 means all 10 LSPs are found, 0 otherwise    */
/*===================================================================*/
Word16 lpc2lsp_fx(Word32 *a, Word16 *freq, Word16 *old_freq)
{
  Word16 i;
  Word16 rt, low, high, prev_rt, rc;
  Word32 p[6], q[6];  // Q26
  Word32 Ltemp, v_low;
  Word40 Lacc;
  Word16 tfreq[ORDER+1];
  /* First construct the P,Q polynomial   */
  /*    p[0] = q[0] = 1                                         */
  /*    p[i] = -lpcCoeff[i] - lpcCoeff[11-i] - p[i-1]  (1<=i<=5)*/
  /*    q[i] = -lpcCoeff[i] + lpcCoeff[11-i] + q[i-1]  (1<=i<=5)*/
  Ltemp=L_deposit_h(0x400);   // Ltemp is 1.0 in Q26
#ifdef WMOPS_FX
	  move32();
	  move32();
#endif
  p[0]=Ltemp;
  q[0]=Ltemp;

  for (i=1;i<6;i++) {
#ifdef WMOPS_FX
	  move32();
#endif
    Lacc=a[10-i];
    Lacc=L_sub40(Lacc,a[i-1]);  // Lacc=-lpcCoeff[i-1] + lpcCoeff[10-i]
    q[i]=(Word32)L_sat32_40(L_add40(L_shr40(Lacc,1),q[i-1]));

    Lacc=L_add40(Lacc,L_shl(a[i-1],1));// Lacc=lpcCoeff[i-1] + lpcCoeff[10-i]

    p[i]=(Word32)L_sat32_40(L_sub40(L_negate40(L_shr40(Lacc,1)),p[i-1]));
  }
  
  /* Search roots of the P and Q polynomials   */

  v_low=polynomial_eval_fx(0, p);
#ifdef WMOPS_FX
	  Nmove16(4);
#endif
  low=0;
  

  high=8;

  
  prev_rt=0;
  rc=0;  // root counter
  
  for (i=0;i<32;i++) {
  
    rt=root_search_fx(low, high, v_low, p);
    low=high;
    

    high=add(high,8);
   
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (sub(rt,prev_rt)>=0) {
      tfreq[rc]=rt;
      rc=add(rc,2);
    }
  
    prev_rt=add(rt,6);
   
  } // End for P roots

  
  tfreq[rc]=0x3f80;  // Set a high enough value as fake root for Q search
#ifdef WMOPS_FX
  test();
#endif
  if (sub(rc,ORDER)<0) { // lost P root
    /* copy from previous LSP and return */
	  for (i=0;i<ORDER;i++) {
#ifdef WMOPS_FX
	  move16();
#endif
		  freq[i]=old_freq[i];
	  }
    return(0);
  }
  else { // Search for Q roots between P roots
    v_low=L_deposit_h(0x800);  // Init a positive value for v_low
    low=shr(tfreq[0],6);

    high=add(shr(tfreq[2],6),1);
   
    rc=1;
    for (i=0;i<ORDER/2;i++) {
      rt=root_search_fx(low, high, v_low, q);

#ifdef WMOPS_FX
  test();
#endif
      if (rt<0) { // No Q root in this interval
	/* copy from previous LSP and return */
		  for (i=0;i<ORDER;i++) {
#ifdef WMOPS_FX
	  move16();
#endif
			  freq[i]=old_freq[i];
		  }
	return(0);
      }
      else {
#ifdef WMOPS_FX
	  move16();
#endif
	tfreq[rc]=rt;
	rc=add(rc,2);
	low=shr(tfreq[rc-1],6);

        high=add(shr(tfreq[rc+1],6),1);
	
      } // end else, find Q root
    } // end for 
  } // end else

  for(i=0;i<ORDER;i++)
    freq[i]=tfreq[i];
  
  return (1);
  
}

/*===================================================================*/
/* FUNCTION      :  poscos_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute cosine by approximation                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16)    w: angle in radians/pi, Q14 (0<=w<=16384)         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _             None                                              */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _             None                                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word32)  :  -cos(w) in Q30                                   */
/*===================================================================*/
/* NOTE: This function uses 4 coefficients in the approx formula     */
/* Approx Formula: cos(w) = z + c1*z^3 + c2*z^5 + c3*z^7 + c4*z^9    */
/*     where z = pi/2 - w, c1=-.1666665668, c2=.8333025139E-2,       */
/*           c3=-.198074182E-3, and c4=.2601903036E-5                */
/*===================================================================*/
Word32 poscos_fx(Word16 w)
{
  Word16 a, z, z2, z3, z5, z7, z9;
  Word32 Ltemp;
  Word40 Lacc;

#ifdef WMOPS_FX
  test();
#endif
  if (w==0) return (0xc0000004);  // -1.0 in Q30
  else {
#ifdef WMOPS_FX
	  move16();
#endif
    z=0x2000;    // z=0.5 in Q14
    z=shl(sub(z,w),2);  // z = 0.5-w in Q16
#ifdef WMOPS_FX
	  move16();
#endif
    a=0x6488;           // a=pi in Q13
    z=mult_r(z,a);      // z=pi*(0.5-w), Q14
    z2=mult_r(z,z);     // z2=z^2, Q13
    z3=round32_16(L_shl(L_mult(z,z2),1)); // z3=z^3, Q13
    z5=mult_r(z2,z3);   // z5=z^5, Q11
    z7=round32_16(L_shl(L_mult(z2,z5),1)); // z7=z^7, Q10
    z9=round32_16(L_shl(L_mult(z2,z7),1)); // z9=z^9, Q9
    
    Lacc=L_mult(z9,cos_coef[0]); // Lacc=c4*z^9, Q38, c4 in Q28
    Lacc=L_mac40(Lacc,z7,cos_coef[1]); // c3 in Q27
    Ltemp=L_mult(z3,cos_coef[3]); // Ltemp=c1*z^3, Q31, c1 in Q17
    Lacc=L_add40(L_shr40(Lacc,8),L_shr(Ltemp,1)); // Lacc=c1*z^3+c3*z^7+c4*^z9, Q30
    Lacc=L_add40(Lacc,L_deposit_h(z));
    Ltemp=L_mult(z5,cos_coef[2]); // Ltemp=-c2*z^5, Q31, cos_coef[2]=-c2 in Q19
    Lacc=L_sub40(Lacc,L_shr(Ltemp,1)); // Lacc=cos(w) in Q30
    
    return((Word32)L_sat32_40(L_negate40(Lacc)));  // return -cos(w), Q30
    
  }  // end else
}

/*===================================================================*/
/* FUNCTION      :  lsp_convolve_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convolution of LSP polynomial for LSP to LPC     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32[]) p1 : 2nd entry of input sequence, Q24              */
/*                     1st entry is 1.0, Q24                         */
/*   _ (Word32)   x :  -2cos(w) in Q24                               */
/*   _ (Word16)   len: length of output-2                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32[]) p2 : 2nd entry of output sequence, Q24             */
/*                     1st entry is 1.0, Q24                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _             None                                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _             None                                              */
/*===================================================================*/
/* NOTE: Convolves (1 S 1) where S is the -2cos(w) value) with the   */
/* long sequence where the input long sequence on consecutive calls  */
/* is (1 X1 1), (1 X1 X2 X1), (1 X1 X2 X3 X2), (1 X1 X2 X3 X4 X3).   */
/* (Since the sequence is symmetric, we only need to output one      */
/*   entry beyond the center point for the next iteration).          */
/* The 1st entry of the convolution is 1, the 2nd is X1+S1, and      */
/*    the rest have the form X(i-1) + X(i)*S + X(i+1).               */
/* Final output sequence is (1 X1 X2 X3 X4 X5 X4)                    */
/*===================================================================*/
void lsp_convolve_fx(Word32 x, Word32 *p1, Word32 *p2, Word16 len)
{
  Word16 i, d1h, d1l, d2h, d2l;
  Word32 Ltemp;
  Word40 Lacc;
  
  d1h=extract_h(x);  // d1h in Q8
  d1l=extract_l(x);  // d1l in Q24

  Ltemp=L_add(x,p1[0]); // first output is p1[0]+x, Q24
#ifdef WMOPS_FX
	  move32();
#endif
  p2[0]=Ltemp;
  Ltemp=L_deposit_h(0x100);  // Ltemp=1.0, Q24
  for (i=0;i<len;i++) {
    Ltemp=L_add(Ltemp,p1[i+1]); // Ltemp2=p1[i-1]+p1[i+1], Q24
    d2h=extract_h(p1[i]);
    d2l=extract_l(p1[i]);
    Lacc=L_mult_su(d1h,d2l);
    Lacc=L_mac40_su(Lacc,d2h,d1l);
    Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(d1h,d2h),1));
    // Lacc=p1[i]* x, Q16
    Lacc=L_add40(L_shl40(Lacc,8),Ltemp); // Lacc=p1[i-1]+p1[i+1]+p1[i]*x, Q24
    p2[i+1]=(Word32)L_sat32_40(Lacc);
#ifdef WMOPS_FX
	  move32();
#endif
    Ltemp=p1[i];
  } // end for
#ifdef WMOPS_FX
	  move16();
#endif
  p2[i+1]=p2[i-1];
}

/*===================================================================*/
/* FUNCTION      :  compute_poly_product_fx ()                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute polynomial product for P(z) for LSP      */
/*                  to LPC conversion                                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16[]) coef : LSP coefficients, Q15                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32[]) p :  output sequence, Q24                          */
/*                     1st entry is always 1.0, Q24                  */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _             None                                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _             None                                              */
/*===================================================================*/
/* NOTE:                                                             */
/* P(z)=(1+z^-1)*PI(j=1to5) (1 - 2*z^-1*cos(2*pi*w(2j-1)) + z^-2)    */
/* Q(z)=(1-z^-1)*PI(j=1to5) (1 - 2*z^-1*cos(2*pi*w(2j)) + z^-2)      */
/*===================================================================*/
void compute_poly_product_fx(Word16 *coef, Word32 *p)
{
  Word16 i, len;
  Word32 lspcos[ORDER/2];
  Word32 p2[7];  // intermediate product, Q24
  Word32 *p_in, *p_out, *p_temp;

  for (i=0;i<ORDER/2;i++) {
    lspcos[i]=poscos_fx(coef[i*2]);  // lspcos =-cos(lsp) in Q30
  }
  /* Set up first polynomial for convolution (1 -2cos(w5) 1)  */
  /* First element of output is 1, all in Q24                 */
  p[0]=L_deposit_h(0x100); // 1.0 in Q24
  p[2]=L_deposit_h(0x100); // 1.0 in Q24
  p[1]=L_shr(lspcos[ORDER/2-1],5); // p2[1]=-2cos(w5), Q24
  p2[0]=L_deposit_h(0x100); // 1.0 in Q24

#ifdef WMOPS_FX
	  move16();
#endif
  len=1;
  p_in=p+1;   // 2nd entry of input sequence
  p_out=p2+1; // 2nd entry of output sequence
  for (i=0;i<4;i++) {
    lsp_convolve_fx(L_shr(lspcos[i],5), p_in, p_out, len);
#ifdef WMOPS_FX
	  move16();
#endif
    p_temp=p_in;
    p_in=p_out;
    p_out=p_temp; // swap input/output buffer
    len=add(len,1);
  }
}

/*===================================================================*/
/* FUNCTION      :  lsp2lpc_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convert LSP coefficients to LPC coefficients     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) freq: LSP coefficients, Q15                       */
/*   _ (Word16 []) prev_a : previous frame LPC coefficients, Q12     */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) a : LPC coefficients, Q12                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _  None                                                         */
/*===================================================================*/
void lsp2lpc_fx(Word16 *freq, Word16 *prev_a, Word16 *a)
{
  Word16 i;
  Word32 pq[7];
  Word32 p[5], q[5];
  Word32 Ltemp;
  Word40 Lacc;

  Word16 tmp_pci[ORDER];

  for(i=0;i<ORDER;i++)
    tmp_pci[i]=prev_a[i];
  

  compute_poly_product_fx(freq, pq);
  giOverflow=0;  // clear overflow flag
  for (i=0;i<ORDER/2;i++) {
    Ltemp=L_add(pq[i],pq[i+1]);
    if (giOverflow==1) break;
#ifdef WMOPS_FX
	  move32();
#endif
    p[i]=Ltemp;
  }
  
  if (giOverflow==1) {
    for (i=0;i<ORDER;i++) {
      a[i]=mult_r(prev_a[i],pwAlpha[i]);
    }
    
    return;
  }

  compute_poly_product_fx(freq+1, pq);

  for (i=0;i<ORDER/2;i++) {
    Ltemp=L_sub(pq[i+1],pq[i]);
    if (giOverflow==1) break;
#ifdef WMOPS_FX
	  move32();
#endif
    q[i]=Ltemp;
  }

  if (giOverflow==1) {
    for (i=0;i<ORDER;i++) {
      a[i]=mult_r(prev_a[i],pwAlpha[i]);
    }
  }
  else {
    for (i=0;i<ORDER/2;i++) {
      Lacc=L_add40(p[i],q[i]);  // p[i], q[i] in Q24
      Lacc=L_negate40(Lacc);    // Lacc=-(p[i]-q[i])/2 in Q25
      Lacc=L_add40(L_shl40(Lacc,3),0x08000);  // rounding
      a[i]=extract_h((Word32)L_sat32_40(Lacc)); // a[i] in Q12
      if (giOverflow==1) break;
    }
    for (i=0;i<ORDER/2;i++) {
      Lacc=L_sub40(q[i],p[i]);
      Lacc=L_add40(L_shl40(Lacc,3),0x08000);  // rounding
      a[ORDER-1-i]=extract_h((Word32)L_sat32_40(Lacc));
      if (giOverflow==1) break;
    }
  }

  if (giOverflow==1) {
    for (i=0;i<ORDER;i++) {
      a[i]=mult_r(tmp_pci[i],pwAlpha[i]);
    }
  }
  

}

/*===================================================================*/
/* FUNCTION      :  Interpol_fx ()                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Interpolate LSP coefficients                     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) lsp_old:  previous frame LSP coefficients, Q15    */
/*   _ (Word16 []) lsp_curr: current frame LSP coefficients, Q15     */
/*   _ (Word16)    k : subframe number                               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) lsp_out  : Intepolated LSP coefficients, Q15      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void  Interpol_fx(Word16 *lsp_old, Word16 *lsp_curr, Word16 *lsp_out,
		  Word16 k)
{
  Word16 i;
  Word16 c1, c2;
  Word32 Ltemp1, Ltemp2;

#ifdef WMOPS_FX
	  move16();
#endif
  c1=lsp_int_tab[k];
#ifdef WMOPS_FX
	  move32();
#endif
  Ltemp1=0x08000; // Ltemp1=1.0, Q15
  Ltemp2=L_deposit_l(c1);
  Ltemp1=L_sub(Ltemp1,Ltemp2);  // Ltemp1=1.0-c1, Q15
  c2=extract_l(Ltemp1);  // c2=1.0-c1, Q15
  for (i=0;i<ORDER;i++) {
    Ltemp1=L_mult(lsp_curr[i],c1);
    Ltemp1=L_mac(Ltemp1,lsp_old[i],c2); // Ltemp1=c1*lsp[i]+c2*lsp_old[i]
    lsp_out[i]=round32_16(Ltemp1);
  }
}

