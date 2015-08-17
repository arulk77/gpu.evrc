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
#include "typedefs.h"

void ACELP_Code(
  Shortword _xn[],        /* (i)     :Target signal for codebook.       */
  Shortword _res2[],      /* (i)     :Residual after pitch contribution */
  Shortword _h1[],        /* (i)     :Impulse response of filters.      */
  Shortword   T0,           /* (i)     :Pitch lag.                        */
  Shortword _pitch_gain,  /* (i)     :Pitch gain.                       */
  Shortword   l_subfr,      /* (i)     :Subframe lenght.                  */
  Shortword _code[],      /* (o)     :Innovative vector.                */
  Shortword *_gain_code,  /* (o)     :Innovative vector gain.           */
  Shortword _y2[],        /* (o)     :Filtered innovative vector.       */
  Shortword  *index,        /* (o)     :Index of codebook + signs         */
  Shortword  choice         /* (i)     :Choice of innovative codebook     */
              /*          0 -> 10 bits                      */
              /*          1 -> 35 bits                      */
);

void code_3i54_10bits(
  Shortword dn[],    /* (i) Q0 : dn[54] correlation between target and h[]     */
  Shortword l_subfr, /* (i)    : lenght of subframe (53 or 54)                 */
  Shortword H[],     /* (i) Q12: impulse response of weighted synthesis filter */
  Shortword code[],  /* (o) Q12: algebraic (fixed) codebook excitation         */
  Shortword y[],     /* (o) Q10: filtered fixed codebook excitation            */
  Shortword *indx    /* (o)    : index 1 words of 10 bits                      */
);

void code_8i55_35bits(
  Shortword dn[],    /* (i) Q0 : dn[55] correlation between target and h[]     */
  Shortword cn[],    /* (i) Q0 : cn[55] residual after long term prediction    */
  Shortword l_subfr, /* (i)    : lenght of subframe (53 or 54)                 */
  Shortword H[],     /* (i) Q12: impulse response of weighted synthesis filter */
  Shortword code[],  /* (o) Q12: algebraic (fixed) codebook excitation         */
  Shortword y[],     /* (o) Q10: filtered fixed codebook excitation            */
  Shortword indx[]   /* (o)    : index 4 words: 8 + 8 + 8 + 11 = 35 bits       */
);

void dec3_10(
  Shortword index,   /* (i)    : indx of 3 pulses (10 bits)               */
  Shortword cod[],   /* (o) Q12: algebraic (fixed) codebook excitation    */
  Shortword l_subfr  /* (i)    : lenght of subframe (53 or 54)            */
);

void dec8_35(
  Shortword *indx,   /* (i)    : indx of 8 pulses (8+8+8+11 = 35 bits)    */
  Shortword cod[],   /* (o) Q12: algebraic (fixed) codebook excitation    */
  Shortword l_subfr  /* (i)    : lenght of subframe (53 or 54)            */
);

void cor_h_x(
  Shortword h[],    /* (i) Q12 : impulse response of weighted synthesis filter */
  Shortword x[],    /* (i) Q0  : correlation between target and h[]            */
  Shortword dn[]    /* (o) Q0  : correlation between target and h[]            */
);

Shortword G_code(     /* out   Q0 : Gain of innovation code.        */
  Shortword xn2[],    /* in    Q0 : target vector                   */
  Shortword y2[],     /* in    Q10: filtered inovation vector       */
  Shortword l_subfr   /* in       : length of subframe (53 or 54)   */ 
);



