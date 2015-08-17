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
#include "typedef_fx.h"
#include "dsp_math.h"
#include "uvgq_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

/*===================================================================*/
/* FUNCTION      :  dequantize_uvg_fx()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function returns the  quantized gain
                    vector given the indices in the gain
                    quantization tables                              */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)       iG1  : index into UVG1CB_fx table (Q0)   */
/*         _ (Word16*)      iG2  : indices into UVG2CB_fx (Q0)       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16*)      G    : Output quantized gain vector      */ 
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/


void dequantize_uvg_fx(Word16 iG1, Word16 *iG2, Word16 *G)
{
  Word16 i, k;
  Word16 L16,L16_1;  
  Word32 L32,Lexp;
  UNS_Word16 Lexpl,Lexph;
  for (i=0;i<2;i++) {
    for (k=0;k<5;k++) {

      //G[i*5+k]=pow(10.0,UVG1CB[iG1][i])*UVG2CB[i][iG2[i]][k];
#ifdef WMOPS_FX
  move16();
#endif
      L16 = UVG1CB_fx[iG1][i]; // y=log10(x) = uvg1cb value in Q13
      
      L32= L_shr(L_deposit_h(L16),3); // Q26
      Lexp= (Word32) dsp_pow10(L32); // Q15

      L16_1 = UVG2CB_fx[i][iG2[i]][k]; //UVG2CB[i][iG2[i]][k] in Q12
      Lexph=extract_h(Lexp); // Q(-1)
      Lexpl=extract_l(Lexp); // Q15
      L32=L_shr((Word32) L_mult_su(L16_1,Lexpl),11); //Q(12+15-11=16) 
      L16=round32_16(L_add(L_shl((Word32) L_mult_su(L16_1,Lexph),5),L32));
      // round(Q(12-1+5) + Q(16)) = Q0
      G[add(shr(extract_l(L_mult(i,5)),1),k)]=L16;
        
     }
  }
}






/*===================================================================*/
/* FUNCTION      :  quantize_uvg_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :   This function returns the quantized gain vector */
/*                   and corresponding table indices, given the
		     input unquantized gain vector                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16*)  G : Un-quantized gain vector (Q0)            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16)   iG1  : UVG1CB_fx table index (Q0)            */
/*         _ (Word16*)  iG2  : UVG2CB_fx table indices (Q0)          */
/*         _ (Word16*)  quantG    : Output quantized gain vector     */ 
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/

void quantize_uvg_fx(Word16 *G, Word16& iG1, Word16 *iG2, Word16 *quantG)
{
  Word16 G1[2], G2[10],L16,L16_1;
  Word16 i, j, k, ind,temp;
  Word32 cont;
  cont = L_negate(187595564); // =-log10(5)/2 | bit(15),Q29 
  Word32 mmse;
  UNS_Word16 Ltempl;
  Word16 Ltemph;
  Word32 Ltemp;
  Word40 Lacc,Lexp[2];
  
 for (i=0;i<2;i++) {
#ifdef WMOPS_FX
  move32();
#endif
    Lacc=0;//G1[i]=0;
    for (j=0;j<5;j++)
      {
	ind=add(shr(extract_l(L_mult(i,5)),1),j);
        Lacc=L_mac40(Lacc,G[ind],G[ind]); //Q1
      }
    Lacc=L_shr40(Lacc,1); // to count for shl by L_mult, Q0
#ifdef WMOPS_FX
  test();
  move32();
#endif
    if (Lacc==0) Lacc=1; // to avoid log10(0)
    
    Ltemp=log10_lut(Lacc); // 10log10(Lacc) //Q23 Ltemph=Q7
    G1[i]= extract_h((Word32) L_add40(L_shl40(L_mult_su(extract_h(Ltemp),26214),3),cont)); // 26214= 1/20 in Q19
                                                                                  
  }
 
#ifdef WMOPS_FX
  move32();
  move16();
#endif
  mmse=MAX_32; //mmse=1e30;
  iG1=0;
  for (i=0;i< UVG1_CBSIZE; i++) {
     L16 = shr(sub(G1[0],UVG1CB_fx[i][0]),1); // Q12
    L16_1 = shr(sub(G1[1],UVG1CB_fx[i][1]),1); // Q12
    Lacc = 0;
    Lacc = L_mac40(Lacc,L16,L16); 
    Lacc = L_mac40(Lacc,L16_1,L16_1);
    Lacc = L_shr40(Lacc,1); 
    
#ifdef WMOPS_FX
  test();
  move16();
  move32();
#endif
    if (L_sub40(Lacc,mmse)<0) {
      iG1=i;
      mmse=(Word32) Lacc;
    }
      
  }
   Ltemp = L_shr(L_deposit_h(UVG1CB_fx[iG1][0]),3); // Q26
   Lexp[0]= dsp_pow10(Ltemp); // Q15
   
   Ltemp = L_shr(L_deposit_h(UVG1CB_fx[iG1][1]),3); // Q26
   Lexp[1]= dsp_pow10(Ltemp); // Q15
   
  for (i=0;i<2;i++) {
   Ltemp=(Word32) L_sat32_40(divide_dp(32768,Lexp[i],-5)); // Q24
   Ltempl=extract_l(Ltemp); 
   Ltemph=extract_h(Ltemp); 

  
    for (j=0;j<5;j++)
      {
      ind=add(shr(extract_l(L_mult(i,5)),1),j);
    Lacc=L_mult_su(G[ind],Ltempl);
    Lacc=L_add40(L_shl40(Lacc,4),0x00008000); // Q12
    Ltemp=L_mult(G[ind],Ltemph); //Q9
    Lacc=L_add40_40(Lacc,L_shl40(Ltemp,19));
    G2[ind]=extract_h((Word32)L_sat32_40(Lacc));  //Q12, saturates for some frames
     
      }
  }

    
  for (i=0;i<2;i++) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
    mmse=MAX_32;
    iG2[i]=0;
    for (j=0;j<UVG2_CBSIZE;j++) {
#ifdef WMOPS_FX
  move32();
#endif
      Lacc=0;
      for (k=0;k<5;k++)
	{
	   ind=add(shr(extract_l(L_mult(i,5)),1),k);
	  temp=sub(G2[ind],UVG2CB_fx[i][j][k]);
	  Lacc=L_mac40(Lacc,temp,temp); //Q1
	  
	}
      
      Lacc=L_shr40(Lacc,1);
#ifdef WMOPS_FX
  test();
  move32();
  move16();
#endif
      if (L_sub40(Lacc,mmse)<0) {
	mmse=(Word32) Lacc;
	iG2[i]=j;
	
      }
    }
  }  
      
  dequantize_uvg_fx(iG1, iG2, quantG);
 
}



    
