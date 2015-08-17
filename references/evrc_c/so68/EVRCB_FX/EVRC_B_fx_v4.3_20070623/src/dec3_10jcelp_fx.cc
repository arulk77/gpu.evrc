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

#include "typedef_fx.h"
#include "macro_fx.h" 
#include "basic_op.h"

#include "proto_fx.h"
#include "index_tbl.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


//extern	Word16 index0_fx[], index1_fx[];	/* in cod3_10vcm.c */


/*===================================================================*/
/* FUNCTION      :  dec3_10jcelp_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Algebraic codebook decoder; 10 bits: 3 pulses in
                    a frame of 55 samples.                           */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16)  pit_lag  : pitch lag  (Q0)                          */
/*   _ (Word16)  pit_gain : pitch gain (Q14)                         */
/*   _ (Word16)  l_subfr  : length of sub-frame (Q0)                 */
/*   _ (Word16*)  indices    :  indices of 3 pulses (Q0)             */
/*                              (10 bits, 1 word)                    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16[]) cod : algebraic (fixed) codebook excitation (Q11)  */  
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*                                                                   */
/*===================================================================*/


void dec3_10jcelp_fx(
	Word16   pit_lag,
	Word16  pit_gain,
	Word16   l_subfr,
  	Word16   *indices,    
 	Word16 cod[]       
)
{
  

       	Word16    i, pos, flag, index=(Word16) indices[0];
  	Word16   r, s, sign,q,rem;
  	static Word16 offset[3] = {0, 2, 4};
  	
	UNS_Word16  pos_table[768];
        Word16 pos1[3],tt,ix;
	/*--------------------------------------*/
	/* 	Reconstruct configuration 	*/
	/*--------------------------------------*/
         
          flag=JCELP_Config_fx(pit_lag, pit_gain, l_subfr, pos_table,&offset[1],&sign, &r);

 
     	
 	/*--------------------------------------------------------------------*
  	 * BUILD THE CODEWORD, THE FILTERED CODEWORD AND INDEX OF CODEVECTOR. *
 	 *--------------------------------------------------------------------*/
	  
  	for (i=0; i<L_CODE; i++) cod[i] = 0;
  	//s = ((index & 0x0200) == 0)? 8192 : -8192;
#ifdef WMOPS_FX
  logic16();
#endif
      s = ((index & 0x0200) == 0)? 2048 : -2048;

#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
	if (flag > 0) {
		ix = index & 0x1ff;
  		
#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
		if ((ix&1)==0) //even
		    {
		      tt=shr(extract_l(L_mult(ix,3)),2); //(ix*3)/2
		      pos1[0]=shr(pos_table[tt],8);  // upper 8
#ifdef WMOPS_FX
  move16();
  logic16();
#endif
                      pos1[1]=(pos_table[tt]& 0xFF); // lower 8
		      pos1[2]=shr(pos_table[tt+1],8);  // upper 8

		    }
		  else
		    {
		      tt=add(shr(extract_l(L_mult(shr(ix,1),3)),1),1); //((ix/2)*3)+1
#ifdef WMOPS_FX
  move16();
  logic16();
#endif
                      pos1[0]=(pos_table[tt]& 0xFF); // lower 8
	              pos1[1]=shr(pos_table[tt+1],8);  // upper 8
#ifdef WMOPS_FX
  move16();
  logic16();
#endif
                      pos1[2]=(pos_table[tt+1]& 0xFF); // lower 8
		    }
		
            for (i=2; i>=0; i--) {
#ifdef WMOPS_FX
  move16();
#endif
			pos = pos1[i];
                        cod[pos] = add(cod[pos],s);
 			s = negate(s);
		}
	} else {
#ifdef WMOPS_FX
  logic16();
#endif
		index &= 0x01ff;		/* strip off sign bit */
		
  		
                q=extract_l(L_shr(L_mult(index,1425),16));
		rem=sub(index,shr(extract_l(L_mult(q,23)),1));
		i=index0_fx[extract_l(L_shr(L_mult(index,1425),16))]; // 1425 = (1/23) in Q15
  		cod[i] = add(cod[i],s);
  		cod[i+offset[1]] = add(cod[i+offset[1]],mult_r(r,s));  //Q(11+15+1-16=11)
  
#ifdef WMOPS_FX
  move16();
#endif
  		i = index1_fx[rem];
  		
                cod[i] =add(cod[i],shr(extract_l(L_mult(s,sign)),1));
            

		
	}
	
		
#ifdef WMOPS_FX
	  test();
#endif
    	if(flag == 2)
		for (i = pit_lag; i < l_subfr; i++) cod[i] =add(cod[i],cod[sub(i,pit_lag)]);
    
 
        
  	return; 
}
