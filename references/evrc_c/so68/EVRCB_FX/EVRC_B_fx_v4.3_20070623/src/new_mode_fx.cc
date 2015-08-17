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
#include "mode_filt_coef.h"
#include "proto_fx.h"
#include "globs_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX
Word16 pattern_m;
/*===================================================================*/
/* FUNCTION      :  new_mode_decision_fx ()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Mode decision routine for EVRC-B                    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16)    va  : Voice activity flag (0=inactive, 1=active)  */
/*   _ (Word16)    prev_voiced : voiced flag of previous frame       */
/*   _ (Word16)    prev_mode   : mode decision of previous frame     */
/*   _ (Word16 []) nacf_ap_fx: NACF values around pitch, Q15         */
/*   _ (Word16 []) curr_ns_snr_fx: SNR from noise suppressor, Q9     */
/*   _ (Word16 []) curr_snr_fx: SNR from voice activity detector, Q9 */
/*   _ (Word16 []) in_fx   : input speech, Q0                        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                                                                   */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : MODE_FX structure                              */
/*===================================================================*/
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
MODE_FX new_mode_decision_fx(Word16 va, Word16 prev_voiced,Word16 prev_mode, Word16 *nacf_ap_fx, Word16 *curr_snr_fx, Word16 *in_fx)
#else
MODE_FX new_mode_decision_fx(Word16 va, Word16 prev_voiced,Word16 prev_mode, Word16 *nacf_ap_fx, Word16 *curr_ns_snr_fx, Word16 *curr_snr_fx, Word16 *in_fx)
#endif
{
  Word16 VOICEDTH;    // voiced threshold
  Word16 LOWVOICEDTH; // quasi-voiced threshold
  Word16 UNVOICEDTH;  // unvoiced threshold

  static Word16 lpf_filt_mem[12]={0,0,0,0,0,0,0,0,0,0,0,0};
  static Word16 hpf_filt_mem[12]={0,0,0,0,0,0,0,0,0,0,0,0};
  
  static Word32 Eprev=0;  // previous frame energy, Q-4

  static Word16 vcount=0;
  static Word32 vE[3]={0,0,0}, vEav=0;  // Q-4
  static Word32 vEprev=6250000;   // 1E8 in Q-4
  //  static Word16 prev_snr[2], prev_ns_snr[2]; // Q9
  
  Word16 zcr;  // Zero crossing rate, Q0
  Word16 out[FSIZE];
  Word32 E, Enext;  // current frame and look-ahead frame energy, Q-4
  Word16 vER, vER2; // Energy ratio, Q9
  Word16 bER;       // band energy ratio, Q9
  Word16 maxsfe_idx=4;  // peaky subframe index, 

  MODE_FX m;
  Word16 i,j;
  Word16 dl, dh, cl;
  Word32 Ltemp1, Ltemp2;
  Word40 Lacc;

  /* Pre-computed conditions */
  Word16 Zdif40;    // zcr-40
  Word16 Zdif60;    // zcr-60
  Word16 Zdif70;    // zcr-70
  Word16 Zdif80;    // zcr-80
  Word16 Zdif90;    // zcr-90
  Word16 Zdif100;   // zcr-100
  Word16 Zdif105;   // zcr-105
  Word16 Zdif110;   // zcr-110

  Word16 Edif_cp;  // E-Eprev, Difference of current and previous frame energy
  Word16 Edif_cp2; // 2*Eprev-E;
  Word16 Edif_cp5; // 5*Eprev-E;
  
  //Word16 Ndifu;     // nacf_fx-UNVOICEDTH
  Word16 Ndif4u;    // nacf_ap_fx[4]-UNVOICEDTH
  Word16 Ndif4lv;   // nacf_ap_fx[4]-LOWVOICEDTH
  Word16 Ndif4v;    // nacf_ap_fx[4]-VOICEDTH
  Word16 Ndif3u;    // nacf_ap_fx[3]-UNVOICEDTH
  Word16 Ndif3lv;   // nacf_ap_fx[3]-LOWVOICEDTH
  Word16 Ndif3v;    // nacf_ap_fx[3]-VOICEDTH
  Word16 Ndif2u;    // nacf_ap_fx[2]-UNVOICEDTH
  Word16 Ndif2lv;   // nacf_ap_fx[2]-LOWVOICEDTH
  Word16 Ndif2v;    // nacf_ap_fx[2]-VOICEDTH
  Word16 Ndif1u;    // nacf_ap_fx[1]-UNVOICEDTH
  Word16 Ndif1lv;   // nacf_ap_fx[1]-LOWVOICEDTH
  Word16 Ndif1v;    // nacf_ap_fx[1]-VOICEDTH
  Word16 Ndif0u;    // nacf_ap_fx[0]-UNVOICEDTH
  Word16 Ndif0lv;   // nacf_ap_fx[0]-LOWVOICEDTH
  Word16 Ndif0v;    // nacf_ap_fx[0]-VOICEDTH
  Word16 Ndif43;    // nacf_ap_fx[4]-nacf_ap_fx[3]
  Word16 Ndif32;    // nacf_ap_fx[3]-nacf_ap_fx[2]
  Word16 Ndif21;    // nacf_ap_fx[2]-nacf_ap_fx[1]
  Word16 Ndif10;    // nacf_ap_fx[1]-nacf_ap_fx[0]
  
  Word16 Vdif3;    // vER+3
  Word16 Vdif4;    // vER+4
  Word16 Vdif5;    // vER+5
  Word16 Vdif6;    // vER+6
  Word16 Vdif10;    // vER+10
  Word16 Vdif12;    // vER+12
  Word16 Vdif13;    // vER+13
  Word16 Vdif14;    // vER+14
  Word16 Vdif15;    // vER+15
  Word16 Vdif16;    // vER+16
  Word16 Vdif18;    // vER+18
  Word16 Vdif20;    // vER+20
  Word16 Vdif21;    // vER+21
  Word16 Vdif22;    // vER+22
  Word16 Vdif25;    // vER+25
  Word16 Vdif30;    // vER+30

  Word16 V2dif15;   // vER2+15

  Word16 Bdifm30;    // bER+30
  Word16 Bdifm7;    // bER+7
  Word16 Bdifm5;    // bER+5
  Word16 Bdifm3;    // bER+3
  Word16 Bdifm2;    // bER+2
  Word16 Bdifm1;    // bER+1
  Word16 Bdif1;    // bER-1
  Word16 Bdif2;    // bER-2
  Word16 Bdif3;    // bER-3
  Word16 Bdif4;    // bER-4
  Word16 Bdif5;    // bER-5
  Word16 Bdif6;    // bER-6
  
  Word16 Sdif0;    // curr_ns_snr_fx[0]-SNR_THLD
  Word16 curr_snr_diff;
  Word16 Idif;    // maxsfe_idx-SFNUM

  static int FirstTime=1;

  if (FirstTime == 1)
    {
      FirstTime=0;
      if (Eargs->operating_point==0)
	pattern_m=1000;
      else
	pattern_m=0; 
    }

#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
  Sdif0=sub(curr_snr_fx[0],SNR_THLD); // Q9 Dependence on NS SNR removed and only VAD SNR is used
#else
  Sdif0=sub(curr_ns_snr_fx[0],SNR_THLD); // Q9
#endif
  

  curr_snr_diff=sub(curr_snr_fx[0],curr_snr_fx[1]);  // Q9
  
  VOICEDTH=24576;    // 0.75 in Q15
  LOWVOICEDTH=18022; // 0.55 in Q15
  UNVOICEDTH=11469;  // 0.35 in Q15
#ifdef WMOPS_FX
  test();
  Nmove16(3);
#endif
  if (Sdif0<0) {
    VOICEDTH=21299;    // 0.65 in Q15
    LOWVOICEDTH=16384; // 0.5 in Q15
    UNVOICEDTH=11469;  // 0.35 in Q15
  }

#ifdef WMOPS_FX
		move16();
#endif
  zcr=0;
  for (i=0;i<FSIZE-1;i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (L_mult(in_fx[i],in_fx[i+1])<0) zcr=add(zcr,1);
  }
  
  for (i=0, Lacc=0;i<FSIZE;i++)
    Lacc=L_mac40(Lacc,in_fx[i],in_fx[i]);
  Lacc=L_shr40(Lacc,5);
  // Lacc is current frame energy in Q-4
  E=(Word32)L_sat32_40(Lacc);
  
  for (i=0, Lacc=0;i<FSIZE;i++)
    Lacc=L_mac40(Lacc,in_fx[i+LOOKAHEAD_LEN],in_fx[i+LOOKAHEAD_LEN]);
  Lacc=L_shr40(Lacc,5);
  // Lacc is look ahead frame energy in Q-4
  Enext=(Word32)L_sat32_40(Lacc);

  /* Compute vER and vER2             */
  /* vER=10*log10(E)-10*log10(vEprev) */
  Ltemp1=log10_lut(E);   // Ltemp1=10*log10(E) in Q23
  Ltemp2=log10_lut(vEprev);
  Lacc=L_sub40(Ltemp1,Ltemp2); // Lacc=vER in Q23
  Lacc=L_add40(L_shl40(Lacc,2),0x08000);
  Lacc=L_sat32_40(Lacc);
  vER=extract_h((Word32)Lacc);
  //  vER=round32_16(L_sat32_40(L_shl40(Lacc,2)));  // vER in Q9

  Ltemp2=log10_lut(vEav);  // Ltemp2=10*log10(vEav), Q23
  Lacc=L_sub40(Ltemp1,Ltemp2); // Lacc=vER2 in Q23
  
  Lacc=L_add40(L_shl40(Lacc,2),0x08000);
  Lacc=L_sat32_40(Lacc);
  vER2=extract_h((Word32)Lacc);
  //  vER2=round32_16(L_sat32_40(L_shl40(Lacc,2))); // vER2 in Q9
  vER2=MIN_FX(20*512,vER2);
  
  /* High-pass and low-pass filter the input speech and calculate */
  /* high band and low band energy                                */
  pz_filter_fx (hpf_num_coef_fx, hpf_den_coef_fx, in_fx, out,
			 hpf_filt_mem, 12, FSIZE, 4);
  
  for (i=0, Lacc=0;i<FSIZE;i++)
    Lacc=L_mac40(Lacc,out[i],out[i]);
  Lacc=L_shr40(Lacc,1);
  // Lacc is high band energy in Q0
  Ltemp2=(Word32)L_sat32_40(log2_lut(Lacc));  // Q26
  
  pz_filter_fx (lpf_num_coef_fx, lpf_den_coef_fx, in_fx, out,
			 lpf_filt_mem, 12, FSIZE, 4);
  
  for (i=0, Lacc=0;i<FSIZE;i++)
    Lacc=L_mac40(Lacc,out[i],out[i]);
  Lacc=L_shr40(Lacc,1);
  // Lacc is low band energy in Q0
  Lacc=log2_lut(Lacc);  // Q26
  
  /* bER=log2(EL/EH)           */
  Lacc=L_sub40(Lacc,Ltemp2);  // Lacc=bER in Q26
  bER=extract_h((Word32)L_sat32_40(L_add40(L_shr40(Lacc,1),0x08000)));  // bER in Q9
  
  Ltemp1=LW_MIN;
  for (i=4; i<SFNUM; i++) {
    for (j=0, Lacc=0; j<FSIZE/SFNUM; j++)
      Lacc=L_mac40(Lacc,in_fx[j+i*FSIZE/SFNUM],in_fx[j+i*FSIZE/SFNUM]);
    Lacc=L_shr40(Lacc,5);  // Lacc is subframe energy, Q-4
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub40(Lacc,Ltemp1)>0) {
      Ltemp1=(Word32)L_sat32_40(Lacc);
      maxsfe_idx=i;
    }
  }
  //  maxsfe_idx=add(maxsfe_idx,1);
  Idif=sub(maxsfe_idx,SFNUM-1);
  
  /* Pre-compute some conditions and store in memory */
  Zdif40=sub(zcr,40);  // Q0
  Zdif60=sub(zcr,60);  // Q0
  Zdif70=sub(zcr,70);
  Zdif80=sub(zcr,80);
  Zdif90=sub(zcr,90);
  Zdif100=sub(zcr,100);
  Zdif105=sub(zcr,105);
  Zdif110=sub(zcr,110);
  
  
  Edif_cp=0;
  Ltemp1=L_sub(E,Eprev);   // E-Eprev
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Ltemp1>0) Edif_cp=1;
  else if (Ltemp1<0) Edif_cp=-1;
  
  Edif_cp2=0;
  Ltemp1=(Word32)L_sat32_40(L_sub40(L_shl40(Eprev,1),E));  // 2*Eprev-E
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Ltemp1>0) Edif_cp2=1;
  else if (Ltemp1<0) Edif_cp2=-1;
  
  Edif_cp5=0;
  Ltemp1=(Word32)L_sat32_40(L_sub40(L_add40(L_shl40(Eprev,2),Eprev),E));  // 5*Eprev-E
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (Ltemp1>0) Edif_cp5=1;
  else if (Ltemp1<0) Edif_cp5=-1;
  
  //Ndifu=sub(nacf_fx,UNVOICEDTH); //0.35
  Ndif4u=sub(nacf_ap_fx[4],UNVOICEDTH); // Q15
  Ndif4lv=sub(nacf_ap_fx[4],LOWVOICEDTH);
  Ndif4v=sub(nacf_ap_fx[4],VOICEDTH);
  Ndif3u=sub(nacf_ap_fx[3],UNVOICEDTH); // Q15
  Ndif3lv=sub(nacf_ap_fx[3],LOWVOICEDTH);
  Ndif3v=sub(nacf_ap_fx[3],VOICEDTH);
  Ndif2u=sub(nacf_ap_fx[2],UNVOICEDTH); // Q15
  Ndif2lv=sub(nacf_ap_fx[2],LOWVOICEDTH);
  Ndif2v=sub(nacf_ap_fx[2],VOICEDTH);
  Ndif1u=sub(nacf_ap_fx[1],UNVOICEDTH); // Q15
  Ndif1lv=sub(nacf_ap_fx[1],LOWVOICEDTH);
  Ndif1v=sub(nacf_ap_fx[1],VOICEDTH);
  Ndif0u=sub(nacf_ap_fx[0],UNVOICEDTH); // Q15
  Ndif0lv=sub(nacf_ap_fx[0],LOWVOICEDTH);
  Ndif0v=sub(nacf_ap_fx[0],VOICEDTH);
  Ndif43=sub(nacf_ap_fx[4],nacf_ap_fx[3]);
  Ndif32=sub(nacf_ap_fx[3],nacf_ap_fx[2]);
  Ndif21=sub(nacf_ap_fx[2],nacf_ap_fx[1]);
  Ndif10=sub(nacf_ap_fx[1],nacf_ap_fx[0]);

  Vdif3=add(vER,3*512);  // Q9
  Vdif4=add(vER,4*512);  // Q9
  Vdif5=add(vER,5*512);  // Q9
  Vdif6=add(vER,6*512);  // Q9
  Vdif10=add(vER,10*512);  // Q9
  Vdif12=add(vER,12*512);  // Q9
  Vdif13=add(vER,13*512);  // Q9
  Vdif14=add(vER,14*512);  // Q9
  Vdif15=add(vER,15*512);  // Q9
  Vdif16=add(vER,16*512);  // Q9
  Vdif18=add(vER,18*512);  // Q9
  Vdif20=add(vER,20*512);  // Q9
  Vdif21=add(vER,21*512);  // Q9
  Vdif22=add(vER,22*512);  // Q9
  Vdif25=add(vER,25*512);  // Q9
  Vdif30=add(vER,30*512);  // Q9

  V2dif15=add(vER2,15*512); // Q9

  Bdifm30=add(bER,30*512);   // Q9
  Bdifm7=add(bER,7*512);   // Q9
  Bdifm5=add(bER,5*512);   // Q9
  Bdifm3=add(bER,3*512);   // Q9
  Bdifm2=add(bER,2*512);   // Q9
  Bdifm1=add(bER,1*512);   // Q9
  Bdif1=sub(bER,1*512);   
  Bdif2=sub(bER,2*512);   
  Bdif3=sub(bER,3*512);   
  Bdif4=sub(bER,4*512);   
  Bdif5=sub(bER,5*512);   
  Bdif6=sub(bER,6*512);

