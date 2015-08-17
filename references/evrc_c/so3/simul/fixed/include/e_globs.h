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
/*  Module:     e_globs.c                                               */
/*----------------------------------------------------------------------*/
/*  Memory Usage:                                                       */
/*      ROM:                0                                           */
/*      Static/Global RAM:  1202                                        */
/*      Stack/Local RAM:    0                                           */
/*----------------------------------------------------------------------*/
#include  "macro.h"
#include  "globs.h"

/*======================================================================*/
/*         ..Globals (encoder).                                         */
/*----------------------------------------------------------------------*/
extern short  *ExconvH; /* Excitation of adaptive codebook convolved with H (uses Scratch)*/

extern long    encode_fcnt;     /* Frame counter */

extern short   HPspeech[FrameSize+GUARD*2];     /* orig. speech signal - memory overlap with residual */
extern short   ConstHPspeech[GUARD*2];  /* temporary buffer to store HPspech  */

extern short   OldlspE[ORDER];      /* Last frame quantized lsp                */

extern short   lsp_nq[ORDER];       /* Correlation coefficients                */
extern short   Oldlsp_nq[ORDER];    /* Last frame quantized lsp                */
extern short   lspi_nq[ORDER];      /* Interpolation of correlation coeff      */
extern short   pci_nq[ORDER];       /* Interpolated prediction coefficients    */
extern short   wpci[ORDER];     /* Interpolated weighted prediction coefficients */

extern short   Excitation[ACBMemSize + SubFrameSize + EXTRA];

extern short   H[Hlength+1];        /* Impulse response [Hlength]              */
extern short   HtH[Hlength+1];      /* Impulse response ^2 [Hlength]           */
extern short   SynMemoryM[ORDER];   /* weighted speech synthesis filter memory */

extern short   TARGET[SubFrameSize+1];  /* Residual - Zero input response       */
extern short   TARGETw[SubFrameSize+1];

extern short   WFmemFIR[ORDER]; /* Weighting filter memory                 */
extern short   WFmemIIR[ORDER]; /* Weighting filter memory                 */

extern short   zir[SubFrameSize];   /* Zero Input Response (can share memory w/ HtH)*/

extern short   residual[2 * GUARD + FrameSize + 10];
extern short   residualm[SubFrameSize + EXTRA];

extern short   origm[SubFrameSize];
extern short  *worigm;      /* shared weighted original memory */

extern short   accshift;
extern short   delay1;
extern short   pdelay;
extern short   beta, beta1;
extern short   dpm;

extern short   LPCgain;     /* used for frame erasures */
extern short   shiftSTATE;
extern short   lastrateE;       /* last bitrate used for encoder*/

extern short   fcbIndexVector[10];     /* ACELP fixed codebook index vector */
extern short   fcbGain;                /* ACELP fixed codebook gain */
extern short   y2[55];                 /* Filtered innovative vector (debug only) */

