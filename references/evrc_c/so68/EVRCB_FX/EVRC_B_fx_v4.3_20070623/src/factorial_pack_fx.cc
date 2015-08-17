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
#include "fac_tab.h"
#include "macro_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

/*===================================================================*/
/* FUNCTION      :  F_fx ()                                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Computes n*(n-1)*...*(n-m+1)/m!                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      n                                         */
/*         _ (Word16)      m                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                     _ None                                        */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word32)   n*(n-1)*...*(n-m+1)/m!                       */
/*                      return 0 if n<m or m<0                       */
/*===================================================================*/

Word32 F_fx(Word16 n, Word16 m)
{
  Word16 den, offset, n1, n2;
  Word32 num_l;

  /* Analysis has m<=7 and n<=54             */
#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if((n<m) || (m<0)) {
    return(0);
  }
  else {
#ifdef WMOPS_FX
  test();
#endif
    if(sub(n,m)>m) {
      den = m;
    }
    else {
      den = sub(n,m);
    }

#ifdef WMOPS_FX
  test();
#endif
    if (den==0) return(1);
#ifdef WMOPS_FX
  test();
#endif
    if (den==1) return(n);

    n1=n;
    n2=sub(n,1);
    num_l=L_shr(L_mult(n1,n2),1);
#ifdef WMOPS_FX
  test();
#endif
    if (den==2) return(L_shr(num_l,1));

    // Use look-up table for den>2
    n1=sub(n,6);
    n2=sub(den,3);
    offset=add(n2,extract_l(L_shr(L_mult(n1,5),1)));
    // offset=(n-6)*5+(den-3)
    return (fact_table[offset]);
  }
}


/*===================================================================*/
/* FUNCTION      :  Ipos_fx ()                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Pack pulse position by factorial                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16 [])   posvect:  position vector                 */
/*         _ (Word16)      vectlen:  length of position vector       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _               None                                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word32)      sum of positions                          */
/*===================================================================*/
/* Note: The maximum vectlen is 7 and maximum F_fx is 0xA8E5710      */
/*       So the result can be represented in one 32-bit word         */
/*===================================================================*/

Word32 Ipos_fx(Word16 *posvect, Word16 vectlen)
{
  Word16 indx;
  Word32 pos, Ltemp;

#ifdef WMOPS_FX
  move32();
#endif
  Ltemp=0;
  for(indx=0; indx<vectlen; indx++) {
    pos = F_fx(posvect[indx], indx+1);
    Ltemp=L_add(Ltemp,pos);
  }
  return (Ltemp);
}


/*===================================================================*/
/* FUNCTION      :  Ioff_fx ()                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      Nd                                        */
/*         _ (Word16)      Np                                        */
/*         _ (Word16)      len                                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _               None                                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word32)      Ioff                                      */
/*===================================================================*/
/* Note: The inputs for this function is very small, max is 7        */
/*       So the result can be represented in one 32-bit word         */
/*===================================================================*/

Word32 Ioff_fx(Word16 Nd, Word16 Np, Word16 len)
{
  Word32 Ltemp, f1, f2;
  Word16 indx;

#ifdef WMOPS_FX
  move32();
#endif
  Ltemp=0;
  for(indx=add(Nd,1); indx<=Np; indx++) {
    f1=F_fx(len,indx);
    f2=F_fx(sub(Np,1),sub(indx,1));
    Ltemp=L_add(Ltemp,L_shr(L_mult(extract_l(f1),extract_l(f2)),1));
    // Ltemp+=f1*f2
  }

  return(Ltemp);
}


/*===================================================================*/
/* FUNCTION      :  Ioff_S_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      Nd                                        */
/*         _ (Word16)      Np                                        */
/*         _ (Word16)      len                                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word32 [])   pw35: 35 bit output stored in 2 32-bit    */
/*                         memory                                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                       _ None.                                     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _               None                                      */
/*===================================================================*/

