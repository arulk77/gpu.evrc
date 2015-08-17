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
/*  voiced encoder/decoder main functions                               */
/*======================================================================*/

#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "globs_fx.h"
#include "proto_fx.h"
#include "tbl_lsp_fx7.h"
#include "cos_tab.dat"
#include "lpcana_tab.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


short nsub[2]={6,4}; 
short nsize[2]={512,32};

// sin( ), cos( ) RAM table
//extern Word16 C_fx[241], S_fx[241];

//extern EvrcArgs *Eargs;

//Word16 ph_offset_E_fx = 0; //Q15

// PPP Specific Stuff
// DTFS_fx currp_nq_fx, prev_cw_E_fx ;

void jumptofullcelp_fx(Word16& rate)
{
  bit_rate=rate=PACKET_RATE=4;
  MODE_BIT=0;
  
#ifdef DYNAMIC_COMPLEXITY_REDUC
  // dynamic complexity reduction
  // Set values for reduced search space
  // Global Variables to be used cod7_35_fx
  fcb_N0 = 2;	/* # of candidate positions for pulse 1 */
  fcb_N1 = 4;	/* # of candidate positions for pulse 2 */	
  fcb_N2 = 6;	/* # of candidate positions for pulse 3 */
  fcb_N3 = 12;

  fcb_N_PRE = 6; //10	/* # of candidate combinations for pulse 1, 2 and 3 */

  //parameters used by 28-bit LSP VQ 
  PRE_LSP_NUM0 = 2;
  PRE_LSP_NUM2 = 3;
#endif//DYNAMIC_COMPLEXITY_REDUC

  celp_encoder_fx(bit_rate);
}


void lsp_spread(Word16 *lsp_fx)
{
  Word16 i;
  if (lsp_fx[0]<LSP_SPREAD_FACTOR_FX) lsp_fx[0]=LSP_SPREAD_FACTOR_FX;
      for (i=1; i<LPCORDER; i++)
        if (sub(lsp_fx[i],lsp_fx[sub(i,1)]) < LSP_SPREAD_FACTOR_FX)
          lsp_fx[i]=add(lsp_fx[sub(i,1)],LSP_SPREAD_FACTOR_FX);
      if (sub(16384,lsp_fx[sub(LPCORDER,1)])< shl(LSP_SPREAD_FACTOR_FX,1)) { 
        lsp_fx[sub(LPCORDER,1)]=sub(16384,shl(LSP_SPREAD_FACTOR_FX,1));
        i=sub(LPCORDER,2);
        while ((sub(lsp_fx[i+1],lsp_fx[i])<LSP_SPREAD_FACTOR_FX) && (i>=0)) {
	      lsp_fx[i]=MIN_FX(sub(lsp_fx[i+1],LSP_SPREAD_FACTOR_FX),0);
          i--;
        }
      }

}
/*===================================================================*/
/* FUNCTION      :  voiced_encoder_fx()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  PPP encoding for the current frame               */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16[])  HPspeech_fx : Input speech signal (Q0)           */
/*    _ (Word16[])  OldlspE_fx  : Last frame quantized lsp (Q15)     */
/*    _ (Word16[])  Oldlsp_nq_fx  : Last frame un-quantized lsp(Q15) */
/*    _ (Word16[])  OldOldlsp_nq_fx  : LastLast frame un-quantized
                                                            lsp(Q15) */
/*    _ (Word16[])  lsp_nq_fx  : Current frame un-quantized lsp (Q15)*/
/*    _ (Word32[])  SynMemoryM_fx : (Q16)                            */
/*    _ (Word16[])  WFmemFIR_fx  : FIR memory (Q0)                   */
/*    _ (Word32[])  WFmemIIR_fx  : IIR memory (Q16)                  */
/*    _ (Word16[])  residualm_fx  : modified residual (Q0)           */
/*    _ (Word16)    beta_fx: periodicity parameter, Q15              */
/*    _ (Word16)    prev_en_fx (Q0)                                  */
/*    _ (Word16)    NS_SNR_fx  (Q9)                                  */
/*    _ (Word16)    prev_dim_and_burstE_fx (Q0)                      */
/*-------------------------------------------------------------------*/
/* GLOBAL OUTPUT ARGUMENTS :                                         */
/*    _ (Word16[]) lsp_fx  :  Current frame quantized lsp output(Q15)*/
/*    _ (Word16[]) out_fx : one frame of quantized residual output (Q0)*/
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT/OUTPUT ARGUMENTS :                                   */
/*    _ (Word32[])  SynMemoryM_fx,  WFmemIIR_fx : Synthesis
                    filter memory (Q16)                              */
/*    _ (Word16[])  WFmemFIR_fx : Prediction filter memory (Q0)      */
/*    _ (Word16[])  residualm  : modified residual (Q0)              */
/*    _ (Word16[])  Excitation_fx : Excitation output signal (Q0)    */
/*    _ (Word16)    delay_fx  :  Q0                                  */
/*    _ (Word16)    pdelayD_fx  : Q0                                 */
/*    _ (Word16)    accshift_fx: accumulated shift, Q8               */
/*    _ (Word16)    PPP_MODE_fx : PPP MODE (Q0)                      */
/*    _ (Word16&)    rate : bit_rate (Q0)                            */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/

void voiced_encoder_fx(Word16 *out_fx, Word16& rate)

