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
/*****************************************************************
 *   (C) COPYRIGHT 1999,2000 Motorola, Inc.
 *    MOTOROLA CONFIDENTIAL PROPRIETARY
 *****************************************************************/
#include "proto_fx.h"
#include "basic_op.h"
#include "basic_op40.h"
#include "dsp_math.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

#ifdef DYNAMIC_COMPLEXITY_REDUC
#include "globs_fx.h"
#endif //DYNAMIC_COMPLEXITY_REDUC

Word16 multabc(Word16 a, Word16 b, Word16 c);

Word16 multabc(Word16 a, Word16 b, Word16 c)
{
    Word16 d, e;
    
    d = extract_l(L_mult(b, c));
    e = extract_l(L_shr(L_mult(a, d), 2));
   
    return(e);
}
    
/*===================================================================*/
/* FUNCTION      :  cod7_35_fx()	                             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16*) xw       : Target signal. Q0                        */
/*   _ (Word16*) x        : Target signal. (through inverse weighting*/
/*                                       filter) Q0                  */
/*   _ (Word16)  l_subfr  : length of current subframe.              */
/*   _ (Word16*) h        : Weighted filter impulse response. Q11    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16*) ck       : Multipulse excitation. Q0                */
/*   _ (Word16*) gain     : Excitation gain parameter. Q0            */
/*   _ (Word16*) codeword : Factorial Packed codeword.               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*===================================================================*/

