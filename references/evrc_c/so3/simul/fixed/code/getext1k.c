/**********************************************************************
Each of the companies; Motorola, Lucent, Nokia, Qualcomm, (hereinafter 
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
all Normative portions of the EVRC Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
/*     Copyright (C) 1997-1998 Telecommunications Industry Association. */
/*     All rights reserved.                                             */
/*----------------------------------------------------------------------*/
/* Note:  Reproduction and use of this software for the design and      */
/*     development of North American Wideband CDMA Digital              */
/*     Cellular Telephony Standards is authorized by the TIA.           */
/*     The TIA does not authorize the use of this software for any      */
/*     other purpose.                                                   */
/*                                                                      */
/*     The availability of this software does not provide any license   */
/*     by implication, estoppel, or otherwise under any patent rights   */
/*     of TIA member companies or others covering any use of the        */
/*     contents herein.                                                 */
/*                                                                      */
/*     Any copies of this software or derivative works must include     */
/*     this and all other proprietary notices.                          */
/*======================================================================*/
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  9                                           */
/*      Stack/Local RAM:    4                                           */
/*----------------------------------------------------------------------*/

/****************************************************************************
* Routine name: GetExc800bps.                                               *
* Function: Energy quantization of the residual signal.                     *
* Inputs: input - signal array.                                             *
*         length - size of signal array.                                    *
* Output: output - quantized signal.                                        *
****************************************************************************/
#include "macro.h"
#include "rom.h"
#include <stdio.h>
#include <math.h>
#include "mathevrc.h"
#include "mathdp31.h"
#include "mathadv.h"

short ran0(short *seed0)
{
long	Ltemp;
Ltemp = 0.0;
Ltemp = L_mac(27698, 25173, *seed0);
Ltemp = L_shr(Ltemp, 1);
Ltemp = Ltemp & 0x0000ffffL; 
*seed0 = extract_l(Ltemp);
return (extract_h(L_shl(Ltemp,15))); 
}

short ran_g(short *seed0)
{
	static int iset = 0;
	static long gset;
	long rsq, ltemp1, ltemp2;
	short sv1, sv2, rsq_s;
	short shft_fctr, stemp1;
	short shft_fctr1;

/* ======================================================================== */
	long ltmp1, ltmp2;
	short ans = 0;
	short stmp2;
/* ======================================================================== */
	if (iset == 0)
	{
		
		
			sv1 = shl(sub(ran0(seed0), 16384), 1);

			sv2 = shl(sub(ran0(seed0), 16384), 1);

			/* rsq = sv1 * sv1 + sv2 * sv2; */
			ltemp1 = L_mult(sv1, sv1);
			ltemp2 = L_mult(sv2, sv2);

			rsq = L_add(L_shr(ltemp1, 1), L_shr(ltemp2, 1));

		
		if (rsq >= 1073741824 || rsq == 0){
		  /* If condition not met, don't iterate; use */
		  /* rough approximation. */
		   ans = shr(sv1,3);
		   ans = add(ans, shr(sv2,3));
		   ans = add(ans, shr(sub(ran0(seed0), 16384),2));
		 
		   return (ans);
		}
/*
 * error in rsq doesn't seem to contribute to the final error in ran_g
 */

		/*
		 * rsq scale down by two: input to fnLog must be scaled up by 2.
		 */
		rsq = L_shl(rsq, 1);

		/* stemp1 = round(L_negate(fnLog(rsq)));  */
		ltmp1 = L_negate(fnLog(rsq));

		/*
		 * rsq must be greater than the log of lsq for the fractional
		 * divide to work. therfore normalize rsq.
		 */
		shft_fctr = norm_l(rsq); 
		rsq_s = round(L_shl(rsq, shft_fctr));
		stmp2 = (divide_s(round(ltmp1), rsq_s));

		/* 
		 * stemp2 must be normalized before taking its square root.
		 * (increases precision).
		 */
		shft_fctr1 = norm_s(stmp2);
		ltmp2 = L_deposit_h(shl(stmp2, shft_fctr1));
		stemp1 = sqroot(ltmp2);

		/* 
		 * shifting involved before taking the square root:
		 * LEFT << shft_fctr. (LEFT because rsq is in the denominator
		 *          of ltemp2 quotion).   
		 * LEFT << 6. (multiply by 2 in original code and multiply by 32
		 *        because output of fnLog scaled down by 32).
		 * RIGHT >> shft_fctr1. (normalization taken before sqroot).
		 */
		shft_fctr = shft_fctr + 6 - shft_fctr1;

		/*
		 * PROPERTY: sqrt(2^n) = 2^(n/2)
		 * if shft_fctr is odd; multiply stemp1 by sqrt(2)/2  and
		 * increment number of shifts by 1. Can now use shft_fctr / 2.
		 */
		if (shft_fctr & 0x0001)
		{
			stemp1 = mult(stemp1, 23170);
			shft_fctr++;
		}

		shft_fctr = shr(shft_fctr, 1);

		/*
		 * normalize stemp1 for the following multiplication.
		 * adjust shft_fctr accordingly.
		 */
		shft_fctr1 = norm_s(stemp1);
		stemp1 = shl(stemp1, shft_fctr1);
		shft_fctr = shft_fctr - shft_fctr1;

		gset = L_mult(sv1, stemp1);

		/*
		 * final output is scaled down by 4, therefore shift up by
		 * shft_fctr - 2.
		 */
		gset = L_shl(gset, shft_fctr - 2);

		iset = 1;
		return round(L_shl(L_mult(sv2, stemp1), shft_fctr - 2));
	}
	else
	{
		iset = 0;
		return round(gset);
	}
}


