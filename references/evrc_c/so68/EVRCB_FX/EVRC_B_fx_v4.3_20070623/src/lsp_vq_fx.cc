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
/*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for           */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/

#include "stdio.h"
#include "typedef_fx.h"

#include "dsp_math.h"
#include "macro_fx.h"
#include "typedef_fx.h"
#include "basic_op40.h"
#include "basic_op.h"
#include "globs_fx.h"

#include  "lsp_cb2233_6697e_fx.h"
#include  "lsp_cb334_778_fx.h"
#include  "lsp_cb55_88_fx.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif

#define SCALED_OneDivPI2 5215

/*===================================================================*/
/* FUNCTION      :  calc_lsp()                                       */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function returns the quantized LSP element  */
/*                  of the lsp segment                               */
/*                                                                   */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      x:        element of the codebook (Q8)    */
/*         _ (Word16)      s:        (upper- lower) end reference    */
/*                                    Q0                             */
/*         _ (Word16)      low:      lower end reference (Q0)        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word16)      res:      quantized lsp element (Q15)     */
/*===================================================================*/



Word16 calc_lsp(Word16 x, Word16 s, Word16 low)
{
  Word16 res;
  Word32 Ltemp;

  Ltemp=L_mult(x,s);
	     
  res =round32_16(L_add(L_deposit_h(low),Ltemp));
    
  return(res);
}




/*----------------------------------------------------------------------------*/
/*------------------------------- FUNCTIONS ----------------------------------*/
/*----------------------------------------------------------------------------*/

/*===================================================================*/
/* FUNCTION      :  dequant_lsp_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function returns the quantized LSP segment  */
/*                  given the index of the lsp quantization table    */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      index:         index of the table (Q0)    */
/*         _ (Word16)      ref_l:         lower end reference(Q0)    */
/*         _ (Word16)      ref_u:         upper end reference (Q0)   */
/*         _ (UNS_Word16 [])  codebook:   packed lsp quantization    */
/*	                                  table (two Q8 entries      */
/*					  packed into 1 word)        */
/*         _ (Word16)      vec_dimension: size of lsp segment(Q0)    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16 [])   lsp_fx:        output quantized lsp       */ 
/*                                          segment Q15              */      
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                            _ None.                                */
/*===================================================================*/
void dequant_lsp_fx(Word16 index, Word16 ref_l, Word16 ref_u, UNS_Word16 codebook[],
	                  Word16 vec_dimension, Word16 lsp_fx[])
{
      Word16 i,s;
      UNS_Word16 *cb;
      //Word16 minspace16=0x105;
      Word16 u,l; // (0.05/(2*pi)) in Q15
      //Word32 minspace32=0x104C26C; //(0.05/(2*pi)) in Q31
      Word16 ri,nindx,zr,end;
      
      s = sub(ref_u,ref_l);
      
      /*********** unpacking *********************/
      
      nindx=shr(index,1);
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
      if (index & 1) // odd
	 ri=1;
      else
	ri=0;
           
 
      cb=codebook + shr(extract_l(L_mult(nindx,vec_dimension)),1);
      
      
#ifdef WMOPS_FX
  test();
#endif
      if (ri==0){ // even indx => upper entry
	for(i=0;i<vec_dimension;i++){
	   u=shl((cb[i]>>8),7); //u8
	   lsp_fx[i]=calc_lsp(u,s,ref_l);
	 }
      }
      else{  //odd indx => lower entry 
	for(i=0;i<vec_dimension;i++){
	   l=shl((cb[i] & 0xFF),7); // l8
	   
	   lsp_fx[i]=calc_lsp(l,s,ref_l);
	   
	 }
      }	    

#ifdef WMOPS_FX
  test();
#endif
      if (ri==0)
	{
	  zr=shl((cb[0]>>8),7); //u8
	  end=shl((cb[sub(vec_dimension,1)]>>8),7); //u8
	}
      else
	{
	  zr=shl((cb[0] & 0xFF),7); // l8
	  end=shl((cb[sub(vec_dimension,1)] & 0xFF),7); // l8
	}

	    
	
	
}



/*===================================================================*/
/* FUNCTION      :  calc_weight()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  This function computes the weight given delta1
                    and delta2                                       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*         _ (Word16)      delta1: (Q0)                              */
/*         _ (Word16)      delta2: (Q0)                              */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*         _ (Word16 *)    n1: o/p weight is Q(32-n1)                */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                            _ None.                                */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*         _ (Word32)    Lsum1: computed weight   Q(32-n1)           */
/*                       (alpha/(sqrt(delta1*delta2))                */
/*===================================================================*/



Word32 calc_weight(Word16 delta1,Word16 delta2,Word16 *n1 )
{
  Word40 Lacc;
  Word16 S1h,S1l,n; 
  static Word16 alpha =0x5180;  // (0.5/(2*pi)) in Q15 , norm-fac = 3 (not fully normalized)
  Word32 Lsum1,Lsum3,Linv;
          Lsum1=L_shr(L_mult(delta1,delta2),1);// Q(31-1)=Q30
          Lsum3 = dsp_sqrt_lut(Lsum1,0);// Q31 (Q(15+16)) 
	  
          Linv=invert_lut(Lsum3,n1);// Q(61-(31)-n1)

#ifdef WMOPS_FX
  move32();
#endif
          Lacc=0;
          S1h=extract_h(Linv);
	  S1l=extract_l(Linv);  
          Lacc=L_mult_su(alpha,S1l);
         Lsum1=(Word32) L_add40(L_shr40(Lacc,16),L_shr(L_mult(S1h,alpha),1));// Q(32-n1)
	  n=norm_l(Lsum1); // Lsum1 never = 0 
	  *n1=sub(*n1,n); 
	  return(L_shl(Lsum1,n)); //Lsum1 in Q(32-n1) where n1=n1-n
}
	   




/*=================================================================== */
/* FUNCTION      :  weight_lsp_fx ()                                  */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes the weights for the
                    given unquantized lsp vector                      */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   lsp_nq_fx:   input unquantized lsp vector  */

/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16 [])  w: weight vector   //Q(16-n_max)            */
         
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */


void weight_lsp_fx(Word16 lsp_nq_fx[],Word16 w[])
{
    Word16 i,n1,norm[ORDER];
    Word32 Lsum1[ORDER];
    Word16 delta1, delta2, temp;
    Word16 n_max=MIN_16;
    
    
    temp=0;
    
    for (i=0; i<sub(ORDER,1); i++) {
          delta1=sub(lsp_nq_fx[i],temp);
	  delta2=sub(lsp_nq_fx[i+1],lsp_nq_fx[i]);
	  Lsum1[i]=calc_weight(delta1,delta2,&n1); //Q(32-n1)
	  norm[i]=n1;
#ifdef WMOPS_FX
  test();
  Nmove16(2);
#endif
	  if (norm[i]>n_max)
	      n_max=norm[i];
	  temp=lsp_nq_fx[i];
      }
    delta1=sub(lsp_nq_fx[i],temp);
    delta2=sub(16384,lsp_nq_fx[i]);
    //delta2=sub(16384,temp);
     
    Lsum1[i]=calc_weight(delta1,delta2,&n1);
    norm[i]=n1;
#ifdef WMOPS_FX
  test();
  move16();
#endif
    if (norm[i]>n_max)
	  n_max=norm[i];
	    
    
    for(i=0;i<ORDER;i++)
      	w[i]=round32_16(L_shl(Lsum1[i],sub(norm[i],n_max)));//convert from Q(32-n1) to Q(16-n_max)
    
}
 

/*----------------------------------------------------------------------------*/




/*=================================================================== */
/* FUNCTION      :   calc_min_update_indx ()                          */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes the weighted MSE (WMSE)
                    for the input codebook vector and updates the
		    minimum weighted MSE (WMSEm) and corresponding
		    table index , if WMSE < WMSEm                     */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   temp: unpacked input codebook vector (Q15  */
/*         _ (Word16 [])   lsp_rel: relative unquantized lsp
	                    vector (Q15)                              */  
/*         _ (Word16 [])   w : weight vector                          */
/*         _ (Word16)      i : index of temp (Q0)                     */
/*         _ (Word16)      vec_dimension: size of lsp segment         */
/*         _ (Word16)      minspace                                   */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word32*)     s0: WMSEm                                  */
/*         _ (Word16*)     i0: corresponding table index (Q0)         */         
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */


void calc_min_update_indx(Word16 temp[],Word16 lsp_rel[], Word16 w[], Word32 *s0,Word16 *i0,Word16 i,Word16 vec_dimension,Word16 minspace)
{
  Word16 j,t;
  Word40 Lacc,Lacc1;
  Word32 Ltemp;
  Word16 temph;
  UNS_Word16 templ;
  
  for(j=0,Lacc=0.0,Lacc1=0.0;j<vec_dimension;j++)
    {
                   t = sub(lsp_rel[j],temp[j]); // Q15
		   Ltemp=L_mult(t,t);
                   templ=extract_l(Ltemp);
		   
                   //Lacc=L_add40(Lacc,(Word32)L_mult_su(w[j],templ));
		   Lacc=L_mac40_su(Lacc,w[j],templ);
				
                   temph=extract_h(Ltemp);
                   Lacc1=L_mac40(Lacc1,w[j],temph);
    }

  //Ltemp=(Word32) L_add40(L_shr40(Lacc,15),(Word32) Lacc1);
   Lacc=L_add40(Lacc1,(Word32) L_shr40(Lacc,15));

#ifdef WMOPS_FX
  test();
  move32();
  move16();
#endif
      if(L_sub40(Lacc,*s0)<0) {
		    
				*s0 = (Word32) Lacc;
				*i0 = i;
	
      }

}



/*=================================================================== */
/* FUNCTION      :  quant_lsp_fx ()                                   */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP segment   */
/*                  and corresponding table index, given the
		    input unquantized lsp segment                     */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   lsp_nq_fx: input unquantized lsp segment
	                   (Q15)                                      */
/*         _ (Word16)      ref_l: lower end reference (Q0)            */
/*         _ (Word16)      ref_u: upper end reference (Q0)            */
/*         _ (UNS_Word16 [])  codebook:   packed lsp quantization     */
/*	                                  table (two Q8 entries       */
/*					  packed into 1 word)         */
/*         _ (Word16)      codebook_size: # of entries in codebook    */
/*         _ (Word16)      vec_dimension: size of lsp segment         */
/*         _ (Word16 [])   w: weight vector (Q11)                     */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16)   i0: return index for quantized lsp (Q0)       */
/*         _ (Word16 []) lsp_q_fx: output quantized lsp segment (Q15) */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */



int quant_lsp_fx( Word16 lsp_nq_fx[], Word16 ref_l, Word16 ref_u, UNS_Word16 codebook[], 
        Word16 codebook_size, Word16 vec_dimension, Word16 w[], Word16 lsp_q_fx[])
{      
    
     Word16 i,i0=0,j,n,s;
     
     Word16 lsp_rel[ORDER];
     UNS_Word16 *cb_ptr = codebook;
     Word16 minspacenr=0x4140,even[vec_dimension],odd[vec_dimension]; // norm-fac = 6, (0.05/(2*pi)=261=0x104) in Q15   
     Word16 minspace,sinv;
     Word16 cs2;
     Word32 s0=MAX_32,sinv32;
                                                                                                                                                                        
         
     /* Convert absolute unquantized lsp to relative unquantized lsp */
    
  
     s = sub(ref_u,ref_l);//Q15
     sinv32=invert_lut(L_deposit_h(s),&n);// Q(30-n)
     
     //n1=norm_l(sinv32);
     //printf("%d\n",n1);
    
     sinv=round32_16(sinv32); //Q(14-n) 
     //n1=sub(n1,n);
     
    
     // minspace = mult_r(minspacenr,sinv); // Q(15+6+14-n+1-16)= Q(20-n)
      //minspace=shr(minspace,sub(5,n)); //Q15
      
      minspace = round32_16(L_shr(L_mult(minspacenr,sinv),sub(5,n))); // Q(15+6+14-n+1-16)= Q(20-n)
      
      for(i=0; i<vec_dimension; i++)
	lsp_rel[i] = round32_16(L_shl(L_mult(sub(lsp_nq_fx[i],ref_l),sinv),add(1,n)));  //Q15
	//lsp_rel[i] = shl(mult_r(sub(lsp_nq_fx[i],ref_l),sinv),add(1,n));  //Q15
	  

	 /* Search for optimal quantized lsp vector */
      cs2=shr(codebook_size,1);
      
      for(i=0;i<cs2;i++)
	  {
	  for(j=0;j<vec_dimension;j++){	    
	     even[j]=shl((*cb_ptr) >> 8,7); //u8
           odd[j]=shl((*cb_ptr) & 0xFF,7); // l8
	     cb_ptr++;
	  }
	     
	  calc_min_update_indx(even, lsp_rel,  w, &s0, &i0, shl(i,1), vec_dimension, minspace);
          calc_min_update_indx(odd, lsp_rel,  w, &s0, &i0, add(shl(i,1),1), vec_dimension, minspace);
          }
        

// Update quantized lsp vector and return corresponding index  
	
dequant_lsp_fx(i0, ref_l, ref_u, codebook, vec_dimension, lsp_q_fx);

 
return(i0);
    

}