void Ioff_S_fx(Word16 Nd, Word16 Np, Word16 len, Word32 *pw35)
{
  Word16 indx, dl, fh, fl;
  Word40 Lacc;
  Word32 Ltemp1, Ltemp2;
  
#ifdef WMOPS_FX
  Nmove32(3);
#endif
  Lacc=0;
  pw35[0]=pw35[1]=0;
  for(indx=add(Nd,1); indx<=Np; indx++) {
    Ltemp1=F_fx(len,indx);
    Ltemp2=F_fx(sub(Np,1),sub(indx,1));
    dl=extract_l(Ltemp2); // Ltemp2 is a small number, less than 16 bits
    fh=extract_h(Ltemp1);
    fl=extract_l(Ltemp1);

    Ltemp1=(Word32)L_mult_su(dl,fl);
    Ltemp1=L_add(Ltemp1,L_shl(L_mult(fh,dl),15));
    // Ltemp1=F_fx(len,indx)*F_fx(sub(Np,1),sub(indx,1)) in Q0
    // Ltemp1 must be in 31 bits or less (i.e. must be positive)
    
    /* Compute pw35[1]*2^16+pw35+Ltemp1<<indx                   */
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp2=pw35[0];
    Lacc=L_add40(L_shl40(Ltemp1,indx),Ltemp2);
    pw35[0]=extract_l40(Lacc) & 0x0ffff;  // Store unsigned 16 bit number, LSW of Lacc
                                          // upper 16 bits are 0
    // Perform extended precision add
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp2=pw35[1];     
    Lacc=L_add40(L_shr40(Lacc,16),Ltemp2);
#ifdef WMOPS_FX
  move32();
#endif
    pw35[1]=(Word32)Lacc;   // Lacc only has 19 significant bits
    
  }

}


/*===================================================================*/
/* FUNCTION      :  factorial_pack_fx                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Pack 35 bit code word for full rate celp FCB     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16 [])   vector_fx: 7 pulses vector                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16 [])   pw35: 35 bit output stored in 3 16-bit    */
/*                         memory locations                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                       _ None.                                     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _               None                                      */
/*===================================================================*/
/* Note: This fixed point implmentation assumes only one 40-bit      */
/*       accumulator in the DSP for general purpose. However, if     */
/*       there is more than one 40-bit register. One can dedicate    */
/*       such a register to hold intermediate 35-bit output          */
/*       to reduce the complexity                                    */
/*===================================================================*/

void factorial_pack_fx(Word16 *vector_fx, Word16 *pw35)
{
  Word16 indx;
  Word16 indx2;
  Word16 Tvect[NUM_PULSES], temp;
  Word16 TLam[NUM_PULSES];
  Word16 Spos;
  Word16 Soff;
  Word16 Td, Tn, Tm;
  Word16 Niter;
  
  Word32 Tcode[2], code[2];
  Word32 Toff;

  Word32 Ltemp1, Ltemp2;
  Word40 Lacc;
  Word16 dl, sh,sl;
  
#ifdef WMOPS_FX
  move16();
#endif
  Td = 0;
  for (indx=0; indx<L_VECTOR; indx++) {
    temp=abs_s(vector_fx[indx]);
#ifdef WMOPS_FX
  test();
#endif
    if(temp!=0) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      TLam[Td] = indx;
      Tvect[Td]=temp;
      Td=add(Td,1);
    }
  }
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  Tm=NUM_PULSES;
  Tn = L_VECTOR;
  
  Ioff_S_fx(Td, Tm, Tn, code);

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  Spos = 0;
  Soff=Td;
  for (indx=0; indx<Td; indx++) {
#ifdef WMOPS_FX
  test();
#endif
    if(vector_fx[TLam[indx]]<0) {
      Spos=add(Spos,shl(1,sub(sub(Td,indx),1)));
    }
  }

  /* Compute Ioff_S_fx(Td, Tm, Tn, code)+Spos        */
  Ltemp1=L_deposit_l(Spos);   // high 16 bits of Ltemp1 is 0
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp2=code[0];             // high 16 bits of Ltemp2 is 0
  Lacc=L_add(Ltemp1,Ltemp2);
  code[0]=extract_l40(Lacc) & 0x0ffff;  // Store unsigned 16 bit number, LSW of Lacc
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp2=code[1];
  Lacc=L_add40(L_shr40(Lacc,16),Ltemp2);
#ifdef WMOPS_FX
  move32();
#endif
  code[1]=(Word32) Lacc;  // Lacc only has 19 significant bits

  Niter=add(sub(Tm,Td),1);

#ifdef WMOPS_FX
  move16();
  move16();
  move16();
