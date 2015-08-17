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
/*  Module:     proto.h                                                 */
#ifndef  _PROTO_H_
#define  _PROTO_H_

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include  "macro.h"

/*======================================================================*/
/*         ..CELP Function definitions.                                 */
/*----------------------------------------------------------------------*/
extern void lpcanalys(
    short *pc, 
    short *rc, 
    short *input, 
    short order, 
    short len,
    long  *R);

extern void weight(
    short *awght, 
    short *a, 
    short gammma, 
    short order);

extern void ZeroInput(
    short *output, 
    short *coef_uq, 
    short *coef, 
    short *in, 
    short gamma1, 
    short gamma2, 
    short order, 
    short length, 
    short type);

extern void ImpulseRzp(
    short *output, 
    short *coef_uq, 
    short *coef, 
    short gamma1, 
    short gamma2, 
    short order, 
    short length);

extern void SynthesisFilter(
    short *output, 
    short *input, 
    short *coef, 
    short *memory, 
    short order, 
    short length);

extern void autocorrelation(
    long  *r, 
    short *r_scale,
    short *winput, 
    short len, 
    short order);

extern void durbin(
    long  *r, 
    short *rc, 
    short *pc, 
    short order);

extern void a2lsp(
    short *freq, 
    short *a);

extern void lsp2a(
    short *a, 
    short *freq);

extern void Interpol(
    short *Lar, 
    short *last, 
    short *current, 
    short SubNum, 
    short order);

extern void Interpol_delay(
    long  *out, 
    short *last, 
    short *current, 
    short SubNum);

extern void acb_excitation(
    short *Ex1, 
    short gain, 
    long  *delay3, 
    short *PitchMemory, 
    short length);

extern void Bitpack(
    short in, 
    unsigned short *TrWords, 
    short NoOfBits, 
    short *ptr);

extern void BitUnpack(
    short *out, 
    unsigned short *RecWords, 
    short NoOfBits, 
    short *ptr);

/*======================================================================*/
/*         ..RCELP routines.                                            */
/*----------------------------------------------------------------------*/
extern void GetResidual(
    short *residual, 
    short *input, 
    short *coef, 
    short *mem, 
    short order, 
    short length);

extern void fndppf(
    short *delay, 
    short *beta, 
    short *buf, 
    short dmin, 
    short dmax, 
    short length);

extern void ComputeACB(
    short *residualm, 
    short *excitation, 
    long  *delay, 
    short *residual, 
    short guard, 
    short *dpm, 
    short *accshift, 
    short beta, 
    short length, 
    short rshift);

extern void putacbc(
    short *exctation, 
    short *input, 
    short dpl, 
    short subframel, 
    short extra, 
    long  *delay3, 
    short freq, 
    short prec);

extern void mod(
    short *residualm, 
    short *accshift, 
    short beta, 
    short shiftr, 
    short resolution, 
    short *exctation, 
    short *Dresidual, 
    short *residual, 
    short guard, 
    short *dpm, 
    long  delay, 
    short subframel, 
    short extra);

extern void cshiftframe(
    short *sfstart, 
    short *sfend, 
    short *maxshift2, 
    short dpm, 
    short *residual, 
    short guard, 
    short accshift, 
    short maxshift, 
    long  delay, 
    short subframel, 
    short extra);

extern void maxeloc(
    short *maxloc, 
    long  *maxener, 
    short *signal, 
    short dp, 
    short length, 
    short ewl);

extern void modifyorig(
    short *residualm, 
    short *accshift, 
    short beta, 
    short *dpm, 
    short shiftrange, 
    short resolution, 
    short *TARGET, 
    short *residual, 
    short dp, 
    short sfend);

extern void bl_intrp(
    short *output, 
    short *input, 
    long  delay, 
    short factor, 
    short fl);

extern void getgain(
    short *exctation, 
    short *lambda, 
    short *H, 
    short *idxcb, 
    short *gcb, 
    short *gcb_mid, 
    short gcb_size, 
    short Quantize, 
    short *mresidual, 
    short subframel, 
    short hlength);

extern void ConvolveImpulseR(
    short *out, 
    short *in, 
    short *H, 
    short hlength, 
    short length);

extern void PickToAverage(
    short *res, 
    short *signal, 
    short length);

extern void lspmaq_dec(
    short ndim, 
    short kdim, 
    short many, 
    short *nsub, 
    short *nsiz, 
    short *y, 
    short *index, 
    short br, 
    short *);

extern void lspmaq(
    short *x, 
    short ndim, 
    short kdim, 
    short many, 
    short *nsub, 
    short *nsiz, 
    short alp, 
    short *y, 
    short *index, 
    short br, 
    short *);

extern void Weight2Res(
    short *, 
    short *, 
    short *, 
    short *, 
    short, 
    short, 
    short, 
    short);

extern void iir(
    short *output, 
    short *input, 
    short *coef, 
    short *IIRmemory, 
    short order, 
    short length);

extern void fir(
    short *output, 
    short *input, 
    short *coef, 
    short *FIRmemory, 
    short order, 
    short length);

extern void apf(
    short *, 
    short *, 
    short *, 
    long, 
    short, 
    short, 
    short, 
    short, 
    short, 
    short, 
    short, 
    short);

extern void GetExc800bps(
    short *output, 
    short *best, 
    short scale, 
    short *input, 
    short length, 
    short flag, 
    short n);

extern void GetExc800bps_dec(
    short *output, 
    short length, 
    short best, 
    short flag, 
    short n, 
    short fer_flag);

extern short  ran_g(
    short *seed0);

extern void fcb_gainq(
    short *index,
    short *gain_q,
    long l_gain_nq,
    short *gain_tab,
    short gain_size);


#endif