#ifdef WMOPS_FX
  test();
#endif
  if (va==ACTIVE) {
#ifdef WMOPS_FX
  test();
#endif
    if ( Ndif2v>=0 ) { // 2nd subframe NACF high

      switch (prev_mode) {
      case SILENCE:
	m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	if (Ndif3u<0 && Zdif70>0 && bER<0)
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	if (Ndif3u<0 && Zdif100>0 && Vdif20<0)
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  move16();
#endif
	if (Vdif25<0)
	  m.speech_type=UNVOICED;
	  
	break;

      case UNVOICED:
	m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	if (Ndif3u<0 && Zdif100>0 && Vdif20<0 && Edif_cp<0)
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  move16();
#endif
	if (Vdif25<0)
	  m.speech_type=UNVOICED;
	  
	break;
  
      case VOICED:
	m.speech_type=VOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	if ((sub(Ndif21,9830)>0) && Ndif1lv<0 && L_sub40(L_shl40(E,1),Eprev)>0) // 9830=0.3 in Q15
	  m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	if (Vdif18<0 && Ndif3v<0) 
	  m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	if (Vdif30<0 && Edif_cp<0)
	  m.speech_type=UNVOICED;
	break;

      case DOWN_TRANSIENT:
	m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  move16();
#endif
	if (Vdif30<0)
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  move16();
#endif
        if (Edif_cp>0) m.speech_type=TRANSIENT;
	break;

      default:  // previous is transient
	m.speech_type=VOICED;
#ifdef WMOPS_FX
  test();
  logic16();
#endif
	if (sub(Ndif21,14746)>0 || Ndif1u<0 ) // 14746 is 0.45 in Q15
	  m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  logic16();
  move16();
#endif
	if (sub(Ndif10,9830)>0 && Ndif1v<0 && sub(prev_mode,TRANSIENT)!=0) // 9830 is 0.3 in Q15
	  m.speech_type=TRANSIENT;
	
#ifdef WMOPS_FX
  test();
  logic16();
  logic16();
  move16();
#endif
	if (((sub(Ndif10,9830)>0)||(sub(Ndif21,9830)>0)) && Ndif1lv<0 && Ndif0u<0)
	  m.speech_type=TRANSIENT;

	Lacc=E;
	cl=3277;  // cl=0.05 in Q16
	dl=extract_l(vEav);
	dh=extract_h(vEav);
	Ltemp1=(Word32)L_mult_uu(dl,cl);
	Ltemp1=L_add(L_shr(Ltemp1,16),(Word32)L_mult_su(dh,cl)); // Ltemp1=0.05*vEav
	Lacc=L_sub40(Lacc,Ltemp1);
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	if (Lacc<0 && Ndif3v<0)
	  m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	if (Vdif30<0 && Edif_cp<0)
	  m.speech_type=UNVOICED;
	break;
      }
    }    

    else { // 2nd subframe NACF not very high

#ifdef WMOPS_FX
  test();
#endif
      if ( Ndif2u<=0 ) {
	switch (prev_mode) {      
	case SILENCE:
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(6);
  move16();
#endif
	  if (Ndif32>0 && Ndif43>0 && Ndif4lv>0 && Zdif110<0 && Vdif21>0 && (Ndif3u>0 || Bdifm2>0))
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Zdif60<0 && Bdif6>0 && Vdif13>0 && sub(curr_snr_diff,6936)>0)
	    m.speech_type=UP_TRANSIENT;	  // 6936=13.5463 in Q9
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if ((Ndif3lv>0 || Ndif4lv>0) && Bdif3>0 && Zdif60<0 && vER>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif3lv>0 && Ndif4lv>0 && Bdif3>0 && Zdif40<0 && Vdif15>0)
	    m.speech_type=UP_TRANSIENT;
	  break;
	  
	case UNVOICED:
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(6);
  move16();
#endif
	  if (Ndif32>0 && Ndif43>0 && Ndif4lv>0 && Zdif110<0 && Vdif21>0 && (Ndif3u>0 || Bdifm2>0))
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif32>0 && Ndif43>0 && Edif_cp5<0 && Bdif3>0 && Vdif16>0)  // E-5*Eprev>0
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif4v>0 && Bdif3>0 && Zdif80<0 && Edif_cp2<0 && Vdif20>0)  // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif3v>0 && bER>0 && Zdif80<0 && Edif_cp2<0 && Vdif16>0)  // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if ((Ndif3lv>0 || Ndif4lv>0) && bER>0 && Zdif80<0 && Idif==0 && Vdif16>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(6);
  move16();
#endif
	  if (Ndif3u>0 && Ndif4u>0 && Zdif80<0 && Bdif4>0 && Vdif20>0 && curr_snr_diff>0 && Sdif0>=0)  // Clean condition
	    m.speech_type=UP_TRANSIENT;

#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif3lv>0 && Ndif4lv>0 && Zdif80<0 && Edif_cp>0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	  if (Ndif3v>0 && Ndif4v>0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (bER>0 && Zdif80<0 && L_sub40(L_add40(L_add40(L_shl40(Eprev,3),Eprev),Eprev),E)<0 && Vdif10>0)  // E>10*Eprev
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(6);
  move16();
#endif
	  if (((Bdif5>0 && Vdif10>0) || (Bdif3>0 && Vdif6>0) || (bER>0 && Vdif3>0)) && Zdif80<0 && curr_snr_diff>0 && Sdif0>=0) // Clean condition
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Bdifm3>0 && Vdif14>0 && Zdif80<0 && Sdif0<0) //Noisy condition
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Zdif60<0 && Bdif2>0 && Vdif5>0 && Sdif0<0) //Noisy condition
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Bdifm5>0 && Vdif4>0 && Ndif3u>0 && Ndif43>0 && Zdif100<0 && Sdif0<0) // Noisy condition
	    m.speech_type=UP_TRANSIENT;

	  break;

	case DOWN_TRANSIENT:  // following down_transient
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
#endif
	  if (bER>0) {
#ifdef WMOPS_FX
  test();
  logic16();
#endif
	    if (Vdif20>0 && Zdif70<0)
	      m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	    if ((Ndif3lv>0 || Ndif4lv>0) && Edif_cp2<=0 && Vdif15>0)  // E>=2*Eprev
	      m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4v>0 && Vdif15>0 && Edif_cp>0)
	      m.speech_type=TRANSIENT;
	  }
	  else {
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4v>0 && Vdif15>0 && Edif_cp2<0)  // E>2*Eprev
	      m.speech_type=TRANSIENT;	    
	  }
	  break;
    
	case VOICED:
	default:
