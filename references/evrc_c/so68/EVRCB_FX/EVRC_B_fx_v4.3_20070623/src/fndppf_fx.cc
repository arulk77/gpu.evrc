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

/************************************************************************
* Routine name: fndppf_fx                                                  *
* Function: forward pitch predictor.		                        *
* Inputs:   buf    - data buffer.                                       *
*           dmin   - minimum delay value.                               *
*           dmax   - maximum delay value.                               *
*           prevdelay - previous frame delay value.                     *
*           length - size of pitch window.                              *
* Outputs:  delay  - predicted delay.                                   *
*           beta   - gain value.                                        *
*                                                                       *
************************************************************************/

#include"dsp_math.h"
#include "typedef_fx.h"
#include "basic_op40.h"
#include "basic_op.h"
#include "macro_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


Word16 get_delay_fx(Word16 center,Word16 range,Word16 buf[],Word16 length,Word40 *Lcorrmax,Word16 dmin,Word16 dmax)
{
  Word16 i, m, m0, M1, M2, k;
  Word40 Lcmax = MIN_40, Lacc; 
  
  M1 = sub(center,range); 
  M2 = add(center,range);
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (M1 < dmin) { M1 = dmin; M2 = add(dmin,shl(range,1)); }
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (M2 > dmax) { M2 = dmax; M1 = sub(dmax,shl(range,1)); }
  

		for (m = M1; m <= M2; m++)
		{
		  k=sub(length, m);
                    for (i = 0, Lacc = 0; i < k; i++)
			Lacc=L_mac40(Lacc,buf[i],buf[i+m]);  
#ifdef WMOPS_FX
  test();
#endif
			if (Lacc>Lcmax)
			{		/* Gives some bias to low delays */
				Lcmax = Lacc;
				m0 = m;
			}
		      
		}
	     
#ifdef WMOPS_FX
    move32();
#endif
		*Lcorrmax =Lcmax;
                
             return m0;
}



/************************************************************************
* Routine name: get_beta_fx                                            *
* Function: Computes beta (NACF=Exy/(sqrt(Exx*Eyy))) given the input
            buffer and Exy
*                    	                        
* Inputs:   Word16    delay   
            Word16    length - size of pitch window.
            Word32    Lcmax  - Max cross correlation value (Exy)
            Word16    Lcmax_scale - Scale factor of above
            Word16[]  buf    - data buffer.                             *
                    
* Outputs:  Word16    beta = Exy/sqrt(Exx*Eyy)                          *
*                                       
*                                                                       *
************************************************************************/





Word16 get_beta_fx(Word16 delay, Word16 length, Word40 Lcmax, Word16 buf[])
{
  Word16 i,k,sum2;//Q15
  Word32 Lsum1, Lsum2,Lsum3;
  Word16 n1,n2,nq,nq1; 
  Word40 Lacc;
  Word16 S1h,S1l,S2h,S2l;

 	for (i = delay, Lacc = 0; i <length ; i++)
          Lacc=L_mac40(Lacc,buf[i],buf[i]);  
        
         nq=norm32_l40(Lacc);
	 Lsum1=(Word32) L_shl40(Lacc,nq); // Lsum1 is Q(nq+1);
       nq =(Lacc==0)?31:nq;

       k=sub(length,delay);
	for (i = 0, Lacc = 0; i < k; i++)
	   Lacc=L_mac40(Lacc,buf[i],buf[i]);  

	nq1=norm32_l40(Lacc);
	Lsum2=(Word32) L_shl40(Lacc,nq1); // Lsum2 is Q(nq1+1);

      nq1 =(Lacc==0)?31:nq1;


	/* following code segment multiplies two 32 bit numbers     */
        /* to give a 32 bit result                               */
      S1h=extract_h(Lsum1);
	S1l=extract_l(Lsum1);
	S2h=extract_h(Lsum2);
	S2l=extract_l(Lsum2);
	Lacc=L_mult_su(S1h,S2l);
	Lacc=L_mac40_su(Lacc,S2h,S1l);
	Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(S1h,S2h),1));   // Lacc in Q(nq1+nq2+2-32)

        n1=norm32_l40(Lacc);
	Lsum1=(Word32) L_shl40(Lacc,n1); // Lsum1 in Q(nq1+nq2-30+n1-1)
       n1 =(Lacc==0)?31:n1;

        n1=sub(n1,1); 


       	Lsum3 = dsp_sqrt_lut(L_shr(Lsum1,1),0);
	n1 = add(add(n1, nq), nq1);
        
	n2 = shr(n1, 1);
        