#endif
  Toff=0;
  Tcode[0]=Tcode[1]=0; // Initialze Tcode

  for(indx=0; indx<Niter; indx++) {
    Ltemp1=Ipos_fx(TLam,Td);
    Ltemp2=F_fx(sub(Tm,1),sub(Td,1));
    dl=extract_l(Ltemp2);  // upper 16 bits of Ltemp2 must be 0
    sh=extract_h(Ltemp1);
    sl=extract_l(Ltemp1);
    Lacc=L_mult_su(sh,dl); // Compute Ltemp1*Ltemp2 in Q0
    Lacc=L_add40(L_shl40(Lacc,16),(Word32)L_mult_uu(sl,dl));
    
    Lacc=L_add40(Lacc,Toff); // Lacc=Ltemp1*Ltemp2 + Toff
    
    /* Compute Tcode+=Ipos_fx(Tlam,Td)*F_fx(sub(Tm,1),sub(Td,1))+Toff */
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp1=Tcode[0];    // upper 16 bit of Ltemp1 is 0
    Lacc=L_add40(Lacc,Ltemp1);
    Tcode[0]=extract_l40(Lacc) & 0x0ffff;  // Store unsigned 16 bit number, LSW of Lacc
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp1=Tcode[1];
    Lacc=L_add40(L_shr40(Lacc,16),Ltemp1);
#ifdef WMOPS_FX
  move32();
#endif
    Tcode[1]=(Word32)Lacc;  // Lacc only has 19 significant bits

#ifdef WMOPS_FX
  move16();
#endif
    Tn = Td;
    Tm=sub(Tm,Tn);
    Td = 0;
    for(indx2=0; indx2<Tn; indx2++) {
      temp = sub(Tvect[indx2],1);
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
      if(temp>0) {
	TLam[Td] = indx2;
	Tvect[Td] = temp;
	Td=add(Td,1);
      }
    }
    Toff=Ioff_fx(Td, Tm, Tn);
  }
  
  /* Compute code+=Tcode<<Soff          */
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp1=Tcode[0];   // Remember high 16 bits are 0s, it's an unsigned number
  Lacc=L_deposit_h(Tcode[1]); // Lacc has the high 19 bits of a 35 bit codeword
  Lacc=L_add40(Lacc,Ltemp1);
  Lacc=L_shl40(Lacc,Soff);   // Lacc = Tcode<<Soff

  /* Compute Lacc+=code            */
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp1=code[0];
  Lacc=L_add40(Lacc,Ltemp1);
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp1=code[1];

  pw35[0]=extract_l40(Lacc);
  Lacc=L_add40(L_shr40(Lacc,16),Ltemp1);  // Lacc has the high 24 bits of extended addition
  pw35[1]=extract_l40(Lacc);
  pw35[2]=extract_h((Word32)Lacc);
  
}


/*===================================================================*/
/* FUNCTION      :  factorial_unpack_fx                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Unpack 35 bit code word for full rate celp FCB   */
/*                  and form the fixed code book vector              */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16 [])   pw35: 35 bit output stored in 3 16-bit    */
/*                         memory locations                          */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16 [])   vector_fx: 7 pulses vector                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                       _ None.                                     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _               None                                      */
/*===================================================================*/
/* Note: This fixed point implmentation assumes only one 40-bit      */
/*       accumulator in the DSP for general purpose. However, if     */
/*       there is more than one 40-bit register. One can dedicate    */
/*       such a register to hold intermediate 35-bit output          */
/*       to reduce the complexity of implementation                  */
/*===================================================================*/

