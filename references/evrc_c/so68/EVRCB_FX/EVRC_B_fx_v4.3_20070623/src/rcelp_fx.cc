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
/*  Adaptive Codebook Excitation functions                              */
/*======================================================================*/
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "rcelp_tab.h"
#include "ppvq_fx.h"
#include "proto_fx.h"
#include "globs_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX

Word16 dp_global;

/*===================================================================*/
/* FUNCTION      :  Interpol_delay_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Interpolate pitch lag for a subframe             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) delay_old:  previous frame delay, Q0                 */
/*   _ (Word16) delay_curr: current frame delay, Q0                  */
/*   _ (Word16)    k : subframe number                               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word32 []) delay_out  : 3 Intepolated delays, Q16            */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: this function uses a 5 entry table (Q15 unsigned)           */
/*===================================================================*/
void Interpol_delay_fx(Word16 delay_old, Word16 delay_curr,
		       Word32 *delay_out, Word16 k)
{
  Word16 i;
  Word16 c1,c2;
  Word32 Ltemp1, Ltemp2;

#ifdef WMOPS_FX
  move32();
#endif
  Ltemp1=0x08000;  // Ltemp1=1.0 in Q15
  for (i=0;i<3;i++) {
#ifdef WMOPS_FX
  move16();
#endif
    c1=delay_int_tab[k+i];  // c1=1-f in Q15 unsigned
    Ltemp2=L_deposit_l(c1)&0x0000FFFF;  // remove sign bits
    c2=extract_l(L_sub(Ltemp1,Ltemp2));
    
    Ltemp2=(Word32) L_mult_su(delay_old,c1);
    Ltemp2=(Word32)L_mac40_su(Ltemp2,delay_curr,c2);
    delay_out[i]=L_shl(Ltemp2,1);  // interpolated output in Q16
  }
}

/*===================================================================*/
/* FUNCTION      :  delay_contour_fx ()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Construct a delay contour for ACB generation     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32 []) delay3: Interpolated delays, Q16                  */
/*   _ (Word16)    len: subframe length                              */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word32 []) delay_out  : Intepolated delays, Q24              */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: this function uses a 2 entry table (Q36)                    */
/*===================================================================*/
/* Formula:     Tc(n)=d(m,0)+n*(d(m,1)-d(m,0))/L      n:0-L          */
/*              Tc(n)=d(m,1)+(n-L)*(d(m,2)-d(m,1))/L  n:L-(L+10)     */
/*                                                                   */
/*              m is the subframe number, L is the subframesize      */
/*              d(m,j) is the interpolated delays                    */
/*===================================================================*/
/*===================================================================*/
void delay_contour_fx(Word32 *delay3, Word16 len, Word32 *delay_out)
{
  Word16 i, j, k;
  Word16 d1h, d1l, d2h, d2l;
  Word32 Ltemp1, Ltemp2;
  Word40 Lacc;

  d1h=extract_h(inv_sflen_tab[len-(SubFrameSize-1)]); // MSW of 1/len in 20
  d1l=extract_l(inv_sflen_tab[len-(SubFrameSize-1)]); // LSW of 1/len in Q36

  for (i=0, k=0; i<2; i++) {  // k=0 used as output index
    Ltemp1=L_sub(delay3[i+1],delay3[i]);  // Ltemp1 in Q16
    Ltemp1=L_shl(Ltemp1,3);   // Ltemp1 in Q19
    
    d2h=extract_h(Ltemp1);  // Q3
    d2l=extract_l(Ltemp1);  // Q19

    Ltemp1=L_shl(delay3[i],8);  // Ltemp1 = delay3[i], Q24
    // compute 1/len*(delay3[i+1]-delay3[i]) in Q24
    Ltemp2=(Word32)L_shr40(L_mult_uu(d1l,d2l),15); // LSW*LSW, Q19+Q36-15=Q40
    Lacc=L_add40(L_shl40(L_mult_su(d1h,d2l),1),Ltemp2);
    Lacc=L_add40_40(Lacc,L_shl40(L_mult_su(d2h,d1l),1));
    Ltemp2=(Word32)L_mac40(L_shr40(Lacc,16),d1h,d2h); // Lacc in Q24, this is delta

    for (j=0; j<len; j++, k++) {
#ifdef WMOPS_FX
  move32();
#endif
      delay_out[k]=Ltemp1;
      Ltemp1=L_add(Ltemp1,Ltemp2);  // delay_out+=delta
    }
    len=10;
  }
}