short e_ran_g(short *seed0)
{
	static int iset = 0;
	static long gset;
	long rsq, ltemp1, ltemp2;
	short sv1, sv2, rsq_s;
	short shft_fctr, stemp1;
	short shft_fctr1;

/* ======================================================================== */
	long ltmp1, ltmp2;
	short ans = 0;
	short stmp2;
/* ======================================================================== */
	if (iset == 0)
	{
		
		
			sv1 = shl(sub(ran0(seed0), 16384), 1);

			sv2 = shl(sub(ran0(seed0), 16384), 1);

			/* rsq = sv1 * sv1 + sv2 * sv2; */
			ltemp1 = L_mult(sv1, sv1);
			ltemp2 = L_mult(sv2, sv2);

			rsq = L_add(L_shr(ltemp1, 1), L_shr(ltemp2, 1));

		
		if (rsq >= 1073741824 || rsq == 0){
		  /* If condition not met, don't iterate; use */
		  /* rough approximation. */
		   ans = shr(sv1,3);
		   ans = add(ans, shr(sv2,3));
		   ans = add(ans, shr(sub(ran0(seed0), 16384),2));
		 
		   return (ans);
		}
/*
 * error in rsq doesn't seem to contribute to the final error in e_ran_g
 */

		/*
		 * rsq scale down by two: input to fnLog must be scaled up by 2.
		 */
		rsq = L_shl(rsq, 1);

		/* stemp1 = round(L_negate(fnLog(rsq)));  */
		ltmp1 = L_negate(fnLog(rsq));

		/*
		 * rsq must be greater than the log of lsq for the fractional
		 * divide to work. therfore normalize rsq.
		 */
		shft_fctr = norm_l(rsq); 
		rsq_s = round(L_shl(rsq, shft_fctr));
		stmp2 = (divide_s(round(ltmp1), rsq_s));

		/* 
		 * stemp2 must be normalized before taking its square root.
		 * (increases precision).
		 */
		shft_fctr1 = norm_s(stmp2);
		ltmp2 = L_deposit_h(shl(stmp2, shft_fctr1));
		stemp1 = sqroot(ltmp2);

		/* 
		 * shifting involved before taking the square root:
		 * LEFT << shft_fctr. (LEFT because rsq is in the denominator
		 *          of ltemp2 quotion).   
		 * LEFT << 6. (multiply by 2 in original code and multiply by 32
		 *        because output of fnLog scaled down by 32).
		 * RIGHT >> shft_fctr1. (normalization taken before sqroot).
		 */
		shft_fctr = shft_fctr + 6 - shft_fctr1;

		/*
		 * PROPERTY: sqrt(2^n) = 2^(n/2)
		 * if shft_fctr is odd; multiply stemp1 by sqrt(2)/2  and
		 * increment number of shifts by 1. Can now use shft_fctr / 2.
		 */
		if (shft_fctr & 0x0001)
		{
			stemp1 = mult(stemp1, 23170);
			shft_fctr++;
		}

		shft_fctr = shr(shft_fctr, 1);

		/*
		 * normalize stemp1 for the following multiplication.
		 * adjust shft_fctr accordingly.
		 */
		shft_fctr1 = norm_s(stemp1);
		stemp1 = shl(stemp1, shft_fctr1);
		shft_fctr = shft_fctr - shft_fctr1;

		gset = L_mult(sv1, stemp1);

		/*
		 * final output is scaled down by 4, therefore shift up by
		 * shft_fctr - 2.
		 */
		gset = L_shl(gset, shft_fctr - 2);

		iset = 1;
		return round(L_shl(L_mult(sv2, stemp1), shft_fctr - 2));
	}
	else
	{
		iset = 0;
		return round(gset);
	}
}