#ifdef WMOPS_FX
  test();
#endif
	  if (bER>0) {
	    m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	    if (V2dif15<0 && Zdif90<0 && Edif_cp<0 && Ndif3v<0)
	      m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	    if (Vdif25<0 && Edif_cp<0)
	      m.speech_type=UNVOICED;
	  }
	  else {
	    m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4u>0 && Zdif100<0 && Vdif20>0 && Sdif0<0) // Noisy condition
	      m.speech_type=TRANSIENT;	    
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4lv>0 && Zdif100<0 && Vdif20>0 && Sdif0>=0) // Clean condition
	      m.speech_type=TRANSIENT;
	    
#ifdef WMOPS_FX
  test();
  Nlogic16(9);
  move16();
#endif
	    if (((Bdifm1>0 && Vdif12>0 && Zdif80<0) || (Bdifm3>0 && Vdif10>0 && Zdif90<0) || (Bdifm7>0 && Vdif3>0 && Zdif105<0)) && Sdif0<0) // Noisy condition
	      m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	    if (Bdifm3>0 && Vdif10>0 && Zdif90<0 &&  Sdif0>=0) // Clean condition
	      m.speech_type=TRANSIENT;	      
	  }
	  break;
	}
      }
    
      else { // UV<nacf_ap[2]<V, major gray area
	switch (prev_mode) {

	case SILENCE:
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif32>0 && Ndif43>0 && Ndif4lv>0 && Zdif110<0 && Vdif22>0)
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif2lv>0 && curr_snr_diff>0 && Edif_cp2<0 && Zdif70<0 && Vdif20>0) // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Ndif2lv>0 && Ndif3lv>0 && Zdif70<0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if ((Ndif4v>0 || Ndif3v>0) && Ndif3u>0 && Zdif70<0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (sub(Ndif32,9830)>0 && Edif_cp2<0 && Zdif70<0 && bER>0 && Vdif20>0) //E>2*Eprev,  9830 is 0.3 in Q15
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif4v>0 && (Ndif2lv>0 || Ndif3lv>0) && Edif_cp2<0 && curr_snr_diff>0 && Vdif20>0)  // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif3lv>0 && Ndif4lv>0 && Bdif3>0 && Zdif40<0 && Vdif15>0)
	    m.speech_type=UP_TRANSIENT;
	  break;	  

	case UNVOICED:
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif32>0 && Ndif43>0 && Ndif4lv>0 && Zdif110<0 && Vdif22>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if ((Ndif4lv>0 || Ndif3lv>0) && Zdif100<0 && Edif_cp>0 && bER>0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif4v>0 && Zdif100<0 && Edif_cp>0 && Bdif1>0 && Vdif16>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Zdif60<0 && Bdif3>0 && Edif_cp5<0 && sub(curr_snr_diff,4624)>0 && Vdif20>0)  // E>5*Eprev,  4624 is 9.0309 in Q9
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if ((Ndif3u>0 || Ndif4u>0) && Bdif1>0 && Vdif16>0 && Zdif80<0 && add(curr_snr_diff,3083)>0)  // 3083 is 6.0206 in Q9
	    m.speech_type=UP_TRANSIENT;
	      
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (Ndif2lv>0 && curr_snr_diff>0 && Edif_cp2<0 && Zdif70<0 && Vdif20>0) // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Ndif2lv>0 && Ndif3lv>0 && Zdif100<0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if ((Ndif4v>0 || Ndif3v>0) && Ndif3u>0 && Zdif80<0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if ((Ndif3lv>0 || Ndif4lv>0) && bER>0 && Zdif100<0 && Idif==0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif3u>0 && Ndif4u>0 && Zdif100<0 && Vdif3>0 && Bdifm5>0 && Sdif0<0)  // Noisy condition
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Bdif4>0 && Zdif100<0 && Idif==0 && Vdif20>0)
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	  if (sub(Ndif32,9830)>0 && Edif_cp2<0 && Zdif70<0 && bER>0 && Vdif20>0) // E>2*Eprev,  9830 is 0.3 in Q15
	    m.speech_type=UP_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(5);
  move16();
