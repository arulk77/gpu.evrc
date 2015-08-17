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
/*  globs_fx.cc -- Global variable initialization                       */
/*======================================================================*/

#include "typedef_fx.h"
#include "macro_fx.h"
#include "globs_fx.h"
#include "stdio.h"
void Init_globs_fx()
{
  Word16 j;
  
  for(j=0;j<ORDER;j++)
    {
      lsp_fx[j]=lspi_fx[j]=pci_fx[j]=lsp_nq_fx[j]= wpci_gma1_fx[j] =0;
      wpci_gma2_fx[j] = lspi_nq_fx[j] = pci_nq_fx[j] = FIRmem_fx[j] = 0;
      wpci_fx[j]= fcbIndexVector_fx[j]= cbprev_E_fx[j]= cbprevprev_E_fx[j]=0;
    }  
  for(j=0;j<SubFrameSize + 6 ;j++)
     Scratch_fx[j]=ExconvH_fx[j]=0;

  for(j=0;j<6;j++)
     SScratch_fx[j]=0;
  
  FCBGainSize=0;   /* Current Fixed Codebook Gain Size */

  
  PACKET_RATE=4;    
  MODE_BIT=0;
  
    /*======================================================================*/
    /* Encoder variables                                                    */
    /*======================================================================*/
   stoporder15 = 10;
   stoporder30  = 10;
   beta_fx=beta1_fx=0;  // periodicity, Q15
   delay_fx= delay1_fx=0; // open loop pitch, Q0
   idxppg=idxcb=idxcbg=0;


    for(j=0;j<GUARD + FrameSize + LOOKAHEAD_LEN;j++)
       residual_fx[j]=0;

    for(j=0;j<GUARD+FrameSize+LOOKAHEAD_LEN;j++)
      HPspeech_fx[j]=0;
    for(j=0;j<GUARD;j++)
       ConstHPspeech_fx[j]=0;

    for(j=0;j<Hlength+1;j++)
       H_fx[j]=0;        	

    for(j=0;j<SubFrameSize+1;j++)
      TARGETw_fx[j]= TARGET_fx[j] = 0;

    fcbGain_fx=0;

    for(j=0;j<SubFrameSize;j++)
      origm_fx[j]=worigm_fx[j]=0;

    for(j=0;j<FrameSize;j++)
      wspeech_fx[j]=mspeech_fx[j]=0;
 
    for(j=0;j<ACBMemSize+SubFrameSize+EXTRA;j++)
      bufferm_fx[j]=0;

    accshift_fx=0;

    for(j=0;j<SubFrameSize + EXTRA;j++)
      residualm_fx[j]=0;

    for(j=0;j<ACBMemSize+SubFrameSize+EXTRA;j++)
      Excitation_fx[j]=0;

    for(j=0;j<SubFrameSize;j++)
      zir_fx[j]=0;

    PPP_BUMPUP=pppcountE=0;
    rcelp_half_rateE= prev_dim_and_burstE= dim_and_burstE=0;
    PPP_MODE_E_fx = lastLgainE_fx = lastHgainE_fx=0;
    for(j=0;j<NUM_ERB;j++)
      lasterbE_fx[j]=0;
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
    VAD_SNR_fx = 0;
#else
    NS_SNR_fx=0;
#endif
        
    LAST_PACKET_RATE_E=lastrateE=lastlastrateE=1;
    
    LAST_MODE_BIT_E=0;
    ph_offset_E_fx=0; 
    /*======================================================================*/
    /* Decoder variables                                                    */
    /*======================================================================*/
    for(j=0;j<SubFrameSize;j++)
      DECspeech_fx[j]=DECspeechPF_fx[j]=0;
    
    for(j=0;j<ACBMemSize + SubFrameSize + EXTRA;j++)
      PitchMemoryD_fx[j]=0;
 
    for(j=0;j<ACBMemSize;j++)
      PitchMemoryD2_fx[j]=PitchMemoryD3_fx[j]=0;

    ave_fcb_gain_fx= ave_acb_gain_fx=ave_acb_gain_back_fx = 0; //Q14

    for(j=0;j<ORDER;j++)
    {
      cbprev_D_fx[j]= cbprevprev_D_fx[j]=0;
    } 

    pdelayD2_fx = pdelayD3_fx = 40;

    fer_counter=0;
    lastrateD=last_last_rateD=last_last_last_rateD = 1;
    //lastrateD=0;
    for (j = 0; j < PACKWDSNUM; j++) {RxPkt[j]=0;}

    lastLgainD_fx = lastHgainD_fx=0;
    for(j=0;j<NUM_ERB;j++)
      lasterbD_fx[j]=0;

    PPP_MODE_D_fx=LAST_PPP_MODE_D_fx=0;
    pdeltaD_fx=0;


    LAST_PACKET_RATE_D=LAST_MODE_BIT_D=0;
    rcelp_half_rateD= prev_rcelp_half =0;


    /*==================================================*/
    /*      BAD-RATE CHECK variables                    */
    /*==================================================*/
    zrbit[0]=0;zrbit[1]=0;
    BAD_RATE=0;

    /*==================================================*/
    /*      TTY variables                               */
    /*==================================================*/

     for(j=0;j<DATA_SIZE+5;j++)
       data_buf[j]=0;

     /* SPL rate vars */
     SPL_HCELP=SPL_HPPP=SPL_HNELP= N_consec_ers=0;

     /* voiced/WI */
     for(j=0;j<481;j++)
         C_fx[j]=S_fx[j]=0;
    
     /* DTX related */
     /* smart blanking and LSP jittering related */
     ER_count =0;
     SID=0;
     SILFrame=0;
     INTERP =NO;
     G_INTERP=NO;
     noSID=0;
     lpcgflag=0;
     ones_dec_cnt =0;
        

}