/*===================================================================*/
/* FUNCTION      :  rclp_block_interp_fx ()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Block interpolate samples of the ACB excitation  */
/*                  with block size 3 (7) or 8 (17)                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32 []) delay: Interpolated delay contour Tc(n), Q24      */
/*   _ (Word16 []) intrp_coef: Interpolation coefficients, Q15       */
/*   _ (Word16)    N : Interpolation size, 3 or 8                    */
/*   _ (Word16)    len: number of samples                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 [])  resid: input/output residual E(n), Q0            */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: this function uses one of the two interpolation tables(Q15) */
/*===================================================================*/
/*     TE(n)=round(Tc(n))                                            */
/*     TIE(n)=trunc(8*(TE(n)-Tc(n))+4.5)                             */
/*     If (TIE(n)==8){ TIE(n)=0,TE(n)=TE(n)-1; }                     */
/*                                                                   */
/*     E(n)=Sum(i=0 to 16 or 6){                                     */
/*             E(n+i-TE(n)-fl) * I_E(i+(2fl+1)*TIE(n))               */
/*          }        for n=0 to L+10                                 */
/*     fl is either 3 or 8, L=53 or 54                               */
/*===================================================================*/
void rclp_block_interp_fx(Word16 *resid, Word32 *delay, Word16 *intrp_coef,
			  Word16 N, Word16 len)
{
  Word16 i, j;
  Word16 wl, TE, Coff, Ioff, Tidx;
  Word32 Ltemp1, T0, TT;
  Word40 Lacc;

  wl=add(shl(N,1),1);  // wl=2*N+1  
#ifdef WMOPS_FX
  move32();
  move32();
#endif
  TT=0x000000ff;  // tt=0x00ff, Q24 for rounding up
  T0=0x00048000;  // T0=4.5 in Q16

  for (i=0; i<len; i++) {      // loop through 63 or 64 samples
                               // i is output index 
    Ltemp1=L_add(delay[i],TT); // Ltemp1=Tc[i]+0x00ff, Q24
    Ltemp1=L_shr(Ltemp1,8);    // Ltemp1=Tc[i], Q16
    TE=round32_16(Ltemp1);     // TE(n)=round(Tc(i))
    Ltemp1=L_sub(Ltemp1,L_deposit_h(TE));  // Ltemp1=Tc(n)-TE(n) Q16
    /* find index to one of 8 interpolation functions in table  */
    Ltemp1=L_sub(T0,L_shl(Ltemp1,3));  // Ltemp1=4.5+8*(TE(n)-Tc(n)), Q16
    Tidx=extract_h(Ltemp1);            // index of interpolation function
    Coff=extract_l(L_shr(L_mult(wl,Tidx),1)); // Coff(i)=(2*N+1)*Tidx

    Ioff=add(TE,N);                    // Ioff=N+TE(n)
    Ioff=negate(Ioff);                 // Ioff=-N-TE(n);

#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (Tidx==8) {
      Coff=0;
      Ioff=add(Ioff,1);
    }  
    
    /* Interpolation loop  */
    for (j=0, Lacc=0;j<wl;j++) {
      Lacc=L_mac40(Lacc,resid[i+j+Ioff],intrp_coef[j+Coff]);
    }
    Lacc=L_add40(Lacc, 0x00008000);  // rounding
    resid[i]=extract_h((Word32)L_sat32_40(Lacc));
  }
}


/*===================================================================*/
/* FUNCTION      :  rclp_fixed_block_interp_fx ()                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Block interpolate samples of the ACB excitation  */
/*                  with block size 3 (7) or 8 (17) and fixed delay  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  resid : input residual E(n), Q0                  */
/*   _ (Word16)     delay : fixed delay value, Q8                    */
/*   _ (Word16 []) intrp_coef: Interpolation coefficients, Q15       */
/*   _ (Word16)    N : Interpolation size, 3 or 8                    */
/*   _ (Word16)    len: number of samples                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) resid_out: interpolated residual output, Q0       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: this function uses one of the two interpolation tables(Q15) */
/*===================================================================*/
/*     TE=round(Tshift)                                              */
/*     TIE=trunc(8*(TE-Tshift)+4.5)                                  */
/*     If (TIE==8){ TIE=0,TE=TE-1; }                                 */
/*                                                                   */
/*     E(n)=Sum(i=0 to 16 or 6){                                     */
/*             E(n+i-TE-fl) * I_E(i+(2fl+1)*TIE)                     */
/*          }        for n=0 to L                                    */
/*     fl is either 3 or 8, L=53 or 54                               */
/*===================================================================*/
void rclp_fixed_block_interp_fx(Word16 *resid, Word16 *resid_out,
				Word16 delay, Word16 *intrp_coef,
				Word16 N, Word16 len,Word16 *over_bound)
{
  Word16 i,j;
  Word16 wl, TE, T0, Tidx, Coff, Ioff;
  Word40 Lacc;

#ifdef WMOPS_FX
  move16();
#endif
  T0=0x0090;           // T0=4.5, Q5
  wl=add(shl(N,1),1);  // wl=2*N+1, interpolation window length
  TE=round32_16(L_shr(L_deposit_h(delay),8)); // TE=round(delay), Q0

  Ioff=add(TE,N);
  Ioff=negate(Ioff);   // Ioff=-TE-N

  Tidx=sub(delay,shl(TE,8));  
  Tidx=shr(sub(T0,Tidx),5);   // Tidx=4.5+8*(TE-delay), Q0

  Coff=extract_l(L_shr(L_mult(wl,Tidx),1)); // Coff=(2*N+1)*Tidx
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Tidx==8) {
    Coff=0;
    Ioff=add(Ioff,1);
  }
  
