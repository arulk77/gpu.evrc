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
/*  1/8th rate coder                                                    */
/*======================================================================*/
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "globs_fx.h"
#include "proto_fx.h"
#include "lpcana_tab.h"
# include "lspq_fx.dat"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif
#define MUTE


#ifdef DTX
#include "math.h"

#define LSP_INTERP 1
#define GAIN_SMOOTH 1
#define LSP_SPREAD_FACTOR_FX_ERDTX 983//328=0.01, 655=0.02, 983=0.03, 262=0.008 (other rates) 
#define THRESHOLD_1     2378       //1.16145 in Q11        
#define THRESHOLD_2     2417        //1.1803 in Q11   


void lsp_spread_ERDTX(Word16 *lsp_fx)
{
  Word16 i;
  if (lsp_fx[0]<LSP_SPREAD_FACTOR_FX_ERDTX) lsp_fx[0]=LSP_SPREAD_FACTOR_FX_ERDTX;
      for (i=1; i<LPCORDER; i++)
        if (sub(lsp_fx[i],lsp_fx[sub(i,1)]) < LSP_SPREAD_FACTOR_FX_ERDTX)
          lsp_fx[i]=add(lsp_fx[sub(i,1)],LSP_SPREAD_FACTOR_FX_ERDTX);
      if (sub(16384,lsp_fx[sub(LPCORDER,1)])< shl(LSP_SPREAD_FACTOR_FX_ERDTX,1)) { 
        lsp_fx[sub(LPCORDER,1)]=sub(16384,shl(LSP_SPREAD_FACTOR_FX_ERDTX,1));
        i=sub(LPCORDER,2);
        while ((sub(lsp_fx[i+1],lsp_fx[i])<LSP_SPREAD_FACTOR_FX_ERDTX) && (i>=0)) {
	      lsp_fx[i]=MIN_FX(sub(lsp_fx[i+1],LSP_SPREAD_FACTOR_FX_ERDTX),0);
          i--;
        }
      }
}

#endif //DTX


/*===================================================================*/
/* FUNCTION      :  ran0_fx()                                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Uniform random number generator                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) seed0 : random seed, Q0                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) ran_num : uniform random number, Q15                 */
/*===================================================================*/
Word16 ran0_fx(Word16& seed0)
{
Word32	Ltemp;
#ifdef WMOPS_FX
  move32();
  logic32();
#endif
Ltemp = 0;
Ltemp = L_mac(27698, 25173, seed0);
Ltemp = L_shr(Ltemp, 1);
Ltemp = Ltemp & 0x0000ffffL; 
seed0 = extract_l(Ltemp);
return (extract_h(L_shl(Ltemp,15))); 
}