#ifdef WMOPS_FX
  test();
  logic16();
#endif
        if (n1 & 1)
	  {
	    /* following code segment multiplies a 16 bit number with */
	    /* a 32 bit number to give a 32 bit result                */
#ifdef WMOPS_FX
    move32();
#endif
              Lacc=0;
              S1h=extract_h(Lsum3);
	      S1l=extract_l(Lsum3);  
              Lacc=L_shl40(L_mult_su(23170,S1l),1); // 23170 is 1/sqrt(2) in Q15
              Lsum1=(Word32)L_add40(L_shr40(Lacc,16),L_mult(S1h,23170));
	      
          }
	  
		
	else 
		Lsum1 = Lsum3;
        
        Lcmax=L_shl40(Lcmax,n2);
        
#ifdef WMOPS_FX
  test();
  logic16();
  move32();
#endif
	if ((Lsum1 == 0) || (Lcmax <= 0))
		sum2 = 0;
	else if (Lcmax > Lsum1)
		sum2 = 0x7FFF;
        else
          {
                //sum2=round32_16((Word32) L_sat32_40(divide_dp(Lcmax,Lsum1,2)));
                sum2=extract_h((Word32) L_sat32_40(L_add40(divide_dp(Lcmax,Lsum1,2),0x00008000))); 


          }      	 
        return(sum2);

}

