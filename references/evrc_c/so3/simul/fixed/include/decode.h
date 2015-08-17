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
/*  Module:     decode.h                                                */
/*----------------------------------------------------------------------*/
#ifndef  _DECODE_H_
#define  _DECODE_H_

/*======================================================================*/
/*         ..Functions.                                                 */
/*----------------------------------------------------------------------*/
extern void InitDecoder();
extern void decode(short*,short,short,short*);
extern void decode_no_fer(short*,short,short*);
extern void decode_fer(short,short*);
extern void decode_rate_1(short*,short,short*);

#endif