/*===================================================================*/
/* FUNCTION      :  ran_g_fx()                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Gaussian random number generator                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) extra_flag : 0 -- no extra Gaussian random number    */
/*                           1 -- one extra number for next call     */
/*   _ (Word16) extra_ran : extra Gaussian random number generated   */
/*            in this call, can be fetched immediately by next call  */
/*   _ (Word16) seed0 : seed for uniform random number generator, Q0 */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) ran_num : Gaussian random number, Q13                */
/*===================================================================*/
Word16 ran_g_fx(Word16& seed0, Word16& extra_flag, Word16& extra_ran)
{
  Word16 sv1, sv2, ran, n1, n2;
  Word16 d1h, d1l, d2h, d2l;
  Word32 Ltemp1, Ltemp2, rsq;
  Word40 Lacc;

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (extra_flag==1) {
    extra_flag=0;
    return(extra_ran);
  }
  else {
    sv1 = shl(sub(ran0_fx(seed0), 16384), 1);  // Q15, 2*ran0-1
    sv2 = shl(sub(ran0_fx(seed0), 16384), 1);  // Q15, 2*ran1-1
    Ltemp1=L_shr(L_mult(sv1,sv1),1);  // sv1*sv1, Q30
    Ltemp2=L_shr(L_mult(sv2,sv2),1);  // sv2*sv2, Q30

    rsq=L_add(Ltemp1,Ltemp2);  // Q30
#ifdef WMOPS_FX
  test();
  logic16();
#endif
    if (rsq==0 || L_sub(rsq,0x40000000)>=0) {
      // returns (v1/8+v2/8)+(ran_u0-0.5)/4 
      ran = shr(sv1,3);
      ran = add(ran, shr(sv2,3));
      ran = add(ran, shr(sub(ran0_fx(seed0), 16384),2));
      return (ran);
    }

    // 	compute fac = sqrt(-2.0 * log(rsq) / rsq)
    Ltemp1=invert_dp(rsq, 5,&n1); // Ltemp1=1/rsq in Q(31-n1)
    
    Ltemp2=log10_lut(rsq); // Ltemp2=10*log10(rsq), Q23
    Ltemp2=L_sub(Ltemp2,0x2D278D45);  // adjust for Q30 input

    Ltemp2=L_negate(Ltemp2); // make it positive
    n2=norm_l(Ltemp2);
    Ltemp2=L_shl(Ltemp2,n2); // Ltemp2=-10*log10(rsq), Q(23+n2)
    d2h=extract_h(Ltemp2);
    d2l=extract_l(Ltemp2);
    Lacc=L_mult_su(0x3333,d2l); // 0x3333 is 2/10 in Q16
    Ltemp2=L_add(L_mult(d2h,0x3333),(Word16)L_shr40(Lacc,15));
    // Ltemp2=-2*log(rsq), Q(24+n2)
    d2h=extract_h(Ltemp2);
    d2l=extract_l(Ltemp2);
    
    d1h=extract_h(Ltemp1);
    d1l=extract_l(Ltemp1);

    Lacc=L_mult_su(d1h,d2l);
    Lacc=L_add40(Lacc,(Word32)L_mult_su(d2h,d1l));
    // compute input to sqrt function
    Ltemp1=(Word32)L_add40(L_shr40(Lacc,15),L_mult(d1h,d2h)); // Ltemp1 in Q(24+n2-n1)

    n2=add(n2,24);
    n2=sub(n2,n1);
    Ltemp1=dsp_sqrt_lut(Ltemp1, 0);

    n1 = shr(n2, 1);  // n1+16 is Q factor of sqrt 
#ifdef WMOPS_FX
  test();
  logic16();
#endif
    if (n2 & 1) {
      d1h=extract_h(Ltemp1);
      d1l=extract_l(Ltemp1);  
      Lacc=L_shl40(L_mult_su(23170,d1l),1); // 23170 is 1/sqrt(2) in Q15
      Ltemp1=(Word32)L_add40(L_shr40(Lacc,16),L_mult(d1h,23170));
    }
    // Ltemp1=fac = sqrt(-2.0 * log(rsq) / rsq), Q(n1+16)
    n2=sub(13,n1);
    extra_flag=1;  // there are 2 random numbers generated
    //  extra_ran=v1 * fac;
    d1h=extract_h(Ltemp1);
    d1l=extract_l(Ltemp1);
    Ltemp1=(Word32)L_mult_su(sv1,d1l);
    Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(sv1,d1h)); // Q(n1+16)
    extra_ran=round32_16(L_shl(Ltemp1,n2));  // Q13
    
    Ltemp1=(Word32)L_mult_su(sv2,d1l);
    Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(sv2,d1h)); // Q(n1+16)
    return(round32_16(L_shl(Ltemp1,n2)));
  }
}