void factorial_unpack_fx(Word16 *pw35, Word16 *vector_fx)
{
  Word16 Nd, Np;
  Word16 Ndmax, Ndmin;
  Word32 Tcode[2];
  Word32 pos;
  Word16 Spos;
  Word16 posvect[NUM_PULSES];
  Word16 TNd;
  Word16 Tlen;
  Word16 indx;
  Word16 signvector[L_MAXSIZE];
  Word16 pv_offset;
  
  Word16 dl,dh,i;
  Word32 Ltemp1, Ltemp2, Ltemp3, r;
  Word40 Lacc;

  for (indx=0; indx<L_MAXSIZE; indx++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    vector_fx[indx]=0;
    signvector[indx]=1;
  }

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  Ndmin=1;
  Ndmax=NUM_PULSES;
  /* Use split search for the number of pulses    */
  for (indx=0; indx<3; indx++) {
    Nd=shr(add(Ndmax,Ndmin),1);
    
    //  do {
    Ioff_S_fx(Nd, NUM_PULSES, L_VECTOR, Tcode);
  /* Compare input codeword and Tcode      */
  /* Tcode[0] has low 16 bits, Tcode[1] has high 19 bits   */
  /* pw35 has 3 16-bit words                               */
#ifdef WMOPS_FX
  move32();
#endif
    Ltemp1=Tcode[0];
    Ltemp2=pw35[0]&0x0FFFF;
    Ltemp1=L_sub(Ltemp2,Ltemp1); // Ltemp1=codeword-Tcode (LSW)
#ifdef WMOPS_FX
  move32();
  logic32();
#endif
    Ltemp2=pw35[1]&0x0FFFF;
    Ltemp3=L_deposit_h(pw35[2]); // This will clear the low 16 bits
    Ltemp2=L_add(Ltemp2,Ltemp3); // Ltemp2 has upper 19 bits of codeword
    Lacc=L_sub40((Word40)Ltemp2,Tcode[1]);
    Lacc=L_add40(L_shl40(Lacc,16),Ltemp1);
    //    Nd=add(Nd,1);
    //  } while (Lacc<0);
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (Lacc<0) Ndmin=Nd;
    else Ndmax=Nd;
  }
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Lacc<0) {  // Need to get a positive Lacc corresponding to Nd
    Nd=Ndmax;
    Ioff_S_fx(Nd, NUM_PULSES, L_VECTOR, Tcode);
  /* Compare input codeword and Tcode      */
  /* Tcode[0] has low 16 bits, Tcode[1] has high 19 bits   */
  /* pw35 has 3 16-bit words                               */
#ifdef WMOPS_FX
  move32();
  move32();
  Nlogic16(2);
#endif
    Ltemp1=Tcode[0];
    Ltemp2=pw35[0]&0x0FFFF;
    Ltemp1=L_sub(Ltemp2,Ltemp1); // Ltemp1=codeword-Tcode (LSW)
    Ltemp2=pw35[1]&0x0FFFF;
    Ltemp3=L_deposit_h(pw35[2]); // This will clear the low 16 bits
    Ltemp2=L_add(Ltemp2,Ltemp3); // Ltemp2 has upper 19 bits of codeword
    Lacc=L_sub40((Word40)Ltemp2,Tcode[1]);
    Lacc=L_add40(L_shl40(Lacc,16),Ltemp1);
  }
  
  //  Nd=sub(Nd,1);
#ifdef WMOPS_FX
  move16();
  logic16();
  move16();
#endif
  dl=pw35[0];
  dh=sub(shl(1,Nd),1);
  Spos=dh&dl;             // Spos=codeword & ((0x0001<<Nd) - 1)

  // Lacc has removed the # of pulses information from codeword
  Lacc=L_shr40(Lacc,Nd);       // Lacc must be in 32-bit range
  Ltemp2=(Word32)L_sat32_40(Lacc); // back up Lacc

  Ltemp1=F_fx(NUM_PULSES-1,sub(Nd,1));
  pos=div_int_dp(Ltemp2,Ltemp1,r);  // pos=Ltemp2/Ltemp1
  for(indx=0; indx<Nd; indx++) {
    /* fast search for pulse location  */
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    Ndmin=0;
    Ndmax=L_VECTOR;
    for (i=0;i<6;i++) {  // maximum vector lenth is 54, so 6 iterations are sufficient
      TNd=shr(add(Ndmin,Ndmax),1);
      Ltemp3=L_sub(F_fx(TNd, sub(Nd,indx)),pos);
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if (Ltemp3<=0) Ndmin=TNd;
      else Ndmax=TNd;
    }
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (Ltemp3>0) TNd=Ndmin;

    /*    
    TNd=sub(sub(Nd,indx),1);
    while(L_sub(F_fx(TNd, sub(Nd,indx)),pos)<=0) {
      TNd=add(TNd,1);
    }
    
    TNd=sub(TNd,1); */
    
    posvect[sub(sub(Nd,indx),1)]=TNd;
    vector_fx[TNd]=add(vector_fx[TNd],1);
    dh=Spos & shl(1,indx);
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (dh==0) 
      signvector[TNd]=1;
    else
      signvector[TNd]=-1;
    
    pos=L_sub(pos,F_fx(TNd, sub(Nd,indx)));
  }

  Ltemp1=r;  // Ltemp1 is the remainder, small number
  Np = sub(NUM_PULSES,Nd);
  Tlen = Nd;
  pv_offset=Tlen;