void fndppf_fx(Word16 *delay_fx,Word16 *beta_fx,Word16 *buf,Word16 dmin,Word16 dmax,
	    Word16 length)
{
  

        static Word16  b = -10224;	/* rom storage */

	static Word16 a[3] =
	{-18739, 16024, -4882};		/* a[] scaled down by 4 */

	Word16 dnew = shr(dmin,2);
	Word40 Lsum;
	register short m, i, n, k;
	static Word16 DECbuf[FrameSize / 4];
	Word40 Lcorrmax, Lcmax,Lacc; 
	Word16 tap1;//Q15
	Word16 dnewtmp = shr(dmin,2);
        Word16 tmp; //Q11
	static Word16 lastgoodpitch = 0;
	static Word16 lastbeta = 0,n1;
	static Word16 memory[3];
	static Word16 FirstTime = 1;
	Word40 Ltempf;
        //Word16 buf[length];
	/* init static variables (should be in init routine for implementation) */
       
       
#ifdef WMOPS_FX
  test();
#endif
	if (FirstTime)
	{
#ifdef WMOPS_FX
    move16();
#endif
		FirstTime = 0;
		n1 = (shr(FrameSize, 2));
		for (i = 0; i < n1; i++) {
#ifdef WMOPS_FX
    move16();
#endif
			DECbuf[i] = 0;
		}
#ifdef WMOPS_FX
    move16();
	move16();
	move16();
#endif
		memory[0] = memory[1] = memory[2] = 0;
	}

       

	/* Shift  memory of DECbuf */
	m=shr(length, 3);
	for (i = 0; i < m; i++)
	{
#ifdef WMOPS_FX
    move16();
#endif
		DECbuf[i] = DECbuf[i + m];
	}

	/* filter signal and decimate */
	k=shr(length, 1);
	for (i = 0, n = m; i < k; i++)
	{
		Ltempf = L_shr40(L_deposit_h(buf[i + k]), 4);
		Ltempf = L_msu40(Ltempf, memory[0], a[0]);
                Ltempf = L_msu40(Ltempf, memory[1], a[1]);
		Ltempf = L_msu40(Ltempf, memory[2], a[2]);
		Ltempf = L_shl40(Ltempf, 2);
		
#ifdef WMOPS_FX
  test();
#endif
		if ((i + 1) % 4 == 0)
		{
			Lsum = L_add40(Ltempf, L_deposit_h(memory[2]));
			Lsum = L_mac40(Lsum, memory[0], b);
			Lsum = L_mac40(Lsum, memory[1], b);

			//DECbuf[n++] = round32_16((Word32)L_sat32_40(L_shl40(Lsum, 1)));
			//                  DECbuf[n++]=extract_h((Word32) L_sat32_40(L_add40(L_shl40(Lsum, 1),0x00008000)));
			DECbuf[n++]=extract_h((Word32) L_sat32_40(L_add40(Lsum,0x00008000))); // Q-2
			
			
		}
#ifdef WMOPS_FX
    move16();
	move16();
#endif
		memory[2] = memory[1];
		memory[1] = memory[0];
		//memory[0] = round32_16((Word32)L_sat32_40(Ltempf));
             memory[0] = extract_h((Word32) L_sat32_40(L_add40(Ltempf,0x00008000))); 

                
	}
        
	

            
/* perform first search for best delay value in decimated domain */
#ifdef WMOPS_FX
    move32();
#endif
	Lcorrmax = (LW_MIN);
        

        k=shr(dmax, 2);
	for (m = shr(dmin, 2); m <= k; m++)
	{
	  n=sub(shr(length, 2), m);
             for (i = 0, Lacc= 0; i < n; i++)
		 Lacc=L_mac40(Lacc,DECbuf[i],DECbuf[i+m]);  // Lacc in Q1
 
#ifdef WMOPS_FX
  test();
#endif
	     if (Lacc>Lcorrmax) 
		{
#ifdef WMOPS_FX
    move32();
	move16();
#endif
			Lcorrmax = Lacc;
			dnew = m;
		}
	}   

		
		
	 

  /* perform first search for best delay value in non-decimated buffer */

   dnew = get_delay_fx(shl(dnew,2), 3, buf, length, &Lcorrmax, dmin, dmax);
   *beta_fx = get_beta_fx(dnew, length, Lcorrmax, buf);
	
   /* perform search for best delay value in around old pitch delay */
 
#ifdef WMOPS_FX
  test();
  logic16();
  logic16();
#endif
  if (lastgoodpitch!=0 && (dnew>add(lastgoodpitch,6) || dnew<sub(lastgoodpitch,6))) {
    dnewtmp = get_delay_fx(lastgoodpitch, 6, buf, length, &Lcmax, dmin, dmax);
    tap1 = get_beta_fx(dnewtmp, length, Lcmax,buf);
    
    /* Replace dnew with dnewtmp if tap1 is large enough */
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (dnew > add(lastgoodpitch,6)) tmp =1229 ;
    else if (dnew < sub(lastgoodpitch,6)) tmp = 2458;
    else tmp = 20480;
				
#ifdef WMOPS_FX
  test();
#endif
    if (shr(tap1,4) > mult_r(tmp,*beta_fx)) {
#ifdef WMOPS_FX
    move16();
	move16();
#endif
      dnew = dnewtmp;
      *beta_fx = tap1;
    }
  }
  

  /* Check if half of the selected pitch a better candidate */
#ifdef WMOPS_FX
  test();
#endif
  if(sub(dnew,40) >= 0) {
    dnewtmp = get_delay_fx(shr(dnew,1), 2, buf, length, &Lcmax, dmin, dmax);
    tap1 = get_beta_fx(dnewtmp, length, Lcmax,buf);
    
#ifdef WMOPS_FX
  test();
#endif
    if (shr(tap1,1) > mult_r(13107, *beta_fx)) {
      dnew = dnewtmp;
      *beta_fx = tap1;
    }
  }
  
  *delay_fx =  dnew;
#ifdef WMOPS_FX
  test();
#endif
  if (*beta_fx > 13107) {
    lastgoodpitch = dnew;
    lastbeta = *beta_fx;
  }
  else {
    lastbeta= mult_r(24576,lastbeta);
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (lastbeta < 9830) lastgoodpitch = 0;
  }
   
}
