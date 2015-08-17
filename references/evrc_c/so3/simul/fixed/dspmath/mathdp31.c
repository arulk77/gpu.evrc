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
/***************************************************************************
 *
 *   File Name:  mathdp31.c
 *
 *   Purpose:  Contains functions increased-precision arithmetic operations.
 *             Unsigned multiplication is used to calculate interim products.
 *
 *      Below is a listing of all the functions in this file.  There
 *      is no interdependence among the functions.
 *
 *      L_mpy_ls()
 *      L_mpy_ll()
 *
 *
 *  There are two types of double precision multiplies supported in this
 *  file. The default uses unsigned multiplication for the interim steps.
 *  The alternate may be used when compiling the library by defining
 *  "USE_ALT_DP31" on the compile line. See the makefile for more info.
 *
 ***************************************************************************/
/*_________________________________________________________________________
 |                                                                         |
 |                              Include Files                              |
 |_________________________________________________________________________|
*/
#include <math.h>
#include "mathevrc.h"
#include "typedefs.h"

#ifndef USE_ALT_DP31
/****************************************************************************
 *
 *
 *     FUNCTION NAME: L_mpy_ll
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var1) and a 32 bit number
 *                 (L_var2), and return a 32 bit result.
 *
 *     INPUTS:
 *
 *       L_var1             A Longword input variable
 *
 *       L_var2             A Longword input variable
 *
 *     OUTPUTS:             none
 *
 *     IMPLEMENTATION:
 *
 *        Performs a 32x32 bit multiply, Complexity=?? ops
 *
 *        Let x1x0, or y1y0, be the two constituent halves
 *        of a 32 bit number.  This function performs the
 *        following:
 *
 *        low = [(unsigned)x0 * (unsigned)y0] <<1;  (low * low)
 *        mid1 = [(unsigned)x0 * y1] << 1;          (low * high)
 *        mid2 = [(unsigned)y0 * x1] << 1;          (low * high)
 *        mid =  [mid1 + mid2] >> 16 ;              (sum so far)
 *        output = (x1*y1)<<1 + mid;                (high * high)
 *
 *        Note that this version uses unsigned * signed to reduce complexity
 *        and gain precision.
 *
 *
 *     RETURN VALUE:        A Longword value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/
Longword L_mpy_ll(Longword L_var1, Longword L_var2)
{
	double  aReg;
    Longword lvar;

    OP_COUNT(3);
    /* (unsigned)low1 * (unsigned)low1 */
    aReg = (double)(0xffff & L_var1) * (double)(0xffff & L_var2) * 2.0;

    OP_COUNT(1);
    /* >> 16 */
    aReg = (aReg / 65536);
    aReg = floor(aReg);

    OP_COUNT(1);
    /* (unsigned)low1 * (signed)high2 */
    aReg += (double)(0xffff & L_var1) * ((double)L_shr(L_var2,16)) * 2.0;

    OP_COUNT(1);
    /* (unsigned)low2 * (signed)high1 */
    aReg += (double)(0xffff & L_var2) * ((double)L_shr(L_var1,16)) * 2.0;

    OP_COUNT(1);
    /* >> 16 */
    aReg = (aReg / 65536);
    aReg = floor(aReg);

    OP_COUNT(1);
    /* (signed)high1 * (signed)high2 */
    aReg += (double)(L_shr(L_var1,16)) * (double)(L_shr(L_var2,16)) * 2.0;

    /* saturate result.. */
    lvar = L_saturate(aReg);

    return(lvar);
}

