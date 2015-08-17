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

#include "typedef_fx.h"
#include "proto_fx.h"
#include "globs_fx.h"
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "lpcana_tab.h"
#include "nelp_fx.tab"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

/*===================================================================*/
/* FUNCTION      :  nelp_random_excit_fx                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function computes the random
                    excitation scaled by gain                        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */

/*         _ (Word16*)      Gains :  Gain vector (Q0)                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16*)      seed  : Random seed (Q0)                 */ 
/*         _ (Word16*)      out1 :  Random excitation
                                    vector scaled by gain (Q0)       */      
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/
#ifdef VOIP_DORA_NELP
 void nelp_random_excit_fx(Word16 *seed, Word16 *Gains, Word16 *out1, Word16 numsamples)
#else
 void nelp_random_excit_fx(Word16 *seed, Word16 *Gains, Word16 *out1)
#endif

{
  Word16 i,j,k1,k2;
  Word16 tmp[16], tmp1[16], tmpf,L16;
  Word16 I[16], tmpi;
  Word32 L32;
#ifdef VOIP_DORA_NELP
  for (i=0;i<numsamples/16;i++) {
#else
  for (i=0;i<10;i++) {
#endif
    for (j=0;j<16;j++) {
      L32 = L_mult(*seed,0x0209);  //L32 = *seed*521;
      L32 = L_shr(L32,1); //to undo the extra lshift within L_mult
      
      L16 = extract_l(
                      L_add(L32,0x00000103L));
#ifdef WMOPS_FX
          Nmove16(3);
#endif
      *seed=L16;   //Q0
      tmp[j]=*seed; //Q15, tmp[j]=*seed/32768

      tmp1[j]=abs_s(tmp[j]);
      I[j]=j;
  
    }

   

   for (k1=0;k1<15;k1++) {
      for (k2=add(k1,1);k2<16;k2++) {
#ifdef WMOPS_FX
  test();
  Nmove16(6);
#endif
        if (tmp1[k2]>tmp1[k1]) {
          tmpi=I[k2]; 
          tmpf=tmp1[k2];
          tmp1[k2]=tmp1[k1]; 
          I[k2]=I[k1];
          tmp1[k1]=tmpf; I[k1]=tmpi;
        }
      }
   }
#ifdef VOIP_DORA_NELP
        //Warping: Changes to support when NUM_SAMPLES > 160
        if (i < 10)
        {
                for (j=0;j<4;j++)
                {
                        //out[add(shl(i,4),I[j])]=mult_r(Gains[i],sqrt(3)*tmp[I[j]];
                        out1[shl(i,4)+I[j]]=mult_r(tmp[I[j]],extract_h(L_shl(L_mult(Gains[i],0x6EDA),1)));
                        /* out1 in Q0 SP, 0x6EDA is sqrt(3) in Q14, tmp in Q15 */
        }
    }
        else
        {
          
                for (j=0;j<4;j++)
                {
                        //out[add(shl(i,4),I[j])]=mult_r(Gains[9],sqrt(3)*tmp[I[j]];
                        out1[shl(i,4)+I[j]]=mult_r(tmp[I[j]],extract_h(L_shl(L_mult(Gains[9],0x6EDA),1)));
                        /* out1 in Q0 SP, 0x6EDA is sqrt(3) in Q14, tmp in Q15 */
        
         }
     }
        //End: Warping: Changes to support when NUM_SAMPLES > 160
#else
        for (j=0;j<4;j++)
                {
                        //out[add(shl(i,4),I[j])]=mult_r(Gains[i],sqrt(3)*tmp[I[j]];
                        out1[shl(i,4)+I[j]]=mult_r(tmp[I[j]],extract_h(L_shl(L_mult(Gains[i],0x6EDA),1)));
                        /* out1 in Q0 SP, 0x6EDA is sqrt(3) in Q14, tmp in Q15 */
        }
#endif
     
#ifdef WMOPS_FX
  Nmove16(16);
#endif
    for (;j<16;j++)
      out1[shl(i,4)+I[j]]=0;
   
  }
 
}

/*===================================================================*/
/* FUNCTION      :  nelp_shape_excit_fx                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function updates filter memory and
                    filters residual to desired shape                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */

/*         _ (Word16)      fid : Decision variabe Q0                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ None                                                    */ 
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ (Word16*)     inp : Input residual signal (Q0),
                           output filtered residual                  */
/*         _ (Word16*)     shape2_filt_mem_fx, shape3_filt_mem_fx    */
/*                         Filter memories (Q-1)
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/
#ifdef VOIP_DORA_NELP
void nelp_shape_excit_fx(Word16 fid, Word16 *inp, Word16 *shape2_filt_mem_fx, Word16 *shape3_filt_mem_fx, Word16 size)
#else
void nelp_shape_excit_fx(Word16 fid, Word16 *inp, Word16 *shape2_filt_mem_fx, Word16 *shape3_filt_mem_fx)
#endif
{

#ifdef VOIP_DORA  
  Word16 filtRes_fx[FSIZE*2];
#else
  Word16 filtRes_fx[FSIZE];
#endif

  
 switch(fid) {
  case 1:
    // Update other filter memory
#ifdef VOIP_DORA_NELP
    pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx, inp,filtRes_fx,
                 shape3_filt_mem_fx, 10, size, 1);
    // filter the residual to desired shape
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp,inp,
                 shape2_filt_mem_fx, 10, size, 1);
#else
    pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx, inp,filtRes_fx,
		 shape3_filt_mem_fx, 10, FSIZE, 1);

    
    // filter the residual to desired shape
    
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp,inp,
		 shape2_filt_mem_fx, 10, FSIZE, 1);
#endif
    
    break;
  case 2:
    // Update other filter memory
#ifdef VOIP_DORA_NELP
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp,filtRes_fx,
                 shape2_filt_mem_fx, 10, size, 1);
     // filter the residual to desired shape
     pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx, inp , inp,
                 shape3_filt_mem_fx, 10, size, 1);
#else
 // Update other filter memory
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp,filtRes_fx,
		 shape2_filt_mem_fx, 10, FSIZE, 1);


     // filter the residual to desired shape
     pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx, inp , inp,
		 shape3_filt_mem_fx, 10, FSIZE, 1);
#endif

    break;
  default:
    // Update other filter memory
#ifdef VOIP_DORA_NELP   
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp ,filtRes_fx,
                 shape2_filt_mem_fx, 10, size, 1);

    pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx,inp,filtRes_fx,
                 shape3_filt_mem_fx, 10, size, 1);
#else
    pz_filter_fx(shape2_num_coef_fx,shape2_den_coef_fx, inp ,filtRes_fx,
		 shape2_filt_mem_fx, 10, FSIZE, 1);

    pz_filter_fx(shape3_num_coef_fx,shape3_den_coef_fx,inp,filtRes_fx,
		 shape3_filt_mem_fx, 10, FSIZE, 1);

#endif

    break;
 }
}


/*===================================================================*/
/* FUNCTION      :  nelp_encoder_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  NELP encoding for the current frame              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16[])  HPspeech_fx : Input speech signal (Q0)           */
/*    _ (Word16[])  OldlspE_fx  : Last frame quantized lsp (Q15)        */
/*    _ (Word16[])  Oldlsp_nq_fx  : Last frame un-quantized lsp (Q15)   */
/*    _ (Word16[])  lsp_nq_fx  :  Current frame un-quantized lsp (Q15)  */

/*-------------------------------------------------------------------*/
/* GLOBAL OUTPUT ARGUMENTS :                                         */
/*    _ (Word16[]) Excitation_fx : Excitation output signal (Q0)     */
/*    _ (Word16[]) lsp_fx  :  Current frame quantized lsp output(Q15)*/
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16[])  pci_fx,lspi_fx : Interpolated prediction/lsp
                                coefficients (Q12/Q15)                   */
