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
#ifndef  __MATHDP31
#define  __MATHDP31

#include "typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

Longword L_mpy_ls(Longword L_var2, Shortword var1);
Longword L_mpy_ll(Longword L_var1, Longword L_var2);
short  isSwLimit(Shortword swIn);
short  isLwLimit(Longword L_In);

#endif