#endif
	  if (Ndif4v>0 && (Ndif2lv>0 || Ndif3lv>0) && Edif_cp2<0 && curr_snr_diff>0 && Vdif20>0)  // E>2*Eprev
	    m.speech_type=UP_TRANSIENT;
	  
#ifdef WMOPS_FX
  test();
  Nlogic16(7);
  move16();
#endif
	  if (((Bdif5>0 && Vdif10>0) || (Bdif3>0 && Vdif6>0) || (bER>0 && Vdif3>0)) && Zdif80<0 && Sdif0>=0 ) // Clean condition
	    m.speech_type=UP_TRANSIENT;
	    
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	  if (Bdifm3>0 && Vdif14>0 && Zdif80<0 && Sdif0<0) // Noisy condition
	    m.speech_type=UP_TRANSIENT;
	  break;

	  
	case DOWN_TRANSIENT:
	  m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
#endif
	  if (bER>0) {
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	    if (Vdif20>0 && Zdif70<0)
	      m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	    if ((Ndif3lv>0 || Ndif4lv>0) && Edif_cp2<=0 && Vdif15>0)  // E>=2*Eprev
	      m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4v>0 && Vdif15>0)
	      m.speech_type=TRANSIENT;
	    
	  }
	  else {
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	    if (Ndif32>0 && Ndif43>0 && Ndif4v>0 && Vdif15>0 && Edif_cp2<0) // E>2*Eprev
	      m.speech_type=TRANSIENT;
	  }
	  break;

	case VOICED:
	default:
#ifdef WMOPS_FX
  test();
#endif
	  if (Ndif2lv>=0) {
	    m.speech_type=VOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	    if (Ndif1lv<0 || (Ndif1v<0 && sub(Ndif10,9830)>0) || bER<0)  // 9830 is 0.3 in Q15
	      m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	    if (Vdif15<0 && Ndif3lv<0 && Edif_cp<0)
	      m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	    if (bER<0 && Vdif20<0 && L_sub(Enext,E)<0 && Ndif3u<0 && Ndif4u<0)
	      m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	    if (Bdifm30<0 && Edif_cp<0)
	      m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	    if ((add(Ndif32,9830)<0) && sub(nacf_ap_fx[3],9830)<0 && sub(nacf_ap_fx[4],8192)<0) // nacf_ap[2]-nacf_ap[3]>0.3,  8192 is 0.25 in Q15
	      m.speech_type=TRANSIENT;
	  }
	  else {
#ifdef WMOPS_FX
  test();
#endif
	    if (bER>0) {
	      m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	      if (V2dif15<0 && Zdif90<0 && Edif_cp<0 && Ndif3v<0)
		m.speech_type=DOWN_TRANSIENT;
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	      if (Vdif25<0 && Edif_cp<0)
		m.speech_type=UNVOICED;
	    }
	    else {
	      m.speech_type=UNVOICED;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	      if (Ndif32>0 && Ndif43>0 && Zdif100<0 && Vdif20>0 && Sdif0>=0) // Clean condition		
		m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(4);
  move16();
#endif
	      if (Ndif32>0 && Ndif43>0 && Zdif110<0 && Vdif15>0 && Sdif0<0) // Noisy condition
		m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  move16();
#endif
	      if (Bdifm1>0 && Vdif12>0 && Zdif80<0)
		m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	      if (Bdifm3>0 && Vdif10>0 && Zdif90<0)
		m.speech_type=TRANSIENT;
#ifdef WMOPS_FX
  test();
  Nlogic16(3);
  move16();
#endif
	      if (Bdifm7>0 && Vdif3>0 && Zdif105<0 && Sdif0<0) // Noisy condition
		m.speech_type=TRANSIENT;
	    }	    	    
	  }

	  break;
	}
      }
    }
    m.curr_voiced=((sub(m.speech_type,VOICED)==0) || (sub(m.speech_type,TRANSIENT)==0 && prev_voiced));
  }
  else {  // inactive
    m.speech_type=SILENCE;
    m.curr_voiced=0;
  }
  
