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
/*  Encoder top level functions                                         */
/*======================================================================*/
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "globs_fx.h"
#include "proto_fx.h"
#include "lpcana_tab.h"
#include  <stdlib.h>

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

//#ifdef DTX
// void LPC_pred2refl_fx (Word16 [], Word16 [], Word16);
//#endif


/*===================================================================*/
/* FUNCTION      :  InitEncoder_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Initialize some encoder static variables         */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _  None                                                         */
/*===================================================================*/
void InitEncoder_fx()
{
  Word16 j;

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  bit_rate=4;
    
  for (j=0; j<ORDER; j++) {
#ifdef WMOPS_FX
  move16();
  move32();
  move32();
#endif
    SynMemoryM_fx[j] = WFmemFIR_fx[j] = WFmemIIR_fx[j] = 0;
  }

  for (j=0; j<ORDER; j++) {
    OldOldlspE_fx[j]=OldlspE_fx[j]=Oldlsp_nq_fx[j]=lsp_init_tab[j];
  }
  
  for (j=0; j<GUARD; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  ConstHPspeech_fx[j] = 0;
  }

  for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  Excitation_fx[j] = 0;
  }
#ifdef WMOPS_FX
  Nmove16(4);
#endif

  Init_ZeroInput_fx(); 
  accshift_fx = 0;
  shiftSTATE = 0;
  dpm = 0;
  pdelay_fx = 40;
  
}


