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
/*  Voice Activity Detection (VAD) functions                            */
/*======================================================================*/
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "rda_tab.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX

/*===================================================================*/
/* FUNCTION      :  initialize_rate_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Initialize static variables for VAD              */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (ENCODER_MEM_fx) *e_mem  : pointer to VAD data structure      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void initialize_rate_fx(ENCODER_MEM_fx *e_mem)
{
  
#ifdef WMOPS_FX
  Nmove16(16);
#endif
  e_mem->last_rate=EIGHTH;
  e_mem->frame_num=0;
  e_mem->num_full_frames=0;
  e_mem->hangover=10;
  e_mem->hangover_in_progress=0;
  e_mem->adaptcount=0;
  e_mem->pitchrun=0;
  e_mem->snr_stat_once=0;
  
  e_mem->frame_energy_sm[0] = 32000;
  e_mem->frame_energy_sm[1] = 3200;
  e_mem->signal_energy[0] = 3200000*16;
  e_mem->signal_energy[1] = 320000*16;
  e_mem->band_noise_sm[0]= HIGH_THRESH_LIM;
  e_mem->band_noise_sm[1]= HIGH_THRESH_LIM;
  e_mem->snr_map[0]=0;
  e_mem->snr_map[1]=0;
  
}


/*===================================================================*/
/* FUNCTION      :  update_background_fx ()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Update background noise estimate, signal energy  */
/*                  estimate, and band snrs                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) rate :  rate decision (1,3 or 4)                     */
/*   _ (Word16) beta :  open loop pitch correlation, Q15             */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (ENCODER_MEM_fx) *e_mem  : pointer to VAD data structure      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void update_background_fx(ENCODER_MEM_fx *e_mem, Word16 rate, Word16 beta)
{
  Word16 i;
  Word16 d1h, d1l, d2h;
  Word32 Ltemp, Ltemp2;
  Word40 Lacc;

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (sub(beta,NACF_ADAP_BGN_THR)<0) {
    e_mem->adaptcount=add(e_mem->adaptcount,1);
  } else {
    e_mem->adaptcount = 0;
  }
  
#ifdef WMOPS_FX
  test();
#endif
  if(sub(e_mem->adaptcount,ADP)>0) {
    for(i=0;i<FREQBANDS;i++) {
      /* just increase background noise until it matches */
      /* lowest stationary period background noise       */
      Ltemp=L_add(e_mem->band_noise_sm[i],1); // Q0
      d1h=extract_h(e_mem->band_noise_sm[i]);
      d1l=extract_l(e_mem->band_noise_sm[i]);
#ifdef WMOPS_FX
  move16();
#endif
      d2h=INC_FACTOR;
      Lacc=L_mult_su(d2h,d1l);
      Lacc=L_add40(L_shl(L_mult(d1h,d2h),1),(Word32)L_shr40(Lacc,14)); // Q0
      e_mem->band_noise_sm[i]=(Word32)MAX_FX(Lacc,Ltemp);
    }
    e_mem->adaptcount = ADP + 1;
  } else{
#ifdef WMOPS_FX
  test();
#endif
    if(sub(e_mem->snr_stat_once,1)==0) {  /* snr is valid  */
      for(i=0;i<FREQBANDS;i++){
#ifdef WMOPS_FX
  test();
  logic16();
#endif
	if((sub(e_mem->snr_map[i],SNR_MAP_THRESHOLD)>0) && 
	   (L_sub(e_mem->band_noise_sm[i],e_mem->frame_energy_sm[i])<0)){
	  
	  Ltemp=L_add(e_mem->band_noise_sm[i],1); // Q0
	  d1h=extract_h(e_mem->band_noise_sm[i]);
	  d1l=extract_l(e_mem->band_noise_sm[i]);
#ifdef WMOPS_FX
  move16();
#endif
	  d2h=IS96_INC;
	  Lacc=L_mult_su(d2h,d1l);
	  Lacc=L_add40(L_shl(L_mult(d1h,d2h),1),(Word32)L_shr40(Lacc,14)); // Q0
	  e_mem->band_noise_sm[i]=(Word32)MAX_FX(Lacc,Ltemp);
	}
      }
    }
  }

  /* if input energy is lower than noise estimate, */
  /* reduce background noise estimate immediately  */

  for(i=0;i<FREQBANDS;i++) {
    /* Update Background Noise Estimate */
#ifdef WMOPS_FX
  test();
#endif
    if(L_sub(e_mem->frame_energy_sm[i],e_mem->band_noise_sm[i])<0) {
      e_mem->band_noise_sm[i] = e_mem->frame_energy_sm[i];
    }
  }

  for(i=0;i<FREQBANDS;i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub(e_mem->band_noise_sm[i],HIGH_THRESH_LIM)>0) {
#ifdef WMOPS_FX
  move16();
#endif
      e_mem->band_noise_sm[i] = HIGH_THRESH_LIM;
    }
#ifdef WMOPS_FX
  test();
#endif
    if (L_sub(e_mem->band_noise_sm[i],LOWEST_LEVEL[i])<0) {
#ifdef WMOPS_FX
  move16();
#endif
      e_mem->band_noise_sm[i] = LOWEST_LEVEL[i];
    }
  }
  /* end of updating backgound noise energy */

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (sub(beta,NACF_SOLID_VOICED)>0) {
    e_mem->pitchrun=add(e_mem->pitchrun,1);
  } else {
    e_mem->pitchrun = 0;
  }

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (sub(e_mem->pitchrun,STATVOICED)>0) {  /* decrease the signal energy   */
	                                     /* if we're in a voiced segment */
    e_mem->snr_stat_once = 1;   /* confidence that we have seen speech */

    /* indicates a stationary voiced segment       */
    for(i=0;i<FREQBANDS;i++) {
      d1h=extract_h(e_mem->signal_energy[i]);
      d1l=extract_l(e_mem->signal_energy[i]);
      d2h=SCALE_DOWN_ENERGY; // 0.97 in Q15
      Lacc=L_mult_su(d2h,d1l);
      e_mem->signal_energy[i]=L_add(L_mult(d1h,d2h),L_shr((Word32)Lacc,15));
    }
  }
  
  for(i=0;i<FREQBANDS;i++) {
    /* Update Signal Energy Estimate */
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if(L_sub(e_mem->frame_energy_sm[i],e_mem->signal_energy[i])>0) {
      e_mem->signal_energy[i] = e_mem->frame_energy_sm[i];
    }
  }
  /* end updating signal energy estimate */

  /* update SNR estimates */
