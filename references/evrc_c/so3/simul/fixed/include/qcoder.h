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
/* coder.h - defines for a particular coder                           */
/**********************************************************************/
#ifndef _QCODER_H_
#define _QCODER_H_

#define PMAX                 120
#define PMIN                  20
#define FILTERORDER           17
#define FREQBANDS              2


#define FULLRATE_VOICED        4
#define HALFRATE_VOICED        3
#define QUARTERRATE_UNVOICED   2
#define QUARTERRATE_VOICED     5
#define EIGHTH                 1
#define ERASURE               14 /* E. Chen: 0xe */ 
#define BLANK                  0


#define FSIZE                160  /* Overall frame size */

#endif