/*===================================================================*/
/* FUNCTION      :  pre_encode_fx ()                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Do LPC analysis, calculate LSP and residual,     */
/*               and open loop pitch estimate and speech correlation */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) inFbuf : input hi-pass filtered speech, Q0        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32 []) Rs : normalized autocorrelation, for VAD          */
/*   _ (Word16) r_scale : norm factor of autocorr                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) beta_fx : speech correlation, Q15                    */
/*===================================================================*/
/* GLOBAL VARIABLES :                                                */
/*   _ (Word16 []) HPspeech_fx : high-pass filtered speech, Q0       */
/*   _ (Word16 []) lsp_nq_fx : unquantized LSP coefficients, Q15     */
/*   _ (Word16 []) residual_fx : residual after LPC filter, Q0       */
/*   _ (Word16) beta_fx : speech correlation, Q15                    */
/*   _ (Word16) delay_fx : open loop pitch estimate, Q0              */
/*   _ (Word16 []) Oldlsp_nq_fx : previous frame unquantized LSP     */
/*                 coefficients, Q15, input                          */
/*===================================================================*/
Word16 pre_encode_fx (Word16 *inFbuf, Word32 *Rs, Word16& r_scale)
{
  /*....(local) variables....*/
  register int i, j;	/* counters */
  
  static Word16 last_delay;
  Word16 tmp_tab[ORDER],BWEXP=0;
  Word16 k, k0;
  Word16 d1h, d1l, offset;
  Word32 Ltemp;
  Word40 Lacc;

  Word32 pci_dp[ORDER];  // LPC coefficients (output of LPC analysis), Q27

  //vars for tone detection
  Word16 tone=0; 
  static Word16 prev_tone=0;
  Word16 possbwexp=0;
  TONE_DETECTED=0;
  
  /*....execute....*/
  
  /* Re-initialize PackWdsPtr */

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  PktPtr[0]=16;PktPtr[1]=0;
  for (i=0; i<PACKWDSNUM; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  TxPkt[i]=0;
  }

  /* Process Data */
  for (j = 0; j < FrameSize + LOOKAHEAD_LEN; j++) {
#ifdef WMOPS_FX
  move16();
#endif
    HPspeech_fx[j + GUARD] = inFbuf[j];
  }
  
  /* Make sure HPspeech - HPspeech+2*GUARD have the right memory */
  for (j=0; j<GUARD; j++) {
	  HPspeech_fx[j] = ConstHPspeech_fx[j];
  }

  /* Update ConstHPspeech for next frame */
  for (j=0; j<GUARD; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  ConstHPspeech_fx[j] = HPspeech_fx[j + FrameSize];
  }
  

  /* Calculate prediction coefficients */
  /* reflection coef. not needed-returned in Scratch */
  r_scale=lpcanalys_fx(pci_dp, HPspeech_fx, ORDER, LPC_ANALYSIS_WINDOW_LENGTH,
			Rs);

#if 1 //tone detector 
  
  if ((L_sub(lpcg,262766452)>0)) //27 db, global var populated in durbin function
    tone=1; 
  
  //30 db in linear Q19 = 524288000, 25 db is Q19  = 165794423, 29 db is 416456762 ,  27 dB in linear Q19 = 262766452 
  if ((lastrateE !=4) && (LAST_MODE_BIT_E ==0))
    { 
      // prev was not ppp/fcelp      
      possbwexp=1;
    }
  else
    {
      // prev was fcelp/ppp
      if ((lastlastrateE !=4) && (LAST_LAST_MODE_BIT_E ==0))
	{
          //prev prev was not ppp/fcelp
	  possbwexp=1;
 	}
      else
	possbwexp=0; //at this point both prev, prevprev were either fcelp/ppp
    }
  
  if ((possbwexp==1) && (tone==1))
    BWEXP=1; // lpcg is > 27db and prev frame is either sil/nelp/hcelp
  else //prev frame is fcelp/ppp
    {
      if ((prev_tone ==1) && (tone==1)) {BWEXP=1;TONE_DETECTED=1;}
      else
	BWEXP=0;
    } 
  
  prev_tone = tone;
  
  
#endif  //tone detector


#if 1
  if (BWEXP==1)
    for(i=0;i<ORDER;i++)
      tmp_tab[i] = gamma9821_tab[i];
  else if (BWEXP==0)
    for(i=0;i<ORDER;i++)
      tmp_tab[i] = gamma994_tab[i];

#endif  
  

  /* Bandwidth expansion */
  for (i=0;i<ORDER;i++) {
    d1h=extract_h(pci_dp[i]);
    d1l=extract_l(pci_dp[i]);
#ifdef WMOPS_FX
  move32();
#endif
  Ltemp=0x08000;  // for rounding
    
  Lacc=L_mult_su(tmp_tab[i],d1l);  // tmp_tab in Q15
  
  Lacc=L_add40(L_shl40(Lacc,1),Ltemp);
  
  
  pci_dp[i]=(Word32)L_add40(L_shr40(Lacc,16),L_mult(d1h,tmp_tab[i])); // Q27
  
  } 
  
  lpc2lsp_fx(pci_dp, lsp_nq_fx, Oldlsp_nq_fx);

  
  /* Get residual signal */
  for (j=0; j<ORDER; j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  Scratch_fx[j]=0; /* Scratch is used as filter memory */
  }

  lsp2lpc_fx(Oldlsp_nq_fx, pci_nq_fx, pci_nq_fx);
  
  prediction_filter_fx(pci_nq_fx,HPspeech_fx,residual_fx, Scratch_fx,ORDER, GUARD ,4);
  
  for (i=0; i<NoOfSubFrames; i++) {
    
    Interpol_fx(Oldlsp_nq_fx, lsp_nq_fx, lspi_nq_fx, i);
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx); //Q12
        
    offset= add(shr(extract_l(L_mult(i,SubFrameSize-1)),1),GUARD);
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2)
      prediction_filter_fx(pci_nq_fx,HPspeech_fx+offset,residual_fx+offset, Scratch_fx,ORDER, SubFrameSize-1 ,4); 
    else
      prediction_filter_fx(pci_nq_fx,HPspeech_fx+offset,residual_fx+offset, Scratch_fx,ORDER, SubFrameSize ,4); 
  }

  for (i=0;i<ORDER;i++) pci_fx[i]=round32_16(L_shl(pci_dp[i],1));  // Q12
  
  prediction_filter_fx(pci_fx,HPspeech_fx + FrameSize + GUARD,residual_fx + FrameSize + GUARD, Scratch_fx,ORDER, LOOKAHEAD_LEN ,4);

    
  /* Calculate pitch period at the end of the frame, use n.q. lpc coef. */

  fndppf_fx(&delay1_fx, &beta1_fx, residual_fx+GUARD, DMIN, DMAX, FrameSize);
  fndppf_fx(&delay_fx, &beta_fx, residual_fx+2*GUARD, DMIN, DMAX, FrameSize);
 
#ifdef WMOPS_FX
  test();