/*=================================================================== */
/* FUNCTION      :   calc_min_update_indx_cand ()                     */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes the weighted MSE (WMSE)
                    for the input codebook vector and updates the
		    max of the 'num_cand' minimum errors(WMSEm)
		    and the corresponding pointer, if WMSE < WMSEm    */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   temp: unpacked input codebook vector (Q15  */
/*         _ (Word16 [])   lsp_rel: relative unquantized lsp
	                   vector (Q15)                               */  
/*         _ (Word16 [])   w : weight vector                          */
/*         _ (Word16)      i : index of temp (Q0)                     */
/*         _ (Word16 [])   err_buf : buffer for storing 'num_cand'
	                   error values                               */
/*         _ (Word16 [])   indx_buf: buffer for storing table index
	                   for corresponding error values in
			   err_buf                                    */
/*         _ (Word16)      num_cand: number of candidates to search   */
/*         _ (Word16)      vec_dimension: size of lsp segment         */
/*         _ (Word16)      minspace                                   */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16*)     j0: ptr in the num_cand error buffer that
	                   contains s0                                */ 
/*         _ (Word32*)     s0: s0 is the max of the num_cand minimum
	                   errors                                     */
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */


void calc_min_update_indx_cand(Word16 temp[],Word16 lsp_rel[], Word16 w[], Word16 i,Word32 err_buf[], Word16 indxbuf[], Word16 *j0, Word32 *s0,  Word16 num_cand, Word16 vec_dimension, Word16 minspace)
{
  Word16 j,t;
  Word40 Lacc,Lacc1;
  Word32 Ltemp;
  Word16 temph;
  UNS_Word16 templ;
  for(j=0,Lacc=0.0,Lacc1=0.0;j<vec_dimension;j++)
    {
                   t = sub(lsp_rel[j],temp[j]); // Q15
		   Ltemp=L_mult(t,t);
                   templ=extract_l(Ltemp);
		   
		   // Lacc=L_add40(Lacc,(Word32)L_mult_su(w[j],templ));
		   Lacc=L_mac40_su(Lacc,w[j],templ);
				
                   temph=extract_h(Ltemp);
                   Lacc1=L_mac40(Lacc1,w[j],temph);
    }

  //Ltemp=(Word32) L_add40(L_shr40(Lacc,15),(Word32) Lacc1);
  Lacc=L_add40(Lacc1, (Word32) L_shr40(Lacc,15));


   /* (*s0) is the max of the num_cand minimums                                                        */ 
   /* (*j0) is the ptr in the num_cand error buffer that contains s0                                   */
   /* i.e. if the current error is more than s0, we just pass                                          */
   /* else we replace s0 with current error, and j0 with current index                                 */
   /* And then we re-search for max among the 'num_cand' error candidates to figure out new *s0, *j0   */
   
#ifdef WMOPS_FX
  test();
  move32();
  Nmove16(3);
#endif
      if(L_sub40(Lacc,(*s0))<0) {
	       
		    //note: *j0 pts to where max of the min err is
	            err_buf[(*j0)]=(Word32) Lacc;
		    indxbuf[(*j0)]=i;

		    
		    //need to find new s0 (max), and j0 now
                     *s0=err_buf[0];
		     *j0=0;
		     for (j=1;j<num_cand;j++) {
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
			  if (err_buf[j]>(*s0)) {
			   *s0=err_buf[j];
			    *j0=j;
			  }
		     }
				
	
      }

}



/*=================================================================== */
/* FUNCTION      :  quant_lsp_mult_cand_fx ()                         */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns 'num_cand' best indices
                    given the  input unquantized lsp segment          */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   lsp_nq_fx: input unquantized lsp segment
	                   (Q15)                                      */
/*         _ (Word16)      ref_l: lower end reference (Q0)            */
/*         _ (Word16)      ref_u: upper end reference (Q0)            */
/*         _ (UNS_Word16 [])  codebook:   packed lsp quantization     */
/*	                                  table (two Q8 entries       */
/*					  packed into 1 word)         */
/*         _ (Word16)      codebook_size: # of entries in codebook    */
/*         _ (Word16)      vec_dimension: size of lsp segment         */
/*         _ (Word16 [])   w: weight vector (Q11)                     */
/*         _ (Word16)      num_cand: number of candidates to search   */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16[])   indxbuf: buffer of 'num_cand' best table
	                  indices                                     */
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */



void quant_lsp_mult_cand_fx( Word16 lsp_nq_fx[], Word16 ref_l, Word16 ref_u, UNS_Word16 codebook[], 
        Word16 codebook_size, Word16 vec_dimension, Word16 w[], Word16 num_cand, Word16 indxbuf[])
{      
    
     Word16 i,j,n,s;
     
     Word16 lsp_rel[ORDER];
     UNS_Word16 *cb_ptr = codebook;
     Word16 minspacenr=0x4140; // norm-fac = 6, (0.05/(2*pi)=261=0x104) in Q15   
     Word16 minspace,sinv,index1,j0=0,even[vec_dimension],odd[vec_dimension],cs2;
     
     Word32 s0=MAX_32,sinv32;
     Word32 index,err_buf[num_cand];
     
     
     
	/* Convert absolute unquantized lsp to relative unquantized lsp */
    

     
     s = sub(ref_u,ref_l);//Q15
     sinv32=invert_lut(L_deposit_h(s),&n);// Q(30-n)
     
    
     sinv=round32_16(sinv32); //Q(14-n)  
     //n1=sub(n1,n);
     
    
     // minspace = mult_r(minspacenr,sinv); // Q(15+6+14+n1+1-16)= Q(20+n1)
      //minspace=shr(minspace,add(n1,5)); //Q15
     minspace = round32_16(L_shr(L_mult(minspacenr,sinv),sub(5,n))); // Q(15+6+14-n+1-16)= Q(20-n)
     
	
	for(i=0; i<vec_dimension; i++) 
          {
	    //lsp_rel[i] = shl(mult_r(sub(lsp_nq_fx[i],ref_l),sinv),sub(1,n1));  //Q15
	   lsp_rel[i] = round32_16(L_shl(L_mult(sub(lsp_nq_fx[i],ref_l),sinv),add(1,n)));  //Q15 
	   
          }

	 /* Search for optimal quantized lsp vector */
       
         //Initialization
	for (i=0;i<num_cand;i++) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
	  err_buf[i]=MAX_32;
	  indxbuf[i]=0;
	}


	cs2=shr(codebook_size,1);
        for(i=0;i<cs2;i++)
	  {
	  for(j=0;j<vec_dimension;j++){	    
	     even[j]=shl((*cb_ptr) >> 8,7); //u8
             odd[j]=shl((*cb_ptr) & 0xFF,7); // l8
             cb_ptr++;
	  }


	  calc_min_update_indx_cand(even,lsp_rel,w,shl(i,1), err_buf, indxbuf, &j0, &s0, num_cand, vec_dimension, minspace);

	  calc_min_update_indx_cand(odd,lsp_rel,w,add(shl(i,1),1), err_buf, indxbuf, &j0, &s0, num_cand, vec_dimension, minspace);
          }

        /************** insertion sorting of err_buf and indx_buf to order best matches **************/
	for (i=1; i < num_cand; i++)
        {
#ifdef WMOPS_FX
  Nmove16(2);
#endif
          index = err_buf[i];
	  index1 =indxbuf[i];
          j = i;
#ifdef WMOPS_FX
  test();
  logic16();
  Nmove16(2);
#endif
         while ((j > 0) && (err_buf[sub(j,1)] > index))
          {
                err_buf[j] = err_buf[sub(j,1)];
		indxbuf[j]=indxbuf[sub(j,1)];
                 j = sub(j,1);
           }
#ifdef WMOPS_FX
  Nmove16(2);
#endif
          err_buf[j] = index;
	  indxbuf[j] = index1;
        }
	   
}




/*=================================================================== */
/* FUNCTION      :  enc_lsp_vq_28_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  28-bit quantization of the input lsp vector       */
/*                  (4 Split VQ)
 ------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
/*         _ (Word16 [])   lsp_nq: input unquantized lsp vector (Q15) */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16[])   codes: buffer of indices for the 4 lsp
	                  segments                                    */
/*         _ (Word16[])   lsp : quantized lsp vector                  */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */




void	enc_lsp_vq_28_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]) 
{
         Word16	i, j, k;
       
	 Word16  w[ORDER];
	 Word16	lsp_index0[PRE_LSP_NUM0], lsp_index2[PRE_LSP_NUM2], i0, i1, i2, i3[6];
	 Word16 s,tj;
	 Word16 lsp_temp[18],temph,lsp_temp1[18];
	 Word32 Ltemp,Lsum,s_min;
	 Word40 Lacc,Lacc1;
	 UNS_Word16 templ;
	 
	 Word16 tmp_tbl0[10];
	 

	weight_lsp_fx(lsp_nq,w);
	
	/* Quantization. First and last segs absolute. The rest delta */
	
        quant_lsp_mult_cand_fx(lsp_nq,0,16384,lsp_tbl28_0_fx,M3_L_LSP_SEG0_28, M3_N_LSP_SEG0_28, w,PRE_LSP_NUM0,lsp_index0);

	quant_lsp_mult_cand_fx(lsp_nq+M3_B_LSP_SEG2_28, 0, 16384, lsp_tbl28_2_fx , M3_L_LSP_SEG2_28, M3_N_LSP_SEG2_28, w+M3_B_LSP_SEG2_28, PRE_LSP_NUM2,lsp_index2);
   


       for(j=0;j<PRE_LSP_NUM2; j++) {
         tj=shr(extract_l(L_mult(j,3)),1);
         i2 = lsp_index2[j];
		 dequant_lsp_fx(i2, 0, 16384, lsp_tbl28_2_fx, M3_N_LSP_SEG2_28, lsp_temp1+tj);
		 i3[j] = quant_lsp_fx(lsp_nq+M3_B_LSP_SEG3_28, lsp_temp1[add(tj,2)], 16384, lsp_tbl28_3_fx, M3_L_LSP_SEG3_28, M3_N_LSP_SEG3_28, w+M3_B_LSP_SEG3_28, lsp_temp+tj);
		    
       }
#ifdef WMOPS_FX
  move32();
#endif
         s_min = MAX_32;
	
	for(i=0; i<PRE_LSP_NUM0; i++) {

#ifdef WMOPS_FX
  move16();
#endif
		i0 = lsp_index0[i];
				
		dequant_lsp_fx(i0, 0, 16384, lsp_tbl28_0_fx, M3_N_LSP_SEG0_28, tmp_tbl0);
		
#ifdef WMOPS_FX
  move32();
#endif
		
		for(j=0;j<sub(PRE_LSP_NUM2,i); j++) {
                  tj=shr(extract_l(L_mult(j,3)),1);
                  i2 = lsp_index2[j];
		  
#ifdef WMOPS_FX
  test();
#endif

# if 1
			if ((tmp_tbl0[sub(M3_B_LSP_SEG1_28,1)]>=lsp_temp1[tj]))
			  continue;
#endif

		  i1 = quant_lsp_fx(lsp_nq+M3_B_LSP_SEG1_28, tmp_tbl0[sub(M3_B_LSP_SEG1_28,1)], lsp_temp1[tj],lsp_tbl28_1_fx,M3_L_LSP_SEG1_28, M3_N_LSP_SEG1_28, w+M3_B_LSP_SEG1_28, tmp_tbl0+M3_B_LSP_SEG1_28);


	         for(k=0;k<3;k++){
                        tmp_tbl0[4+k]=lsp_temp1[tj+k];
			tmp_tbl0[7+k]=lsp_temp[tj+k];
			 }	
			
		 for(k=0,Lacc=0.0,Lacc1=0.0; k<10; k++) {
				s = sub(tmp_tbl0[k],lsp_nq[k]);
                                
				Ltemp=L_mult(s,s);
                                templ=extract_l(Ltemp);
		   
                                //Lacc=L_add40(Lacc,(Word32)L_mult_su(w[k],templ));
				Lacc=L_mac40_su(Lacc,w[k],templ);
				
                                temph=extract_h(Ltemp);
                                Lacc1=L_mac40(Lacc1,w[k],temph);
				
			}
			
			 Lsum=(Word32) L_sat32_40(L_add40(Lacc1, (Word32) L_shr40(Lacc,15)));
#ifdef WMOPS_FX
  test();
#endif		
#ifdef WMOPS_FX
  test();
#endif
			if(Lsum < s_min) {
			 
#ifdef WMOPS_FX
  Nmove16(4);
  move32();
#endif
			  	codes[0] = i0;
				codes[1] = i1;
				codes[2] = i2;
				codes[3] = i3[j];
				for(k=0;k<10;k++) lsp[k]=tmp_tbl0[k];
				s_min = Lsum;
			}
			
		}
	}

      
}



