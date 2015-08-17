/**********************************************************************
Each of the companies; Qualcomm, and Lucent, and Motorola (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the EVRC-B Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the EVRC-B be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC-B or the field of use of practice of the 
EVRC-B Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/

static char const rcsid[]="$Id: packet.cc,v 1.14 2006/12/07 00:02:25 apadmana Exp $";

/*======================================================================*/
/*  Lucent Technologies Network Wireless Systems                        */
/*  EVRC Floating-point C Simulation.                                   */
/*                                                                      */
/*  Copyright (C) 1996 Lucent Technologies Incorporated. All rights     */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
/*  Module:     Bitpack                                                 */
/*----------------------------------------------------------------------*/
/*  History:                                                            */
/*     01/01/95  Written By Dror Nahumi, AT&T                           */
/*----------------------------------------------------------------------*/

/************************************************
* Routine name: Bitpack                         *
* Function: pack input data into bitstream.     *
* Inputs:                                       *
*    in - input data.                           *
*    TrWords - pointer to transmit words memory.*
*    NoOfBits - number of bits in input data.   *
*    ptr - bit and word pointers.               *
*                                               *
************************************************/
#include "typedef_fx.h"
#include "basic_op40.h"
#include "basic_op.h"
#include "proto_fx.h"
#include "macro_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif //WMOPS_FX

void  Bitpack(short in,unsigned short *TrWords,short NoOfBits,short *ptr)
{
  short   temp;
  unsigned short *WordPtr;
  
  WordPtr = TrWords + ptr[1];
  
  *ptr -= NoOfBits;
#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if (*ptr >= 0) {
    *WordPtr = *WordPtr | (in << *ptr);
  }
  else {
    temp = (unsigned short)in >> (-*ptr);
    *WordPtr = *WordPtr | temp;
    WordPtr++;
    *ptr = 16 + *ptr;
    *WordPtr = (short) ((long) ((long) in << *ptr) & 0xffff);
  }
  ptr[1] = (short) (WordPtr - TrWords);
}

/*======================================================================*/
/*  Lucent Technologies Network Wireless Systems                        */
/*  EVRC Floating-point C Simulation.                                   */
/*                                                                      */
/*  Copyright (C) 1996 Lucent Technologies Incorporated. All rights     */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
/*  Module:    bitupack.c                                               */
/*----------------------------------------------------------------------*/
/*  History:                                                            */
/*     1/01/95  Born                                                    */
/*----------------------------------------------------------------------*/

/***********************************************
* Routine name: BitUnpack                      *
* Function: pack input data into bitstream.    *
* Inputs:                                      *
*    RecWords - Location of receive words.     *
*    NoOfBits - number of bits for output data.*
*    ptr - bit and word pointers.              *
* Output:                                      *
*    out - output bits.                        *
*                                              *
* Written by: Dror Nahumi.                     *
***********************************************/

void BitUnpack(short *out,unsigned short *RecWords,short NoOfBits,short *ptr)
{
  unsigned short *WordPtr;
  long    temp;
  
  WordPtr = RecWords + ptr[1];
  
  *ptr -= NoOfBits;
#ifdef WMOPS_FX
  test();
#endif
  if (*ptr >= 0) {
    temp = (long) (*WordPtr) << NoOfBits;
  }
  else {
    temp = (long) (*WordPtr) << (NoOfBits + *ptr);
    WordPtr++;
    temp = (temp << (-*ptr)) | ((long) *WordPtr << (-*ptr));
    *ptr = 16 + *ptr;
  }
  
#ifdef WMOPS_FX
  logic16();
  logic16();
#endif
  *WordPtr = (short) (temp & 0xffff);
  *out = (short) ((long) (temp & 0xffff0000) >> 16);
  
  ptr[1] = (short) (WordPtr - RecWords);
}


/*===================================================================*/
/* FUNCTION      :  TTY_DTMF_pack ().                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function converts the tty data              */
/*                   into the bit-stream representation.             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (PARAMETER  *)  data_buf    : data_buf                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16     [])  TxPkt: bit-stream.                     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ (Word16     [])  PktPtr:   pointer to the bit-stream. */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/
	
void TTY_DTMF_pack (Word16 *data_buf, Word16 TxPkt [], Word16 PktPtr[])
{
  /*-------------------------------------------------------------------*/
        
        Word16 illegal_half_id=0x78; //delay_idx = 120

	  Word16 i;

	  /*------------------------------------------------------------------*/
	  /*                Initalizing bit packing parameters                */
	  /*------------------------------------------------------------------*/
	 
	  
	 
	  Bitpack(illegal_half_id,(unsigned short *)TxPkt,7,PktPtr); //packed in place of delay 

	  
	  

	  Bitpack(data_buf[0], (unsigned short *) TxPkt, 3, PktPtr);
	  
          for (i = 1; i< DATA_SIZE-1; i++)
	    Bitpack(data_buf[i], (unsigned short *) TxPkt, 16, PktPtr);
	  
	   /*--------------------------------------------------------------*/
	   /*                Only 61 bits for half rate data               */
	   /*--------------------------------------------------------------*/
          Bitpack(0, (unsigned short *) TxPkt, 16,PktPtr); //RESERVED
	  Bitpack(0, (unsigned short *) TxPkt, 6,PktPtr); //RESERVED
	  
	   

	  /*-----------------------------------------------------------------*/
	  
	  return;
	  
	  /*-----------------------------------------------------------------*/
}



/*===================================================================*/
/* FUNCTION      :  TTY_DTMF_unpack ().                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function converts the bit-stream into 
                    data buffer                                      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16     [])  RxPkt: bit-stream.                     */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16     [])  data_buf:                              */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*         _ (Word16     [])  PktPtr:   pointer to the bit-stream.   */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*         _ (Word16     )  flag=1=> TTY , flag=0 => Non-TTY          */
/*===================================================================*/
	
Word16 TTY_DTMF_unpack(Word16 RxPkt [], Word16 PktPtr [], Word16* data_buf)
{
  Word16 i,poss_illegal_id,flag=0;
  Word16 illegal_half_id=0x78; //delay_idx = 120
  BitUnpack (&poss_illegal_id, (UNS_Word16 *) RxPkt,7, PktPtr);
  if (poss_illegal_id == illegal_half_id)
   flag=1;
  if (flag ==1)
    {
      BitUnpack (&data_buf[0], (UNS_Word16 *) RxPkt,3, PktPtr);	
      for (i = 1; i < DATA_SIZE-1; i++)
	BitUnpack (&data_buf[i], (UNS_Word16 *) RxPkt, 16,PktPtr);	
    }
  return(flag);
}



