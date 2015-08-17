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
/*  Module:     globs.c                                                 */
/*----------------------------------------------------------------------*/
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  102                                         */
/*      Stack/Local RAM:    0                                           */
/*----------------------------------------------------------------------*/

#include  "macro.h"

extern short   Scratch[SubFrameSize + 6];
extern short   SScratch[6];     /* scratch short memory */

extern short   lsp[ORDER];      /* Correlation coefficients                */
extern short   lspi[ORDER];     /* Interpolation of correlation coeff      */

extern short   pci[ORDER];      /* Interpolated prediction coefficients    */
extern short  *gnvq;            /* Quantization table for fcb gain */

extern short   idxppg;          /* Pitch gain c.b. - selected gain index   */
extern short   idxcb;           /* Shape c.b. - selected shape index       */
extern short   idxcbg;          /* FCB gain c.b. - selected gain index     */

extern short   bit_rate;        /* Current bitrate for encoder/decoder */

extern short   FCBGainSize;     /* Current Fixed Codebook Gain Size */
extern short   delay;           /* current frames delay */
extern short   LPCflag;     

extern short   fcbIndexVector[10];
extern short   PackWdsPtr[2];       /* Pointer to current TX/RX receive word */
extern short   PackedWords[PACKWDSNUM]; /* TX/RX memory */

extern short  *nsize, *nsub, *lognsize, knum;
extern short  *lsptab;