/*=================================================================== */
/* FUNCTION      :  dec_lsp_vq_28_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP vector    */
/*                  given the LSPs codevector, using the table for
		    28-bit lsp quantization                           */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :                                                 */
/*           _ (Word16 [])  codes: LSPs codevector                    */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
                               
/*           _ (Word16 [])  lsp_fx: output quantized lsp vector       */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */



void dec_lsp_vq_28_fx( Word16 codes[], Word16 lsp[]) 
{
	
	/* Decode the LSPs codevector codes[] */
	dequant_lsp_fx(codes[0], 0, 16384, lsp_tbl28_0_fx, M3_N_LSP_SEG0_28, lsp);
	
	dequant_lsp_fx(codes[2], 0, 16384, lsp_tbl28_2_fx, M3_N_LSP_SEG2_28, lsp+M3_B_LSP_SEG2_28);
	
	dequant_lsp_fx(codes[1], lsp[sub(M3_B_LSP_SEG1_28,1)], lsp[M3_B_LSP_SEG2_28], lsp_tbl28_1_fx, 
		M3_N_LSP_SEG1_28, lsp+M3_B_LSP_SEG1_28);
	
	dequant_lsp_fx(codes[3], lsp[sub(M3_B_LSP_SEG3_28,1)], 16384, lsp_tbl28_3_fx, 
		M3_N_LSP_SEG3_28, lsp+M3_B_LSP_SEG3_28);
	
}


/*=================================================================== */
/* FUNCTION      :  enc_lsp_vq_22_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  22-bit quantization of the input lsp vector       */
/*                  (3 Split VQ)
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
/*         _ (Word16 [])   lsp_nq: input unquantized lsp vector (Q15) */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16[])   codes: buffer of indices for the 3 lsp
	                  segments                                    */
/*         _ (Word16[])   lsp : quantized lsp vector                  */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */




void	enc_lsp_vq_22_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]) 
{
         Word16	i, j, k;
	 Word16  w[ORDER];
	 Word16	lsp_index0[PRE_LSP_NUM0], lsp_index2[PRE_LSP_NUM2], i0, i1, i2;
	 Word16 s,tj;
	 Word16 temph,lsp_temp1[24];
	 Word32 Ltemp,Lsum,s_min;
	 Word40 Lacc,Lacc1;
	 UNS_Word16 templ;
	 Word16 tmp_tbl0[10];
	 
     
	weight_lsp_fx(lsp_nq,w);
     
	
	/* Quantization. First and last segs absolute. The rest delta */
	
        quant_lsp_mult_cand_fx(lsp_nq,0,16384,lsp_tbl22_0_fx,M3_L_LSP_SEG0_22, M3_N_LSP_SEG0_22, w,PRE_LSP_NUM0,lsp_index0);
	 
	     
	quant_lsp_mult_cand_fx(lsp_nq+M3_B_LSP_SEG2_22, 0, 16384, lsp_tbl22_2_fx , M3_L_LSP_SEG2_22, M3_N_LSP_SEG2_22, w+M3_B_LSP_SEG2_22, PRE_LSP_NUM2,lsp_index2);
	 	     
	      
	 
#ifdef WMOPS_FX
  move32();
#endif

       for(j=0;j<PRE_LSP_NUM2;j++){
	  tj=shr(extract_l(L_mult(j,4)),1);
	  i2 = lsp_index2[j];		  
	  dequant_lsp_fx(i2, 0, 16384, lsp_tbl22_2_fx, M3_N_LSP_SEG2_22, lsp_temp1+tj);    
	}

  
         s_min = MAX_32;
	
	for(i=0; i<PRE_LSP_NUM0; i++) {

#ifdef WMOPS_FX
  move16();
#endif
		i0 = lsp_index0[i];
				
		dequant_lsp_fx(i0, 0, 16384, lsp_tbl22_0_fx, M3_N_LSP_SEG0_22, tmp_tbl0);
		
		
		for(j=0;j<sub(PRE_LSP_NUM2,i); j++) {
                  tj=shr(extract_l(L_mult(j,4)),1);
#ifdef WMOPS_FX
  move16();
#endif



# if 1
		 if ((tmp_tbl0[sub(M3_B_LSP_SEG1_22,1)]>=lsp_temp1[tj]))
		  continue;
#endif
  
		  i1 = quant_lsp_fx(lsp_nq+M3_B_LSP_SEG1_22, tmp_tbl0[sub(M3_B_LSP_SEG1_22,1)], lsp_temp1[tj],lsp_tbl22_1_fx,M3_L_LSP_SEG1_22, M3_N_LSP_SEG1_22, w+M3_B_LSP_SEG1_22, tmp_tbl0+M3_B_LSP_SEG1_22);

		 for(k=0;k<4;k++)
			  tmp_tbl0[6+k]=lsp_temp1[tj+k]; 
		 
		 for(k=0,Lacc=0.0,Lacc1=0.0; k<10; k++) {
				s = sub(tmp_tbl0[k],lsp_nq[k]);
                                
				Ltemp=L_mult(s,s);
                                templ=extract_l(Ltemp);
		   
                                //Lacc=L_add40(Lacc,(Word32)L_mult_su(w[k],templ));
				Lacc=L_mac40_su(Lacc,w[k],templ);
				
                                temph=extract_h(Ltemp);
                                Lacc1=L_mac40(Lacc1,w[k],temph);
				
			}
		  Lsum=(Word32)  L_sat32_40(L_add40(Lacc1,(Word32) L_shr40(Lacc,15)));
			
#ifdef WMOPS_FX
  test();
#endif
		  if(Lsum < s_min) {
			 
#ifdef WMOPS_FX
  Nmove16(3);
#endif
			  	codes[0] = i0;
				codes[1] = i1;
				codes[2] = lsp_index2[j];
				for(k=0;k<10;k++) lsp[k]=tmp_tbl0[k];
				s_min = Lsum;
			}
			
		}
	}
}