#endif
  if (sub(sub(beta1_fx,beta_fx),13107)>0) {  // 0.4, Q15
#ifdef WMOPS_FX
  test();
#endif
    if (sub(abs_s(sub(delay_fx,delay1_fx)),15)>0) {
      beta_fx = beta1_fx;
      delay_fx = delay1_fx;
    }
    else {
      beta_fx = L_shr(L_add(beta_fx,beta1_fx),1);
      delay_fx = shr(add(delay_fx,delay1_fx),1);
    }
  }
  else if (sub(sub(beta1_fx,beta_fx),1638)>0 && sub(abs_s(sub(delay1_fx,last_delay)),7)<0) { // 0.05, Q15
    /* if delay1 is close to delay for last frame and beta1 > beta */
#ifdef WMOPS_FX
  test();
#endif
    if (sub(abs_s(sub(delay_fx,delay1_fx)),15)>0) {
      beta_fx = beta1_fx;
      delay_fx = delay1_fx;
    }
    else {
      beta_fx = L_shr(L_add(beta_fx,beta1_fx),1);
      j = shr(add(add(delay_fx,delay1_fx),1),1);

#ifdef WMOPS_FX
  move32();
#endif
      Ltemp=LW_MIN;
      k0=MAX(DMIN,j-1); 
      for(k=MAX(DMIN,j-1); k<=MIN(DMAX,j+1); k++) {
        for (i=0,Lacc=0; i<FrameSize-k; i++)
	  Lacc=L_mac40(Lacc,residual_fx[GUARD*3/2+i],residual_fx[GUARD*3/2+i+k]);
	Lacc=L_shr40(Lacc,5);  // Q(-4)
#ifdef WMOPS_FX
  test();
#endif
	if(L_sub40(Lacc,Ltemp)>0) {
          Ltemp=(Word32)L_sat32_40(Lacc);
          k0 = k;
        }
      }
      delay_fx = k0;
    }
  }
  
  last_delay = delay_fx;

  return (beta_fx);
}

/*===================================================================*/
/* FUNCTION      :  get_rcelp_fx                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Modify residual generated with unquantized       */
/*                  LSPs. This function outputs one frame of modified*/
/*                  speech and weighted modified speech              */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16 []) residual_fx : original residual, Q0              */
/*    _ (Word16)    beta_fx: periodicity parameter, Q15              */
/*    _ (Word16)    lastrateE: previous encoding rate, Q0            */
/*    _ (Word16[])  Oldlsp_nq_fx  : Last frame unquantized lsp (Q15) */
/*    _ (Word16[])  lsp_nq_fx  : Current frame unquantized lsp (Q15) */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*    _ (Word16[]) mspeech_fx : Modified speech (Q0)                 */
/*    _ (Word16[]) wspeech_fx : Weighted modified speech (Q0)        */
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16[])  lspi_nq_fx, pci_nq_fx  :
                                Interpolated prediction/lsp
				coefficients(unquantized) (Q12/Q15)  */
/*   _ (Word16)    accshift_fx: accumulated shift, Q8                */
/*   _ (Word16)    dpm_fx: last modified position, Q0                */
/*   _ (Word16)    shiftSTATE:Q0                                     */
/*   _ (Word16)    delay_fx:Q0                                       */
/*   _ (Word16)    pdelay_fx:Q0                                      */
/*   _ (Word32[])  SynMemoryM_fx,WFmemIIR_fx: synthesis filter
                   memories(Q16)                                     */
