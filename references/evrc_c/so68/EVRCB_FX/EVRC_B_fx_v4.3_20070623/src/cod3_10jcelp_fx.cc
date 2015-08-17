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


#include "basic_op.h"
#include "basic_op40.h"
#include "typedef_fx.h"
#include "dsp_math.h"
#include "macro_fx.h"

#include "proto_fx.h"
#include "index_tbl.h"

#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif



/*===================================================================*/
/* FUNCTION      :  JCELP_Config_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Construct VCM Configuration                      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16) lag : pitch lag   (Q0)                               */
/*   _ (Word16) gain : pitch gain  (Q14)                             */
/*   _ (Word16) l_subfr : length of  subframe (Q0)                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16 []) pos : packed position table                       */ 
/*      packed 2 table entries (Q0) into a single 16 bit Word        */
/*                                                                   */
/*   _ (Word16*) offset : Q0                                         */
/*   _ (Word16*) sign   : Q0                                         */  
/*   _ (Word16*) ratio  : Q15                                        */ 
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */ 
/*                                                                   */
/*     _ (Word16) : Q0                                               */
/*===================================================================*/

Word16	JCELP_Config_fx(Word16 lag, Word16 gain, Word16	l_subfr, UNS_Word16 pos[768], Word16 *offset, Word16 *sign, Word16 *ratio)
 {	
      static Word16 skew_table1[31] = { 1, -1, -2, -3, 0, 0, 3, -2,
					0, -1, -3, -3, 1, 2, 0, -3,
		                        1,  0, -1,  0, 0, 0, 0,  0,
		                        0,  0,  0,  0, 0, 0, 0};

      
      static Word16 skew_table0[31] = { 0, -2, -3, -1, 1, 0, -2, -3,
	                               -1, -1,  2,  0, 2, 0,  3, -3,
		                        0,  3, -3,  0, 0, 0,  0,  0, 
		                        0,  0,  0,  0, 0, 0,  0};
       


      Word16	t0, t1, t2;
      Word16	l0, l1, l2;
      Word16	span, rem, iskew;
      Word16	skew0, skew1;
      Word16	space, inc, offs;
      Word16	i, return_code,j;
      Word16    q,qt=0;
      Word32    Ltemp;
      Word16    temp1,temp2,temp3,l0temp;
      

#ifdef WMOPS_FX
	  test();
	  logic16();
	  logic16();
#endif
      if( (lag <= VCM_UDU_PIT_FX) || (lag < VCM_UD_PIT_FX && gain <= PIT_THRSH_FX) ) {

#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
		if(gain > PIT_THRSH_FX && lag < l_subfr) {
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
			if (lag < 24)
				span = 24;
			else
				span = lag;
			return_code = 2;
		} else {
			span = l_subfr;
			return_code = 1;
		}

      /* fill position table */

		t0 = mult(span,10923);
		t1 = t0;
		t2 = t0;
		rem = sub(sub(sub(span,t0),t1), t2);
#ifdef WMOPS_FX
	  test();
#endif
		if (rem == 2) {
			t0++;
			t1++;
		} else if (rem == 1) {
			t0++;
		}

		space =  L_shr(L_mult(t0,extract_l(L_mult(t1,t2))),2);
	       
         	inc = space;   // Q9
		
		offs = space;  // Q10
		
		iskew = sub(54,span);
#ifdef WMOPS_FX
  move16();
  move16();
#endif
		skew0 = skew_table0[iskew];
		skew1 = skew_table1[iskew];

                		
#ifdef WMOPS_FX
  move16();
#endif
		j=0;
		
		for (i = 0; i < 512; i++) {
		          
		 l0temp=add(extract_l(L_mult(i,t0)),t0);
	         l0=shr(l0temp,10);

		 l1=extract_l(L_shr(L_mult(l0temp,t1),11));

#ifdef WMOPS_FX
	  test();
#endif
		 if (shr(extract_l(L_mult(add(qt,1),t1)),1)<=l1)
		    qt++;
	         l1=sub(l1,shr(extract_l(L_mult(qt,t1)),1));
			  
	          	   
#ifdef WMOPS_FX
  move32();
#endif
		 Ltemp=0;
                 Ltemp=L_add(L_mult(i,inc),offs);
	         l2=extract_l(L_shr((Word32) L_mac40(L_mac40(L_shr(Ltemp,9),l0,skew0),l1,skew1),1));
                 q=div_int_sp(l2,t2,l2);
                            
                 temp1 = shr(extract_l(L_mult(3,l0)),1);
		 temp2 = add(shr(extract_l(L_mult(3,l1)),1),1);
	         temp3 = add(shr(extract_l(L_mult(3,l2)),1),2);

                     		     	     
		/* packing two table entries into 1 16 bit word  */

#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
		   if (i & 1)
			 {
#ifdef WMOPS_FX
  logic16();
  logic16();
#endif
			   pos[j]=pos[j] | (temp1);
			   j++;
			   pos[j]=(temp2<<8)| (temp3);
			   j++;
                         }
                   else
		         {
#ifdef WMOPS_FX
  logic16();
#endif
			   pos[j] = (temp2) |  (temp1 << 8); 
                           j++;
                           pos[j]=(temp3<<8);
			 }

		}

		return (return_code);

   	} else {
#ifdef WMOPS_FX
	  test();
#endif
    		if(lag >= VCM_UD_PIT_FX) { 
			*sign = 1;
			Ltemp=L_mult(VCM_FAC_FX,sub(lag,VCM_UD_PIT_FX)); 
#ifdef WMOPS_FX
	  test();
	  move16();
#endif
    			if(Ltemp < 3277) *ratio = 3277;
			else if (Ltemp > 26214) *ratio = 26214;
			else *ratio=extract_l(Ltemp);
   			*offset = (lag < 100)? 1 : VCM_UuU_OVERSTEP_FX;
			
    		} else {
#ifdef WMOPS_FX
  move16();
  move16();
  move16();
#endif
    			*sign = -1;
			*ratio = 0; 
			*offset = 0;
    		}
		
		return(0);
    	}
 }	   	



