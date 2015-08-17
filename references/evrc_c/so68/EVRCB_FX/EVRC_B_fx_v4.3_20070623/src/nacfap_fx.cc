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

#include"dsp_math.h"

#include "typedef_fx.h"
#include "basic_op40.h"
#include "basic_op.h"
#include "proto_fx.h"
#include "macro_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX

static Word16 rlpf_coef[RLPF_ORDER+1]={
  223,497,1293,2555,3971,5088,5513,5088,3971,2555,1293,497,223};  //Q15



/*=================================================================== */
/* FUNCTION      :  PIT_GetNormXCorr_fx ().                           */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes sign(Exy)*Exy*Exy/Exx*Eyy  */
/*                  given Exx, Eyy, Exy and their corresponding      */ 
/*	            scaling factors.                                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      Exy: Cross-Correlation between x and y Q0 */
/*         _ (Word16)      Exy_scale: scaling factor of Exy Q0       */
/*         _ (Word16)      Exx: Dot-Product of x                     */
/*         _ (Word16)      Exx_scale: scaling factor of Exx Q0       */
/*         _ (Word16)      Eyy: Dot-Product of y                     */
/*         _ (Word16)      Eyy_scale: scaling factor of Eyy Q0       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word16)      sign(Exy)*Exy*Exy/Exx*Eyy  Q15            */
/*===================================================================*/


Word16 PIT_GetNormXCorr_fx(Word16 Exy, Word16 Exy_scale, Word16 Exx,
			   Word16 Exx_scale, Word16 Eyy, Word16 Eyy_scale)

{
  
  Word32 Ltemp1=0,Ltemp2=0;
  
  Word16 n,corr;
  
#ifdef WMOPS_FX
  test();
#endif
  if (Exy==0) return 0;
  
  Ltemp2=L_mult(Exy,abs_s(Exy));
  
    
  Ltemp1=L_mult(Exx,Eyy);
    
 
  
  /****  Effective norm fac  ****/
  
  n=sub(shl(Exy_scale,1),add(Eyy_scale,Exx_scale));
  
  //corr=round32_16((Word32) L_sat32_40(divide_dp(Ltemp2,Ltemp1,2-n2))); //Q15

  corr=extract_h((Word32) L_sat32_40(L_add40(divide_dp(Ltemp2,Ltemp1,2-n),0x00008000))); //Q15
    
  return corr;
}



/*=================================================================== */
/* FUNCTION      :    get_corr_fx ()                                  */         
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes the cross correlation     */
/*                  between  segments of the input signal, for the   */
/*		    lag range = 8 - 61                               */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16 [])   lpfr : Signal input in Q0                 */
/*	   _ (Word16)      posn: specifies the segment of the        */
/*	                   input signal for nacf computation  Q0     */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16 [])   corr_res : cross-correlation              */
/*                         coefficients for the lag range = 8 - 61   */
/*			   Q15                                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None                                 */
/*===================================================================*/

void get_corr_fx(Word16 lpfr[],Word16 posn, Word16 corr_res[D_RANGE])
{
  

   Word16 Exy[D_RANGE],Eyy[LPFRMEM_LEN];
   Word16 Eyy_scale[LPFRMEM_LEN],Exy_scale[D_RANGE],n;
   Word16 i,j,k=0;
   Word40 Ltemp_40;
     
#ifdef WMOPS_FX
	  move16();
	  move32();
#endif
   Eyy[0]=0;
   Ltemp_40= (Word40) 0;
   for (i=0;i<(FSIZE/SUBSAMP/2);i++)   // FSIZE/SUBSAMP/2=40
   {
	   Ltemp_40 = L_mac40(Ltemp_40, lpfr[i+posn], lpfr[i+posn]);
   }
   
   n=norm32_l40(Ltemp_40);
   Eyy[0]=round32_16((Word32) L_shl40(Ltemp_40,n));

   n =(Ltemp_40==0)?31:n;
#ifdef WMOPS_FX
	  move16();
#endif
   Eyy_scale[0] = n;

  /*   Compute the energy of lpfr by adding the newest sample      */
  /*   and subtracting the oldest sample                           */
	
	for (i=1;i<LPFRMEM_LEN;i++,k++){
	Ltemp_40 = L_mac40 (Ltemp_40,lpfr[posn-1-k], lpfr[posn-1-k]);
	Ltemp_40 = L_msu40 (Ltemp_40, lpfr[posn+(FSIZE/2/SUBSAMP)-1-k], lpfr[posn+(FSIZE/2/SUBSAMP)-1-k]);
	 
	n = norm32_l40(Ltemp_40) ;
	 
	Eyy[i] = round32_16((Word32) L_shl40(Ltemp_40,n));
	n =(Ltemp_40==0)?31:n;
	
#ifdef WMOPS_FX
	  move16();
#endif
	Eyy_scale[i] = n ;
	 
	}

        
	for (i=(DMIN-4)/SUBSAMP,k=0;i<(DMAX+2)/SUBSAMP;i++,k++) {
		Ltemp_40= (Word40) 0;
		for (j=0;j<40;j++) Ltemp_40 = L_mac40(Ltemp_40, lpfr[posn+j],lpfr[posn-i+j]);
		Exy_scale[k] =norm32_l40(Ltemp_40);
		Exy[k] = round32_16((Word32) (L_shl40(Ltemp_40,Exy_scale[k])));
		Exy_scale[k] =(Ltemp_40==0)?31:Exy_scale[k];
	}

	//Eyy[0]=((Eyy[0]==0)?1:Eyy[0]);


	for (i=0,j=(DMIN-4)/SUBSAMP;i<D_RANGE;i++,j++)
	{
		//Eyy[j]=((Eyy[j]==0)?1:Eyy[j]);

		/* compute the cross correlation for lag range = 8 - 61 */

		corr_res[i]=PIT_GetNormXCorr_fx(Exy[i],Exy_scale[i],
				Eyy[0],Eyy_scale[0],Eyy[j],Eyy_scale[j]);

	}
	

}


