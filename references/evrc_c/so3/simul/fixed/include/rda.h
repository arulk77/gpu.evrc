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
/* rate_integrate.h - basic structures for celp coder                 */
/**********************************************************************/
#ifndef _RDA_H_
#define _RDA_H_

#include <stdio.h>

#include "qcoder.h"

typedef struct
{
    long band_noise_sm[FREQBANDS];  /* G. Foerster: Band Noise in Q0 - long */
    short last_rate;            /* E. Chen: int; rate decisision after 2nd stage select_mode2() */
    short last_rate_1st_stage;  /* E. Chen: int; rate decision after 1st stage */
    short last_rate_2nd_stage;  /* E. Chen: int; rate decision after 2nd stage */
    short num_full_frames;      /* E. Chen: int */
    short hangover, hangover_in_progress;   /* E. Chen: int */
    short band_rate[FREQBANDS]; /* E. Chen: int */
    long band_power[4];         /* E. Chen: float (5.000000 to 1675191040.000000); change 4 to FREQBANDS? */
    long signal_energy[FREQBANDS];  /* G. Foerster: Signal Energy in Q0 - long */
    /* E. Chen: float (299010.312500 to 1244383488.00000), (640864.250000 to 1516120832.000000) / 65536 */
    long frame_energy_sm[FREQBANDS];    /* E. Chen: float (3.406658 to 1244383488.000000) */
    short adaptcount;           /* E. Chen: int */
    short pitchrun;             /* E. Chen: int */
    short band_power_last[FREQBANDS];   /* E. Chen: float (0.000000 to 0.000000); may delete because not used? */
    short snr_stat_once;        /* E. Chen: int */
    short snr_map[FREQBANDS];   /* E. Chen: int */
    long snr[FREQBANDS];        /* G. Foerster: SNR in Q8 - long */
    /* E. Chen: float (2241.314941 to 6294507.000000) / 512 */
    short r_filt[FREQBANDS][FILTERORDER];   /* E. Chen: float (0.000151 to 0.475050, -0.001076 to -0.318032) */
    short frame_num;            /* E. Chen: int */
}
ENCODER_MEM;

extern short select_rate(
                            long *R_interp,     /* E. Chen: float* */
                            short max_rate,
                            short min_rate,
                            short beta  /* E. Chen: float */
);

#endif