/*   _ (Word16[])  WFmemFIR_fx : prediction filter memory (Q0)       */
/*   _ (Word16 []) residualm_fx : modified residual, Q0              */
/*   _ data_packet.DELAY_IDX, data_packet.DELTA_DELAY_IDX            */
/*------------------------------------------------------------------ */
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
void get_rcelp_fx(Word16 *mspeech_fx, Word16 *wspeech_fx)
{
  register int i, j;
  
  short   subframesize;
  Word32 delayi_fx[3];

  
  /* Update shiftSTATE with hysteresis */
  
#ifdef WMOPS_FX
	  test();
	  Nmove16(3);
#endif
  if (sub(beta_fx,3277)<0) {
    accshift_fx = 0;
    dpm = 0;
    shiftSTATE = 0;
  }

#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (sub(accshift_fx,5120)>0) shiftSTATE = -1;
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (add(accshift_fx,5120)<0) shiftSTATE = 1;
#ifdef WMOPS_FX
	  test();
	  logic16();
	  move16();
#endif
  if (sub(accshift_fx,2560)<=0 && add(shiftSTATE,1) == 0) shiftSTATE = 0;
#ifdef WMOPS_FX
	  test();
	  logic16();
	  move16();
#endif
  if (add(accshift_fx,2560)>=0 && sub(shiftSTATE,1) == 0) shiftSTATE = 0;

  /* Control accshift */
#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
  if (sub(shiftSTATE,1) == 0 && sub(beta_fx,13107)<0) delay_fx = add(delay_fx, 1);                   // Q0
  else if (add(shiftSTATE,1) == 0 && sub(beta_fx,13107)<0) delay_fx = sub(delay_fx, 1);             // Q0
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (sub(delay_fx,DMAX)>0) delay_fx = DMAX;
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (sub(delay_fx,DMIN)<0) delay_fx = DMIN;
  data_packet.DELAY_IDX=sub(delay_fx,DMIN);
#ifdef WMOPS_FX
	  test();
#endif
  if (sub(lastrateE,3) <0) pdelay_fx=delay_fx;

  //Prepare for the delta delay
  j = sub(delay_fx, pdelay_fx);
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (sub(abs_s(j),15)>0) j = 0;
  else j = add(j, 16);
  data_packet.DELTA_DELAY_IDX=(unsigned short) j;
  
  /* Smooth interpolation if the difference between delays is too big */
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (sub(abs_s(sub(delay_fx, pdelay_fx)),15)>0) pdelay_fx = delay_fx;
  
  /***********************************
   * Generate RCELP modified residual*
   **********************************/
  for (i=0; i<NoOfSubFrames; i++) {   
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = sub(SubFrameSize,1);
    else subframesize = SubFrameSize;
    /* interpolate lsp */
    Interpol_fx(Oldlsp_nq_fx, lsp_nq_fx, lspi_nq_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx);
        
    weight_fx(pci_nq_fx, gamma90_tab, wpci_gma1_fx, 10);
    weight_fx(pci_nq_fx, gamma50_tab, wpci_gma2_fx, 10);
    /* Interpolate delay */
    Interpol_delay_fx(pdelay_fx, delay_fx, delayi_fx, i);


    ComputeACB_fx(residualm_fx,delayi_fx,
		  residual_fx+GUARD+i*(SubFrameSize-1),
		  FrameSize+GUARD-i*(SubFrameSize-1) , dpm,
		  accshift_fx, beta_fx, subframesize);


    /* Get weighted speech */
    /* ORIGM */


#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(residualm_fx[j],SCALE);
#endif    
    
    synthesis_filter_fx(pci_nq_fx, Scratch_fx, mspeech_fx+i*(SubFrameSize-1), SynMemoryM_fx, ORDER, subframesize, 3);

#if 1
    
    for (j=0;j<subframesize;j++) mspeech_fx[i*(SubFrameSize-1)+j]=shl(mspeech_fx[i*(SubFrameSize-1)+j],SCALE);
#endif
    
    /* Weighting filter */
    prediction_filter_fx(wpci_gma1_fx, mspeech_fx+i*(SubFrameSize-1), Scratch_fx, WFmemFIR_fx, ORDER, subframesize, 4);

#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(Scratch_fx[j],SCALE);
#endif 
    
    synthesis_filter_fx(wpci_gma2_fx, Scratch_fx, wspeech_fx+i*(SubFrameSize-1), WFmemIIR_fx, ORDER, subframesize, 3);

#if 1
    
    for (j=0;j<subframesize;j++) wspeech_fx[i*(SubFrameSize-1)+j]=shl(wspeech_fx[i*(SubFrameSize-1)+j],SCALE);
#endif
    
    /* Update residualm */
#ifdef WMOPS_FX
  Nmove16(dpm);
#endif
    for (j = 0; j < dpm; j++) residualm_fx[j] = residualm_fx[j + subframesize];

  } /* End of subframe loop */
    
}