/*===================================================================*/
/* FUNCTION      :  GetExc800bps_fx()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generate excitation for 1/8th rate encoder, and  */
/*                  find the quantized gain index                    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) input : residual, Q0                              */
/*   _ (Word16)  length : length of subframe, Q0                     */
/*   _ (Word16)  n : current subframe number (0-2), Q0               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) output : quantized random noise residual, Q0      */
/*   _ (Word16)  best : best gain index, Q0                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: Global vairiable LAST_PACKET_RATE_E is input                */
/* NOTE: This function updates random number generator static        */
/*       variables such as the seed                                  */
/*===================================================================*/
void GetExc800bps_fx(Word16 *input, Word16 length, Word16 n,
		     Word16 *output, Word16& best)
{
  Word16 i, j, k, subframesize;
  static Word16 Sum[3];  // subframe FCB gain, Q7
  static Word16 maxSFEnergyQLast; // Q13
  Word16 d1h, d1l, d2h, n1;
  Word16 maxSFEnergy; // Q13
  Word16 maxSFEnergyQ; // Q13
  Word16 D;
  Word32 Ltemp,  minErr;
  Word40 Lacc;
  Word16 ran_res[FrameSize];
  static Word16 seed_e=0, ran_flag_e=0, extra_ran_e; // for random number generator
  
  for (i=0, Ltemp=0; i<length;i++) {
    Ltemp=L_add(Ltemp,abs_s(input[i]));
  }

  n1=norm_l(Ltemp);
  Ltemp=L_shl(Ltemp,n1); // Ltemp in Q(n1)
  
  d1h=extract_h(Ltemp);
  d1l=extract_l(Ltemp);
  d2h=0x6B66;  // d2h=1/(54*10^0.15), Q21
  
  Ltemp=(Word32)L_mult_su(d2h,d1l);
  Ltemp=L_add(L_shr(Ltemp,15),L_mult(d1h,d2h)); // Ltemp in Q(n1+22-16)

  n1=add(n1,6); // Q factor of Ltemp
  Ltemp=MAX_FX(Ltemp,1);  // make sure it's not 0
  Ltemp=log10_lut(Ltemp); // Ltemp=10log(Ltemp), Q23

  Lacc=L_mult_uu(n1,0xC0A9); // C0A9=10log(2), Q14, unsigned, Lacc in Q14

  Lacc=L_sub40_40(Ltemp,L_shl40(Lacc,9)); // adjust for Q(n1+6), Lacc in Q23

  Ltemp=(Word32)L_sat32_40(L_shl40(Lacc,3)); // Q26

  d1h=extract_h(Ltemp); // Q10
  d1l=extract_l(Ltemp); // Q26
#ifdef WMOPS_FX
  move16();
#endif
  d2h=0x6666;  // d2h=1/10 in Q18
  Ltemp=(Word32)L_mult_su(d2h,d1l); // Q44
  Ltemp=L_add(L_shr(Ltemp,15),L_mult(d1h,d2h)); // Ltemp is subframe gain in log domain, Q29
  Sum[n]=round32_16(Ltemp); // log domain gain in Q13

  /* if it's last subframe, quantize log domain gain, and generate
     random noise residual scaled by quantized linear gains        */
#ifdef WMOPS_FX
  test();
  move32();
#endif
  if (sub(n,2)==0) {
    maxSFEnergy = Sum[0];
	for (j = 1; j < 3; j++) { // find maximum log domain gain from 3 subframes
#ifdef WMOPS_FX
  test();
  move32();
#endif
      if (sub(Sum[j],maxSFEnergy)>0) maxSFEnergy = Sum[j];
	}
    minErr=LW_MAX;  // init min error to maximum
    D=sub(maxSFEnergy,MIN_LOG_ENERGY_FX); // Q13
    for (i = 0; i < NUM_Q_LEVELS; i++) {
      Ltemp=L_mult(D,D);
#ifdef WMOPS_FX
  test();
#endif
      if (L_sub(Ltemp,minErr)<0) {
	best=i;
	minErr=Ltemp;
      }
      D=sub(D,LOG_ENERGY_STEP_FX);
    }
    // End gain quantization
    maxSFEnergyQ=add(MIN_LOG_ENERGY_FX,L_shr(L_mult(best,LOG_ENERGY_STEP_FX),1)); // Q13
#ifdef WMOPS_FX
  test();
  move32();
#endif

#ifdef MUTE  
    if (best==0) maxSFEnergyQ = LOG_ZERO_ENERGY_FX;
#endif
    
#ifdef WMOPS_FX
  test();
  move32();
#endif
    if (LAST_PACKET_RATE_E != 1)
      maxSFEnergyQLast = maxSFEnergyQ;

    /* Interpolate for 3 subframe gains */
    d2h=0xAAC0;  // 0.667 in Q16, unsigned
    Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQLast,d2h),
		L_shr(L_mult(maxSFEnergyQ,d2h),1));
    Sum[0]=round32_16(Ltemp); // Q13
    Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQ,d2h),
		L_shr(L_mult(maxSFEnergyQLast,d2h),1));
    Sum[1]=round32_16(Ltemp); // Q13
    Sum[2]=maxSFEnergyQ;
    maxSFEnergyQLast = maxSFEnergyQ;
    
    /* Convert to linear domain */
    for (j = 0; j < 3; j++) {
      Ltemp=(Word32)dsp_pow10(L_shr(L_deposit_h(Sum[j]),3)); // input in Q26
      // Ltemp in Q15
      Sum[j]=round32_16(L_shl(Ltemp,8)); // linear gain in Q7
    }
    /* Get excitation */
    for (i=0, k=0; i<3; i++) { // k is output index
#ifdef WMOPS_FX
  test();
#endif
      if (i < 2) subframesize = SubFrameSize - 1;
      else subframesize = SubFrameSize;
      for (j=0;j<subframesize;j++,k++) {
	d2h=ran_g_fx(seed_e, ran_flag_e, extra_ran_e); // Q13
	Ltemp=L_mult(d2h,Sum[i]); // Q(13+7+1)
	ran_res[k]=round32_16(L_shr(Ltemp,5));  // Q0
      }
    }
#ifdef WMOPS_FX
  Nmove16(ACBMemSize);
#endif
    for (i=0;i<ACBMemSize;i++) output[i]=ran_res[i+FrameSize - ACBMemSize];
    
  }
}