/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ls
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var2) and a 16 bit
 *                 number (var1) returning a 32 bit result.
 *
 *     INPUTS:
 *
 *       L_var2             A Longword input variable
 *
 *       var1               A Shortword input variable
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        A Longword value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/
Longword L_mpy_ls(Longword L_var2, Shortword L_var1)
{
	double  aReg;
    Longword lvar;

    OP_COUNT(1);
    /* (unsigned)low2 * (signed)low1 */
	aReg  = (double)(0xffff & L_var2) * (double)(L_var1) * 2.0;

    OP_COUNT(1);
    /* >> 16 */
    aReg = (aReg / 65536);
    aReg = floor(aReg);

    OP_COUNT(1);
    /* (signed)high2 * (signed)low1 */
    aReg += ((double)L_shr(L_var2,16)) * (double)(L_var1) * 2.0;

    /* Saturate to 32 bits.. */
    lvar = L_saturate(aReg);
    return(lvar);
}

#else
/* (Use alternate double precision library routines... ) */
/*======================================================================*/
/*  Motorola Diversified Technology Services                            */
/*  EVRC fixed-point C simulation.                                      */
/*  Copyright (C) 1996 Motorola Diversified Technology Services.        */
/*  All rights reserved. Motorola proprietary and confidential.         */
/*----------------------------------------------------------------------*/
/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ll
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var1) and a 32 bit number
 *                 (L_var2), and return a 32 bit result.
 *
 *     INPUTS:
 *
 *       L_var1             A Longword input variable
 *
 *       L_var2             A Longword input variable
 *
 *     OUTPUTS:             none
 *
 *     IMPLEMENTATION:
 *
 *        Performs a 31x31 bit multiply, Complexity=24 Ops.
 *
 *        Let x1x0, or y1y0, be the two constituent halves
 *        of a 32 bit number.  This function performs the
 *        following:
 *
 *        low = ((x0 >> 1)*(y0 >> 1)) >> 16     (low * low)
 *        mid1 = [(x1 * (y0 >> 1)) >> 1 ]       (high * low)
 *        mid2 = [(y1 * (x0 >> 1)) >> 1]        (high * low)
 *        mid =  (mid1 + low + mid2) >> 14      (sum so far)
 *        output = (y1*x1) + mid                (high * high)
 *
 *
 *     RETURN VALUE:        A Longword value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/

Longword L_mpy_ll(Longword L_var1, Longword L_var2)
{
  Shortword swLow1,
         swLow2,
         swHigh1,
         swHigh2;
  Longword L_varOut,
         L_low,
         L_mid1,
         L_mid2,
         L_mid;


  swLow1 = shr(extract_l(L_var1), 1);
  swLow1 = SW_MAX & swLow1;

  swLow2 = shr(extract_l(L_var2), 1);
  swLow2 = SW_MAX & swLow2;
  swHigh1 = extract_h(L_var1);
  swHigh2 = extract_h(L_var2);

  L_low = L_mult(swLow1, swLow2);
  L_low = L_shr(L_low, 16);

  L_mid1 = L_mult(swHigh1, swLow2);
  L_mid1 = L_shr(L_mid1, 1);
  L_mid = L_add(L_mid1, L_low);

  L_mid2 = L_mult(swHigh2, swLow1);
  L_mid2 = L_shr(L_mid2, 1);
  L_mid = L_add(L_mid, L_mid2);

  L_mid = L_shr(L_mid, 14);
  L_varOut = L_mac(L_mid, swHigh1, swHigh2);

  return (L_varOut);
}

/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ls
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var2) and a 16 bit
 *                 number (var1) returning a 32 bit result. L_var2
 *                 is truncated to 31 bits prior to executing the
 *                 multiply.
 *
 *     INPUTS:
 *
 *       L_var2             A Longword input variable
 *
 *       var1               A Shortword input variable
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        A Longword value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/

Longword L_mpy_ls(Longword L_var2, Shortword var1)
{
  Longword L_varOut;
  Shortword swtemp;

  swtemp = shr(extract_l(L_var2), 1);
  swtemp = (short) 32767 & (short) swtemp;

  L_varOut = L_mult(var1, swtemp);
  L_varOut = L_shr(L_varOut, 15);
  L_varOut = L_mac(L_varOut, var1, extract_h(L_var2));
  return (L_varOut);
}
#endif
