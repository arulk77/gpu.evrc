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

#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "proto_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

 static Word16 encFirMem[ORDER]; // Q0, regular weighted LPC prediction filter memory
 static Word32 encIir0Mem[ORDER]; // Q16, non-weighted LPC synthesis filter memory
 static Word32 encIir1Mem[ORDER]; // Q16, weighted LPC synthesis filter memory

/*===================================================================*/
/* FUNCTION      :  Init_ZeroInput_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Initialize filter memory for ZeroInput function  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16 []) encFirMem[ORDER], regular weighted LPC 
                   prediction filter memory ,Q0                      */
/*   _ (Word32 []) encIir0Mem[ORDER],non-weighted LPC synthesis 
                   filter memory, Q16                                */
/*   _ (Word32 []) encIir1Mem[ORDER],weighted LPC synthesis
                   filter memory,Q16                                 */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void Init_ZeroInput_fx(void)
{
    Word16 i;
    
    for (i=0;i<ORDER;i++) {
#ifdef WMOPS_FX
    move16();
	move32();
	move32();
#endif
      encFirMem[i]=0;
      encIir0Mem[i]=encIir1Mem[i]=0;
    }
    return;
}

/*===================================================================*/
/* FUNCTION      :  fir_filter_fx ()                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generic FIR filter routine                       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) b   : filter coefficients (Qc).                   */
/*   _ (Word16 []) x   : input signal  (Qn).                         */
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
/*   _               : None                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void fir_filter_fx (Word16 b [], Word16 x [], Word16 y [],
			 Word16 P, Word16 N, Word16 Qa)
{

  Word16 i, j, reg16;
  Word32 reg32;
  Word40 acc40;
 
  for (i = 0; i < N; i ++) {
#ifdef WMOPS_FX
    move32();
#endif
    acc40  = (Word40)0;

    for (j=0; j<=P; j++)
      acc40=L_mac40(acc40, b[j], x[i-j]);
    /*--------------------------------------------------------------*/
    /*            The result is in Qn + Qc + 1 in 40bits            */
    /*           Bring the result back to Qn+16: sum << Qa-1        */
    /*--------------------------------------------------------------*/
    acc40 = L_shl40 (acc40, sub(Qa,1)); 
    /*--------------------------------------------------------------*/
    /*                      To avoid overflow                       */
    /*--------------------------------------------------------------*/
    acc40=L_add40(acc40, 0x08000);
    reg32 = (Word32)L_sat32_40(acc40);
    reg16 = extract_h(reg32);
#ifdef WMOPS_FX
    move16();
#endif
    y[i] = reg16;
  }
  
  return;
}

/*===================================================================*/
/* FUNCTION      :  pz_filter_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generic pole-zero filter routine, with single    */
/*                  precision memory                                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) b   : zero filter coefficients (Qc).              */
/*   _ (Word16 []) a   : pole filter coefficients (Qc), a(0)=1 in Qc */
/*   _ (Word16 []) x   : input signal (Qn).                          */
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
/*   _ (Word16 []) buf : filter memory (Qn-Qa).                      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void pz_filter_fx (Word16 b [], Word16 a [], Word16 x [], Word16 y [],
			 Word16 buf [], Word16 P, Word16 N, Word16 Qa)
{
  Word16 i, j;
  Word16 s, s_mem;
  Word32 Ltemp1;
  Word40 Lacc;

  s=negate(Qa);
  s=add(s,s);  // s=-2Qa
  s=add(s,1);
  for (i=0;i<N;i++) {
    Lacc=L_deposit_h(x[i]); // Lacc in Q(16+Qn)
    Lacc=L_shl40(Lacc,s);   // Lacc=x[i] in Q(16+Qn-2Qa+1)
    for (j=P-1;j>=0;j--)
      Lacc=L_msu40(Lacc,a[j+1],buf[j]);
    /* Lacc=x(i)-sum(a(j+1)*buf(j)), Qn+Qc-Qa+1           */
    Lacc=L_shr40(Lacc,1);  // take out the left shift 1 in Lmsu_40
    //    s_mem=round32_16((Word32)L_sat32_40(L_shl40(Lacc,Qa)));
    Ltemp1=(Word32)L_sat32_40(L_add40(L_shl40(Lacc,Qa),0x08000));
    s_mem=extract_h(Ltemp1);
    // s_mem in Qn-Qa
#ifdef WMOPS_FX
    move32();
#endif
    Lacc=0;
    for (j=P-1;j>=0;j--)
      Lacc=L_mac40(Lacc,b[j+1],buf[j]);
    Lacc=L_mac40(Lacc,b[0],s_mem);
    // Lacc in Q(1+Qc+Qn-Qa)
	for (j=P-1;j>0;j--) { // Update filter memory
#ifdef WMOPS_FX
    move16();
#endif
      buf[j]=buf[j-1];
	}
#ifdef WMOPS_FX
    move16();
#endif
    buf[0]=s_mem;

    Ltemp1=(Word32)L_sat32_40(L_add40(L_shr40(Lacc,s),0x08000)); // Ltemp1 in Qc+Qa+Qn=Q(16+Qn)
    y[i]=extract_h(Ltemp1);  // y[i] in Qn
  }
}