/*===================================================================*/
/* FUNCTION      :  cod3_10jcelp_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Half-rate FCB search implementation              */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*                                                                   */
/*   _ (Word16*) x : target signal   (Q0)                            */
/*   _ (Word16*) h : impulse response of weighted                    */
/*                   synthesis filter (Q11)                          */
/*   _ (Word16)  pit_lag  : pitch lag  (Q0)                          */
/*   _ (Word16)  pit_gain : pitch gain (Q14)                         */
/*   _ (Word16)  l_subfr  : length of sub-frame (Q0)                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                                                                   */
/*   _ (Word16[]) cod : algebraic (fixed) codebook excitation (Q11)  */  
/*   _ (Word16*)  gain_code  :  gain in Q0                           */
/*   _ (Word16*)  indices    :  indices of 3 pulses (Q0)             */
/*                              (10 bits, 1 word)                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _None                                    */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :   _None                                        */ 
/*                                                                   */
/*===================================================================*/



void cod3_10jcelp_fx(
  Word16 xn[],
  Word16 h[],     
  Word16 pit_lag,
  Word16 pit_gain,
  Word16 l_subfr, 
  Word16 cod[],
  Word16 *gain_code, 
  Word16 *indices  
)
{
  	Word16 	i0, i1, i2;
	Word16 	i, j, k, pos,index,flag;
  	Word16 	codvec[3];
  	Word16 	psk=-1, ps1, ps2, sq2;
	Word16 	alp1, alp2;
	Word16 	s;
	Word16  r, sign; 
	Word16 dn[55];
	
        UNS_Word16 pos_table1[768];
	static	Word16 offset[3] = {0, 2, 4};
	

	Word16 ix,j1,pos1[3];
	Word16 sqk,alpk;
	Word40 ps;
	Word32 s1;
	
	Word16 rr[VCM_L0];
	Word16 ndn,nrr,q;


	/* Construct JCELP configuration */

	  
        flag=JCELP_Config_fx(pit_lag, pit_gain, l_subfr, pos_table1,&offset[1],&sign, &r);
                
       
	
#ifdef WMOPS_FX
	  test();
#endif
    	if(flag == 2)
		for (i = pit_lag; i < l_subfr; i++) h[i] += h[sub(i,pit_lag)];
   
	 
       	corr_xh_fx(xn, dn, h, l_subfr,&ndn,2);//  returns dn (block normalized) in Q(ndn-4)
        
        
		for(i=l_subfr; i< VCM_L0; i++) {
#ifdef WMOPS_FX
	move16();
#endif
			dn[i] = 0; /* Put dn[i] = 0 for i>l_subfr */
		}
	
 	/*----------------------------------------------------------------*
 	 *        BUILD "rr[][]", MATRIX OF AUTOCORRELATION.              *
	 *----------------------------------------------------------------*/
	//Doing regular diag elements
  	for (k=0, ps=0.0; k<l_subfr; k++) {
    		ps = L_mac40(ps,h[k],h[k]);
  	}

	nrr=sub(norm32_l40(ps),3);
	rr[0]= round32_16((Word32) L_shl40(ps,nrr)); // Q(23+nrr-16)
	                                             // note rr0 has the first row of matrix
	                                             // instead of diagonal in EVRC
	 //Make first row
  	for (i=1; i<l_subfr; i++) {
    		for (k=0, ps=0.0; k<sub(l_subfr,i); k++) {
      			ps =L_mac40(ps, h[k],h[k+i]);
    		}
      		rr[i] = round32_16((Word32) L_shl40(ps,nrr));//block normalized in Q(23+nrr-16)
		
  	}


        for(i=l_subfr; i< VCM_L0; i++) rr[i] = 0; /* Put rr[i] = 0 for i>l_subfr */
       
	/*----------------------------------------------------------------*
 	 *              SEARCH THE BEST CODEVECTOR.                       *
 	 *----------------------------------------------------------------*/
#ifdef WMOPS_FX
	Nmove16(4);
#endif
	sqk = -1;
	alpk = 1;
	ix = 0;
	j1=0;
#ifdef WMOPS_FX
	  test();
#endif
	if ( flag > 0 ) {	/* Search 3 pulses */
   		for (i=0; i<512; i++) {                      
#ifdef WMOPS_FX
	  test();
	  logic16();
#endif
		  if ((i&1)==0) //even
		    {
                      i0=shr(pos_table1[j1],8); //u8
#ifdef WMOPS_FX
	move16();
	logic16();
#endif
		      i1=(pos_table1[j1]& 0xFF); // l8
		      j1++;
                      i2=shr(pos_table1[j1],8); //u8
		     }
		  else
		    {
#ifdef WMOPS_FX
	move16();
	logic16();
#endif
                      i0=(pos_table1[j1]& 0xFF); // l8
		      j1++;
		      i1=shr(pos_table1[j1],8); // u8
#ifdef WMOPS_FX
	move16();
	logic16();
#endif
	              i2=(pos_table1[j1]& 0xFF); // l8
		      j1++;
		    }
                       
		  	ps1 = sub(dn[i0],dn[i1]);
			

	 	 	ps2 = add(ps1,dn[i2]);
	  		
			alp2=add(shl(sub(add(rr[0],rr[abs_s(sub(i0,i2))]),add(rr[abs_s(sub(i0,i1))],rr[abs_s(sub(i1,i2))])),1),rr[0]);
	 	 	sq2 = mult_r(ps2,ps2);
			


			s1 = L_msu(L_mult(alpk, sq2), sqk, alp2); //(alpk*sq2) > (sqk*alp2)
                   
                  
			                         
#ifdef WMOPS_FX
	  test();
#endif
	  		if (s1>0) {
#ifdef WMOPS_FX
	Nmove16(3);
#endif
	    			sqk = sq2;
	    			alpk = alp2;
	    			psk = ps2;
				ix = i;
				pos1[0]=i0;pos1[1]=i1;pos1[2]=i2;
				
				 
	  		}
   	   	}

		//printf("%d %d %d \n",pos1[0],pos1[1],pos1[2]);		      

		
	} else
	  {
	    /* Search 2 pulses */

                    
    		for (i=0; i<22; i++) {
#ifdef WMOPS_FX
	move16();
#endif
			i0 = index0_fx[i];
			i2 = add(i0,offset[1]);
			
			ps1  = round32_16(L_add(L_deposit_h(dn[i0]),L_mult(r,dn[i2]))); 
      			alp1 = add(rr[0],mult_r(r,add(mult_r(r,rr[0]),shl(rr[abs_s(sub(i0,i2))],1))));
			
			for (j=0; j<23; j++) {
#ifdef WMOPS_FX
	move16();
#endif
				i1 = index1_fx[j];
				ps2  = add(ps1,extract_l(L_shr(L_mult(sign,dn[i1]),1)));
				
                              alp2 = add(add(alp1,rr[0]),extract_l(L_mult(sign,round32_16(L_add(L_deposit_h(rr[abs_s(sub(i0,i1))]),L_mult(r,rr[abs_s(sub(i2,i1))]))))));
				sq2 = mult_r(ps2, ps2);
                                s1 = L_msu(L_mult(alpk, sq2), sqk, alp2); //(alpk*sq2) > (sqk*alp2))
                             	
				
#ifdef WMOPS_FX
	  test();
#endif
	  			if (s1 > 0) {
#ifdef WMOPS_FX
	Nmove16(3);
#endif
	    				sqk = sq2;
	    				alpk = alp2;
	    				psk = ps2;
	    				codvec[0] = i0;
	    				codvec[1] = i1;
	    				codvec[2] = i2;
					
					
	  			}
      			}
    		}

		//printf("%d %d %d \n",codvec[0],codvec[1],codvec[2]);  			
	}
		
 	/*--------------------------------------------------------------------*
 	 * BUILD THE CODEWORD, THE FILTERED CODEWORD AND INDEX OF CODEVECTOR. *
 	 *--------------------------------------------------------------------*/

   
	
	  for (i=0; i<VCM_L0; i++) {
#ifdef WMOPS_FX
	move16();
#endif
		  cod[i] = 0;
	  }

        *gain_code = round32_16((Word32) divide_dp(psk,alpk,sub(nrr,add(2,ndn))));
	

       
#ifdef WMOPS_FX
	  test();
	  Nlogic16(3);
#endif
  	if(((psk<0) && (alpk>0))||((psk>0) && (alpk<0))) {
  		*gain_code = negate(*gain_code);
  		//s = -8192; // 1 in Q13
#ifdef WMOPS_FX
	move16();
	move16();
#endif
                 s = -2048; // 1 in Q11
  		 index = 1;
  	} else {
  		//s = 8192;
#ifdef WMOPS_FX
	move16();
	move16();
#endif
                s=2048;// Q11
  		index = 0;
  	}

#ifdef WMOPS_FX
	  test();
#endif
	if (flag > 0) {
	  for(i=0; i<3; i++) {
#ifdef WMOPS_FX
	move16();
#endif
			pos = pos1[i];
			
    			cod[pos] =add(cod[pos],s);
			s = negate(s);
		}
		index=shl(index,9); //index <<= 9;
		index =add(index,ix);
		
           
	} else {
#ifdef WMOPS_FX
	move16();
#endif
	        pos = codvec[0];//Q0
		cod[pos] =add(cod[pos], s);
		cod[pos+offset[1]]=add(cod[pos+offset[1]], mult_r(s,r)); // Q(11+15+1-16=11)
		q=shr(extract_l(L_mult(pos,171)),11); // 171=(1/6) in Q10
		i0 = shr(sub(pos,q),1);
  
#ifdef WMOPS_FX
	move16();
#endif
		pos = codvec[1];
		cod[pos] =add(cod[pos],shr(extract_l(L_mult(s,sign)),1));
		q=shr(extract_l(L_mult(pos,171)),11); // 171=(1/6) in Q10
		i1 = shr(sub(pos,q),1);
    
		index = add(add(shl(index,9),shr(extract_l(L_mult(i0,23)),1)),i1);
            

  	}
	
#ifdef WMOPS_FX
	move16();
#endif
  	*indices = (int) index;

#ifdef WMOPS_FX
	  test();
#endif
    	if(flag == 2)
		for (i = pit_lag; i < l_subfr; i++) cod[i]= add(cod[i], cod[sub(i,pit_lag)]);

      //printf("%d\n",*indices);
	
	  
      	return;
	
}