#ifdef WMOPS_FX
  test();
#endif
  if(e_mem->snr_stat_once != 0) {
    /* we have seen some speech and are confident in our SNR measure */
    /* UPDATE THIS FOR GENERAL SNR MAPS! */
    for(i=0;i<FREQBANDS;i++) {
      Ltemp=log10_lut(e_mem->signal_energy[i]);
      Ltemp2=log10_lut(e_mem->band_noise_sm[i]); 
      Ltemp=L_sub(Ltemp,Ltemp2); // Q23
      d1h=extract_h(Ltemp);
      d1l=extract_l(Ltemp);
#ifdef WMOPS_FX
  move16();
#endif
      d2h=0x3333;  // 0.2 in Q16
      Ltemp=(Word32)L_mult_su(d2h,d1l);
      Ltemp=L_add(L_shr(L_mult(d1h,d2h),1),L_shr(Ltemp,16)); // Q23
      /* Ltemp=10*log10(s/n)/5, Q23   */
      d1h=round32_16(L_shr(Ltemp,7)); // Q0
      e_mem->snr_map[i]=sub(d1h,4);
      
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if(e_mem->snr_map[i] < 0) e_mem->snr_map[i]=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if(sub(e_mem->snr_map[i],7)>0) e_mem->snr_map[i]=7;
    }
  } else {
    /* we haven't seen speech, aren't confident in our estimate of the */
    /* signal energy and will use a nominal energy of */
    /* VOICE_INITIAL dB (-18dbm0) for our signal energy */
	
    for(i=0;i<FREQBANDS;i++) {
      Ltemp=log10_lut(voice_initial_fx[i]);
      Ltemp2=log10_lut(e_mem->band_noise_sm[i]); 
      Ltemp=L_sub(Ltemp,Ltemp2); // Q23
      d1h=extract_h(Ltemp);
      d1l=extract_l(Ltemp);
#ifdef WMOPS_FX
  move16();
#endif
      d2h=0x3333;  // 0.2 in Q16
      Ltemp=(Word32)L_mult_su(d2h,d1l);
      Ltemp=L_add(L_shr(L_mult(d1h,d2h),1),L_shr(Ltemp,16)); // Q23
      d1h=round32_16(L_shr(Ltemp,7)); // Q0
      e_mem->snr_map[i]=sub(d1h,4);
      
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if(e_mem->snr_map[i] < 0) e_mem->snr_map[i]=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
      if(sub(e_mem->snr_map[i],7)>0) e_mem->snr_map[i]=7;
    }
  }

  e_mem->last_rate = rate;
}

