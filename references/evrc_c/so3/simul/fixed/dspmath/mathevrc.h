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
/***********************************************************************
 *
 *   FILE : mathevrc.h
 *
 *   PURPOSE:
 *     
 *     Modified ETSI basic operations.  Bit-exact simulation of a
 *     generic 32 bit accumulator DSP chip (fractional math).  This
 *     version has a latching overflow bit (giOverflow) and
 *     non-compound MAC's (One where initial mult does not saturate)
 *     
 *   SCCS Data:
 *             File Version                  = 1.1
 *             Archived Date (put/delta)     = 5/20/96  14:49:49
 *             Archive Extraction Date (get) = 5/20/96  15:17:56
 *                
 *   Eric Winter, October 1995
 *     
 *
 ***********************************************************************/

#ifndef __MATHHALF
#define __MATHHALF

#include "typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

/* addition */
/************/

Shortword add(Shortword var1, Shortword var2);  /* 1 ops */
Shortword sub(Shortword var1, Shortword var2);  /* 1 ops */
Longword L_add(Longword L_var1, Longword L_var2);       /* 2 ops */
Longword L_sub(Longword L_var1, Longword L_var2);       /* 2 ops */

/* multiplication */
/******************/

Shortword mult(Shortword var1, Shortword var2); /* 1 ops */
Longword L_mult(Shortword var1, Shortword var2);        /* 1 ops */
Shortword mult_r(Shortword var1, Shortword var2);       /* 2 ops */


/* arithmetic shifts */
/*********************/

Shortword shr(Shortword var1, Shortword var2);  /* 1 ops */
Shortword shl(Shortword var1, Shortword var2);  /* 1 ops */
Longword L_shr(Longword L_var1, Shortword var2);        /* 2 ops */
Longword L_shl(Longword L_var1, Shortword var2);        /* 2 ops */
Shortword shift_r(Shortword var, Shortword var2);       /* 2 ops */
Longword L_shift_r(Longword L_var, Shortword var2);     /* 3 ops */

/* absolute value  */
/*******************/

Shortword abs_s(Shortword var1);       /* 1 ops */
Longword L_abs(Longword var1);         /* 3 ops */


/* multiply accumulate  */
/************************/

Longword L_mac(Longword L_var3,
                      Shortword var1, Shortword var2);  /* 1 op */
Shortword mac_r(Longword L_var3,
                       Shortword var1, Shortword var2); /* 2 op */
Longword L_msu(Longword L_var3,
                      Shortword var1, Shortword var2);  /* 1 op */
Shortword msu_r(Longword L_var3,
                       Shortword var1, Shortword var2); /* 2 op */

/* negation  */
/*************/

Shortword negate(Shortword var1);      /* 1 ops */
Longword L_negate(Longword L_var1);    /* 2 ops */


/* Accumulator manipulation */
/****************************/

Longword L_deposit_l(Shortword var1);  /* 1 ops */
Longword L_deposit_h(Shortword var1);  /* 1 ops */
Shortword extract_l(Longword L_var1);  /* 1 ops */
Shortword extract_h(Longword L_var1);  /* 1 ops */

/* Round */
/*********/

Shortword round(Longword L_var1);      /* 1 ops */

/* Normalization */
/*****************/

Shortword norm_l(Longword L_var1);     /* 30 ops */
Shortword norm_s(Shortword var1);      /* 15 ops */

/* Division */
/************/
Shortword divide_s(Shortword var1, Shortword var2);     /* 18 ops */

/* Saturation manipulation routines */
/************************************/

int  clearOverflow(void);
int  isOverflow(void);
int  popOverflow(void);
int  setOverflow(void);
Longword L_saturate(double dvar1);


/* Non-saturating instructions */
/*******************************/
Longword L_add_c(Longword L_Var1, Longword L_Var2);     /* 2 ops */
Longword L_sub_c(Longword L_Var1, Longword L_Var2);     /* 2 ops */
Longword L_sat(Longword L_var1);       /* 4 ops */
Longword L_macNs(Longword L_var3,
                        Shortword var1, Shortword var2);        /* 1 ops */
Longword L_msuNs(Longword L_var3,
                        Shortword var1, Shortword var2);        /* 1 ops */



/* OP Counter defines  LT 6/96  */
/********************************/
extern Longword op_counter;     /* Operation counter LT 6/96 */

#define OP_COUNT(x) op_counter+=x
#define OP_RESET    op_counter=0

#endif