void cod7_35_fx(
	Word16 *xw,	 /* (input)  Target signal. */
   	Word16 *x,     	 /* (input)  Target signal (through inverse weighting filter). */
   	Word16 l_subfr,	 /* (input)  Length of current subframe. */
   	Word16 *h,	 /* (input)  Weighted filter impulse response. */
   	Word16 *ck,	 /* (output) Multipulse excitation. */
   	Word16 *gain,    /* (output) Excitation gain parameter. */
   	Word16 *codeword)/* (output) Factorial Packed codeword. */
{
   	Word16 alpn[fcb_N_PRE];
	Word16 sqn[fcb_N_PRE];
  	Word16 sign[MAX_TLEN];
   	Word16 ordered_pos[MAX_TLEN], ivect[N_PULSES], jvect[N_PULSES];
   	Word16 i, j, k, n, dec, pos, vect3[fcb_N_PRE][4];
   	register int i0, i1, i2, ia, ib, i3, i4, i5, i6;
   	Word40 ps, ps1, ps2;
	Word16 xps, xps0, xps1, xps2, xpsk;
        Word16 accum[MAX_TLEN], d2[MAX_TLEN], rr0[L_CODE];
        Word16 temp, temp1, nd2, nacc, nrr0;
	Word32 L_tmp[55];
	Word16 alp, alp0, alp1, alp2, alpk;
   	Word16 sq, sq2, sqk, halfrr0,mx16=0,SAT=0;
	Word32 nmax, Ltemp;

	/* Generate backward filtered target vector d[n] (d' = xw'H) */

        corr_xh_fx(xw, d2, h, l_subfr, &nd2, 3);  // d2 is Q(nd2)	

  	/* Put h[i]= d2[i] = x[i] = 0; for i>l_subfr */
	for(i=l_subfr; i<L_CODE; i++) {
#ifdef WMOPS_FX
  Nmove16(3);
#endif
			h[i] = d2[i] = x[i] = 0;
	}

 	/*----------------------------------------------------------------*
  	*     BUILD "d2[]", "sign[]" AND "d6[]" VECTORS.                 *
  	*----------------------------------------------------------------*/
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	ps1 = 0;
  	ps2 = 0;
  	for (i=0; i<l_subfr; i++) {
  	     ps1 = L_mac40(ps1, x[i], x[i]);     // Q1
  	     ps2 = L_mac40(ps2, d2[i], d2[i]);   // Q(2*nd2-7)
   	}
	Ltemp = sqrt_divide_dp(ps2, ps1, sub(shl(nd2, 1), 8), &temp1);


	temp = extract_h(L_shl(Ltemp, sub(16,sub(temp1, 9)))); //Q9

#ifdef WMOPS_FX
  move32();
#endif
	nmax=0L;
 	for (j=0; j<l_subfr; j++) {
   	     Ltemp = L_add(L_mult(temp, x[j]), L_shr(L_deposit_h(d2[j]), add(nd2, 1)));  // Q10
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
   	     if (Ltemp >= 0)
    	 	 sign[j] = 1;
   	     else {
                 sign[j] = -1;
      		 d2[j] = negate(d2[j]);
      		 Ltemp = L_negate(Ltemp);
      	     }
             L_tmp[j] = Ltemp;          // Q10
#ifdef WMOPS_FX
	  test();
#endif
	     if(Ltemp < 0) Ltemp = L_negate(Ltemp);
#ifdef WMOPS_FX
	  test();
#endif
	     if(L_sub(Ltemp, nmax) > 0L)
		 nmax = Ltemp;
        }
	nacc = norm_l(nmax);
 	for (j=0; j<l_subfr; j++) {
	     accum[j] = round32_16(L_shl(L_tmp[j], nacc));  // Q(10+nacc-16)=Q(nacc-6)
	}

 	/*----------------------------------------------------------------*
  	*        BUILD "rr[][]", MATRIX OF AUTOCORRELATION.              *
  	*----------------------------------------------------------------*/
	//Doing regular diag elements

  	for (k=0, ps=0; k<l_subfr; k++) {
   	     ps = L_mac40(ps, h[k], h[k]);   //Q23
    }
	nrr0 = sub(norm32_l40(ps), 4);      
    rr0[0] = round32_16((Word32)L_shl40(ps, nrr0));   // Q(23+nrr0-16)=Q(nrr0+7)

	//Make first row

  	for (dec=1; dec<l_subfr; dec++) {
   	     for (k=0, ps=0; k<sub(l_subfr, dec); k++) {
    		  ps = L_mac40(ps, h[k], h[k+dec]);      // Q23
   	     }
   	     rr0[dec] = round32_16((Word32)L_shl40(ps, nrr0));  //Q(nrr0+7)
  	}

	/******************************************************************
	     Stage #1:	Pre-pre-select
	 	Select number of candidate positions for the first 3 pulses
		by searching for the positions with largest d6[] values.
	******************************************************************/
 
    for(k=0; k<fcb_N3; k++) {
   	    for (temp= -32768, i=0, j=0; j<l_subfr; j++) {
#ifdef WMOPS_FX
	  test();
	  move16();
	  move16();
#endif
			if (accum[j] > temp) {
  				temp = accum[j];
  				i = j;
	  		}
		}
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      	accum[i] = -32768;
	    ordered_pos[k] = i;
    }

	/******************************************************************
	     Stage #2:	Pre-select
	 	Select candidate combinations of pulse 1, 2 and 3
		by searching for the highest gain.
	******************************************************************/

	halfrr0=shr(rr0[0], 1);

	for(i=0; i<fcb_N_PRE; i++) {
	    alpn[i] = shl(1, add(nrr0, 7));    // Q(nrr0+7)
	    sqn[i]  = shl(-1, sub(shl(nd2, 1), 23));  // Q(2*nd2-23)
	}

	alpk = shl(1, add(nrr0,7));            // Q(nrr0+7)
	sqk = shl(-1, sub(shl(nd2, 1),23));    // Q(2*nd2-23)
#ifdef WMOPS_FX
  move16();
#endif
	pos = 0;
	ia=0;
	
 	for(i=0; i<fcb_N0; i++) {
#ifdef WMOPS_FX
  move16();
#endif
 	    i0 = ordered_pos[i];
	    for(j=i+1; j<fcb_N1; j++) {
#ifdef WMOPS_FX
  move16();
#endif
    		i1   = ordered_pos[j];
			xps0 = add(d2[i0],d2[i1]);     // Q(d2)
			alp0 = add(rr0[0], multabc(rr0[abs_s(i0-i1)], sign[i0], sign[i1]));

    		for(k=j+1; k<fcb_N2-i; k++) {
#ifdef WMOPS_FX
  move16();
#endif
				i2  = ordered_pos[k];
				xps1  = add(xps0, d2[i2]);
    		    alp1 = add(add(alp0, halfrr0), add(multabc(rr0[abs_s(i0-i2)], sign[i0], sign[i2]), multabc(rr0[abs_s(i1-i2)], sign[i1], sign[i2])));

				for(n=k+1; n<fcb_N3-j; n++) {
#ifdef WMOPS_FX
  move16();
#endif
					i3  = ordered_pos[n];
					xps  = add(xps1, d2[i3]);
					temp = add(add(alp1, halfrr0), add(multabc(rr0[abs_s(i0-i3)], sign[i0], sign[i3]), multabc(rr0[abs_s(i1-i3)],sign[i1],sign[i3]))); 
     	    		alp =  add(temp, multabc(rr0[abs_s(i2-i3)],sign[i2],sign[i3]));
					sq = mult_r(xps, xps); //Q(2*nd2-23)
#ifdef WMOPS_FX
	  test();
#endif



	   if (L_mult(sq, alpk) > L_mult(sqk, alp))
	      {
			                       sqn[ia] = sq;
                       				alpn[ia] = alp;
			                       vect3[ia][0] = i0;
			                       vect3[ia][1] = i1;
			                       vect3[ia][2] = i2;
			                       vect3[ia][3] = i3;

                         			alpk = alpn[0];
			                        sqk = sqn[0];
                       			  	ia = 0;
			                         for(ib=1; ib<fcb_N_PRE; ib++) {
			                         if(L_mult(sqn[ib], alpk) < L_mult(sqk, alpn[ib])) {  
			                             ia = ib;
			                             alpk = alpn[ib];
			                             sqk = sqn[ib];
			                           }
			                         }
	      }



	  
				}
			}
	    }
	}

	
	
#if 1 //saturation detector not inside any loop , once per SF
	for(i=0; i<fcb_N_PRE; i++) 
	  mx16  = MAX_FX(abs_s(alpn[i]),mx16);  // Q(nrr0+7)
	
	if (L_add(L_shr(L_mult(rr0[0],18),1),mx16)>= MAX_16)
	  SAT=1;
	
	if (SAT==1)
	  {
	    nrr0 = sub(nrr0,1);	    
	    for (i=0; i<l_subfr; i++) 
	      rr0[i] = shr(rr0[i],1); //Q(nrr0+7)
	    
	    for(i=0;i<fcb_N_PRE;i++)
	      alpn[i] = shr(alpn[i],1) ; //Q(nrr0+7)
	    
	    halfrr0 = shr(halfrr0,1);
	  }
	
	
#endif
	/*********************************************************************
	    Stage #3:	Search
		Search the rest of the pulses over the candidate combinations
		of the first 3 pulses.
	*********************************************************************/
  	alpk = shl(1, add(nrr0,7));
   	sqk = shl(-1, sub(shl(nd2,1),23));
   	xpsk = 0;

	for(i=0; i<fcb_N_PRE; i++) {
#ifdef WMOPS_FX
  Nmove16(5);
#endif
	    i0 = vect3[i][0];
	    i1 = vect3[i][1];
	    i2 = vect3[i][2];
	    i3 = vect3[i][3];

	    alp0 = alpn[i];
	    xps0 = add(add(d2[i0], d2[i1]), add(d2[i2], d2[i3]));

	    for(j=0; j<l_subfr; j++) 
		accum[j] = add(add(multabc(rr0[abs_s(i0-j)],sign[i0],sign[j]), multabc(rr0[abs_s(i1-j)],sign[i1],sign[j])), add(multabc(rr0[abs_s(i2-j)],sign[i2],sign[j]), halfrr0));
				// Q(nrr0+7)
	    ia = i3;
	    for(j=0; j<3; j++) {
	        alp =  shl(1, add(nrr0,7));
	        sq  = shl(-1, sub(shl(nd2,1),23));

	        for(k=0; k<l_subfr; k++) accum[k] = add(accum[k], multabc(rr0[abs_s(ia-k)],sign[ia],sign[k]));
	 			for(k=0; k<l_subfr; k++) {
					xps2  = add(xps0, d2[k]);
					alp2 = add(alp0, accum[k]);
					sq2  = mult_r(xps2, xps2);    // Q(2*nd2-23)
#ifdef WMOPS_FX
	  test();
#endif
	 				if(L_mult(sq2, alp) > L_mult(sq, alp2)) {
#ifdef WMOPS_FX
  move16();
#endif
						sq  = sq2;
	   					xps  = xps2;
	   					alp = alp2;
   						jvect[j] = k;
					}
				}
		
#ifdef WMOPS_FX
  move16();
  move16();
  move16();
#endif
	        ia   = jvect[j];
	        xps0  = xps;
			alp0 = alp;
	    }

#ifdef WMOPS_FX
	  test();
#endif
	    if(L_mult(sq, alpk) > L_mult(sqk, alp)) {
			sqk  = sq;
	   		xpsk  = xps;
	   		alpk = alp;
			
			for(j=0; j<4; j++) {
				ivect[j] = vect3[i][j];
			}
			for(j=0; j<3; j++) {
#ifdef WMOPS_FX
  move16();
#endif
			ivect[j+4] = jvect[j];
			}
	    }
	}

#ifdef WMOPS_FX
  Nmove16(7);
#endif
	i0 = ivect[0];
	i1 = ivect[1];
	i2 = ivect[2];
	i3 = ivect[3];
	i4 = ivect[4];
	i5 = ivect[5];
	i6 = ivect[6];

	for(i=0; i<l_subfr; i++) {
	    temp = add(add(halfrr0, multabc(rr0[abs_s(i0-i)],sign[i0],sign[i])), add(multabc(rr0[abs_s(i1-i)],sign[i1],sign[i]), multabc(rr0[abs_s(i2-i)],sign[i2],sign[i])));
	    temp1 = add(add(multabc(rr0[abs_s(i3-i)],sign[i3],sign[i]), multabc(rr0[abs_s(i4-i)],sign[i4],sign[i])), add(multabc(rr0[abs_s(i5-i)],sign[i5],sign[i]), multabc(rr0[abs_s(i6-i)],sign[i6],sign[i])));
	    accum[i] = add(temp, temp1);
	}
        // Q(nrr0+7)

	for(i=0; i<N_POST; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	    ia = ivect[i%7];
	    xps0 = sub(xpsk, d2[ia]);
	    alp0 = sub(alpk, sub(accum[ia], rr0[0]));

	    for(j=0; j<l_subfr; j++) {
			xps2  = add(xps0, d2[j]);
			alp2 = add(alp0, sub(accum[j], multabc(rr0[abs_s(ia-j)],sign[ia],sign[j])));

			sq2 = mult_r(xps2, xps2);  // Q(2*nd2-23)

#ifdef WMOPS_FX
	  test();
#endif
	 		if(L_mult(sq2, alpk) > L_mult(sqk, alp2)) {
#ifdef WMOPS_FX
  move16();
#endif
				sqk  = sq2;
	   			xpsk  = xps2;
	   			alpk = alp2;
  				ivect[i%7] = j;
			}
	    }
		
#ifdef WMOPS_FX
  move16();
#endif
	    ib = ivect[i%7];
#ifdef WMOPS_FX
	  test();
#endif
	    if(ib != ia) {
		for(j=0; j<l_subfr; j++) 
		    accum[j] = add(accum[j], sub(multabc(rr0[abs_s(ib-j)],sign[ib],sign[j]), multabc(rr0[abs_s(ia-j)],sign[ia],sign[j])));
	    }
	}

#ifdef WMOPS_FX
  Nmove16(7);
#endif
	i0 = ivect[0];
	i1 = ivect[1];
	i2 = ivect[2];
	i3 = ivect[3];
	i4 = ivect[4];
	i5 = ivect[5];
	i6 = ivect[6];

 	/*--------------------------------------------------------------------*
  	* BUILD THE CODEWORD, THE FILTERED CODEWORD AND INDEX OF CODEVECTOR. *
  	*--------------------------------------------------------------------*/
   	for (i=0; i<L_CODE; i++) { 
#ifdef WMOPS_FX
  move16();
#endif
	    ck[i]=0;
	}

	*gain = round32_16((Word32) L_sat32_40(divide_dp(xpsk, alpk, sub(nrr0, add(3, nd2))))); // Q0

   	for (k=0; k<N_PULSES; k++) {
#ifdef WMOPS_FX
  move16();
#endif
    	pos = ivect[k];
    	ck[pos] += sign[pos];
	}

	factorial_pack_fx(ck, codeword);

   	return;
}

