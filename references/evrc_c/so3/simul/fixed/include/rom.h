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
/*  Module:     rom.h                                                   */
/*----------------------------------------------------------------------*/
#ifndef  _ROM_H_
#define  _ROM_H_

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include  "macro.h"

/*======================================================================*/
/*         ..Globals.                                                   */
/*----------------------------------------------------------------------*/

/* ROM tables */
extern short ppvq[ACBGainSize];
extern short ppvq_mid[ACBGainSize-1];
extern short gnvq_8[maxFCBGainSize];
extern short gnvq_4[maxFCBGainSize];

extern short nsize8[2];
extern short lognsize8[2];
extern short nsub8[2];
extern short rnd_delay[NoOfSubFrames + 2];

extern short nsize22[3];
extern short lognsize22[3];
extern short nsub22[3];

extern short nsize28[4];
extern short lognsize28[4];
extern short nsub28[4];

/* new AT&T quantizer */
extern short lsptab8[160];
extern short lsptab22[1792];
extern short lsptab28[2176];

extern short Logqtbl[256 * 3];
extern short Powqtbl[256 * 3];

#endif