/*=================================================================== */
/* FUNCTION      :  dec_lsp_vq_22_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP vector    */
/*                  given the LSPs codevector, using the table for
		    22-bit lsp quantization                           */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :                                                 */
/*           _ (Word16 [])  codes: LSPs codevector                    */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
                               
/*           _ (Word16 [])  lsp_fx: output quantized lsp vector       */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */



void dec_lsp_vq_22_fx( Word16 codes[], Word16 lsp[]) 
{
		
	/* Decode the LSPs codevector codes[] */
	dequant_lsp_fx(codes[0], 0, 16384, lsp_tbl22_0_fx, M3_N_LSP_SEG0_22, lsp);
	
	dequant_lsp_fx(codes[2], 0, 16384, lsp_tbl22_2_fx, M3_N_LSP_SEG2_22, lsp+M3_B_LSP_SEG2_22);
	
	dequant_lsp_fx(codes[1], lsp[sub(M3_B_LSP_SEG1_22,1)], lsp[M3_B_LSP_SEG2_22], lsp_tbl22_1_fx, 
		M3_N_LSP_SEG1_22, lsp+M3_B_LSP_SEG1_22);
	
}






/*=================================================================== */
/* FUNCTION      :  enc_lsp_vq_16_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  16-bit quantization of the input lsp vector       */
/*                  (2 Split VQ)
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
/*         _ (Word16 [])   lsp_nq: input unquantized lsp vector (Q15) */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16[])   codes: buffer of indices for the 2 lsp
	                  segments                                    */
/*         _ (Word16[])   lsp : quantized lsp vector                  */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */




void	enc_lsp_vq_16_fx( Word16 lsp_nq[], Word16 codes[], Word16 lsp[]) 
{
         Word16	i, k;
	 Word16  w[ORDER];
	 Word16	lsp_index1[PRE_LSP_NUM2], i0, i1;
	 Word16 s;
	 Word16 temph;
	 Word32 Ltemp,Lsum,s_min;
	 Word40 Lacc,Lacc1;
	 UNS_Word16 templ;
	 Word16 indx[6],tmp_tbl1[10];
	 

	weight_lsp_fx(lsp_nq,w);
	
	/* Quantization. First and last segs absolute. The rest delta */
	
       quant_lsp_mult_cand_fx(lsp_nq+M3_B_LSP_SEG1_16,0,16384,lsp_tbl16_1_fx,M3_L_LSP_SEG1_16, M3_N_LSP_SEG1_16, w+M3_B_LSP_SEG1_16,PRE_LSP_NUM2,indx);
	   
       for(i=0;i<PRE_LSP_NUM2;i++)
	    lsp_index1[i] = indx[i];
	 
#ifdef WMOPS_FX
  move32();
#endif
       s_min = MAX_32;
	
       for(i=0; i<PRE_LSP_NUM2; i++) {

#ifdef WMOPS_FX
  move16();
#endif
		i1 = lsp_index1[i];
				
		dequant_lsp_fx(i1, 0, 16384, lsp_tbl16_1_fx, M3_N_LSP_SEG1_16, tmp_tbl1+M3_B_LSP_SEG1_16);
		
#ifdef WMOPS_FX
  move32();
#endif
		
		i0 = quant_lsp_fx(lsp_nq,0, tmp_tbl1[M3_B_LSP_SEG1_16],lsp_tbl16_0_fx,M3_L_LSP_SEG0_16, M3_N_LSP_SEG0_16, w, tmp_tbl1);
		 
 
			for(k=0,Lacc=0.0,Lacc1=0.0; k<10; k++) {
				s = sub(tmp_tbl1[k],lsp_nq[k]);
                                
				Ltemp=L_mult(s,s);
                                templ=extract_l(Ltemp);
		   
                                //Lacc=L_add40(Lacc,(Word32)L_mult_su(w[k],templ));
				Lacc=L_mac40_su(Lacc,w[k],templ);
				
                                temph=extract_h(Ltemp);
                                Lacc1=L_mac40(Lacc1,w[k],temph);
				
			}
		Lsum=(Word32) L_sat32_40(L_add40(Lacc1,(Word32) L_shr40(Lacc,15)));
		
#ifdef WMOPS_FX
  test();
#endif
		if(Lsum < s_min) {
			 
#ifdef WMOPS_FX
  Nmove16(2);
  move32();
#endif
			  	codes[0] = i0;
				codes[1] = i1;
				
				for(k=0;k<10;k++) lsp[k]=tmp_tbl1[k];
				
				s_min = Lsum;
			}
			
       }

}




/*=================================================================== */
/* FUNCTION      :  dec_lsp_vq_16_fx ()                               */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP vector    */
/*                  given the LSPs codevector, using the table for
		    16-bit lsp quantization                           */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :                                                 */
/*           _ (Word16 [])  codes: LSPs codevector                    */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
                               
/*           _ (Word16 [])  lsp_fx: output quantized lsp vector       */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */

void dec_lsp_vq_16_fx(Word16 codes[], Word16 lsp[]) 
{
		
	/* Decode the LSPs codevector codes[] */
	dequant_lsp_fx(codes[1], 0, 16384, lsp_tbl16_1_fx, M3_N_LSP_SEG1_16, lsp+M3_B_LSP_SEG1_16);
	dequant_lsp_fx(codes[0], 0,lsp[M3_B_LSP_SEG1_16], lsp_tbl16_0_fx, M3_N_LSP_SEG0_16, lsp);
	
	
}



/*=================================================================== */
/* FUNCTION      :   calc_min_update_indx1 ()                         */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function computes the weighted MSE (WMSE)
                    for the input codebook vector and updates the
		    minimum weighted MSE (WMSEm) and corresponding
		    table index , if WMSE < WMSEm                     */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
           _ (Word16 [])   temp: unpacked input codebook vector (Q15  */
/*         _ (Word16 [])   x: relative unquantized lsp
	                    vector (Q15)                              */  
/*         _ (Word16 [])   w : weight vector                          */
/*         _ (Word16)      i : index of temp (Q0)                     */
/*         _ (Word16)      vec_dimension: size of lsp segment         */
/*         _ (Word16)      im : (im==0) -> 1st split else
                           2nd split
           _ (Word16)      jpm,old : local variables required from
	                   lspmaq_fx1() function.
	                   df : min spacing between splits
			   (applicable for 1/8th rate only)           */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word32*)     s0: WMSEm                                  */
/*         _ (Word16*)     i0: corresponding table index (Q0)         */         
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */



void calc_min_update_indx1(Word16 temp[],Word16 x[], Word16 w[], Word32 *s0,Word16 *i0,Word16 i,Word16 vec_dimension,Word16 im,Word16 jpm,Word16 df,Word16 old)
{
  Word16 j,t;
  Word40 Lacc,Lacc1;
  Word32 Ltemp;
  Word16 temph;
  UNS_Word16 templ;
  
  for(j=0,Lacc=0.0,Lacc1=0.0;j<vec_dimension;j++)
    {
                   t = sub(x[j+im],temp[j]); // Q15
		   Ltemp=L_mult(t,t);
                   templ=extract_l(Ltemp);
		   
                   //Lacc=L_add40(Lacc,(Word32)L_mult_su(w[j+im],templ));
		   Lacc=L_mac40_su(Lacc,w[j+im],templ);
				
                   temph=extract_h(Ltemp);
                   Lacc1=L_mac40(Lacc1,w[j+im],temph);
    }

  
   Lacc=L_add40(Lacc1,(Word32) L_shr40(Lacc,15));

#ifdef WMOPS_FX
  test();
#endif
   if (im==0)
	 {
#ifdef WMOPS_FX
  test();
  move32();
  move16();
#endif
                    if(L_sub40(Lacc,*s0)<=0) {
	        	*s0 = (Word32) Lacc;
			*i0=i;
	
                    }
         }
   else
         {
                                      
#ifdef WMOPS_FX
  test();
  logic16();
  move32();
  move16();
#endif
	            if ((temp[0] > add(old,df)) && (L_sub40(Lacc,*s0)<=0)) {
		         *s0 = (Word32) Lacc;
		         *i0=i;
	           
	            }
         }

}


/*=================================================================== */
/* FUNCTION      :  lspmaq_fx1()                                      */
/*------------------------------------------------------------------- */
/* PURPOSE       :  16-bit quantization of the input lsp vector
                    (for QPPP LSP_VQ) 
                    (2 Split VQ, using EVRC lsp-vq routine)
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
/*         _ (Word16*)   x: input unquantized lsp vector (Q15)        */
/*         _ (Word16*)   nsub : array containing the size of each 
/*                       split (number of columns in each sub-matrix) */                   
/*         _ (Word16*)   nsiz : array containing the number of
/*                       rows in each sub-matrix                      */
/*         _ (Word16*)   w : Weight vector , block normalized         */
/*         _ (Word16*)   lsptab8_fx1: Codebook containing 2 
/*                       sub-matrices of size (nsiz[i]*nsub[i], i=1,2)*/
/*         _ (Word16)   flag =0 => Eighth-rate lsp_vq,                */
/*                           =1 => Voiced lsp_vq                      */
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16*)   indx: buffer of indices for the 2 lsp
	                  segments                                    */
/*         _ (Word16*)   y : quantized lsp vector                     */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */

void lspmaq_fx1(Word16 *x,Word16 *nsub,Word16 *nsiz,Word16 *w,Word16 *y, Word16 *index,Word16 *lsptab_fx1)
{

  /*===========================================================================
                         LOCAL VARIABLE DECLARATIONS
  /*===========================================================================*/

  
	
	
      short df;
      df=MIN_16;
            
	register short i, j, m, im, jp, jm, jpm, jj;
	
	
      
	Word16 old=0,i0=0,ri,nindx;
	Word32 s0;
	Word16 even[6],odd[6];
	
/*===========================================================================
                         START C CODE
===========================================================================*/
#ifdef WMOPS_FX
  Nmove16(3);
#endif
        jj=0;

	/* Code all sub-matrices */
	im = 0;
	jp = 0;
	for (m = 0; m < 2; m = add(m, 1))
	{
          
	  //dm=0x7FFF;
#ifdef WMOPS_FX
  move32();
#endif
	  s0=MAX_32;
            /* km = nsub[m] * kdim, since kdim always =1 , km=nsub[m]*/
		for (j = 0; j < nsiz[m]; j = add(j, 1))
		{
                 
		 	/* jm = km * j; */
			jm = shr(extract_l(L_mult(nsub[m], j)), 1);

			
			jpm = add(jp, jm);

	    
		 	  
                        for(i=0;i<nsub[m];i++)
			  {
			    even[i]= shl(shr(lsptab_fx1[jpm+i],8),8); // u8, Q15
			    odd[i]= extract_l(L_mult(lsptab_fx1[jpm+i],0x80)); // l8,Q15
			  }
#ifdef WMOPS_FX
  test();
  move16();
#endif
                if (im!=0) old=y[im-1];
		
		calc_min_update_indx1(even, x, w, &s0, &i0, shl(j,1), nsub[m],im,jpm,df,old);
                calc_min_update_indx1(odd, x, w, &s0, &i0, add(shl(j,1),1), nsub[m],im,jpm,df,old);
              	}
			    
		index[m] = i0;
                nindx=shr(index[m],1);
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	        if (index[m] & 1) //odd
	         ri=1;
	        else
	         ri=0;
	        jpm = add(jp, extract_l(L_shr(L_mult(nsub[m], nindx), 1)));
#ifdef WMOPS_FX
  test();
#endif
		if (ri==0){ // even index => upper entry
		  for (i = 0; i < nsub[m]; i++)
			y[im + i] = shl(shr(lsptab_fx1[jpm+i],8),8);
		}  
		else{ // odd index => lower entry
		 for (i = 0; i < nsub[m]; i++)
		  y[im+i]= extract_l(L_mult(lsptab_fx1[jpm+i],0x80)); // l8,Q15
		}

	
		im = add(im, nsub[m]);

		jp = add(jp, shr(extract_l(L_mult(nsub[m], nsiz[m])), 1));
	}

	
}								/* END MODULE lspmaq */





/*=================================================================== */
/* FUNCTION      :  lspmaq_fx8()                                      */
/*------------------------------------------------------------------- */
/* PURPOSE       :  8-bit quantization of the input lsp vector
                    (For 1/8th rate LSP_VQ)
/*                  (2 Split VQ, using EVRC lsp-vq routine)           */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :
/*         _ (Word16*)   x: input unquantized lsp vector (Q15)        */
/*         _ (Word16*)   nsub : array containing the size of each 
/*                       split (number of columns in each sub-matrix) */                   
/*         _ (Word16*)   nsiz : array containing the number of
/*                       rows in each sub-matrix                      */
/*         _ (Word16*)   w : Weight vector , block normalized         */
/*         _ (Word16*)   lsptab8_fx1: Codebook containing 2 
/*                       sub-matrices of size (nsiz[i]*nsub[i], i=1,2)*/
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
/*         _ (Word16*)   indx: buffer of indices for the 2 lsp
	                  segments                                    */
