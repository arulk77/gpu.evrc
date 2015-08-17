
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
/*  4GV - Fourth Generation Vocoder Speech Service Option for           */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
/* This file contains Qualcomm math library functions as implemented    */
/* in DSP. It can be considered an extension to the fixed point library */
/* functions.                                                           */
/*----------------------------------------------------------------------*/

#include "typedef_fx.h"
#include "dsp_math.h"
#include "basic_op.h"
#include "basic_op40.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

#define __INIT__

#if defined(__INIT__)
#include <assert.h>
#include <cstdlib>
#include <iostream>
using namespace std;
#endif

/***************************************************************************
 *
 *   FUNCTION NAME: L_negate40
 *
 *   PURPOSE:
 *
 *     Negate the 40 bit input. 0x80 0000 0000's negated value is
 *     0x7f ffff ffff.
 *
 *   INPUTS:
 *
 *     L_var1
 *                     40 bit long signed integer (Word40) whose value
 *                     falls in the range
 *                     0x80 0000 0000 <= L_var1 <= 0x7f ffff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     40 bit long signed integer (Word40) whose value
 *                     falls in the range
 *                     0x80 0000 0001 <= L_var1 <= 0x7f ffff ffff.
 *
 *   KEYWORDS: negate, negative
 *
 *************************************************************************/

Word40 L_negate40(Word40 L_var1)
        {
	  Word40 L_Out;
	  
	  
	  if (L_var1 == MIN_40)
	    {
	      L_Out = MAX_40;
	      giOverflow = 1;
	    }
	  else
	    L_Out = -L_var1;
	  
#ifdef WMOPS_FX
	  counter_fx.L_negate++;
#endif
	  
	  return (L_Out);
	}


/***************************************************************************

 *   FUNCTION NAME: L_mult_su
 *
 *   PURPOSE:
 *
 *     Perform multipy of a signed 16-bit number and an unsigned 16-bit 
 *     number with saturation.  Output a 40 bit number.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short signed integer (Word16) whose value
 *                     falls in the range 0xffff 8000 <= var1 <= 0x0000 7fff.
 *     var2
 *                     16 bit short unsigned integer whose value
 *                     falls in the range 0x0000 <= var2 <= ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     40 bit long signed integer (Word40) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *
 *   KEYWORDS: multiply, mult, mpy
 *
 *************************************************************************/
Word40 L_mult_su(Word16 var1, UNS_Word16 var2)
{
  Word40 L_product;
	  
  L_product = (Word32) (var1 *var2);	/* integer multiply */
  
#ifdef WMOPS_FX
	  counter_fx.L_mult++;
#endif
  return (L_product);
}


/***************************************************************************

 *   FUNCTION NAME: L_mac40_su
 *
 *   PURPOSE:

 *   Multiply signed var1 by unsigned var2 and shift the result left by 1.
     Add the 40 bit result to L_var3 with saturation, return a 40 bit result:
 *        L_mac(L_var3,var1,var2) = L_add(L_var3,(L_mult(var1,var2)).        *
 *                                                                           *
 *   Complexity weight : 1                                                   *
 *                            
 *    
 *    Inputs :                                                               * 
 
 *    L_var3   40 bit long signed integer (Word40) whose value falls in the  *
 *             range : MIN_40 <= L_var3 <= MAX_40.                           *
 *                                                                           * 
 *    var1     16 bit short signed integer (Word16) whose value falls in     *
 *             the : MIN_16 <= var1 <= MAX_16.                               *
 *                                                                           *
      var2     16 bit short unsigned integer whose value falls in 
               the range 0x0000 <= var2 <= ffff.
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *    L_var_out                                                              *
 *             40 bit long signed integer (Word40) whose value falls in the  *
 *             range : MIN_40 <= L_var_out <= MAX_40.                        *
 *
 *
 *   KEYWORDS: multiply, mult, mpy
 *
 *************************************************************************/
Word40 L_mac40_su(Word40 L_var3, Word16 var1, UNS_Word16 var2)
{
  Word40 L_var_out;
	  
  L_var_out = L_var3 + (Word40) (var1 *var2);	

  //L_var_out = L_saturate40(L_var_out);
  
#ifdef WMOPS_FX
	  counter_fx.L_mac40++;
#endif
  return (L_var_out);
}