#ifdef WMOPS_FX
  test();
#endif
  if (sub(m.speech_type,UNVOICED)==0) {m.curr_voiced=0;}

  /* Skip the statistics update for PPP to CELP bump-up for now */

  
#ifdef WMOPS_FX
  test();
  Nmove16(5);
#endif
  if (m.curr_voiced) {
    vE[vcount%3]=E;
    vcount=add(vcount,1);
    Lacc=L_add40(vE[0],vE[1]);
    Lacc=L_add40(Lacc,vE[2]);
    Ltemp1=(Word32)L_sat32_40(Lacc);
    cl=0x5555;  // cl=1/3 in Q16
    dh=extract_h(Ltemp1);
    dl=extract_l(Ltemp1);
    Lacc=L_add40(L_mult_su(cl,dl),0x8000);  // mult with rounding
    Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(dh,cl),1));
    // Lacc=vEav
    vEav=(Word32)L_sat32_40(Lacc);
      //    vEav=(vE[0]+vE[1]+vE[2])/3.0;
  }
  else if (sub(m.speech_type,UNVOICED)<=0) {
    vE[0]=vE[1]=vE[2]=0;
    vcount=0;
    vEav=0;
  }
  
  Eprev=E;
#ifdef WMOPS_FX
  test();
#endif
  if (m.curr_voiced) vEprev=vEav;
  /*
  for (i=0;i<2;i++) {
    prev_snr[i]=curr_snr_fx[i];  // Back up band SNR from VAD
    prev_ns_snr[i]=curr_ns_snr_fx[i]; // Back up band SNR from NS
  }
  */
  return m;
}

