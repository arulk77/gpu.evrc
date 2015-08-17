
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

static char const rcsid[]="$Id: decode_fx.cc,v 1.46 2007/06/24 11:45:12 apadmana Exp $";

//*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for             */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 1999 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
#include "macro_fx.h"
#include "proto_fx.h"
#include "globs_fx.h"
#include "basic_op.h"
#include "dsp_math.h"
#include "lpcana_tab.h"
#include "ppvq_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

 
#include "tty.h"
#include "dtmf.h"

#if MOT_SUPPRESSED_EIGHTH_RATE
void update_bbg_estimate(short *buf16, int rate, Word16 walpha);
void init_bbg();
void gen_encode_bbg(short *buf16);
#endif

/*======================================================================*/
/*         ..Reset RCELP decoder parameters.                            */
/*----------------------------------------------------------------------*/
void    InitDecoder_fx()
{
  /*....(local) variables....*/
  Word16  j;
    
#ifdef WMOPS_FX
  Nmove16(3);
#endif
  bit_rate=4;
  last_valid_rate = 1;	/* reset last_valid_rate */
  for (j=0; j<ORDER; j++) {
#ifdef WMOPS_FX
  move32();
#endif
	  SynMemory_fx[j] = 0;
  }

  for (j=0; j<ORDER; j++) {
    OldOldlspD_fx[j]=OldlspD_fx[j]=lsp_init_tab[j];
  }

  for(j=0;j<ORDER;j++)
    prev_pci_fx[j]= pci_init_tab[j];

  for (j=0; j<ACBMemSize; j++) {
#ifdef WMOPS_FX
  Nmove16(3);
#endif
    PitchMemoryD_fx[j]  = PitchMemoryD2_fx[j] = PitchMemoryD3_fx[j] = 0;
  }

  Init_Post_Filter_fx();
#ifdef WMOPS_FX
  Nmove16(8);
#endif
  pdelayD_fx = 40;
  ave_acb_gain_fx = ave_fcb_gain_fx = ave_acb_gain_back_fx = 0;
  FadeScale_fx = 16384;   // Q14
  prev_frame_error = 0;
  prev_prev_frame_error = 0;
  erasureFlag = 0;
  errorFlag = 0;

#if MOT_SUPPRESSED_EIGHTH_RATE
  init_bbg();
#endif
  SILFrame=prev_SILFrame=0;
}


void data_packet_struc_init(struct PACKET *f)
{
  short i,j;
  f->PACKET_RATE=0;
  f->MODE_BIT=0;//for the bit rate
  for(i=0;i<4;i++)
    f->LSP_IDX[i]=0;

  f->DELAY_IDX=0;
  f->DELTA_DELAY_IDX=0;

  for(i=0;i<3;i++)
    f->ACBG_IDX[i]=0;

  for(i=0;i<3;i++)
    for(j=0;j<4;j++)
      f->FCB_PULSE_IDX[i][j]=0;//3=NoOfSubFrames

  for(i=0;i<3;i++)
    f->FCBG_IDX[i]=0;

  for(i=0;i<2;i++)
    for(j=0;j<2;j++)
      f->NELP_GAIN_IDX[i][j]=0;
  
  f->NELP_FID=0;
  f->SILENCE_GAIN=0;
    
  f->Q_delta_lag=0; /*Delta pitch for QPPP*/
  f->POWER_IDX=0;     /*Codebook index for the power quantization for QPPP*/
  for(i=0;i<2;i++)
    f->AMP_IDX[i]=0;    /*Codebook index for the Amplitude quantization for QPPP*/
  f->F_ROT_IDX=0; /*Codebook index for the global alignment for FPPP*/
  f->A_POWER_IDX=0;/*Codebook index for the absolute power quantization for FPPP*/
  for(i=0;i<3;i++)
    f->A_AMP_IDX[i]=0; /*Codebook index for the absolute amplitude quantization for FPPP*/
  for(i=0;i<17;i++)
    f->F_ALIGN_IDX[i]=0; /*Codebook index for the alignment shifts for FPPP*/

}