/*===================================================================*/
/* FUNCTION      :  GetExc800bps_dec_fx()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Generate excitation for 1/8th rate decoder       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16)  best : best gain index, Q0                          */
/*   _ (Word16)  length : length of subframe, Q0                     */
/*   _ (Word16)  n : current subframe number (0-2), Q0               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) output : quantized random noise residual, Q0      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: Global vairiable LAST_PACKET_RATE_D is input                */
/* NOTE: This function updates random number generator static        */
/*       variables such as the seed                                  */
/*===================================================================*/
void GetExc800bps_dec_fx(Word16 best, Word16 length, Word16 n,
			 Word16 *output)
{
  Word16 i,j;
  Word16 d2h;
  static Word16 Sum[3]; // Q7
  static Word16 maxSFEnergyQLast; // Q13
  Word16 maxSFEnergyQ; // Q13
  Word32 Ltemp;
  Word16 allone_gain[3]={60,65,73}; //Q7 gains
  static Word16 seed_d=0, ran_flag_d=0, extra_ran_d;  // for random number generator

#ifdef WMOPS_FX
  test();
#endif
  if (n==0) {  // dequantize FCB gain
    maxSFEnergyQ=add(MIN_LOG_ENERGY_FX,L_shr(L_mult(best,LOG_ENERGY_STEP_FX),1)); // Q13
#ifdef WMOPS_FX
  test();
  move32();
#endif

#ifdef MUTE  
    if (best==0) maxSFEnergyQ = LOG_ZERO_ENERGY_FX;
#endif
  
#ifdef WMOPS_FX
  test();
  move32();
#endif
    if (LAST_PACKET_RATE_D != 1)
      maxSFEnergyQLast = maxSFEnergyQ;
    
#ifdef GAIN_PRINT
    printf("%f\n",maxSFEnergyQ/8192.0);
#endif
   

#if GAIN_SMOOTH
      Word16 m, temp;  

		if (SILFrame ==1) // valid silence frame , not blank
		  {
		    if ((last_last_rateD!=1)|| (lastrateD!=1) || (prev_SILFrame==1)) // voiced burst sil frame , no interp
		      //if (lastrateD!=1) // tried this but preferred line above thro listening
		      {
			G_INTERP=NO;
		      }
		    else
		      {
			G_INTERP=YES;
			
			
			final_gain = maxSFEnergyQ;    //Q13 Format
			maxSFEnergyQ = maxSFEnergyQLast; // no interp for the SID frame Q13
			init_gain = maxSFEnergyQLast ;   //Q13 Format
						
		      }
		    NG_inter=1;
		  }
		
		
		
#endif

		

#if GAIN_SMOOTH
		
		if ((SILFrame ==1) || (G_INTERP == NO))
		  {
                    /* Interpolate for 3 subframe gains */
		d2h=0xAAC0;  // 0.667 in Q16, unsigned
		Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQLast,d2h),
			    L_shr(L_mult(maxSFEnergyQ,d2h),1));
		Sum[0]=round32_16(Ltemp); // Q13
		Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQ,d2h),
			    L_shr(L_mult(maxSFEnergyQLast,d2h),1));
		Sum[1]=round32_16(Ltemp); // Q13
		Sum[2]=maxSFEnergyQ;
		maxSFEnergyQLast = maxSFEnergyQ;


		   
		  }
		else // interpolate gains for blank frame 
		  {
		     
		    //m= (final_gain - init_gain) / (32.0);
			//temp =  init_gain + (m * NG_inter);
			m = shr(sub(final_gain,init_gain),5); 
   		    temp =  add (init_gain,shr(mult(m,NG_inter),1));  //temp in Q13
		    Sum[0] = Sum[1] = Sum[2] = temp;                 //Q13 Format
		    maxSFEnergyQLast = maxSFEnergyQ = temp;          //Q13 Format
		    
		  }
		
		if ((G_INTERP == YES) && (SILFrame ==0)) 
		  {
		    NG_inter++;
		    if (NG_inter > 32) NG_inter =32; //cap it to 32 => repeat the lsp , no interp
		  }
#else
		/* Interpolate for 3 subframe gains */
		d2h=0xAAC0;  // 0.667 in Q16, unsigned
		Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQLast,d2h),
			    L_shr(L_mult(maxSFEnergyQ,d2h),1));
		Sum[0]=round32_16(Ltemp); // Q13
		Ltemp=L_sub((Word32)L_mult_su(maxSFEnergyQ,d2h),
			    L_shr(L_mult(maxSFEnergyQLast,d2h),1));
		Sum[1]=round32_16(Ltemp); // Q13
		Sum[2]=maxSFEnergyQ;
		maxSFEnergyQLast = maxSFEnergyQ;