//extern EvrcArgs *Eargs;

//static Word32 ave_rate=0;
static UNS_Word16 ave_rate=0;
Word32 ave_rate_bps=0;
Word16 numactive=0;
static Word32 NUMFRAMES[4]={0,0,0,0};//zero,quarter,half,full
//extern long int encode_fcnt;
Word16 patterncount=0;


static Word32 RATE[4]={0,24,48,96}; //rate x10
void update_average_rate(int rate) //valid rates are 1,2,3,4
{
  Word40 Lacc;
  Word16 S1h,S1l,S2l,S2h;
  Word32 Lsum1,Lsum2;

  #ifdef WMOPS_FX
  test();
#endif
  if (numactive==Eargs->ratewin) {
    //float ar=ave_rate*1./Eargs->ratewin;

    //ave_rate_bps=ave_rate*1000; //to be used in voiced for ave-rate control
			    // = ar*ratewin*10
    ave_rate_bps = L_shl((Word32) L_mult_su(1000,ave_rate),1); 
    
  }
#ifdef WMOPS_FX
  test();
#endif
    if (Eargs->avg_rate_control) {
#ifdef WMOPS_FX
  test();
#endif
      if (Eargs->operating_point==0) {
#ifdef WMOPS_FX
  test();
#endif
             if (numactive==Eargs->ratewin) {
	  //if initial value of ave_rate_bps make it equal to target
#ifdef WMOPS_FX
  test();
#endif
                 if (ave_rate_bps == 0) ave_rate_bps = L_mult(Eargs->avg_rate_target,CONST1); // 6000 = Eargs->ratewin*10
		 Word32 avratetarg= L_sub(L_mult(Eargs->avg_rate_target, CONST2),ave_rate_bps); //(2.0-0.02)*Eargs->ratewin*10
		 Lsum1= L_sub(avratetarg, CONST3); //2*7.5*Eargs->ratewin*10*1000
		 Lsum2=  CONST4; //1/(10*Eargs->ratewin*(9.0-7.5)) in Q31
		 S1h=extract_h(Lsum1);
		 S1l=extract_l(Lsum1);
		 S2h=extract_h(Lsum2);
		 S2l=extract_l(Lsum2);
		 Lacc=L_mult_su(S1h,S2l);
		 Lacc=L_add40(Lacc,(Word32)L_mult_su(S2h,S1l));
		 Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(S1h,S2h),1));   
		 pattern_m=(Word16) Lacc;
		 // pattern_m=(int)((avratetarg-7.5*Eargs->ratewin*10)/(10*Eargs->ratewin*(9.0-7.5)));
   
	  
#ifdef WMOPS_FX
  test();
  move16();
#endif
                  if (pattern_m<0) pattern_m=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
                  if (pattern_m>1000) pattern_m=1000;
	  
                  patterncount=0;
		}
      }
      if (Eargs->operating_point==1) {
#ifdef WMOPS_FX
  test();
#endif
             if (numactive==Eargs->ratewin) {
	  //if initial value of ave_rate_bps make it equal to target
#ifdef WMOPS_FX
  test();
#endif

                 if (ave_rate_bps == 0) ave_rate_bps = L_mult(Eargs->avg_rate_target,CONST1); // 6000 = Eargs->ratewin*10
		 Word32 avratetarg= L_sub(L_mult(shl(Eargs->avg_rate_target,1), CONST1),ave_rate_bps); 
		 Lsum1= L_sub(avratetarg,CONST5); //2*6.6*Eargs->ratewin*10*1000
		 Lsum2= CONST6; //1/(10*Eargs->ratewin*(7.5-6.6)) in Q31
		 S1h=extract_h(Lsum1);
		 S1l=extract_l(Lsum1);
		 S2h=extract_h(Lsum2);
		 S2l=extract_l(Lsum2);
		 Lacc=L_mult_su(S1h,S2l);
		 Lacc=L_add40(Lacc,(Word32)L_mult_su(S2h,S1l));
		 Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(S1h,S2h),1));   
		 pattern_m=(Word16) Lacc;
      