#if 1 /* to prevent array bound reads */
    *over_bound = sub(add(add(add(Ioff,len),wl),dp_global),133);
    
    if (*over_bound > 0)
      len=sub(len,*over_bound);
     else
       *over_bound=0; 

#endif  
  

  for (i=0; i<len; i++) {  // i is also the output index
    for (j=0, Lacc=0; j<wl; j++) {
      Lacc=L_mac40(Lacc,resid[i+j+Ioff],intrp_coef[j+Coff]);
    }
    
    Lacc=L_add40(Lacc, 0x00008000);  // rounding
    resid_out[i]=extract_h((Word32)L_sat32_40(Lacc));
  }
}


/*===================================================================*/
/* FUNCTION      :  maxeloc_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Find the offset that maximizes energy estimates  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  signal : input E(n), Q0                          */
/*   _ (Word16)     offset: offset of current segment, Q0            */
/*   _ (Word16)     len: number of samples                           */
/*   _ (Word16)     WL: (L-1)/2, L is length of window               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) loc : location of the energy maxima                  */
/*===================================================================*/
Word16 maxeloc_fx(Word16 *signal, Word16 offset, Word16 len, Word16 WL)
{
  Word16 i;
  Word16 loc, L;
  Word40 Lmax, Lacc;

  L=add(shl(WL,1),1); // L=2*WL+1, length of energy window
#ifdef WMOPS_FX
  move16();
#endif
  loc=0;

  for (i=0,Lmax=0; i<L; i++) {
    Lmax=L_mac40(Lmax,signal[i+offset-WL],signal[i+offset-WL]);
  }

  Lacc=Lmax;  // init for recursive energy calculation
  for (i=1; i<len; i++) {
    Lacc=L_mac40(Lacc,signal[i+offset+WL],signal[i+offset+WL]);
    Lacc=L_msu40(Lacc,signal[i+offset-WL-1],signal[i+offset-WL-1]);
#ifdef WMOPS_FX
  test();
  move16();
  move32();
#endif
    if (L_sub40_40(Lmax,Lacc)<0) {
      loc=i;
      Lmax=Lacc;
    }
  }
  return (add(loc,offset));
}

/*===================================================================*/
/* FUNCTION  :  cshiftframe_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE   :  Compute the start and end positions of the           */
/*              segment to be shifted.  It also computs the max      */
/*              shift range within which the segment can be shifted  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  resid : input E(n), Q0                           */
/*   _ (Word16)     iacshift: integer shift trunc(-accshift+0.5), Q0 */
/*   _ (Word16)     dpm: last modified position, Q0                  */
/*   _ (Word16)     guard: remaining segment length, Q0              */
/*   _ (Word32)     delay:  interpolated delay, Q16                  */
/*   _ (Word16)     subframel: length of current subframe, Q0        */
/*   _ (Word16)     extra: # of samples beyond current subframe, Q0  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16)     sfstart : start position of segment, Q0          */
/*   _ (Word16)     sfend : end position of segment, Q0              */
/*   _ (Word16)     maxshift : shift flag, 3 or 0, Q0                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16)     sl : sfend-sfstart, Q0                           */
/*===================================================================*/
Word16 cshiftframe_fx(Word16 *resid, Word16 iacshift, Word16 dpm, 
		    Word16 guard, Word32 delay, Word16 subframel,
		    Word16 extra, Word16& sfstart, Word16& sfend,
		    Word16& maxshift)
{
  Word16 offset;
  //  Word16 iacshift;
  Word16 length;
  Word16 loc, loc2, g;
  Word16 bg,diff=0;

  bg = add(guard, 2-(GUARD + FrameSize + LOOKAHEAD_LEN));
  /*
  if (accshift > 0)
    iacshift = sub(1, accshift);
  else
    iacshift = negate(accshift);  // This is computed outside and passed in
  */
  /*
  if (delay < 0)  // this is redundant, since delay always >0
    iacshift = negate(iacshift);
  */
  /* determine first a pitch pulse somewhere near dpm */
  length = shr(extract_h(L_sub(L_shl(delay, 2), delay)), 1);  // 1.5 *delay

  offset = add(dpm, iacshift);
  offset = sub(offset, shr(extract_h(delay), 2));  // dpm+iacshift-0.25*delay
#ifdef WMOPS_FX
  test();
#endif
  if (sub(offset,bg)<0) offset=bg; // keep it within array

  g = sub(sub(guard, offset), 2);   /* Maximum allowed search size */

#ifdef WMOPS_FX
  test();
#endif
  if (sub(length,g)>0) length = g;

#if 1 /* to prevent array bound reads */
  diff=sub(add(offset,2),133);
  
  if (diff>0)
    offset=sub(offset,diff);
  
#endif 

  loc=maxeloc_fx(resid, offset, length, 2);
  
  loc = sub(loc, iacshift);
  
  /* now find the first pitch pulse for sure */
#ifdef WMOPS_FX
  test();
#endif
  if (sub(loc,dpm)<0) {
    offset = round32_16(L_sub(delay,L_shr(delay,2)));// offset=round(0.75*delay)
    offset = add(offset, iacshift);
    offset = add(offset, loc);
#ifdef WMOPS_FX
  test();
#endif
    if (sub(offset,bg)<0) offset=bg; // keep it within array
    length = shr(extract_h(delay), 1);
    g = sub(sub(guard, offset), 2);	/* Maximum allowed search size */

#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (sub(length,g)>0) length = g;

    
#if 1 /* to prevent array bound reads */
    diff=sub(add(offset,2),133);
    
    if (diff>0)
      offset=sub(offset,diff);

#endif    
    
    
    loc=maxeloc_fx(resid, offset, length, 2);
    loc = sub(loc, iacshift);
  }

#ifdef WMOPS_FX
  test();
#endif
  if (sub(loc,add(dpm,extract_h(delay)))>0) {
    offset = round32_16(L_add(delay,L_shr(delay,2)));
    offset = sub(iacshift, offset);
    offset = add(offset, loc);
#ifdef WMOPS_FX
  test();
#endif
    if (sub(offset,bg)<0) offset=bg; // keep it within array

    length = shr(extract_h(delay), 1);

    g = sub(sub(guard, offset), 2);	/* Maximum allowed search size */

#ifdef WMOPS_FX
  test();
#endif
    if (sub(length, g)>0) length = g;

    loc2=maxeloc_fx(resid, offset, length, 2);

    loc2 = sub(loc2, iacshift);
#ifdef WMOPS_FX
  test();
#endif
    if (sub(loc2,dpm)>=0) loc = loc2;
  }
  
#ifdef WMOPS_FX
  move16();
#endif
  sfstart = dpm;
  sfend = add(loc, extra);
  maxshift = RSHIFT;
#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if (sub(loc,sub(subframel, shr(extra, 1)))<0 && sub(sfend,subframel)>0)
    sfend = subframel;
#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if (sub(loc,add(subframel, shr(extra, 1)))<0 && sub(sfend,add(subframel, extra))>0)
    sfend = add(subframel, extra);
#ifdef WMOPS_FX
  test();
#endif
  if (sub(loc,add(subframel, shr(extra, 1)))>=0)
    sfend = subframel;

#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
  if (sub(loc,sfend)>=0 || sub(loc,sfstart)<0)
    maxshift = 0;
  return (sub(sfend,sfstart));
}