/*======================================================================*/
/*         ..Decode bitstream data.                                     */
/*----------------------------------------------------------------------*/
#ifdef VOIP_DORA
void  decode_fx(short*  codeBuf,short  rate,short  post_filter,short*  outFbuf_fx,short run_length, short phase_offset,short time_warp_fraction,short* obuf_len)
#else
void  decode_fx(short*  codeBuf,short  rate,short  post_filter,short*  outFbuf_fx)
#endif
{


  Word16 spl_hcelp=0x6E;
  Word16 poss_spl_hcelp;
  Word16 poss_qnelp;
  register int i;
  short   local_rate;
  erasureFlag = 0;
  /*-------------------------------------------------------------------*/
  /*                        DATA,TTY DTMF                              */
  /*-------------------------------------------------------------------*/
  Word16 sigout_16[FSIZE];
  Word16 dtmf_digit;
  Word16 tty_type;
  Word16 chk_data;    /*=0 speech, 1 data, 2 speech+data */
  static Word16 chk_data_last=0;
  static Word16 dtmf_digit_last=-1;
  static Word16 header_last=-1;
  Word16 DTMF;
#if MOT_SUPPRESSED_EIGHTH_RATE
  Word16 update_bbg_flag;
  // Move this static declaration to a global
  static Word32 consec_eighth_rates = 3;
#endif

#ifdef VOIP_DORA
  //Phase Matching variable
  Word16	do_phase_match = 0;
  if (phase_offset != 10) //Phase Offset == 10 denotes Phase Matching disabled
	do_phase_match = 1; //check for Phase Matching
Lable:
#endif

  
#if 1 //routine to initialize the data_packet structure
  
data_packet_struc_init(&data_packet);


#endif
 
  
  for (i = 0; i < PACKWDSNUM; i++) {RxPkt[i]=codeBuf[i];}
  data_packet.PACKET_RATE=rate;
  
#ifndef DTX
 #if !MOT_SUPPRESSED_EIGHTH_RATE
   if (rate==0) rate=0xE;
 #endif
#endif  
  
  
  
#ifdef WMOPS_FX
  Nmove16(4);
#endif
  PktPtr[0]=16;PktPtr[1]=0;
  PACKET_RATE=data_packet.PACKET_RATE;
  rcelp_half_rateD=0;
  chk_data=0;
  tty_dec_flag=0;
  BAD_RATE=0;
  SPL_HCELP=0;
  SPL_HPPP=0;
  SPL_HNELP=0;
  // Check for Erasure - from codeBuf or from packet.cc or from external ctrl
#ifdef WMOPS_FX
  test();
  move16();
#endif

#if defined(DTX) || MOT_SUPPRESSED_EIGHTH_RATE
// This part addded to original Qualcomm DTX
static Word16 DTX_STATE = 0;

   if ((rate == 4) || (rate ==3) || (rate==2))
					DTX_STATE =0;
   else if (rate==1) DTX_STATE=1;
  
  if ((rate==0) || (rate==0xE))
    {
      if ((lastrateD ==4) || (lastrateD ==3) ||(lastrateD ==2))
	            rate=0xE;
	  
      else if ((lastrateD ==1) || (lastrateD ==0))
					rate=0;
      else
			{
				if (DTX_STATE ==1)rate=0;
				else rate=0xE;
	         }
    }

  data_packet.PACKET_RATE=rate;
  PACKET_RATE=data_packet.PACKET_RATE;
#endif

    
# ifdef DTX
    unsigned int Isil=0;
    unsigned char tmpbuf[2]={0,0};
    tmpbuf[1] = RxPkt[0]>> 8; //upper 8 bits => LSF
    tmpbuf[0] = RxPkt[0] & 0x00FF;//lower 8 bits => power
    short rt;
    rt = (short) data_packet.PACKET_RATE;
    if (rt == 1) SILFrame = 1;
    else
      SILFrame=0;
    Isil = SB_dec.Decode(&rt, tmpbuf);    
    rate= rt;
   
    if (rate !=0)
      {
	RxPkt[0] = (tmpbuf[1] << 8) | tmpbuf[0];
	data_packet.PACKET_RATE =PACKET_RATE =rate; //erasure
      codeBuf[0]=RxPkt[0];
      }
    else
      rate= data_packet.PACKET_RATE =PACKET_RATE =0xE; //erasure
      
#endif

#ifdef VOIP_DORA
  if (rate==0xE && !do_phase_match) { //erasure 
  
#else
  if (rate==0xE) { //erasure 
#endif
    ones_dec_cnt=0;
    chk_data=chk_data_last;
    data_buf[0]=header_last;
    N_consec_ers++;
    for (i=1; i<DATA_SIZE+5;i++)
      data_buf[i]=-1;
    erasureFlag = 1;

    if (chk_data==0) //prev frame was speech
#ifdef VOIP_DORA
       fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD,run_length, phase_offset,time_warp_fraction, obuf_len);
#else
       fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD);
#endif
         
  }
  else // no erasure 
    {
      
    erasureFlag = 0;
    if (PACKET_RATE == 3) //half rate
      {
        // For TTY/DTMF , reading in the data_buf
	/* check for TTY/DTMF */
	for(i=0;i<DATA_SIZE;i++)
	  data_buf[i]=0;
        chk_data= TTY_DTMF_unpack(RxPkt, PktPtr, data_buf);
        if (chk_data == 1)
	  N_consec_ers=0;
      }
    
    for (i = 0; i < PACKWDSNUM; i++) {RxPkt[i]=codeBuf[i];}
    PktPtr[0]=16;PktPtr[1]=0;
  
    //printf ("\n Unpacking starting \n");

    if ((chk_data !=1) && (dtmf_digit_last==-1)) // speech case , no data 
      {
	
	if ((PACKET_RATE==2)||(PACKET_RATE==4))
	  BitUnpack((short *)&data_packet.MODE_BIT,(unsigned short *)RxPkt,1,PktPtr);
   
	if (PACKET_RATE == 3) //SPL packet detector 
	  {
	    //chk for spl. 1/2 rate CELP and PPP
        
	    BitUnpack(&poss_spl_hcelp,(unsigned short *)RxPkt,7,PktPtr);
	    BitUnpack(&poss_qnelp,(unsigned short *)RxPkt,2,PktPtr);
	    if (poss_spl_hcelp == spl_hcelp)
	      {
		if (poss_qnelp == 0)
		  {  
		    BitUnpack((short *) &data_packet.MODE_BIT ,(unsigned short *)RxPkt,1,PktPtr);
	        
		    if (data_packet.MODE_BIT == 0) //unpacking for SPL_HCELP
		      {
			BitUnpack((short *) &data_packet.LSP_IDX[0] ,(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[1] ,(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[2] ,(unsigned short *)RxPkt,9,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[3] ,(unsigned short *)RxPkt,7,PktPtr);
	    
			for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.ACBG_IDX[i] ,(unsigned short *)RxPkt,3,PktPtr);

			BitUnpack((short *) &data_packet.DELAY_IDX ,(unsigned short *)RxPkt,7,PktPtr);
			data_packet.PACKET_RATE=4;
			PACKET_RATE=data_packet.PACKET_RATE;
			SPL_HCELP=1;
		      }
		    else //unpacking for SPL_HPPP
		      {
	       
			BitUnpack((short *) &data_packet.LSP_IDX[0] ,(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[1] ,(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[2] ,(unsigned short *)RxPkt,9,PktPtr);
			BitUnpack((short *) &data_packet.LSP_IDX[3] ,(unsigned short *)RxPkt,7,PktPtr);
			BitUnpack((short *) &data_packet.F_ROT_IDX,(unsigned short *)RxPkt,7,PktPtr);
			BitUnpack((short *) &data_packet.A_AMP_IDX[0],(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.A_AMP_IDX[1],(unsigned short *)RxPkt,6,PktPtr);
			BitUnpack((short *) &data_packet.A_AMP_IDX[2],(unsigned short *)RxPkt,8,PktPtr);
			BitUnpack((short *) &data_packet.A_POWER_IDX,(unsigned short *)RxPkt,8,PktPtr);
			BitUnpack((short *) &data_packet.DELAY_IDX,(unsigned short *)RxPkt,7,PktPtr);
			for (i=0;i<NUM_FIXED_BAND;i++) {
			  if (i<3)
			    data_packet.F_ALIGN_IDX[i]= 0x10;
			  else
			    data_packet.F_ALIGN_IDX[i]= 0x20;
			}       
			data_packet.PACKET_RATE=4;
			PACKET_RATE=data_packet.PACKET_RATE;
			SPL_HPPP=1;
		      }
		  }
		else // for poss_qnelp, unpacking for SPL_HNELP
		  {
		    BitUnpack((short *)&data_packet.LSP_IDX[0],(unsigned short*)RxPkt,8,PktPtr);
		    BitUnpack((short *)&data_packet.LSP_IDX[1],(unsigned short*)RxPkt,8,PktPtr);
                    BitUnpack((short *)&data_packet.NELP_GAIN_IDX[0][0],(unsigned short*)RxPkt,5,PktPtr);
		    BitUnpack((short *)&data_packet.NELP_GAIN_IDX[1][0],(unsigned short*)RxPkt,6,PktPtr);
		    BitUnpack((short *)&data_packet.NELP_GAIN_IDX[1][1],(unsigned short*)RxPkt,6,PktPtr);
		    BitUnpack((short *)&data_packet.NELP_FID,(unsigned short*)RxPkt,2,PktPtr);
		    data_packet.PACKET_RATE =2;
		    PACKET_RATE=data_packet.PACKET_RATE;
		    SPL_HNELP=1;
		  }
	      }      
	
	    else {// not a spl. 1/2 rate , restore RxPkt
	      for (i = 0; i < PACKWDSNUM; i++) {RxPkt[i]=codeBuf[i];}
	      PktPtr[0]=16;PktPtr[1]=0;
	    }
	  } // SPL packet detector 
 
     //printf ("\n Unpacking ...... \n");

	if  (((LAST_PACKET_RATE_D ==1) || ((LAST_PACKET_RATE_D ==2) && (LAST_MODE_BIT_D==0))) && (SPL_HCELP ==1)) //prev was QNELP/Sil
	  {
	    ones_dec_cnt=0;
	    dec_lsp_vq_28_fx((short *) data_packet.LSP_IDX, OldlspD_fx);
#if 1 
	    PACKET_RATE=data_packet.PACKET_RATE=0xE;//erasure , bug-fix in the way the next frame is processed
            rate=0xE;
#endif
	    
#ifdef  VOIP_DORA        
		fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD,run_length,phase_offset,time_warp_fraction, obuf_len);
#else
	    fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD);
#endif

	    /* Use curr frames good LSP if curr frame is spl hcelp and prev frame was QNELP/Sil */ 
	  }
  
	else //do bad-rate check for speech
	  {
      
	    //initialize zrbit for bad-rate detection
	    zrbit[0]=1;
    
	    //unpacking routine
	    switch(data_packet.PACKET_RATE) {
	    case 1:
	      //silence
	      BitUnpack((short *)&data_packet.LSP_IDX[0],(unsigned short*)RxPkt,4,PktPtr);
	      BitUnpack((short *)&data_packet.LSP_IDX[1],(unsigned short*)RxPkt,4,PktPtr);
	      BitUnpack((short *)&data_packet.SILENCE_GAIN,(unsigned short*)RxPkt,NUM_EQ_BITS,PktPtr);
	      BitUnpack(zrbit,(unsigned short*)RxPkt,1,PktPtr); //15th bit
           
	      //do bad-rate check
	      BAD_RATE = bad_rate_check(0,codeBuf);
	      
	      break;
	    case 2:
#ifdef WMOPS_FX
  test();
#endif
  

              if (SPL_HNELP==0)
		 {
		   if(data_packet.MODE_BIT==0){
		     //NELP
		     BitUnpack((short *)&data_packet.LSP_IDX[0],(unsigned short*)RxPkt,8,PktPtr);
		     BitUnpack((short *)&data_packet.LSP_IDX[1],(unsigned short*)RxPkt,8,PktPtr);
		     BitUnpack((short *)&data_packet.NELP_GAIN_IDX[0][0],(unsigned short*)RxPkt,5,PktPtr);
		     BitUnpack((short *)&data_packet.NELP_GAIN_IDX[1][0],(unsigned short*)RxPkt,6,PktPtr);
		     BitUnpack((short *)&data_packet.NELP_GAIN_IDX[1][1],(unsigned short*)RxPkt,6,PktPtr);
		     BitUnpack((short *)&data_packet.NELP_FID,(unsigned short*)RxPkt,2,PktPtr);
		     BitUnpack(zrbit,(unsigned short*)RxPkt,4,PktPtr); // no free bits
		     //do bad-rate check
		     BAD_RATE = bad_rate_check(1,codeBuf);
		   }
		   else if(data_packet.MODE_BIT==1){
		     //QPPP
		     BitUnpack((short *)&data_packet.LSP_IDX[0],(unsigned short *)RxPkt,10,PktPtr);
		     BitUnpack((short *)&data_packet.LSP_IDX[1],(unsigned short *)RxPkt,6,PktPtr);
		     BitUnpack((short *)&data_packet.Q_delta_lag,(unsigned short *)RxPkt,4,PktPtr);
		     BitUnpack((short *)&data_packet.AMP_IDX[0],(unsigned short *)RxPkt,6,PktPtr);
		     BitUnpack((short *)&data_packet.AMP_IDX[1],(unsigned short *)RxPkt,6,PktPtr);
		     BitUnpack((short *)&data_packet.POWER_IDX,(unsigned short *)RxPkt,6,PktPtr);
		     BitUnpack(zrbit,(unsigned short*)RxPkt,1,PktPtr); //40th bit
		     //do bad-rate check
		     BAD_RATE = bad_rate_check(2,codeBuf);
                 
		   }
		 }

	      break;
	    case 3:
	      //Half Rate Celp
	      BitUnpack((short *) &data_packet.DELAY_IDX ,(unsigned short *)RxPkt,7,PktPtr);
	      BitUnpack((short *) &data_packet.LSP_IDX[0] ,(unsigned short *)RxPkt,7,PktPtr);
	      BitUnpack((short *) &data_packet.LSP_IDX[1] ,(unsigned short *)RxPkt,7,PktPtr);
	      BitUnpack((short *) &data_packet.LSP_IDX[2] ,(unsigned short *)RxPkt,8,PktPtr);
      
	      for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.ACBG_IDX[i] ,(unsigned short *)RxPkt,3,PktPtr);
	      for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.FCB_PULSE_IDX[i][0] ,(unsigned short *)RxPkt,10,PktPtr);
	      for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.FCBG_IDX[i] ,(unsigned short *) RxPkt,4,PktPtr);
	      //do bad-rate check
	      BAD_RATE = bad_rate_check(3,codeBuf);
       
	      break;
	    case 4:
#ifdef WMOPS_FX
  test();
#endif
              if (data_packet.MODE_BIT==0){
		if (SPL_HCELP ==0)
		  {
		    //FCELP
		    BitUnpack((short *) &data_packet.DELAY_IDX ,(unsigned short *)RxPkt,7,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[0] ,(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[1] ,(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[2] ,(unsigned short *)RxPkt,9,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[3] ,(unsigned short *)RxPkt,7,PktPtr);
	
		    BitUnpack((short *) &data_packet.DELTA_DELAY_IDX ,(unsigned short *)RxPkt,5,PktPtr);
		    for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.ACBG_IDX[i] ,(unsigned short *)RxPkt,3,PktPtr);
		    for(i=0;i<NoOfSubFrames;i++) {
		      BitUnpack((short *) &data_packet.FCB_PULSE_IDX[i][0] ,(unsigned short *)RxPkt,16,PktPtr);
		      BitUnpack((short *) &data_packet.FCB_PULSE_IDX[i][1] ,(unsigned short *)RxPkt,16,PktPtr);
		      BitUnpack((short *) &data_packet.FCB_PULSE_IDX[i][2] ,(unsigned short *)RxPkt,3,PktPtr);
		    }

		    for(i=0;i<NoOfSubFrames;i++) BitUnpack((short *) &data_packet.FCBG_IDX[i] ,(unsigned short *) RxPkt,5,PktPtr);
		  } // for SPL_HCELP
		//do bad-rate check
		BAD_RATE = bad_rate_check(5,codeBuf); //do this for spl. packet also
	      }     
      
	      else if(data_packet.MODE_BIT==1){
		if (SPL_HPPP ==0)
		  {
		    //FPPP
		    BitUnpack((short *) &data_packet.DELAY_IDX,(unsigned short *)RxPkt,7,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[0] ,(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[1] ,(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[2] ,(unsigned short *)RxPkt,9,PktPtr);
		    BitUnpack((short *) &data_packet.LSP_IDX[3] ,(unsigned short *)RxPkt,7,PktPtr);
	
		    BitUnpack((short *) &data_packet.F_ROT_IDX,(unsigned short *)RxPkt,7,PktPtr);
		    BitUnpack((short *) &data_packet.A_AMP_IDX[0],(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.A_AMP_IDX[1],(unsigned short *)RxPkt,6,PktPtr);
		    BitUnpack((short *) &data_packet.A_AMP_IDX[2],(unsigned short *)RxPkt,8,PktPtr);
		    BitUnpack((short *) &data_packet.A_POWER_IDX,(unsigned short *)RxPkt,8,PktPtr);
		    for (i=0;i<NUM_FIXED_BAND;i++) {
#ifdef WMOPS_FX
  test();
#endif
                    if (i<3)
		      BitUnpack((short *) &data_packet.F_ALIGN_IDX[i],(unsigned short *)RxPkt,5,PktPtr);
		    else
		      BitUnpack((short *) &data_packet.F_ALIGN_IDX[i],(unsigned short *)RxPkt,6,PktPtr);
		    }
		  } // for SPL_HPPP
		//do bad-rate check
		BAD_RATE = bad_rate_check(4,codeBuf);
	  
	      }
	    
	      break;
	    } // for case
	    	    
#if MOT_SUPPRESSED_EIGHTH_RATE
    if (PACKET_RATE == 1 && !BAD_RATE)
      {
      if (consec_eighth_rates < 3) // Most likely in DTX
	{
	consec_eighth_rates = L_add(consec_eighth_rates, 0x00000001L);;
      // Decode packet to update bgn estimate before re-encoding
      silence_decoder_fx(outFbuf_fx, 1, 0);
      // Update bgn estimate
      update_bbg_estimate(outFbuf_fx, PACKET_RATE, 0x7fff);
      PACKET_RATE = 0;
      }
      }
    else if (PACKET_RATE == 0)
      {
      consec_eighth_rates = 0;
      }

    if (PACKET_RATE == 0) //suppressed eighth rate
      {
	//printf("MOTO DTX ON\n");
	
//      consec_eighth_rates = 0;
      gen_encode_bbg(codeBuf);
//      for (i = 0; i < PACKWDSNUM; i++) {RxPkt[i]=codeBuf[i];}
      // After this, process as eighth rate.
      PACKET_RATE = data_packet.PACKET_RATE = 1;
      update_bbg_flag = 0;

      // Reset PktPtr
      for (i = 0; i < PACKWDSNUM; i++) {RxPkt[i]=codeBuf[i];}
      PktPtr[0]=16;PktPtr[1]=0;
      BitUnpack((short *)&data_packet.LSP_IDX[0],(unsigned short*)RxPkt,4,PktPtr);
      BitUnpack((short *)&data_packet.LSP_IDX[1],(unsigned short*)RxPkt,4,PktPtr);
      BitUnpack((short *)&data_packet.SILENCE_GAIN,(unsigned short*)RxPkt,NUM_EQ_BITS,PktPtr);
      BitUnpack(zrbit,(unsigned short*)RxPkt,1,PktPtr); //15th bit
      //do bad-rate check
      BAD_RATE = bad_rate_check(0,codeBuf);
      }
    else
      {
      update_bbg_flag = 1;
      }
#endif
   
	    if (!BAD_RATE)  //speech proc, not bad-rate and not TTY
	      {
		N_consec_ers=0;
		fer_counter--;
    
#ifdef WMOPS_FX
  test();
  move16();
#endif
                if (fer_counter<0) fer_counter=0;
   
		//Translate the PACKET_RATE to the appropriate bit_rate and PPP_MODE
		switch(PACKET_RATE) {
		case 1:
#ifdef WMOPS_FX
  move16();
  move16();
#endif
                  bit_rate=local_rate=1;
		  break;
		case 2:
      		  MODE_BIT=data_packet.MODE_BIT;
#ifdef WMOPS_FX
  test();
#endif
                  if (MODE_BIT==0)
		    bit_rate=local_rate=2;
		  else {
#ifdef WMOPS_FX
  Nmove16(3);
#endif
                    bit_rate=local_rate=3;
		    PPP_MODE_D_fx=0;
		    data_packet.MODE_BIT=1;data_packet.PACKET_RATE=2;
		  }
		  break;
		case 3:
#ifdef WMOPS_FX
  move16();
  move16();
#endif
                  bit_rate=local_rate=3;
		  rcelp_half_rateD=1;   //Half-rate CELP
		  break;
		case 4:
      
		  MODE_BIT=data_packet.MODE_BIT;
#ifdef WMOPS_FX
  test();
#endif
                  if (MODE_BIT==0)
		    bit_rate=local_rate=4;
		  else {
#ifdef WMOPS_FX
  Nmove16(3);
#endif
                    bit_rate=local_rate=3;
		    PPP_MODE_D_fx=1;
		    data_packet.MODE_BIT=1;data_packet.PACKET_RATE=4;
		  }
		  break;

		default:
		  bit_rate=0xE;
		    
		} //end of switch
#ifdef VOIP_DORA

  //If not full-rate, disable Phase Matching
  if (((bit_rate != 4  && rcelp_half_rateD == 1)||(bit_rate < 3)) && do_phase_match == 1)
  {
	  do_phase_match = 0;
	  if (phase_offset == -1)
	  {
		rate = 0xE;
		printf ("\n going back to Lable 1 %d", bit_rate);
		go_back_input = 1;
	    goto Lable;
	  }
	  //printf ("\n 1setting phase_offset to 10 %d", bit_rate);
	  phase_offset = 10;
  }
  
  //printf("\n Regular Decoding \n");

  //Phase Matching: if delay change is big, disable Phase Matching
  if (((bit_rate == 3 && PPP_MODE_D_fx == 'F' && abs_s(data_packet.DELAY_IDX + DMIN - pdelayD_fx) >= 15) || (bit_rate == 4 && abs_s(data_packet.DELAY_IDX + DMIN - pdelayD_fx) >= 15)) && do_phase_match == 1)
  {
	  do_phase_match = 0;
	  if (phase_offset == -1)
	  {
		rate = 0xE;
		//printf ("\n going back to Lable 2 %d %d %f", bit_rate, data_packet.delayindex + DMIN, pdelayD_fx);
		go_back_input = 1;
	    goto Lable;
	  }

	  //if (bit_rate == 3)
		  //printf ("\n 2setting phase_offset to 10 %d %d %f %d", bit_rate, data_packet.DELAY_IDX + DMIN, pdelayD_fx, data_packet.DELAY_IDX + DMIN);
	  //else
		//printf ("\n 2setting phase_offset to 10 %d %d %f", bit_rate, data_packet.DELAY_IDX + DMIN, pdelayD_fx);	
	  phase_offset = 10;
  }
  //If full-rate CELP and delta delay == 0, disable Phase Matching
  if (bit_rate == 4 && data_packet.DELTA_DELAY_IDX == 0 && do_phase_match == 1)
  {
	  do_phase_match = 0;
	  if (phase_offset == -1)
	  {
		rate = 0xE;
		printf ("\n going back to Lable 3 %d %d", bit_rate, data_packet.DELTA_DELAY_IDX);
		go_back_input = 1;
	    goto Lable;
	  }
	  //printf ("\n setting phase_offset to 10 %d", bit_rate);
	  phase_offset = 10;
  }

#endif
      
        
		// Do regular decoding
		switch(bit_rate) {
		case 1:
		  silence_decoder_fx(outFbuf_fx, post_filter, 0);

#ifdef VOIP_DORA
		  //printf ("\n SILENCE decoding %d",decode_fcnt);
	      *obuf_len = 160;
#endif



		  break;
		case 2:
#ifdef VOIP_DORA_NELP		  
		  nelp_decoder_fx(outFbuf_fx, post_filter,0,time_warp_fraction, obuf_len);
		  //printf("\n NELP Decoding \n");
		  
#else
		  nelp_decoder_fx(outFbuf_fx, post_filter,0);
#endif //VOIP_DORA
#ifdef GAIN_PRINT
                printf("%f\n",0.0);
#endif	

		  break;
		case 3:
#ifdef WMOPS_FX
  test();
#endif

          if (rcelp_half_rateD) {
#ifdef VOIP_DORA_CELP
		  celp_decoder_fx(outFbuf_fx,post_filter,bit_rate,phase_offset, run_length,time_warp_fraction, obuf_len);

#ifdef GAIN_PRINT
                printf("%f\n",0.0);
#endif

#else
          celp_decoder_fx(outFbuf_fx,post_filter,bit_rate);
#endif
		  //printf("\n celp Decoding \n");
		  }
#ifdef VOIP_DORA_VOICE		  
		  else {voiced_decoder_fx(outFbuf_fx,post_filter,0,phase_offset, run_length,time_warp_fraction, obuf_len);
		  //printf("\n Voiced Decoding \n");
		 //decode_fcnt++;
		  }
#else
          else {
		  voiced_decoder_fx(outFbuf_fx,post_filter,0);
		  //printf("\n Voiced Decoding \n");
		  }
#endif
    	  break;
		case 4:

#ifdef VOIP_DORA_CELP
		  celp_decoder_fx(outFbuf_fx,post_filter,bit_rate,phase_offset, run_length,time_warp_fraction, obuf_len);

#ifdef GAIN_PRINT
                printf("%f\n",0.0);
#endif


#else		  
		  celp_decoder_fx(outFbuf_fx,post_filter,bit_rate);
		  //printf("\n celp Decoding \n");
#endif
		  break;

		}

		/* Add fading when there is no erasure*/
		FadeScale_fx = (Word16)MIN_FX(16384, add(FadeScale_fx, 9830)); 
		//Back up this frame for potential pitch correction in case of erasures
#ifdef WMOPS_FX
  test();
#endif
                if (fer_counter==2) {
		  pdelayD3_fx=pdelayD2_fx;
		  for (i=0;i<ACBMemSize;i++) PitchMemoryD3_fx[i]=PitchMemoryD2_fx[i];
		}
#if MOT_SUPPRESSED_EIGHTH_RATE
	    if (update_bbg_flag == 1) //not suppressed eighth rate
	      {
	      update_bbg_estimate(outFbuf_fx, PACKET_RATE, 0x7fff);
	      }
#endif
	      }//for speech proc
	    else
	      {
		// bad-rate detecte
		N_consec_ers++;
#if 1 
		PACKET_RATE=data_packet.PACKET_RATE=0xE;//erasure , bug-fix in the way the next frame is processed
#endif	
#ifdef  VOIP_DORA        
		fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD,run_length, phase_offset, time_warp_fraction, obuf_len);
#else
	    fer_processing_fx(outFbuf_fx,post_filter,bit_rate=lastrateD);
#endif		
		
	    }
	  } //end of bad-rate check 
      } //end of check_data  
    
    else
      {
	InitDecoder_fx();
        for(i=0;i<FSIZE;i++)
          outFbuf_fx[i]=0;
      } 
	
  } //else for no erasure

  /*-------------------------------------------------------------------*/
  /*                       Generate DTMF, TTY tones                    */
  /*-------------------------------------------------------------------*/
	 
  /*-------------------------------------------------------------------*/
  /*                    Zero out speech if only data or               */
  /*                     first frame after dtmf                        */
  /*-------------------------------------------------------------------*/
  
  if (chk_data == 0) 
    data_buf[0] = -1;
  

  if (chk_data == 1 ) 
    for(i=0;i<FSIZE;i++)
      outFbuf_fx[i]=0;
  
  for (i = 0; i < FSIZE; i++)
    sigout_16[i] = outFbuf_fx[i];

  DTMF=0;
  if (chk_data >= 1) // curr frame is TTY/DTMF
    {
       
      if (data_buf[0] == TTY_HEADER) 
	       {
		 tty_dec_header = data_buf[1]&0x00ff;
		 tty_type = data_buf[1]>>8;
		 tty_dec_char = data_buf[2];
		 
		 tty_dec_baud_rate = data_buf[3];
		 tty_dec_flag = tty_dec (sigout_16, 0, tty_dec_header,
					 tty_dec_char, tty_dec_baud_rate, 
					 erasureFlag, /*Erasure flag */
					 0,   /* Subframe index */
					 1,   /* Num of subframes */
					 FSIZE);
		
		 if (tty_dec_flag !=0)
		   for (i = 0; i < FSIZE; i++)
		     outFbuf_fx[i] = sigout_16[i];
	       }


             /*---------------------------------------------------------------*/
	     /*   If 0<N_consec_ers<3 and were generating DTMF tone, continue tone   */
	     /*---------------------------------------------------------------*/

      if ((N_consec_ers>0)&&(N_consec_ers<3)&&(dtmf_digit_last>0)) 
	{
	  data_buf[0] = DTMF_HEADER;
	  data_buf[1] = dtmf_digit_last;
	}
	     
      /*---------------------------------------------------------------*/
      /*    If more than 2 bad frames, stop generating dtmf tones      */
      /*---------------------------------------------------------------*/

      if  ((data_buf[0] == DTMF_HEADER)&&(N_consec_ers<3))
	{
	  /*-----------------------------------------------------------*/
	  /*                       Get dtmf_digit                      */
	  /*-----------------------------------------------------------*/

	  dtmf_digit = data_buf[1];
	  if (dtmf_digit > 0) 
	    {
	      /*-------------------------------------------------------*/
	      /*                  Generate dtmf tones                  */
	      /*-------------------------------------------------------*/
	      dtmf_tone_gen(dtmf_digit,dtmf_digit_last,FSIZE,outFbuf_fx);
		      		      
	    } 
	  else 
	    {
	      /*-------------------------------------------------------*/
	      /*              Silence frame after dtmf tone            */
	      /*-------------------------------------------------------*/

	      for(i=0;i<FSIZE;i++)
		outFbuf_fx[i]=0;
	    }
	  dtmf_digit_last = dtmf_digit;
	  DTMF=1;
	}
      else
	{
	  dtmf_digit_last = -1; //nore than 2 bad frames
	  DTMF=1;
	}
      
	  
    }
  else 
    {
      tty_dec_flag = tty_dec(sigout_16, 1, tty_dec_header, tty_dec_char, tty_dec_baud_rate, erasureFlag,
			             0,              /* Subframe index */
				     1,              /* Num of subframes */
				     FSIZE);
      dtmf_digit_last = -1; //if non-dtmf frame then set dtmf_digit_last to
      //prevent skipping speech decoder for subsequent frames
      if (tty_dec_flag !=0)
	for (i=0; i<FSIZE; i++)
	  outFbuf_fx[i] = sigout_16[i];
    }

  
  if ((tty_dec_flag == 0) && (DTMF==0)) // speech 
    {
   
      /* update decoder variables */
      for (i=0; i<LPCORDER; i++) {
	OldOldlspD_fx[i] = OldlspD_fx[i];
	OldlspD_fx[i] = lsp_fx[i];
      }
#ifdef WMOPS_FX
  Nmove16(4);
#endif
  prev_SILFrame=SILFrame;
  last_last_last_rateD = last_last_rateD;
  last_last_rateD=lastrateD;
  lastrateD = bit_rate;
  prev_rcelp_half=rcelp_half_rateD;
  LAST_PACKET_RATE_D = PACKET_RATE;
  LAST_MODE_BIT_D = MODE_BIT;
  prev_prev_frame_error = prev_frame_error;
  prev_frame_error=((rate==0xE||errorFlag==1)||(BAD_RATE==1));
#ifdef WMOPS_FX
  WMP_fwc_fx("Finish Rest of the Decode process ........................");
#endif
    }
  decode_fcnt++;
  chk_data_last = chk_data;
  header_last = data_buf[0];
 
  
}
  


  
  