/*         _ (Word16*)   y : quantized lsp vector                     */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */

void lspmaq_fx8(Word16 *x,Word16 *nsub,Word16 *nsiz,Word16 *w,Word16 *y, Word16 *index,Word16 *lsptab8_fx1)
{

  /*===========================================================================
                         LOCAL VARIABLE DECLARATIONS
  /*===========================================================================*/

  
	
	
      short df;
     
      df=MIN_LSP_SEP;
      
	register short i, j, m, im, jp, jm, jpm, jj;
	
	 
      
	
	Word16 i0,old=0;
	Word32 s0;
		
	
	
/*===========================================================================
                         START C CODE
===========================================================================*/
#ifdef WMOPS_FX
  Nmove16(3);
#endif
        jj=0;

	/* Code all sub-matrices */
	im = 0;
	jp = 0;
      
	for (m = 0; m < 2; m = add(m, 1))
	{
          
	  //dm=0x7FFF;
#ifdef WMOPS_FX
  move32();
#endif
	  s0=MAX_32;
            /* km = nsub[m] * kdim, since kdim always =1 , km=nsub[m]*/
		for (j = 0; j < nsiz[m]; j = add(j, 1))
		{
                 
		 	/* jm = km * j; */
			jm = shr(extract_l(L_mult(nsub[m], j)), 1);

			
			jpm = add(jp, jm);
			
#ifdef WMOPS_FX
  test();
  move16();
#endif
           if (im!=0)
	      old=y[im-1];

	      
           calc_min_update_indx1(lsptab8_fx1+jpm, x, w, &s0, &i0, j, nsub[m],im,jpm,df,old);
            
		}
			    
				

#ifdef WMOPS_FX
  move16();
#endif
		index[m] = i0;
                jpm = add(jp, shr(extract_l(L_mult(nsub[m], index[m])), 1));

		for (i = 0; i < nsub[m]; i = add(i, 1))
		{
#ifdef WMOPS_FX
  move16();
#endif
			y[im + i] = lsptab8_fx1[jpm + i];
		}

		im = add(im, nsub[m]);

		jp = add(jp, shr(extract_l(L_mult(nsub[m], nsiz[m])), 1));
	}
	
 
	

}								/* END MODULE lspmaq */




/*=================================================================== */
/* FUNCTION      :  lspmaq_dec_fx ()                                  */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP vector    */
/*                  given the LSPs codevector, using the table for
		    16-bit QPPP lsp quantization                      */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :   
/*         _ (Word16*)   nsub : array containing the size of each 
/*                       split (number of columns in each sub-matrix) */                   
/*         _ (Word16*)   nsiz : array containing the number of
/*                       rows in each sub-matrix                      */
/*         _ (Word16 *)  index: LSPs codevector                       */
/*         _ (Word16*)   lsptab8_fx: Codebook containing 2 
/*                       sub-matrices of size (nsiz[i]*nsub[i], i=1,2)*/
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
                               
/*           _ (Word16 *)  y: output quantized lsp vector             */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */

void lspmaq_dec_fx(short *nsub, short *nsiz, short *y, short *index,short *lsptab_fx)
{
	register int i, m, im, jp, jpm;
        Word16 nindx,ri;
	/* Code all sub-matrices */
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	im = 0;
	jp = 0;
        
	
	for (m = 0; m < 2; m++)
	{
	  nindx=shr(index[m],1);
#ifdef WMOPS_FX
  test();
  logic16();
  move16();
#endif
	  if (index[m] & 1) //odd
	    ri=1;
	  else
	    ri=0;
	  
	  
		jpm = add(jp, extract_l(L_shr(L_mult(nsub[m], nindx), 1)));
#ifdef WMOPS_FX
  test();
#endif
		if (ri==0){ // even index => upper entry
		  for (i = 0; i < nsub[m]; i++)
			y[im + i] = shl(shr(lsptab_fx[jpm+i],8),8);
		}  
		else{ // odd index => lower entry
		 for (i = 0; i < nsub[m]; i++)
		  y[im+i]= extract_l(L_mult(lsptab_fx[jpm+i],0x80)); // l8,Q15
		}
		    
		im = add(im, nsub[m]);
		jp = add(jp, extract_l(L_shr(L_mult(nsub[m], nsiz[m]), 1)));
	}
}


/*=================================================================== */
/* FUNCTION      :  lspmaq_dec_fx8 ()                                 */
/*------------------------------------------------------------------- */
/* PURPOSE       :  This function returns the quantized LSP vector    */
/*                  given the LSPs codevector, using the table for
		    8-bit lsp quantization  (for 1/8th rate)          */
/*------------------------------------------------------------------- */
/* INPUT ARGUMENTS  :   
/*         _ (Word16*)   nsub : array containing the size of each 
/*                       split (number of columns in each sub-matrix) */                   
/*         _ (Word16*)   nsiz : array containing the number of
/*                       rows in each sub-matrix                      */
/*         _ (Word16 *)  index: LSPs codevector                       */
/*         _ (Word16*)   lsptab8_fx: Codebook containing 2 
/*                       sub-matrices of size (nsiz[i]*nsub[i], i=1,2)*/
/*------------------------------------------------------------------- */
/* OUTPUT ARGUMENTS :                                                 */
                               
/*           _ (Word16 *)  y: output quantized lsp vector             */ 
/*------------------------------------------------------------------- */
/* INPUT/OUTPUT ARGUMENTS :                                           */
/*                            _ None.                                 */
/*------------------------------------------------------------------- */
/* RETURN ARGUMENTS :                                                 */
/*                            _ None.                                 */
/*=================================================================== */

void lspmaq_dec_fx8(short *nsub, short *nsiz, short *y, short *index,short *lsptab_fx)
{
	register int i, m, im, jp, jpm;

	/* Code all sub-matrices */
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	im = 0;
	jp = 0;
	for (m = 0; m < 2; m++)
	{
		jpm = add(jp, extract_l(L_shr(L_mult(nsub[m], index[m]), 1)));
		for (i = 0; i < nsub[m]; i++)
			y[im + i] = lsptab_fx[jpm + i];
		im = add(im, nsub[m]);
		jp = add(jp, extract_l(L_shr(L_mult(nsub[m], nsiz[m]), 1)));
	}
}