/***************************************************************************

 *   FUNCTION NAME: L_mult_uu
 *
 *   PURPOSE:
 *
 *     Perform multipy of an unsigned 16-bit number and an unsigned 16-bit 
 *     number with saturation.  Output a 40 bit number.
 *
 *   INPUTS:
 *
 *     var1
 *                     16 bit short unsigned integer whose value
 *                     falls in the range 0x0000 <= var2 <= ffff.
 *     var2
 *                     16 bit short unsigned integer whose value
 *                     falls in the range 0x0000 <= var2 <= ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     40 bit long signed integer (Word40) whose value
 *                     falls in the range
 *                     0x8000 0000 <= L_var1 <= 0x7fff ffff.
 *
 *   IMPLEMENTATION:
 *
 *
 *   KEYWORDS: multiply, mult, mpy
 *
 *************************************************************************/
Word40 L_mult_uu(UNS_Word16 var1, UNS_Word16 var2)
{
  unsigned long L_product;
  Word40 temp;

  L_product = (var1 *var2);	/* integer multiply */
  temp=L_product;
#ifdef WMOPS_FX
	  counter_fx.L_mult++;
#endif
  return (temp);
}

/***************************************************************************

 *   FUNCTION NAME: L_mult_ll
 *
 *   PURPOSE:
 *
 *     Perform multipy of a signed 32-bit number and another signed 32-bit 
 *     number with saturation.  Output a 32 bit number.
 *
 *   INPUTS:
 *
 *     var1
 *                     32 bit long signed integer (Word32) whose value
 *                     falls in the range 0x8000 0000<= var1 <= 0x7fff ffff.
 *     var2
 *                     32 bit long signed integer whose value
 *                     falls in the range 0x8000 0000<= var1 <= 0x7fff ffff.
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     L_Out
 *                     40 bit long signed integer (Word40)
 *
 *   IMPLEMENTATION:
 *
 *
 *   KEYWORDS: multiply, mult, mpy
 *
 *************************************************************************/
