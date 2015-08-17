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

static char const rcsid[]="$Id: fer_fx.cc,v 1.12 2006/12/07 00:02:24 apadmana Exp $";

/*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for             */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 1999 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/

#include "proto_fx.h"
#include "globs_fx.h"
#include "basic_op.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

#ifdef VOIP_DORA
void fer_processing_fx(short *outFbuf_fx, short pf, short prev_rate,Word16 run_length, short phase_offset, short time_warp_fraction, short* obuf_len)

#else
void fer_processing_fx(short *outFbuf_fx, short pf, short prev_rate)
#endif
#define MAX_FX(x,y) (((x)>(y))?(x):(y))

{
     Word16 i, FCB_ATTN=0;
  
    	if (fer_counter==3) {
	  ave_acb_gain_fx = mult_r(ave_acb_gain_fx, 27307); //0.75/0.9 in Q15
# ifdef FCB_ATTN_FLAG
	  
	  switch(N_consec_ers){
	  case 2:  FCB_ATTN = 24576; // 0.75 in Q15
	    break;
	  case 3 : FCB_ATTN = 16384; //0.5 in Q15
            break;
	  case 4 : FCB_ATTN = 8192; //0.25 in Q15
            break;
	  default:  FCB_ATTN = 8192; //0.25 in Q15
	  }
	  
	  
	  //ave_fcb_gain_fx = MAX_FX(mult_r(ave_fcb_gain_fx, FCB_ATTN),5); 
	  ave_fcb_gain_fx = mult_r(ave_fcb_gain_fx, FCB_ATTN); 
	 
#endif
	  
	  
	}
    fer_counter=3;
#ifdef WMOPS_FX
  test();
#endif
    if (lastrateD>2) {
      ave_acb_gain_back_fx=ave_acb_gain_fx;
      pdelayD2_fx=pdelayD_fx;
      for (i=0;i<ACBMemSize;i++)
	     PitchMemoryD2_fx[i]=PitchMemoryD_fx[i];
    }

#ifdef WMOPS_FX
  test();
#endif
    if (lastrateD!=3) prev_rcelp_half=0;
#ifdef WMOPS_FX
  test();
#endif
    if (prev_rcelp_half==1) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      lastrateD=4;
      prev_rcelp_half=0;
    }

#ifdef WMOPS_FX
  test();
#endif
  prev_rate=bit_rate=lastrateD;
#ifdef WMOPS_FX
  WMP_fwc_fx("");
#endif
  for (i=0;i<ORDER;i++) lsp_fx[i]=OldlspD_fx[i];
    
  switch(prev_rate) {
  case 1:
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    ave_acb_gain_fx=ave_fcb_gain_fx=0;
    silence_decoder_fx(outFbuf_fx, pf, 1);
    FadeScale_fx = (Word16)MAX(0, sub(FadeScale_fx, 2458));
    break;
  case 2:
#ifdef VOIP_DORA_NELP
    nelp_decoder_fx(outFbuf_fx, pf,1,time_warp_fraction, obuf_len);
#else
    nelp_decoder_fx(outFbuf_fx, pf,1);
#endif
    FadeScale_fx = (Word16)MAX(0, sub(FadeScale_fx, 2458));
    break;
  case 3:


#ifdef VOIP_DORA_VOICE
    voiced_decoder_fx(outFbuf_fx,pf,1,phase_offset, run_length, time_warp_fraction, obuf_len );
#else
    voiced_decoder_fx(outFbuf_fx,pf,1);
#endif
    FadeScale_fx = (Word16)MAX(0, sub(FadeScale_fx, 2458));
    break;
  case 4:
#ifdef WMOPS_FX
  move16();
#endif
    ave_fcb_gain_fx=ave_fcb_gain_fx; 

#ifdef VOIP_DORA_CELP
    celp_erasure_decoder_fx(outFbuf_fx, pf,phase_offset,run_length, time_warp_fraction, obuf_len);
#else
    celp_erasure_decoder_fx(outFbuf_fx, pf);
#endif

    break;
  default:

#ifdef VOIP_DORA_CELP
     celp_erasure_decoder_fx(outFbuf_fx, pf,phase_offset,run_length, time_warp_fraction, obuf_len);
#else
      celp_erasure_decoder_fx(outFbuf_fx, pf);
#endif

  break;


  }

  /* Post FER ACB gain attenuation */
  ave_acb_gain_fx = mult_r(ave_acb_gain_fx, 29491); //0.9 in Q15 
#ifdef WMOPS_FX
      WMP_fwc_fx("FER processing ...........................................");
#endif
}
