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

static char const rcsid[]="$Id: celp_fx.cc,v 1.64 2006/12/07 00:02:23 apadmana Exp $";

/*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for             */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 1999 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/

#include "proto_fx.h"
#include "typedef_fx.h"
#include "globs_fx.h"
#include "ppvq_fx.h"
#include "lpcana_tab.h"
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "rcelp_tab.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


Word16 gnvq_4_fx[16]={
2, 3, 4, 7, 12, 20, 33, 55, 
90, 148, 245, 403, 665, 1097, 1808, 2981
};

Word16 gnvq_8_fx[32]={
1, 2, 2, 3, 3, 4, 6, 7, 
9, 12, 16, 20, 26, 33, 43, 55, 
70, 90, 116, 148, 191, 245, 314, 403, 
518, 665, 854, 1097, 1408, 1808, 2322, 2981
};

/*===================================================================*/
/* FUNCTION      :  celp_encoder_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  CELP encoding for the current frame              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16[])  HPspeech_fx : Input speech signal (Q0)           */
/*    _ (Word16[])  OldlspE_fx  : Last frame quantized lsp (Q15)     */
/*    _ (Word16[])  Oldlsp_nq_fx  : Last frame un-quantized lsp(Q15) */
/*    _ (Word16[])  lsp_nq_fx  : Current frame un-quantized lsp (Q15)*/
/*    _ (Word32[])  SynMemoryM_fx : (Q16)                            */
/*    _ (Word16[])  WFmemFIR_fx  : FIR memory (Q0)                   */
/*    _ (Word32[])  WFmemIIR_fx  : IIR memory (Q16)                  */
/*    _ (Word16[])  residualm  : modified residual (Q0)              */

/*-------------------------------------------------------------------*/
/* GLOBAL OUTPUT ARGUMENTS :                                         */
/*    _ (Word16[]) Excitation_fx : Excitation output signal (Q0)     */
/*    _ (Word16[]) lsp_fx  :  Current frame quantized lsp output(Q15)*/
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16[])  pci_fx,lspi_fx : Interpolated prediction/lsp
                                coefficients (Q12/Q15)               */
/*    _ (Word32[])  SynMemoryM_fx,  WFmemIIR_fx : Synthesis
                    filter memory (Q16)                              */

/*    _ (Word16[])  WFmemFIR_fx : Prediction filter memory (Q0)      */ 
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
void celp_encoder_fx(int bit_rate)
{
  register int i, j, n;
  short   subframesize;
  Word32  delayi_fx[3];
  Word16  sum1_fx;
  Word16  temp;
  Word32 delay[SubFrameSize+10];

  // Gain Quantization Setups
#ifdef WMOPS_FX
	  test();
	  Nmove16(2);
#endif
  if (bit_rate==3)
    {FCBGainSize=16; gnvq_fx=gnvq_4_fx;}
  else
    {FCBGainSize=32; gnvq_fx=gnvq_8_fx;}
  
  // LSP Quantization
#ifdef WMOPS_FX
	  test();
#endif
  if (bit_rate==3) {
   enc_lsp_vq_22_fx(lsp_nq_fx, SScratch_fx, lsp_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("CELP_ENCODER: HR LSPVQ ...................................");
#endif
   for (i=0; i<3; i++) {
        data_packet.LSP_IDX[i]=SScratch_fx[i];
   }
  }
  else {
    enc_lsp_vq_28_fx(lsp_nq_fx, SScratch_fx, lsp_fx);
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_ENCODER: FR LSPVQ ...................................");
#endif
	for (i=0; i<4; i++) {
         data_packet.LSP_IDX[i]=SScratch_fx[i];
	}
  }
 
  for (i=0;i<ORDER;i++) {
	  cbprevprev_E_fx[i]=cbprev_E_fx[i]=lsp_fx[i];  
  }

#ifdef LSP_SPRD
 lsp_spread(lsp_fx);
#endif  
  /* Update filter memory for voiced coder to generate residual with
     quantized LSPs                                               */
  for (i=0;i<ORDER;i++) FIRmem_fx[i]=mspeech_fx[FrameSize-1-i];

  // Start Full/Half CELP Processing
  for (i=0; i<NoOfSubFrames; i++) {
#ifdef WMOPS_FX
	  char tmpstr[100];
#endif
#ifdef WMOPS_FX
	  test();
#endif
    if (i < 2) subframesize = sub(SubFrameSize, 1);
    else subframesize = SubFrameSize;
    /* interpolate lsp */
    Interpol_fx(OldlspE_fx, lsp_fx, lspi_fx, i);
    Interpol_fx(Oldlsp_nq_fx, lsp_nq_fx, lspi_nq_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, pci_fx, pci_fx);
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Interpol LSP & LSP2LPC SF-%d ...............",i);
      WMP_fwc_fx(tmpstr);
#endif
    /* Get zir */
    weight_fx(pci_nq_fx, gamma90_tab, wpci_gma1_fx, 10);
    weight_fx(pci_nq_fx, gamma50_tab, wpci_gma2_fx, 10);
    ZeroInput_fx(Excitation_fx+ACBMemSize, pci_fx, wpci_gma1_fx,
		  wpci_gma2_fx, zir_fx, subframesize, 0);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: GET WLSP and ZIR SF-%d ......................",i);
      WMP_fwc_fx(tmpstr);
#endif
    
    /* Calculate impulse response of 1/A(z) * A(z/g1) / A(z/g2) */
    ImpulseRzp_fx(pci_fx, wpci_gma1_fx, wpci_gma2_fx, H_fx, Hlength);   
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: ImpResp SF-%d ...............................",i);
      WMP_fwc_fx(tmpstr);
#endif
    /* Interpolate delay */
    Interpol_delay_fx(pdelay_fx, delay_fx, delayi_fx, i);
    delay_contour_fx(delayi_fx, subframesize, delay);
    rclp_block_interp_fx(Excitation_fx+ACBMemSize, delay, intrp_tab8, 8, subframesize+10);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Interp Delay, ACB ext and RCELP SF-%d ......",i);
      WMP_fwc_fx(tmpstr);
#endif
   
    /* Remove Zero input response from weighted speech */
    for (j=0; j<subframesize; j++) worigm_fx[j] = sub(wspeech_fx[i*(SubFrameSize-1)+j], zir_fx[j]);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Sub ZIR to get Targ SF-%d ...................",i);
      WMP_fwc_fx(tmpstr);
#endif

    /* Calculate closed loop gain */
    getgain_fx(Excitation_fx+ACBMemSize, worigm_fx, H_fx, subframesize, sum1_fx, idxppg);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: ACB Gain compute and quant SF-%d ............",i);
      WMP_fwc_fx(tmpstr);
#endif
    
    /* Get TARGET for fixed c.b. */
    /* Convolve excitation with H */
    /* ExconvH stored in Scratch memory */
    ConvolveImpulseR_fx(Excitation_fx+ACBMemSize, H_fx, ExconvH_fx, Hlength, subframesize);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Convolve ACB with ImpResp SF-%d .............",i);
      WMP_fwc_fx(tmpstr);
#endif
    for (j=0; j<subframesize; j++) TARGETw_fx[j] = sub(worigm_fx[j], ExconvH_fx[j]);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Get FCB Target SF-%d ........................",i);
      WMP_fwc_fx(tmpstr);
#endif
    /* Convert TARGET from weighted domain to residual domain */
	Weight2Res_fx(TARGETw_fx, TARGET_fx, pci_fx, wpci_gma1_fx, wpci_gma2_fx, ORDER, subframesize);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Converting WeightTarg to Res SF-%d ..........",i);
      WMP_fwc_fx(tmpstr);
#endif

#ifdef WMOPS_FX
	  test();
	  Nmove16(3);
#endif
    if (subframesize < SubFrameSize)
      TARGETw_fx[subframesize]=TARGET_fx[subframesize]=Scratch_fx[subframesize]=0;
    /* get delay for current subframe */
    n = round32_16(L_shr(L_add(delayi_fx[1], delayi_fx[0]), 1));
    /* Compute fixed codebook contribution */
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
    if (sub(n,subframesize)>0) n = 200;
#ifdef WMOPS_FX
	  test();
#endif
    if (sub(bit_rate,4)==0) {
      /* ACELP fixed codebook search */
      ACELP_Code_fx(TARGETw_fx, TARGET_fx, H_fx, (int) n, sum1_fx, subframesize, Scratch_fx,
		 &fcbGain_fx, fcbIndexVector_fx, 1);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: FR FACTORIAL FCB SF-%d ......................",i);
      WMP_fwc_fx(tmpstr);
#endif
    }
    else {//bit_rate==3 case
      cod3_10jcelp_fx(TARGETw_fx, H_fx, (Word16)n, ppvq_fx[idxppg], subframesize, Scratch_fx, &fcbGain_fx, fcbIndexVector_fx);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: HR JCELP FCB SF-%d ..........................",i);
      WMP_fwc_fx(tmpstr);
#endif
    }
   
    /* constrain fcb gain */
    fcbGain_fx = round32_16(L_shl(L_mult(fcbGain_fx, sub(16384, round32_16(L_mult(ppvq_fx[idxppg], 4915)))), 1));
    
#ifdef WMOPS_FX
	  test();
#endif
    if (sub(lastrateE,2)<=0 ) {
#ifdef WMOPS_FX
	  Ntest(2);
#endif
      if (i==0)
	fcbGain_fx = round32_16(L_mult(fcbGain_fx, 14746));    // Q0
      else if (i==1)
	fcbGain_fx = round32_16(L_mult(fcbGain_fx, 22938));    
      else	
	fcbGain_fx = round32_16(L_mult(fcbGain_fx, 27853));
    }

    for (idxcbg=0,j=1; j<FCBGainSize; ++j) {
#ifdef WMOPS_FX
	  test();
#endif
      if (sub(fcbGain_fx,shr(add(gnvq_fx[j], gnvq_fx[j - 1]), 1))>0) idxcbg = j;
    }
#ifdef WMOPS_FX
	move16();
#endif
    fcbGain_fx = gnvq_fx[idxcbg];
    /* Add to total excitation */
    for (j=0; j<subframesize; j++) { 
         temp = round32_16(L_shl(L_mult(Scratch_fx[j], fcbGain_fx), 4));    // Scratch_fx is Q11 
         Excitation_fx[j+ACBMemSize] = add(Excitation_fx[j+ACBMemSize], temp);    // Q0
    }
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: FCB Gain Quant and Compute total excit SF-%d ",i);
      WMP_fwc_fx(tmpstr);
#endif
	/* Update filters memory */
    ZeroInput_fx(Excitation_fx+ACBMemSize, pci_fx, wpci_gma1_fx,
		  wpci_gma2_fx, zir_fx, subframesize, 1);
    for (j=0; j<ACBMemSize; j++) {
      Excitation_fx[j] = Excitation_fx[j + subframesize];
    }
    
#ifdef WMOPS_FX
	  test();
	  Nmove16(8);
#endif
    if (sub(bit_rate,4)==0) {
      /* Pack bits */
      /* ACB gain index */
      data_packet.ACBG_IDX[i]=idxppg;
      idxcb = fcbIndexVector_fx[0];
      data_packet.FCB_PULSE_IDX[i][0]=idxcb;
      idxcb = fcbIndexVector_fx[1];
      data_packet.FCB_PULSE_IDX[i][1]=idxcb;
      idxcb = fcbIndexVector_fx[2];
      data_packet.FCB_PULSE_IDX[i][2]=idxcb;
      /* FCB gain index */
      data_packet.FCBG_IDX[i]=idxcbg;
    }
    else {//bit_rate==3 case
      /* Pack bits */
      /* ACB gain index */
#ifdef WMOPS_FX
	  Nmove16(4);
#endif
       data_packet.ACBG_IDX[i]=idxppg;
      idxcb = fcbIndexVector_fx[0];
      //Bitpack(idxcb, (unsigned short *) PackedWords, 11, PackWdsPtr);
      data_packet.FCB_PULSE_IDX[i][0]=idxcb;
      /* FCB gain index */
      data_packet.FCBG_IDX[i]=idxcbg;
    }
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_ENCODER: Update mem and Finish encoder SF-%d .........",i);
      WMP_fwc_fx(tmpstr);
#endif

  }
#ifdef WMOPS_FX
	  Nmove16(2);
#endif
  pdelay_fx = delay_fx;
  //packing routine
  PktPtr[0]=16;PktPtr[1]=0;
#ifdef WMOPS_FX
	  test();
	  move16();
	  move16();
#endif
  if (sub(bit_rate,4)==0){//full rate
    data_packet.PACKET_RATE=4; data_packet.MODE_BIT=0;
  }
  else{ //for(bit_rate==3) 
    data_packet.PACKET_RATE=3;
  }

  return;
}


/*===================================================================*/
/* FUNCTION      :  celp_decoder_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  CELP decoding for the current frame              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16[])  OldlspD_fx  : Last frame quantized lsp (Q15)     */
/*    _ (Word32[])  SynMemory_fx : (Q16)                             */
/*    _ (Word16[])  PitchMemoryD_fx  : Pitch Memory                  */
/*    _ (Word16)    delay_fx  :                                      */
/*    _ (Word16)    pdelayD_fx  :                                    */
/*    _ (Word16)    data_pack.LSP_IDX  :                             */
/*-------------------------------------------------------------------*/
/* GLOBAL OUTPUT ARGUMENTS :                                         */
/*    _ (Word16[]) DECspeechPF_fx  :  output speech (Q0)             */
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16[])  lsp_fx, lspi_fx : Interpolated prediction/lsp
                                coefficients (Q12/Q15)               */
