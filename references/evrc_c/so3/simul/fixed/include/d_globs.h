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
/*  Module:     d_globs.h                                               */
/*----------------------------------------------------------------------*/
#ifndef  _D_GLOBS_H_
#define  _D_GLOBS_H_

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include  "macro.h"

/*======================================================================*/
/*         ..Globals (decoder).                                         */
/*----------------------------------------------------------------------*/
extern long decode_fcnt;    /* Frame counter */

extern short OldlspD[ORDER];    /* Last frame quantized lsp (decoder)      */

extern short PitchMemoryD[ACBMemSize + SubFrameSize + EXTRA];
extern short PitchMemoryD_back[ACBMemSize];

extern short DECspeech[SubFrameSize];   /* Output decoder speech           */
extern short DECspeechPF[SubFrameSize]; /* Output decoder speech with post filter */

extern short SynMemory[ORDER];  /* Synthesis filter's memory             */

extern short erasureFlag;
extern short errorFlag;
extern short lastrateD;
extern short pdelayD;
extern short pdelayD_back;

extern short fer_flag;
extern short fer_counter;
extern short FadeScale;
extern short ave_acb_gain;
extern short ave_fcb_gain;

extern short last_valid_rate;
extern short last_fer_flag;
#endif
