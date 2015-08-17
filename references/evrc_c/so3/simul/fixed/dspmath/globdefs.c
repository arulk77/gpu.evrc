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

/* ________________________________________________________________________
 * |                                                                        |
 * |                Include Files                                  |
 * |________________________________________________________________________|
 */

#include "typedefs.h"

/*
 * Storage of global variables (definition)
 * to reference (declaration) these you must include "typeDefs.h"
 */

int giFrmCnt;					/* Frame count: 0,1,2,3,4,... */
int giSfrmCnt = 0;				/* Subframe count: 0,1,2,3 */

int giOverflow;					/* latching overflow bit:
								 * has saturation occurred? 0=no, 1=yes */
int giOldOverflow;

Longword op_counter = 0;        /* Operation counter LT 6/96 */