/*    _ (Word32[])  SynMemoryD_fx 
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
#ifdef VOIP_DORA_CELP
     short celp_decoder_fx(Word16 *outFbuf,short post_filter,int bit_rate,Word16 phase_offset, Word16 run_length, Word16 time_warp_fraction, short*     obuf_len)
#else
     short celp_decoder_fx(Word16 *outFbuf,short post_filter,int bit_rate)
#endif
{
  
  register Word16 i, j, n;
  register Word16 *foutP;
  short   subframesize;
  DTFS_fx    LAST_BAD_CW;
  bool    local_lpc_flag=FALSE;
#ifdef VOIP_DORA_CELP  
  Word16  PitchMemoryD_Frame_fx[160*2];
#else
  Word16  PitchMemoryD_Frame_fx[160];
#endif
  Word16  temp, sum1_fx;

  Word16  prev_ave_fcb_gain_fx,curr_ave_acb_gain_fx,ct;
  
  Word16 sum_acb_fx;
 
  Word16 ck_fx[55];

  Word32 delayi_fx[3];  // Q16
  
  Word16 sin_tab[480], cos_tab[480];
  Word32 Ltemp;
  DTFS_fx prev, curr;
  Word16 tmplsp[ORDER], lpc1[ORDER], lpc2[ORDER], ph_offset = 0;
  Word16 pf_temp1[MAXLAG_WI]; //maynot need more than MAXLAG_WI/2+1
  Word16 pf_temp2[MAXLAG_WI];
  Word16 pf_temp[MAXLAG_WI];
  Word16 pf_n2[MAXLAG_WI];
  //extern Word16 C_fx[241], S_fx[241];
  extern void GetCosSinTable_fx(short L);

    /*some half-rate specific stuff*/
  Word16 rnd,prd;

  /* some FER quality improvements */
  Word16 tmp_curr_rc_fx[LPCORDER];
  Word16 tmp_prev_rc_fx[LPCORDER];
  Word16 tmp_pci_fx[LPCORDER];
  Word16 diag_flag=0;
  
  
  
#if 0
  time_warp_fraction = 16384;
#endif


#ifdef VOIP_DORA_CELP
 
 //Variables used for Phase Matching and Warping

  Word16    do_phase_matching = 0;
  int		num_residual = 160;
  int		pm_var1 = 0, pm_var2;
  int		temp_samples_count = 0;
  int		num_samples;
  int		temp_pitch[320];
  int		b, c, merge_start, merge_end, merge_stop, sample_counter;
  int		store_ddelay;
  //End Variables used for Phase Matching and Warping
#endif

 

  static Word16 Seed_fx = 0, celp_extra_flag_fx=0, celp_extra_ran_fx ;
  
  LAST_BAD_CW.lag=0;
  Word16 ACB_ATTN=0;
  // Gain Quantization Setups
#ifdef WMOPS_FX
	  test();
	  move16();
	  move16();
#endif


#ifdef VOIP_DORA_CELP
 
  if (phase_offset != 10) //Phase Offset == 10 denotes Phase Matching disabled
	do_phase_matching = 1; //check for Phase Matching
#endif

  if (sub(bit_rate,3)==0)
    {FCBGainSize=16; gnvq_fx=gnvq_4_fx;}                // Q0
  else {
	  FCBGainSize=32; 
	  gnvq_fx=gnvq_8_fx;
  }

  // LSP De-Quantization
#ifdef WMOPS_FX
	  test();
#endif
  if (sub(bit_rate,3)==0) {
#ifdef WMOPS_FX
#endif

#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: HR LSPVQ ...................................");
#endif
  }
  else {
#ifdef WMOPS_FX
#endif

#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: FR LSPVQ ...................................");
#endif
  }

  /* Check for monontonic LSP */
#ifdef WMOPS_FX
	  Ntest(ORDER);
	  Nmove16(ORDER);
#endif
  for (j=1; j<ORDER; j++) if (lsp_fx[j] <= lsp_fx[j - 1]) errorFlag=1;  

  for (i=0;i<ORDER;i++) cbprevprev_D_fx[i]=cbprev_D_fx[i]=lsp_fx[i];    
  
#ifdef LSP_SPRD
  lsp_spread(lsp_fx);
#endif


  
  // Do CELP Decoding
  delay_fx= add(data_packet.DELAY_IDX,DMIN);                  // Q0