#endif		

  

    /* Convert to linear domain */
    for (j = 0; j < 3; j++) {
      Ltemp=(Word32)dsp_pow10(L_shr(L_deposit_h(Sum[j]),3)); // input in Q26
      // Ltemp in Q15
      Sum[j]=round32_16(L_shl(Ltemp,8)); // linear gain in Q7
      if (ones_dec_cnt > 2)
         Sum[j]=allone_gain[j]; //Q7
    }
  }

  /* Get excitation */
  for (i=0;i<length;i++) {
    d2h=ran_g_fx(seed_d, ran_flag_d, extra_ran_d); // Q13
    Ltemp=L_mult(d2h,Sum[n]); // Q(13+7+1)
    output[i]=round32_16(L_shr(Ltemp,5));  // Q0
  }
}


/*===================================================================*/
/* FUNCTION      :  silence_encoder_fx()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  1/8th rate encoder main function                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: This function uses global variable for input and output     */
/*===================================================================*/
void silence_encoder_fx()
{
  register int i,j;
  Word16 subframesize, best_index;

  Word16 wt_fx[ORDER];
  short nsub8[2]={5,5};
  short nsize8[2]={16,16};

  // LSP Quantization
  weight_lsp_fx(lsp_nq_fx,wt_fx);
  lspmaq_fx8(lsp_nq_fx,nsub8,nsize8,wt_fx,lsp_fx,SScratch_fx,lsptab8_fx);
    
  for (i=0;i<ORDER;i++) cbprevprev_E_fx[i]=cbprev_E_fx[i]=lsp_fx[i];   

  for (i=0; i<2; i++){
    data_packet.LSP_IDX[i]=SScratch_fx[i];
  }
      
#ifdef LSP_SPRD
  lsp_spread(lsp_fx);
#endif  

# ifdef DTX
  
  
    
  //Word16 lpci_fx[ORDER];
        Word16 Htemp_fx[54];
	static Word32 prev_lpc_gain_fx = 0;
	Word32 lpc_gain_fx;
	Word32 Ltemp;
	Word16 temp_in[Hlength];
        Word32 temp_mem[ORDER];
        Word40 Lacc1=0;

   
	
	/* Convert to lpcs */
    lsp2lpc_fx(lsp_nq_fx, pci_nq_fx, pci_nq_fx); //Q12    
       
		for(j=0;j<54;j++) Htemp_fx[j]=0;

    /* Generate impulse response */
	 temp_in[0]=0x0800;   // one impulse 1.0 in Q11
    for (j=1; j<Hlength; j++) temp_in[j]=0;
    for (j=0;j<ORDER;j++) temp_mem[j]=0;
    synthesis_filter_fx (pci_nq_fx, temp_in, Htemp_fx, temp_mem,
		       ORDER, Hlength, 3);


	//ImpulseRzp_fx(lpci_fx, lpci_fx, lpci_fx, Htemp_fx, Hlength);  //Htemp_fx in Q11 format
	/* Get energy of H */
	lpc_gain_fx = 0;
    for (j=0,Lacc1=0; j<Hlength; j++) 
		Lacc1 = L_mac40(Lacc1, Htemp_fx[j], Htemp_fx[j]);  // Q23
	lpc_gain_fx = (Word32) L_sat32_40(Lacc1);
	
	noSID=0;
	lpcgflag=0;
    if (lastrateE==1) {
           Ltemp=(Word32)L_sat32_40(divide_dp(lpc_gain_fx, prev_lpc_gain_fx, -2)); // Ltemp in Q27
           if (L_sub(Ltemp,L_deposit_h(THRESHOLD_1))>0) { 
                noSID=TRUE;
               }
           if (L_sub(Ltemp,L_deposit_h(THRESHOLD_2))>0) { 
                lpcgflag=TRUE;
               }

       }    
     prev_lpc_gain_fx = lpc_gain_fx;

        
#endif


// Start Silence Processing
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  accshift_fx = 0;
  dpm = 0;

  for (i=0; i<NoOfSubFrames; i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;
    
    /* interpolate lsp */
    Interpol_fx(OldlspE_fx,lsp_fx,lspi_fx, i);
    Interpol_fx(Oldlsp_nq_fx,lsp_nq_fx,lspi_nq_fx, i);
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, pci_fx, pci_fx); //Q12
    lsp2lpc_fx(lspi_nq_fx, pci_nq_fx, pci_nq_fx); //Q12

#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(residual_fx[j+GUARD+shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1)],SCALE);

#endif    
    synthesis_filter_fx (pci_nq_fx , Scratch_fx , origm_fx ,SynMemoryM_fx,ORDER,subframesize,3);
    
#if 1
    
    for (j=0;j<subframesize;j++) origm_fx[j]=shl(origm_fx[j],SCALE);
#endif
    
    /* Weighting filter */
    weight_fx(pci_nq_fx , gamma90_tab, wpci_gma1_fx, ORDER);
    prediction_filter_fx(wpci_gma1_fx, origm_fx ,Scratch_fx,WFmemFIR_fx, ORDER , subframesize, 4);
    weight_fx(pci_nq_fx , gamma50_tab , wpci_gma2_fx, ORDER);

#if 1
    
    for (j=0;j<subframesize;j++) Scratch_fx[j]=shr(Scratch_fx[j],SCALE);
#endif 
    
    synthesis_filter_fx(wpci_gma2_fx, Scratch_fx, worigm_fx , WFmemIIR_fx,
			ORDER, subframesize, 3);

#if 1
    
    for (j=0;j<subframesize;j++) worigm_fx[j]=shl(worigm_fx[j],SCALE);
#endif
    
  
    GetExc800bps_fx(residual_fx+GUARD+shr(extract_l(L_mult(i,sub(SubFrameSize,1))),1), subframesize, i, Excitation_fx, best_index);
    
    ZeroInput_fx(Excitation_fx+ACBMemSize-subframesize, pci_fx, wpci_gma1_fx,
		  wpci_gma2_fx, zir_fx, subframesize, 1);

  }
  
  data_packet.SILENCE_GAIN=best_index; // only 6 bits for gain, 2 unused bits
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  data_packet.PACKET_RATE=1;
  data_packet.MODE_BIT=0;
  
}