void GetExc800bps(
		/* 1 */ short *output,
		/* 2 */ short *best,
		/* 3 */ short scale,
		/* 4 */ short *input,
		/* 5 */ short length,
		/* 6 */ short flag,
		/* 7 */ short n)
{
	short k, j;
	short D;
	long tmp;
	long ltmp;
	short stmp;
	short *ptr;
	long sum, lscale;
	short ssum, sscale;
	static short Seed = 1234;
	static short Sum[NoOfSubFrames];
	short shft_scale;
	short shft_sum;
	short stemp1;

	if (!flag)
		Seed = 1234;

/*
 * sum is an integer value, not a fractional value.
 */
	/* Get energy of next sub frame */
	for (k = 0, sum = 0; k < length; k++)
	{
		sum = L_add(sum, L_deposit_l(abs_s(input[k])));
	}
	sum = L_shl(sum, 1);	/* correct for scaling */

	if (sum < SubFrameSize)
	{
		sum = fnLog10(L_deposit_h(scale));
		sum = L_negate(L_add(L_shl(sum, 3), 484942745));	/* add (log8)/4 = 484842745 (VALUE ADJUSTED TO BETTER MATCH FLOAT MODEL) */
	}
	else
	{
		lscale = L_mult(SubFrameSize, scale);
		shft_scale = norm_l(lscale); 
		sscale = round(L_shl(lscale, shft_scale)); 
		shft_sum = norm_l(sum) - 1; 
		ssum = round(L_shl(sum, shft_sum));

		/*
		 * The following divide/L_shl produced sum scaled down by 64.
		 * sum can have a max value of 40.xx in the sample data.
		 */
		/* sum = L_shl(L_divide(sum, lscale), (shft_scale + 7 - shft_sum)); */
		sum = L_deposit_h(shl(divide_s(ssum, sscale), (shft_scale - shft_sum)));
		/* sum = fnLog10(sum); */
		sum = L_add(fnLog10(sum), 141412467);   /* add log (2^7) scaled down by 32 */
		sum = L_add(L_shl(sum, 3), 969485490);	/* add (log8)/2 = 969685490 (VALUE ADJUSTED TO BETTER MATCH FLOAT MODEL) */
	}

	/*
	 * Sum scaled down by 4.
	 */
	Sum[n] = round(sum);

	/* Quantize if last frame */
	if (n == NoOfSubFrames - 1)
	{

		/* Quantize to 8 bits */
		for (k = 0, sum = 2147483647, ptr = Logqtbl; k < 256; k++)
		{
			for (j = 0, tmp = 0; j < 3; j++)
			{
				/*
				 * Sum and Logqtbl both scaled down by 4.
				 * Change Logqtbl to short if Lw not required. 
				 */
				D = sub(Sum[j], (*ptr++));
				tmp = L_mac(tmp, D, D);
			}

			if (tmp < sum)
			{
				ltmp = sum;
				sum = tmp;
				*best = k;
			}
		}

		for (j = 0; j < 3; j++)
		{
			Sum[j] = Powqtbl[*best * 3 + j];
		}

		/* Get excitation */
		j = FrameSize - ACBMemSize;
		for (k = 0; k < FrameSize - 1; k++)
		{
			if (k >= j)
			{
				stmp = e_ran_g(&Seed);
				stemp1 = k / (length - 1);
				output[k - j] = round(L_shr(L_mult(stmp, Sum[stemp1]), 5));
			}
		}
		stmp = e_ran_g(&Seed);
		output[k - j] = round(L_shr(L_mult(stmp, Sum[2]), 5));	/* last excitation */

	}
}

void GetExc800bps_dec(short *output, short length, short best, short flag, short n, short fer_flag)
{
	short i, j;
	short sum;
	long Ltemp;
	short temp;
	static short Seed = 1234;
	static short Sum[NoOfSubFrames];
	static short PrevBest = 0;

#define P333	10923			/* (1/3) */


	if (!flag && !n)
		Seed = 1234;

	if (n == 0)
	{
		/* De-quantize */
		if (fer_flag == 0)
		{
			for (j = 0; j < 3; j++)
				Sum[j] = Powqtbl[best * 3 + j];
				
			PrevBest = best;
		}
		else
		{
			for (j = 0, Ltemp = 0; j < 3; j++)
				Ltemp = L_mac(Ltemp, Powqtbl[PrevBest * 3 + j], P333);
				
			for (j = 0; j < 3; j++)
				Sum[j] = round(Ltemp);
				
		}
	}

	/* Convert to linear domain */
	sum = Sum[n];

/* NOTE: Logqtbl[] and pow() function has been replaced with Powqtbl[] */

	for (i = 0; i < length; i++)
	{
		temp = ran_g(&Seed); 
		Ltemp = L_mult(sum, temp);
		Ltemp = L_shr(Ltemp, 5);
		output[i] = round(Ltemp);
	}
}