#ifdef VOIP_DORA_CELP
   store_ddelay = data_packet.DELTA_DELAY_IDX; //Phase Matching: need to store ddelay
#endif


#ifdef WMOPS_FX
	  test();
#endif
  if (sub(lastrateD,3)<0) pdelayD_fx=delay_fx;            // Q0
  /* Fix delay countour of previous erased frame */
#ifdef VOIP_DORA_CELP
   // Fix delay contour only if no Phase Matching; else PM fixes it anyway
  if (do_phase_matching){
	
    GetSinCosTab_fx(pdelayD_fx, sin_tab, cos_tab);
	LAST_BAD_CW.to_fs_fx(PitchMemoryD_fx+ACBMemSize-(int)pdelayD_fx,(int)pdelayD_fx,sin_tab,cos_tab);
	}
    
    
#endif

#ifdef WMOPS_FX
	  test();
#endif

#ifdef  VOIP_DORA_CELP
   if (!do_phase_matching)
  {
#endif
     if (sub(fer_counter,2)==0) {  
        Word40 lpc_gain1_fx, lpc_gain2_fx;
        Word16 LPCG_THD_fx;
        Word16 temp_in[Hlength];
        Word32 temp_mem[ORDER];

#ifdef WMOPS_FX
	  test();
	  move16();
#endif
    if (sub(lastrateD,4)==0) LPCG_THD_fx=5120;       // 2.5: Q11
    else LPCG_THD_fx=10240;                   // 5.0: Q11

    //prev
    lsp2lpc_fx(OldlspD_fx, pci_fx, pci_fx);
#ifdef WMOPS_FX
    Nmove16(ORDER+Hlength);
#endif
    temp_in[0]=0x0800;   // one impulse 1.0 in Q11
    for (j=1; j<Hlength; j++) temp_in[j]=0;
    for (j=0;j<ORDER;j++) temp_mem[j]=0;
    synthesis_filter_fx (pci_fx, temp_in, H_fx, temp_mem,
		       ORDER, Hlength, 3);
    
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: fer 2: ImpulseRzp ..........................");
#endif
    /* Get energy of H */
    for (j=0,lpc_gain1_fx=0; j<Hlength; j++) 
		lpc_gain1_fx = L_mac40(lpc_gain1_fx, H_fx[j], H_fx[j]);  // Q23 , prev
#ifdef WMOPS_FX
    Nmove16(ORDER+Hlength);
#endif

    for(j=0;j<ORDER;j++)
      tmp_pci_fx[j] = -(pci_fx[j]); //Q12
    
    LPC_pred2refl_fx(tmp_pci_fx,tmp_prev_rc_fx,ORDER);   //tmp_prev_rc_fx in Q15
    
    //curr
    lsp2lpc_fx(lsp_fx, pci_fx, pci_fx);
    temp_in[0]=0x0800;   // one impulse 1.0 in Q11
    for (j=1; j<Hlength; j++) temp_in[j]=0;
    for (j=0;j<ORDER;j++) temp_mem[j]=0;
    synthesis_filter_fx (pci_fx, temp_in, H_fx, temp_mem,
		       ORDER, Hlength, 3);
    
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: fer 2:lsp2lpc and ImpulseRzp,get eneryg of H");
#endif
    /* Get energy of H */
    for (j=0,lpc_gain2_fx=0; j<Hlength; j++) 
      lpc_gain2_fx = L_mac40(lpc_gain2_fx, H_fx[j], H_fx[j]); // Q23, curr


    for(j=0;j<ORDER;j++)
      tmp_pci_fx[j] = -(pci_fx[j]); //Q12
    
    LPC_pred2refl_fx(tmp_pci_fx,tmp_curr_rc_fx,ORDER);   //tmp_curr_rc_fx in Q15


   
    
    /* determine spectral transistion degree (set flag if large) --
       for frame erasures */
    Ltemp=(Word32)L_sat32_40(divide_dp(lpc_gain1_fx, lpc_gain2_fx, -2)); // Ltemp in Q27

    //printf("\n Nr = %f Dr = %f FL div = %f FX div = %d\n",lpc_gain1_fx, lpc_gain2_fx,(lpc_gain1_fx/lpc_gain2_fx)*pow(2.0,27),Ltemp);
#ifdef WMOPS_FX
	  test();
	  move16();
#endif 
	  
	  /* 0x3880626C is 14.125375 in Q26 , which is ~11.5 dB decrease in LPC gain from prev to curr */  
	  if (((sub(tmp_curr_rc_fx[0],-13763)<0) && (sub(tmp_prev_rc_fx[0],20644)>0)) || (L_sub(L_shr(Ltemp,1),0x3880626C)>0)) {
	    //setting voicing thld to -0.42 and unvoicing thld to +0.63 
	    //printf("Frame %lf, fer_counter is %d - curr_rc is %f, curr_lpcg is %f, prev_rc is %f, prev_lpcg is %f, div is %d %d %f\n",(decode_fcnt+1)*0.02,fer_counter,tmp_curr_rc_fx[0]/32768.0,10*log10(lpc_gain2_fx/pow(2.0,23)),tmp_prev_rc_fx[0]/32768.0,10*log10(lpc_gain1_fx/pow(2.0,23)), L_shr(Ltemp,1),0x3880626C,(lpc_gain1_fx/lpc_gain2_fx)*pow(2.0,26));
	    
	    diag_flag=1;
	  }


/* If an unvoiced --> voiced transition is detected or if the lpc
gain decreases from prev to curr && the previous frame was erased
then disable the LSP interpolation for the current frame */
	  if (diag_flag==1)
	    {
	      for(j=0;j<LPCORDER;j++)
		{
		OldlspD_fx[j]=lsp_fx[j];
		  //OldlspD_fx[j] = round32_16(L_add(L_mult(24576,OldlspD_fx[j]),L_mult(8192,lsp_init_tab[j])));
			                                  //0.75                      //0.25
		}
	    }

	    
	  if (L_sub(Ltemp,L_deposit_h(LPCG_THD_fx))>0) { 
	    local_lpc_flag = TRUE;
	  }

    if (prev_prev_frame_error == 1) // 2 consec erasures before this clean frame
      {
      	if (lpc_gain2_fx > 0xFCFB725) //15dB= 0xFCFB725
	  ACB_ATTN=1;
	
      }
     }
     
  j=data_packet.DELTA_DELAY_IDX;
#ifdef WMOPS_FX
	  test();
	  logic16();
	  move16();
#endif
  if (sub(lastrateD,2)<=0 && sub(fer_counter,2)==0) data_packet.ACBG_IDX[0]=0;
#ifdef WMOPS_FX
	  test();
	  Nlogic16(7);
#endif
  if (sub(bit_rate,4)==0 && sub(lastrateD,2)>0 && j!=0  && (sub(fer_counter,2)==0 || (sub(fer_counter,1)==0 && sub(lastrateD,3) ==0 && prev_rcelp_half==0 && LAST_PPP_MODE_D_fx ==0))) {
    
    pdelayD_fx=sub(delay_fx,sub(j,16));
    if (pdelayD_fx > DMAX) pdelayD_fx=DMAX;
    else if (pdelayD_fx < DMIN) pdelayD_fx=DMIN;

    GetSinCosTab_fx(pdelayD_fx, sin_tab, cos_tab);
    LAST_BAD_CW.to_fs_fx(PitchMemoryD_fx+sub(ACBMemSize,pdelayD_fx), pdelayD_fx, sin_tab, cos_tab);
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: fer to_fs of LAST_BAD_CW ...................");
#endif
    
#ifdef WMOPS_FX
	  test();
#endif
    if (sub(fer_counter,2)==0) {
#ifdef WMOPS_FX
	  test();
#endif
	  if (!prev_prev_frame_error){ 
      if (sub(abs_s(sub(pdelayD_fx,pdelayD2_fx)),15) >0 ) pdelayD2_fx=pdelayD_fx;
	  for (i=0; i<ACBMemSize; i++) {
		  PitchMemoryD_fx[i] = PitchMemoryD2_fx[i];
	  }
      for (i=0; i<NoOfSubFrames; i++) {	
#ifdef WMOPS_FX
	  test();
#endif
	if (i < 2) subframesize = SubFrameSize - 1;
	else subframesize = SubFrameSize;      
	// Interpolate delay
	Interpol_delay_fx(pdelayD2_fx, pdelayD_fx, delayi_fx, i);
	// Compute adaptive codebook contribution
#ifdef WMOPS_FX
	  test();
#endif
	  
	  if (i==0) sum_acb_fx=MIN_FX(ave_acb_gain_fx,15565); //capping to 0.95  
	  else sum_acb_fx=15565; // 0.95 in Q14
	acb_excitation_fx(PitchMemoryD_fx + ACBMemSize, delayi_fx, sum_acb_fx, subframesize);
	for (j=0; j<ACBMemSize; j++) {
		PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
	}
      }
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: fer2 ACB extension .........................");
#endif
	  }
    }
    else {
      pdelayD2_fx=sub(pdelayD_fx,pdeltaD_fx);
    if (pdelayD2_fx > DMAX) pdelayD2_fx=DMAX;
    else if (pdelayD2_fx < DMIN) pdelayD2_fx=DMIN;
	  for (i=0; i<ACBMemSize; i++) {
		  PitchMemoryD_fx[i] = PitchMemoryD3_fx[i];
	  }
      for (i=0; i<NoOfSubFrames; i++) {	
	if (i < 2) subframesize = SubFrameSize - 1;
	else subframesize = SubFrameSize;      
	// Interpolate delay
	Interpol_delay_fx(pdelayD3_fx, pdelayD2_fx, delayi_fx, i);
	// Compute adaptive codebook contribution
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
	if (i==0) sum_acb_fx=MIN_FX(ave_acb_gain_back_fx,15565);
	else sum_acb_fx=15565;  // 0.95
	acb_excitation_fx(PitchMemoryD_fx + ACBMemSize, delayi_fx, sum_acb_fx, subframesize);
	for (j=0; j<ACBMemSize; j++) {
		PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
	}
      }
#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER: fer1 ACB extension .........................");
#endif

	 
 //Do another ACB extension with pdelayD2_fx and pdelayD_fx 
for (i=0; i<NoOfSubFrames; i++) {	
	if (i < 2) subframesize = SubFrameSize - 1;
	else subframesize = SubFrameSize;      
	// Interpolate delay
	Interpol_delay_fx(pdelayD2_fx, pdelayD_fx, delayi_fx, i);
	// Compute adaptive codebook contribution
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
	if (i==2) sum_acb_fx=MIN_FX(14746,ave_acb_gain_fx); //0.9 in Q14
	else sum_acb_fx=14746;  // 0.9
	acb_excitation_fx(PitchMemoryD_fx + ACBMemSize, delayi_fx, sum_acb_fx, subframesize);
	for (j=0; j<ACBMemSize; j++) {
		PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
	}
}
	    
     }//else
   }
#ifdef VOIP_DORA_CELP
  }
  // End: Fix delay contour only if no Phase Matching; else PM fixes it anyway