/*===================================================================*/
/* FUNCTION      :  encode_fx ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Encoder top level code, including packing        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) rate : Encoding bit rate                             */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) codeBuf : output packet, Q0                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _  None                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _  None                                                         */
/*===================================================================*/
void  encode_fx (Word16& rate, Word16 *codeBuf)
{
  register int i, j;
  lpcgflag=0;
  Word16 q_resid_fx[160];

  bit_rate=rate;

#ifdef DYNAMIC_COMPLEXITY_REDUC
  // initialize values for maximum search space
  // dynamic complexity reduction
  // Global Variables to be used cod7_35_fx
  fcb_N0 = 4;	/* # of candidate positions for pulse 1 */
  fcb_N1 = 8;	/* # of candidate positions for pulse 2 */	
  fcb_N2 = 12;	/* # of candidate positions for pulse 3 */
  fcb_N3 = 16;

  fcb_N_PRE = 12; //10	/* # of candidate combinations for pulse 1, 2 and 3 */

  // initialize values for maximum search space
  // Global Variables to be used by enc_lsp_vq_28_fx
  PRE_LSP_NUM0 = 4;
  PRE_LSP_NUM2 = 6;
#endif//DYNAMIC_COMPLEXITY_REDUC
  /* Do encoding here */
  /* RCELP is a front end process, can do it here */
  if (bit_rate>2) get_rcelp_fx(mspeech_fx, wspeech_fx);
 
  switch(bit_rate) {
  case 1:
    silence_encoder_fx();
#ifdef WMOPS_FX
  move16();
#endif
    PACKET_RATE=1;
    break;

  case 2:
    nelp_encoder_fx();
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    PACKET_RATE=2; MODE_BIT=0;
    break;
    
  case 3:
#ifdef WMOPS_FX
  test();
  logic16();
#endif
    if (rcelp_half_rateE || dim_and_burstE) {
      celp_encoder_fx(bit_rate);
      PACKET_RATE=3;
    }
    else {
      voiced_encoder_fx(q_resid_fx, bit_rate);

#ifdef WMOPS_FX
  test();
#endif
      if (bit_rate == 3) { //No bumpup
#ifdef WMOPS_FX
  test();
#endif
	if (PPP_MODE_E_fx == 0) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	  PACKET_RATE=2; MODE_BIT=1;
	}
	else if (PPP_MODE_E_fx ==1) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	  PACKET_RATE=4; MODE_BIT=1;
	}
      }
    }

    break;
    
  case 4:
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    PACKET_RATE=4; MODE_BIT=0;
    celp_encoder_fx(bit_rate);
    break;
  }

  switch(data_packet.PACKET_RATE) {
  case 1:
    //silence
    Bitpack(data_packet.LSP_IDX[0],(unsigned short*)TxPkt,4,PktPtr);
    Bitpack(data_packet.LSP_IDX[1],(unsigned short*)TxPkt,4,PktPtr);
    Bitpack(data_packet.SILENCE_GAIN,(unsigned short*)TxPkt,NUM_EQ_BITS,PktPtr);
    break;
  case 2:
#ifdef WMOPS_FX
  test();
#endif
    if(data_packet.MODE_BIT==0){
      if (SPL_HNELP==0){
      //NELP
      Bitpack(data_packet.MODE_BIT,(unsigned short *)TxPkt,1,PktPtr);
      Bitpack(data_packet.LSP_IDX[0],(unsigned short*)TxPkt,8,PktPtr);
      Bitpack(data_packet.LSP_IDX[1],(unsigned short*)TxPkt,8,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[0][0],(unsigned short*)TxPkt,5,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[1][0],(unsigned short*)TxPkt,6,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[1][1],(unsigned short*)TxPkt,6,PktPtr);
      Bitpack(data_packet.NELP_FID,(unsigned short*)TxPkt,2,PktPtr);
      }
      else {
      data_packet.PACKET_RATE=3;
      Bitpack(0x6E ,(unsigned short*)TxPkt,7,PktPtr); //110 in dec
      Bitpack(1,(unsigned short*)TxPkt,2,PktPtr);//QNELP identifier 
      Bitpack(data_packet.LSP_IDX[0],(unsigned short*)TxPkt,8,PktPtr);
      Bitpack(data_packet.LSP_IDX[1],(unsigned short*)TxPkt,8,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[0][0],(unsigned short*)TxPkt,5,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[1][0],(unsigned short*)TxPkt,6,PktPtr);
      Bitpack(data_packet.NELP_GAIN_IDX[1][1],(unsigned short*)TxPkt,6,PktPtr);
      Bitpack(data_packet.NELP_FID,(unsigned short*)TxPkt,2,PktPtr);
     
      }
	 
    }
    else if(data_packet.MODE_BIT==1){
      //QPPP
      Bitpack(data_packet.MODE_BIT,(unsigned short *)TxPkt,1,PktPtr);
      Bitpack(data_packet.LSP_IDX[0],(unsigned short *)TxPkt,10,PktPtr);
      Bitpack(data_packet.LSP_IDX[1],(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.Q_delta_lag,(unsigned short *)TxPkt,4,PktPtr);
      Bitpack(data_packet.AMP_IDX[0],(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.AMP_IDX[1],(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.POWER_IDX,(unsigned short *)TxPkt,6,PktPtr);
    }

    break;
  case 3:
    //Half Rate Celp
    Bitpack(data_packet.DELAY_IDX ,(unsigned short *)TxPkt,7,PktPtr);
    Bitpack(data_packet.LSP_IDX[0] ,(unsigned short *)TxPkt,7,PktPtr);
    Bitpack(data_packet.LSP_IDX[1] ,(unsigned short *)TxPkt,7,PktPtr);
    Bitpack(data_packet.LSP_IDX[2] ,(unsigned short *)TxPkt,8,PktPtr);
    
    for(i=0;i<NoOfSubFrames;i++) Bitpack(data_packet.ACBG_IDX[i] ,(unsigned short *)TxPkt,3,PktPtr);
    for(i=0;i<NoOfSubFrames;i++) Bitpack(data_packet.FCB_PULSE_IDX[i][0] ,(unsigned short *)TxPkt,10,PktPtr);
    for(i=0;i<NoOfSubFrames;i++) Bitpack(data_packet.FCBG_IDX[i] ,(unsigned short *) TxPkt,4,PktPtr);

    break;
  case 4:
#ifdef WMOPS_FX
  test();
#endif
    if(data_packet.MODE_BIT==0){
      //FCELP
      Bitpack(data_packet.MODE_BIT   ,(unsigned short *)TxPkt,1,PktPtr);
      Bitpack(data_packet.DELAY_IDX ,(unsigned short *)TxPkt,7,PktPtr);
      Bitpack(data_packet.LSP_IDX[0] ,(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.LSP_IDX[1] ,(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.LSP_IDX[2] ,(unsigned short *)TxPkt,9,PktPtr);
      Bitpack(data_packet.LSP_IDX[3] ,(unsigned short *)TxPkt,7,PktPtr);
      
      Bitpack(data_packet.DELTA_DELAY_IDX ,(unsigned short *)TxPkt,5,PktPtr);
      for(i=0;i<NoOfSubFrames;i++) Bitpack(data_packet.ACBG_IDX[i] ,(unsigned short *)TxPkt,3,PktPtr);
      for(i=0;i<NoOfSubFrames;i++) {
	Bitpack(data_packet.FCB_PULSE_IDX[i][0] ,(unsigned short *)TxPkt,16,PktPtr);
	Bitpack(data_packet.FCB_PULSE_IDX[i][1] ,(unsigned short *)TxPkt,16,PktPtr);
	Bitpack(data_packet.FCB_PULSE_IDX[i][2] ,(unsigned short *)TxPkt,3,PktPtr);
      }
    
      for(i=0;i<NoOfSubFrames;i++) Bitpack(data_packet.FCBG_IDX[i] ,(unsigned short *) TxPkt,5,PktPtr);
      
    }
    else if(data_packet.MODE_BIT==1){
      //FPPP
      Bitpack(data_packet.MODE_BIT,(unsigned short *)TxPkt,1,PktPtr);
      Bitpack(data_packet.DELAY_IDX,(unsigned short *)TxPkt,7,PktPtr);
      Bitpack(data_packet.LSP_IDX[0] ,(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.LSP_IDX[1] ,(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.LSP_IDX[2] ,(unsigned short *)TxPkt,9,PktPtr);
      Bitpack(data_packet.LSP_IDX[3] ,(unsigned short *)TxPkt,7,PktPtr);
      
      Bitpack(data_packet.F_ROT_IDX,(unsigned short *)TxPkt,7,PktPtr);
      Bitpack(data_packet.A_AMP_IDX[0],(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.A_AMP_IDX[1],(unsigned short *)TxPkt,6,PktPtr);
      Bitpack(data_packet.A_AMP_IDX[2],(unsigned short *)TxPkt,8,PktPtr);
      Bitpack(data_packet.A_POWER_IDX,(unsigned short *)TxPkt,8,PktPtr);
      for (i=0; i<NUM_FIXED_BAND; i++) {
#ifdef WMOPS_FX
  test();
#endif
       if (i<3) 
          Bitpack(data_packet.F_ALIGN_IDX[i],(unsigned short *)TxPkt,5,PktPtr); 
        else
          Bitpack(data_packet.F_ALIGN_IDX[i],(unsigned short *)TxPkt,6,PktPtr); 
      }
    }
    break;
    
  }


#ifdef DTX // spectral tilt algorithm
  if (Eargs->dtx) {

  static int posn_prev_NER=0;
  static int ft=1;
  static Word16 prev_refl0=0, flag=0;
  static Word32 ra_deltasum=0;
  static Word16 running_avg=0;
  short nsid=0;
  static short nocompute=0;
  Word16  rc_fx[ORDER + 1];
  static  int nshft=0;
  
  if ((data_packet.PACKET_RATE ==1) && (lastrateE !=1))
    posn_prev_NER=encode_fcnt;
  
  if ((lpcgflag==0) && (nocompute==2))
    nocompute=0;
  
  if (lpcgflag==1) //maybe moving to a FR frame
    nocompute=1;
  
  if ((lpcgflag==0) && (nocompute==1))
    nocompute++;
  
  
  if ((data_packet.PACKET_RATE == 1) && (sub(encode_fcnt,posn_prev_NER)>5) && (nocompute==0))
    {
      /* computing the tilt parameter for noise trigger */
      
      flag=0;
            
	  lsp2lpc_fx(lsp_fx, pci_fx, pci_fx);  //pci_fx in Q12
     
	  for(i=0;i<ORDER;i++)
	     pci_fx[i] = -(pci_fx[i]);
     

	  LPC_pred2refl_fx(pci_fx,rc_fx,ORDER);   //rc_fx in Q15
     
	   
      if (ft==1)
	{
	     prev_refl0=rc_fx[0];
	  ft=0;
		 running_avg=rc_fx[0];
	}
      
	  Word32 temp;
	  Word32 temp1;
      
	  //ra_deltasum += (0.80*running_avg + 0.20*refl[0]) - running_avg; 
	  temp = L_add(L_mult(26213,running_avg),L_mult(6554,rc_fx[0]));
      
	  temp1 = (L_shr(L_mult(32767,running_avg),1));
	  ra_deltasum = L_add(ra_deltasum,(L_sub(L_shr(temp,1),temp1))); //in Q30
      
      prev_refl0=rc_fx[0];                               //Q15
      
      //running_avg= 0.80*running_avg + 0.20*refl[0];
      
	  running_avg = round32_16(temp);                    // running_avg in Q15
      
      
      if (L_abs(ra_deltasum) > 214748364)                //0.2 in Q30
	{
	  flag=1;
	  ra_deltasum=0;
	  running_avg=0;
	  ft=1;
	  nsid=0;
	  if (noSID==1)
	    nsid=1;
	  
	}           
    }
  else
    flag=0;
  
   //printf("flag_fx: %d, %d \n", flag,encode_fcnt);
   //printf("%d, %d %d \n", lpcgflag,noSID,encode_fcnt);
    
    unsigned int Isil=0;
    unsigned char tmpbuf[2]={0,0};
    tmpbuf[1] = TxPkt[0]>> 8; //upper 8 bits => LSF
    tmpbuf[0] = TxPkt[0] & 0x00FF;//lower 8 bits => power
    short rt;
    rt = (short) data_packet.PACKET_RATE;

    Isil = SB_enc.Encode(&rt, tmpbuf,flag,nsid);
    //Isil = SB_enc.Encode(&rt, tmpbuf);
         
    rate=data_packet.PACKET_RATE = rt;
    if (rate !=0)
      TxPkt[0] = (tmpbuf[1] << 8) | tmpbuf[0];
    else
      TxPkt[0]=0;
//    printf("%d %d\n",rate,encode_fcnt);


  }
        
#endif
  
#ifdef WMOPS_FX
  Nmove16(2);
#endif
  lastlastrateE = lastrateE;
  lastrateE = bit_rate;
  rate=bit_rate;
  
  prev_dim_and_burstE = dim_and_burstE;
  LAST_PACKET_RATE_E = PACKET_RATE;
  LAST_LAST_MODE_BIT_E = LAST_MODE_BIT_E;
  LAST_MODE_BIT_E = MODE_BIT;
  
  for (j = 0; j < ORDER; j++) {
    OldOldlspE_fx[j] = OldlspE_fx[j];
    OldlspE_fx[j] = lsp_fx[j];
    Oldlsp_nq_fx[j] = lsp_nq_fx[j];
  }

  for (i = 0; i < PACKWDSNUM; i++)  {
	  codeBuf[i] = TxPkt[i];
  }
  encode_fcnt++;
}

//#ifdef DTX

/*=======================================================================*/
/* convert predictor coefficients (Q12) to reflection coefficients (Q15) */
/* this is identical to EVRC "ptok" routine except that ptok output      */
/* refl coeffs are in Q12.  A loop has been added at the end of this     */
/* routine to convert the Q12 refl coeffs to Q15.                        */
/* Inputs : p_fx: in Q12                                                 */
/*          NPTS:ORDER													 */
/* Output:  k_fx in Q15													 */
/*=======================================================================*/
#define MAX_LPC_ORDER  ORDER

void LPC_pred2refl_fx(Word16 *p_fx, Word16 *k_fx,Word16 NPTS)

{

  Word16   f_fx[MAX_LPC_ORDER];
  Word16   km_fx;
  Word32   L_tmp1,L_tmp2;
  Word16   tmp;
  Word16   denom_mant,denom_exp;
  
  Word16   tmp_denom_exp;
  Word32   new_mant;
  Word16   temp;

  Word16    m,j,n;
  
  /* for debug only !!!! */
  if ( NPTS > MAX_LPC_ORDER ) {
    printf("LPC order is too large!!!\n"); 
    exit(-1);
  } 
  
  /* copy into internal vars so they can be changed */
  for (m=0 ; m < NPTS ; m++) {

#ifdef WMOPS_FX
    move16();
#endif
 //f_fx[m] = p_fx[m];
    f_fx[m] = -p_fx[m];
  }
  
  for (m = NPTS-1; m >= 0; m--)
    {
#ifdef WMOPS_FX
      move16();
#endif
      km_fx = f_fx[m];
      
/* we assume that if there is an overflow in km_fx it will not be above 8. note that under any
	 normal operation, i.e., if the LSFs are properly ordered, there should NEVER be an overflow, 
	 but overflow can occur because of fixed point accuracy!! */
      
/* have to replace this code with a default value of reflection coeffs!!!!  Eyal 17/6/98 */
      if (sub(km_fx,-4096) <= 0 || sub(km_fx,4096) >= 0) { 
//      printf("in Fixed: Nonstable reflection coeffs at frame %ld!!!\n", frm_count); 
	printf("in LPC_pred2refl_fx: Nonstable reflection coeffs!!!\n"); 
	for (j=0 ; j < NPTS ; j++) {
	  k_fx[j] = 0;

#ifdef WMOPS_FX
	  move16();
#endif        
	       }
	         return; 
      }
      
      k_fx[m] = negate(km_fx);
      
#ifdef WMOPS_FX
      move32();
#endif
      L_tmp1 = 0x02000000;                 /* 1 in Q25 */
      L_tmp1 = L_msu(L_tmp1,km_fx,km_fx);   /* 1-km*km in Q25 */
      
#ifdef WMOPS_FX
      move16(); move16();
#endif
      denom_exp = norm_l(L_tmp1);
//      denom_mant = round32_16(L_shl(L_tmp1,denom_exp));    /* in Q(9+denom_exp)  */
      
//           printf("m = %hd denom_exp = %hd denom_mant = %hX\n",m,denom_exp,denom_mant);
//           printf("(1.0-km*km) = %f %f\n",1-km*km,denom_mant*pow(2.,-(9+(double)denom_exp)));
      
#ifdef WMOPS_FX
      move16(); move16();
#endif
//           denom_mant = div_s(0x4000,denom_mant);    /* Q14/Q9 = Q5. have to negate exp and add 5 */
//     divide_s is from TIA basic math library....
//      denom_mant = divide_s(0x4000,denom_mant);   /* Q14/Q9 = Q5. have to negate exp and add 5 */
      
 
	     new_mant = invert_dp(L_tmp1,4, &tmp_denom_exp);   //sum in Q61-Q25-n=Q36-n
         temp =  round32_16(L_shr(new_mant,0));           //in Q14

	  denom_mant = temp;
	  denom_exp = abs_s(sub (tmp_denom_exp,5));
      
            
//           denom = 1./(1.-km*km);
//           printf("tmp_denom_exp = %hd\n",tmp_denom_exp);
//           printf("%f %f\n",denom,denom_mant*pow(2.,(double)tmp_denom_exp)/32768.);
      
      L_tmp1 = L_mult(km_fx,denom_mant);   /* km*denom. Q12*Q15 = Q28 */
      L_tmp1 = L_shl(L_tmp1,3);            /* change to Q31. simulation showed no overflow */
#ifdef WMOPS_FX
      move16();
#endif
      tmp = round32_16(L_tmp1);             /* extract in Q15 */
      
      
      for (j = 0; j < m/2; j++)
	{
	  n = sub(sub(m,(Word16)1),j);
	  L_tmp1 = L_mult(denom_mant,f_fx[j]);     /* denom*f[j]. Q15*Q12 = Q28 (floating with exp) */
	  L_tmp1 = L_mac(L_tmp1,tmp,f_fx[n]);      /* denom*f[j]+km*denom*f[n] in Q28 (floating with exp) */
	  
	  L_tmp2 = L_mult(denom_mant,f_fx[n]);     /* denom*f[n]. Q15*Q12 = Q28 (floating with exp) */
	  L_tmp2 = L_mac(L_tmp2,tmp,f_fx[j]);      /* denom*f[n]+km*denom*f[j] in Q28 (floating with exp) */
	  
	  L_tmp1 = L_shl(L_tmp1,denom_exp); /* bringing to true Q28 */
	  L_tmp2 = L_shl(L_tmp2,denom_exp); /* bringing to true Q28 */
	  
#ifdef WMOPS_FX
	  move16(); move16();
#endif
	  f_fx[j] = round32_16(L_tmp1);             /* extracting in Q12 */
	  f_fx[n] = round32_16(L_tmp2);             /* extracting in Q12 */
	  
	}
      
#ifdef WMOPS_FX
      logic16(); test();
#endif
      if (m & 1) { 
	L_tmp1 = L_mult(denom_mant,f_fx[j]);  /* denom*f[j]. Q15*Q12 = Q28 (floating with exp) */
	L_tmp1 = L_mac(L_tmp1,tmp,f_fx[j]);   /* denom*f[j]+km*denom*f[j] in Q28 (floating with exp) */
	L_tmp1 = L_shl(L_tmp1,denom_exp);     /* bringing to true Q28 */
#ifdef WMOPS_FX
	move16();
#endif
	f_fx[j] = round32_16(L_tmp1);                 /* extracting in Q12 */
      }
    }

/* not in original "ptok" routine */
  for (j = 0; j < 10; j++)
    k_fx[j] = shl(k_fx[j], 3);  /* convert to Q15 */
  
  return;
}

//#endif