/*===================================================================*/
/* FUNCTION      :  silence_decoder_fx()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  1/8th rate decoder main function                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) post_filter : flag indicating PF on/off, Q0          */
/*   _ (Word16) fer_flag : flag indicating frame erasure, Q0         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 [])  outFbuf : synthesized speech output buffer, Q0   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*===================================================================*/
/* NOTE: This function uses global variable for input and output     */
/*===================================================================*/
void silence_decoder_fx(Word16 *outFbuf, Word16 post_filter, Word16 fer_flag)
{


#ifdef LSP_INTERP
  static Word16 frac_fx[32]={1024,2048,3072,4096,5120,6144,7168,8096,9216,10240,11264,12288,13312,14336,15360,16384,17408,18432,19456,20480, 21504, 22528,23552, 24576,25600, 26624, 27648,28672, 29696,30720,31744,32767};
   //Q15 
  Word16 lsp_store_fx[ORDER],lsp[ORDER],OldlspD[ORDER],lspi[ORDER];
  Word16 lsp_store[ORDER];
#endif

#ifndef LSP_INTERP
 float lsp_store[ORDER];
#endif

   
  register int i, j;
  register Word16 *foutP;
  Word16 subframesize, best_index, d1h, d1l, d2h;
  Word16 gain;  // Q7, for erasure
  Word32 G[3], sum; // for erasure
  Word16 ran_num; // Q13
  Word16 temp_res[FrameSize], scaled_res[SubFrameSize]; // hold one frame's residual
  static Word16 seed_fer=0, ran_flag, extra_ran;  // random number generator for FER

  Word32 Ltemp;
  Word40 Lacc;

  foutP = outFbuf;



#ifdef WMOPS_FX
      WMP_fwc_fx("Start Silence decoder");
#endif
#ifdef WMOPS_FX
  test();
#endif

   if (fer_flag==0) { // No erasure, regular case
#ifdef WMOPS_FX
  Nmove16(4);
#endif

     
     errorFlag=0;
    /* Check for monontonic LSP */
	 for (j=1; j<ORDER; j++) {
#ifdef WMOPS_FX
  test();
  move16();
#endif
		 if (lsp_fx[j] <= lsp_fx[j - 1]) errorFlag=1;
	 }
#ifdef WMOPS_FX
  test();

#endif
    if (errorFlag==1)
      {
       for (j=0;j<ORDER;j++) lsp_fx[j]=OldlspD_fx[j];
      }

    for (i=0;i<ORDER;i++) cbprevprev_D_fx[i]=cbprev_D_fx[i]=lsp_fx[i];


#ifdef LSP_SPRD
    lsp_spread(lsp_fx);
#endif  
    
 
   /* Generate one frame of random noise residual   */
    best_index=data_packet.SILENCE_GAIN;
    
    for (i=0;i<NoOfSubFrames;i++) {
#ifdef WMOPS_FX
  test();
#endif
      if (i < 2) subframesize = SubFrameSize - 1;
      else subframesize = SubFrameSize;
      GetExc800bps_dec_fx(best_index, subframesize, i,
			  temp_res+i*(SubFrameSize-1));
    }
  }
  else { // Erasure
    //    for (j=0;j<ORDER;j++) lsp_fx[j]=OldlspD_fx[j];

    /* Estimate the FCB gain in erasure  */
    for (i=0,G[2]=0;i<SubFrameSize;i++)
      G[2]=L_mac(G[2],PitchMemoryD_fx[ACBMemSize-1-i],PitchMemoryD_fx[ACBMemSize-1-i]);
    G[2]=L_shr(G[2],1); // Q0
    Ltemp=dsp_sqrt_lut(G[2], 0); // Ltemp=sqrt(G[2]), Q16
    // scaled by 1/sqrt(54)
    d1h=extract_h(Ltemp);
    d1l=extract_l(Ltemp);
    d2h=0x8B59; // 1/sqrt(54), Q18, unsigned
    Lacc=L_shl40(L_mult_uu(d2h,d1l),1);
    G[2]=L_add((Word32)L_shr40(Lacc,16),L_shl((Word32)L_mult_su(d1h,d2h),1)); // Q19
    
    for (i=0,G[1]=0;i<SubFrameSize-1;i++)
      G[1]=L_mac(G[1],PitchMemoryD_fx[ACBMemSize-SubFrameSize-1-i],PitchMemoryD_fx[ACBMemSize-SubFrameSize-1-i]);
    G[1]=L_shr(G[1],1); // Q0
    Ltemp=dsp_sqrt_lut(G[1], 0); // Ltemp=sqrt(G[1]), Q16
    // scaled by 1/sqrt(53)
    d1h=extract_h(Ltemp);
    d1l=extract_l(Ltemp);
#ifdef WMOPS_FX
  move16();
#endif
    d2h=0x8CA8; // 1/sqrt(53), Q18, unsigned
    Lacc=L_shl40(L_mult_uu(d2h,d1l),1);
    G[1]=L_add((Word32)L_shr40(Lacc,16),L_shl((Word32)L_mult_su(d1h,d2h),1)); // Q19
    
    for (i=0,G[0]=0;i<ACBMemSize-(2*SubFrameSize-1);i++)
      G[0]=L_mac(G[0],PitchMemoryD_fx[i],PitchMemoryD_fx[i]);
    G[0]=L_shr(G[0],1); // Q0
    Ltemp=dsp_sqrt_lut(G[0], 0); // Ltemp=sqrt(G[0]), Q16
    // scaled by 1/sqrt(ACBMemSize-(2*SubFrameSize-1)
    d1h=extract_h(Ltemp);
    d1l=extract_l(Ltemp);
#ifdef WMOPS_FX
  move16();
#endif
    d2h=0xDF75; // 1/sqrt(21), Q18, unsigned
    Lacc=L_shl40(L_mult_uu(d2h,d1l),1);
    G[0]=L_add((Word32)L_shr40(Lacc,16),L_shl((Word32)L_mult_su(d1h,d2h),1)); // Q19

    for (i=0, sum=0;i<3;i++) {
      Ltemp=log10_lut(MAX_FX(G[i],1)); // make sure input >0, Ltemp=10log10(G[i]), Q23
      // adjust for Q19 input
      Ltemp=L_sub(Ltemp,0x1C990CAC); // 10*log10(2^19), Q23
      sum=L_add(sum,Ltemp);
    }
    // sum=10*(log10(G[0])+log10(G[1])+log10(G[2])), Q23
    d1h=extract_h(sum);
    d1l=extract_l(sum);
#ifdef WMOPS_FX
  move16();
#endif
    d2h=0x8889; // 1/30, Q20, unsigned
    Lacc=L_mult_uu(d2h,d1l);
    Ltemp=L_add((Word32)L_shr40(Lacc,16),(Word32)L_mult_su(d1h,d2h)); // Q27
    Ltemp=(Word32)dsp_pow10(L_shr(Ltemp,1)); // input in Q26
    // Ltemp=gain in Q15
    gain=round32_16(L_shl(Ltemp,8)); // gain in Q7
    
    /* Generate one frame of random noise residual   */
    for (i=0;i<FrameSize;i++) {
      ran_num=ran_g_fx(seed_fer, ran_flag, extra_ran); // Q13
      Ltemp=L_mult(ran_num,gain); // Q(13+7+1)
      temp_res[i]=round32_16(L_shr(Ltemp,5));  // Q0
    }
  }

  /* Synthesize speech and update filter memory  */
  for (i=0; i<NoOfSubFrames; i++) {
#ifdef WMOPS_FX
  test();
#endif
    if (i < 2) subframesize = SubFrameSize - 1;
    else subframesize = SubFrameSize;

    /* Linear interpolation of lsp */
    Interpol_fx(OldlspD_fx, lsp_fx, lspi_fx, i);


#if (LSP_INTERP)
for(j=0;j<ORDER;j++)
      {
	lsp[j]= lsp_fx[j];
	OldlspD[j] = OldlspD_fx[j];
	lspi[j]= lspi_fx[j];
      }
#endif


#if LSP_INTERP
    Word32 temp=0;
	Word32 temp1=0;

    if (SILFrame ==1) // valid silence frame , not blank
      {
	if ((last_last_rateD!=1) || (lastrateD!=1) || (prev_SILFrame==1)) // voiced burst sil frame , no interp
	//if (lastrateD!=1) // tried this but preferred line above thro listening
	  {
	    INTERP=NO;
	    for(j=0;j<ORDER;j++)
	      lsp_store_fx[j]= lsp[j];
	  }
	else 
	  {
	    INTERP=YES;

	    if (i==0) //1st sf
	      for(j=0;j<ORDER;j++)
		{	  
		  final_lsp[j]=lsp[j];
		  lspi[j]= OldlspD[j]; //this is the final val of the interp of the past N frames!
		  lsp[j] = lspi[j];
		  init_lsp[j]=OldlspD[j];
		  lsp_store_fx[j] = final_lsp[j];
		}
	  }
	N_inter=1;
      }
    
    if ((INTERP == YES) && (SILFrame ==0)) 
      {
	for (j = 0; j < ORDER; j++) {
	  //lspi[j] = ((1.0 - frac[N_inter-1]) * init_lsp[j] + frac[N_inter-1] * final_lsp[j]);
	  temp= L_mult(sub(32767,frac_fx[N_inter-1]),init_lsp[j]);  //temp in Q31
	  temp1=L_mult(frac_fx[N_inter-1],final_lsp[j]);  //temp1 in Q31
	  temp= L_add(temp,temp1); //temp in Q31
	  lspi[j]= extract_h(temp); // lspi[j] in Q15
	}
	
	lsp_spread_ERDTX(lspi);
	for(j=0;j<ORDER;j++)
	  lsp[j]=lspi[j];
	
	
	if (i==2) N_inter++;
	if (N_inter > 32) N_inter =32; //cap it to 32 => repeat the lsp , no interp
      }
    
    for(j=0;j<ORDER;j++)
      lsp_store[j] = (lsp_store_fx[j]);

#else
    for(j=0;j<ORDER;j++)
      lsp_store[j] = (lsp_fx[j]/32768.0);
    
#endif

    
#if (LSP_INTERP)
   
    for(j=0;j<ORDER;j++)
      {
        lsp_fx[j] = lsp[j];
	lspi_fx[j] = lspi[j];
	
      }
#endif

   

    
    /* Convert lsp to PC */
    lsp2lpc_fx(lspi_fx, prev_pci_fx, pci_fx); //Q12
       for(j=0;j<subframesize;j++)
        scaled_res[j]=shr(temp_res[i*(SubFrameSize-1)+j],SCALE);


    /* Synthesis of decoder output signal and postfilter output signal */
    synthesis_filter_fx(pci_fx , scaled_res , DECspeech_fx ,SynMemory_fx,ORDER,subframesize,3);

#ifdef WMOPS_FX
  test();
#endif
    if (post_filter) {
      Post_Filter_fx(DECspeech_fx, lspi_fx, pci_fx, DECspeechPF_fx, 
		       0x140000 ,0, subframesize); // 20 in Q16
   }
    else {
      for(j=0;j<subframesize;j++)
	DECspeechPF_fx[j]=DECspeech_fx[j];
    }
    
         for(j=0;j<subframesize;j++)
	     DECspeechPF_fx[j]=shl(DECspeechPF_fx[j],SCALE);





    for (j = 0; j < subframesize; j++) {
#ifdef WMOPS_FX
  move16();
#endif
      *foutP++=DECspeechPF_fx[j];
    } 
  }
  
#if 1
  for(j=0;j<ORDER;j++)
    prev_pci_fx[j]=pci_fx[j];
#endif
  
  /* Update pitch memory for next frame */
  for (j=0;j<ACBMemSize;j++) PitchMemoryD_fx[j]=temp_res[FrameSize-ACBMemSize+j];
#ifdef WMOPS_FX
      WMP_fwc_fx("Silence Decoder ..........................................");
#endif


}