/*===================================================================*/
/* FUNCTION      :  pz_filter_dp_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generic pole-zero filter routine, with double    */
/*                  precision memory, transposed direct form II      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16 []) b   : zero filter coefficients (Qc).              */
/*   _ (Word16 []) a   : pole filter coefficients (Qc), a(0)=1       */
/*   _ (Word16 []) x   : input signal (Q0).                          */
/*   _ (Word16)    P   : filter order.                               */
/*   _ (Word16)    N   : number of input samples.                    */
/*   _ (Word16)    Qa  : Q factor compensation (Qa=16-Qc)            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) y : output signal  (Q0)                           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*   _ (Word32 []) buf : filter memory (Qc+1)                        */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void pz_filter_dp_fx (Word16 b [], Word16 a [], Word16 x [], Word16 y [],
			 Word32 buf [], Word16 P, Word16 N, Word16 Qa)
{
  Word16 i, j;
  Word16 s;
  Word16 d1h,d1l;
  Word32 Ltemp1, Ltemp2, Ltemp;
  Word40 Lacc;

  s=sub(Qa,1);  // s=Qa-1
  for (i=0;i<N;i++) {
    /* y[i]=b[0]*x[i]+buf[0]   */
    Ltemp1=L_add(buf[0],L_shr(L_mult(b[0],x[i]),1));  // Qc
    //    y[i]=round32_16(L_shl(Ltemp1,Qa)); // Q0
#ifdef WMOPS_FX
    move32();
#endif
    Ltemp=Ltemp1;

    /* Update filter memory  */
    d1h=extract_h(Ltemp1);
    d1l=extract_l(Ltemp1);
    for (j=0; j<P-1; j++) {
      /* buf[j]=b[j+1]*x[i]+a[j+1]*y[i]+buf[j+1]  */
      Lacc=L_mult_su(a[j+1],d1l); // Qc+Qc
      Ltemp1=(Word32) L_shr40(Lacc,sub(16,Qa)); // Qc
      Lacc=L_add40(L_shl40(L_mult(a[j+1],d1h),s),Ltemp1); // Qc

      Ltemp2=L_mult(b[j+1],x[i]);  // b[j+1]*x[i], Qc+1
      Lacc=L_add40(Lacc,L_shr(Ltemp2,1)); // Qc
      Lacc=L_add40(Lacc,buf[j+1]); // Qc
      buf[j]=(Word32) L_sat32_40(Lacc); // Qc
    }

    /* buf[P-1]=b[P]*x[i]+a[P]*y[i]  */
    Lacc=L_mult_su(a[P],d1l);
    Ltemp1=(Word32) L_shr40(Lacc,sub(16,Qa)); // Qc
    Lacc=L_add40(L_shl40(L_mult(a[P],d1h),s),Ltemp1); // Qc
    Ltemp2=L_mult(b[P],x[i]);  // b[P]*x[i], Qc+1
    Lacc=L_add40(Lacc,L_shr(Ltemp2,1));
    buf[P-1]=(Word32) L_sat32_40(Lacc);

    y[i]=round32_16(L_shl(Ltemp,Qa));
  }
}

/*===================================================================*/
/* FUNCTION      :  ZeroInput_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute zero input response or just update filter*/
/*                  memories of 3 cascaded filters                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) input  : input to filters, Q0                     */
/*   _ (Word16 []) acoef  : 1st stage synthesis filter coeffs, Q12   */
/*   _ (Word16 []) acoef_n: 2nd stage prediction filter coeffs, Q12  */
/*   _ (Word16 []) acoef_d: 3rd stage synthesis filter coeffs, Q12   */
/*   _ (Word16)    len   : length of output                          */
/*   _ (Word16)    type : type of operations                         */
/*                        0 -- zir                                   */
/*                        1 -- update filter memory                  */
/*                        2 -- update filter memory for voiced coder */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) output : Output after 3-stage LPC filters, Q0     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void ZeroInput_fx(Word16 *input, Word16 *acoef, Word16 *acoef_n,
		  Word16 *acoef_d, Word16 *output, Word16 len, Word16 type)
{
  Word16 i;

 
  Word16 mem_sp[ORDER];  // Q0
  Word32 mem_dp0[ORDER]; // Q16
  Word32 mem_dp1[ORDER]; // Q16
  Word16 scaled_res[SubFrameSize]; // to handle high level

#ifdef WMOPS_FX
  test();
#endif
  if (type==1) {
    /* Update filters memory */
    for (i=0;i<len;i++) scaled_res[i]=shr(input[i],SCALE);
    lpc_weighted_filtering_fx(scaled_res, acoef, acoef_n, acoef_d,
			      encIir0Mem, encFirMem, encIir1Mem,
			      output, len);
    for (i=0;i<len;i++) output[i]=shl(output[i],SCALE);
  }
  else { // type==0
    /* Get zir */
    /* ring ZIR to the future with zero input */
	for (i=0; i<len; i++) {
#ifdef WMOPS_FX
  move16();
#endif
		  output[i] = 0;
	}
    for (i=0;i<ORDER;i++) { // avoid corrupting filter memories
#ifdef WMOPS_FX
  move16();
  move32();
  move32();
#endif
      mem_sp[i]=encFirMem[i];
      mem_dp0[i]=encIir0Mem[i];
      mem_dp1[i]=encIir1Mem[i];
    }
    lpc_weighted_filtering_fx(output, acoef, acoef_n, acoef_d,
			      mem_dp0, mem_sp, mem_dp1,
			      output, len);
    for (i=0;i<len;i++) output[i]=shl(output[i],SCALE);
  }
}