/*===================================================================*/
/* FUNCTION      :  getgain_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Find the adaptive codebook gain and index,       */
/*                  apply the quantized ACB gain to ACB excitation   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  mresid: modified weighted speech, Q0             */
/*   _ (Word16 [])  h: impulse response, Q11                         */
/*   _ (Word16)     sflen: length of subframe                        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16)     lambda: quantized ACB gain, Q14                  */
/*   _ (Word16)     g_idx:  ACB gain index (0-7)                     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 [])  excit : ACB excitation/ACB contribution, Q0      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: this function uses 2 small tables for ACB gain quantization */
/*===================================================================*/
/* This implementation deviates from DSP (non-bit-exact) but matches */
/* more closely to floating point                                    */
/*===================================================================*/
void getgain_fx(Word16 *excit, Word16 *mresid, Word16 *h,
		Word16 sflen, Word16& lambda, Word16& g_idx)
{
  Word16 i;
  Word16 temp[SubFrameSize];
  Word16 n1, n2, ndiff, d1h, d1l;
  Word32 ene, Ltemp;
  Word40 Lacc;
  
  ConvolveImpulseR_fx(excit, h, temp, SubFrameSize, sflen);

  for (i=0,Lacc=0;i<sflen;i++) {
    Lacc=L_mac40(Lacc,temp[i],temp[i]); // Lacc = energy of filtered ACB, Q1
  }
  n1=norm32_l40(Lacc);
  n1=sub(n1,1);  // n1'=n1-1, leave 1 more bit headroom
  ene=(Word32)L_shl40(Lacc,n1); // ene in Q(n1)
  for (i=0,Lacc=0;i<sflen;i++) {
    Lacc=L_mac40(Lacc,temp[i],mresid[i]); // Lacc=cross correlation, Q1
  }
  n2=norm32_l40(Lacc);

  ndiff=sub(n2,n1); // ndiff=n2-(n1-1)
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Lacc<=0) {  // if xcor<=0, set acb gain to 0
    g_idx=0;
  }
  else {
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (ndiff<0) { // lambda >=2.0
      g_idx=7;
    }
    else {
      // ACB gain quantization search
      Lacc=L_shl40(Lacc,n1); // normalize xcor by n1
      d1h=extract_h(ene);    // d1h has 2 sign bits
      d1l=extract_l(ene);    // d1h, d1l is MSW, LSW of energy
      g_idx=0;
      for (i=1;i<8;i++) {
	Ltemp=(Word32)L_shr40(L_mult_uu(d1l,ppvq_mid_fx[i-1]),15);  // mid gain in Q15
	Ltemp=L_add(Ltemp,L_shl((Word32)L_mult_su(d1h,ppvq_mid_fx[i-1]),1));
	// Ltemp=mid_gain*ene
#ifdef WMOPS_FX
  test();
#endif
	if (L_sub40(Lacc,Ltemp)>0) g_idx=i;
      }
    }
  }
