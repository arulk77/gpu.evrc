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
/*  Postfilter functions for decoder                                    */
/*======================================================================*/
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "proto_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX

static Word32 PF_mem_syn_pst_fx[ORDER];
static Word16 FIRmem_fx[ORDER];

#ifdef VOIP_DORA
  //Size increased to support Warping
static Word16 Residual_fx[ACBMemSize+SubFrameSize*2];
  //End: Size increased to support Warping
#else
static Word16 Residual_fx[ACBMemSize+SubFrameSize];
#endif

/*===================================================================*/
/* FUNCTION      :  Init_Post_Filter_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Initialize filter memory for post filter         */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 []) FIRmem_fx : prediction filter memory, Q0          */
/*   _ (Word32 []) PF_mem_syn_pst_fx : synthesis filter memory, Q16  */
/*   _ (Word16 []) Residual_fx : prediction filter memory, Q0        */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void Init_Post_Filter_fx(void)
{
    Word16 i;
    
    for (i=0; i<ORDER; i++) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
      PF_mem_syn_pst_fx[i] = 0;
      FIRmem_fx[i] = 0;
    }
	for (i=0;i<ACBMemSize;i++) {
#ifdef WMOPS_FX
  move16();
#endif
      Residual_fx[i]=0;
	}
    
    return;
}

/*===================================================================*/
/* FUNCTION      :  agc_fx ()                                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Adaptive gain control of residual                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) sig_in : Postfilter input speech, Q0              */
/*   _ (Word16 []) sig_out: Postfilter output speech, Q0             */
/*   _ (Word16) l_trm: length of subframe, Q0                        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 []) res_out : residual signal, Q0                     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
/* NOTE: agc_fac is always 0 in floating point, so it's removed from */
/*       fixed point                                                 */
/*===================================================================*/
void agc_fx(Word16 *sig_in, Word16 *sig_out, Word16 *res_out, Word16 l_trm)
{
  Word16 i;
  Word16 gain;  // agc gain, Q14
  Word16 s_out; // shift factors for energies
  Word32 E_out; // normalized energies
  Word32 Ltemp;
  Word40 Lacc;

  for (i=0,Lacc=0;i<l_trm;i++)
    Lacc=L_mac40(Lacc,sig_out[i],sig_out[i]);

#ifdef WMOPS_FX
  test();
#endif
  if (Lacc==0) return;  // output is all 0s
  
  s_out=norm32_l40(Lacc);
  E_out=(Word32)L_shl40(Lacc,s_out); // Q(s_out+1)

  s_out=negate(s_out);
  for (i=0,Lacc=0;i<l_trm;i++)
    Lacc=L_mac40(Lacc,sig_in[i],sig_in[i]);  // Q1
  
  /* Now compute sqrt(E_in/E_out), Q14   */
  Ltemp=sqrt_divide_dp(Lacc, E_out, s_out, &s_out);
  // Ltemp in Q(s_out)
  gain=round32_16(L_shl(Ltemp,sub(30,s_out)));  // gain in Q14

  /* apply gain to residual res_out */
  for (i=0;i<l_trm;i++) {
    Lacc=L_shl40(L_mult(res_out[i],gain),1); // Lacc in Q16
    res_out[i]=extract_h((Word32)L_sat32_40(L_add40(Lacc,0x08000)));
  }
}

/*===================================================================*/
/* FUNCTION      :  ltf_fx ()                                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Long term filtering of residual                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32) delayi : average delay for the subframe, Q16         */
/*   _ (Word16) ltgain: Long term pitch gain, Q15                    */
/*   _ (Word16) len: length of subframe, Q0                          */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 []) res : residual signal, Q0                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void ltf_fx(Word16 *res, Word32 delayi, Word16 ltgain, Word16 len)
{
  Word16 i, n;
  Word16 best, idelay, gain;
  Word32 Ltemp;
  Word40 Lacc, maxcor;

  for (i=0;i<len;i++) Residual_fx[ACBMemSize + i] = res[i];
  
  /* long term filtering */
  /* Find best integer delay around delayi */
  best=round32_16(delayi);
  idelay=best;
  maxcor=0;
  for (i=MAX_FX(DMIN,sub(idelay,3));i<=MIN_FX(DMAX,add(idelay,3));i++) { // maximum 7 times
    for (n=ACBMemSize, Lacc=0; n<ACBMemSize+len; n++)
      Lacc=L_mac40(Lacc,Residual_fx[n],Residual_fx[n-i]); // Q1
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub40_40(Lacc,maxcor)>0) {
      best=i;   // new best pitch estimate 
      maxcor=Lacc;
    }
  }
  
  /* Get beta for delayi */
  
  for (i = ACBMemSize,Lacc = 0; i < ACBMemSize + len; i++) {
    Lacc= L_mac40(Lacc,Residual_fx[i-best],Residual_fx[i-best]); // Q1
  }