{
  register int i, j;
  short   subframesize;
  Word16 delta_lag_E=0;

  static Word16 LASTLAST_PPP_MODE_E_fx=0,LAST_PPP_MODE_E_fx=0; //fx
  
  
  Word16 NEWBUMPUP_fx=FALSE; //fx

#ifdef WMOPS_FX
      WMP_fwc_fx("Start Voiced Encoder .....................................");
#endif
   //Set NEWBUMPUP to TRUE wherever you want the new bumpup scheme to kick in
  if      (Eargs->operating_point==0) NEWBUMPUP_fx = FALSE;
  else if (Eargs->operating_point==1) NEWBUMPUP_fx = FALSE;
  else if (Eargs->operating_point==2) NEWBUMPUP_fx = TRUE; //fx

  //Reset the pattern  when the last frame is non-PPP or full-rate PPP
  if (LAST_PACKET_RATE_E==1 || LAST_PACKET_RATE_E==3 || LAST_MODE_BIT_E==0) {
    LASTLAST_PPP_MODE_E_fx = 0;
    LAST_PPP_MODE_E_fx = 0;
    //GetSinCosTab_fx(pl, S_fx, C_fx); //BASED ON PREVIOUS LAG 
    //prev_cw_E_fx.to_fs_fx(prev_en_fx+ACBMemSize-pl, pl, S_fx, C_fx);
   
  } 
  
    // Figure out  the PPP_MODE
  if (Eargs->rfileP==NULL) {// Do only if external rate is not provided
    if (Eargs->operating_point==1) {
      if (lastrateE!=4) {
         bit_rate=rate=PACKET_RATE=4;
          MODE_BIT=0;
	celp_encoder_fx(rate);return;}
      else PPP_MODE_E_fx=0;
    }
    else if (Eargs->operating_point==2) {
	//QQF Deterministic Pattern
      if ((LASTLAST_PPP_MODE_E_fx==0)&&(LAST_PPP_MODE_E_fx==0)) PPP_MODE_E_fx=1;
      else PPP_MODE_E_fx = 0;
    }
  }
 
   // Put rate into data_packet strucure
  if (PPP_MODE_E_fx==0 ) data_packet.PACKET_RATE=2;
  else if (PPP_MODE_E_fx==1 ) data_packet.PACKET_RATE=4;
  
  // Put mode bit into data_packet strucure
  data_packet.MODE_BIT=1;	// Mode bit for PPP in full and 1/4-rates
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Get Initial PPP Mode .....................");
#endif
  // LSP Quantization
  if (PPP_MODE_E_fx==0)  {
    // NEW 13 bit LSP VQ
   Word16 temp[2];
   Word16 w_fx[ORDER], tmplsi_fx[ORDER];
   weight_lsp_fx(lsp_nq_fx,w_fx);

   for (i=0;i<LPCORDER;i++)
     tmplsi_fx[i]=sub(lsp_nq_fx[i],round32_16(L_add(L_mult(13107,cbprev_E_fx[i]),L_mult(6554,cbprevprev_E_fx[i]))));  //Q(15+15+1-16=15)
   
   lspmaq_fx1(tmplsi_fx,nsub,nsize,w_fx,lsp_fx,temp,tbl_lsp_fx7); // new pac
    
  data_packet.LSP_IDX[0]=(unsigned short) temp[0];
  data_packet.LSP_IDX[1]=(unsigned short) temp[1];
  for (i=0;i<LPCORDER;i++) tmplsi_fx[i]=lsp_fx[i];
  for (i=0;i<LPCORDER;i++) {
      lsp_fx[i]=add(round32_16(L_add(L_mult(6554,cbprevprev_E_fx[i]),L_mult(13107,cbprev_E_fx[i]))),lsp_fx[i]);
      cbprevprev_E_fx[i]=cbprev_E_fx[i];
      cbprev_E_fx[i]=round32_16(L_shl(L_mult(20480,tmplsi_fx[i]),2));// Q(13+15+1+2-16=15)need this to make it MALSPVQ  
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: QPPP LSPVQ ...............................");
#endif
       
#ifdef WMOPS_FX
  Ntest(4);
  move16();
#endif
  if (lsp_fx[0]<LSP_SPREAD_FACTOR_FX_QPPP) lsp_fx[0]=LSP_SPREAD_FACTOR_FX_QPPP;
      for (i=1; i<LPCORDER; i++)
        if (sub(lsp_fx[i],lsp_fx[sub(i,1)]) < LSP_SPREAD_FACTOR_FX_QPPP)
          lsp_fx[i]=add(lsp_fx[sub(i,1)],LSP_SPREAD_FACTOR_FX_QPPP);
      if (sub(16384,lsp_fx[sub(LPCORDER,1)])< shl(LSP_SPREAD_FACTOR_FX_QPPP,1)) { 
        lsp_fx[sub(LPCORDER,1)]=sub(16384,shl(LSP_SPREAD_FACTOR_FX_QPPP,1));
        i=sub(LPCORDER,2);
        while ((sub(lsp_fx[i+1],lsp_fx[i])<LSP_SPREAD_FACTOR_FX_QPPP) && (i>=0)) {
          lsp_fx[i]=MIN_FX(sub(lsp_fx[i+1],LSP_SPREAD_FACTOR_FX_QPPP),0);
          i--;
        }
      }
          
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: QPPP LSPVQ STABILIZE .....................");
#endif
  }


  else if (PPP_MODE_E_fx==1) {
    enc_lsp_vq_28_fx(lsp_nq_fx, SScratch_fx, lsp_fx);
    for (i=0; i<4; i++)
      data_packet.LSP_IDX[i]=SScratch_fx[i];

  for (i=0;i<ORDER;i++) cbprevprev_E_fx[i]=cbprev_E_fx[i]=lsp_fx[i];    

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP LSPVQ ...............................");
#endif
  
#ifdef LSP_SPRD
 lsp_spread(lsp_fx);
#endif
      
     
  }
  else {
    fprintf(stderr,"Bad PPP_MODE '%c' in voiced.cc\n",PPP_MODE_E_fx);
  }
  
#ifdef WMOPS_FX
      WMP_fwc_fx("Get Residual .............................................");
#endif
  // Start Voiced/PPP  Processing
  Word16 mres_fx[FrameSize];

  for (i=0; i<NoOfSubFrames; i++) {   
    if (i < 2) subframesize = sub(SubFrameSize,1);
    else subframesize = SubFrameSize;
    /* interpolate lsp */
    Interpol_fx(OldlspE_fx, lsp_fx, lspi_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, pci_fx, pci_fx);
    /* Generating quantized residual */
    prediction_filter_fx(pci_fx,mspeech_fx+i*(SubFrameSize-1),mres_fx+i*(SubFrameSize-1),FIRmem_fx,ORDER, subframesize, 4);
  }

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: RCELP ....................................");
#endif
 
  Word16 pl=pdelay_fx,l=delay_fx;
  Word16 *in_fx=mres_fx;
 
  static Word32 prev_cw_en_fx; 
  static Word16 Q_prev_cw_en_fx;
  Word16 tmp_fx,tmp;
  Word32 Ltemp, logLag, Ltemp_q; //for setEngyHarm_fx computations
  Word16 Ql, Qh,n;
  Word16 res_enratio_fx=0, sp_enratio_fx=0;
  Word16 tmplsp_fx[ORDER], lpc1_fx[ORDER], lpc2_fx[ORDER];  // Fixed-pt for WIl
  
    
  DTFS_fx TMP3_fx,TMP_fx, TMP2_fx,currp_q_E_fx;                // Fixed-pt for WI
  
  Word16 Qtmpres,Qtmpsp;
  Word32 tmpres_fx,tmpsp_fx;
  Word40 Lacc,Lacc1;
  Word16 p_idx, a_idx1[2], a_idx2[3], rot_idx, band_idx[17]; //decls for qppp and fppp indices

    Word16 pf_temp1[MAXLAG_WI]; //maynot need more than MAXLAG_WI/2+1
    Word16 pf_temp2[MAXLAG_WI];
    Word16 pf_temp[MAXLAG_WI];
    Word16 pf_n2[MAXLAG_WI];
    Word16 sft,flag1;
  if (LAST_PACKET_RATE_E==1 || LAST_PACKET_RATE_E==3 || LAST_MODE_BIT_E==0) {
    GetSinCosTab_fx(pl, S_fx, C_fx); //BASED ON PREVIOUS LAG 
    prev_cw_E_fx.to_fs_fx(Excitation_fx+ACBMemSize-pl, pl, S_fx, C_fx);

 
  } 
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Get prev DFS for prev frame non-PPP ......");
#endif
  Interpol_fx(OldOldlspE_fx, OldlspE_fx, tmplsp_fx, 2);
 
  lsp2lpc_fx(tmplsp_fx, pci_fx, lpc1_fx); //Q12
  
  
  Interpol_fx(OldlspE_fx, lsp_fx, tmplsp_fx, 2);
 
  lsp2lpc_fx(tmplsp_fx, pci_fx, lpc2_fx); //Q12

  
  if (lastrateE !=3 && lastrateE !=4) pl=l;
  
 
   tmp=round32_16(L_shl(L_mult(l,30310),1)); // 30310=1.85, Q14
   if (sub(tmp,pl)<0) pl=shr(pl,1);
   tmp=round32_16(L_mult(l,17695)); // 17695=0.54, Q15
   if (sub(MAXLAG,shl(pl,1))>=0 && sub(tmp,pl)>=0) pl=shl(pl,1);
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Interpol LSP and adjust lag ..............");
#endif
  
  //Use the out array as a temp storage for currp
  ppp_extract_pitch_period_fx(in_fx, out_fx, l) ;
  
 
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Extract curr PPP .........................");
#endif
  GetSinCosTab_fx(l, S_fx, C_fx); //TABLE BASED ON CL
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Get Curr CosSin Table ....................");
#endif
  currp_nq_fx.to_fs_fx(out_fx, l, S_fx, C_fx);
    

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: DFS of extracted curr PPP ................");
#endif
  tmp_fx = alignment_extract_td_fx(out_fx, in_fx+FSIZE-l, l) ; //Q0
  tmp_fx = shl(tmp_fx,2); //Q2

  
//  tmp_fx=shl(tmp_fx,5); //Q2->Q7

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: curr PPP align extract ...................");
#endif

  currp_nq_fx.Q2phaseShift_fx(tmp_fx, l, S_fx, C_fx) ; //Q2 tmp_fx
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: phase-shift after curr PPP align extract .");
#endif
  //Need to make sure the "in" array has the past 160 samples as well as
  //the current 160 samples available before using the getSCR function!
  //scr = getSCR(in, (pl+l)>>1) ;
  //scr=0.0;  /***TURN off SCR feature for the time being***/
  
  //Restoring PPP memories when the last frame is non-PPP or full-rate PPP
#ifdef WMOPS_FX
  test();
  Nlogic16(2);
  Nmove16(2);
#endif
  if (LAST_PACKET_RATE_E==1 || LAST_PACKET_RATE_E==3 || LAST_MODE_BIT_E==0) {
    
    ph_offset_E_fx = 0 ; 
    
    Lacc=prev_cw_E_fx.getEngy_fx();//2Q+1
    Q_prev_cw_en_fx=norm32_l40(Lacc);
    Q_prev_cw_en_fx = (Lacc==0)?31: Q_prev_cw_en_fx;
    prev_cw_en_fx=(Word32) L_shl40(Lacc,Q_prev_cw_en_fx); //2Q+1+Q_prev_cw_en_fx
    Q_prev_cw_en_fx=add(Q_prev_cw_en_fx,add(shl(prev_cw_E_fx.Q,1),1));
       
    TMP_fx=prev_cw_E_fx;
    
    TMP_fx.car2pol_fx();
        
    logLag=log10_lut(TMP_fx.lag); // logLag=10*log10(pl), Q23
    Ltemp_q=L_shl(L_mult(shl(TMP_fx.Q,1),24660),9); // Ltemp_q=2Q*10log10(2), Q23
    
  /* Process low band */
    Ltemp=TMP_fx.setEngyHarm_fx(377, 4524, 0, 4524, 1, 0, &Ql);// Ltemp in 2*TMP.Q
    /* Compensate for Q factor of energy to get log10(lag*eng)  */
    Ltemp=log10_lut(Ltemp); // Ltemp=10log10(eng), Q23
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag);  // Ltemp=10*log10(lag*eng), Q23
    
    lastLgainE_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11, 0x6666 = 0.1 in Q18
    

    /* Process high band */
    Ltemp=TMP_fx.setEngyHarm_fx(4524, 13517, 4524, 16384, 1, 0, &Qh);
    Ltemp=log10_lut(Ltemp);
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag); // Ltemp=10*log10(lag*eng), Q23
    lastHgainE_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11
   
    /* Need to unify the Q factors of both bands */
    TMP_fx.Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
    n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
    if (n<0) TMP_fx.rshiftHarmBand_fx(4524, 16384,n);
    else if (n>0) TMP_fx.rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
    
    TMP_fx.to_erb_fx(lasterbE_fx); // Q13 output
    //TMP.to_erb(lasterbE);

    LASTLAST_PPP_MODE_E_fx = 0;
    LAST_PPP_MODE_E_fx = 1;
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Restore mem if last frame non-PPP ........");
#endif
  }
  else if (LAST_PACKET_RATE_E==4 && LAST_MODE_BIT_E==1) {
    TMP_fx = prev_cw_E_fx ;
    TMP_fx.car2pol_fx();
    
    logLag=log10_lut(TMP_fx.lag); // logLag=10*log10(pl), Q23
    Ltemp_q=L_shl(L_mult(shl(TMP_fx.Q,1),24660),9); // Ltemp_q=2Q*10log10(2), Q23
    
  /* Process low band */
    Ltemp=TMP_fx.setEngyHarm_fx(377, 4524, 0, 4524, 1, 0, &Ql);// Ltemp in 2*TMP.Q
    /* Compensate for Q factor of energy to get log10(lag*eng)  */
    Ltemp=log10_lut(Ltemp); // Ltemp=10log10(eng), Q23
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag);  // Ltemp=10*log10(lag*eng), Q23
    
    lastLgainE_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11, 0x6666 = 0.1 in Q18
    

   /* Process high band */
    Ltemp=TMP_fx.setEngyHarm_fx(4524, 13517, 4524, 16384, 1, 0, &Qh);
    Ltemp=log10_lut(Ltemp);
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag); // Ltemp=10*log10(lag*eng), Q23
    lastHgainE_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11
   
    /* Need to unify the Q factors of both bands */
    TMP_fx.Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
    n=sub(Ql,Qh); // compare band Q factors
    if (n<0) TMP_fx.rshiftHarmBand_fx(4524, 16384,n);
    else if (n>0) TMP_fx.rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
    
    TMP_fx.to_erb_fx(lasterbE_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Restore mem if last frame FPPP ...........");
#endif
  }

  //Compute prototype-prediction-SNR
  //getpredSNR(curr_nq,prev_cw_E);
  //DECIDE IF prototype-prediction-SNR-based BUMPING UP IS NECESSARY
  //Need to compute this SNR if desired, else it is set to 0.0 dB now.
   
  if (Eargs->rfileP==NULL) {
    //-----Open-loop Bump-Up
    
    //Energy ratio calculation in residual and speech domain
    //Also, compute correlation between the previous and the
    //current prototype.
    //res_enratio_fx=currp_nq_fx.getEngy_fx()

    
    Lacc=currp_nq_fx.getEngy_fx();
    Lacc1=prev_cw_E_fx.getEngy_fx();
    sft=add(shl(sub(currp_nq_fx.Q,prev_cw_E_fx.Q),1),4);
    
    if (sft>0)
      {
        if (L_sub40_40(Lacc1,L_shr40(Lacc,sft))<0)
	  res_enratio_fx=0x7FFF;
      }
    else
      {
        if (L_sub40_40(L_shr40(Lacc1,negate(sft)),Lacc)<0)
	  res_enratio_fx=0x7FFF;
      }
    /* max value res_enratio compared against is 0x7400 (14.5 in Q11) */
    if (res_enratio_fx!=0x7FFF)
      {
	Lacc=divide_dp(Lacc,Lacc1,sub(negate(shl(sub(currp_nq_fx.Q,prev_cw_E_fx.Q),1)),2));//Q(27)
	
	res_enratio_fx=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(27-16)=11
      }


    
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: Res_en_ratio OLP .........................");
#endif
    TMP_fx=currp_nq_fx, TMP2_fx=prev_cw_E_fx;
         
    //GetSinCosTab_fx(TMP_fx.lag, S_fx, C_fx); //table based on CL

    
    tmp_fx = TMP2_fx.alignment_full_fx(TMP_fx, ph_offset_E_fx, S_fx, C_fx);   //Q1,align of prev_cw wrt curr_cw, new method
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: alignment_full .........................");
#endif
    
    tmp_fx=shl(TMP_fx.lag,1)-tmp_fx;// (C_l-tmptmp) , Q1
    
    TMP_fx.Q2phaseShift_fx(-shl(tmp_fx,1), TMP_fx.lag, S_fx, C_fx) ; //Q1->Q7, modify in future
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: phase shift after alignment_full .........");
#endif
	
    tmpres_fx=TMP_fx.freq_corr_fx(TMP2_fx, 100, 3700, &Qtmpres);//tmpres_fx in Q(Qtmpres)
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: res freq corr OLP ........................");
#endif
        
    poleFilter_setup_fx(lpc2_fx, ORDER, TMP_fx.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);

    TMP_fx.poleFilter_fx(lpc2_fx,ORDER, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
   
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: poleFilter of TMP OLP ....................");
#endif

    TMP2_fx.adjustLag_fx(TMP_fx.lag); // operate in CL domain
     
    //GetSinCosTab_fx(TMP2_fx.lag, S_fx, C_fx);// prevlag based table no longer needed 
    TMP2_fx.poleFilter_fx(lpc1_fx,ORDER, S_fx, C_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: poleFilter of TMP2 OLP ...................");
#endif

    tmpsp_fx=TMP_fx.freq_corr_fx(TMP2_fx, 100, 3700, &Qtmpsp); //tmpsp_fx in Q(Qtmpsp)
    
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: speech freq corr OLP .....................");
#endif



# if 0
      
    Lacc=divide_dp(TMP_fx.getEngy_fx(),TMP2_fx.getEngy_fx(),add(negate(shl(sub(TMP_fx.Q,TMP2_fx.Q),1)),1));//Q(30)
    
    sp_enratio_fx=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q14
#endif

# if 1
    Lacc=TMP_fx.getEngy_fx();
    Lacc1=TMP2_fx.getEngy_fx();
    sft=add(shl(sub(TMP_fx.Q,TMP2_fx.Q),1),1);// to check if Lacc>2*Lacc1
    
    if (sft>0)
      {
        if (L_sub40_40(Lacc1,L_shr40(Lacc,sft))<0)
	  sp_enratio_fx=0x7FFF;
      }
    else
      {
        if (L_sub40_40(L_shr40(Lacc1,negate(sft)),Lacc)<0)
	  sp_enratio_fx=0x7FFF;
      }
    /* max value res_enratio compared against is 0x7852 ( 0.94 in Q15) */
    if (sp_enratio_fx!=0x7FFF)
      {
	Lacc=divide_dp(Lacc,Lacc1,add(negate(shl(sub(TMP_fx.Q,TMP2_fx.Q),1)),1));//Q(30)
	sp_enratio_fx=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q14
      }

#endif    


    
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: SP enratio OLP ...........................");
#endif
       
						   
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (PPP_MODE_E_fx == 0) {	
      //Bump up if the lag is out of range
#ifdef WMOPS_FX
  test();
  move16();
  logic16();
#endif
      if ((sub(sub(l,pl),8)>0) || (add(sub(l,pl),7)<0)) PPP_MODE_E_fx = 2;
      else delta_lag_E=sub(l,pl);
      
      //Bump up if big change between the previous and the current CWs
     
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	  if ((sub(res_enratio_fx,0x2800)>0) && (sub(extract_h(L_shl(tmpsp_fx,sub(31,Qtmpsp))),0x5333)<0)) PPP_MODE_E_fx = 2; 
	    //5 in Q11, 0.65 in Q15
	}
	else {
	if ((sub(res_enratio_fx,0x1800)>0) && (sub(extract_h(L_shl(tmpsp_fx,sub(30,Qtmpsp))),0x4CCD)<0)) PPP_MODE_E_fx = 2; //3 in Q11, 1.2 in Q14 
	}
      
      
  }

#ifdef WMOPS_FX
  Ntest(15);
#endif
    if (Eargs->operating_point==1) {
      //Rapid rampdown frame where time resolution is important
      //Not a suitable PPP frame -> Bump to CELP
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	if (sub(shl(res_enratio_fx,4),0x333)<0) {jumptofullcelp_fx(rate);return;} //0.025 in Q15
      }
      else {
	if (sub(shl(res_enratio_fx,4),0x99A)<0) {jumptofullcelp_fx(rate);return;} //0.075 in Q15
      }
      //Rapid rampup frame where time resolution is important
      //Not a suitable PPP frame -> Bump to CELP 
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	if (sub(res_enratio_fx,0x7400)>0) {jumptofullcelp_fx(rate);return;} //14.5 in Q11
      }
      else {
	if (sub(res_enratio_fx,0x3800)>0) {jumptofullcelp_fx(rate);return;} // 7.0 in Q11
      }
    }
    if (Eargs->operating_point==2) {
      //Rapid rampdown frame where time resolution is important
      //Not a suitable PPP frame -> Bump to CELP
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	if (sub(shl(res_enratio_fx,4),0x333)<0) {jumptofullcelp_fx(rate);return;} //0.025 in Q15
       }
      else {
	if (sub(shl(res_enratio_fx,4),0x99A)<0) {jumptofullcelp_fx(rate);return;} //0.075 in Q15
	if ((sub(MIN_FX(shl(res_enratio_fx,4),shl(sp_enratio_fx,1)),0x99A)<0) && (sub(extract_h(L_shl(tmpsp_fx,sub(31,Qtmpsp))),0x4000) < 0)) {jumptofullcelp_fx(rate);return;} //0.5 in Q15
      }
      //Rapid rampup frame where time resolution is important
      //Not a suitable PPP frame -> Bump to CELP 
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	if (sub(res_enratio_fx,0x7400)>0) {jumptofullcelp_fx(rate);return;} //14.5 in Q11
        }
      else {
	if (sub(res_enratio_fx,0x3800)>0) {jumptofullcelp_fx(rate);return;} // 7.0 in Q11
	if ((tmpres_fx<=0) && (PPP_MODE_E_fx == 1)) {jumptofullcelp_fx(rate);return;} 
      }
    }
   
    //Bump up when the previous frame is an unvoiced or a silent frame
    if (lastrateE == 2 || lastrateE == 1) {jumptofullcelp_fx(rate);return;}
    //-----End Open-loop Bump-Up
   

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: OL bumpup ................................");
#endif
   
    //Quarter-rate or half-rate PPP Quantization
    if (PPP_MODE_E_fx == 0) {
      Word16 flag = TRUE ;
      if (PPP_MODE_E_fx==0)
	flag = ppp_quarter_encoder_fx(&currp_q_E_fx,prev_cw_E_fx.lag,currp_nq_fx,lpc2_fx,&TMP_fx,S_fx,C_fx,p_idx,a_idx1);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder ......................");
#endif
      if (flag) {
	//TMP: Target prototype: Amp Quantized + Phase Unquantized
	//TMP2: Quantized prototype: Amp Quantized + Phase Quantized
	//TMP3: Delta prototype: Diff betw. target and quant. in speech dom
	
	//-----Closed-loop Bump-Up
	Word32 energy_impz,Ltemp;
	Word32 pos_nq_fx, neg_nq_fx,pos_q_fx,neg_q_fx, pos_nq0_fx, neg_nq0_fx;
	Word16 Qposnq, Qnegnq, Qposq,Qnegq,Qposnq0,Qnegnq0;

	TMP_fx.peaktoaverage_fx(&pos_nq_fx, &Qposnq, &neg_nq_fx, &Qnegnq);
		  
	currp_q_E_fx.peaktoaverage_fx(&pos_q_fx, &Qposq, &neg_q_fx, &Qnegq);
	
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: CL bumup TMP and currp_q_E peak to average");
#endif

	//Before we perform the peak-to-average ratio comparison, we have to
	//ensure that the energy is not decaying and also the pitch pulse
	//is clearly defined

	Word16 rt1,rt2,rt3,rt4,qfac;
	Word32 rt2m;
	qfac=add(negate(sub(add(shl(currp_nq_fx.Q,1),1),Q_prev_cw_en_fx)),1);
        Lacc=divide_dp(currp_nq_fx.getEngy_fx(),prev_cw_en_fx,qfac); //Q30
	rt1=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(30-16)=14, currp_nq/prev_cw_en

	qfac=add(negate(sub(Qposnq,Qnegnq)),1);
	Lacc=divide_dp(pos_nq_fx,neg_nq_fx,qfac); //Q30
	rt2=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(30-16)=14, posnq/negnq

	qfac=negate(sub(Qposnq,Qposq));
	Lacc=divide_dp(pos_nq_fx,pos_q_fx,qfac); //Q29
	rt3=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(29-16)=13, posnq/posq

	qfac=negate(sub(Qnegnq,Qnegq));
	Lacc=divide_dp(neg_nq_fx,neg_q_fx,qfac); //Q29
	rt4=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(29-16)=13, negnq/negq

	//computing max of pos_nq and neg_nq
	if (sub(rt2,0x4000)>0) //1 in Q14
	  rt2m=L_shl(pos_nq_fx,sub(29,Qposnq)); //Q29
	else
	  rt2m=L_shl(neg_nq_fx,sub(29,Qnegnq)); //Q29
	
	
	
#ifdef WMOPS_FX
  Ntest(12);
#endif
	if (Eargs->operating_point==1) {
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)>=0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)>=0) { //25 in Q9
#endif
	  if ((sub(shl(rt1,1),0x6666)>0)&&(sub(extract_h(rt2m),0x6000)>0)) { //0.8 in Q15, 3 in Q13
	    if ((sub(rt2,0x4000)>0) && (sub(rt3,0x4000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2 in Q13
	     if ((sub(rt2,0x4000)<0) && (sub(rt4,0x4000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2 in Q13
	  }
	    }
	}
	else if (Eargs->operating_point==2) {
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	   if ((sub(shl(rt1,1),0x6666)>0)&&(sub(extract_h(rt2m),0x6000)>0)) { //0.8 in Q15, 3 in Q13
	    if ((sub(rt2,0x4000)>0) && (sub(rt3,0x4000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2 in Q13
	     if ((sub(rt2,0x4000)<0) && (sub(rt4,0x4000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2 in Q13
	   }
	  }
	  else {
	    if ((sub(rt1,0x4000)>0)&&(sub(extract_h(rt2m),0x7000)>0)) { //1 in Q14, 3.5 in Q13
	      if ((sub(rt2,0x4000)>0) && (sub(rt3,0x5000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2.5 in Q13
	     if ((sub(rt2,0x4000)<0) && (sub(rt4,0x5000)>0)) PPP_MODE_E_fx=2; //1 in Q14 and 2.5 in Q13
	    }
	    
	    currp_nq_fx.peaktoaverage_fx(&pos_nq0_fx, &Qposnq0, &neg_nq0_fx, &Qnegnq0);
	    
	    Word16 impzi[160],mx;
	    Word16 impzo[160];
       	    Word32  mem[10];
	    
	    Word32 diffarg;

#ifdef WMOPS_FX
  Nmove16(160);
  Nmove16(160);
  Nmove16(10);
#endif
	    for (impzi[0]=2048,i=1;i<160;i++) impzi[i]=0; //Q11
	    for (i=0;i<160;i++) impzo[i]=0;
	    for (i=0;i<10;i++) mem[i]=0;

	    synthesis_filter_fx(lpc2_fx, impzi, impzo, mem,ORDER, FSIZE, 3);
	    	    
	    for (i=0,Lacc=0;i<160;i++) Lacc=L_mac40(Lacc,impzo[i],impzo[i]); //Q23

	    energy_impz = L_sub(log10_lut(Lacc),0x22800000); //Q23, 10log10(2^23), in Q23


	    Lacc=currp_q_E_fx.getEngy_fx(); //2Q+1
	    n=norm32_l40(Lacc);
	    n = (Lacc==0)?31: n;
	    Ltemp=(Word32) L_shl40(Lacc,n); //Ltemp in qfac
            	    
	    qfac=add(n,add(shl(currp_q_E_fx.Q,1),1));
	    
#ifdef WMOPS_FX
  Ntest(8);
#endif
	    if (qfac>Q_prev_cw_en_fx)
	      diffarg=L_sub(L_shr(Ltemp,sub(qfac,Q_prev_cw_en_fx)),prev_cw_en_fx);
	    else
	      diffarg=L_sub(Ltemp,L_shr(prev_cw_en_fx,sub(Q_prev_cw_en_fx,qfac)));
	    
	    if (diffarg>0) //proceed to next stage
	      {
		//max(posq,negq)>3.5 ?
	        if (Qposq>Qnegq)
		  diffarg=L_sub(L_shr(pos_q_fx,sub(Qposq,Qnegq)),neg_q_fx);
		else
		  diffarg=L_sub(pos_q_fx,L_shr(neg_q_fx,sub(Qnegq,Qposq)));

		if (diffarg>0)
		  mx=extract_h(L_shl(pos_q_fx,sub(29,Qposq))); //Q29
		 else
		  mx=extract_h(L_shl(neg_q_fx,sub(29,Qnegq))); //Q29 

		if (sub(mx,0x7000)>0)
		  {                           //3.5 in Q13 //next stage
		    if (sub(extract_h(L_shl(energy_impz,4)),0x7800)>0)
		    {                         //15.0 in Q11, next stage
		    if (sub(extract_h(L_shl(tmpres_fx,sub(31,Qtmpres))),0x599A)>0) //next stage, 0.7 in Q15
		      {
			qfac=add(negate(sub(Qposq,Qposnq0)),1); //Q30
			Lacc=divide_dp(pos_q_fx,pos_nq0_fx,qfac); //Q30
			rt1=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(30-16)=14
	
			if ((diffarg>0)&&(sub(rt1,0x6000)>0)) //1.5 in Q14
			  PPP_MODE_E_fx=2;
			else
			  {
			    qfac=add(negate(sub(Qnegq,Qnegnq0)),1); //Q30
			    Lacc=divide_dp(neg_q_fx,neg_nq0_fx,qfac); //Q30
			    rt2=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(30-16)=14
                            if ((diffarg<=0)&&(sub(rt2,0x6000)>0)) //1.5 in Q14 
			      PPP_MODE_E_fx=2;
			  }
		      }
		    }
		  }
	      }
		
	  
	  } //else
	} //op
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: CL bumpup stage 1 QPPP ...................");
#endif

       TMP2_fx=currp_q_E_fx;

       TMP_fx.poleFilter_fx(lpc2_fx,ORDER, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
       TMP2_fx.poleFilter_fx(lpc2_fx,ORDER, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
       

       TMP3_fx = TMP_fx - TMP2_fx;
   
       Lacc=divide_dp(TMP_fx.getEngy_fx(1500,3300),TMP_fx.getEngy_fx(),2);//Q31
       rt1=extract_h((Word32) L_sat32_40(L_add40(Lacc,0x00008000))); //Q(31-16)=15


# if 1
        Lacc=TMP_fx.getEngy_fx(1500,3300);
	Lacc1= TMP3_fx.getEngy_fx(1500,3300);
	qfac = add(negate(shl(sub(TMP_fx.Q,TMP3_fx.Q),1)),2); //Q31
	sft=add(shl(sub(TMP_fx.Q,TMP3_fx.Q),1),1); // to check if Lacc<=2*Lacc1
	flag1=0;
	if (sft>0)
	  {
	    if (L_sub40_40(L_shr40(Lacc,sft),Lacc1)<=0)
	      flag1=1; // do the divide
	  }
	else
	  {
	    if (L_sub40_40(Lacc,L_shr40(Lacc1,negate(sft)))<=0)
	      flag1=1; // do the divide
	  }

	if (flag1==1)
	  {
	    Lacc=divide_dp(Lacc,Lacc1,qfac); //Q31
	    Ltemp=L_sub(log10_lut(Lacc),0x2E800000);// => sub 0x2E800000 , 10*log10(2^31) in Q23
	  }
	else
	  Ltemp=MAX_32;
	
#endif
	
	if (Eargs->operating_point==1) {
	   if (sub(rt1,0x666)>0)  //0.05 in Q15
	     if (sub(extract_h(L_shl(Ltemp,8)),0xCCD)<0) //0.1 in Q15
	       if (sub(shl(res_enratio_fx,4),0x6666)>0) PPP_MODE_E_fx = 2; //0.8 in Q15
	}

	
	//////////////to increase bump up, raise first threshold, lower second     /////////////////////////
# if 0	
	Ltemp=L_sub(log10_lut(divide_dp(TMP_fx.getEngy_fx(),TMP3_fx.getEngy_fx(),qfac)),0x2B800000);
	//Q23 op => sub 0x2B800000 , 10*log10(2^(29)) in Q23
# endif  
# if 1
        

        Lacc=TMP_fx.getEngy_fx();
	Lacc1= TMP3_fx.getEngy_fx();

	qfac=negate(shl(sub(TMP_fx.Q,TMP3_fx.Q),1)); //Q29
	sft=add(shl(sub(TMP_fx.Q,TMP3_fx.Q),1),1);// to check if Lacc<=2*Lacc1
	flag1=0;
	if (sft>0)
	  {
	    if (L_sub40_40(L_shr40(Lacc,sft),Lacc1)<=0)
	      flag1=1; // do the divide
	  }
	else
	  {
	    if (L_sub40_40(Lacc,L_shr40(Lacc1,negate(sft)))<=0)
	      flag1=1; // do the divide
	  }

      if (flag1==1)
	{
	  Lacc=divide_dp(Lacc,Lacc1,qfac);
          Ltemp=L_sub(log10_lut(Lacc),0x2B800000); //Q23 op => sub 0x2B800000 , 10*log10(2^(29)) in Q23
	}
      else
	Ltemp=MAX_32;
 #endif     
               
#ifdef WMOPS_FX
  Ntest(7);
#endif
	if (Eargs->operating_point==1) {
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	    if ((sub(extract_h(L_shl(Ltemp,6)),0x619A)<0) && (sub(MAX_FX(shl(res_enratio_fx,4),shl(sp_enratio_fx,1)),0x6666)>0)) PPP_MODE_E_fx = 2; //3.05 in Q13, 0.8 in Q15
	  }
	  else {
	    if ((sub(extract_h(L_shl(Ltemp,5)),0x4800)<0) && (sub(MAX_FX(shl(res_enratio_fx,4),shl(sp_enratio_fx,1)),0x4000)>0)) PPP_MODE_E_fx = 2; //4.5 in Q12, 0.5 in Q15
	  }
	}
	else if (Eargs->operating_point==2) {
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	if (sub(VAD_SNR_fx,SNR_THLD)<0) { //25 in Q9
#else
        if (sub(NS_SNR_fx,0x3200)<0) { //25 in Q9
#endif
	      if ((sub(extract_h(L_shl(Ltemp,6)),0x599A)<0) && (sub(MAX_FX(shl(res_enratio_fx,4),shl(sp_enratio_fx,1)),0x5333)>0)) PPP_MODE_E_fx = 2; //2.8 in Q13, 0.65 in Q15
	    }
	  else {
             if ((sub(extract_h(L_shl(Ltemp,6)),0x4CCD)<0) && (sub(MAX_FX(shl(res_enratio_fx,4),shl(sp_enratio_fx,1)),0x7852)>0)) PPP_MODE_E_fx = 2;	     //2.4 in Q13 and 0.94 in Q15
	   
	  }
	}
	//-----End closed-loop Bump-Up
	//////////////////////////////////////////////////////////////////////////////
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: QPPP CL bumpup stage 2 ...................");
#endif
 
      } 
      else PPP_MODE_E_fx = 2 ; //Amplitude quantization is failing
    }
    else if (PPP_MODE_E_fx == 1) {
       ppp_full_encoder_fx(&currp_q_E_fx, currp_nq_fx, lpc2_fx,  p_idx, a_idx2, rot_idx, band_idx,S_fx, C_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_full_encoder .........................");
#endif
    }
	      	
    else if (PPP_MODE_E_fx!=2)
      fprintf(stderr,"Bad PPP_MODE '%c' in PPP\n",PPP_MODE_E_fx);
    
        
    // Now Look for Bumpups either to FCELP or to FPPP when PPP_MODE_E='B'
   
#ifdef WMOPS_FX
  Ntest(10);
#endif
    PPP_BUMPUP=0;
    if (Eargs->operating_point==1) {
      if (Eargs->avg_rate_control) {
      	extern Word16 patterncount;
	extern Word16 pattern_m;
	if (PPP_MODE_E_fx==0) {
		patterncount += pattern_m;
		if (patterncount >= 1000) {
			patterncount -= 1000;
			PPP_MODE_E_fx = 2;
		}
	}
    }
      if (PPP_MODE_E_fx == 2 || prev_dim_and_burstE) {
	PPP_BUMPUP=1;// Closed loop bumpup is true
	pppcountE=0;// To start QP-FC-FC pattern again
	{jumptofullcelp_fx(rate);return;}
      }
    }
    else if (Eargs->operating_point==2) {
    if (Eargs->avg_rate_control) {
      {
      	extern Word16 patterncount;
	extern Word16 pattern_m;

	if (LAST_PPP_MODE_E_fx==0 && PPP_MODE_E_fx==0) {
		patterncount += pattern_m;
		if (patterncount >= 1000) {
			patterncount -= 1000;
			PPP_MODE_E_fx = 2;
		}
	}
      }
    }
      if (PPP_MODE_E_fx == 2 || prev_dim_and_burstE) {
	PPP_BUMPUP=1;// Closed loop bumpup is true
	{jumptofullcelp_fx(rate);return;}
      }
    }
  }
  
  //Packetization of the delta lag in QPPP
  if (PPP_MODE_E_fx == 0) 
     data_packet.Q_delta_lag = add(delta_lag_E,7);
  
  //If QPPP put amp quant and power indices in data_packet structure
  if (PPP_MODE_E_fx == 0) {
    data_packet.POWER_IDX=p_idx; // Power idx
    data_packet.AMP_IDX[0]=a_idx1[0]; // Low-band power idx
    data_packet.AMP_IDX[1]=a_idx1[1]; // hi-band power idx
      }
  //If FPPP put amp quant, power indices, and aligns in data_packet structure
  else if (PPP_MODE_E_fx == 1) {
    data_packet.F_ROT_IDX=rot_idx; // Rotation
    data_packet.A_POWER_IDX=p_idx; // Power idx
    data_packet.A_AMP_IDX[0]=a_idx2[0]; // Low-band power idx
    data_packet.A_AMP_IDX[1]=a_idx2[1]; // hi-band power idx
    data_packet.A_AMP_IDX[2]=a_idx2[2]; // hi-band power idx
    for (i=0; i<NUM_FIXED_BAND; i++) 
      data_packet.F_ALIGN_IDX[i]=band_idx[i]; 
  }
  LASTLAST_PPP_MODE_E_fx = LAST_PPP_MODE_E_fx;
  LAST_PPP_MODE_E_fx = PPP_MODE_E_fx;

  WIsyn_fx(prev_cw_E_fx,&currp_q_E_fx,lpc1_fx,lpc2_fx,&ph_offset_E_fx,out_fx,FSIZE,pf_temp1,pf_temp2,pf_temp,pf_n2);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: WIsyn ....................................");
#endif
  prev_cw_E_fx = currp_q_E_fx ; //modified by WIsyn
  Lacc=currp_nq_fx.getEngy_fx();//2Q+1
  Q_prev_cw_en_fx=norm32_l40(Lacc);
  Q_prev_cw_en_fx = (Lacc==0)?31: Q_prev_cw_en_fx;
  prev_cw_en_fx=(Word32) L_shl40(Lacc,Q_prev_cw_en_fx); //2Q+1+Q_prev_cw_en_fx
  Q_prev_cw_en_fx=add(Q_prev_cw_en_fx,add(shl(currp_nq_fx.Q,1),1));
 
  /* Update reconstruction filter memory */
  for (i=0;i<NoOfSubFrames;i++) {    
    if (i < 2) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;
    /* interpolate lsp */
    Interpol_fx(OldlspE_fx, lsp_fx, lspi_fx, i);
    Interpol_fx(Oldlsp_nq_fx, lsp_nq_fx, lspi_nq_fx, i);

    /* Convert lsp to PC */
     lsp2lpc_fx(lspi_fx, pci_fx, pci_fx);
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx);
        
    weight_fx(pci_nq_fx, gamma90_tab, wpci_gma1_fx, 10);
    weight_fx(pci_nq_fx, gamma50_tab, wpci_gma2_fx, 10);

    /* Update filters memory */
    ZeroInput_fx(out_fx+shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1), pci_fx, wpci_gma1_fx, wpci_gma2_fx, zir_fx, subframesize, 1);
  }
  /* Update excitation */
  for (j = 0; j < ACBMemSize; j++) Excitation_fx[j] = out_fx[sub(FrameSize,ACBMemSize)+j];
  
  /* Update encoder variables */
  pdelay_fx = delay_fx;
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER ...........................................");
#endif
  
}


/*===================================================================*/
/* FUNCTION      :  voiced_decoder_fx()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Voiced decoder main function                     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) post_filter: postfilter flag (1=enabled, 0=disabled) */
/*   _ (Word16) fer_flag: frame erasure flag (1=erasure, 0=no fer)   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) outFbuf: synthesized output speech, Q0,160 samples*/
/*-------------------------------------------------------------------*/
/* GLOBAL INPUT ARGUMENTS  :                                         */
/*    _ (Word16[])  lsp_fx  : current frame quantized lsp (Q15)      */
/*                            This is needed for erasure decoder     */
/*    _ (Word16[])  OldlspD_fx  : Last frame quantized lsp (Q15)     */
/*    _ (Word16[])  OldOldlspD_fx : last last frame quantized lsp    */
/*    _ (Word32[])  SynMemory_fx : (Q16)                             */
/*    _ (Word16[])  PitchMemoryD_fx  : Pitch Memory, Q0              */
/*    _ (Word16)    pdelayD_fx  :                                    */
/*    _ (Word16)    data_packet :                                    */
/*                                LSP_IDX[]                          */
/*                                delayindex                         */
/*                                Q_delta_lag -- for QPPP            */
/*                                POWER_IDX   -- for QPPP            */
/*                                AMP_IDX[2]  -- for QPPP            */
/*                                A_POWER_IDX     -- for FPPP        */
/*                                A_AMP_IDX[3]    -- for FPPP        */
/*                                F_ROT_IDX       -- for FPPP        */
/*                                F_ALIGN_IDX[17] -- for FPPP        */
/*    _ (Word32 [])  cbprevprev_D_fx: (Q15)                          */
/*    _ (Word32 [])  cbprev_D_fx: (Q15)                              */
/*    _ (Word16)     lastLgainD_fx :  Q11                            */
/*    _ (Word16)     lastHgainD_fx :  Q11                            */
/*    _ (Word16 [])  lasterbD_fx :  Q13                              */
/*    _ (Word16)  PPP_MODE_D_fx  :    (0='Q', 1='F')                 */
/*    _ (Word16) lastrateD : last vocoding rate                      */
/*    _ (Word16) LAST_PACKET_RATE_D :                                */
/*    _ (Word16) LAST_MODE_BIT_D :                                   */
/*    _ (Word16)  ave_acb_gain_fx:  Q14                              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* GLOBAL OUTPUT ARGUMENTS :                                         */
/*    _ (Word16[])  PitchMemoryD_fx  : Pitch Memory, Q0              */
/*    _ (Word32[])  SynMemory_fx : (Q16)                             */
/*    _ (Word16)    delay_fx  :  Q0                                  */
/*    _ (Word16)    pdelayD_fx  : Q0                                 */
/*    _ (Word16)   pdeltaD_fx  : Q0                                  */
/*    _ (Word16[])  lsp_fx  : current frame quantized lsp (Q15)      */
/*    _ (Word16)  LAST_PPP_MODE_D_fx  :    (0='Q', 1='F')            */
/*    _ (Word16)  ave_acb_gain_fx:  Q14                              */
/*    _ (Word16)  ave_fcb_gain_fx : Q0                               */
/*    _ (Word32 [])  cbprevprev_D_fx: (Q15)                          */
/*    _ (Word32 [])  cbprev_D_fx: (Q15)                              */
/*    _ (Word16)     lastLgainD_fx :  Q11                            */
/*    _ (Word16)     lastHgainD_fx :  Q11                            */
/*    _ (Word16 [])  lasterbD_fx :  Q13                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/*===================================================================*/
#ifdef VOIP_DORA_VOICE
void voiced_decoder_fx(Word16 *outFbuf,Word16 post_filter, Word16 fer_flag, Word16 phase_offset, Word16 run_length, Word16 time_warp_fraction, short*      obuf_len)
#else
void voiced_decoder_fx(Word16 *outFbuf,Word16 post_filter, Word16 fer_flag)
#endif
{
  
  Word16 i, j, n;
  Word16 subframesize;
  Word16 l, pl, tmp;
  Word16 tmplsp[ORDER], lpc1[ORDER], lpc2[ORDER];
  
  Word32 Ltemp, logLag, Ltemp_q;
  Word32 delayi_fx[3]; // Q16
  Word16 sin_tab[480], cos_tab[480];
  static Word16 ph_offset_d;
  Word16 Ql, Qh;
  DTFS_fx TMP, currp_q_D ;
  static DTFS_fx prev_cw_D_fx;
  Word16 *ppp_res=outFbuf; // reuse the output buffer for quantized residual
#ifdef VOIP_DORA_VOICE  
  Word16 scaled_res[SubFrameSize*2];
  static Word16 fl_phoffset = 0;
  
#else
  Word16 scaled_res[SubFrameSize];
#endif

    Word16 pf_temp1[MAXLAG_WI]; //maynot need more than MAXLAG_WI/2+1
    Word16 pf_temp2[MAXLAG_WI];
    Word16 pf_temp[MAXLAG_WI];
    Word16 pf_n2[MAXLAG_WI];

#ifdef VOIP_DORA_VOICE
  //Variables used for Phase Matching

  Word16		do_phase_matching = 0;
  Word16		pm_var1 = 0;
  Word16		temp_samples_count = 0;
  Word16		num_samples;
  Word16		store_ddelay,b;
  int 		    merge_start, merge_end, merge_stop, sample_counter;
  //End Variables used for Phase Matching

  if (phase_offset != 10) //Phase Offset == 10 denotes Phase Matching disabled
	do_phase_matching = 1; //check for Phase Matching
#endif

  
#ifdef WMOPS_FX
      WMP_fwc_fx("Start Voiced decoder .....................................");
#endif
  
#ifdef WMOPS_FX
  test();
#endif
  if (!fer_flag) { // good frame, no erasure
    /* Dequantize lsp from packet index */
    if (PPP_MODE_D_fx==0) { // QPPP
      lspmaq_dec_fx(nsub,nsize, tmplsp, (short *)data_packet.LSP_IDX,tbl_lsp_fx7); 

      for (i=0;i<LPCORDER;i++) {
	lsp_fx[i]=add(round32_16(L_add(L_mult(6554,cbprevprev_D_fx[i]),L_mult(13107,cbprev_D_fx[i]))),tmplsp[i]);
	cbprevprev_D_fx[i]=cbprev_D_fx[i];
	cbprev_D_fx[i]=round32_16(L_shl(L_mult(20480,tmplsp[i]),2));//need this to make it MALSPVQ, 20480 is 2.5 in Q13
      }
      // Stabilize LSP
#ifdef WMOPS_FX
  Ntest(4);
  move16();
#endif
      if (lsp_fx[0]<LSP_SPREAD_FACTOR_FX_QPPP) lsp_fx[0]=LSP_SPREAD_FACTOR_FX_QPPP;
      for (i=1; i<LPCORDER; i++)
	if (sub(lsp_fx[i],lsp_fx[i-1]) < LSP_SPREAD_FACTOR_FX_QPPP)
	  lsp_fx[i]=add(lsp_fx[i-1],LSP_SPREAD_FACTOR_FX_QPPP);
      
      if (sub(16384,lsp_fx[LPCORDER-1])< shl(LSP_SPREAD_FACTOR_FX_QPPP,1)) { 
	lsp_fx[LPCORDER-1]=sub(16384,shl(LSP_SPREAD_FACTOR_FX_QPPP,1));
	i=LPCORDER-2;
      while ((sub(lsp_fx[i+1],lsp_fx[i])<LSP_SPREAD_FACTOR_FX_QPPP) && (i>=0)) {
	  lsp_fx[i]=MIN_FX(sub(lsp_fx[i+1],LSP_SPREAD_FACTOR_FX_QPPP),0);
	  i--;
	}
      }
    
    }
    else { // FPPP
      for (i=0;i<ORDER;i++) cbprevprev_D_fx[i]=cbprev_D_fx[i]=lsp_fx[i];
#ifdef LSP_SPRD
      lsp_spread(lsp_fx);
#endif 
    }

    // Do Voiced/PPP Decoding
    delay_fx=add(data_packet.DELAY_IDX,DMIN);
  
#ifdef WMOPS_FX
  Ntest(4);
  Nlogic16(2);
#endif
    if (PPP_MODE_D_fx==0) { // QPPP
      pdeltaD_fx=sub(data_packet.Q_delta_lag,7);
      delay_fx=add(pdelayD_fx,pdeltaD_fx);
      if (delay_fx > 120) delay_fx=DMAX;
      else if (delay_fx < 20) delay_fx=DMIN;
    }

    l=delay_fx;
    pl=pdelayD_fx;
  
    if (sub(lastrateD,3) !=0 && sub(lastrateD,4) !=0) pl=l;

    tmp=round32_16(L_shl(L_mult(l,30310),1)); // 30310=1.85, Q14
    if (sub(tmp,pl)<0) pl=shr(pl,1);
    tmp=round32_16(L_mult(l,17695)); // 17695=0.54, Q15
    if (sub(MAXLAG,shl(pl,1))>=0 && sub(tmp,pl)>=0) pl=shl(pl,1);
  }
  else { // fer_flag=1
    PPP_MODE_D_fx=-1; // erasure
    delay_fx=pdelayD_fx;
    pl=pdelayD_fx;
    l=pl;
  }
#ifdef WMOPS_FX
	  WMP_fwc_fx("VOICED_DECODER: PPP MODE and LSPVQ .......................");
#endif

  /* Common for clean & erasure */
  Interpol_fx(OldOldlspD_fx, OldlspD_fx, tmplsp, 2);
  lsp2lpc_fx(tmplsp, pci_fx, lpc1);
  Interpol_fx(OldlspD_fx, lsp_fx, tmplsp, 2);
  lsp2lpc_fx(tmplsp, pci_fx, lpc2);
#ifdef WMOPS_FX
	  WMP_fwc_fx("VOICED_DECODER: Interpol and lsp2lpc .....................");
#endif
  
  //Restoring PPP memories when the last frame is non-PPP or full-rate PPP
  if (sub(LAST_PACKET_RATE_D,1)==0 || sub(LAST_PACKET_RATE_D,3)==0 || LAST_MODE_BIT_D==0 || prev_frame_error) {
    GetSinCosTab_fx(pl, sin_tab, cos_tab);
    prev_cw_D_fx.to_fs_fx(PitchMemoryD_fx+sub(ACBMemSize,pl), pl, sin_tab, cos_tab);
    ph_offset_d = 0 ;
	
  }
#ifdef WMOPS_FX
	  WMP_fwc_fx("VOICED_DECODER: GetSinCosTab and to_fs_fx ................");
#endif

  /* Update lastLgainD_fx lastHgainD_fx and lasterbD_fx */
  if (sub(LAST_PACKET_RATE_D,1)==0 || sub(LAST_PACKET_RATE_D,3)==0 || LAST_MODE_BIT_D==0 || prev_frame_error || (sub(LAST_PACKET_RATE_D,4)==0 && sub(LAST_MODE_BIT_D,1)==0)) {
    TMP = prev_cw_D_fx;
    TMP.car2pol_fx();
    logLag=log10_lut(TMP.lag); // logLag=10*log10(pl), Q23
    Ltemp_q=L_shl(L_mult(shl(TMP.Q,1),24660),9); // Ltemp_q=2Q*10log10(2), Q23
  /* Process low band */
    Ltemp=TMP.setEngyHarm_fx(377, 4524, 0, 4524, 1, 0, &Ql);// Ltemp in 2*TMP.Q
    /* Compensate for Q factor of energy to get log10(lag*eng)  */
    Ltemp=log10_lut(Ltemp); // Ltemp=10log10(eng), Q23
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag);  // Ltemp=10*log10(lag*eng), Q23
  
    lastLgainD_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11
    /* Process high band */
    Ltemp=TMP.setEngyHarm_fx(4524, 13517, 4524, 16384, 1, 0, &Qh);
    Ltemp=log10_lut(Ltemp);
    Ltemp=L_add(L_sub(Ltemp,Ltemp_q),logLag); // Ltemp=10*log10(lag*eng), Q23
    lastHgainD_fx=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11
  
    /* Need to unify the Q factors of both bands */
    TMP.Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
    n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
    if (n<0) TMP.rshiftHarmBand_fx(4524, 16384,n);
    else if (n>0) TMP.rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
    
    TMP.to_erb_fx(lasterbD_fx); // Q13 output
  }
#ifdef WMOPS_FX
	  WMP_fwc_fx("VOICED_DECODER: update lastLgainD, lastHgainD, lasterbD ..");
#endif

#ifdef WMOPS_FX
  test();
#endif
  
  if (!fer_flag) { // no frame erasure
    //Prototype Dequantization
    //NOTE that currp_q_D has garbage at this point

    currp_q_D.lag = l ;
    GetSinCosTab_fx(l, sin_tab, cos_tab);

#ifdef WMOPS_FX
  test();
#endif
    if (PPP_MODE_D_fx == 0) // QPPP
      ppp_quarter_decoder_fx(data_packet.POWER_IDX,(short *)data_packet.AMP_IDX,&currp_q_D, pl, lpc2 ,sin_tab, cos_tab);
    else // FPPP
      ppp_full_decoder_fx(data_packet.A_POWER_IDX, (short *)data_packet.A_AMP_IDX,
			  data_packet.F_ROT_IDX, (short *)data_packet.F_ALIGN_IDX,
			  lpc2, &currp_q_D, sin_tab, cos_tab);
    //WI 2-D SYNTHESIS SCHEME, output in ppp_res
#ifdef WMOPS_FX
	  WMP_fwc_fx("Finish ppp (full or quarter) decoder .....................");
#endif

#if 0
    time_warp_fraction = 16384;
#endif


    
#ifdef VOIP_DORA_VOICE
 
  if (do_phase_matching)
  {
	int prev_delay, prev_delay2;
	int prev_delay_end, prev_delay2_end;
	int encoder_phase, decoder_phase;
	Word16 temp1=0, temp2=0, temp3=0, temp=0, temp4=0, n,n1;
	Word16 one_pdelay, one_pdelayD;
	Word32 one_prevdelay,one_predelayD;
	Word32 one_lmems;
	Word16 one_mems, nshft, t16, tmp2;
	Word32 tmp, tmp1, tmp3;

	
	//if (idxppg != 0) //get delay of previous frame from current frame
	{
		prev_delay_end = pdelayD_fx; /* Q0 */
		prev_delay2_end = pdelayD_fx;
	
		prev_delay = pdelayD_fx;
		prev_delay2 = pdelayD_fx;
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
			encoder_phase = L_shr(L_mult(temp,one_pdelay),1);  // Result in Q20  //
			decoder_phase = L_shr(L_mult(temp2,one_pdelayD),1); // Result in Q20 //
		}
		else if (run_length == 2)
		{
			div_int_sp(add (temp,temp3),prev_delay,temp4);
			encoder_phase = L_shr(L_mult(temp4,one_pdelay),1);  // Result in Q20  
			                /*fmod (fmod (160, prev_delay) + fmod (160, prev_delay2), prev_delay)/prev_delay;*/
			div_int_sp(add (temp2,temp2),pdelayD_fx, temp4);
			decoder_phase = L_shr(L_mult(temp4,one_pdelayD),1); // Result in Q20 
			                /*fmod (fmod (160, pdelayD_fx) * 2, pdelayD_fx)/pdelayD_fx;*/
		}
	}
	else if (phase_offset == 1)
	{
		if (run_length == 1)
		{
			encoder_phase = 0;
			decoder_phase = L_shr(L_mult(temp2,one_pdelayD),1);
			                 /*fmod (160, pdelayD_fx)/pdelayD_fx;*/
             //printf("\n enc_phase: %d dec_phase: %d \n", encoder_phase, decoder_phase);

		}
		else if (run_length == 2)
		{
			encoder_phase = L_shr(L_mult(temp,one_pdelay),1);  // Result in Q20  
			                 /*fmod (160, prev_delay)/prev_delay;*/
            div_int_sp(add (temp2,temp2),pdelayD_fx, temp4);
			decoder_phase = L_shr(L_mult(temp4,one_pdelayD),1); // Result in Q20 
			                 /*fmod (fmod (160, pdelayD_fx) * 2, pdelayD_fx)/pdelayD_fx;*/
		}
	}
	else if (phase_offset == 2)
	{
		encoder_phase = 0;
		div_int_sp(add (temp,temp),pdelayD_fx, temp4);
		decoder_phase =  div_int_sp(temp4,pdelayD_fx,temp1);
		                /*fmod (fmod (160, pdelayD) * 2, pdelayD_fx)/pdelayD_fx;*/
	}
	else if (phase_offset == -1)
	{
	    div_int_sp(add (temp,temp3),prev_delay,temp4);
		encoder_phase = L_shr(L_mult(temp4,one_pdelay),1);
		               /*fmod (fmod (160, prev_delay) + fmod (160, prev_delay2), prev_delay)/prev_delay;*/
		div_int_sp(temp2,pdelayD_fx,temp1);
		decoder_phase =L_shr(L_mult(temp1,one_pdelayD),1); // Result in Q20 ;

		               /*fmod (160, pdelayD_fx)/pdelayD_fx */;
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
		
            if ( sub(pm_var1,100)>0 ) pm_var1 = 0; /* if pm is too large */
	    
            num_samples = sub (160,pm_var1);
		//printf("\n num_samples = %d \n",num_samples);
            if (sub(delay_fx,num_samples)> 0) num_samples = 160;
 
	    Word16 var1, var2;
            var1 = extract_l(shr(L_mult(delay_fx,2),1));
          
          if (num_samples <= 120) {
                if (time_warp_fraction < 0x4000)
                   time_warp_fraction = 0x4000;
                else 
                  time_warp_fraction = 0x6000; /* 1.5 in Q14 */ 
          }
		
	
	if (time_warp_fraction > 0x4000 || time_warp_fraction < 0x4000)
	{
		if (time_warp_fraction > 0x4000) //Expansion
		{
			temp = extract_l(shr(L_mult(delay_fx,3),1));
			if (phase_offset == -1 && (add(num_samples ,temp) <= 320))
			{
				num_samples =add (num_samples, temp);
                GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
	
				//WIsyn(prev_cw_D,&currp_q_D,lpc1,lpc2,&ph_offset_D,out, (int)(num_samples + 3 * delay_fx));

			}
			
			else if (phase_offset == -1 && (add(num_samples,var1) <= 320))
			{
				
				num_samples =add (num_samples, var1);
                GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
	
				//WIsyn(prev_cw_D,&currp_q_D,lpc1,lpc2,&ph_offset_D,out, (int)(num_samples + 2 * delay_fx));
				//num_samples += (int)(2 * delay_fx);
			}
			else
			{
				num_samples =add (num_samples, delay_fx);
                GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
				
				//WIsyn(prev_cw_D,&currp_q_D,lpc1,lpc2,&ph_offset_D,out, (int)(num_samples + delay_fx));
				//num_samples += (int)delay_fx;
			}
		}
		else if (time_warp_fraction < 0x4000) //Compression
		 {
			    if (sub(num_samples,delay_fx) > currp_q_D.lag) {
				     num_samples =sub (num_samples, delay_fx);
                     GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                     poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
					 }
                 else {
                     GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                     poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     merge_start = 0;
					 merge_end = prev_cw_D_fx.lag;
					  if (sub(merge_end,merge_start) > shr(num_samples,1))
		                 merge_stop = sub(num_samples,sub (merge_end, merge_start));
	                  else
		                 merge_stop = sub(merge_end, merge_start);
                      if (num_samples <= prev_cw_D_fx.lag)
				  				merge_stop = 0;
                       
					   temp1 = sub (merge_stop,merge_start);
	                   one_lmems=invert_lut(L_deposit_h(temp1),&n);
			           one_mems=round32_16(one_lmems);       //1/merge_stop-merge_start in Qn//
			           //printf("\n  n:%d, fx: %f, fl: %f", n,one_mems/pow(2.0,39-n), (1.0/merge_end-merge_start));

					   if (merge_stop != 0)
				         {		
					         //Regular compression as requested by de-jitter: produce one less pitch period
					         //Compress only if compressed samples >= 40
					        if (sub(num_samples,sub (merge_end, merge_start)) >= 20)
					         {
						         for (b = merge_start; b < merge_stop; b++){
							     tmp =  L_mult(sub(sub(merge_stop,merge_start),b),ppp_res[b]);//Q1
								 tmp1 = L_mult(b,ppp_res[b+(merge_end-merge_start)]); //Q1 format
								 tmp3=L_add(tmp,tmp1); //Q1
								 nshft=norm_l(tmp3);
								 t16= extract_h(L_shl(tmp3,nshft)); // Q(nshft+1-16)
								 tmp2= extract_h(L_mult(t16,one_mems));// one_mems in Q(29-n) 
				                 ppp_res[b]= shr(tmp2,add(add(-1,-n),nshft)); //Q0, shr by -1-n+nshft 
				                 //tmp2=ppp_res[b] * (1.0 * (merge_stop-merge_start-b)/(merge_stop-merge_start)) 
								                + ppp_res[b+(merge_end-merge_start)] * (1.0 * (b)/(merge_stop-merge_start));	
				                //printf("\n fx: %d, fl: %d",pp_res[b],tmp2);										

								 }
							     for (b = merge_stop; b + merge_end - merge_start < num_samples; b++)
								     	      ppp_res[b] = ppp_res[b + merge_end - merge_start];
						          num_samples = sub(num_samples,sub(merge_end,merge_start));
					       }
				}  //end merge start

			 }
		  }
	   }
	    // No Compression or Expansion i.e time_warp_fraction = 0x4000
	
	  else {                                     
	         GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
              poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
              WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
	     }
    
    } //no phase_matching_case starts
	
	else {
	num_samples = 160;
	Word16 one_mems, n1, nshft, t16, tmp2;
    Word32 tmp, tmp1, tmp3;
    int temp1;
    Word32 one_lmems ;
	if (time_warp_fraction > 0x4000 || time_warp_fraction < 0x4000)
	{
		if (time_warp_fraction > 0x4000) //Expansion
		{

        	num_samples =add (num_samples, delay_fx);
            GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
            poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
			WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
        
		
		}
		else if (time_warp_fraction < 0x4000) //Compression
		{
			 if (sub(num_samples,delay_fx) > currp_q_D.lag) {
				     num_samples =sub (num_samples, delay_fx);
                     GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                     poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
					 }
                 else {
                     GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
                     poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
                     merge_start = 0;
					 merge_end = prev_cw_D_fx.lag;
					  if (sub(merge_end,merge_start) > shr(num_samples,1))
		                 merge_stop = sub(num_samples,sub (merge_end, merge_start));
	                  else
		                 merge_stop = sub(merge_end, merge_start);
                      if (num_samples <= prev_cw_D_fx.lag)
				  				merge_stop = 0;
                       
					   temp1 = sub (merge_stop,merge_start);
	                   one_lmems=invert_lut(L_deposit_h(temp1),&n);
			           one_mems=round32_16(one_lmems);       //1/merge_stop-merge_start in Qn//
			           //printf("\n  n:%d, fx: %f, fl: %f", n,one_mems/pow(2.0,39-n), (1.0/merge_end-merge_start));

					   if (merge_stop != 0)
				         {		
					         //Regular compression as requested by de-jitter: produce one less pitch period
					         //Compress only if compressed samples >= 40
					        if (sub(num_samples,sub (merge_end, merge_start)) >= 20)
					         {
						         for (b = merge_start; b < merge_stop; b++){
							     tmp =  L_mult(sub(sub(merge_stop,merge_start),b),ppp_res[b]);//Q1
								 tmp1 = L_mult(b,ppp_res[b+(merge_end-merge_start)]); //Q1 format
								 tmp3=L_add(tmp,tmp1); //Q1
								 nshft=norm_l(tmp3);
								 t16= extract_h(L_shl(tmp3,nshft)); // Q(nshft+1-16)
								 tmp2= extract_h(L_mult(t16,one_mems));// one_mems in Q(29-n) 
				                 ppp_res[b]= shr(tmp2,add(add(-1,-n),nshft)); //Q0, shr by -1-n+nshft 
				                 //tmp2=ppp_res[b] * (1.0 * (merge_stop-merge_start-b)/(merge_stop-merge_start)) 
								                + ppp_res[b+(merge_end-merge_start)] * (1.0 * (b)/(merge_stop-merge_start));	
				                //printf("\n fx: %d, fl: %d",pp_res[b],tmp2);										

								 }
							     for (b = merge_stop; b + merge_end - merge_start < num_samples; b++)
								     	      ppp_res[b] = ppp_res[b + merge_end - merge_start];
						          num_samples = sub(num_samples,sub(merge_end,merge_start));
					       }
				}  //end merge stop

			 }


		}
   }
	else // No Compression or Expansion
    	{	

    	    GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
            poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
            WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,num_samples, pf_temp1, pf_temp2, pf_temp, pf_n2);
       }
   }
#else	
	GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
    poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
    WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,FSIZE, pf_temp1, pf_temp2, pf_temp, pf_n2);
#endif


#ifdef VOIP_DORA_VOICE
    *obuf_len = num_samples;
    //printf("\n voiced num samples: %d, fr_cnt:%d",num_samples, decode_fcnt);

#endif


#ifdef WMOPS_FX
	  WMP_fwc_fx("VOICED_DECODER: poleFilter_setup, WIsyn, GetSinCosTab ....");
#endif
   
    /* Update fer coefficients */
    Ltemp=sqrt_divide_dp(currp_q_D.getEngy_fx(), prev_cw_D_fx.getEngy_fx(), shl(sub(currp_q_D.Q,prev_cw_D_fx.Q),1), &n);
    //Ltemp=sqrt(Ecurr/Eprev), in Q(n)
    Ltemp=L_shr(Ltemp,1); // Ltemp in Q(n-1)
    n=sub(n,1);
    Ltemp=dsp_sqrt_lut(Ltemp, 0); // Q(n/2+16)
    if (n&1) Ltemp=(Word32) Mpy_32_16(Ltemp,23170); // adjust for odd Q factor
    n=shr(n,1); // n+16 is Q factor of Ltemp
    ave_acb_gain_fx=MIN_FX(16384,round32_16(L_shl(Ltemp,sub(14,n)))); // Q14
  
    ave_fcb_gain_fx = 0;
  }
  
  else { // fer_flag=1, erasure case
  
	
	//Use the RCELP extended pitch memory to get the phase of the current prototype
    //Extending the ACB memory
#ifdef WMOPS_FX
	  WMP_fwc_fx("Start Voiced Erasure processing ..........................");
#endif
    delayi_fx[0]=L_deposit_h(pl); // no need to interpolate because pl & l are same
    delayi_fx[1]=L_deposit_h(pl);
    delayi_fx[2]=L_deposit_h(pl);
#ifndef VOIP_DORA_VOICE   
    for (i=0; i<NoOfSubFrames; i++) {      
#ifdef WMOPS_FX
  test();
#endif
      
	  if (i < 2) subframesize = SubFrameSize - 1;
      else subframesize = SubFrameSize;
      // use pitch memory as working buffer because there is no need to preserve it
     
      acb_excitation_fx(PitchMemoryD_fx+ACBMemSize, delayi_fx, 16384, subframesize);
      
#ifdef WMOPS_FX
  Nmove16(subframesize);
  Nmove16(ACBMemSize);
#endif
      for (j=0; j<subframesize; j++) ppp_res[j+i*(SubFrameSize-1)]=PitchMemoryD_fx[j+ACBMemSize]; // need to save 160 samples of extrapolated residual
      
      for (j=0; j<ACBMemSize; j++) PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
    }

#else //VOIP Case
    
	num_samples = 160;
	*obuf_len = num_samples;

	for (i=0; i<NoOfSubFrames; i++) {   

#ifdef WMOPS_FX
  test();
#endif
Word16 temp1=0, temp2=0;
Word16 temp=0;
   
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

      acb_excitation_fx(PitchMemoryD_fx+ACBMemSize, delayi_fx, 16384, subframesize);
   
#ifdef WMOPS_FX
  Nmove16(subframesize);
  Nmove16(ACBMemSize);
#endif
      for (j=0; j<subframesize; j++) ppp_res[j+i*(SubFrameSize-1)]=PitchMemoryD_fx[j+ACBMemSize]; // need to save 160 samples of extrapolated residual
      
      for (j=0; j<ACBMemSize; j++) PitchMemoryD_fx[j]=PitchMemoryD_fx[j+subframesize];
      
	  }

#endif

    //Perform extraction on the ACB extended memory
    //Using temp_acb as a temporary storage for the extracted prototype
    ppp_extract_pitch_period_fx(ppp_res, PitchMemoryD_fx, l);
    GetSinCosTab_fx(l, sin_tab, cos_tab);
    TMP.to_fs_fx(PitchMemoryD_fx, l, sin_tab, cos_tab);
    currp_q_D.to_fs_fx(ppp_res+FSIZE-l, l, sin_tab, cos_tab);

    tmp=currp_q_D.alignment_extract_fx(TMP,lpc2,sin_tab,cos_tab); // tmp in Q2
    TMP.Q2phaseShift_fx(tmp, l, sin_tab,cos_tab);

    //Prototype Repeat with an appropriate scaling
    currp_q_D = prev_cw_D_fx*ave_acb_gain_fx;
    
    currp_q_D.car2pol_fx();
    currp_q_D=TMP.copy_phase_fx(currp_q_D); // copy phase of TMP over to currp_q_D

    //WI 2-D SYNTHESIS SCHEME, output in ppp_res
    GetSinCosTab_fx(currp_q_D.lag, S_fx, C_fx); //BASED ON PREVIOUS LAG 
    poleFilter_setup_fx(lpc2, ORDER, currp_q_D.lag, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
   WIsyn_fx(prev_cw_D_fx,&currp_q_D,lpc1,lpc2,&ph_offset_d,ppp_res,FSIZE, pf_temp1, pf_temp2, pf_temp, pf_n2);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: fer case .................................");
#endif

      
  }

 
  /* Common for clean & erasure */
  LAST_PPP_MODE_D_fx = PPP_MODE_D_fx ;
  prev_cw_D_fx = currp_q_D ;

  /* Update pitch memory */
#ifdef WMOPS_FX
  Nmove16(ACBMemSize);
#endif

#ifndef VOIP_DORA_VOICE  
  for (j=0;j<ACBMemSize;j++) PitchMemoryD_fx[j] = ppp_res[FSIZE-ACBMemSize+j];
#endif

#ifdef VOIP_DORA_VOICE
  Word16 temp1=0, temp2=0;
  Word16 temp=0;
#endif
 Word16 ind=0;
  n=0; // output counter
  
  for (i=0; i<NoOfSubFrames; i++) {    

#ifdef WMOPS_FX
  test();
#endif

#ifdef WMOPS_FX 
	  char tmpstr[100];
	  sprintf(tmpstr,"VOICED_DECODER: Start Subframe loop ......................",i);
      WMP_fwc_fx(tmpstr);
#endif

#ifndef VOIP_DORA_VOICE
    if (sub(i,2)<0) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;
#endif

     

#ifdef VOIP_DORA_VOICE
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


#ifndef VOIP_DORA_VOICE
      for (j=0;j<subframesize;j++)
      {
      ind=shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1);	
      PitchMemoryD_fx[ACBMemSize+j]=ppp_res[ind+j];
      }
          
    for (j = 0; j < ACBMemSize; j++)
      PitchMemoryD_fx[j] = PitchMemoryD_fx[j + subframesize];
#endif


    Interpol_fx(OldlspD_fx, lsp_fx, lspi_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, prev_pci_fx, pci_fx);

#ifdef VOIP_DORA_VOICE
     for (j=0;j<subframesize;j++)
           
		 PitchMemoryD_fx[ACBMemSize+j]=ppp_res[temp_samples_count+j];
     for (j = 0; j < ACBMemSize; j++)
		 PitchMemoryD_fx[j] = PitchMemoryD_fx[j + subframesize];
#endif 

#ifdef WMOPS_FX      
	  sprintf(tmpstr,"VOICED_DECODER: Interpol LSP and lsp2lpc SF-%d ............",i);
      WMP_fwc_fx(tmpstr);
#endif
    
    /* Scaled down residual to avoid overflow filter memory */
#ifdef VOIP_DORA_VOICE
    for(j=0;j<subframesize;j++) scaled_res[j]=shr(ppp_res[j+temp_samples_count],SCALE);  
#else
    for(j=0;j<subframesize;j++) scaled_res[j]=shr(ppp_res[j+i*(SubFrameSize-1)],SCALE);  
#endif
	synthesis_filter_fx(pci_fx , scaled_res, DECspeech_fx ,SynMemory_fx,ORDER,subframesize,3);

#ifdef WMOPS_FX      
	  sprintf(tmpstr,"VOICED_DECODER: synthesis_filter SF-%d ....................",i);
      WMP_fwc_fx(tmpstr);
#endif

    /* Postfilter */
#ifdef WMOPS_FX
  test();
#endif
    if (post_filter) {
      Post_Filter_fx(DECspeech_fx, lspi_fx, pci_fx, DECspeechPF_fx,
		     L_deposit_h(delay_fx), 0x3333, subframesize); // 0.4 in Q15
#ifdef WMOPS_FX      
	  sprintf(tmpstr,"VOICED_DECODER: Post Filter SF-%d .........................",i);
      WMP_fwc_fx(tmpstr);
#endif
    }
    
    else {
#ifdef WMOPS_FX
  Nmove16(subframesize);
#endif
      for(j=0;j<subframesize;j++) DECspeechPF_fx[j]=DECspeech_fx[j];
    }
#ifdef VOIP_DORA_VOICE    	
		temp_samples_count =add(temp_samples_count,subframesize); //Addition to support Warping

#endif
    //scale back up
    for(j=0;j<subframesize;j++) outFbuf[n++]=shl(DECspeechPF_fx[j],SCALE);
  }
  
#if 1
  for(j=0;j<ORDER;j++)
    prev_pci_fx[j]=pci_fx[j];
#endif
  
  pdelayD_fx = delay_fx;




  
}