Word40 L_mult_ll(Word32 var1, Word32 var2)
{
  Word16 d1h, d1l, d2h, d2l;
  Word40 L_product;

  d1h=extract_h(var1);
  d1l=extract_l(var1);
  d2h=extract_h(var2);
  d2l=extract_l(var2);

  L_product=L_mult_su(d1h,d2l);
  L_product=L_mac40_su(L_product,d2h,d1l);

  L_product=L_add40(L_shr40(L_product,15),L_mult(d1h,d2h));
  
  return (L_product);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add40_40                                              |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Add L_var1 by L_var2. Return a 40 bit result without saturation:        |
 |        L_add40_40 (L_var1, L_var2).                                       |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   40 bit long signed integer (Word40) whose value falls in the  |
 |             range : MIN_40 <= L_var1 <= MAX_40.                           |
 |                                                                           |
 |                                                                           |
 |    L_var2   40 bit long signed integer (Word40) whose value falls in      |
 |             the range: MIN_40 <= L_var2 <= MAX_40.                        |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             40 bit long signed integer (Word40) whose value falls in the  |
 |             range : MIN_40 <= L_var_out <= MAX_40.                        |
 |___________________________________________________________________________|
*/

Word40 L_add40_40(Word40 L_var1, Word40 L_var2)
{
  Word40 L_var_out;

  L_var_out = L_var1 + L_var2;

  if (L_var_out > MAX_40)
  {
    L_var_out = MAX_40;
    giOverflow = 1;
  }
  else if (L_var_out < MIN_40)
  {
    L_var_out = MIN_40;
    giOverflow = 1;
  }

#ifdef WMOPS_FX
	  counter_fx.L_add++;
#endif
  return(L_var_out);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub40_40                                              |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Subtract L_var1 by L_var2. Return a 40 bit result without saturation:   |
 |        L_sub40_40 (L_var1, L_var2).                                       |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   40 bit long signed integer (Word40) whose value falls in the  |
 |             range : MIN_40 <= L_var1 <= MAX_40.                           |
 |                                                                           |
 |                                                                           |
 |    L_var2   40 bit long signed integer (Word40) whose value falls in      |
 |             the range: MIN_40 <= L_var2 <= MAX_40.                        |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             40 bit long signed integer (Word40) whose value falls in the  |
 |             range : MIN_40 <= L_var_out <= MAX_40.                        |
 |___________________________________________________________________________|
*/

Word40 L_sub40_40(Word40 L_var1, Word40 L_var2)
{
  Word40 L_var_out;

  L_var_out = L_var1 - L_var2;

  if (L_var_out > MAX_40)
  {
    L_var_out = MAX_40;
    giOverflow = 1;
  }
  else if (L_var_out < MIN_40)
  {
    L_var_out = MIN_40;
    giOverflow = 1;
  }

#ifdef WMOPS_FX
	  counter_fx.L_sub++;
#endif
  return(L_var_out);
}

/*===================================================================*/
/* FUNCTION      :  dsp_sqrt_lut ().                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  squared root by look up table                    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word32)      Linput: input in Q0                       */
/*         _ (Word32)      Lround: rounding bit                      */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word32) Lout: squared root of Linput,  Q16            */
/*                     0 if Linput<0                                 */
/*===================================================================*/
/* Note:        If input is larger or equal to 0x40000000, the       */
/*              output would overflow.  For that case, should        */
/*              saturate before using it.                            */
/*===================================================================*/
static Word16 sqrt_table[25]={
    16384, 17382, 18322, 19215, 20069, 20888, 21676, 22437,
    23172, 23885, 24578, 25251, 25907, 26546, 27171, 27782,
    28379, 28964, 29538, 30100, 30652, 31195, 31728, 32253,
    32768};


Word32 dsp_sqrt_lut(Word32 Linput, Word32 Lround)
{
  Word32 Ltemp1, Ltemp2;
  Word16 n1, n2, n3, val1, val2;
  
  if (Linput<=0) return(0);

  n1=norm_l(Linput);
  Ltemp1=L_shl(Linput,n1);
  Ltemp1=L_shr(Ltemp1,10);

  Ltemp2=L_mult(n1,0xC000);
  n1=extract_h(Ltemp2);
  if ((Ltemp2 & 0x8000)==0) Ltemp1=L_shr(Ltemp1,1);
  n2=extract_h(Ltemp1);
  n3=extract_l(Ltemp1);
  val1=sqrt_table[sub(n2,8)];
  Ltemp2=L_deposit_h(val1);
  Ltemp2=L_sub(Ltemp2,(Word32) L_mult_su(val1,(UNS_Word16)n3));
  n1=add(n1,1);
  val2=sqrt_table[sub(n2,7)];

  Ltemp2=L_add(Ltemp2,(Word32) L_mult_uu((UNS_Word16)val2,(UNS_Word16)n3));
  
  Ltemp2=L_shl(Ltemp2,n1);
  return(L_add(Lround,Ltemp2));

}

/*===================================================================*/
/* FUNCTION      :  divide_dp ().                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  double precision division                        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word40)      Lnum:   numerator                         */
/*         _ (Word40)      Ldenom: denominator                       */
/*         _ (Word16)      Qadj: Q factor adjustor                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _(Word32)       Lquotient in Q(29+Qadj)                   */
/*                         0 if Ldenom=0                             */
/*===================================================================*/
Word40 divide_dp(Word40 Lnum, Word40 Ldenom, Word16 Qadj)
{
  Word40 Lacc;
  Word32 Ltemp1, Ltemp2, Ltemp3, Lm2, ltmp3;
  Word16 dl,dh, al,ah, ch, cl;
  Word16 n1,n3, i;
  
  if (Ldenom==0) return(0);

  n1=norm32_l40(Lnum);  // normalize numerator
  Ltemp1=(Word32) L_shl40(Lnum,n1);
  n3=sub(n1,Qadj);
  n1=norm32_l40(Ldenom); // normalize denominator 
  Ltemp2=(Word32) L_shl40(Ldenom,n1);
  Ltemp2=L_abs(Ltemp2);  // take only positive value
  n3=sub(n1,n3);         // n3 is final Q adjustor
  
  Ltemp3=0x55555555;     // Initial guess of invert
  Lm2=0xc0000000;        // Lm2=-2 in Q29
  //  Lround=0xffff8000;
  dl=extract_l(Ltemp2);  // LSW of denominator
  dh=extract_h(Ltemp2);  // MSW of denominator 

#if defined(__INIT__)
  /* Use short division for initialization */
  ah=divide_s(0x4000,dh);  // Up to 16-bit accuracy.

  /* Use only 1 iteration for Newton divide algorithm. */
  Lacc=L_mac40_su(0x00008000,ah,(UNS_Word16)dl);
/*    Lacc=L_mac40_su(Lacc,dh,(UNS_Word16)al);*/   // al=0.
  Lacc=L_add40(L_shr40(Lacc,16),Lm2);
  Lacc=L_add40(Lacc,L_shr(L_mult(ah,dh),1));
  ch=extract_h((Word32) Lacc);
  cl=extract_l((Word32) Lacc);

  Lacc=L_mult_su(ah,cl);
/*    Lacc=L_mac40_su(Lacc,ch,al);*/               // al=0.
  Lacc=L_shl40(Lacc,3);
  Lacc=L_negate40(L_sub40(Lacc,0xffff8000));
  Lacc=L_sub40(L_shl40(L_mult(ah,ch),2),(Word32)L_shr40(Lacc,16));
  ltmp3=L_negate((Word32)L_sat32_40(Lacc));

#if defined(__DUMP_DELTA__)
  //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  for (i=0;i<5;i++) {                                                 // This block
    al=extract_l(Ltemp3);                                             //  is no
    ah=extract_h(Ltemp3);                                             //  longer 
    Lacc=L_mac40_su(0x00008000,ah,(UNS_Word16)dl);                    //  needed, if
    Lacc=L_mac40_su(Lacc,dh,(UNS_Word16)al);                          //  16-bit
    Lacc=L_add40(L_shr40(Lacc,16),Lm2);                               //  accuarcy
    Lacc=L_add40(Lacc,L_shr(L_mult(ah,dh),1));                        //  initialization
    ch=extract_h((Word32) Lacc);                                      //  is used.
    cl=extract_l((Word32) Lacc);                                      //
                                                                      // Keeping this
    Lacc=L_mult_su(ah,cl);                                            //  block is just
    Lacc=L_mac40_su(Lacc,ch,al);                                      //  for comparison
    Lacc=L_shl40(Lacc,3);                                             //  purpose.
    Lacc=L_negate40(L_sub40(Lacc,0xffff8000));                        //
    Lacc=L_sub40(L_shl40(L_mult(ah,ch),2),(Word32)L_shr40(Lacc,16));  //
    Ltemp3=L_negate((Word32)L_sat32_40(Lacc));                        //
  }                                                                   //
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  Word32 acc;                                                         // This block is
  Word32 delta;                                                       //  is purely
  Word32 truevalue;                                                   //  for comparision
  delta=labs(Ltemp3-ltmp3);                                           //  purpose only.
  if (delta>0x8) {                                                    // It displays the
    /*Compute true 32-bit reciprocal value for comparison purpose.*/  //  true 32-bit
    truevalue=0;                                                      //  reciprocal
    acc=0x4000L<<16;                                                  //  value when
    for (i=0; i<32; i++) {                                            //  the discrepancy
      emph=acc-Ltemp2;                                                //  between the
      acc=(emph >= 0)?(emph<<1):acc<<1;                               //  short-cut
      truevalue<<=1;                                                  //  version and
      if (emph >= 0) truevalue++;  /* Add one to the LSB.*/           //  the full 5
    }                                                                 //  iteration
    /*............................................................*/  //  version.
    cout<<" 1/"<<hex<<Ltemp2<<"  "                                    //
        <<truevalue<<"  "<<Ltemp3<<" vs. "<<ltmp3<<"  delta="<<"0x"   //
        <<delta<<endl; // Print in hex.                               //
  }                                                                   //
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#endif //__DUMP_DELTA__

  Ltemp3=ltmp3; // Use single iteration result instead of 5 iteration Newton method.
#else //__INIT__
  for (i=0;i<5;i++) {
    al=extract_l(Ltemp3);
    ah=extract_h(Ltemp3);
    Lacc=L_mac40_su(0x00008000,ah,(UNS_Word16)dl);
    Lacc=L_mac40_su(Lacc,dh,(UNS_Word16)al);
    Lacc=L_add40(L_shr40(Lacc,16),Lm2);
    Lacc=L_add40(Lacc,L_shr(L_mult(ah,dh),1));
    ch=extract_h((Word32) Lacc);
    cl=extract_l((Word32) Lacc);
    
    Lacc=L_mult_su(ah,cl);
    Lacc=L_mac40_su(Lacc,ch,al);
    Lacc=L_shl40(Lacc,3);
    Lacc=L_negate40(L_sub40(Lacc,0xffff8000));
    Lacc=L_sub40(L_shl40(L_mult(ah,ch),2),(Word32)L_shr40(Lacc,16));
    Ltemp3=L_negate((Word32)L_sat32_40(Lacc));
  }
#endif //__INIT__

  if (Ldenom<0) Ltemp3=L_negate(Ltemp3);
  al=extract_l(Ltemp3);
  ah=extract_h(Ltemp3);
  cl=extract_l(Ltemp1);
  ch=extract_h(Ltemp1);
  Lacc=L_mac40_su(0x00008000,ah,cl);
  Lacc=L_mac40_su(Lacc,ch,al);
  Lacc=L_add40(L_shr40(Lacc,16),L_shr(L_mult(ah,ch),1));
  return(L_shl40(Lacc,n3));
  
}