/*===================================================================*/
/* FUNCTION      :  voice_activity_detection_fx ()                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Voice activity detection main routine            */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32 []) Rs :  Normalized autocorrelation of speech        */
/*   _ (Word16) norm_r : Normalization factor of Rs                  */
/*   _ (Word16) beta :  Open loop pitch correlation, Q15             */
/*   _ (Word16) mode :  Operating point of encoder, Q0               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (ENCODER_MEM_fx) *e_mem  : pointer to VAD data structure      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) rate :  VAD decision (1,3 or 4)                      */
/*===================================================================*/
Word16 voice_activity_detection_fx(Word32 *Rs, ENCODER_MEM_fx *e_mem,
				   Word16 norm_r, Word16 beta, Word16 mode)
{
  static Word16  first = 0;
  static Word16 (*THRESH_SNR)[TLEVELS][2];
  static Word16 *hangover;
  Word16 rate;
  Word16 i, k;
  Word16 d1h, d1l, d2h, d2l, n;
  Word32 Ltemp, band_power;
  Word40 Lacc;

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if(first == 0){
    /*do initialization of rate_mem */
    initialize_rate_fx(e_mem);
    first = 1;
  }
  
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
  if (mode==0) {
    hangover=hangover0;
    THRESH_SNR=THRESH_SNR0_fx;
  }
  else if (sub(mode,1)==0) {
    hangover=hangover1;
    THRESH_SNR=THRESH_SNR1_fx;
  }
  else {
    hangover=hangover2;
    THRESH_SNR=THRESH_SNR2_fx;
  }

  /* compute band power, update smooth signal energy and compute band rate */
  
  for(k=0; k<FREQBANDS;k++) {
    /* find the energy in each band   */
#ifdef WMOPS_FX
  move32();
#endif
    Lacc=0;
    for(i=0;i<FILTERORDER;i++) {
      d1h=extract_h(Rs[i]);
      d2h=rate_filt_auto_fx[k][i];  // Q15
      Lacc=L_mac40(Lacc,d1h,d2h);
    }
    
    n=norm32_l40(Lacc);
    Ltemp=(Word32) L_shl40(Lacc,n); // normalize MSW of band power
    
#ifdef WMOPS_FX
  move32();
#endif
    Lacc=0;
    for(i=0;i<FILTERORDER;i++) {
      d1l=extract_l(Rs[i]);
      d2h=rate_filt_auto_fx[k][i];  // Q15
      Lacc=L_add40(Lacc,(Word32)L_mult_su(d2h,d1l));  // compute DP*SP, LSW
    }
    Lacc=L_shl40(Lacc,1);
    Ltemp=(Word32)L_sat32_40(L_add40(L_shl40(Lacc,sub(n,16)),Ltemp));
    // Ltemp is band power in Q(n+norm_r)
    n=add(n,norm_r);

    /* Update smooth frame energy  */
    band_power=L_shr(Ltemp,n); // band_power in Q0
    e_mem->band_power[k]=band_power; // saved for SNR calculation for mode decision

    d1h=extract_h(e_mem->frame_energy_sm[k]);
    d1l=extract_l(e_mem->frame_energy_sm[k]);
    d2h=extract_h(band_power);
    d2l=extract_l(band_power);
    Lacc=L_mult_uu(d1l,SMSNR);
    Lacc=L_sub40(Lacc,(Word32)L_mult_su(SMSNR,d2l)); // frame_energy_sm*0.6+band_powe*0.4, LSW
    Ltemp=(Word32)L_shr40(Lacc,16);
    Lacc=L_mult_su(d1h,SMSNR);
    Lacc=L_sub40(Lacc,L_shr(L_mult(d2h,SMSNR),1));
    e_mem->frame_energy_sm[k]=(Word32)L_sat32_40(L_add40(Lacc,Ltemp));

    n=norm_l(e_mem->band_noise_sm[k]);  // normalize noise estimate
    Ltemp=L_shl(e_mem->band_noise_sm[k],n);

    d1h=extract_h(Ltemp); // Q(n-16), MSW of band_noise_sm[k]
    d1l=extract_l(Ltemp); // Q(n), LSW of band_noise_sm[k]

    e_mem->band_rate[k]=EIGHTH;
    n=sub(10,n);  // n=10-n
    for (i=0; i<2; i++) {
      // make rate decision 
      Lacc=L_mult_su(THRESH_SNR[k][e_mem->snr_map[k]][i],d1l); // Q(15-n)
      Ltemp=L_mult(THRESH_SNR[k][e_mem->snr_map[k]][i],d1h); // Q(-n)
    
      Ltemp=L_add(Ltemp,(Word32)L_shr40(Lacc,15)); // Q(-n)
      Ltemp=L_sub(band_power,L_shl(Ltemp,n));
#ifdef WMOPS_FX
  test();
#endif
      if (Ltemp>0) {
#ifdef WMOPS_FX
  test();
  move16();
#endif
	if(sub(e_mem->band_rate[k],EIGHTH)==0) {
	  e_mem->band_rate[k]=HALFRATE_VOICED;
	} else if (sub(e_mem->band_rate[k],HALFRATE_VOICED)==0) {
	  e_mem->band_rate[k]=FULLRATE_VOICED;
	}
      }
    }
  }

  rate=MAX_FX(e_mem->band_rate[0],e_mem->band_rate[1]);

#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if((sub(e_mem->num_full_frames,FULL_THRESH)>0) || (sub(e_mem->hangover_in_progress,1)==0)) {
    /* only use hangover after so many full rate frames in a row*/
#ifdef WMOPS_FX
  test();
#endif
    if(sub(rate,FULLRATE_VOICED) !=0 ) {
      e_mem->hangover =add(e_mem->hangover,1); 
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
      if (sub(e_mem->hangover,hangover[e_mem->snr_map[0]])<=0) {
	rate = FULLRATE_VOICED;
	e_mem->hangover_in_progress = 1;
      } else {
	e_mem->hangover_in_progress = 0;
      }
    } else {
      e_mem->hangover_in_progress = 0;
    }
  }

#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if((sub(rate,FULLRATE_VOICED)==0) && (e_mem->hangover_in_progress == 0)) {
    e_mem->hangover=0;   
    e_mem->num_full_frames=add(e_mem->num_full_frames,1);
  } else {
    e_mem->num_full_frames = 0;
  }

#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if ((sub(e_mem->last_rate,FULLRATE_VOICED)==0) && sub(rate,EIGHTH)==0) {
    rate = HALFRATE_VOICED;
  }
  
  /* allows fast convergence for EIGHTH rate */
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if(e_mem->frame_num == 0){
    rate = FULLRATE_VOICED;
  }

  e_mem->last_rate = rate;

  /* update background noise and signal energy estimates */
  update_background_fx(e_mem, rate, beta);

  e_mem->frame_num=add(e_mem->frame_num,1);

  return(rate);
}