#endif

#ifdef WMOPS_FX
	  WMP_fwc_fx("CELP_DECODER:Prepare for FER related WI processing in CELP");
#endif

  /* Smooth interpolation if the difference between delays is too big */
#ifdef WMOPS_FX
	  test();
	  Nmove16(2);
#endif
  if (sub(abs_s(sub(delay_fx,pdelayD_fx)),15) > 0) pdelayD_fx = delay_fx;

  
curr_ave_acb_gain_fx=0;
if (SPL_HCELP==1)
  {
  ct=0;
  for(i=0;i<3;i++)
    {
    if (ppvq_fx[data_packet.ACBG_IDX[i]] < 9011) //0.5499
      ct=add(ct,1);
    Ltemp=L_shl(L_mult(ppvq_fx[data_packet.ACBG_IDX[i]],add(i,1)),14); // Ltemp=sum_acb_fx*(i+1), Q29
    Ltemp=(Word32)Mpy_32_16(Ltemp,10923); // Ltemp in Q30, 10923=1/6 in Q16
    curr_ave_acb_gain_fx = add(curr_ave_acb_gain_fx, round32_16(Ltemp));
    }
   if (curr_ave_acb_gain_fx==0)
     curr_ave_acb_gain_fx = ave_acb_gain_fx;
  }
 
  prev_ave_fcb_gain_fx = ave_fcb_gain_fx;
    
  ave_acb_gain_fx = ave_fcb_gain_fx = 0;
  foutP = outFbuf;
  for (i=0; i<NoOfSubFrames; i++) {
#ifdef WMOPS_FX 
	  char tmpstr[100];
#endif
#ifdef WMOPS_FX
	  test();
#endif
    if (i < 2) subframesize = sub(SubFrameSize, 1);
    else subframesize = SubFrameSize;
    Interpol_fx(OldlspD_fx, lsp_fx, lspi_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, pci_fx, pci_fx);
    Interpol_delay_fx(pdelayD_fx, delay_fx, delayi_fx, i);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_DECODER: Interpol LSP and lsp2lpc SF-%d ..............",i);
      WMP_fwc_fx(tmpstr);
#endif
    
#ifdef WMOPS_FX
	  test();
	  Nmove16(3);
#endif
if (SPL_HCELP==0)
  { 
    if (bit_rate==4) {
     fcbIndexVector_fx[0]=data_packet.FCB_PULSE_IDX[i][0];
     fcbIndexVector_fx[1]=data_packet.FCB_PULSE_IDX[i][1];
     fcbIndexVector_fx[2]=data_packet.FCB_PULSE_IDX[i][2];
     /* FCB gain index */
    }
    else {//bit_rate==3 case
      fcbIndexVector_fx[0] = data_packet.FCB_PULSE_IDX[i][0];
    }
#ifdef WMOPS_FX
	  move16();
#endif
    sum_acb_fx = ppvq_fx[data_packet.ACBG_IDX[i]];          // Q14

    if (ACB_ATTN ==1) sum_acb_fx = mult_r(sum_acb_fx, 24576);

    if (fer_counter == 2)
      sum_acb_fx=MIN_FX(sum_acb_fx,15565); //0.95 in Q14
      
   
    Ltemp=L_shl(L_mult(sum_acb_fx,add(i,1)),14); // Ltemp=sum_acb_fx*(i+1), Q29
    Ltemp=(Word32)Mpy_32_16(Ltemp,10923); // Ltemp in Q30, 10923=1/6 in Q16
    ave_acb_gain_fx = add(ave_acb_gain_fx, round32_16(Ltemp));
    

    if (lastrateD == 1) ave_acb_gain_fx = 0; 
    acb_excitation_fx(PitchMemoryD_fx+ACBMemSize, delayi_fx, sum_acb_fx, subframesize); 

    /* Compute fixed codebook contribution */
    temp = round32_16(L_mult(gnvq_fx[ data_packet.FCBG_IDX[i] ], 10923)); //Q0, 10923 is 1/NoOfSubFrames in Q15
    ave_fcb_gain_fx = add(ave_fcb_gain_fx, temp); // Q0

    if (sub(sum_acb_fx,14746)>0) sum_acb_fx = 14746;   // 0.9, Q14 
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
	  
    if (sub(sum_acb_fx,3277)<0) sum_acb_fx = 3277;     // 0.2, Q14
    /* get intrpolated delay for this subframe */
    n = round32_16(L_shr(L_add(delayi_fx[1], delayi_fx[0]), 1));
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
    if (sub(n,subframesize)>0) n = 200;
    /* Compute fixed codebook contribution */
#ifdef WMOPS_FX
	  test();
#endif
  
    if (sub(bit_rate,4)==0) {
      factorial_unpack_fx(fcbIndexVector_fx,ck_fx);
      for(j=0;j<subframesize;j++) Scratch_fx[j]=shl(ck_fx[j], 11);   // Q11
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_DECODER: fact_unpack  SF-%d ..........................",i);
      WMP_fwc_fx(tmpstr);
#endif
    }
    else {// bit_rate==3 case
       dec3_10jcelp_fx(n, sum_acb_fx, subframesize, fcbIndexVector_fx, Scratch_fx);
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_DECODER: dec3_10jcelp SF-%d ..........................",i);
      WMP_fwc_fx(tmpstr);
#endif
    }
#ifdef WMOPS_FX
	  test();
#endif
    if (sub(bit_rate,4)==0) pit_shrp_fx(Scratch_fx, (int) (n), shl(sum_acb_fx, 1), subframesize);
#ifdef WMOPS_FX
	  move16();
#endif
    sum1_fx = gnvq_fx[data_packet.FCBG_IDX[i]];    // Q0

#ifdef VOIP_DORA_CELP

	if (do_phase_matching) //check for Phase Matching: main Phase Matching processing
	{
		//calculation of phase here
		if (i == 0)
		{
			int prev_delay, prev_delay2;
			int prev_delay_end, prev_delay2_end;
			int encoder_phase, decoder_phase;
			Word16 temp1=0, temp2=0, temp3=0, temp=0, temp4=0, n,n1;
			Word16 one_pdelay, one_pdelayD;
			Word32 one_prevdelay,one_predelayD;
								
			//if (idxppg != 0) //get delay of previous frame from current frame
			{
				prev_delay_end =  sub(delay_fx,sub (store_ddelay, 16));
				prev_delay2_end = shr(add(prev_delay_end, pdelayD_fx),1);
	
				if ((phase_offset == 0 && run_length == 1) || (phase_offset == 1 && run_length == 2))
					prev_delay = shr(add(prev_delay_end, pdelayD_fx),1);
				else
					prev_delay = shr(add(prev_delay_end,prev_delay2_end ),1);

				prev_delay2 = shr(add(prev_delay2_end, pdelayD_fx),1);
			}
						
			//Different cases using Phase Matching
			//Get phase of encoder and current phase of decoder
			//The difference between these is the amount of Phase Matching to be done

			div_int_sp(160, prev_delay,  temp); /*fmod(160,prev_delay)*/
			div_int_sp(160, pdelayD_fx,  temp2);
			div_int_sp(160, prev_delay2, temp3);


			one_prevdelay=invert_lut(L_deposit_h(prev_delay),&n);
			one_predelayD=invert_lut(L_deposit_h(pdelayD_fx),&n1);
			one_pdelay=round32_16(one_prevdelay);       //1/prev_delay in Q20//
			one_pdelayD=round32_16(one_predelayD);      //1/pdelayD_fx in Q20//
			
			if (phase_offset == 0) 
			{
				if (run_length == 1)
				{
					encoder_phase = L_shr(L_mult(temp,one_pdelay),1);  // Result in Q20  
					                //fmod (160, prev_delay)/prev_delay;
					decoder_phase = L_shr(L_mult(temp2,one_pdelayD),1); // Result in Q20 
					                //fmod (160, pdelayD)/pdelayD;
				}
				else if (run_length == 2)
				{
					div_int_sp(add (temp,temp3),prev_delay,temp4);
					encoder_phase = L_shr(L_mult(temp4,one_pdelay),1);  // Result in Q20 
					    ///fmod (fmod (160, prev_delay) + fmod (160, prev_delay2), prev_delay)/prev_delay;
					div_int_sp(add (temp2,temp2),pdelayD_fx, temp4);
					decoder_phase = L_shr(L_mult(temp4,one_pdelayD),1); // Result in Q20 
					    //fmod (fmod (160, pdelayD) * 2, pdelayD)/pdelayD;
				}
			}
			else if (phase_offset == 1)
			{
				if (run_length == 1)
				{
					encoder_phase = 0;
					decoder_phase = L_shr(L_mult(temp2,one_pdelayD),1);
					                //fmod (160, pdelayD)/pdelayD;
				}
				else if (run_length == 2)
				{
					encoder_phase = L_shr(L_mult(temp,one_pdelay),1);  // Result in Q20 
					                        //fmod (160, prev_delay)/prev_delay;
					div_int_sp(add (temp2,temp2),pdelayD_fx, temp4);
					decoder_phase = L_shr(L_mult(temp4,one_pdelayD),1); // Result in Q20  
					               //fmod (fmod (160, pdelayD) * 2, pdelayD)/pdelayD;
				}
			}
			else if (phase_offset == 2)
			{
				encoder_phase = 0;
				div_int_sp(add (temp,temp),pdelayD_fx, temp4);
		        decoder_phase =  div_int_sp(temp4,pdelayD_fx,temp1);
								               //fmod (fmod (160, pdelayD) * 2, pdelayD)/pdelayD;
			}
			else if (phase_offset == -1)
			{
				
				div_int_sp(add (temp,temp3),prev_delay,temp4);
		        encoder_phase = L_shr(L_mult(temp4,one_pdelay),1);
				//encoder_phase = fmod (fmod (160, prev_delay) + fmod (160, prev_delay2), prev_delay)/prev_delay;
				
				div_int_sp(temp2,pdelayD_fx,temp1);
		        decoder_phase =L_shr(L_mult(temp1,one_pdelayD),1); // Result in Q20 ;
				
				//decoder_phase = fmod (160, pdelayD)/pdelayD;
			}

			//End Different cases using Phase Matching

			if (decoder_phase >= encoder_phase)
			{
				Word32 Lsum1,Lsum3;
				Word16 t_S1h;
				UNS_Word16 t_S1l;
				Word40 Lacc;
				Lsum3= L_sub(decoder_phase,encoder_phase);

		/* following code segment multiplies a 16 bit number with */
		/* a 32 bit number to give a 32 bit result                */
         
    			Lacc=0;
				t_S1h=extract_h(Lsum3);
				t_S1l=extract_l(Lsum3);  
				Lacc=L_shl40(L_mult_su(prev_delay,t_S1l),1); 
				Lsum1=(Word32)L_add40(L_shr40(Lacc,16),L_mult(t_S1h,prev_delay));
				pm_var1 = extract_l(L_shr(Lsum1,6));	
		   }
			else{

            Word32 Lsum1,Lsum3;
			Word16 t_S1h;
			UNS_Word16 t_S1l;
			Word40 Lacc;
			Lsum3= L_sub(encoder_phase,decoder_phase);

			/* following code segment multiplies a 16 bit number with */
			/* a 32 bit number to give a 32 bit result                */
         
    		Lacc=0;
			t_S1h=extract_h(Lsum3);
			t_S1l=extract_l(Lsum3);  
			Lacc=L_shl40(L_mult_su(prev_delay,t_S1l),1); 
			Lsum1=(Word32)L_add40(L_shr40(Lacc,16),L_mult(t_S1h,prev_delay));
			pm_var1 = sub(prev_delay,(extract_l(L_shr(Lsum1,6))));

			}
			
			num_residual = sub(160, pm_var1);

                        if (sub(pm_var1,100) >0) pm_var1=0;

			//printf ("\n delays %f %f %f %f %d phases %f %f", prev_delay_end, pdelayD, delay, prev_delay, pm_var1, encoder_phase, decoder_phase); //fflush(stdout);

			//If Phase Matching delta > subframesize, PM extends to two subframes
			if (pm_var1 > 53)
			{
				pm_var2 = pm_var1 - 53;
				pm_var1 = 53;
			}
			else
				pm_var2 = 0;
		}
		else
		{
			//If Phase Matching delta > subframesize, PM extends to two subframes

			pm_var1 = pm_var2;
			pm_var2 = 0;
		}

		for (j = 0; j < subframesize - pm_var1; j++){
              temp = round32_16(L_shl(L_mult(sum1_fx, Scratch_fx[j]), 4));    // Q0
              PitchMemoryD_fx[j+ACBMemSize]=add(PitchMemoryD_fx[j+pm_var1], temp);
		  }
		
			 
	}
	else     
	{           //no phase matching
		pm_var1 = 0;
 
		for (j = 0; j < subframesize; j++){
               temp = round32_16(L_shl(L_mult(sum1_fx, Scratch_fx[j]), 4));    // Q0
               PitchMemoryD_fx[j+ACBMemSize]=add(PitchMemoryD_fx[j+ACBMemSize], temp);
					//PitchMemoryD[j + ACBMemSize] += sum1 * Scratch[j];
               }
	}

     for (j=0; j<ACBMemSize; j++) {
        //Changed to support Phase Matching
		PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize-pm_var1];
		//End Changed to support Phase Matching
     }
#else
     for (j=0; j<subframesize; j++) {
      temp = round32_16(L_shl(L_mult(sum1_fx, Scratch_fx[j]), 4));    // Q0
   
      PitchMemoryD_fx[j+ACBMemSize]=add(PitchMemoryD_fx[j+ACBMemSize], temp);
     }

    
    /* update PM */
    for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
	  move16();
#endif
        PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
    }