/*===================================================================*/
/* FUNCTION      :  sqrt_divide_dp ().                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  compute sqrt(x/y)                                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word40)      x:   numerator                            */
/*         _ (Word40)      y: denominator                            */
/*         _ (Word16)      Qdif: Q factor difference (Qx-Qy)         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16)      Qout: Q factor of sqrt(x/y)               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _(Word40)       sqrt(x/y) in Qout                         */
/*                         0 if Ldenom=0                             */
/*===================================================================*/
Word32 sqrt_divide_dp(Word40 x, Word40 y, Word16 Qdif, Word16 *Qout)
{
  Word32 Ltemp1, Ltemp2;
  Word16 n1, n2;

  n1=norm32_l40(x);
  Ltemp1=(Word32)L_shl40(x,n1);
  n1=add(n1,Qdif);
  n2=norm32_l40(y);
  Ltemp2=(Word32)L_shl40(y,n2);
  n2=sub(n2,29);
  n1=sub(n1,n2);
  Ltemp1=(Word32)divide_dp(Ltemp1,Ltemp2,0);
  n1=sub(n1,1);
  if ((n1&1)==0) Ltemp1=L_shr(Ltemp1,1);
  Ltemp1=dsp_sqrt_lut(Ltemp1,0);
  Ltemp2=L_shr(L_mult(n1,0x8000),1);
  *Qout=extract_h(Ltemp2);
  *Qout=sub(16,*Qout);
  return(Ltemp1);
}


