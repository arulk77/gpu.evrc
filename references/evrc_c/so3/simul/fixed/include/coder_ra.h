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
/* defines for the improved rate decision algorithm */

#ifndef _CODER_RATE_H_
#define _CODER_RATE_H_

#include  "qcoder.h"

#define PITCH_NUM 2
#define INC_FACTOR 16876        /* GBF: 1.03 in Q14 */
#define LPC_ORDER_NOISE 8
#define SNR_MAP_THRESHOLD 3
#define IS96_INC 16474          /* G. Foerster: 1.00547 in Q14 */
#define VOICE_INITIAL 65+12     /* for EVRC increasing dynamic range */
#define VOICE_INITIAL_HI 55+12  /* for EVRC increasing dynamic range */

#define STATVOICED 5
#define SCALE_DOWN_ENERGY 31785 /* E. Chen: 0.97; changed from 0.96 on 20-Jan-95 */

#define TLEVELS 8

#define NEW_FORM_RESID YES

#define THR_SIL_1_4  15         /* 14db below current energy is declared 1/4 rate silence */
#define THR_SIL_1_2  9          /* 9db below current energy is declared 1/2 rate silence */
#define THR_DIFFLSP 0.02        /* lsp must not change too quickly also for rate to
                                 * get bumped down                                 */
#define SMSNR  19661            /* E. Chen: 0.6; leaky integration constant used smooth snr estimate */
                          /* changed on 8-Dec-94 per Sharath's recommendation */

#define RATEM 0.02              /* bounds around avg rate that are acceptable */
#define ALPHA_STAT 0.9995       /* 40second time constant   */
#define ADP 8

#define NACF_ADAP_BGN_THR  9830 /* E. Chen: 0.3; threshold signifying frame does */
                                /* not have any voiced speech in it           */
                                /* so we might start to adapt thresholds      */
#define NACF_SOLID_VOICED  16384    /* E. Chen: 0.5; threshold above which we are pretty sure  */
                                 /* speech is present and thus SNRs can be     */
                                 /* adjusted accordingly                       */

#define FULLRATE_BPS 14.4
#define HALFRATE_BPS 7.2
#define QUARTER_BPS  3.6
#define HIGH_BND_SNR 25.0    /*14.0*/   /* in db */

#define LOW_BND_SNR 6.0         /* in db */
#define THR_NACF_BUMPUP 0.4     /* if nacf is lower than this and not declared */
                            /* unvoiced then bump up rate                 */
#define THR_PRED_GAIN 5         /* in db, cc:08-23-94 */

#define FIR_UNV_LEN 21

#define HIGH_THRESH_LIM  5059644*16
#define DECIMATE_F 2
#define DEC_ORDER 17

#define FULL_THRESH 1           /* the number of full rates in a row before */
                        /* a hangover can occur                     */
static short hangover[TLEVELS] =
{8, 8, 8, 4, 1, 0, 0, 0};
/* {7, 7, 7, 3, 0, 0, 0, 0}; */
                        /* hangover as a function of signal to noise */
                        /*ratio                                      */

/* linear discriminant coefficients for the voiced/unvoiced decision      */
/* 25-Oct-94 Weights */
/*static float dis_coef[5] = {2.795007,-0.031928,0.0,0.973946,0.0}; */

/* 03-March-95  Weights */
/*static float dis_coef[5] = {320.3,-2.2086,0.0,1.01,0.0}; */

/* 06-March-95  Weights */
/*static float dis_coef[5] = {841.38,-5.778275,0.0,0.972353,0.0}; */

/* 08-March-95  Weights, Revision 5.17,  A5178f2.sln  */
/*static float dis_coef[5] = {5.190283,-0.092413,0.0,3.091836,0.0}; */

/* 09-March-95  Weights B5178f2.sln */
/*static float dis_coef[5] = {5.142671,-0.092313,0.0,3.134354,0.0}; */

/* 30-March-95  Weights , Revision 5.21  C5178f2.sln */
/*static float dis_coef[5] = {5.616,-0.1,0.0,3.65,0.0}; */

static short THRESH_SNR[FREQBANDS][TLEVELS][2] =
{
/* low band thresholds */
    {
        {1792, 2304},           /* E. Chen: { {7.0, 9.0} ,  * 256 */
        {1792, 3226},           /* E. Chen:  {7.0, 12.6} ,  * 256 */
        {2048, 4352},           /* E. Chen:  {8.0, 17.0} ,  * 256 */
        {2202, 4736},           /* E. Chen:  {8.6, 18.5} ,  * 256 */
        {2278, 4966},           /* E. Chen:  {8.9, 19.4} ,  * 256 */
        {2406, 5350},           /* E. Chen:  {9.4, 20.9} ,  * 256 */
        {2816, 6528},           /* E. Chen:  {11.0, 25.5} , * 256 */
        {8090, 20378}},         /* E. Chen:  {31.6, 79.6} },* 256 */
/* high band thresholds, these are same as above */
    {
        {1792, 2304},           /* E. Chen: { {7.0, 9.0} ,  * 256 */
        {1792, 3226},           /* E. Chen:  {7.0, 12.6} ,  * 256 */
        {2048, 4352},           /* E. Chen:  {8.0, 17.0} ,  * 256 */
        {2202, 4736},           /* E. Chen:  {8.6, 18.5} ,  * 256 */
        {2278, 4966},           /* E. Chen:  {8.9, 19.4} ,  * 256 */
        {2406, 5350},           /* E. Chen:  {9.4, 20.9} ,  * 256 */
        {2816, 6528},           /* E. Chen:  {11.0, 25.5} , * 256 */
        {8090, 20378}}          /* E. Chen:  {31.6, 79.6} } * 256 */

};

static short LOWEST_LEVEL[FREQBANDS] =
{160, 80};                      /* G. Foerster: Thresholds in Q0 */

#endif