#endif //end of voiced_dora_celp

  }
 else  //SPL_HCELP ==1
   {  
     sum_acb_fx = ppvq_fx[data_packet.ACBG_IDX[i]];          // Q14
     if (sum_acb_fx < 9011) //0.5499
      sum_acb_fx = curr_ave_acb_gain_fx;

     if ((i!=0) && (ct==3))
       sum_acb_fx=16384;

     if (ct!=3)
       {
	 Ltemp=L_shl(L_mult(sum_acb_fx,add(i,1)),14); // Ltemp=sum_acb_fx*(i+1), Q29
	 Ltemp=(Word32)Mpy_32_16(Ltemp,10923); // Ltemp in Q30, 10923=1/6 in Q16
	 ave_acb_gain_fx = add(ave_acb_gain_fx, round32_16(Ltemp));
       }
     else
       ave_acb_gain_fx = curr_ave_acb_gain_fx;

     acb_excitation_fx(PitchMemoryD_fx+ACBMemSize, delayi_fx, sum_acb_fx, subframesize);
    
     if (curr_ave_acb_gain_fx < 6554) //0.4 in Q14
       {
          /* Compute fixed codebook contribution */
	   for (j=0; j<subframesize; j++) PitchMemoryD_fx[ACBMemSize+j]=round32_16(L_shl(L_mult(PitchMemoryD_fx[ACBMemSize+j],FadeScale_fx),1));     // Q(14+1)
            
       }

       
     /* update PM */
     for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
	  move16();
#endif
        PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
     }
  
     
   if (curr_ave_acb_gain_fx < 6554) //0.4 in Q14
       {
	 FadeScale_fx=sub(FadeScale_fx,819);  //0.05 in Q14
    

#ifdef WMOPS_FX
	  test();
	  move16();
#endif
         if (FadeScale_fx<0) FadeScale_fx=0;
     
#ifdef WMOPS_FX
	  test();
#endif
         for (j=0; j<subframesize; j++)
	       {
		      rnd=ran_g_fx(Seed_fx,celp_extra_flag_fx,celp_extra_ran_fx);//Q13
		      prd=round32_16(L_shl(L_mult(round32_16(L_mult(rnd,3277)),prev_ave_fcb_gain_fx),2)); //0.1 in Q15, Q(13+15+1=29-16=13), Q(13+0+3-16=0);
		      PitchMemoryD_fx[ACBMemSize+j]=add(PitchMemoryD_fx[ACBMemSize+j],prd);
	       }
       }
   } //SPL_HCELP 
  
    /* Collecting the entire frame of excitation */
