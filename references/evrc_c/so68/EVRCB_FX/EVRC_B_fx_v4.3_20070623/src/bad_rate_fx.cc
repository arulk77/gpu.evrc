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
#include "proto_fx.h"
#include "globs_fx.h"
#include "basic_op.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "ppvq_fx.h"
extern short lsptab8_fx[160];


Word16 bad_rate_check(Word16 opt,Word16* cbuf)
{
  Word16 i;
  Word16 m0,m1,allzr=0;
  short nsub8[2]={5,5};
  short nsize8[2]={16,16};
 
 switch(opt){
  case 0: //silence
    
    if ((unsigned short) cbuf[0] == (unsigned short) 0xFFFF)
      {
	ones_dec_cnt++;
        if (ones_dec_cnt > 2)
	  {
	    ones_dec_cnt =3;
	    for (i=0;i<ORDER;i++) lsp_fx[i]=OldlspD_fx[i];
	    data_packet.SILENCE_GAIN=0;
	    //setting up structure for clean case silence decoding
	    return(0);
	  }
      }
     else
       ones_dec_cnt=0;
     
     
    for(i=0;i<1;i++) //bug-fix 
       if (cbuf[i]==0) allzr++;

    if (allzr==1) //bug-fix
    {
      printf(" \n ALL ZEROS ER FRAME %d \n",decode_fcnt);
      return(1);
    }
    
     if (zrbit[0]!=0) //1 unused bit
      return(1);

     
     lspmaq_dec_fx8(nsub8,nsize8, lsp_fx, (short int *)data_packet.LSP_IDX,lsptab8_fx); 
     m0=MAX_FX(MAX_FX(MAX_FX(MAX_FX(lsp_fx[0],lsp_fx[1]),lsp_fx[2]),lsp_fx[3]),lsp_fx[4]);
     m1=MIN_FX(MIN_FX(MIN_FX(MIN_FX(lsp_fx[5],lsp_fx[6]),lsp_fx[7]),lsp_fx[8]),lsp_fx[9]);
     if (m0>=m1)
      return(1);
#if 1
     if ((LAST_PACKET_RATE_D == 4) && (prev_frame_error==0))
       return(1);
#endif    
     
     return(0);
      break;
  case 1: //QNELP
     ones_dec_cnt=0;
     for(i=0;i<2;i++)
       if (cbuf[i]==0) allzr++;

     if ((cbuf[2]&(0xFF00)) == 0)
       allzr++;
       
     if (allzr==3)
     {
      printf(" \n ALL ZEROS QNELP FRAME %d \n",decode_fcnt);
      return(1);
    
     }
   
      if (zrbit[0]!=0)
        return(1);
      
      if (data_packet.NELP_FID ==3)
        return(1);
 
      
      return(0);
       
      // maybe check prev frame cannot be FPPP/QPPP
      //info in LAST_PACKET_RATE_D, LAST_MODE_BIT_D
      break;
  case 2:  //QPPP
     ones_dec_cnt=0;
     for(i=0;i<2;i++)
       if (cbuf[i]==0) allzr++;

     if ((cbuf[2]&(0xFF00)) == 0)
       allzr++;
       
     if (allzr==3)
     {
      printf(" \n ALL ZEROS QPPP FRAME %d \n",decode_fcnt);
      return(1);
     }
     
   
    
      if (zrbit[0]!=0)
        return(1); //1 unused bit

      if ((LAST_PACKET_RATE_D ==1) || ((LAST_PACKET_RATE_D ==2)&&(LAST_MODE_BIT_D==0)) &&((prev_frame_error)==0)) //last frame was silence or QNELP and not erased
       return(1);
    

      
      return(0);
      break;
  case 3: //HCELP
     ones_dec_cnt=0;
     for(i=0;i<5;i++)
       if (cbuf[i]==0) allzr++;
        
     if (allzr==5)
     {
      printf(" \n ALL ZEROS HCELP FRAME %d \n",decode_fcnt);
      return(1);
   
     }
          
     if ((data_packet.DELAY_IDX != 120) && (data_packet.DELAY_IDX != 115))
       /*  spl tty lag=120, do speech proc on frames following tty ( not erasure proc)*/
       /*  memories in sync with encoder,  but output tty digit; 115= RESERVED LAG    */ 
     { 
      if (data_packet.DELAY_IDX > 100) 
	return(1);

      dec_lsp_vq_22_fx((short int *)data_packet.LSP_IDX, lsp_fx);
      m0=MAX_FX(lsp_fx[0],MAX_FX(lsp_fx[1],lsp_fx[2]));
      m1=MIN_FX(lsp_fx[6],MIN_FX(MIN_FX(lsp_fx[7],lsp_fx[8]),lsp_fx[9]));
      if (m0>=m1)
	return(1); 
     }
     
      return(0);
      break;
  case 4:    //FPPP
      ones_dec_cnt=0;
      for(i=0;i<10;i++)
     	if (cbuf[i]==0) allzr++;
      
      if ((cbuf[10]&(0xFFE0))==0) allzr++;
      if  (allzr==11)
      {
        printf(" \n ALL ZEROS FPPP FRAME %d \n",decode_fcnt);
	return(1);
      }
           
      dec_lsp_vq_28_fx((short *)data_packet.LSP_IDX, lsp_fx);

      if (SPL_HPPP == 0)
      {
      if (data_packet.DELAY_IDX > 100) // and not eq to special packet pattern
       	return(1);

      
      m0=MAX(lsp_fx[0],lsp_fx[1]);
      m1=MIN(MIN(lsp_fx[4],lsp_fx[5]),lsp_fx[6]);
      if (m0>=m1)
	return(1);

      if ((LAST_PACKET_RATE_D ==1) || ((LAST_PACKET_RATE_D ==2)&&(LAST_MODE_BIT_D==0)) &&((prev_frame_error)==0)) //last frame was silence or QNELP and not erased
       return(1);

      
      if (((LAST_PACKET_RATE_D !=2) && (prev_frame_error ==0)) || ((LAST_PACKET_RATE_D == 2) && (fer_counter == 0))) 
	{
	  delay_fx=add(data_packet.DELAY_IDX,DMIN);
	  if (abs_s(sub(delay_fx,pdelayD_fx))>15){
	       // zer out mems
	      for(i=0;i<ACBMemSize;i++)
	        PitchMemoryD_fx[i]=0;
	      for(i=0;i<10;i++)
	        SynMemory_fx[i]=0;
	      Init_Post_Filter_fx();
	      return(1);
	}
	}
      }
      
      return(0);
      break;
  case 5: //FCELP
      ones_dec_cnt=0;
      for(i=0;i<10;i++)
     	if (cbuf[i]==0) allzr++;
      
      if ((cbuf[10]&(0xFFE0)) ==0) allzr++;
      if  (allzr==11)
      {
        printf(" \n ALL ZEROS FCELP FRAME %d \n",decode_fcnt);
	return(1);
	
      }
    dec_lsp_vq_28_fx((short *)data_packet.LSP_IDX, lsp_fx); // do this for spl_hcelp also

    if (SPL_HCELP==0)
    {   
      if (data_packet.DELAY_IDX > 100)   // and not eq to special packet pattern
	return(1);

      
       m0=MAX(lsp_fx[0],lsp_fx[1]);
       m1=MIN(MIN(lsp_fx[4],lsp_fx[5]),lsp_fx[6]);
       if (m0>=m1)
	 return(1);
    }      
      return(0);
      break;
      
 }
 return(0);
}
      

         
       
       
      
      
  
	  
         
       
       
      
     


     
      