/*    _ (Word32[])  SynMemoryM_fx,  WFmemIIR_fx : Synthesis
                    filter memory (Q16)                              */

/*    _ (Word16[])  WFmemFIR_fx : Prediction filter memory (Q0)      */ 
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
void nelp_encoder_fx()
  
{    
  Word16 i,j; 
  Word16 subframesize;
  Word16 ptr1_fx[FSIZE];
  Word16 *in_fx=residual_fx+GUARD;
  Word32 EL1_fx,EH1_fx,EL2_fx,temp;
  Word40 Lacc,Lacc1;
  Word16 filtRes_fx[FSIZE],fid_fx;
  Word16 expo_EL1,expo_EH1,expo_EL2;
  Word16 reg16,reg16_1;
  Word16 Gains_fx[10];
  Word16 iG1_fx,iG2_fx[2];
  static Word16 seed_fx=0;
  
  Word16 Qout,R_fx16,ind;
  
  UNS_Word16 E1l,E3l;
  Word32 E1h,E3h,R_fx;
  Word32 LEL1,LEL2,LEH1,LEH2;
  Word16 filtRes1_fx[FSIZE];
  
  
  static Word16 bp1_filt_mem_fx[12];
  static Word16 shape1_filt_mem_fx[10];
  static Word16 shape2_filt_mem_fx[10];
  static Word16 shape3_filt_mem_fx[10];
  static Word16 txlpf1_filt1_mem_fx[10];
  static Word16 txlpf1_filt2_mem_fx[10];
  static Word16 txhpf1_filt1_mem_fx[10];
  static Word16 txhpf1_filt2_mem_fx[10];

#ifdef WMOPS_FX
  test();
#endif
  if (lastrateE != 2) {
#ifdef WMOPS_FX
          Nmove16(12+10*7);
#endif
    for (i=0;i<12;i++) bp1_filt_mem_fx[i]=0;
    for (i=0;i<10;i++) shape1_filt_mem_fx[i]=0;
    for (i=0;i<10;i++) shape2_filt_mem_fx[i]=0;
    for (i=0;i<10;i++) shape3_filt_mem_fx[i]=0;
    for (i=0;i<10;i++) txlpf1_filt1_mem_fx[i]=0;
    for (i=0;i<10;i++) txlpf1_filt2_mem_fx[i]=0;
    for (i=0;i<10;i++) txhpf1_filt1_mem_fx[i]=0;
    for (i=0;i<10;i++) txhpf1_filt2_mem_fx[i]=0;
  }

  // LSP Quantization
  Word16 kn=2;
   
  enc_lsp_vq_16_fx(lsp_nq_fx, SScratch_fx, lsp_fx);
    
  for (i=0; i<kn; i++)
    data_packet.LSP_IDX[i]=SScratch_fx[i];

  for (i=0;i<ORDER;i++) cbprevprev_E_fx[i]=cbprev_E_fx[i]=lsp_fx[i];

#ifdef LSP_SPRD
 lsp_spread(lsp_fx);
#endif

  // Start Unvoiced/NELP Processing
  for(i=0,Lacc=0.0;i<FSIZE;i++)
    Lacc=L_mac40(Lacc,in_fx[i],in_fx[i]); //E1 as Word40
  Lacc=L_shr40(Lacc,1); // Q0
  
   
  E1l=extract_l40(Lacc);
  E1h=(Word32) (L_shr40(Lacc,16));
   
  pz_filter_fx(txlpf1_num_coef_fx,txlpf1_den_coef_fx, in_fx, filtRes_fx,
                txlpf1_filt1_mem_fx, 10, FSIZE, 3);

 
  for(i=0,Lacc1=0.0;i<FSIZE;i++)
    Lacc1=L_mac40(Lacc1,filtRes_fx[i],filtRes_fx[i]);
  expo_EL1 = norm32_l40(Lacc1);
  EL1_fx = (Word32) L_shl40(Lacc1,expo_EL1);
  expo_EL1 = add(expo_EL1,1);

  pz_filter_fx(txhpf1_num_coef_fx,txhpf1_den_coef_fx, in_fx, filtRes_fx,
                txhpf1_filt1_mem_fx, 10, FSIZE, 3);

  for(i=0,Lacc1=0.0;i<FSIZE;i++)
    Lacc1=L_mac40(Lacc1,filtRes_fx[i],filtRes_fx[i]);
  expo_EH1 = norm32_l40(Lacc1);
  EH1_fx = (Word32) L_shl40(Lacc1,expo_EH1);
  expo_EH1 = add(expo_EH1,1);
  
  /* Reset accumulated shift */
#ifdef WMOPS_FX
          Nmove16(2);
#endif
  accshift_fx = 0;
  dpm = 0;

  for (i=0;i<10;i++) {
    reg16=shl(i,4); 
    reg16_1=add(reg16,16);

#ifdef WMOPS_FX
          move32();
#endif
    Lacc1=0.0;
    for (j=reg16;j<reg16_1;j++) 
      Lacc1 = L_mac40 (Lacc1,in_fx[j],in_fx[j]); //Q1 DP
    Gains_fx[i] = round32_16(L_shr(dsp_sqrt_lut((Word32)L_sat32_40(L_shr40(Lacc1,1)),0),2)); // Q0 , L_shr by 2 is for sqrt(1/16)
  }
  
  quantize_uvg_fx(Gains_fx,iG1_fx,iG2_fx,Gains_fx);

#ifdef VOIP_DORA_NELP
  nelp_random_excit_fx(&seed_fx,Gains_fx,ptr1_fx,FSIZE);
#else
  nelp_random_excit_fx(&seed_fx,Gains_fx,ptr1_fx);
#endif


  pz_filter_fx(bp1_num_coef_fx,bp1_den_coef_fx, ptr1_fx, ptr1_fx,
                bp1_filt_mem_fx, 12, FSIZE, 4);
 
  
  for(i=0,Lacc1=0.0;i<FSIZE;i++)
    Lacc1=L_mac40(Lacc1,ptr1_fx[i],ptr1_fx[i]);
  Lacc1=L_shr40(Lacc1,1); // Q0
  E3l=extract_l40(Lacc1);
  E3h=(Word32) (L_shr40(Lacc1,16));

  pz_filter_fx(shape1_num_coef_fx,shape1_den_coef_fx, ptr1_fx, ptr1_fx,
        shape1_filt_mem_fx, 10, FSIZE, 1);

  for(i=0,Lacc1=0.0;i<FSIZE;i++)
    Lacc1=L_mac40(Lacc1,ptr1_fx[i],ptr1_fx[i]);
     Lacc1=L_shr40(Lacc1,1); // Q0 E2 in Lacc1
     Lacc=L_add40(L_shl40(E1h,16),E1l); // E1 in 40 bit
     
  R_fx=sqrt_divide_dp(Lacc, Lacc1,0,&Qout); // R_fx in Qout

 
  Qout=sub(28,Qout);
  R_fx16=round32_16(L_shl(R_fx,Qout)); // in Q12
  
  for (i=0;i<FSIZE;i++)
    {
      filtRes_fx[i]=extract_h((Word32) L_sat32_40(L_add40(L_shl40(L_mult_su(ptr1_fx[i],R_fx16),4),0x00008000))); // Q0
      filtRes1_fx[i]=filtRes_fx[i]; // for back-up
    } 
    
  pz_filter_fx(txlpf1_num_coef_fx,txlpf1_den_coef_fx, filtRes_fx,filtRes_fx,
                 txlpf1_filt2_mem_fx, 10, FSIZE, 3);

  for (i=0, Lacc=0.0;i<FSIZE;i++) Lacc=L_mac40(Lacc,filtRes_fx[i],filtRes_fx[i]);
  
  expo_EL2 = norm32_l40(Lacc);
  EL2_fx = (Word32) L_shl40(Lacc,expo_EL2);
  expo_EL2 = add(expo_EL2,1);

  // use backed up filtRes1_fx instead of above

  pz_filter_fx(txhpf1_num_coef_fx,txhpf1_den_coef_fx, filtRes1_fx,filtRes_fx,
                 txhpf1_filt2_mem_fx, 10, FSIZE, 3);


  for (i=0, Lacc=0.0;i<FSIZE;i++) Lacc=L_mac40(Lacc,filtRes_fx[i],filtRes_fx[i]);


  Lacc = L_shr40(Lacc,1);
  
#ifdef WMOPS_FX
  test();
  move32();
#endif
  if (Lacc == 0) Lacc = 1;

  LEH2= log10_lut(Lacc); //Q23

 
  LEH2=L_sub(LEH2,L_deposit_h(384)); // LEH2=10log(EH2) - 3 ,  Q23

     
  temp = L_shr(EH1_fx,expo_EH1);
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (temp==0) temp=1;
 
  LEH1 = log10_lut(temp); //Q23
 
    
  LEH1=L_sub(LEH1,LEH2); // LEH1=10log10(EH1/EH2)+3,Q23


  temp = L_shr(EL2_fx,expo_EL2);
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (temp==0) temp=1;
  

  LEL2 = log10_lut(temp); // 10log10(EL2), Q23

  LEL2=L_sub(LEL2,L_deposit_h(384)); // LEL2=10log(EL2) - 3 ,  Q23

  temp = L_shr(EL1_fx,expo_EL1);
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (temp==0) temp=1;
  
  LEL1 = log10_lut(temp); // 10log10(EL1) in Q23

  LEL1=L_sub(LEL1,LEL2); // LEL1=10log10(EL1/EL2)+3,Q23
  
   
  fid_fx=0;
   

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (LEL1<0) fid_fx=1;
  else if (LEH1<0) fid_fx=2;

#ifdef VOIP_DORA_NELP   
  nelp_shape_excit_fx(fid_fx, ptr1_fx, shape2_filt_mem_fx, shape3_filt_mem_fx,FSIZE);
#else
  nelp_shape_excit_fx(fid_fx, ptr1_fx, shape2_filt_mem_fx, shape3_filt_mem_fx);
#endif
 
  for (i=0, Lacc1=0;i<FSIZE;i++)
    Lacc1=L_mac40(Lacc1,ptr1_fx[i],ptr1_fx[i]); //E2
  Lacc1=L_shr40(Lacc1,1);

  
  Lacc=L_add40(L_shl40(E3h,16),E3l); // E3 in 40 bit
    

  R_fx=sqrt_divide_dp(Lacc, Lacc1,0,&Qout); // R_fx in Qout
  Qout=sub(27,Qout);
  

  R_fx16=extract_h(L_shl(R_fx,Qout)); // in Q(27-16)=11
  

  for (i=0;i<FSIZE;i++)
    ptr1_fx[i]=extract_h((Word32) L_sat32_40(L_add40(L_shl40(L_mult_su(ptr1_fx[i],R_fx16),5),0x00008000)));
  

#ifdef WMOPS_FX
          Nmove16(3);
#endif
  data_packet.NELP_GAIN_IDX[0][0]=iG1_fx;

  data_packet.NELP_GAIN_IDX[1][0]=iG2_fx[0];

  data_packet.NELP_GAIN_IDX[1][1]=iG2_fx[1];

  data_packet.NELP_FID=fid_fx;  

  /* Update target filter memory and reconstruction filter memory */
  for (i=0; i<NoOfSubFrames; i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;
    
    /* interpolate lsp */
    Interpol_fx(OldlspE_fx,lsp_fx,lspi_fx, i);
    
    
    Interpol_fx(Oldlsp_nq_fx,lsp_nq_fx,lspi_nq_fx, i);
    
    /* Convert lsp to PC */
   
    lsp2lpc_fx(lspi_fx, pci_fx, pci_fx); //Q12
    
    
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx); //Q12

#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(residual_fx[j+GUARD+shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1)],SCALE);

#endif
    

    synthesis_filter_fx (pci_nq_fx , Scratch_fx , origm_fx ,SynMemoryM_fx,ORDER,subframesize,3);

#if 1
    
    for (j=0;j<subframesize;j++) origm_fx[j]=shl(origm_fx[j],SCALE);
#endif
    

     
    /* Weighting filter */
   
    weight_fx(pci_nq_fx , gamma90_tab, wpci_gma1_fx, ORDER); // Q(12+15+1-16) = Q12
  
    prediction_filter_fx(wpci_gma1_fx, origm_fx ,Scratch_fx,WFmemFIR_fx, ORDER , subframesize, 4);
   
    
    weight_fx(pci_nq_fx , gamma50_tab , wpci_gma2_fx, ORDER); // Q(12+15+1-16) = Q12
    
#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(Scratch_fx[j],SCALE);
#endif 
    
    
    synthesis_filter_fx(wpci_gma2_fx, Scratch_fx, worigm_fx , WFmemIIR_fx,
                        ORDER, subframesize, 3);

#if 1
    
    for (j=0;j<subframesize;j++) worigm_fx[j]=shl(worigm_fx[j],SCALE);
#endif
    

    ind=shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1);

     ZeroInput_fx(ptr1_fx+ind, pci_fx, wpci_gma1_fx, wpci_gma2_fx, zir_fx, subframesize, 1);
      
  }
  