/*===================================================================*/
/* FUNCTION      :  log10_lut ().                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute 10log10 by look up table                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word40)      Linput: input in Q0                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word32) Lout: 10log10(Linput),  Q23                   */
/*                     0x8000 0000 if Linput<=0                      */
/*===================================================================*/
static Word16 log2_table[33]={
      0x7800, 0x782D, 0x785A, 0x7884, 0x78AE, 0x78D6, 0x78FE, 0x7924,
      0x794A, 0x796E, 0x7992, 0x79B4, 0x79D6, 0x79F8, 0x7A18, 0x7A38,
      0x7A57, 0x7A75, 0x7A93, 0x7AB1, 0x7ACD, 0x7AE9, 0x7B05, 0x7B20,
      0x7B3B, 0x7B55, 0x7B6F, 0x7B88, 0x7BA1, 0x7BB9, 0x7BD1, 0x7BE9, 
      0x7C00};
Word32 log10_lut(Word40 Linput)
{
  Word16 n1, frac, p1, p2, q1;
  Word32 Ltemp1, Ltemp2;

  if (Linput<=0) return(LW_MIN);
  n1=norm32_l40(Linput);
  Ltemp1=(Word32)L_shl40(Linput,n1);
  
  Ltemp2=L_mult(extract_h(Ltemp1),0x40);
  frac=extract_l(Ltemp2);

  p1=log2_table[sub(extract_h(Ltemp2),0x20)];
  p2=log2_table[sub(extract_h(Ltemp2),0x1F)];
  Ltemp2=L_mult(n1,0x200);
  n1=extract_l(Ltemp2);
  
  Ltemp1=L_add(L_deposit_h(p1),0x8000); // Add rounding bit
  Ltemp1=L_sub(Ltemp1,(Word32)L_mult_su(p1,frac));
  Ltemp1=L_add(Ltemp1,(Word32)L_mult_su(p2,frac));
  q1=extract_h(Ltemp1);
  Ltemp1=L_mult(q1,0x6054);
  Ltemp1=L_msu(Ltemp1,0x6054,n1);
  return(L_shr(Ltemp1,1));
      
}