#ifdef WMOPS_FX
  move16();
#endif
  lambda=ppvq_fx[g_idx];  // quantized ACB gain in Q14

  for (i=0;i<sflen;i++) {
    // apply quantized gain to ACB
    excit[i]=round32_16(L_shl(L_mult(excit[i],lambda),1));
  }
}

/*===================================================================*/
/* FUNCTION      :  putacbc_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generate unscaled adaptive codebook contribution */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32 [])  delay3: Interpolated delays, Q16                 */
/*   _ (Word16 [])  intrp_coef: Interpolation coefficients, Q15      */
/*   _ (Word16)     sflen: length of subframe                        */
/*   _ (Word16)     N: Interpolation size, 3 or 8                    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 [])  excit : ACB memory/ACB contribution, Q0          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
void putacbc_fx(Word16 *excit, Word16 *intrp_coef, Word32 *delay3,
		Word16 sflen, Word16 N)
{
  Word32 delay[SubFrameSize+10];
  
  delay_contour_fx(delay3, sflen, delay);
  rclp_block_interp_fx(excit, delay, intrp_coef, N, sflen+10);
}

/*===================================================================*/
/* FUNCTION      :  acb_excitation_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generate adaptive codebook contribution          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32 [])  delay3: Interpolated delays, Q16                 */
/*   _ (Word16)     gain: Adaptive codebook gain, Q14                */
/*   _ (Word16)     sflen: length of subframe                        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 [])  excit : ACB memory/ACB contribution, Q0          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
void acb_excitation_fx(Word16 *excit, Word32 *delay3, Word16 gain, Word16 sflen)
{
  Word16 i;
  Word32 delay[SubFrameSize+10];

  delay_contour_fx(delay3, sflen, delay);
  rclp_block_interp_fx(excit, delay, intrp_tab8, 8, sflen+10);
  
  for (i=0; i<sflen; i++) 
    excit[i]=round32_16(L_shl(L_mult(excit[i],gain),1));
}

/*===================================================================*/
/* FUNCTION      :  PeakToAverage_fx ()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compare peak energy to average energy ratio with */
/*                  a threshold and modify shift decision, to ensure */
/*                  the frame contains a valid pulse.                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  resid: residual input, Q0                        */
/*   _ (Word16)     len: length of residual segment                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16)     maxshift : shift flag, 3 or 0, Q0                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: len must be greater than 5, otherwise output is not valid   */
/*===================================================================*/
void PeakToAverage_fx(Word16 *resid, Word16& maxshift, Word16 len)
{
  Word16 i;
  Word16 front;
  Word16 se;
  Word16 coef, d1h, d1l, d2h, d2l;
  Word32 energy, Ltemp;
  Word40 Lacc, maxener;

#ifdef WMOPS_FX
  move16();
#endif
  front=4;
  for (i=0,Lacc=0; i<=front; i++) {  // compute 1st energy
    Lacc=L_mac40(Lacc,resid[i],resid[i]); // Q1
  }
  energy=(Word32)L_sat32_40(L_shr40(Lacc,1)); // saturate, Q0
  maxener=Lacc;  // maxener in Q1
  for (i=0; i<len-5; i++) { // loop to find max energy
    front=add(front,1);
    Lacc=L_msu40(Lacc,resid[i],resid[i]);
    Lacc=L_mac40(Lacc,resid[front],resid[front]); // Lacc=next energy, by recursive, Q1


   
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub40_40(Lacc,maxener)>0) maxener=Lacc;
  }
  // maxener in Q1
  maxener=L_sat32_40(L_shr40(maxener,1)); // saturate maxener, Q0
  
  se=norm_l((Word32)maxener);  // se is norm factor for max energy
  //  maxener=L_shl((Word32)maxener,se);
  energy=L_shl(energy,se);  // normalize energy to make sure no overflow

  se=sub(se,1);  // compensate for L_mult's left shift 1
  /* Now find the moving average energy */
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  front=5; // initial pointer offset
  coef=0xe000; // 0.875 in Q16 (unsigned), also -0.125 in Q16
  for (i=0; i<len-5; i++, front++) { // loop to find max energy
    Ltemp=L_mult(resid[front],resid[front]);  // Ltemp=resid[front]^2, Q1
    Ltemp=L_shl(Ltemp,se);  // align with energy's Q factor
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub(energy,L_shr(Ltemp,2))>0) {
      /* Do moving average energy update, energy=0.875*energy+.125*Ltemp */
      d1h=extract_h(Ltemp);
      d1l=extract_l(Ltemp);
      d2h=extract_h(energy);
      d2l=extract_l(energy);
      Lacc=L_mult_uu(coef,d2l);
      Lacc=L_sub40(Lacc,(Word32)L_mult_su(coef,d1l)); // Lacc=LSW(0.875*energy+0.125*Ltemp)
      Lacc=L_mac40_su(L_shr40(Lacc,16),d2h,coef);
      Lacc=L_sub40(Lacc,L_shr(L_mult(coef,d1h),1));
      energy=(Word32)Lacc;  // Lacc must be in 32 bit range due to moving average
    }
  }
  /* Now compare maxener/energy with threshold, they have the same Q factor  */
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (energy==0) maxshift=0;
  else {
  /* if ((maxener/energy)*(54/len)<16) maxshift=0;                           */
  /* i.e. compare maxener with energy*16/54*len                              */
    //    d1h=0x012F;
    //    d1l=0x684C;  // 16/54 in Q26
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    d1h=0x0109;
    d1l=0x7B42;   // 14/54, Q26
    Ltemp=L_mult(d1h,len); // Q11
    Ltemp=L_add(L_shl(Ltemp,15),(Word32)L_mult_su(len,d1l));  // Ltemp=16/54*len, Q26
    d1h=extract_h(Ltemp);
    d1l=extract_l(Ltemp);
    d2h=extract_h(energy);
    d2l=extract_l(energy);
    Lacc=L_mult_su(d1h,d2l);
    Lacc=L_mac40_su(Lacc,d2h,d1l);
    Ltemp=(Word32)L_add40(L_shr(L_mult(d1h,d2h),1),(Word32)L_shr40(Lacc,16));
    // Lacc=energy*16/54*len in Q(se-5)
    se=sub(se,5);  // for alignment of Q factor
    Lacc=L_sub40(L_shl40(maxener,se),Ltemp);

#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (Lacc<0) maxshift=0;
  }
}
  