#ifdef WMOPS_FX
  test();
  move16();
#endif
  while(Np>0) {
    Nd=0;
#ifdef WMOPS_FX
  test();
#endif
    while (L_sub(Ltemp1,Ioff_fx(Nd, Np, Tlen))<0) {
      Nd=add(Nd,1);
    }
    Ltemp2=L_sub(Ltemp1,Ioff_fx(Nd, Np, Tlen));
    Ltemp3=F_fx(sub(Np,1),sub(Nd,1));
    pos=div_int_dp(Ltemp2, Ltemp3, r);  
    
    for(indx=0; indx<Nd; indx++) {
      TNd=sub(sub(Nd,indx),1);
#ifdef WMOPS_FX
  test();
#endif
      while(L_sub(F_fx(TNd, sub(Nd,indx)), pos)<=0) {
	TNd=add(TNd,1);
      }
      TNd=sub(TNd,1);
      vector_fx[posvect[TNd+sub(pv_offset,Tlen)]]=
	add(vector_fx[posvect[TNd+sub(pv_offset,Tlen)]],1);
      posvect[sub(sub(pv_offset,indx),1)]=posvect[TNd+sub(pv_offset,Tlen)];
      pos=L_sub(pos,F_fx(TNd, sub(Nd,indx)));
    }
#ifdef WMOPS_FX
  move32();
  move16();
#endif
    Ltemp1=r;  // Ltemp1 is remainder, small number
    /* Update Np and Tlen for next iteration */
    Np = sub(Np,Nd);
    Tlen = Nd;
  }
  for(indx=0; indx<L_VECTOR; indx++) {
    vector_fx[indx]=extract_l(L_shr(L_mult(vector_fx[indx],signvector[indx]),1));
  }
  
}



/*===================================================================*/
/* FUNCTION      :  corr_xh_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute the correlation between the target       */
/*                  signal and the impulse response of the weighted  */
/*		    synthesis filter.                                */     
/*                  y[i]=sum(j=i,l-1) x[j]*h[j-i], i=0,l-1           */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16*) x : target signal   (Q0)                            */
/*   _ (Word16*) h : impulse response of weighted                    */
/*                   synthesis filter (Q11)                          */
/*   _ (Word16) l  : vector size  (Q0)                               */
/*   _ (Word16) ns  : number of side bits  (Q0)                      */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16*) dn :  correlation between x[] and h[]               */ 
/*   _ (Word16*) n  :  dn in Q(n-4), where n=MIN (norm factor of
                       elements of dn)-ns (side bits)                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*                                                                   */
/*===================================================================*/

void corr_xh_fx(
	Word16 *x,
	Word16 *dn,
	Word16 *h,
	Word16 l,	
	Word16 *n,
      Word16 ns)
{
  Word16     i, j, l_1, l2;
 
  Word40 s;
  Word32 max,y_32[l];
  Word16 *px, *px2, *ph;
  
  l_1 = sub(l,1);
#ifdef WMOPS_FX
  move16();
#endif
  l2 = l_1;
  px2 = x;
  max=0;
  for (i = 0; i <= l_1; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    px = px2;
    ph = h;
    s = 0.0;
     
    for (j = 0; j <= l2; j++)
      {
	 
       s = L_mac40 ( s, (*px), (*ph));
       px++;
       ph++;
      }
    
    y_32[i]=(Word32) L_sat32_40(s);//Q12
    
    //    if (s < 0) s = L_negate40(s);
#ifdef WMOPS_FX
  test();
#endif
    if (s < 0) { s = L_negate40(s); s=L_sub40(s,1);}

    
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub40(s,max) > (Word40) 0)
      max=(Word32) L_sat32_40(s);
        
    l2--;
    px2++;
  }

  //*n=sub(norm_l(max),2);
  *n=sub(norm_l(max),ns);
  for(i=0;i<=l_1;i++)
    {
      *dn = round32_16(L_shl(y_32[i],*n)); //Q(12+n-16)
      dn++;
    }
  
  return;
}