/*===================================================================*/
/* FUNCTION      :  ACELP_Code_fx()	                                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16*) xn       : Target signal for codebook. Q0           */
/*   _ (Word16*) res2     : Residual after pitch contribution. Q0    */
/*   _ (Word16*) h1       : Impulse response of filters.      Q11    */
/*   _ (Word16)  T0       : Pitch lag.                               */
/*   _ (Word16)  pitch_gain  : Pitch gain.  Q14                      */
/*   _ (Word16)  l_subfr  : subframe length.                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16*) code     : Innovative vector.     Q11               */
/*   _ (Word16*) gain_code: Innovative vector gain.. Q0              */
/*   _ (Word16*) index    : Index of codebook + signs.               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*===================================================================*/

void ACELP_Code_fx(
  Word16 xn[],         /* (i)     :Target signal for codebook.       */
  Word16 res2[],       /* (i)     :Residual after pitch contribution */
  Word16 h1[],         /* (i)     :Impulse response of filters.      */
  int   T0,           /* (i)     :Pitch lag.                        */
  Word16 pitch_gain,   /* (i)     :Pitch gain.                       */
  int   l_subfr,      /* (i)     :Subframe lenght.                  */
  Word16 code[],       /* (o)     :Innovative vector.                */
  Word16 *gain_code,   /* (o)     :Innovative vector gain.           */
  Word16  *index,        /* (o)     :Index of codebook + signs         */
  int  choice         /* (i)     :Choice of innovative codebook     */
                      /*          0 -> 10 bits                      */
                      /*          1 -> 35 bits                      */
)
{
  int i;
  Word16 pit_sharp_fx;
  

  /*-----------------------------------------------------------------*
   * - Find pitch sharpening.                                        *
   *-----------------------------------------------------------------*/

  pit_sharp_fx = pitch_gain;                // Q14
  
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if(pit_sharp_fx > 14746) pit_sharp_fx = 14746; // Q14
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
  if(pit_sharp_fx < 3277) pit_sharp_fx = 3277;   // Q14

  /*-----------------------------------------------------------------*
   * - Include fixed-gain pitch contribution into impulse resp. h1[] *
   * - Correlation between target xn[] and impulse response h1[]     *
   * - Innovative codebook search                                    *
   *-----------------------------------------------------------------*/

  pit_shrp_fx(h1, T0, shl(pit_sharp_fx, 1), L_SUBFR);   // Q15 for pit_shrp_fx
  
  cod7_35_fx(xn, res2, l_subfr, h1, code, gain_code, index);

  for(i=0;i<L_CODE;i++) code[i]=shl(code[i], 11);   // Q11
  pit_shrp_fx(code, T0, shl(pit_sharp_fx, 1), l_subfr);

  return;
}

/*===================================================================*/
/* FUNCTION      :  pit_shrp_fx()	                                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16)  pit_lag       : Pitch lag.                          */
/*   _ (Word16)  sharp         : Pitch sharping factor.  Q15         */
/*   _ (Word16)  l_subfr       : subframe length.                    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS : _None                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : (Word16*) x  : impulse response. Q11     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*===================================================================*/

void pit_shrp_fx(
	Word16 *x,	/* in/out: impulse response (or algebraic code) */
	int pit_lag,	/* input : pitch lag                            */
	Word16 sharp,	/* input : pitch sharpening factor              */
	int L_subfr	/* input : subframe size                        */
)
{
  int i;
  
#ifdef WMOPS_FX
	  test();
#endif
  if (pit_lag < L_subfr)
    for (i = pit_lag; i < L_subfr; i++)
      x[i] = add(x[i], round32_16(L_mult(sharp, x[i - pit_lag])));         //Q(15+11+1-16)=Q11
}