/*===================================================================*/
/* FUNCTION  :  modifyorig_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE   :  Modify the original residual to match the target     */
/*              residual buffer                                      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  residual : original residual, Q0                 */
/*   _ (Word16 [])  TARGET : target residual, Q0                     */
/*   _ (Word16)     beta: periodicity parameter, Q15                 */
/*   _ (Word16)     shiftrange: shift range (0 or 3), Q0             */
/*   _ (Word16)     dp : start position of segment, Q0               */
/*   _ (Word16)     sfend : end position of segment, Q0              */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 [])  residualm : modified residual, Q0                */
/*   _ (Word16)     dpm: last modified position, Q0                  */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16)     accshift: accumulated shift, Q8                  */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
void modifyorig_fx(Word16 *residual, Word16 *TARGET, Word16 beta,
		   Word16 shiftrange, Word16 dp, Word16 sfend,
		   Word16 *residualm, Word16& dpm, Word16& accshift)
{
  Word16 i, j, k, n,over_bound=0;
  Word16 best, tmp;
  Word32 ex0y2[RSHIFT*2+2]; // integer sampled correlation function, Q0

  Word16 Residual[2*SubFrameSize+2*RSHIFT+1];
  Word16 sfstart, length, shiftrangel, shiftranger, len;
  Word32 e01, e02, maxcor, Ltemp;
  Word16 d1h, d1l, d2h,d2l, d3h, d3l;
  Word40 Lacc;
  Word16 shft_fctr1, shft_fctr2, shft_fctr3;
  
  length = sub(sfend, dp);
  sfstart = dp;

#ifdef WMOPS_FX
  test();
#endif
  if (shiftrange != 0) {
    /* Limit the search range to control accshift */
    shiftrangel = shiftranger = shiftrange;
#ifdef WMOPS_FX
  test();
#endif
    if (accshift < 0)
      shiftrangel = add(shiftrangel, 1);
#ifdef WMOPS_FX
  test();
#endif
    if (accshift > 0)
      shiftranger = add(shiftranger, 1);
    
    /* For non-periodic signals */
    tmp = abs_s(accshift);  // tmp=|accshift|, Q8
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
#endif
    if ((sub(beta,6554)<0 && sub(tmp,15*256)>0) || (sub(beta,9830)<0 && sub(tmp,30*256)>0)) {
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if (accshift < 0)
	shiftranger = 1;
      else
	shiftrangel = 1;
    }
    

#ifdef WMOPS_FX
  test();
#endif
    if (sub(add(shiftrangel, shr(accshift, 8)),72)>0) {
      shiftrangel = sub(72, shr(accshift, 8));
    }
#ifdef WMOPS_FX
  test();
#endif
    if (sub(sub(shiftranger, shr(accshift, 8)),72)>0) {
      shiftranger = add(72, shr(accshift, 8));
    }
    /* Create a buffer of modify residual for match at low cut-off frequency */
    len=add(length,add(shiftrangel,add(shiftranger,1))); // len=length+sl+sr+1
    
    dp_global=dp;
    rclp_fixed_block_interp_fx(residual+dp, Residual, add(accshift,shl(shiftrangel,8)), intrp_tab3, 3, len,&over_bound);
    
#if 0
    length=sub(length,over_bound);
#endif
    
    /* Search for all integer delays of residual */
    tmp = add(shiftrangel, shiftranger);
    for (n = 0; n <= tmp; n++) {
#ifdef WMOPS_FX
  move32();
#endif
      Lacc = 0;
      for (i = 0; i < length-over_bound; i++) {
	Lacc = L_mac40(Lacc, Residual[n + i], TARGET[sfstart + i]);
      }
      ex0y2[n] = (Word32)L_sat32_40(L_shr40(Lacc, 1));
    }

    /* Do quadratic interpolation of ex0y and search for maximum */
#ifdef WMOPS_FX
  move16();
  move32();
#endif
    best=0;
    maxcor=LW_MIN;
    tmp=add(shiftrangel,shiftranger);
    for (n = 1, k = 0; n < tmp; n++) {
      for (j = 0; j < 8; j++, k++){
	Lacc=0x04000; // for rounding
	d1h=extract_h(ex0y2[n-1]);
	d1l=extract_l(ex0y2[n-1]);
	d2h=extract_h(ex0y2[n]);
	d2l=extract_l(ex0y2[n]);
	d3h=extract_h(ex0y2[n+1]);
	d3l=extract_l(ex0y2[n+1]);
	/* Do LSW*quad_int_coef, coef is in Q15          */
	Lacc=L_mac40_su(Lacc,quad_int_tab1[j],d1l);
	Lacc=L_mac40_su(Lacc,quad_int_tab2[j],d2l);
	Lacc=L_mac40_su(Lacc,quad_int_tab3[j],d3l);
	Ltemp=(Word32)L_shr40(Lacc,15);
	/* Do MSW*quad_int_coef            */
	Lacc=L_mult(d1h,quad_int_tab1[j]);
	Lacc=L_mac40(Lacc,d2h,quad_int_tab2[j]);
	Lacc=L_mac40(Lacc,d3h,quad_int_tab3[j]);
	Ltemp=(Word32)L_sat32_40(L_add40(Lacc,Ltemp));  // combine LSW & MSW
#ifdef WMOPS_FX
  test();
#endif
	if (L_sub(Ltemp,maxcor)>0) {
	  maxcor=Ltemp;  // maxcor in Q0
	  best=k;
	}
      }
    }
    best=add(best,4);   // pre-add 4 to topt

#ifdef WMOPS_FX
  test();
#endif
    if (maxcor>0) {
      /* Calculate energy in selected shift index */
      for (i = shiftrangel,Lacc=0; i < add(length-over_bound,shiftrangel); i++)
	Lacc = L_mac40(Lacc, Residual[i], Residual[i]);
      e01=(Word32)L_sat32_40(L_shr40(Lacc,1)); // Saturate e01, make it Q0
      for (i = 0, Lacc=0; i < length-over_bound; i++)
	Lacc = L_mac40(Lacc, TARGET[i + sfstart], TARGET[i + sfstart]);
      e02=(Word32)L_sat32_40(L_shr40(Lacc,1)); // Saturate e02, make it Q0
      
#ifdef WMOPS_FX
  test();
  logic16();
#endif
      if (e01!=0 && e02!=0) { // both >0
	/* Normalize maxcor, e01, e02, calculate maxcor^2 and e01*e02  */
	/* Compare maxcor^2 and 0.49*e01*e02                           */
	shft_fctr1=norm_l(maxcor);
	maxcor=L_shl(maxcor,shft_fctr1);  // normalize maxcor, in Q(shft_fctr1)
	shft_fctr1=shl(shft_fctr1,1);  // shft_fctr1*2 for ^2
	shft_fctr2=norm_l(e01);
	e01=L_shl(e01,shft_fctr2); // e01 in Q(shft_fctr2)
	shft_fctr3=norm_l(e02);
	e02=L_shl(e02,shft_fctr3); // e02 in Q(shft_fctr3)

	/* Compute maxcor^2   */
	d1h=extract_h(maxcor);
	d1l=extract_l(maxcor);
	Lacc=L_mult_uu(d1l,d1l);
	Ltemp=(Word32)L_shr40(Lacc,15);
	Lacc=L_mult_su(d1h,d1l);
	Lacc=L_add40(L_shl40(Lacc,2),Ltemp);
	maxcor=(Word32)L_mac40(L_shr40(Lacc,16),d1h,d1h); // maxcor=maxcor^2*2
	/* Compute e01*e02    */
	d1h=extract_h(e01);
	d1l=extract_l(e01);
	d2h=extract_h(e02);
	d2l=extract_l(e02);
	Lacc=L_mult_uu(d1l,d2l);
	Ltemp=(Word32)L_shr40(Lacc,15); // LSW(e01)*LSW(e02), left shift 1
	Lacc=L_mult_su(d1h,d2l);
	Lacc=L_mac40_su(Lacc,d2h,d1l);
	Lacc=L_add40(L_shl40(Lacc,1),Ltemp); // Add mid term of e01*e02
	Lacc=L_mac40(L_shr40(Lacc,16),d1h,d2h); // Lacc=e01*e02*2, must be in 32-bit range

	shft_fctr2=add(shft_fctr2,shft_fctr3);
	shft_fctr1=sub(shft_fctr2,shft_fctr1);  //shft_fctr1=shft_fctr2+shft_fctr3-shft_fctr1, Q factor difference between e01*e02 and maxcor^2
	
	d1h=extract_h((Word32)Lacc);
	d1l=extract_l((Word32)Lacc);
	Lacc=L_mult_su(16056,d1l); // 16056 is 0.49, Q15
	Ltemp=L_mac((Word32)L_shr40(Lacc,15),16056,d1h); // Ltemp is e01*e02*2*0.49
	
	shft_fctr2=norm_l(Ltemp);
	shft_fctr2=add(shft_fctr1,shft_fctr2); // adjust the difference

#ifdef WMOPS_FX
  test();
#endif
	if (shft_fctr2>=0) { // means e01*e02*0.49 < maxcor^2
#ifdef WMOPS_FX
  test();
#endif
	  if (L_sub(maxcor,L_shr(Ltemp,shft_fctr1))>0) {
	    // accshift=accshift+shiftrangel-best/8, best pre-added 4
	    accshift=add(accshift,shl(shiftrangel,8)); // Q8
	    accshift=sub(accshift,shl(best,5));
	    
	  }
	}
      } //end if (e01!=0 && e02!=0)
    } // end if(maxcor>0)
  }  // end if (shiftrange != 0)
  
  dp_global=dp;
  rclp_fixed_block_interp_fx(residual+dp, residualm+dp, accshift, intrp_tab8, 8, length-over_bound,&over_bound);
#if 0
    length=sub(length,over_bound);
#endif
  dpm=add(dp,length);
}


