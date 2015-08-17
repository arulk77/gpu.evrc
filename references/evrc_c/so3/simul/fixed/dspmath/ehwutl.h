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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void xDispSw(Shortword in);
void xDispLw(Longword in);
void xDispSns(struct NormSw snsIn);
double toFloatLw(Longword lwIn);
double toFloatSw(Shortword swIn);
double toFloatSns(struct NormSw snsIn);
void fDispSw(Shortword swIn);
void fDispLw(Longword lwIn);
void fDispSns(struct NormSw snsIn);
Shortword toSwFloat(double dIn);
Longword toLwFloat(double dIn);