#ifdef WMOPS_FX
  test();
  move16();
#endif
  
                 if (pattern_m<0) pattern_m=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
                 if (pattern_m>1000) pattern_m=1000;
	  
	  
	         patterncount=0;
	    }
      }
      if (Eargs->operating_point == 2) {
      
             if (numactive==Eargs->ratewin) {
	  //if initial value of ave_rate_bps make it equal to target
#ifdef WMOPS_FX
  test();
#endif
                 if (ave_rate_bps == 0) ave_rate_bps = L_mult(Eargs->avg_rate_target,CONST1); // 6000 = Eargs->ratewin*10
		 Word32 avratetarg= L_sub(L_mult(Eargs->avg_rate_target,CONST2),ave_rate_bps); //(2.0-0.02)*Eargs->ratewin*10
		 Lsum1= L_sub(avratetarg,CONST7); // 2*5.75*Eargs->ratewin*10*1000
		 Lsum2= CONST8; //1/(10*Eargs->ratewin*(6.6-5.75)) in Q31
		 S1h=extract_h(Lsum1);
		 S1l=extract_l(Lsum1);
		 S2h=extract_h(Lsum2);
		 S2l=extract_l(Lsum2);
		 Lacc=L_mult_su(S1h,S2l);
		 Lacc=L_add40(Lacc,(Word32)L_mult_su(S2h,S1l));
		 Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(S1h,S2h),1));   
		 pattern_m=(Word16) Lacc;
   
	  
#ifdef WMOPS_FX
  test();
  move16();
#endif
  
                 if (pattern_m<0) pattern_m=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
	         if (pattern_m>1000) pattern_m=1000;
	  
	         patterncount=0;
	     }
      }
    }
    
  if (numactive==Eargs->ratewin) {ave_rate=0;numactive=0;}
  if (rate!=1&& rate!=0) {
    NUMFRAMES[rate-1]++;
    numactive++;
    ave_rate+=RATE[rate-1];
  }
  /*
  if (control.ave_rate_out==YES && numactive==control.ratewin) {
    float ar=ave_rate*1./control.ratewin;
    int write_floats(int,float*,int);
    write_floats(1,&ar,1);
  }
  */
  
  
}