#ifdef VOIP_DORA_CELP  
	for (j=0; j<subframesize-pm_var1; j++) {
      PitchMemoryD_Frame_fx[temp_samples_count+j]=PitchMemoryD_fx[ACBMemSize+j];
	}
	temp_samples_count += subframesize - pm_var1;	//Added to support Phase Matching
#else
	for (j=0; j<subframesize; j++) {
#ifdef WMOPS_FX
	  move16();
#endif
      PitchMemoryD_Frame_fx[i*(SubFrameSize-1)+j]=PitchMemoryD_fx[ACBMemSize+j];
      
	}
#endif

#ifdef WMOPS_FX      
	  sprintf(tmpstr,"CELP_DECODER: Update memory SF-%d .........................",i);
      WMP_fwc_fx(tmpstr);
#endif
  }

 

  //Post WI synthesis for the purpose of smoothing 
#ifdef WMOPS_FX
      Nlogic16(5);
	  test();
#endif
  if (LAST_BAD_CW.lag!=0 && sub(abs_s(sub(delay_fx,pdelayD_fx)),8)<0 && local_lpc_flag == FALSE &&
      ppvq_fx[data_packet.ACBG_IDX[0]]>0 && ppvq_fx[data_packet.ACBG_IDX[1]]>0 &&
      ppvq_fx[data_packet.ACBG_IDX[2]]>0) {
    
    GetSinCosTab_fx(delay_fx, sin_tab, cos_tab);
    curr.to_fs_fx(PitchMemoryD_fx+sub(ACBMemSize,delay_fx), delay_fx, sin_tab, cos_tab);

    Ltemp=(Word32)L_sat32_40(divide_dp(curr.getEngy_fx(),LAST_BAD_CW.getEngy_fx(),
			       sub(shl(sub(LAST_BAD_CW.Q,curr.Q),1),2))); // Q27
    temp=round32_16(Ltemp); //Q11
    //Ensure there is no drastic energy change between successive CWs before interp
#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
    if (sub(temp,51)>0  && sub(temp,29696) < 0) { // 0.025 , 14.5 in Q11
      Interpol_fx(OldOldlspD_fx, OldlspD_fx, tmplsp, 2);
      lsp2lpc_fx(tmplsp, pci_fx, lpc1);
      Interpol_fx(OldlspD_fx, lsp_fx, tmplsp, 2);
      lsp2lpc_fx(tmplsp, pci_fx, lpc2);

#ifdef VOIP_DORA_CELP
    //printf ("\n post_WI_SYN");
	if (do_phase_matching)
	  {
	    // GetSinCosTab_fx(curr.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
            //poleFilter_setup_fx(lpc2, ORDER, curr.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
           //WIsyn_fx(LAST_BAD_CW,&curr,lpc1,lpc2,&ph_offset,outFbuf,num_residual, pf_temp1, pf_temp2, pf_temp, pf_n2);
	//	 for(j=0;j<ACBMemSize;j++) 
         //  		PitchMemoryD_fx[j]=outFbuf[num_residual-ACBMemSize+j];
         //		 for(j=0;j<num_residual;j++) 
         //				PitchMemoryD_Frame_fx[j]=outFbuf[j];
   
      }
     else{
         GetSinCosTab_fx(curr.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
         poleFilter_setup_fx(lpc2, ORDER, curr.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
         WIsyn_fx(LAST_BAD_CW,&curr,lpc1,lpc2,&ph_offset,outFbuf,FSIZE, pf_temp1, pf_temp2, pf_temp, pf_n2);
		 for(j=0;j<ACBMemSize;j++) 
           	   PitchMemoryD_fx[j]=outFbuf[FSIZE-ACBMemSize+j];
         for(j=0;j<FSIZE;j++) 
			   PitchMemoryD_Frame_fx[j]=outFbuf[j];
		}
			

#else      /// old case
      // use output buffer as working buffer for WIsyn_fx
#ifdef WMOPS_FX
	  move16();
#endif
      ph_offset=0;
    GetSinCosTab_fx(curr.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
    poleFilter_setup_fx(lpc2, ORDER, curr.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
#ifdef WMOPS_FX
    extern int WIcallcount;
    WIcallcount=1;
#endif //WMOPS_FX
   WIsyn_fx(LAST_BAD_CW,&curr,lpc1,lpc2,&ph_offset,outFbuf,FSIZE, pf_temp1, pf_temp2, pf_temp, pf_n2);


   
   for(j=0;j<ACBMemSize;j++) {
#ifdef WMOPS_FX
	  move16();
#endif
	   PitchMemoryD_fx[j]=outFbuf[FSIZE-ACBMemSize+j];
   }
   for(j=0;j<FSIZE;j++) {
#ifdef WMOPS_FX
	  move16();
#endif
	   PitchMemoryD_Frame_fx[j]=outFbuf[j];

            }
#endif    // end old case
       
	   }
  }
#ifdef VOIP_DORA_CELP
	num_samples = temp_samples_count;
        if (num_samples <=120){
           if (time_warp_fraction < 0x4000) 
              time_warp_fraction = 0x4000;
           
           else {
              time_warp_fraction = 0x6000; /* 1.5 in Q14 */
            }
       }
#endif

#ifdef WMOPS_FX      
      WMP_fwc_fx("CELP_DECODER: post WI synthesis ..........................");
#endif


#ifdef VOIP_DORA_CELP 
  Word16 one_mems, n1, nshft, t16, tmp2;
  Word32 tmp, tmp1, tmp3;
  int temp1;
  Word32 one_lmems ;

 if (time_warp_fraction > 0x4000 || time_warp_fraction < 0x4000) //Expansion or Compression
  {
   	  merge_start = 0;
			  
	  merge_end = delay_fx;

	  if (sub(merge_end,merge_start) > shr(num_residual,1))
		merge_stop = sub(num_residual,sub (merge_end, merge_start));
	  else
		merge_stop = sub(merge_end, merge_start);

				
	  if (num_residual <= delay_fx)
	  	merge_stop = 0;

   	  num_samples = num_residual;
			
	  for (b = 0; b < num_residual; b++)
	  	temp_pitch[b] = PitchMemoryD_Frame_fx[b];

	  sample_counter = 0;

            temp1 = sub (merge_stop,merge_start);
	        one_lmems=invert_lut(L_deposit_h(temp1),&n);
			one_mems=round32_16(one_lmems);       //1/merge_stop-merge_start in Qn//
			//printf("\n  n:%d, fx: %f, fl: %f", n,one_mems/pow(2.0,39-n), (1.0/merge_end-merge_start));
			

	        
	  if (merge_stop != 0)
	  {
	 	if (time_warp_fraction > 0x4000) //Expansion
		{
			for (b = merge_start; b < merge_end; b++)
				PitchMemoryD_Frame_fx[b] = temp_pitch[b];

			sample_counter =add(sample_counter, merge_end);

			for (b = merge_start; b < merge_stop; b++){
			     tmp =  L_mult(sub(sub(merge_stop,merge_start),b),temp_pitch[b + (merge_end-merge_start)]);//Q1
				 tmp1 = L_mult(b,temp_pitch[b]); //Q1 format
				 tmp3=L_add(tmp,tmp1); //Q1
				 nshft=norm_l(tmp3);
                 t16= extract_h(L_shl(tmp3,nshft)); // Q(nshft+1-16)
				 tmp2= extract_h(L_mult(t16,one_mems));// one_mems in Q(29-n) 
				 PitchMemoryD_Frame_fx[b+merge_end]= shr(tmp2,add(add(-1,-n),nshft)); //Q0, shr by -1-n+nshft 
				
				 //tmp2= temp_pitch[b + (merge_end-merge_start)] * (1.0 * (merge_stop-merge_start-b)/(merge_stop-merge_start)) 
								                                       + temp_pitch[b] * (1.0 * (b)/(merge_stop-merge_start));	
                 //printf("\n fx: %d, fl: %d",PitchMemoryD_Frame_fx[b+merge_end],tmp2);										
										}

			sample_counter = add(sample_counter,sub(merge_stop, merge_start));

			 //If erased packet is being reconstructed, need to produce two extra pitch periods
			// But only two produce two extra pitch periods if length will be less than 320
			if (phase_offset == -1 && (num_residual + 2 * (merge_end - merge_start)) <= 320)
			{
				if (merge_end - merge_stop > 0)
				{
					for (b = sample_counter, c = 0; b < merge_end+merge_end; b++, c++)
						PitchMemoryD_Frame_fx[b] = temp_pitch[merge_stop+c];

					sample_counter = add(sample_counter, sub(merge_end,merge_stop));
				}

				for (b = 0; b < merge_stop; b++){
                 tmp =  L_mult(sub(sub(merge_stop,merge_start),b),temp_pitch[b + (merge_end-merge_start)]);//Q1
				 tmp1 = L_mult(b,temp_pitch[b]); //Q1 format
				 tmp3=L_add(tmp,tmp1); //Q1
				 nshft=norm_l(tmp3);
                 t16= extract_h(L_shl(tmp3,nshft)); // Q(nshft+1-16)
				 tmp2= extract_h(L_mult(t16,one_mems));// one_mems in Q(29-n) 
				 PitchMemoryD_Frame_fx[b+sample_counter]= shr(tmp2,add(add(-1,-n),nshft)); //Q0, shr by -1-n+nshft 
				
				 //printf("\n fx: %d, fl: %d",PitchMemoryD_Frame_fx[b+merge_end],tmp2);	     
					//PitchMemoryD_Frame_fx[b+sample_counter] = temp_pitch[b + (merge_end-merge_start)] * (1.0 * (merge_stop-merge_start-b)/(merge_stop-merge_start)) 
					//+ temp_pitch[b] * (1.0 * (b)/(merge_stop-merge_start));	

					}
				sample_counter =add( sample_counter, merge_stop);


				for (b = merge_start + merge_end + merge_stop; b < num_residual + merge_end - merge_start; b++)
					PitchMemoryD_Frame_fx[b+merge_end] = temp_pitch[b - (merge_end - merge_start)];
							
				num_samples = add(num_residual,add(sub(merge_end,merge_start),sub(merge_end, merge_start)));
				
			}
			else //Else, regular expansion as requested by de-jitter: produce one extra pitch period
			{
				for (b = merge_start + merge_end + merge_stop; b < num_residual + merge_end - merge_start; b++)
					PitchMemoryD_Frame_fx[b] = temp_pitch[b - (merge_end - merge_start)];
							
				num_samples = add(num_residual,sub (merge_end, merge_start));
				
			}
		}
		//Regular compression as requested by de-jitter: produce one less pitch period
		//Compress only if compressed samples >= 40
		else if (time_warp_fraction < 0x4000 && (num_residual - (merge_end - merge_start)) >= 20)
		{
			for (b = merge_start; b < merge_stop; b++){
                 tmp =  L_mult(sub(sub(merge_stop,merge_start),b),PitchMemoryD_Frame_fx[b]);//Q1
				 tmp1 = L_mult(b,PitchMemoryD_Frame_fx[b+(merge_end-merge_start)]); //Q1 format
				 tmp3=L_add(tmp,tmp1); //Q1
				 nshft=norm_l(tmp3);
                 t16= extract_h(L_shl(tmp3,nshft)); // Q(nshft+1-16)
				 tmp2= extract_h(L_mult(t16,one_mems));// one_mems in Q(29-n) 
				 PitchMemoryD_Frame_fx[b]= shr(tmp2,add(add(-1,-n),nshft)); //Q0, shr by -1-n+nshft 
				
				//PitchMemoryD_Frame_fx[b] = PitchMemoryD_Frame_fx[b] * (1.0 * (merge_stop-merge_start-b)/(merge_stop-merge_start)) 
				//+ PitchMemoryD_Frame_fx[b+(merge_end-merge_start)] * (1.0 * (b)/(merge_stop-merge_start));	

				}

			for (b = merge_stop; b + merge_end - merge_start < num_residual; b++)
				PitchMemoryD_Frame_fx[b] = PitchMemoryD_Frame_fx[b + merge_end - merge_start];

			num_samples = sub(num_residual,sub(merge_end, merge_start));
			//printf("\n num_samples: %d num_residual:%d \n",num_samples,num_residual);
		}
	}
  }
  //End Warping-expanding or compressing

  //More support for Phase Matching/Warping

  //printf ("\n num_samples:%d decode_fcnt:%d", num_samples,decode_fcnt);

  temp_samples_count = 0;
  *obuf_len = num_samples;
  //End: More support for Phase Matching/Warping

#endif  
  
#ifdef VOIP_DORA_CELP 
  //SECOND PART OF THE SYNTHESIS
  temp_samples_count = 0;
  *obuf_len = num_samples;

   Synthesis_of_decsph_fx(OldlspD_fx, lsp_fx, pdelayD_fx, delay_fx, PitchMemoryD_Frame_fx, SynMemory_fx, outFbuf, 0, post_filter, num_samples,temp_samples_count);
#else
   //SECOND PART OF THE SYNTHESIS
   Synthesis_of_decsph_fx(OldlspD_fx, lsp_fx, pdelayD_fx, delay_fx, PitchMemoryD_Frame_fx, SynMemory_fx, outFbuf, 0, post_filter);
#endif

#ifdef WMOPS_FX      
      WMP_fwc_fx("CELP_DECODER: second part of the synthesis ...............");
#endif


  pdelayD_fx = delay_fx;
  
  return(0);
}

/*===================================================================*/
/* FUNCTION      :  Weight2Res_fx()                                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*    _ (Word16[])  input     : Input speech signal (Q0)             */
/*    _ (Word16[])  pci_fx    : Last frame quantized lsp (Q12)       */
/*    _ (Word16[])  wpci_gma1_fx  : Weight filter gamma1 (Q12)       */
/*    _ (Word16[])  wpci_gma2_fx  : Weight filter gamma1 (Q12)       */
/*    _ (Word16)    order     : filter order                         */
/*    _ (Word16)    length    : input signal length                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*    _ (Word16[]) output : output signal (Q0)                       */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/

void Weight2Res_fx(
     Word16 *input,
     Word16 *output,
     Word16 *pci_fx,
     Word16 *wpci_gma1_fx,
     Word16 *wpci_gma2_fx,
     short order,
     short length
)
{
  register short i;
  Word16   mem16[ORDER];
  Word32   mem32[ORDER];
  
  /* A(Z) */
  for (i=0; i<order; i++) {
#ifdef WMOPS_FX
	  move16();
#endif
	  mem16[i] = 0;
  }

  /* Cascade with of 1/A(Z/gamma1) */
  prediction_filter_fx(pci_fx, input, output, mem16, order, length, 4);
  
  for (i=0; i<order; i++) {
#ifdef WMOPS_FX
	  move32();
#endif
	  mem32[i] = 0;
  }

#if 1 
  for(i=0;i<length;i++)
    output[i]=shr(output[i],SCALE);
#endif  
  
  synthesis_filter_fx(wpci_gma1_fx, output, output, mem32, order, length, 3);

#if 1 
  for(i=0;i<length;i++)
    output[i]=shl(output[i],SCALE);
#endif
  
  
  /* Cascade with of A(Z/gamma2) */
  for (i=0; i<order; i++) {
#ifdef WMOPS_FX
	  move16();
#endif
	  mem16[i] = 0;
  }
  prediction_filter_fx(wpci_gma2_fx, output, output, mem16, order, length, 4);
}

/*===================================================================*/
/* FUNCTION      :  Synthesis_of_decsph_fx()                         */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Synthesis of decoder output signal and
                    post-filtering of output signal                  */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                                  */
/*    _ (Word16[]) LOldlspD_fx : Last frame quantized lsp (Q15)               */
/*    _ (Word16[]) Llsp_fx : Current frame quantized lsp (Q15)                */
/*    _ (Word16)   LpdelayD_fx    (Q0)                                        */
/*    _ (Word16)   Ldelay_fx : Current frame delay (Q0)                       */
/*    _ (Word16[]) LPitchMemoryD_frame_fx : Entire frame of excitation (Q0)   */
/*    _ (Word32[]) LSynMemory_fx : Synthesis filter memory (Q16)              */
/*    _ (Word16)   postfilter  : Postfilter on/off flag                       */
/*    _ (Word16)   erasure     : Erasure decoder on/off flag                  */  
/*----------------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                         */
/*    _ (Word16[]) LoutFbuf_fx : Decoded speech signal (one frame)(Q0)        */
/*--------------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                                   */
/*    _ (Word32[]) LSynMemory_fx : Synthesis filter memory (Q16)              */
/*----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :
/*    _ (Word16[])  pci_fx,lspi_fx : Interpolated prediction/lsp
                                     coefficients (Q12/Q15)                   */
/*--------------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                         */
/*     _ None                                                                 */
/*==========================================================================  */

#ifdef VOIP_DORA_CELP
void Synthesis_of_decsph_fx(Word16 LOldlspD_fx[], Word16 Llsp_fx[], Word16 LpdelayD_fx, Word16 Ldelay_fx,Word16 LPitchMemoryD_frame_fx[], Word32 LSynMemory_fx[],Word16 LoutFbuf_fx[],Word16 erasure,Word16 post_filter, Word16 num_samples, Word16 temp_samples_count)
#else
void Synthesis_of_decsph_fx(Word16 LOldlspD_fx[], Word16 Llsp_fx[], Word16 LpdelayD_fx, Word16 Ldelay_fx,Word16 LPitchMemoryD_frame_fx[], Word32 LSynMemory_fx[],Word16 LoutFbuf_fx[],Word16 erasure,Word16 post_filter)
#endif
{
  Word16 i,p,subframesize,j;
  Word32 delaypf_fx,Ldelayi_fx[3]; // Q16
  
 
#ifdef   VOIP_DORA_CELP
  Word16 scaled_res[SubFrameSize*2];
#else
  Word16 scaled_res[SubFrameSize];
#endif
  Word16 agc_fac; 
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if (erasure)
    {
       lsp2lpc_fx(LOldlspD_fx, prev_pci_fx, pci_fx);
       for(i=0;i<10;i++)
	 lspi_fx[i]=LOldlspD_fx[i];
       delaypf_fx=L_deposit_h(LpdelayD_fx); //Q16
       agc_fac=ave_acb_gain_fx;
    }
  else
    agc_fac=0x3333; //0.4 in Q15
  p=0;

#ifdef   VOIP_DORA_CELP
   Word16 temp, temp1,temp2;
#endif
  
  
  for(i=0;i<NoOfSubFrames;i++)
    {
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
#ifndef VOIP_DORA_CELP

     if (i < 2) subframesize = SubFrameSize - 1;
      else subframesize = SubFrameSize;
#else
     //Warping: Change in size of subframesize, not always equal to 53 since residual size may not be = 160
       
	    temp1 = div_int_sp(num_samples, 3,  temp);
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
#endif
      
#ifdef WMOPS_FX
	  test();
#endif
      if (!erasure)
	{
          Interpol_fx(LOldlspD_fx, Llsp_fx, lspi_fx, i);
	       lsp2lpc_fx(lspi_fx, prev_pci_fx, pci_fx);
          Interpol_delay_fx(LpdelayD_fx,Ldelay_fx, Ldelayi_fx, i);
	      delaypf_fx = L_shr(L_add(Ldelayi_fx[0],Ldelayi_fx[1]),1); // Q16
	}
      
     
/* Synthesis of decoder output signal and postfilter output signal */
#ifdef VOIP_DORA_CELP

       for(j=0;j<subframesize;j++)
        scaled_res[j]=shr(LPitchMemoryD_frame_fx[j+temp_samples_count],SCALE);  
        synthesis_filter_fx(pci_fx , scaled_res, DECspeech_fx ,LSynMemory_fx,ORDER,subframesize,3);
#else   
      
       for(j=0;j<subframesize;j++)
        scaled_res[j]=shr(LPitchMemoryD_frame_fx[j+i*(SubFrameSize-1)],SCALE);  

      synthesis_filter_fx(pci_fx , scaled_res, DECspeech_fx ,LSynMemory_fx,ORDER,subframesize,3);
#endif
      
    /* Postfilter */
#ifdef WMOPS_FX
	  test();
#endif
    if (post_filter) {
            
      Post_Filter_fx(DECspeech_fx, lspi_fx, pci_fx, DECspeechPF_fx,
		     delaypf_fx,agc_fac, subframesize); 
      

    }
    
    else
     {
       for(j=0;j<subframesize;j++) {
	 DECspeechPF_fx[j]=DECspeech_fx[j];
       }
     }
    
    
    for(j=0;j<subframesize;j++)
      DECspeechPF_fx[j]=shl(DECspeechPF_fx[j],SCALE);
    
    
    /* Write p.f. decoder output and variables to files */
    for (j = 0; j < subframesize; j++) {
      LoutFbuf_fx[p] = DECspeechPF_fx[j];
      p++;
    }
#ifdef VOIP_DORA_CELP      
    temp_samples_count =add(temp_samples_count, subframesize); //Phase Matching: counting number of samples syntthesized
#endif
    
    
    
    
    }
  
#if 1
  for(j=0;j<ORDER;j++)
    prev_pci_fx[j]=pci_fx[j];
#endif

  
  
	
   

}


/*===================================================================*/
/* FUNCTION      :  celp_erasure_decoder_fx()                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  CELP erasure decoding for the current frame      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*    _ (Word16[]) OldlspD_fx  : Last frame quantized lsp (Q15)      */
/*    _ (Word16)   postfilter  : Postfilter on/off flag              */
/*    _ (Word16)   ave_acb_gain_fx : ACB gain (Q14)                  */
/*    _ (Word16)   ave_fcb_gain_fx : FCB gain (Q0)                   */
/*    all variables above except post_filter are global              */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*    _ (Word16[]) outFbuf_fx : Decoded speech signal (one frame)(Q0)*/
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*    _ (Word16)    FadeScale_fx  : Q14                              */
/*    _ (Word16)    delay_fx,pdelayD_fx     : Q0                     */
/*    _ (Word16[])  pci_fx,lspi_fx : Interpolated prediction/lsp
                                coefficients (Q12/Q15)               */
/*    _ (Word32[])  SynMemory_fx : Synthesis filter memory (Q16)     */

/*    _ (Word16[])  PitchMemoryD_fx:  (Q0)                           */ 
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
#ifdef VOIP_DORA_CELP

  void celp_erasure_decoder_fx(Word16 outFbuf_fx[],Word16 post_filter, Word16 phase_offset, Word16 run_length, Word16 time_warp_fraction, short*     obuf_len )
#else

 void celp_erasure_decoder_fx(Word16 outFbuf_fx[],Word16 post_filter)
#endif
 
{
  
 
  Word16  i, j,p;
  
  short   subframesize;
  
  Word16 sum_acb_fx;
  Word16 prd,rnd;

#ifdef  VOIP_DORA_CELP
  Word16 PitchMemoryD_Frame_fx[FSIZE*2];
  Word16 num_samples = 160;
  Word16 temp_samples_count = 0;
  *obuf_len = num_samples;
#else
  Word16 PitchMemoryD_Frame_fx[FSIZE];
#endif


   
  Word32 delayi_fx[3];  // Q16
    
  static Word16 Seed_fx = 0, celp_extra_flag_fx=0, celp_extra_ran_fx ;    
 
  
  delay_fx=pdelayD_fx;
  
  
  delayi_fx[0]=delayi_fx[1]=delayi_fx[2]=L_deposit_h(pdelayD_fx);// for acb
  
  /* Convert lsp to PC */

   
#ifdef WMOPS_FX
	  move16();
#endif
  p=0;
  for (i=0; i<NoOfSubFrames; i++)
    {
    
#ifdef WMOPS_FX
	  test();
#endif
    if (i < 2) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;

    
#ifdef WMOPS_FX
	  move16();
#endif
	    
     
#ifdef WMOPS_FX
	  test();
#endif
	  if (ave_acb_gain_fx >13107) sum_acb_fx = 16384; else sum_acb_fx = ave_acb_gain_fx; 	  
    if (i==0) sum_acb_fx=ave_acb_gain_fx; // must be Q14
      

    acb_excitation_fx(PitchMemoryD_fx+ACBMemSize, delayi_fx, sum_acb_fx, subframesize);
    
 
    /* Compute fixed codebook contribution */
    for (j=0; j<subframesize; j++) PitchMemoryD_fx[ACBMemSize+j]=round32_16(L_shl(L_mult(PitchMemoryD_fx[ACBMemSize+j],FadeScale_fx),1));     // Q(14+1)
    
  
	for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
	  move16();
#endif
		PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];  
	}

   
    FadeScale_fx=sub(FadeScale_fx,819);  //0.05 in Q14
    

#ifdef WMOPS_FX
	  test();
	  move16();
#endif
     if (FadeScale_fx<0) FadeScale_fx=0;
     
#ifdef WMOPS_FX
	  test();
#endif
     if (ave_acb_gain_fx < 6554)  // 0.4 in Q14
      {                      
	
      for (j=0; j<subframesize; j++)
	{
	  rnd=ran_g_fx(Seed_fx,celp_extra_flag_fx,celp_extra_ran_fx);//Q13
	  prd=round32_16(L_shl(L_mult(round32_16(L_mult(rnd,3277)),ave_fcb_gain_fx),2)); //0.1 in Q15, Q(13+15+1=29-16=13), Q(13+0+3-16=0);
	  PitchMemoryD_fx[ACBMemSize+j]=add(PitchMemoryD_fx[ACBMemSize+j],prd);

	}
      }
                 
     for(j=0;j<subframesize;j++,p++) {
#ifdef WMOPS_FX
	  move16();
#endif
       PitchMemoryD_Frame_fx[p]= PitchMemoryD_fx[j+ACBMemSize];

        }
    }

#ifdef  VOIP_DORA_CELP
   Synthesis_of_decsph_fx(OldlspD_fx, OldlspD_fx, pdelayD_fx,pdelayD_fx, PitchMemoryD_Frame_fx, SynMemory_fx,outFbuf_fx,1,post_filter,num_samples,temp_samples_count);
#else
   Synthesis_of_decsph_fx(OldlspD_fx, OldlspD_fx, pdelayD_fx,pdelayD_fx, PitchMemoryD_Frame_fx, SynMemory_fx,outFbuf_fx,1,post_filter);
#endif

  pdelayD_fx = delay_fx;
  
}