/*===================================================================*/
/* FUNCTION      :   get_max_corr_fx ()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function computes the maxima in the         */
/*                  cross correlation between segments of the input  */
/*		    signal in the specified lag range around         */
/*		    the pitch lag                                    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16 [])     lpfr : Signal input in Q0               */
/*         _ (Word16)        posn : specifies the segment of the     */
/*	                     input signal for nacf computation  Q0   */
/*         _ (Word16)        pitch : pitch lag Q0                    */
/*         _ (Word16)        ll : lower lag limit  Q0                */
/*         _ (Word16)        ul : higher lag limit Q0                */
/*------------------------------------------------------------------ */
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */ 
/*         _ (Word16)        Maxima in the cross-correlation Q15     */
/*===================================================================*/


Word16 get_max_corr_fx(Word16 lpfr[],Word16 posn,Word16 pitch,Word16 ll,Word16 ul)
{
  
  
    Word16 i,j;
    Word16 Exx,Eyy,Exy,Exy_f,maxcorr;
    Word16 Exx_scale,Eyy_scale,Exy_scale;
    Word40 Ltemp_40;
    Word16 temp;
    Word32 Ltemp1,Ltemp2;
    Word16 Exy_best=0, Exy_best_scale=0 ;
    Word16 Eyy_best=0, Eyy_best_scale=0 ;
    Word16 n;
            
    Ltemp_40= (Word40) 0;
    for (i=0;i<(FSIZE/SUBSAMP/2);i++)   // FSIZE/SUBSAMP/2=40
      {
	Ltemp_40 = L_mac40(Ltemp_40, lpfr[i+posn], lpfr[i+posn]);
      }
	Exx_scale = norm32_l40(Ltemp_40);
	Exx=round32_16((Word32) L_shl40(Ltemp_40,Exx_scale));
	Exx_scale=(Ltemp_40==0)?31:Exx_scale;
	
        //Exx=(Exx==0)?1:Exx;
    
#ifdef WMOPS_FX
	  Nmove16(2);
#endif
     Exy_best=0x8000;
     Eyy_best=1; 
    for (i=ll;i<ul;i++) {
      Ltemp_40= (Word40) 0;
      for (j=0;j<(FSIZE/SUBSAMP/2);j++)           // FSIZE/SUBSAMP/2=40
       {
        Ltemp_40 = L_mac40(Ltemp_40, lpfr[posn+j-i-shr(pitch,1)],lpfr[posn+j-i-shr(pitch,1)]);
       }
       Eyy_scale = norm32_l40(Ltemp_40);
       Eyy = round32_16((Word32)L_shl40(Ltemp_40,Eyy_scale));
	Eyy_scale=(Ltemp_40==0)?31:Eyy_scale;       
     Ltemp_40= (Word40) 0;
                   
     for (j=0;j<(FSIZE/SUBSAMP/2);j++)           // FSIZE/SUBSAMP/2=40
       {
        Ltemp_40 = L_mac40(Ltemp_40, lpfr[posn+j],lpfr[posn+j-i-shr(pitch,1)]);
       }
       Exy_scale = norm32_l40(Ltemp_40);
       Exy = round32_16((Word32) L_shl40(Ltemp_40,Exy_scale));
       Exy_scale=(Ltemp_40==0)?31:Exy_scale;  	
       
       Eyy=(Eyy==0)?1:Eyy;
  
      
      /* Computing Exy * abs(Exy) * Eyy_best */

       temp = mult_r(Exy,abs_s(Exy)) ;
       Ltemp1 = L_mult(temp, Eyy_best) ;

     /* Computing Eyy * abs(Exy_best) * Exy_best */

       Ltemp2 = L_mult(Eyy, Exy_best) ;

     /* Getting the net normalization factor for Ltemp2 */

       n =  add(shl(sub(Exy_best_scale,Exy_scale),1),sub(Eyy_scale,Eyy_best_scale)) ;
                     
      
       //if((n < 0) && (Ltemp2!=0))
#ifdef WMOPS_FX
  test();
#endif
       if (n < 0) 
	  Ltemp1=L_shl(Ltemp1,n);
       else
          Ltemp2=L_shr(Ltemp2,n);
       
       
     /* See if Exy*abs(Exy)*Eyy_best > Eyy*abs(Exy_best)*Exy_best */

        
       //if (i==ll || (Ltemp1 > L_shr(Ltemp2,n))) {
       // if (Ltemp1 > L_shr(Ltemp2,n)) {
#ifdef WMOPS_FX
  test();
#endif
       if (Ltemp1 > Ltemp2) {
	      Exy_best= temp;
	      Exy_f=Exy;
	      Exy_best_scale = Exy_scale ;
	      Eyy_best = Eyy ;
	      Eyy_best_scale = Eyy_scale ;
	      
	    }

    }
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (Exy_f<0)  Exy_f=0;
    
    maxcorr=PIT_GetNormXCorr_fx(Exy_f,Exy_best_scale,Exx,Exx_scale,Eyy_best,Eyy_best_scale);
    
    return(maxcorr);
}