#ifdef WMOPS_FX
          Nmove16(ACBMemSize);
#endif
  for (j=0;j<ACBMemSize;j++) Excitation_fx[j]=ptr1_fx[sub(FrameSize,ACBMemSize)+j];

#ifdef WMOPS_FX
          Nmove16(2);
#endif
  data_packet.PACKET_RATE=2;data_packet.MODE_BIT=0;
}

/*===================================================================*/
/* FUNCTION      :  nelp_decoder_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  NELP decoding for the current frame              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Struct)   data_packet                                       */
/*    _ (Word16[]) OldlspD_fx  : Last frame quantized lsp (Q15)      */
/*    _ (Word16[]) Oldlsp_nq_fx: Last frame un-quantized lsp (Q15)   */
/*    _ (Word16[]) lsp_nq_fx   :  Current frame un-quantized lsp (Q15)*/
/*    _ (Word16)   postfilter  : Postfilter on/off flag              */
/*    _ (Word16)   erasure     : Erasure decoder on/off flag         */  
#ifdef VOIP_DORA_NELP
/*    _ (Word16)   time_warp_fraction: time wrping factor            */
/*    _ (Word16)   obuf_len    :obuf Length                          */
#endif
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*    _ (Word16[]) outFbuf_fx : Decoded speech signal (one frame)(Q0)*/
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16[])  pci_fx,lspi_fx : Interpolated prediction/lsp
                                coefficients (Q12/Q15)               */
