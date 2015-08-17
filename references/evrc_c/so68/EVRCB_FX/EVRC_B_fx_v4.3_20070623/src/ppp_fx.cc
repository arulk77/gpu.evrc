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
/*  FPPP/QPPP encoder/decoder functions                                 */
/*======================================================================*/

#include "basic_op.h"
#include "proto_fx.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "cos_tab.dat"
#include "WI_tab.h"
#include "globs_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


/*===================================================================*/
/* FUNCTION      :  ppp_full_encoder_fx()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Full rate PPP encoder main routine               */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (DTFS_fx) CWin :  prototype in Cartesian domain               */
/*                (Word16) lag: length of prototype                  */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices, 3 words                  */
/*   _ (Word16) rot_idx: Rotation index                              */
/*   _ (Word16[]) band_idx:Band shift indices                        */
/*   _ (DTFS_fx *) CWout :  quantized prototype in Cartesian domain  */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
void ppp_full_encoder_fx(DTFS_fx *CWout, DTFS_fx CWin, Word16 *curr_lpc,
			 Word16& p_idx, Word16 *a_idx, Word16& rot_idx,
			 Word16 *band_idx, Word16 *sin_tab, Word16 *cos_tab)
{
  Word16 i;
  Word16 tmplpc[ORDER], tmp;
  DTFS_fx target;
  
  *CWout = CWin ;
  
  //Amplitude Quantization
  CWout->car2pol_fx();
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP car2pol .............................");
#endif
  CWout->quant_cw_memless_fx(curr_lpc, p_idx, a_idx, sin_tab, cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP quant_cw_memless ....................");
#endif
  
  target=CWin.copy_phase_fx(*CWout); // target has phase from CWin and amplitude from *CWout (unquantized phase & quantized amplitude)
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP copy phase ..........................");
#endif
  CWout->pol2car_fx();
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP pol2car .............................");
#endif

  /* Both *CWout & target in Cartesian domain at this point */

  for (i=0;i<ORDER;i++) tmplpc[i]=mult_r(curr_lpc[i],pwf80_fx[i]);

  target.poleFilter_fx(tmplpc,ORDER,sin_tab,cos_tab);
  CWout->poleFilter_fx(tmplpc,ORDER,sin_tab,cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP two pole filters b4 glbl_align ......");
#endif
  rot_idx=CWout->glbl_alignment_FPPP_fx(target);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP global aligX ........................");
#endif

  GetSinCosTab_fx(64, sin_tab, cos_tab); // Get sin/cos tables for phaseShift
  CWout->Q2phaseShift_fx(negate(shl(rot_idx,1)),64, sin_tab,cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP phaseShift after global align .......");
#endif

  GetSinCosTab_fx(CWout->lag, sin_tab, cos_tab); // Create tables for phaseShift_band
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP get sin cos tab .....................");
#endif
  
  for (i=0;i<NUM_FIXED_BAND;i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (i<3) {
      band_idx[i]=CWout->alignment_band_fx(target, F_BAND_fx[i],
                                       F_BAND_fx[i+1], 2, sin_tab, cos_tab);
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
      if (band_idx[i]<0) band_idx[i]=0;
    }
    else {
      band_idx[i]=CWout->alignment_band_fx(target, F_BAND_fx[i],
                                       F_BAND_fx[i+1], 1, sin_tab, cos_tab);
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
      if (band_idx[i]<0) band_idx[i]=0;
    }
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP band alignment ......................");
#endif

  for (i=0;i<NUM_FIXED_BAND;i++) {
#ifdef WMOPS_FX
  test();
#endif
     if (sub(i,3)<0) {
       tmp=add(-64,shl(band_idx[i],2)); // tmp in Q2
       CWout->phaseShift_band_fx(negate(tmp),F_BAND_fx[i], F_BAND_fx[i+1],
				 sin_tab, cos_tab);
     }
     else {
       tmp=add(-64,shl(band_idx[i],1)); // tmp in Q2
       CWout->phaseShift_band_fx(negate(tmp),F_BAND_fx[i], F_BAND_fx[i+1],
				 sin_tab, cos_tab);
     }
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP phaseShift band .....................");
#endif
  CWout->zeroFilter_fx(tmplpc,ORDER, sin_tab, cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: FPPP zero filter after quant .............");
#endif
  
}

/*===================================================================*/
/* FUNCTION      :  ppp_full_decoder_fx()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Full rate PPP decoder main routine               */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices, 3 words                  */
/*   _ (Word16) rot_idx: Rotation index                              */
/*   _ (Word16[]) band_idx:Band shift indices                        */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (DTFS_fx *) CWout :  prototype in polar domain                */
/*                (Word16) lag: length of prototype                  */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) CWout :  prototype in Cartesian domain              */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : RX                                                  */
/*===================================================================*/
void ppp_full_decoder_fx(Word16 p_idx, Word16 *a_idx, Word16 rot_idx,
			 Word16 *band_idx, Word16 *curr_lpc,
			 DTFS_fx *CWout, Word16 *sin_tab, Word16 *cos_tab)
{
  Word16 i;
  Word16 tmp;
  Word16 tmplpc[ORDER];
 
  //Amplitude Dequantization
  CWout->dequant_cw_memless_fx(p_idx, a_idx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP dequant_cw_memless ..................");
#endif //WMOPS_FX
  CWout->pol2car_fx();
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP pol2car .............................");
#endif //WMOPS_FX
  for (i=0;i<ORDER;i++) {
    tmplpc[i]=mult_r(curr_lpc[i],pwf80_fx[i]);
  }

  CWout->poleFilter_fx(tmplpc,ORDER,sin_tab,cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP poleFilter ..........................");
#endif //WMOPS_FX
  GetSinCosTab_fx(64, sin_tab, cos_tab); // Create tables for phaseShift
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP 1st GetSinCosTab ....................");
#endif //WMOPS_FX
  CWout->Q2phaseShift_fx(negate(shl(rot_idx,1)),64, sin_tab,cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP phaseShift ..........................");
#endif //WMOPS_FX

  GetSinCosTab_fx(CWout->lag, sin_tab, cos_tab); // Create tables for phaseShift_band
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP 2nd GetSinCosTab ....................");
#endif //WMOPS_FX
  for (i=0;i<NUM_FIXED_BAND;i++) {
#ifdef WMOPS_FX
  test();
#endif
     if (sub(i,3)<0) {
       tmp=add(-64,shl(band_idx[i],2)); // tmp in Q2
       CWout->phaseShift_band_fx(negate(tmp),F_BAND_fx[i], F_BAND_fx[i+1],
				 sin_tab, cos_tab);
     }
     else {
       tmp=add(-64,shl(band_idx[i],1)); // tmp in Q2
       CWout->phaseShift_band_fx(negate(tmp),F_BAND_fx[i], F_BAND_fx[i+1],
				 sin_tab, cos_tab);
     }
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP phaseShift band .....................");
#endif //WMOPS_FX
  CWout->zeroFilter_fx(tmplpc,ORDER, sin_tab, cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: FPPP zeroFilter ..........................");
#endif //WMOPS_FX
}


/*===================================================================*/
/* FUNCTION      :  ppp_quarter_encoder_fx()                         */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Quarter rate PPP encoder main routine            */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) curr_lsp_fx: LPC coefficients, Q12                */
/*   _ (DTFS_fx) currCW_fx :  prototype in Cartesian domain          */
/*                (Word16) lag: length of prototype                  */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*   _ (Word16 []) S_fx: sine table based on lag, Q15                */
/*   _ (Word16 []) C_fx: cosine table based on lag, Q15              */
/*   _ (Word16)    pl: Previous lag, Q0                              */  
/*   _ (Word16 *) Excitation_fx : Global input (Q0)                  */ 
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) pidx: Power index                                    */
/*   _ (Word16[]) aidx: Amplitude indices, 2 words                   */
/*   _ (DTFS_fx *) currCW_q_fx :  quantized prototype in Cartesian domain */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*   _ (DTFS_fx *) targetCW_fx : Target prototype in Cartesian domain */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a/b: harmonics, normalized             */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*   _ (Word16[]) lasterbE_fx: ERB history for differential          */
/*                quantization, Q13                                  */
/*   _ (Word16) lastLgainE_fx: low band power history, log domain,
                                                            Q11      */
/*   _ (Word16) lastHgainE_fx: high band power history, log domain,
                                                            Q11      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) returnFlag: flag indicating success/failure
                     (TRUE/FALSE)                                    */                               
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/



Word16  ppp_quarter_encoder_fx(DTFS_fx *currCW_q_fx, Word16 pl, DTFS_fx currCW_fx,Word16 *curr_lsp_fx, DTFS_fx *targetCW_fx,Word16 *S_fx, Word16 *C_fx,Word16& pidx,Word16 *aidx )
{
  *currCW_q_fx=currCW_fx;
  DTFS_fx TMP_fx, TMP2_fx;
  
  Word16 i, j, l=currCW_q_fx->lag;
  short subframesize;
  Word16 returnFlag=TRUE;
  Word16 tmp_fx ;
  Word16 temp_acb_fx[ACBMemSize+SubFrameSize+10], temp_res_fx[160];
  Word32 delayD3_fx[3];
  //Extending the ACB memory
  for (i=0;i<ACBMemSize; i++) temp_acb_fx[i]=Excitation_fx[i];
  for (i=0; i<NoOfSubFrames; i++) {      
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = sub(SubFrameSize,1);
    else subframesize = SubFrameSize;
    
    // Interpolate delay
    Interpol_delay_fx(pl, l, delayD3_fx, i);
    // Compute adaptive codebook contribution
    acb_excitation_fx(temp_acb_fx + ACBMemSize, delayD3_fx, 16384, subframesize);
	for (j=0; j<subframesize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
		temp_res_fx[j+i*(SubFrameSize-1)]=temp_acb_fx[j+ACBMemSize];
	}
	for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
		temp_acb_fx[j]=temp_acb_fx[j+subframesize];
	}
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder ACB extension ........");
#endif

  //Perform extraction on the ACB extended memory
  //Using temp_acb as a temporary storage for the extracted prototype
  ppp_extract_pitch_period_fx(temp_res_fx, temp_acb_fx, l) ;
     
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder Extract pitch ........");
#endif
  TMP_fx.to_fs_fx(temp_acb_fx, l, S_fx, C_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder to_fs of extracted PPP");
#endif
  

  tmp_fx = alignment_extract_td_fx(temp_acb_fx, temp_res_fx+FSIZE-l, l) ; //Q0
  tmp_fx = shl(tmp_fx,2); //Q2


 
  //  tmp_fx=shr(tmp_fx,3); //Q5->Q2,future modification
 
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder align_extract ........");
#endif
 
  TMP_fx.Q2phaseShift_fx(tmp_fx, l, S_fx, C_fx) ; //Q7 input

#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder phaseShift/align_ext..");
#endif
  //Amplitude Quantization
  
  *targetCW_fx=*currCW_q_fx;
  currCW_q_fx->car2pol_fx();
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder car2pol ..............");
#endif
  returnFlag=currCW_q_fx->quant_cw_fx(pl, curr_lsp_fx,pidx,aidx,S_fx,C_fx);
 
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder Amp Quant ............");
#endif
  *targetCW_fx=targetCW_fx->copy_phase_fx(*currCW_q_fx);
  
  //Copying phase spectrum over
  *currCW_q_fx=TMP_fx.copy_phase_fx(*currCW_q_fx);
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder copy phase spect .....");
#endif
  tmp_fx = currCW_q_fx->alignment_fine_fx(*targetCW_fx, S_fx, C_fx) ;   // Q2
  //revisit to improve precision
  
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder alignment fine .......");
#endif
  targetCW_fx->Q2phaseShift_fx(tmp_fx, targetCW_fx->lag, S_fx, C_fx) ;
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_ENCODER: ppp_quarter_encoder phase shift/alignfine ");
#endif
  
  return returnFlag;
}


/*===================================================================*/
/* FUNCTION      :  ppp_quarter_decoder_fx                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function dequantizes the prototype
                     extracted from ACB memory extension             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*    _ (Word16) pidx: Power index                                   */
/*    _ (Word16[]) aidx: Amplitude indices, 2 words                  */
/*    _ (Word16)    pl: Previous lag (Q0)                            */
/*    _ (Word16[])  curr_lsp_fx: current frame lpc coefficients
                                          in Q12                     */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*CL)) table, Q15                 */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*CL)) table, Q15                 */
/*   _ (Word16 *) PitchMemoryD_fx : Global input (Q0)                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*    _ (DTFS_fx*) currCW_q_fx :  prototype in cartesian domain      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* GLOBAL VARIABLES UPDATED :                                        */
/*   _ (Word16[]) lasterbD_fx: ERB history for differential          */
/*                quantization, Q13                                  */
/*   _ (Word16) lastLgainD_fx: low band power history, log domain,
                                                            Q11      */
/*   _ (Word16) lastHgainD_fx: high band power history, log domain,
                                                            Q11      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*     _ None                                                        */
/* This function uses a table derived from CL(current lag)           */
/*===================================================================*/


void ppp_quarter_decoder_fx(Word16 pidx,Word16 *aidx,DTFS_fx *currCW_q_fx, Word16 pl, Word16 *curr_lsp_fx,Word16 *S_fx, Word16 *C_fx)
{
  DTFS_fx TMP_fx, TMP2_fx;            
  
  Word16 i, j;
  short subframesize;
  Word16 temp_acb_fx[ACBMemSize+SubFrameSize+10],temp_res_fx[160];
  Word32 delayD3_fx[3];
  Word16 tmp_fx;
  Word16 l=currCW_q_fx->lag;
  Word16 cerb[NUM_ERB]; //dummy var

  //Extending the ACB memory
  for (i=0;i<ACBMemSize; i++) temp_acb_fx[i]=PitchMemoryD_fx[i];
  for (i=0; i<NoOfSubFrames; i++) {      
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = sub(SubFrameSize,1);
    else subframesize = SubFrameSize;
    
    // Interpolate delay
    Interpol_delay_fx(pl, l, delayD3_fx, i);
    // Compute adaptive codebook contribution
    acb_excitation_fx(temp_acb_fx + ACBMemSize, delayD3_fx, 16384, subframesize);
	for (j=0; j<subframesize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
		temp_res_fx[j+i*(SubFrameSize-1)]=temp_acb_fx[j+ACBMemSize];
	}
	for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
		temp_acb_fx[j]=temp_acb_fx[j+subframesize];
	}
  }
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP ACB extension .......................");
#endif //WMOPS_FX

  //Perform extraction on the ACB extended memory
  //Using temp_acb as a temporary storage for the extracted prototype
  ppp_extract_pitch_period_fx(temp_res_fx, temp_acb_fx, l) ;
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP ppp_extract_pitch_period ............");
#endif //WMOPS_FX
    
  TMP_fx.to_fs_fx(temp_acb_fx, l, S_fx, C_fx);
  
  //Ensure the extracted prototype is time-synchronous to the
  //last l samples of the frame. 
   TMP2_fx.to_fs_fx(temp_res_fx+FSIZE-l,l, S_fx, C_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP 2 to_fs .............................");
#endif //WMOPS_FX
  
  tmp_fx = TMP2_fx.alignment_extract_fx(TMP_fx, curr_lsp_fx, S_fx, C_fx) ; //Q2
  //  tmp_fx=shr(tmp_fx,3); //Q5->Q2,future modification
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP alignment_extract ...................");
#endif //WMOPS_FX
 
  TMP_fx.Q2phaseShift_fx(tmp_fx, l, S_fx, C_fx) ; //Q7 input
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP phaseShift ..........................");
#endif //WMOPS_FX
  
  //Amplitude Dequantization
  currCW_q_fx->dequant_cw_fx(pl,pidx,aidx,lasterbD_fx,lastLgainD_fx,lastHgainD_fx,cerb);
  *currCW_q_fx=TMP_fx.copy_phase_fx(*currCW_q_fx);
#ifdef WMOPS_FX
      WMP_fwc_fx("VOICED_DECODER: QPPP dequant_cw and copy phase ...........");
#endif //WMOPS_FX
    
}