/*===================================================================== */
/* FUNCTION    :   get_nacf_at_pitch_fx                                 */
/*--------------------------------------------------------------------- */
/* PURPOSE     :  This routine computes nacf at pitch for 2 subframes   */
/*                and the last nacf with full search (not pitch based)  */
/*		  on the look-ahead frame.                               */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16[])    resid : Input residual signal Q0          */
/*         _ (Word16[])    pitch : pitch lag Q0                      */

/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16[])    nacf  : nacf values Q15                   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ None                                                    */
/*===================================================================*/


void get_nacf_at_pitch_fx(Word16 resid[], Word16 pitch[], Word16 nacf[])
{
  Word16 i,k;
  static int FirstTime=1;
  Word16 decim_buf[FSIZE+LOOKAHEAD_LEN];
  static Word16 lpfr[LPFRMEM_LEN+(FSIZE+LOOKAHEAD_LEN)/SUBSAMP];
  //  static Word16 rlpf_filt_mem[RLPF_ORDER];
  //  Word16 rlpf_filt_mem_saved[RLPF_ORDER];
  
  Word16 wl,ll,ul;
  Word16 posn;
  Word16 pitch1[3]={pitch[0],pitch[1],0}; 
  

#ifdef WMOPS_FX
  test();
  Nmove16(1+LPFRMEM_LEN);
#endif
  if (FirstTime) {
    FirstTime=0;
    for (i=0;i<LPFRMEM_LEN;i++)
      lpfr[i]=0;
    /*
    for (i=0;i<RLPF_ORDER;i++)
      rlpf_filt_mem[i]=0;
    */  }


  fir_filter_fx(rlpf_coef, resid, decim_buf, RLPF_ORDER,FSIZE,1);

   
  //  for (i=0;i<RLPF_ORDER;i++) rlpf_filt_mem_saved[i]=rlpf_filt_mem[i];
  
  fir_filter_fx(rlpf_coef, resid+FSIZE, decim_buf+FSIZE, RLPF_ORDER,LOOKAHEAD_LEN,1);
  
  //  for (i=0;i<RLPF_ORDER;i++) rlpf_filt_mem[i]=rlpf_filt_mem_saved[i];
   
  for (i=0;i<(FSIZE+LOOKAHEAD_LEN)/SUBSAMP;i++){
      lpfr[i+LPFRMEM_LEN]=decim_buf[SUBSAMP*i];  
     
  }

   
 
  for (k=0;k<3;k++)  {
    // compute nacf around pitch at 2 subframes       
       
#ifdef WMOPS_FX
  test();
  Nmove16(2);
#endif
    if (k==2)
      {
        ll=DMIN/SUBSAMP;
        ul=DMAX/SUBSAMP;
	posn=add(LPFRMEM_LEN,(FSIZE/2+LOOKAHEAD_LEN)/SUBSAMP); //LPFRMEM_LEN+(FSIZE/2+LOOKAHEAD_LEN)/SUBSAMP;
      }
    else
      {
	//wl=mult(add(MAX_FX(6,MIN_FX(mult_r(6554,pitch[k]),16)),1),16384);
        wl=shr(add(MAX_FX(6,MIN_FX(mult_r(6554,pitch[k]),16)),1),1); // shr( ,1) =  /SUBSAMP, macro-substitution


 	

      ll=-wl;ul=wl+1;
      posn=add(LPFRMEM_LEN,extract_l(L_mult(add(k,1),20))); // macro-substitution, FSIZE/SUBSAMP/2 = 40
      
      
      }
   nacf[k]=get_max_corr_fx(lpfr,posn,pitch1[k],ll,ul);
         
  }
 
#ifdef WMOPS_FX
  Nmove16(LPFRMEM_LEN);
#endif
  for (i=0;i<LPFRMEM_LEN;i++) lpfr[i]=lpfr[i+(FSIZE/SUBSAMP)];
  
}