#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if (maxcor!=0 && Lacc!=0) {
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub40_40(L_shl40(maxcor,1),Lacc)>0) { // gain>0.5
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if (L_sub40_40(maxcor,Lacc)>=0) gain=0x8000; //  1.0 Q15 unsigned
      else {
	Lacc=divide_dp(maxcor, Lacc, 2); // Lacc is gain in Q31
	gain=round32_16((Word32)L_sat32_40(Lacc));  // round to Q15
      }
      /* gain in Q15 unsigned  */
      Ltemp=(Word32)L_mult_su(ltgain,gain);  // Q30
      gain=round32_16(L_shl(Ltemp,1)); // gain in Q15
      /* Do actual filtering */
      for (i = 0; i < len; i++) {
	Ltemp=L_deposit_h(Residual_fx[ACBMemSize + i]);
	Ltemp=L_mac(Ltemp,gain,Residual_fx[ACBMemSize + i - best]);
	res[i] = extract_h(Ltemp);
      }
    }
  }
  /* Update residual buffer */
  for (i = 0; i < ACBMemSize; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    Residual_fx[i] = Residual_fx[i + len];
  }
}


/*===================================================================*/
/* FUNCTION      :  Post_Filter_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Perform adaptive filtering of synthesis speech   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) syn : Postfilter input speech, Q0                 */
/*   _ (Word16 []) Lsp : LSP coeffs of current subframe, Q15         */
/*   _ (Word16 []) Az :  LPC coeffs of current subframe, Q12         */
/*   _ (Word32) delayi : average delay for the subframe, Q16         */
/*   _ (Word16) agc_fac: Long term pitch gain, Q15                   */
/*   _ (Word16) l_subfr: length of subframe, Q0                      */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) synpf: Postfilter output speech, Q0               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void Post_Filter_fx(Word16 *syn, Word16 *Lsp, Word16 *Az, Word16 *synpf,
		    Word32 delayi, Word16 agc_fact, Word16 l_subfr)
{
  Word16 i;
#ifdef VOIP_DORA
  //Sizes increased to support Warping
  Word16 res2[SubFrameSize*2];       /* inverse filtered synthesis */

  Word16 syn_pst[SubFrameSize*2];    /* post filtered synthesis speech   */

#else
  Word16 res2[SubFrameSize];       /* inverse filtered synthesis */

  Word16 syn_pst[SubFrameSize];    /* post filtered synthesis speech   */
#endif


  Word16 Ap3[ORDER], Ap4[ORDER];  /* bandwidth expanded LP parameters */
  Word16 Lsp3[ORDER], Lsp4[ORDER]; // Q15
  Word16 lsp_ref[ORDER]; // Q15
  Word16 c0, c1;  // Q15, for LSP interpolation
  Word32 tmp_lpc[ORDER]; // Q27 for lpc2lsp
  Word32 mem[ORDER]; // Q16, temporary synthesis filter memory
  Word32 Ltemp;
  Word40 Lacc0, Lacc1;
  

  /*-----------------------------------------------------*
   * Post filtering                                      *
   *-----------------------------------------------------*/
  Lacc0=L_mult(syn[0],syn[0]);  // R(0), Q1
  Lacc1=0;  // R(1), Q1
  for (i=1;i<l_subfr;i++) {
    Lacc0=L_mac40(Lacc0,syn[i],syn[i]);
    Lacc1=L_msu40(Lacc1,syn[i-1],syn[i]); // Lacc1=-sum(x(i-1)*x(i)), Q1
  }
  
  tmp_lpc[0]=(Word32)divide_dp(Lacc1,Lacc0,-2);  // Q27 output
  for (i=1;i<ORDER;i++) tmp_lpc[i]=0;

  lpc2lsp_fx(tmp_lpc,lsp_ref, Lsp);

  /* Interpolate between lsp_ref and Lsp  */
#ifdef WMOPS_FX
  move16();
#endif
  c0=9830; // c0=0.3 in Q15
  for (i=0;i<5;i++) {
    c1=extract_l(L_sub(0x08000,c0));  // c1=1-c0, Q15
    Ltemp=L_mult(c0,lsp_ref[i]); // Q31
    Ltemp=L_mac(Ltemp,c1,Lsp[i]);
    Lsp3[i]=round32_16(Ltemp);
    c0=add(c0,1311);  // c0+=0.04
  }
  for (;i<ORDER;i++) Lsp3[i]=shift_r(add(lsp_ref[i],Lsp[i]),-1); // average

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  c0=9830;  // 0.3, Q15
  c1=22938; // 0.7, Q15
  for (i=0;i<ORDER;i++) {
    Ltemp=L_mult(c0,lsp_ref[i]); // Q31
    Ltemp=L_mac(Ltemp,c1,Lsp[i]);
    Lsp4[i]=round32_16(Ltemp);
  }

  /* Convert Lsp3 and Lsp4 back to LPC  */
  lsp2lpc_fx(Lsp3, Az, Ap3);
  lsp2lpc_fx(Lsp4, Az, Ap4);

  /* filtering of synthesis speech by A(z/GAMMA3) to find res2[] */
  prediction_filter_fx(Ap3, syn, res2, FIRmem_fx,
		       ORDER, l_subfr, 4);
  /* long term filtering */
  ltf_fx(res2, delayi, agc_fact, l_subfr);
  
  /* filtering through  1/A(z/GAMMA4) */
  for (i=0; i<ORDER; i++) {
    mem[i] = PF_mem_syn_pst_fx[i];
  }
  
  synthesis_filter_fx (Ap4, res2, syn_pst, mem,
		       ORDER, l_subfr, 3);

  /* scale output to input */
  agc_fx(syn, syn_pst, res2, l_subfr);
  
  synthesis_filter_fx (Ap4, res2, synpf, PF_mem_syn_pst_fx,
		       ORDER, l_subfr, 3);
  
}