/*===================================================================*/
/* FUNCTION      :  log2_lut ().                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute log2 by look up table                    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word40)      Linput: input in Q0                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word40) Lout: log2(Linput),  Q26                      */
/*                     0x8000 0000 if Linput<=0                      */
/*===================================================================*/
Word40 log2_lut(Word40 Linput)
{
  Word16 n1, frac, p1, p2;
  Word32 Ltemp1, Ltemp2;
  Word40 Lacc;

  if (Linput<=0) return(LW_MIN);
  n1=norm32_l40(Linput);
  Ltemp1=(Word32)L_shl40(Linput,n1);
  
  Ltemp2=L_shr(Ltemp1,9);
  frac=extract_l(Ltemp2);

  p1=log2_table[sub(extract_h(Ltemp2),0x20)];
  p2=log2_table[sub(extract_h(Ltemp2),0x1F)];
  Ltemp2=L_mult(n1,0x200);
  
  Ltemp1=L_deposit_h(p1);
  Ltemp1=L_sub(Ltemp1,(Word32)L_mult_su(p1,frac));
  Ltemp1=L_add(Ltemp1,(Word32)L_mult_su(p2,frac));
  Lacc=L_shl40(Ltemp2,16);
  return(L_sub40_40(Ltemp1,Lacc));
      
}


/*===================================================================*/
/* FUNCTION      :  invert_lut ().                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  invert by look up table                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word32)      Linput (>0): input in Qin                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16)      Qout                                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word32) Lout: 1/Linput,  in Q(61-Qin-Qout)            */
/*===================================================================*/
/* Note:        Linput must be >0                                    */
/*===================================================================*/
#define INVBITS 7
static Word16 inv_table[65]={
0x7FFF, 0x7E07, 0x7C1E, 0x7A44, 0x7877, 0x76B8, 0x7506, 0x7360,
0x71C6, 0x7037, 0x6EB3, 0x6D39, 0x6BC9, 0x6A63, 0x6906, 0x67B2,
0x6666, 0x6522, 0x63E6, 0x62B2, 0x6186, 0x6060, 0x5F41, 0x5E29,
0x5D17, 0x5C0B, 0x5B05, 0x5A05, 0x590B, 0x5816, 0x5726, 0x563B,
0x5555, 0x5474, 0x5397, 0x52BF, 0x51EB, 0x511C, 0x5050, 0x4F88,
0x4EC5, 0x4E05, 0x4D48, 0x4C8F, 0x4BDA, 0x4B28, 0x4A79, 0x49CD,
0x4924, 0x487F, 0x47DC, 0x473C, 0x469F, 0x4604, 0x456C, 0x44D7,
0x4444, 0x43B4, 0x4326, 0x429A, 0x4210, 0x4189, 0x4104, 0x4081,
0x4000};

Word32 invert_lut(Word32 Linput, Word16 *Qout)
{
  Word16 n1, xh, xl, p1, p2;
  Word32 Ltemp1;

  n1=norm_l(Linput);
  *Qout=n1;
  Ltemp1=L_shl(Linput,n1);

  Ltemp1=L_shr(Ltemp1,15-INVBITS);
  xh=extract_h(Ltemp1);
  xl=extract_l(Ltemp1);

  p1=inv_table[sub(xh,64)];
  p2=inv_table[sub(xh,63)];

  Ltemp1=L_deposit_h(p1);
  Ltemp1=L_sub(Ltemp1,(Word32) L_mult_su(p1,xl));
  Ltemp1=L_add(Ltemp1,(Word32) L_mult_su(p2,xl));
  return(Ltemp1);
  
}


