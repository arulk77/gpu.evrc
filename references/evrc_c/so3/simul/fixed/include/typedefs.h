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
/*******************************************************************
 *
 * typedef statements of types used in all half-rate GSM routines
 *
 ******************************************************************/

#ifndef __TYPEDEFS
#define __TYPEDEFS

#define DATE    "March 6, 1995     "
#define VERSION "Version 4.1       "

#define LW_SIGN (long)0x80000000       /* sign bit */
#define LW_MIN (long)0x80000000
#define LW_MAX (long)0x7fffffff

#define SW_SIGN (short)0x8000          /* sign bit for Shortword type */
#define SW_MIN (short)0x8000           /* smallest Ram */
#define SW_MAX (short)0x7fff           /* largest Ram */

/* Definition of Types *
 ***********************/

typedef long int Longword;             /* 32 bit "accumulator" (L_*) */
typedef short int Shortword;           /* 16 bit "register"  (sw*) */
typedef short int ShortwordRom;        /* 16 bit ROM data    (sr*) */
typedef long int LongwordRom;          /* 32 bit ROM data    (L_r*)  */

struct NormSw
{                                      /* normalized Shortword fractional
                                        * number snr.man precedes snr.sh (the
                                        * shift count)i */
  Shortword man;                       /* "mantissa" stored in 16 bit
                                        * location */
  Shortword sh;                        /* the shift count, stored in 16 bit
                                        * location */
};


/* global variables */
/********************/

extern int giFrmCnt;                   /* 0,1,2,3,4..... */
extern int giSfrmCnt;                  /* 0,1,2,3 */

extern int giDTXon;                    /* DTX Mode on/off */

extern int giOverflow;
extern int giOldOverflow;
#endif