/*    _ (Word32[])  SynMemory_fx : Synthesis filter memory (Q16)     */

/*    _ (Word16[])  PitchMemoryD_fx:  (Q0)                           */ 
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
#ifdef VOIP_DORA_NELP
void nelp_decoder_fx(Word16 outFbuf_fx[], Word16 post_filter,Word16 erasure, Word16     time_warp_fraction, short*    obuf_len)
#else
void nelp_decoder_fx(Word16 outFbuf_fx[], Word16 post_filter,Word16 erasure) 
#endif

{
  Word16 subframesize;
  
  Word16 i,j;
    
  Word40 Lacc,Lacc1;
  Word16 fid_fx=0;
  Word16 Gains_fx[10],p,tempG;
  Word16 iG1_fx,iG2_fx[2];
  static Word16 seed_fx=0;
  static Word16 seed_fxe=0;
#ifdef VOIP_DORA_NELP
  Word16 ptr1_fx[160*2],Qout,R_fx16,ind; 
#else
    Word16 ptr1_fx[160],Qout,R_fx16,ind; 
#endif
  Word32 R_fx;
#ifdef VOIP_DORA_NELP
  Word16 scaled_res[SubFrameSize*2];
#else
  Word16 scaled_res[SubFrameSize];
#endif

#if 0 
  time_warp_fraction = 16384;
#endif

     
  static Word16 bp1_filt_mem_fx[12];
  static Word16 shape1_filt_mem_fx[10];
  static Word16 shape2_filt_mem_fx[10];
  static Word16 shape3_filt_mem_fx[10];
#ifdef WMOPS_FX
      WMP_fwc_fx("Start NELP decoder .......................................");
#endif
  
#ifdef WMOPS_FX
  test();
#endif
  if (erasure)
    {  
#ifdef WMOPS_FX
  test();
  Nmove16(12+10*3);
#endif
     if (!prev_frame_error) {
       for (i=0;i<12;i++) bp1_filt_mem_fx[i]=0;
       for (i=0;i<10;i++) shape1_filt_mem_fx[i]=0;
       for (i=0;i<10;i++) shape2_filt_mem_fx[i]=0;
       for (i=0;i<10;i++) shape3_filt_mem_fx[i]=0;
     }
    }
  else
   {
#ifdef WMOPS_FX
  test();
  logic16();
  Nmove16(12+3*10);
#endif

        if (lastrateD !=2 || prev_frame_error) {
        for (i=0;i<12;i++) bp1_filt_mem_fx[i]=0;
        for (i=0;i<10;i++) shape1_filt_mem_fx[i]=0;
        for (i=0;i<10;i++) shape2_filt_mem_fx[i]=0;
        for (i=0;i<10;i++) shape3_filt_mem_fx[i]=0;
   }

   }


#ifdef VOIP_DORA_NELP
  //Additions to support Warping
  int NUM_SAMPLES = 160;
  if (time_warp_fraction > 0x4000)
                NUM_SAMPLES = 192;
  else if (time_warp_fraction < 0x4000)
                NUM_SAMPLES = 128;
  *obuf_len = NUM_SAMPLES;
  //printf ("\n NELP warping %d", NUM_SAMPLES);
  //End: Additions to support Warping
#endif

#ifdef WMOPS_FX
  test();
#endif
  if (!erasure)
     {
  // LSP De-Quantization
  dec_lsp_vq_16_fx((short int *)data_packet.LSP_IDX, lsp_fx);
  
  /* Check for monontonic LSP */
  for (j=1; j<ORDER; j++) {
#ifdef WMOPS_FX
  test();
  move16();
#endif
          if (lsp_fx[j] <= lsp_fx[j - 1]) errorFlag=1;
  }

#ifdef WMOPS_FX
          Nmove16(2*ORDER);
#endif
  for (i=0;i<ORDER;i++) cbprevprev_D_fx[i]=cbprev_D_fx[i]=lsp_fx[i];  

#ifdef LSP_SPRD
  lsp_spread(lsp_fx);
#endif 
  
  // Do Unvoiced/NELP Decoding
    
#ifdef WMOPS_FX
          Nmove16(4);
#endif
  iG1_fx=data_packet.NELP_GAIN_IDX[0][0];

  iG2_fx[0]=data_packet.NELP_GAIN_IDX[1][0];

  iG2_fx[1]=data_packet.NELP_GAIN_IDX[1][1];
  
  fid_fx=data_packet.NELP_FID;

  dequantize_uvg_fx(iG1_fx,iG2_fx,Gains_fx);
     }

 else
 {
  
  //for (j=0;j<ORDER;j++) lsp_fx[j]=OldlspD_fx[j]; // explicitly copy ( done in fer.cc)
  for (i=0,Lacc=0.0;i<SubFrameSize;i++)
   Lacc = L_mac40(Lacc,PitchMemoryD_fx[sub(ACBMemSize-1,i)],PitchMemoryD_fx[sub(ACBMemSize-1,i)]);

  Lacc=L_shr40(Lacc,1);
  
  tempG = mult_r(round32_16(dsp_sqrt_lut((Word32)L_sat32_40(Lacc),0)),4459);
   // Q0 , 4459= sqrt(1/54) in Q15
  tempG=mult_r(tempG,26214); //Q0
  //Some scale down of energy since it is an erasure

#ifdef WMOPS_FX
          Nmove16(ORDER);
#endif
  for(i=0;i<ORDER;i++)
    Gains_fx[i]=tempG;
  
 }
 
#ifdef WMOPS_FX
  test();
#endif

#ifdef VOIP_DORA_NELP
  if (!erasure)
      nelp_random_excit_fx(&seed_fx,Gains_fx,ptr1_fx,NUM_SAMPLES);
  else
     nelp_random_excit_fx(&seed_fxe,Gains_fx,ptr1_fx,NUM_SAMPLES);
#else
 if (!erasure)
     nelp_random_excit_fx(&seed_fx,Gains_fx,ptr1_fx);
  else
     nelp_random_excit_fx(&seed_fxe,Gains_fx,ptr1_fx);
#endif


#ifdef VOIP_DORA_NELP
  pz_filter_fx(bp1_num_coef_fx,bp1_den_coef_fx, ptr1_fx, ptr1_fx,
                bp1_filt_mem_fx, 12, NUM_SAMPLES, 4);
#else
  pz_filter_fx(bp1_num_coef_fx,bp1_den_coef_fx, ptr1_fx, ptr1_fx,
                bp1_filt_mem_fx, 12, FSIZE, 4);
#endif


#ifdef VOIP_DORA_NELP
  for(i=0,Lacc1=0.0;i<NUM_SAMPLES;i++)
#else
  for(i=0,Lacc1=0.0;i<FSIZE;i++)
#endif
    Lacc1=L_mac40(Lacc1,ptr1_fx[i],ptr1_fx[i]);
  Lacc1=L_shr40(Lacc1,1); // Q0

#ifdef VOIP_DORA_NELP
pz_filter_fx(shape1_num_coef_fx,shape1_den_coef_fx, ptr1_fx, ptr1_fx,
		shape1_filt_mem_fx, 10, NUM_SAMPLES, 1);
#else
    pz_filter_fx(shape1_num_coef_fx,shape1_den_coef_fx, ptr1_fx, ptr1_fx,
		shape1_filt_mem_fx, 10, FSIZE, 1);
#endif

#ifdef VOIP_DORA_NELP   
   nelp_shape_excit_fx(fid_fx, ptr1_fx, shape2_filt_mem_fx, shape3_filt_mem_fx,NUM_SAMPLES);
#else
   nelp_shape_excit_fx(fid_fx, ptr1_fx, shape2_filt_mem_fx, shape3_filt_mem_fx);
#endif

#ifdef VOIP_DORA_NELP 
  for(i=0,Lacc=0.0;i<NUM_SAMPLES;i++)
#else
  for(i=0,Lacc=0.0;i<FSIZE;i++)
#endif
    Lacc=L_mac40(Lacc,ptr1_fx[i],ptr1_fx[i]);
    Lacc=L_shr40(Lacc,1); // Q0
  
  R_fx=sqrt_divide_dp(Lacc1, Lacc,0,&Qout); // R_fx in Qout

  
  Qout=sub(27,Qout);
  R_fx16=extract_h(L_shl(R_fx,Qout)); // in Q11
  
  
#ifdef VOIP_DORA_NELP 
  for (i=0;i<NUM_SAMPLES;i++)
#else
  for(i=0;i<FSIZE;i++)
#endif
    ptr1_fx[i]=extract_h((Word32) L_sat32_40(L_add40(L_shl40(L_mult_su(ptr1_fx[i],R_fx16),5),0x00008000))); // Q0
    
    
  p=0;

#ifdef VOIP_DORA_NELP
  //More support for Warping
  int temp_samples_count = 0;
  //End: More support for Warping
  Word16 temp1=0, temp2=0;
  Word16 temp=0;
#endif

  for (i = 0; i < NoOfSubFrames; i++)
    {
#ifdef WMOPS_FX
  test();
#endif


#ifndef VOIP_DORA_NELP
   if (i < 2)
      subframesize = SubFrameSize - 1;
    else
      subframesize = SubFrameSize;

    for (j=0;j<subframesize;j++)
      {
      ind=shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1);      
      PitchMemoryD_fx[ACBMemSize+j]=ptr1_fx[ind+j];
      }
                        
#else

//Warping: Change in size of subframesize, not always equal to 53 since residual size may not be = 160
        temp1 = div_int_sp(NUM_SAMPLES, 3,  temp);
        temp2 = add (temp1,1);
        if (temp == 0)
                subframesize = temp1;
        else if (temp == 1)
        {
                if (i < 2)
                        subframesize = temp1;
                else
                        subframesize = temp2;
        }
        else if (temp == 2)
        {
                if (i < 1)
                        subframesize = temp1;
                else
                        subframesize = temp2;
        }
        //End Change in size of subframesize, not always equal to 53
    for (j=0;j<subframesize;j++)
      {
          PitchMemoryD_fx[ACBMemSize+j]=ptr1_fx[temp_samples_count+j];
      }

#endif
#ifdef WMOPS_FX
          Nmove16(ACBMemSize);
#endif
    for (j = 0; j < ACBMemSize; j++)
      PitchMemoryD_fx[j] = PitchMemoryD_fx[j+subframesize];
       
    /* Linear interpolation of lsp */
    Interpol_fx(OldlspD_fx, lsp_fx, lspi_fx, i);

    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, prev_pci_fx, pci_fx); //Q12
   
       for(j=0;j<subframesize;j++)
        scaled_res[j]=shr(PitchMemoryD_fx[j+ACBMemSize],SCALE);  

    /* Synthesis of decoder output signal and postfilter output signal */
    synthesis_filter_fx(pci_fx , scaled_res , DECspeech_fx ,SynMemory_fx,ORDER,subframesize,3);
  

#ifdef WMOPS_FX
  test();
#endif
   if (post_filter) {
        Post_Filter_fx(DECspeech_fx, lspi_fx, pci_fx, DECspeechPF_fx, 
                       0x140000 ,0, subframesize); // 20 in Q15
   }
    else
      {
        for(j=0;j<subframesize;j++)
          DECspeechPF_fx[j]=DECspeech_fx[j];
      }
   

         for(j=0;j<subframesize;j++)
             DECspeechPF_fx[j]=shl(DECspeechPF_fx[j],SCALE);


   /* Write decoder output and variables to files */
   for (j = 0; j < subframesize; j++) {
        outFbuf_fx[p] = DECspeechPF_fx[j];
        p++;
        
   }
#ifdef VOIP_DORA_NELP    
        temp_samples_count =add(temp_samples_count, subframesize); //Addition to support Warping
#endif

    }

#if 1
  for(j=0;j<ORDER;j++)
    prev_pci_fx[j]=pci_fx[j];
#endif
  
#ifdef WMOPS_FX
      WMP_fwc_fx("NELP Decodex..............................................");
#endif

#if 0
   printf("%d %d\n",NUM_SAMPLES,decode_fcnt+1);
#endif


  }
 