/*===================================================================*/
/* FUNCTION      :  invert_dp ().                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  invert by newton recursion                       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word40)      Linput: input in Qin                      */
/*         _ (Word16)      n: number of iterations                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16)      Qout                                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word32) Lout: 1/Linput,  in Q(61-Qin-Qout)            */
/*===================================================================*/
/* Note:        Linput can be negative, if Linput=0, return 0        */
/*===================================================================*/
Word32 invert_dp(Word40 Linput, Word16 n, Word16 *Qout)
{
  Word40 Lacc;
  Word32 Ltemp2, Ltemp3, Lm2;
  Word16 dl,dh, al,ah, ch, cl;
  Word16 n1,i;

  if (Linput==0) return(0);

  n1=norm32_l40(Linput);  // normalize Linput
  Ltemp2=(Word32) L_shl40(Linput,n1);
  Ltemp2=L_abs(Ltemp2);  // take only positive value
  
  Ltemp3=0x55555555;     // Initial guess of invert
  Lm2=0xc0000000;        // Lm2=-2 in Q29
  //  Lround=0xffff8000;
  dl=extract_l(Ltemp2);  // LSW of denominator
  dh=extract_h(Ltemp2);  // MSW of denominator 

#if defined(__INIT__)
  Word32 acc,emph;
  Word32 ltmp3;
  Word16 hi;

  assert(n > 2);  // If n<=2, the original method is more efficient.
  ah=divide_s(0x4000,dh);
  Lacc=L_mac40_su(0x00008000,ah,(UNS_Word16)dl);
/*  Lacc=L_mac40_su(Lacc,dh,(UNS_Word16)al);*/    // al=0.
  Lacc=L_add40(L_shr40(Lacc,16),Lm2);
  Lacc=L_add40(Lacc,L_shr(L_mult(ah,dh),1));
  ch=extract_h((Word32) Lacc);
  cl=extract_l((Word32) Lacc);

  Lacc=L_mult_su(ah,cl);
/*  Lacc=L_mac40_su(Lacc,ch,al);*/                // al=0.
  Lacc=L_shl40(Lacc,3);
  Lacc=L_negate40(L_sub40(Lacc,0xffff8000));
  Lacc=L_sub40(L_shl40(L_mult(ah,ch),2),(Word32)L_shr40(Lacc,16));
  Ltemp3=L_sat32_40(Lacc);

  if (Linput>=0) Ltemp3=L_negate(Ltemp3);
#else
  for (i=0;i<n;i++) {
    al=extract_l(Ltemp3);
    ah=extract_h(Ltemp3);
    Lacc=L_mac40_su(0x00008000,ah,(UNS_Word16)dl);
    Lacc=L_mac40_su(Lacc,dh,(UNS_Word16)al);
    Lacc=L_add40(L_shr40(Lacc,16),Lm2);
    Lacc=L_add40(Lacc,L_shr(L_mult(ah,dh),1));
    ch=extract_h((Word32) Lacc);
    cl=extract_l((Word32) Lacc);
    
    Lacc=L_mult_su(ah,cl);
    Lacc=L_mac40_su(Lacc,ch,al);
    Lacc=L_shl40(Lacc,3);
    Lacc=L_negate40(L_sub40(Lacc,0xffff8000));
    Lacc=L_sub40(L_shl40(L_mult(ah,ch),2),(Word32)L_shr40(Lacc,16));
    Ltemp3=L_negate((Word32)L_sat32_40(Lacc));
  }
  if (Linput<0) Ltemp3=L_negate(Ltemp3);
#endif
  *Qout=n1;
  return (Ltemp3);
}


/*===================================================================*/
/* FUNCTION      :  dsp_pow10                                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  compute pow(10,x)                                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word32)      x: input in Q26                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*          _ (Word40) Lout: pow(10,x) in Q15                        */
/*===================================================================*/