/*===================================================================*/
/* FUNCTION  :  ComputeACB_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE   :  Modify the original residual to match the target     */
/*              residual buffer, and generate ACB contribution       */
/*              according to the interpolated delay contour          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 [])  residual : original residual, Q0                 */
/*   _ (Word16)     beta: periodicity parameter, Q15                 */
/*   _ (Word32 [])  delay3 : interpolated delay for current          */
/*                           subframe, Q16                           */
/*   _ (Word16)     guard : remaining segment length, Q0             */
/*   _ (Word16)     length :length of current subframe, Q0           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 [])  residualm : modified residual, Q0                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16)     accshift: accumulated shift, Q8                  */
/*   _ (Word16)     dpm: last modified position, Q0                  */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: This routine uses a static buffer for RCELP target          */
/*       It's defined as global -- bufferm_fx[]                      */
/*===================================================================*/
void ComputeACB_fx(Word16 *residualm, Word32 *delay3,
		   Word16 *residual, Word16 guard, Word16& dpm,
		   Word16& accshift, Word16 beta, Word16 length)
{
  static Word16 FirstTime = 1;
  Word16 i,diff=0;
  Word32 delay[SubFrameSize+10];

  Word16 shiftr2, ishift, iacshift;
  Word16 sfstart, sfend;
  
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (FirstTime) {
    FirstTime = 0;
	for (i=0; i<ACBMemSize+SubFrameSize+EXTRA; i++) {
#ifdef WMOPS_FX
  move16();
#endif
		bufferm_fx[i] = 0;
	}
  }
  
  delay_contour_fx(delay3, length, delay);
  rclp_block_interp_fx(bufferm_fx+ACBMemSize, delay, intrp_tab3, 3, length+10);
  /* Do mod() function in-line here   */
#ifdef WMOPS_FX
  test();
#endif
  while (dpm < length) {
    // compute trunc(-accshift+0.5)
    ishift=add(negate(accshift),128);  // add 0.5 (Q8) to -accshift
    iacshift=shr(ishift,8);
#ifdef WMOPS_FX
  test();
#endif
    if (ishift<0) iacshift=negate(shr(negate(ishift),8));
    
    cshiftframe_fx(residual, iacshift, dpm, 
		   guard, delay3[1], length,
		   10, sfstart, sfend,
		   shiftr2);
    ishift=shr(accshift,8);
#ifdef WMOPS_FX
  test();
#endif
    if (accshift<0) ishift=negate(shr(negate(accshift),8)); // fixed point truncation


#if 1 /* to prevent array bound reads */

    diff = sub(sub(sfend,ishift),133);

    if (diff <0) diff=0; 
#endif  
    PeakToAverage_fx(residual+sub(sfstart,ishift), shiftr2, sub(sfend,sfstart)-diff);
    modifyorig_fx(residual, bufferm_fx+ACBMemSize, beta,
		  shiftr2, sfstart, sfend,
		  residualm, dpm, accshift);
    
  }
  dpm=sub(dpm,length);
  /* End of mod() function   */
  for (i = 0; i < ACBMemSize - length; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    bufferm_fx[i] = bufferm_fx[i + length];
  }
  for (i = 0; i < length; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    bufferm_fx[ACBMemSize - length + i] = residualm[i];
  }

}