Word40 dsp_pow10(Word32 x)
{
  Word16 xl,xh, t1, t2, n;
  Word32 Ltemp1;
  Word40 Lacc;

  xl=extract_l(x);
  xh=extract_h(x);
  
  Ltemp1=(Word32) L_mult_su(0x6a4d,xl);
  Ltemp1=L_add(L_shr(Ltemp1,16),L_shr(L_mult(xh,0x6a4d),1));
  
  
  Lacc=L_shl40(Ltemp1,9);
  Lacc=L_sub40(L_negate40(Lacc),1); // Lacc=~Lacc, 1's complement
  t1=extract_l((Word32)L_shr40(Lacc,16));
  
  Ltemp1=L_shr(Ltemp1,7);
  t2=extract_h(Ltemp1);
  n=sub(t2,15);
  
  t2=extract_l((Word32)L_shr40(L_mult_uu(t1,t1),16));
  
  Lacc=(Word40) L_deposit_h(0x7fbd);
  Lacc=L_add40(Lacc,(Word32)L_mult_uu(t2,0x15ef));
  Lacc=L_sub40(Lacc,(Word32)L_mult_uu(t1,0x556f));
  return(L_shl40(Lacc,n));

}


/*===================================================================*/
/* FUNCTION      :  div_int_sp                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  compute n/m and n%m, single precision            */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      n: numerator in Q0                        */
/*         _ (Word16)      m: denominator in Q0                      */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16)      r: r=n%m, remainder in Q0                 */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word16)      q: q=n/m, quotient in Q0                  */
/*===================================================================*/
/* Note:  If n or m is less than or equal to 0, return 0 in q        */
/*===================================================================*/
Word16 div_int_sp(Word16 n, Word16 m, Word16 &r)
{
  Word16 i, q1, q2, qd;
  Word32 Ltemp2;
  Word40 Lacc;

  if (n<=0 || m<=0 || n<m) {
    r=n;
    return (0);
  } 

  q1=norm_s(n);
  Lacc=L_deposit_h(shl(n,q1));
  qd=sub(q1,1);
  q2=norm_s(m);
  Ltemp2=L_deposit_h(shl(m,q2));
  qd=sub(q2,qd);
  q2=add(q2,1);

  for (i=0;i<qd;i++) {
    Lacc=L_sub40(Lacc,Ltemp2);
    if (Lacc>=0) 
      Lacc=L_add40(L_shl40(Lacc,1),1);
    else {
      Lacc=L_add40(Lacc,Ltemp2);
      Lacc=L_shl40(Lacc,1);
    }
  }
  q1=extract_l40(Lacc);
  Ltemp2=(Word32)L_sat32_40(L_shr40(Lacc,q2));
  r=extract_h(Ltemp2);
  return(q1);
}


/*===================================================================*/
/* FUNCTION      :  div_int_dp                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  compute n/m and n%m, double precision            */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word32)      n: numerator in Q0                        */
/*         _ (Word32)      m: denominator in Q0                      */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word32)      r: r=n%m, remainder in Q0                 */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ None                                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word32)      q: q=n/m, quotient in Q0                  */
/*===================================================================*/
/* Note:  If n or m is less than or equal to 0, return 0 in q        */
/*===================================================================*/
Word32 div_int_dp(Word32 n, Word32 m, Word32 &r)
{
  Word16 i, q1, q2, qd;
  Word32 Ltemp2, qo;
  Word40 Lacc;

  if (n<=0 || m<=0 || n<m) {
    r=n;
    return (0);
  } 

  q1=norm_l(n);
  Lacc=L_shl(n,q1);
  qd=sub(q1,1);
  q2=norm_l(m);
  Ltemp2=L_shl(m,q2);
  qd=sub(q2,qd);
  q2=add(q2,1);
  qo=0;
  for (i=0;i<qd;i++) {
    Lacc=L_sub40(Lacc,Ltemp2);
    qo=L_shl(qo,1);
    if (Lacc>=0) {
      Lacc=L_shl40(Lacc,1);
      qo=L_add(qo,1);
    }
    else {
      Lacc=L_add40(Lacc,Ltemp2);
      Lacc=L_shl40(Lacc,1);
    }
  }
  r=(Word32)L_sat32_40(L_shr40(Lacc,q2));
  return(qo);
}
