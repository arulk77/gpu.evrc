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
/*  WI related functions                                                */
/*======================================================================*/


#include "basic_op.h"
#include "proto_fx.h"
#include "basic_op40.h"
#include "dsp_math.h"
#include "macro_fx.h"
#include "cos_tab.dat"
#include "WI_tab.h"
#include "globs_fx.h"
#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif




/*===================================================================*/
/* FUNCTION      :  Mpy_32_16 ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Multiply 32 bit by 16 bit                        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word32)Lx :  in Q(Lx)                                        */
/*   _ (Word16)x  :  in Q(x)                                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word40)  :           Q(Lx)+Q(x)+1-16                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
Word40 Mpy_32_16(Word32 Lx, Word16 x)
{
  UNS_Word16 l;
  Word16 h;
  Word40 Lacc, Lsum;

#ifdef WMOPS_FX
  move32();
#endif
  Lacc=0;
  h=extract_h(Lx);
  l=extract_l(Lx);
  Lacc=L_mult_su(x, l);
  Lsum=L_add40(L_shr40(Lacc, 15), L_mult(x, h));

  return Lsum;
}

//The constructor for DTFS_fx class
DTFS_fx::DTFS_fx()
{
  int i ;
#ifdef WMOPS_FX
  move16();
#endif
  lag = 0 ;
  for (i=0;i<MAXLAG_WI;i++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    a[i]=b[i]=0 ;
  }
}

/*===================================================================*/
/* FUNCTION      :  operator= ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :    Operator '=' Overload                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X2: a/b in X2.Q                                     */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (DTFS_fx) : a/b in X2.Q, lag=X2.lag in Q0                     */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

DTFS_fx DTFS_fx::operator=(DTFS_fx X2)
{
  int i ;
  
  lag = X2.lag ;
  for (i=0; i<=lag>>1; i++) {
    a[i] = X2.a[i] ;
    b[i] = X2.b[i] ;
  }
  Q = X2.Q;
  return *this ;
} 

/*===================================================================*/
/* FUNCTION      :  operator+ ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1: a/b in X1.Q, lag in Q0                          */
/*   _ (DTFS_fx) X2: a/b in X2.Q = X1.Q, lag in Q0                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (DTFS_fx) : a/b in X1.Q=X2.Q,                                 */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

DTFS_fx DTFS_fx::operator+(DTFS_fx X2)
{
  DTFS_fx tmp ;
  Word16 i, HalfLag ;

  HalfLag = shr(lag, 1);
  for (i=0; i<=HalfLag; i++) {
    tmp.a[i] = a[i] ;
    tmp.b[i] = b[i] ;
  }
  HalfLag = shr(X2.lag, 1);
  for (i=0; i<=HalfLag; i++) {
    tmp.a[i] = add(tmp.a[i], X2.a[i]) ;
    tmp.b[i] = add(tmp.b[i], X2.b[i]) ;
  }
  tmp.lag = MAX(lag, X2.lag) ;
  tmp.Q = Q;
  return tmp ;
}

/*===================================================================*/
/* FUNCTION      :  operator- ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1: a/b in X1.Q, lag in Q0                          */
/*   _ (DTFS_fx) X2: a/b in X2.Q, lag in Q0                          */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (DTFS_fx) : a/b in MIN(X1.Q, X2.Q),                           */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

DTFS_fx DTFS_fx::operator-(DTFS_fx X2)
{
  DTFS_fx tmp ;
  Word16 i, diff, HalfLag ;

  diff = sub(MAX(Q, X2.Q), MIN(Q, X2.Q));
  HalfLag = shr(lag, 1);
#ifdef WMOPS_FX
  test();
#endif
  if(Q>=X2.Q) {
	for(i=0; i<=HalfLag; i++) {
		tmp.a[i] = sub(shr(a[i], diff), X2.a[i]) ;
		tmp.b[i] = sub(shr(b[i], diff), X2.b[i]) ;
	}
  }
  else {
	for(i=0; i<=HalfLag; i++) {
		tmp.a[i] = sub(a[i], shr(X2.a[i], diff)) ;
		tmp.b[i] = sub(b[i], shr(X2.b[i], diff)) ;
	}
  }
  tmp.lag = MAX(lag, X2.lag) ;
  tmp.Q=MIN(Q, X2.Q);

  return tmp ;
}

/*===================================================================*/
/* FUNCTION      :  operator* ()                                     */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1: a/b in X1.Q, lag in Q0                          */
/*   _ (Word16) factor: Q14                                          */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (DTFS_fx) X2: a/b in X1.Q, X2.lag = X1.lag                    */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

DTFS_fx DTFS_fx::operator*(Word16 factor)  // factor in Q14, <=1.0
{
  DTFS_fx tmp ;
  Word16 i, HalfLag ;

  HalfLag = shr(lag, 1);
  for (i=0; i<=HalfLag; i++) {
    tmp.a[i] = round32_16(L_shl(L_mult(a[i],factor),1)) ;
    tmp.b[i] = round32_16(L_shl(L_mult(b[i],factor),1)) ;
  }
  tmp.lag = lag ;
  tmp.Q=Q;
  return tmp ;
}

/*===================================================================*/
/* FUNCTION      :  zeroPadd_fx ()                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  zeroPadding                                      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) M:    length  , Q0                                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (Word16) lag: pitch lag of *this, Q0                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::zeroPadd_fx(Word16 M) 
{
  Word16 i, start, end ;

#ifdef WMOPS_FX
  test();
#endif
  if (M == lag) return ;
  start = add(shr(lag,1), 1);
  end = shr(M, 1);
  for (i=start; i<=end; i++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    a[i]=b[i]=0 ;
  }
#ifdef WMOPS_FX
  move16();
#endif
  lag = M ;
}

/*===================================================================*/
/* FUNCTION      :  zeroInsert_fx()(code not run into this function) */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  zero Inserting                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) I:    input length valule                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (Word16) lag: pitch lag of *this                              */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::zeroInsert_fx(Word16 I)
{
  Word32 i, j ;
  Word32 nH_fx;
  Word16 temp[MAXLAG_WI] ;

#ifdef WMOPS_FX
  test();
#endif
  if (sub(I, 1)<=0) return ;

  nH_fx = L_shr(L_mult(I, lag), 2);               // Q0
  
  for (i=0; i<=nH_fx; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  temp[i] = 0 ;
  }
  for (i=0, j=0; i<=nH_fx; i+=I, j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  temp[i] = a[j] ;
  }
  for (i=0; i<=nH_fx; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  a[i] = temp[i] ;
  }

  for (i=0; i<=nH_fx; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  temp[i] = 0 ;
  }

  for (i=0, j=0; i<=nH_fx; i+=I, j++) {
#ifdef WMOPS_FX
  move16();
#endif
	  temp[i] = b[j] ;
  }
  for (i=0; i<=nH_fx; i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  b[i] = temp[i] ;
  }

  lag = extract_l(L_shr(L_mult(lag, I), 1)) ;     // Q0
}

/*===================================================================*/
/* FUNCTION      :  transform_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1: a/b in X1.Q, lag in Q0                          */
/*   _ (DTFS_fx) X2. a/b in X2.Q, lag in Q0 (X1.Q==X2.Q)             */
/*   _ (Word16 *) phase_fx: 2pi normalized, Q15                      */
/*   _ (Word16) M:      length of series.                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 *) out_fx: output transformed series. Q(X1.Q).        */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::transform_fx(DTFS_fx X2, Word16 *phase_fx, Word16 *out_fx, Word16 M)
{
  DTFS_fx tmp1, tmp2 ;
  Word16 i, j ;
  Word32 L_temp;
  Word16 w_fx;

  Word16 inv1, inv2, inv, q1, q2, n;
  Word32 Lw;
  
  tmp1 = *this ;
  tmp2 = X2 ;

  Word16 x2_128[128], x1_128[128];
  Word16 k, m, l1;
  Word16 tmptmp1, tmptmp2;
  Word40 tmptmp1_40, tmptmp2_40;

  //obtain 128 point time waveforms of curr and prev
  tmp1.fast_fs_inv_fx(x1_128,128,7) ;
  tmp2.fast_fs_inv_fx(x2_128,128,7) ;

  L_temp=invert_dp(M,4,&n);			// 1/M, Q(61-n)
  inv1=round32_16(L_temp);			// 1/M in Q(45-n)
  q1=sub(n,15);
  L_temp=invert_dp(sub(lag,M),4,&n);// 1/(lag-M), Q(61-n)
  L_temp=(Word32) Mpy_32_16(L_temp,22904);	// L_temp=log(0.2)*log10(e)/(lag-M), Q(61-n)
  inv2=round32_16(L_temp);			// log(0.2)*log10(e)/(lag-M), Q(45-n)
  q2=sub(n,19);						// adjust Q factor to Q26 

#ifdef WMOPS_FX
  test();
#endif
  if (sub(sub(M, 20), lag) > 0 ) inv=inv2;
  else inv=inv1;
  Lw=L_deposit_l(inv);
  
  for (i=0; i<M; i++) {
#ifdef WMOPS_FX
    test();
#endif
    if ( sub(sub(M, 20), lag) > 0 ) {
      L_temp=(Word32) dsp_pow10(L_shl(Lw,q2)); // L_temp=exp(- (i+1) * log(.2)/(lag-M)), Q15
      w_fx=sub(16384,round32_16(L_shl(L_temp,15))); // w_fx in Q14
      
    }
    else {
      w_fx=round32_16(L_shl(Lw,q1));	// Q14
    }
    Lw=L_add(Lw,inv);					// (i+1)*inv
    j=mult_r(phase_fx[i],128*8);//mapping phase to 8x128 length signal
    k=j%8; l1=shr(j,3);	  		//reminder and quotient
    for (j=0,tmptmp1_40=tmptmp2_40=0;j<12;j++) {
	    m=(1000*128+l1-12/2+j)%128; //use circular addressing
	    tmptmp1_40=L_mac40(tmptmp1_40,x1_128[m],sinc_fx[k][j]);
	    tmptmp2_40=L_mac40(tmptmp2_40,x2_128[m],sinc_fx[k][j]);
    }
    tmptmp1=round32_16(L_shl((Word32) L_sat32_40(tmptmp1_40),1));
    tmptmp2=round32_16(L_shl((Word32) L_sat32_40(tmptmp2_40),1));
    out_fx[i]=round32_16(L_shl(L_mac(L_mult(tmptmp1,sub(16384,w_fx)),tmptmp2,w_fx),1));
  }

  return;
}

/*===================================================================*/
/* FUNCTION      :  to_fs_fx ()                                      */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 *) x:    input time domain series. Q0                 */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16) M:    Lag                                            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) (Word16 a[]) :  in Q                                */
/*   _ (DTFS_fx) (Word16 b[]) :  in Q                                */
/*   _ (DTFS_fx) (Word16 lag) :  Q0                                  */
/*   _ (DTFS_fx) (Word16 Q  ) :  Q value of a/b                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::to_fs_fx(Word16 *x, Word16 M, Word16 *S_fx, Word16 *C_fx)
{
  Word16 nH, k, n, temp, inv_lag, sum ;
  Word32 L_temp, Lx0;
  Word40 L_a, L_b;
  Word32 La[MAXLAG_WI], Lb[MAXLAG_WI], Labmax;

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  lag = M ;
  nH = shr(sub(M, 1), 1);
  *a = 0 ; *b = 0 ;

#ifdef WMOPS_FX
  move16();
#endif
  L_temp = invert_dp(M, 4, &temp);                   // 1/M in Q(61-temp)  
  inv_lag = round32_16(L_shl(L_temp,sub(temp,26)));  // Q(61-temp+temp-26-16)=Q(19)
  Lx0 = L_deposit_h(x[0]);
  Labmax=0;
  for ( k=1; k<=nH; k++ ) {
    L_a = (Word40)Lx0 ;                             // Q16
#ifdef WMOPS_FX
  move16();
#endif
    L_b = 0 ;
	sum = k;
	temp = k;
    for ( n=1; n<M; n++ ) {
      L_a = L_mac40(L_a, x[n], C_fx[(4*sum)%(4*M)]);  // Q16 of x[n]*cos(sum)
      L_b = L_mac40(L_b, x[n], S_fx[(4*sum)%(4*M)]);  // Q16 of x[n]*sin(sum)
	  sum +=  temp; //not using basic operator, this is a pointer autoincrement
    }
	La[k]=(Word32)L_sat32_40(L_shr40(L_a, 7));          // Q8 of a[k]*2.0
	Lb[k]=(Word32)L_sat32_40(L_shr40(L_b, 7));          // Q8 of b[k]*2.0
#ifdef WMOPS_FX
  test();
  test();
#endif
	L_temp=L_abs(La[k]);
	if(L_temp>Labmax) Labmax=L_temp;
	L_temp=L_abs(Lb[k]);
	if(L_temp>Labmax) Labmax=L_temp;
  }

  // The harmonic at 'pi'
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if ( M%2 == 0 ) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
    L_a = 0 ;
    temp = 1 ;
	for ( n=0; n<M; n++, temp*=-1 ) {
		L_a = L_mac40(L_a, x[n], temp);               // Q1
	}
	La[k]=(Word32)L_sat32_40(L_shl40(L_a, 7));          // Q8 of a[k]*1.0
#ifdef WMOPS_FX
  test();
#endif
	L_temp=L_abs(La[k]);                              
	if(L_temp>Labmax) Labmax=L_temp;
#ifdef WMOPS_FX
  move16();
#endif
    b[k] = 0 ;
  }

  temp = norm_l(Labmax);
  temp = (Labmax==0)?31:temp; 
  for ( k=1; k<=nH; k++ ) {
    a[k]=round32_16(L_shl(La[k], temp));     //Q(8+temp-16)=Q(temp-8)
	a[k]=round32_16(L_mult(a[k], inv_lag));  //Q(temp-8+19+1-16)=Q(temp-4) of a[k]*2.0/M
    b[k]=round32_16(L_shl(Lb[k], temp));     //Q(8+temp-16)=Q(temp-8)
	b[k]=round32_16(L_mult(b[k], inv_lag));  //Q(temp-8+19+1-16)=Q(temp-4) of b[k]*2.0/M
  }

#ifdef WMOPS_FX
  test();
  move16();
#endif
  if ( M%2 == 0 ) {
    a[k]=round32_16(L_shl(La[k], temp));     //Q(8+temp-16)=Q(temp-8)
	a[k]=round32_16(L_mult(a[k], inv_lag));  //Q(temp-8+19+1-16)=Q(temp-4) of a[k]*1.0/M
	b[k]=0;
  }

  Q=sub(temp, 4);
}

/*===================================================================*/
/* FUNCTION      :  phaseShift_fx ()                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Phase is shifted by 2pi*ph/Lag                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) ph:    shift index, Q7                               */
/*   _ (Word16) Lag:   Pitch Lag value as for shift                  */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::phaseShift_fx(Word16 ph, Word16 Lag, Word16 *S_fx, Word16 *C_fx)
{
  Word16 k ;
  Word16 temp, HalfLag;
  Word32 L_temp, L_temp2=0 ;

  HalfLag = shr(lag, 1);
#ifdef WMOPS_FX
    test();
#endif
  if(ph>0) {
	for (k=0; k<=HalfLag; k++) {
	   temp = a[k];
	   L_temp = L_shr(L_temp2, 5);   // Q2
       a[k] = round32_16(L_sub(L_mult(temp, C_fx[L_temp%(4*Lag)]), L_mult(b[k], S_fx[L_temp%(4*Lag)]))) ; // X.Q
       b[k] = round32_16(L_add(L_mult(b[k], C_fx[L_temp%(4*Lag)]), L_mult(temp, S_fx[L_temp%(4*Lag)]))) ;
	   L_temp2 = L_add(L_temp2, ph);
	}
  }
#ifdef WMOPS_FX
    test();
#endif
  if (ph<0) {
	for (k=0; k<=HalfLag; k++) {
	   temp = a[k];
	   L_temp = L_shr(L_negate(L_temp2), 5);   // Q2
       a[k] = round32_16(L_add(L_mult(temp, C_fx[L_temp%(4*Lag)]), L_mult(b[k], S_fx[L_temp%(4*Lag)]))) ;   // X.Q
       b[k] = round32_16(L_sub(L_mult(b[k], C_fx[L_temp%(4*Lag)]), L_mult(temp, S_fx[L_temp%(4*Lag)]))) ;
	   L_temp2 = L_add(L_temp2, ph);
	}
  }
}

/*===================================================================*/
/* FUNCTION      :  Q2phaseShift_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Phase is shifted by 2pi*ph/Lag                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) ph:    shift index, Q2                               */
/*   _ (Word16) Lag:   Pitch Lag value as for shift                  */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::Q2phaseShift_fx(Word16 ph, Word16 Lag, Word16 *S_fx, Word16 *C_fx)
{
  Word16 k ;
  Word16 temp, temp2=0, HalfLag;

  HalfLag = shr(lag, 1);
#ifdef WMOPS_FX
    test();
#endif
  
  if(ph>0) {
	for (k=0; k<=HalfLag; k++) {
	   temp = a[k];
       a[k] = round32_16(L_sub(L_mult(temp, C_fx[temp2%(4*Lag)]), L_mult(b[k], S_fx[temp2%(4*Lag)]))) ; // X.Q
       b[k] = round32_16(L_add(L_mult(b[k], C_fx[temp2%(4*Lag)]), L_mult(temp, S_fx[temp2%(4*Lag)]))) ;
	   temp2 = add(temp2, ph);
	}
  }
#ifdef WMOPS_FX
    test();
#endif
  if (ph<0) {
	for (k=0; k<=HalfLag; k++) {
	   temp = a[k];
       a[k] = round32_16(L_add(L_mult(temp, C_fx[temp2%(4*Lag)]), L_mult(b[k], S_fx[temp2%(4*Lag)]))) ;   // X.Q
       b[k] = round32_16(L_sub(L_mult(b[k], C_fx[temp2%(4*Lag)]), L_mult(temp, S_fx[temp2%(4*Lag)]))) ;
	   temp2 = add(temp2, negate(ph));
	}
  }
}

/*===================================================================*/
/* FUNCTION      :  phaseShift_band_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Phase is shifted by 2pi*ph/Lag                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) ph:    shift index   Q2                              */
/*   _ (Word16) lag:   Pitch Lag value as for shift                  */
/*   _ (Word16) lband: lower band      Q0                            */
/*   _ (Word16) hband: higher band     Q0                            */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::phaseShift_band_fx(Word16 ph, Word16 lband, Word16 hband, Word16 *S_fx, Word16 *C_fx)
{
  Word16 k ;
  Word16 temp, HalfLag;
  Word32 freq_fx, L_lband, L_hband, X, L_temp2=0L;

  L_lband = L_mult(lband, lag);
  L_hband = L_mult(hband, lag);
  HalfLag = shr(lag, 1);
  /* get lband and hband */
  for (k=0; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_lband)>=0) break;
  }
  lband = k;
  for (k=lband; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_hband)>=0) break;
  }
  hband = k;

#ifdef WMOPS_FX
      test();
#endif
  if(ph>0) {
	for (k=lband; k<hband; k++) {
		L_temp2=L_shr(L_mult(k, ph), 1);
		temp = a[k];
		X = L_temp2;
		a[k] = round32_16(L_sub(L_mult(temp, C_fx[X%(4*lag)]), L_mult(b[k], S_fx[X%(4*lag)]))) ;
		b[k] = round32_16(L_add(L_mult(b[k], C_fx[X%(4*lag)]), L_mult(temp, S_fx[X%(4*lag)]))) ;
	}
  }
#ifdef WMOPS_FX
      test();
#endif
  if(ph<0) {
	for (k=lband; k<hband; k++) {
		L_temp2=L_shr(L_mult(k, ph), 1);
		temp = a[k];
		X = L_negate(L_temp2);
		a[k] = round32_16(L_add(L_mult(temp, C_fx[X%(4*lag)]), L_mult(b[k], S_fx[X%(4*lag)]))) ;
		b[k] = round32_16(L_sub(L_mult(b[k], C_fx[X%(4*lag)]), L_mult(temp, S_fx[X%(4*lag)]))) ;
	}
  }
}

/*===================================================================*/
/* FUNCTION      :  alignment_full_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  search for alignment                             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) fshift_fx :           Q1                             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/

Word16 DTFS_fx::alignment_full_fx(DTFS_fx X2, Word16 ph_offset_fx,Word16 *S_fx, Word16 *C_fx)
{
  Word16 temp, temp1, k, start, end, HalfLag, ab1[MAXLAG_WI], ab2[MAXLAG_WI] ;
  Word16 n, fshift_fx;
  Word40 corr_fx, maxcorr_fx;
  DTFS_fx X1 ;
  Word16 Eshift, Adiff_fx;

  X1 = *this ;

  	//Calculating the expected alignment shift
  	Eshift = mult_r(ph_offset_fx, shl(X2.lag, 7));      //confirmed I<2 by smv12.org, Q7
	div_int_sp((short)FSIZE, shr(add(X2.lag, X1.lag), 1), temp);
	temp = add(shl(temp, 7), Eshift);                            // Q7
#ifdef WMOPS_FX
    test();
#endif
	if(temp<0) temp = add(temp, shl(X1.lag, 7));                // Q7
	div_int_sp(temp, shl(X1.lag, 7), Eshift);                   // Q7
	Eshift=shl(shr(Eshift,7),1); //Q1 but integer
#ifdef WMOPS_FX
  test();
#endif
  if (X2.lag > 60) {
	Adiff_fx = shl(X2.lag,1-3); //lag/8 in Q1
  }
  else {
	  Adiff_fx = shl(X2.lag,1-2); //lag/4 in Q1
  }

#ifdef WMOPS_FX
  test();
#endif
  if (lag < X2.lag)
    X1.zeroPadd_fx(X2.lag) ;

#ifdef WMOPS_FX
  move32();
#endif
  maxcorr_fx = MIN_40;
  HalfLag = shr(X2.lag, 1);

  for(k=0; k<=HalfLag; k++) {
	  ab1[k] = round32_16(L_add(L_mult(X1.a[k], X2.a[k]), L_mult(X1.b[k], X2.b[k])));   // Q(-15);
	  ab2[k] = round32_16(L_sub(L_mult(X1.b[k], X2.a[k]), L_mult(X1.a[k], X2.b[k])));   // Q(-15);
  }
  start=sub(Eshift, Adiff_fx);
  end=add(Eshift, Adiff_fx);
  for (fshift_fx=n=start; n<=end; n++) {   // Q1
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    corr_fx = 0 ;
	temp = 0;
	temp1 = add(n,shl(X2.lag,1)); //add lag in Q1to make positive
    for (k=0; k<=HalfLag; k++) {
      corr_fx = L_mac40(corr_fx, ab1[k], C_fx[(2*temp)%(4*X2.lag)]);
      corr_fx = L_mac40(corr_fx, ab2[k], S_fx[(2*temp)%(4*X2.lag)]);
 	  temp = add(temp, temp1);
    }
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub40_40(corr_fx, maxcorr_fx) > 0) {
#ifdef WMOPS_FX
  move16();
  move32();
#endif
      fshift_fx = n ;   // Q1
      maxcorr_fx = corr_fx ;
    }
  }

  return fshift_fx ;
}

/*===================================================================*/
/* FUNCTION      :    alignment_fine_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  search for alignment                             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) fshift_fx :           Q2                             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/

Word16 DTFS_fx::alignment_fine_fx(DTFS_fx X2, Word16 *S_fx, Word16 *C_fx)
{
  Word16 temp, temp1, k, Qcorr, Qmaxcorr;
  Word16 n, fshift_fx, HalfLag, ab1[MAXLAG_WI], ab2[MAXLAG_WI];
  Word40 corr_fx;
  Word32 maxcorr_fx, wcorr_fx, diff_corr;
  DTFS_fx X1 ;

  X1 = *this ;
#ifdef WMOPS_FX
  test();
#endif
  if (lag < X2.lag)
    X1.zeroPadd_fx(X2.lag) ;

#ifdef WMOPS_FX
  move32();
  move32();
#endif
  maxcorr_fx = -0x80000000L;
  Qmaxcorr = 0;
  HalfLag = shr(X2.lag, 1);

  for (k=0; k<=HalfLag; k++) {
	  ab1[k] = round32_16(L_add(L_mult(X1.a[k],X2.a[k]),L_mult(X1.b[k],X2.b[k])));
	  ab2[k] = round32_16(L_sub(L_mult(X1.b[k],X2.a[k]),L_mult(X1.a[k],X2.b[k])));
  }

  for (n=-16; n<=16; n++ ) {   // n is Q2
#ifdef WMOPS_FX
  move16();
  move16();
  move16();
#endif
    corr_fx = 0 ;
	temp = 0;
	temp1 = n;
#ifdef WMOPS_FX
    test();
#endif
	if(n<0) temp1=add(temp1, shl(X2.lag, 2));           // avoid negative
    for (k=0; k<=HalfLag; k++) {
      corr_fx=L_mac40(corr_fx, ab1[k], C_fx[temp%(4*X2.lag)]);
      corr_fx=L_mac40(corr_fx, ab2[k], S_fx[temp%(4*X2.lag)]);
 	  temp = add(temp, temp1);
    }
	temp=sub(32767, extract_l(L_shr(L_mult(82, abs_s(n)), 1)));    // Q15
	Qcorr = norm32_l40(corr_fx);
	Qcorr = (corr_fx==0)?31:Qcorr; 
	temp1 = round32_16((Word32)L_shl40(corr_fx, Qcorr));        // Q(Qcorr-16)
	wcorr_fx = L_mult(temp1, temp);                     // Q(Qcorr-16+15+1)=Q(Qcorr)
#ifdef WMOPS_FX
    test();
#endif
	if(Qmaxcorr >= Qcorr) 
		diff_corr = L_sub(wcorr_fx, L_shl(maxcorr_fx, sub(Qcorr, Qmaxcorr)));    // Qcorr
	else diff_corr = L_sub(L_shl(wcorr_fx, sub(Qmaxcorr, Qcorr)), maxcorr_fx);   // Qmaxcorr

#ifdef WMOPS_FX
    test();
#endif
    if ( diff_corr > 0 ) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      fshift_fx = n ;
      maxcorr_fx = (Word32)L_shl40(corr_fx, Qcorr) ;                // Qcorr
	  Qmaxcorr = Qcorr;
    }
  }

  return fshift_fx ;
}

/*===================================================================*/
/* FUNCTION      :  alignment_weight_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  search for alignment                             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16) Eshift: Q7
/*   _ (Word16 *) LPC1:  lpc coefficients. Q12                       */
/*   _ (Word16 *) LPC2:  lpc coefficients. Q12                       */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) cos_table: 512-entry cos( ) table   Q15            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) fshift_fx :           Q7                             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

Word16 DTFS_fx::alignment_weight_fx(DTFS_fx X2, Word16 Eshift, Word16 *LPC1, Word16 *LPC2, Word16 *S_fx, Word16 *C_fx, Word16 *cos_table, Word16 *pf_temp1,Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2)
{  //Eshift is w.r.t  X2 
  Word16 k, HalfLag, start, end, ab1[MAXLAG_WI], ab2[MAXLAG_WI] ;
  Word40 corr_fx;
  Word32 maxcorr_fx, wcorr_fx, diff_corr;
  DTFS_fx X1 ;
  Word16 temp, temp1, Qcorr, Qmaxcorr, inv_lag, n_fx, diff_fx, fshift_fx, Adiff_fx;
  Word16 tmplpc_fx[ORDER];
  Word32 L_temp;

  X1 = *this ;
  X1.adjustLag_fx(X2.lag) ;

  X1.poleFilter_fx(LPC1,ORDER, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
  for (k=0;k<ORDER;k++) {
	  tmplpc_fx[k]=round32_16(L_mult(LPC1[k], pwf_fx[k]));           // Q12
  }
  X1.zeroFilter_fx(tmplpc_fx,ORDER, S_fx, C_fx) ;

  X2.poleFilter_fx(LPC1,ORDER, S_fx, C_fx, pf_temp1, pf_temp2, pf_temp, pf_n2);
  for (k=0;k<ORDER;k++) {
	  tmplpc_fx[k]=round32_16(L_mult(LPC2[k], pwf_fx[k]));           // Q12
  }
  X2.zeroFilter_fx(tmplpc_fx,ORDER, S_fx, C_fx) ; 

#ifdef WMOPS_FX
  move32();
  move16();
  move16();
#endif
  maxcorr_fx = -0x80000000L;
  Qmaxcorr = 0;
  fshift_fx = Eshift ;
  Adiff_fx = (Word16)MAX_FX(768, mult_r(4915, shl(X2.lag, 7)));    // Q7, 768=6*128, 4915 = 0.15*32768
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (X2.lag < 60) diff_fx = 32;                                   // Q7 of 0.25
  else diff_fx = 64;                                               // Q7 of 0.5
 
  HalfLag = shr(X2.lag, 1);
  L_temp = invert_dp(X2.lag, 4, &temp);              // 1/X2.lag in Q(61-temp)  
  inv_lag = round32_16(L_shl(L_temp,sub(temp,26)));

  for(k=0; k<=HalfLag; k++) {
	  ab1[k] = round32_16(L_add(L_mult(X1.a[k], X2.a[k]), L_mult(X1.b[k], X2.b[k]))); // Q(-15)
	  ab2[k] = round32_16(L_sub(L_mult(X1.a[k], X2.b[k]), L_mult(X1.b[k], X2.a[k]))); // Q(-15)
  }

  start=sub(Eshift, Adiff_fx);
  end=add(Eshift, Adiff_fx);
  for (n_fx=start; n_fx<=end; n_fx=add(n_fx, diff_fx)) {   // Q7
#ifdef WMOPS_FX
  move32();
  move32();
#endif
	corr_fx = 0;
	temp = 0;
	temp1 = round32_16(L_shr(L_mult(inv_lag, n_fx), 2));       // Q(19+7+1-2-16)=Q9 of n/X2.lag

	if(temp1<0) temp1=add(temp1, 0x200);                       // avoid negative, 0x200 is Q9 of 1 ( 2*pi )
    for (k=0; k<=HalfLag; k++) {
	  corr_fx = L_mac40(corr_fx, ab1[k], cos_table[temp%512]);          // Q(-15+15+1)=Q1
	  corr_fx = L_mac40(corr_fx, ab2[k], cos_table[(temp+128)%512]);    // Q(-15+15+1)=Q1
	  temp = add(temp, temp1);  
    }
	temp = sub(8192, mult_r(20972, abs_s(sub(n_fx, Eshift))));  // Q13, 20972 = Q21 of 0.01.
	Qcorr = norm32_l40(corr_fx);
	Qcorr = (corr_fx==0)?31:Qcorr; 
	temp1 = round32_16((Word32)L_shl40(corr_fx, Qcorr));        // Q(Qcorr-16)
	wcorr_fx = L_mult(temp1, shl(temp, 2));                     // Q(Qcorr-16+13+2+1)=Q(Qcorr)
#ifdef WMOPS_FX
    test();
	move16();
#endif
	if(Qmaxcorr >= Qcorr) 
		diff_corr = L_sub(wcorr_fx, L_shl(maxcorr_fx, sub(Qcorr, Qmaxcorr)));    // Qcorr
	else diff_corr = L_sub(L_shl(wcorr_fx, sub(Qmaxcorr, Qcorr)), maxcorr_fx);   // Qmaxcorr
#ifdef WMOPS_FX
    test();
#endif
    if ( diff_corr > 0 ) {
#ifdef WMOPS_FX
  move16();
  move32();
  move16();
#endif
      fshift_fx = n_fx ;
      maxcorr_fx = wcorr_fx ;
	  Qmaxcorr = Qcorr;
    }
  }

  return fshift_fx ;
}

/*===================================================================*/
/* FUNCTION      :  alignment_extract_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  search for alignment                             */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16 *) LPC2:  lpc coefficients. Q12                       */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) fshift_fx :           Q2                             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

Word16 DTFS_fx::alignment_extract_fx(DTFS_fx X2, Word16 *LPC2, Word16 *S_fx, Word16 *C_fx)
{  
  Word16 k, HalfLag, ab1[MAXLAG_WI], ab2[MAXLAG_WI] ;
  Word16 Adiff_fx, diff_fx, temp, temp1, fshift_fx, n_fx, Qcorr, Qmaxcorr;
  Word32 maxcorr_fx, wcorr_fx, diff_corr, L_temp, L_temp1;
  Word40 corr_fx;
  DTFS_fx X1 ;
  
  X1 = *this ;
  X1.adjustLag_fx(X2.lag) ;

#ifdef WMOPS_FX
  move32();
  move16();
  move16();
#endif
  maxcorr_fx = - 0x80000000L;
  Qmaxcorr = 0;
  fshift_fx = 0 ;

  Adiff_fx = (Word16)(MAX_FX(4, shr(lag, 3)));    
  Adiff_fx = shl(Adiff_fx, 2);                    // Q2
  diff_fx = 4;                                    // Q2
  
  HalfLag = shr(X2.lag, 1);

  for (k=0; k<=HalfLag; k++) {
	  ab1[k] = round32_16(L_add(L_mult(X1.a[k], X2.a[k]), L_mult(X1.b[k], X2.b[k]))); // Q(-15)
	  ab2[k] = round32_16(L_sub(L_mult(X1.a[k], X2.b[k]), L_mult(X1.b[k], X2.a[k]))); // Q(-15)
  }

  for (n_fx=negate(Adiff_fx); n_fx<=Adiff_fx; n_fx=add(n_fx, diff_fx)) {              //Q2
#ifdef WMOPS_FX
  move32();
  move32();
#endif
	corr_fx = 0;
	L_temp = 0;
	temp1 = add(n_fx, shl(lag, 2));                                  // avoid negative
	L_temp1 = L_deposit_l(temp1);                                    
    for (k=0; k<=HalfLag; k++) {
	  corr_fx = L_mac40(corr_fx, ab1[k], C_fx[L_temp%(4*lag)]);      // Q(-15+15+1)=Q1
	  corr_fx = L_mac40(corr_fx, ab2[k], S_fx[L_temp%(4*lag)]);      // Q(-15+15+1)=Q1
	  L_temp = L_temp+L_temp1;     // index auto-increment, don't need basic operator.
    }
	temp = sub(2048, mult_r(20972, abs_s(n_fx)));               // Q11, 20972 = (0.01*2048/32)*32768
	Qcorr = norm32_l40(corr_fx);
	Qcorr = (corr_fx==0)?31:Qcorr; 
	temp1 = round32_16((Word32)L_shl40(corr_fx, Qcorr));        // Q(Qcorr-16)
	wcorr_fx = L_mult(temp1, shl(temp, 4));                     // Q(Qcorr-16+11+4+1)=Q(Qcorr)
#ifdef WMOPS_FX
    test();
#endif
	if(Qmaxcorr >= Qcorr) 
		diff_corr = L_sub(wcorr_fx, L_shl(maxcorr_fx, sub(Qcorr, Qmaxcorr)));    // Qcorr
	else diff_corr = L_sub(L_shl(wcorr_fx, sub(Qmaxcorr, Qcorr)), maxcorr_fx);   // Qmaxcorr

#ifdef WMOPS_FX
    test();
#endif
    if ( diff_corr > 0 ) {
      fshift_fx = n_fx ;
      maxcorr_fx = (Word32)L_shl40(corr_fx, Qcorr) ;                // Qcorr
	  Qmaxcorr = Qcorr;
    }
  }

  return fshift_fx ;     // Q2
}

Word16 alignment_extract_td_fx(Word16 *x1, Word16 *x2, Word16 lag)
{
  Word16 j, k, idx, Adiff_fx ;
  Word40 maxcorr, corr;

#ifdef WMOPS_FX
  move32();
#endif
//  maxcorr = LW_MIN;
    maxcorr=MIN_40; 
  Adiff_fx = (Word16)(MAX_FX(4, shr(lag, 3)));    
  for (j=negate(Adiff_fx); j<=Adiff_fx; j++) {
    for (k=0,corr=0; k<lag; k++) {
      corr=L_mac40(corr,x1[k],x2[(k-j+lag)%lag]);
    }
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub40_40(corr,maxcorr)>0) {
#ifdef WMOPS_FX
  move16();
  move32();
#endif
      idx = j ;
      maxcorr = corr ;
    } 
  }
  return idx;
}


/*===================================================================*/
/* FUNCTION      :  alignment_band_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16) lband:  lower band, Q0                               */
/*   _ (Word16) hband:  higher band, Q0                              */
/*   _ (Word16) diff:   Q1                                           */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS : _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) fshift :           Q0                                */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/

Word16 DTFS_fx::alignment_band_fx(DTFS_fx X2, Word16 lband, Word16 hband, Word16 diff, Word16 *S_fx, Word16 *C_fx)
{ 
  Word16 k, flag=0, idx, fshift, HalfLag ;
  Word40 corr_fx, maxcorr_fx;
  Word32 freq_fx, L_temp, L_lband, L_hband;
  Word16 ab1[MAXLAG_WI], ab2[MAXLAG_WI];
  Word16 temp1, n_fx;
  DTFS_fx X1 ;
  
  X1 = *this ;
#ifdef WMOPS_FX
  move32();
#endif
  maxcorr_fx = MIN_40 ;

  L_lband = L_mult(lband, lag);
  L_hband = L_mult(hband, lag);
  HalfLag = shr(X2.lag, 1);
  /* get lband and hband */
  for (k=0; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
    test();
#endif
	if(L_sub(freq_fx, L_lband)>=0) break;
  }
  lband = k;
  for (k=lband; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
    test();
#endif
	if(L_sub(freq_fx, L_hband)>=0) break;
  }
  hband = k;

  if(hband>lband) flag=1;          // set flag here to save WMOPS

  for(k=0; k<=HalfLag; k++) {
	ab1[k] = round32_16(L_add(L_mult(X1.a[k],X2.a[k]), L_mult(X1.b[k],X2.b[k]))); // Q(1-16)=Q(-15)
	ab2[k] = round32_16(L_sub(L_mult(X1.b[k],X2.a[k]), L_mult(X1.a[k],X2.b[k])));
  }
  for (idx=0, n_fx=-32; n_fx<32; n_fx=add(n_fx, diff), idx++) {   // Q1 for n
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    corr_fx = 0;
	temp1 = n_fx;         // for TWO_PI*n_fx/(2*X2.lag)
#ifdef WMOPS_FX
    test();
#endif
	if(temp1<0) temp1=add(temp1, shl(X2.lag, 2));                 // avoid negative
    for (k=lband; k<hband; k++) {
	  L_temp = L_mult(k, temp1);                                               // Q2
	  corr_fx = L_mac40(corr_fx, ab1[k], C_fx[L_temp%(4*X2.lag)]);             // Q(-15+15+1)=Q1
 	  corr_fx = L_mac40(corr_fx, ab2[k], S_fx[L_temp%(4*X2.lag)]);             // Q1
    }
#ifdef WMOPS_FX
    test();
	logic16();
#endif
    if ((flag==1)&&(L_sub40_40(corr_fx, maxcorr_fx)>0)) {
      fshift = idx;
      maxcorr_fx = corr_fx ;
    }
  }
#ifdef WMOPS_FX
  test();
#endif
  if (flag == 0)
    return -1;
  else
    return fshift ;
}

/*===================================================================*/
/* FUNCTION      :  freq_corr_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q                                   */
/*   _ (DTFS_fx) X2 :  a/b in X2.Q                                   */
/*   _ (Word16) lband:  Q0                                           */
/*   _ (Word16) hband:  Q0                                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32) Result :       Qout                                  */
/*   _ (Word16) *Qout :        Q of output result                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/

Word32 DTFS_fx::freq_corr_fx(DTFS_fx X2, Word16 lband, Word16 hband, Word16 *Qout)
{
  Word16 k, temp, HalfLag, lk, hk ;
  Word40 corr_fx;
  Word32 freq_fx, L_lband, L_hband;
  DTFS_fx X1 ;
  Word40 E_fx;
  Word32 Num, Den, Result;
  Word16 E1_fx, E2_fx, Q1, Q2, Qr;

  X1 = *this ;
#ifdef WMOPS_FX
  test();
#endif
  if (lag < X2.lag)
    X1.zeroPadd_fx(X2.lag) ;

#ifdef WMOPS_FX
  move32();
#endif
  corr_fx = 0;

  L_lband = L_mult(lband, X2.lag);
  L_hband = L_mult(hband, X2.lag);
  HalfLag = shr(X2.lag, 1);
  /* get lband and hband */
  for (k=0; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_lband)>=0) break;
  }
  lk = k;
  for (k=0; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_hband)>=0) break;
  }
  hk = k;

  for (k=lk; k<hk; k++) {
      corr_fx = L_mac40(corr_fx, X1.a[k], X2.a[k]);       // Q(1)
	  corr_fx = L_mac40(corr_fx, X1.b[k], X2.b[k]);       // Q(1)
  }

  Qr = norm32_l40(corr_fx);              
  Qr = (corr_fx==0)?31:Qr; 
  E1_fx = round32_16((Word32)L_shl40(corr_fx, Qr));      // Q(1+Qr-16)
  Num = L_mult(E1_fx, E1_fx);                            // Q(2+2*Qr-32+1)

  E_fx = X1.getEngy_fx(lband, hband);                    // Q(1)
  Q1 = norm32_l40(E_fx);              
  Q1 = (E_fx==0)?31:Q1; 
  E1_fx = round32_16((Word32)L_shl40(E_fx, Q1));         // Q(1+Q1-16)

  E_fx = X2.getEngy_fx(lband, hband);                    // Q(1)
  Q2 = norm32_l40(E_fx);
  Q2 = (E_fx==0)?31:Q2; 
  E2_fx = round32_16((Word32)L_shl40(E_fx, Q2));         // Q(1+Q2-16)

  Den = L_mult(E1_fx, E2_fx);                            // Q(2+Q1+Q2-32+1)

#ifdef WMOPS_FX
  test();
#endif
  if(corr_fx>0) Result = sqrt_divide_dp(Num, Den, sub(sub(shl(Qr, 1), Q1), Q2), &temp) ;
  else Result = L_negate(sqrt_divide_dp(Num, Den, sub(sub(shl(Qr, 1), Q1), Q2), &temp)); 
#ifdef WMOPS_FX
  move16();
#endif
  *Qout = temp;

  return Result;
}

/*===================================================================*/
/* FUNCTION      :  adjustLag_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) M:  lag value, Q0                                    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) :    a/b in X1.Q                                    */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  lag in Q0                                     */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::adjustLag_fx(Word16 M)
{
  Word16 HalfLag, HalfM;
  
#ifdef WMOPS_FX
  test();
#endif
  if (M==lag) return ;
#ifdef WMOPS_FX
  test();
#endif
  if (M>lag)
    this -> zeroPadd_fx(M) ;
  else {
    int k ;
	Word40 en_fx;
    HalfLag = shr(lag, 1);
    HalfM = add(shr(M, 1), 1);
    en_fx = this -> getEngy_fx() ;
    for (k=HalfM; k<=HalfLag; k++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      a[k] = 0 ;
      b[k] = 0 ;
    }
    this -> setEngy_fx(en_fx) ;
#ifdef WMOPS_FX
  move16();
#endif
    lag = M ;
  }
}

/*===================================================================*/
/* FUNCTION      :  getEngy_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) :    a/b in X1.Q, lag in Q0                         */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS : _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS : _ None                                   */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word40) en_fx: output energy, 2*X1.Q+1                       */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

Word40 DTFS_fx::getEngy_fx()
{
  Word16 k, HalfLag;
#ifdef WMOPS_FX
  move32();
#endif
  Word40 en_fx=0;

  HalfLag = shr(sub(lag, 1), 1);
  for (k=1; k<=HalfLag; k++) {
    en_fx = L_mac40(en_fx, a[k], a[k]);                 // 2*X1.Q+1
    en_fx = L_mac40(en_fx, b[k], b[k]);
  }

  en_fx = L_shr40(en_fx, 1);        
  en_fx = L_mac40(en_fx, a[0], a[0]);

#ifdef WMOPS_FX
  test();
#endif
  if (lag%2 == 0) {
    en_fx = L_mac40(en_fx, a[k], a[k]);
    en_fx = L_mac40(en_fx, b[k], b[k]);
  }

  return en_fx ;                                      // 2*X1.Q+1
}

/*===================================================================*/
/* FUNCTION      :  getEngy_fx (Word16 lband, Word16 hband)          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  compute the energy of X1.a[k] and X2.b[k]        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q, lag in Q0                        */
/*   _ (Word16) lband:  Q0                                           */
/*   _ (Word16) hband:  Q0                                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word40) en_fx :       2*X1.Q+1                               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/

Word40 DTFS_fx::getEngy_fx(Word16 lband, Word16 hband)
{
  Word16 k, lk, hk, HalfLag ;
  Word32 freq_fx, L_lband, L_hband;
  Word40 en_fx=0;

  L_lband = L_mult(lband, lag);
  L_hband = L_mult(hband, lag);
  HalfLag = shr(sub(lag, 1), 1);
  /* get lband and hband */
  for (k=1; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_lband)>=0) break;
  }
  lk = k;
  for (k=1; k<=HalfLag; k++) {
    freq_fx = L_mult(k, 8000);
#ifdef WMOPS_FX
	test();
#endif
	if(L_sub(freq_fx, L_hband)>=0) break;
  }
  hk = k;

  for (k=lk; k<hk; k++) {
	en_fx = L_mac40(en_fx, a[k], a[k]);          // 2*X1.Q+1
	en_fx = L_mac40(en_fx, b[k], b[k]);
  }
  en_fx = L_shr40(en_fx, 1);                         // 2*X1.Q+1

#ifdef WMOPS_FX
  test();
#endif
  if (lband == 0)
    en_fx = L_mac40(en_fx, a[0], a[0]) ;             // 2*X1.Q+1

#ifdef WMOPS_FX
  test();
  logic16();
#endif
  if ((lag%2 == 0) && (hband == 4000)) {
    en_fx = L_mac40(en_fx, a[k], a[k]);
	en_fx = L_mac40(en_fx, b[k], b[k]);
  }

  return en_fx ;                                     // 2*X1.Q+1
}

/*===================================================================*/
/* FUNCTION      :  setEngy_fx ( )                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word40) en2_fx:        2*X1.Q+1                              */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word40) en1_fx :       2*X1.Q+1                              */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X1 :  a/b in X1.Q, lag in Q0                        */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

Word40 DTFS_fx::setEngy_fx(Word40 en2_fx)
{
  Word16 k, temp, factor, HalfLag ;
  Word40 en1_fx;
  Word32 L_temp;

  HalfLag = shr(lag, 1);
  en1_fx = this -> getEngy_fx();          // 2*X1.Q+1
#ifdef WMOPS_FX
  test();
#endif
  if (en1_fx ==  0) return 0;

  L_temp = sqrt_divide_dp(en2_fx, en1_fx, 0, &temp);
  factor = extract_l(L_shr(L_temp, sub(temp, 14))); // Q14

  for (k=0; k<=HalfLag; k++) {
    a[k] = round32_16(L_shl(L_mult(factor, a[k]), 1));  // X1.Q, tmp==sqrt(en2/en1)==1.00xxx
    b[k] = round32_16(L_shl(L_mult(factor, b[k]), 1));  // X1.Q
  }

  return en1_fx ;                        // 2*X1.Q+1
}

/*===================================================================*/
/* FUNCTION      :  setEngyHarm_fx ( )                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Set a band of harmonics to specified energy      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) : lag in Q0                                         */
/*   _ (Word16) f1:   lower bound of input, normalized by 8000, Q15  */
/*   _ (Word16) f2:   upper bound of input, normalized by 8000, Q15  */
/*   _ (Word16) g1:   lower bound of output, normalized by 8000, Q15 */
/*   _ (Word16) g2:   upper bound of output, normalized by 8000, Q15 */
/*   _ (Word32) en2:  in Q(Qen2)                                     */
/*   _ (Word16) Qen2: Q value of en2                                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) Qa: Q value of output a[].                           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) : a[] in X1.Q at start, then changed to *Qa later.  */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*   _ (Word32) en1_fx:  Q(2*X1.Q)                                   */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: This function cannot change Q because it works on a band    */
/*       of harmonics, instead of the whole DTFS                     */
/*===================================================================*/
Word32 DTFS_fx::setEngyHarm_fx(Word16 f1, Word16 f2, Word16 g1, Word16 g2, Word32 en2, Word16 Qen2, Word16 *Qa)
{
  Word16 k, temp=31, count=0, HalfLag;
  Word16 f_low, f_high, g_low, g_high;
  Word32 L_temp, factor_fx;
  Word32 en1_fx;
  Word40 Lacc;

  f_low=mult(f1,lag); // Q0
  f_high=mult(f2,lag); // Q0
  g_low=mult(g1,lag); // Q0
  g_high=mult(g2,lag); // Q0
  HalfLag = shr(sub(lag, 1), 1);
  for (k=f_low+1, Lacc=0; k<=f_high; k++) {
      Lacc = L_mac40(Lacc, a[k], a[k]);         // 2*X1.Q+1
      count=add(count,1);
  }
  en1_fx =(Word32) divide_dp(Lacc, count, -30);           // 2*X1.Q
#ifdef WMOPS_FX
  test();
#endif
  if (en1_fx>0) 
    factor_fx = sqrt_divide_dp((Word40)en2, en1_fx, sub(Qen2, shl(Q, 1)), &temp);      // Q(temp)
  else 
    factor_fx = 0;

  for (k=g_low+1; k<=g_high; k++) {
      L_temp =(Word32)Mpy_32_16(factor_fx, a[k]);            // Q(temp+X1.Q-15)
      a[k] = round32_16(L_temp);  // Q(temp+X1.Q-15-16)=Q(temp+X1.Q-31);
  }

  *Qa = sub(add(temp, Q), 31);

  return en1_fx;       // Q(2*X1.Q)
}

/*===================================================================*/
/* FUNCTION      :  zeroFilter_fx()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  zero filtering                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lpc[] :  lpc coefficients in Q12                     */
/*   _ (Word16) N     :  lpc order                                   */
/*   _ (Word16) this->lag:  in Q0                                    */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None.                                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) this->a[] :    in Q(this->Q)                         */
/*   _ (Word16) this->b[] :    in Q(this->Q)                         */
/*   _ (Word16) this->Q:       in Q0                                 */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::zeroFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx)
{
  Word40 sum1_fx, sum2_fx ;
  Word16 k, n, HalfLag ;
  Word16 temp, temp1, temp2;
  Word32 L_temp1, L_temp2;
  Word16 Qmin, Qab[61], na, nb;

#ifdef WMOPS_FX
  move16();
#endif
  Qmin = 32767;
  HalfLag = shr(lag, 1);
  for ( k=0 ; k<=HalfLag ; k++ ) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	 temp = temp2 = k;
     /* Calculate sum1 and sum2 */
     sum1_fx = L_deposit_h(4096);      // 1: Q(12+15+1) 
#ifdef WMOPS_FX
  move32();
#endif
	 sum2_fx = 0;
     for ( n=0 ; n<N ; n++, temp2 = add(temp2, temp) ) {
       sum1_fx = L_msu40(sum1_fx, LPC[n], C_fx[(4*temp2)%(4*lag)]) ;   //Q(12+15+1)
       sum2_fx = L_msu40(sum2_fx, LPC[n], S_fx[(4*temp2)%(4*lag)]) ;
     }

	 temp1 = round32_16((Word32)L_sat32_40(sum1_fx));   //Q(12+15+1-16)=Q(12)
	 temp2 = round32_16((Word32)L_sat32_40(sum2_fx));   //              Q(12)

     /* Calculate the circular convolution */
	 L_temp1 = L_mult(temp1, a[k]);
	 L_temp1 = L_msu(L_temp1, temp2, b[k]); // Q(12+Q+1)
	 L_temp2 = L_mult(temp1, b[k]);
	 L_temp2 = L_mac(L_temp2, temp2, a[k]); // Q(12+Q+1)

	 // normalization
	 na = norm_l(L_temp1);
	 na = (L_temp1==0)?31:na; 
	 nb = norm_l(L_temp2);
	 nb = (L_temp2==0)?31:nb; 
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(na, nb)<0) nb=na;
	 a[k] = round32_16((Word32)L_shl(L_temp1, nb));  //Q(13+Q+nb-16)=Q(Q+nb-3)
	 b[k] = round32_16((Word32)L_shl(L_temp2, nb));  //              Q(Q+nb-3)

	 Qab[k] = sub(nb, 3);
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(Qab[k], Qmin)<0) Qmin = Qab[k];
  }
  //bring to the same Q
  for ( k=0 ; k<=HalfLag ; k++ ) {
	  a[k] = shl(a[k], sub(Qmin, Qab[k]));   // Q(Q+Qab[k]+Qmin-Qab[k]=Q(Q+Qmin)
	  b[k] = shl(b[k], sub(Qmin, Qab[k]));   //                        Q(Q+Qmin)
  }

  Q = add(Q, Qmin);
}

/*===================================================================*/
/* FUNCTION      :  poleFilter_fx()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  pole filtering                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lpc[] :  lpc coefficients in Q12                     */
/*   _ (Word16) N     :  lpc order                                   */
/*   _ (Word16) this->lag:  in Q0                                    */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) this->a[] :    in Q(this->Q)                         */
/*   _ (Word16) this->b[] :    in Q(this->Q)                         */
/*   _ (Word16) this->Q:       in Q0                                 */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::poleFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx)
{ 
  Word16 temp, temp1, temp2, HalfLag ;
  Word40 sum1_fx, sum2_fx;
  Word32 sum, L_temp1, L_temp2;
  Word16 k, n, na, nb;
  Word16 Qmin, Qab[61];

#ifdef WMOPS_FX
  move16();
#endif
  Qmin = 32767;
  HalfLag = shr(lag, 1);
  for ( k=0; k<=HalfLag; k++ ) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	 temp = temp2 = k;
     /* Calculate sum1 and sum2 */
     sum1_fx = L_deposit_h(4096);      // 1: Q(12+15+1) 
#ifdef WMOPS_FX
  move32();
#endif
	 sum2_fx = 0;
     for ( n=0 ; n<N ; n++, temp2 = add(temp2, temp) ) {
       sum1_fx = L_msu40(sum1_fx, LPC[n], C_fx[(4*temp2)%(4*lag)]) ;   //Q(12+15+1)
       sum2_fx = L_msu40(sum2_fx, LPC[n], S_fx[(4*temp2)%(4*lag)]) ;   //Q(12+15+1)
     }

	 temp1 = round32_16((Word32)L_sat32_40(sum1_fx));   //Q(12+15+1-16)=Q(12)
	 temp2 = round32_16((Word32)L_sat32_40(sum2_fx));   //              Q(12)

	 /* Calculate the circular convolution */
	 sum = L_add(L_mult(temp1, temp1), L_mult(temp2, temp2)); // Q(12+12+1)=Q(25)

	 L_temp1 = L_mult(temp1, a[k]);
	 L_temp1 = L_mac(L_temp1, temp2, b[k]); // Q(12+Q+1)=Q(13+Q)
	 L_temp2 = L_mult(temp1, b[k]);
     L_temp2 = L_msu(L_temp2, temp2, a[k]); // Q(12+Q+1)=Q(13+Q)

	 sum = invert_dp(sum, 4, &temp1);
	 temp = round32_16(sum);                  // Q(61-25-temp1-16)=Q(20-temp1)
	 sum1_fx = Mpy_32_16(L_temp1, temp);      // Q(13+Q+20-temp1-15)=Q(Q-temp1+18)
     sum2_fx = Mpy_32_16(L_temp2, temp);      //                     Q(Q-temp1+18)
	 // normalization
	 na = norm32_l40(sum1_fx);
	 na = (sum1_fx==0)?31:na; 
	 nb = norm32_l40(sum2_fx);
	 nb = (sum2_fx==0)?31:nb; 
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(na, nb)<0) nb=na;
	 nb=sub(nb,1); // leave one more sign bit
	 a[k] = round32_16((Word32)L_shl40(sum1_fx, nb));  //Q(Qtemp1+18+nb-16)=Q(Q-temp1+nb+2)
	 b[k] = round32_16((Word32)L_shl40(sum2_fx, nb));  //                   Q(Q-temp1+nb+2)

	 Qab[k] = sub(add(nb, 2), temp1);
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(Qab[k], Qmin)<0) Qmin = Qab[k];
  }
  // bring to the same Q
  for ( k=0; k<=HalfLag; k++ ) {
	  a[k] = shl(a[k], sub(Qmin, Qab[k]));          // Q(Q+Qab[k]+Qmin-Qab[k])=Q(Q+Qmin)
	  b[k] = shl(b[k], sub(Qmin, Qab[k]));          // Q(Q+Qab[k]+Qmin-Qab[k])=Q(Q+Qmin)
  }

  Q = add(Q, Qmin);
}

/*===================================================================*/
/* FUNCTION      :  poleFilter_setup_fx()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Sets up pole filtering LPC dependent intermediate*/
/*		    values to be used by poleFilter 		     */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lpc[] :  lpc coefficients in Q12                     */
/*   _ (Word16) N     :  lpc order                                   */
/*   _ (Word16) lag      :  in Q0                                    */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void poleFilter_setup_fx(Word16 *LPC, Word16 N, Word16 lag, Word16 *S_fx, Word16 *C_fx, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2_temp1)
{ 
  Word16 temp, temp1, temp2, HalfLag ;
  Word40 sum1_fx, sum2_fx;
  Word32 sum;
  Word16 k, n, n1, n2;

  HalfLag = shr(lag, 1);
  for ( k=0; k<=HalfLag; k++ ) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	 temp = temp2 = k;
     /* Calculate sum1 and sum2 */
     sum1_fx = L_deposit_h(4096);      // 1: Q(12+15+1) 
#ifdef WMOPS_FX
  move32();
#endif
	 sum2_fx = 0;
     for ( n=0 ; n<N ; n++, temp2 = add(temp2, temp) ) {
       sum1_fx = L_msu40(sum1_fx, LPC[n], C_fx[(4*temp2)%(4*lag)]) ;   //Q(12+15+1)
       sum2_fx = L_msu40(sum2_fx, LPC[n], S_fx[(4*temp2)%(4*lag)]) ;   //Q(12+15+1)
     }

	 n1 = norm32_l40(sum1_fx);
	 n1 = (sum1_fx==0)?31:n1; 
	 n2 = norm32_l40(sum2_fx);
	 n2 = (sum2_fx==0)?31:n2; 
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(n1, n2)<0) n2=n1;
	 n2 = sub(n2, 1); 
	 temp1 = pf_temp1[k] = round32_16((Word32)L_shl40(sum1_fx, n2));   //Q(12+15+1+n2-16)=Q(12+n2)
	 temp2 = pf_temp2[k] = round32_16((Word32)L_shl40(sum2_fx, n2));   //                 Q(12+n2)

	 /* Calculate the circular convolution */
	 sum = L_add(L_mult(temp1, temp1), L_mult(temp2, temp2)); // Q(12+n2+12+n2+1)=Q(25+2*n2)
	 

	 sum = invert_dp(sum, 4, &temp1);
	 pf_n2_temp1[k] = add(n2, temp1); 
	 pf_temp[k] = round32_16(sum);                  // Q(61-25-2*n2-temp1-16)=Q(20-2*n2-temp1)
  }
}

/*===================================================================*/
/* FUNCTION      :  poleFilter_fx()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  pole filtering                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lpc[] :  lpc coefficients in Q12                     */
/*   _ (Word16) N     :  lpc order                                   */
/*   _ (Word16) lag:  in Q0                                          */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) S_fx: sin(2pi*n/(4*lag)) table, Q15                */
/*   _ (Word16 *) C_fx: cos(2pi*n/(4*lag)) table, Q15                */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ None                                                          */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16) this->a[] :    in Q(this->Q)                         */
/*   _ (Word16) this->b[] :    in Q(this->Q)                         */
/*   _ (Word16) this->Q:       in Q0                                 */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/

void DTFS_fx::poleFilter_fx(Word16 *LPC, Word16 N, Word16 *S_fx, Word16 *C_fx, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2_temp1)
{ 
  Word16 temp, temp1, temp2, HalfLag ;
  Word40 sum1_fx, sum2_fx;
  Word32 L_temp1, L_temp2;
  Word16 k,n2_temp1, na, nb;
  Word16 Qmin, Qab[MAXLAG_WI];

#ifdef WMOPS_FX
  move16();
#endif
  Qmin = 32767;
  HalfLag = shr(lag, 1);
  for ( k=0; k<=HalfLag; k++ ) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	 temp = temp2 = k;
     /* Calculate sum1 and sum2 */

	 n2_temp1 = pf_n2_temp1[k]; 
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	 temp1 = pf_temp1[k];   //Q(12+15+1+n2-16)=Q(12+n2)
	 temp2 = pf_temp2[k];   //                 Q(12+n2)

	 /* Calculate the circular convolution */
	 
	 L_temp1 = L_add(L_mult(temp1, a[k]), L_mult(temp2, b[k])); // Q(12+n2+Q+1)=Q(13+n2+Q)
     L_temp2 = L_sub(L_mult(temp1, b[k]), L_mult(temp2, a[k])); // Q(12+n2+Q+1)=Q(13+n2+Q)

#ifdef WMOPS_FX
  move16();
#endif
	 temp = pf_temp[k];                  // Q(61-25-2*n2-temp1-16)=Q(20-2*n2-temp1)

	 sum1_fx = Mpy_32_16(L_temp1, temp);      // Q(13+n2+Q+20-2*n2-temp1-15)=Q(Q-n2-temp1+18)
     sum2_fx = Mpy_32_16(L_temp2, temp);      //                             Q(Q-n2-temp1+18)
	 // normalization
	 na = norm32_l40(sum1_fx);
	 na = (sum1_fx==0)?31:na; 
	 nb = norm32_l40(sum2_fx);
	 nb = (sum2_fx==0)?31:nb; 
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(na, nb)<0) nb=na;
	 nb=sub(nb,1); // leave one more sign bit
	 a[k] = round32_16((Word32)L_shl40(sum1_fx, nb));  //Q(Q-n2-temp1+18+nb-16)=Q(Q-n2-temp1+nb+2)
	 b[k] = round32_16((Word32)L_shl40(sum2_fx, nb));  //                       Q(Q-n2-temp1+nb+2)

	 Qab[k] = sub(add(nb, 2), n2_temp1);
#ifdef WMOPS_FX
     test();
	 move16();
#endif
	 if(sub(Qab[k], Qmin)<0) Qmin = Qab[k];
  }
  // bring to the same Q
  for ( k=0; k<=HalfLag; k++ ) {
	  a[k] = shl(a[k], sub(Qmin, Qab[k]));          // Q(Q+Qab[k]+Qmin-Qab[k])=Q(Q+Qmin)
	  b[k] = shl(b[k], sub(Qmin, Qab[k]));          // Q(Q+Qab[k]+Qmin-Qab[k])=Q(Q+Qmin)
  }

  Q = add(Q, Qmin);
}

/*===================================================================*/
/* FUNCTION      :  peaktoaverage_fx ()                              */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1: a/b in Q(X1.Q), lag in Q0                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word32 *) pos_fx :  in *Qpos                                 */
/*   _ (Word32 *) neg_fx :  in *Qneg                                 */
/*   _ (Word16 *) Qpos: Q valule of output *pos_fx                   */
/*   _ (Word16 *) Qneg: Q valule of output *neg_fx                   */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void DTFS_fx::peaktoaverage_fx(Word32 *pos_fx, Word16 *Qpos, Word32 *neg_fx, Word16 *Qneg)
{
  Word40 L_sum40;
  Word32 maxPosEn_fx=0, maxNegEn_fx=0, L_temp;
  Word16 i, n;
  DTFS_fx TMP = *this ;
  Word16 time_fx[128];

  L_sum40=TMP.getEngy_fx(); // 2Q+1
  TMP.fast_fs_inv_fx(time_fx , 128, 7);

  for (i=0;i<128;i++) {
	L_temp = L_mult(time_fx[i], time_fx[i]);         // Q(1)
#ifdef WMOPS_FX
    test();
#endif
    if (time_fx[i] >= 0) {
#ifdef WMOPS_FX
      test();
	  move32();
#endif
	  if (L_temp>maxPosEn_fx) maxPosEn_fx = L_temp ; // Q(1)
    }
    else {
#ifdef WMOPS_FX
      test();
	  move32();
#endif
      if (L_temp>maxNegEn_fx) maxNegEn_fx = L_temp ; // Q(1)
    }
  }
#ifdef WMOPS_FX
  test();
  move16();
  move16();
#endif
  if (L_sum40==0)
    *pos_fx=*neg_fx=0;
  else {
    n=negate(shl(TMP.Q,1)); //-2Q
    *pos_fx= sqrt_divide_dp(maxPosEn_fx, L_sum40, n, Qpos);    
    
    *neg_fx= sqrt_divide_dp(maxNegEn_fx, L_sum40, n, Qneg);    
  }
}

/*===================================================================*/
/* FUNCTION      :  WIsyn_fx ()                                      */
/*-------------------------------------------------------------------*/
/* PURPOSE       :                                                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) prevCW_fx: a/b in prevCW_fx.Q                       */
/*   _ (DTFS_fx *) curr_cw_fx: a/b in curr_cw_fx->Q                  */
/*   _ (Word16 *) prev_lsp_fx: lpc coefficients in Q12               */
/*   _ (Word16 *) curr_lsp_fx: lpc coefficients in Q12               */
/*   _ (Word16 *) ph_offset_fx: in Q15, normalized by 2pi            */
/*   _ (Word16 *) N:     length, Q0                                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 *) out_fx: Q0                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx *) curr_cw_fx: a/b in curr_cw_fx->Q                  */
/*   _ (Word16 *) ph_offset_fx: in Q15, normalized by 2pi            */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/

void WIsyn_fx(DTFS_fx prevCW_fx, DTFS_fx *curr_cw_fx, Word16 *prev_lsp_fx, Word16 *curr_lsp_fx, Word16 *ph_offset_fx, Word16 *out_fx, Word16 N, Word16 *pf_temp1, Word16 *pf_temp2, Word16 *pf_temp, Word16 *pf_n2)
{
#define SCR_PITCH_THRESHLD 60
  Word16 i;
  unsigned short I=1, flag=0;
#ifdef VOIP_DORA_VOICE
  Word16 *phase_fx = new Word16[2*N];
#else
  Word16 *phase_fx = new Word16[N];
#endif

  Word16 alignment_fx;
  Word16 tmp_fx, temp;
  Word32 L_temp;
  
#ifdef WMOPS_FX
  extern int WIcallcount;
  char tmpstr[100];
#endif

#ifdef WMOPS_FX
  test();
#endif
  if(prevCW_fx.Q<curr_cw_fx->Q) {
	  temp = sub(curr_cw_fx->Q, prevCW_fx.Q);
	  for(i=0;i<=shr(curr_cw_fx->lag, 1);i++) {
		curr_cw_fx->a[i] = shr(curr_cw_fx->a[i], temp);
		curr_cw_fx->b[i] = shr(curr_cw_fx->b[i], temp);
	  }
	  curr_cw_fx->Q=prevCW_fx.Q;
  }
#ifdef WMOPS_FX
  test();
#endif
  if(curr_cw_fx->Q<prevCW_fx.Q) {
	  temp = sub(prevCW_fx.Q, curr_cw_fx->Q);
	  for(i=0;i<=shr(prevCW_fx.lag, 1);i++) {
		prevCW_fx.a[i] = shr(prevCW_fx.a[i], temp);
		prevCW_fx.b[i] = shr(prevCW_fx.b[i], temp);
	  }
	  prevCW_fx.Q=curr_cw_fx->Q;
  }
  
  DTFS_fx currCW_fx = *curr_cw_fx ;

  //Calculating the expected alignment shift
  alignment_fx = mult_r(*ph_offset_fx, shl(prevCW_fx.lag, 7));      //confirmed I<2 by smv12.org, Q7
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (flag==1) alignment_fx = extract_l(L_shr(L_mult(alignment_fx, I), 1)) ;   //Q7

  //Calculating the expected alignment shift
  div_int_sp((short)N, shr(add(prevCW_fx.lag, currCW_fx.lag), 1), temp);
  temp = add(shl(temp, 7), alignment_fx);                            // Q7
#ifdef WMOPS_FX
  test();
#endif
  if(temp<0) temp = add(temp, shl(currCW_fx.lag, 7));                // Q7
  div_int_sp(temp, shl(currCW_fx.lag, 7), tmp_fx);                   // Q7

#ifdef WMOPS_FX
      sprintf(tmpstr,"WIsyn until adjust lag, Eshift compute %d .................",WIcallcount);
	  WMP_fwc_fx(tmpstr);
#endif
  // Compute the alignment shift
  GetSinCosTab_fx(currCW_fx.lag, S_fx, C_fx);


  alignment_fx = prevCW_fx.alignment_weight_fx(currCW_fx, tmp_fx, curr_lsp_fx, curr_lsp_fx, S_fx, C_fx, cos_table, pf_temp1, pf_temp2, pf_temp, pf_n2);//Q7
  
  

#ifdef WMOPS_FX
      sprintf(tmpstr,"WIsyn alignment_weight %d..................................",WIcallcount);
      WMP_fwc_fx(tmpstr);
#endif
#ifdef WMOPS_FX
  test();
#endif



  if(alignment_fx>=shl(currCW_fx.lag, 7)) {
	  temp=sub(alignment_fx, shl(currCW_fx.lag, 7));
	  tmp_fx = round32_16((Word32) L_sat32_40(divide_dp(temp, shl(currCW_fx.lag, 7), 2)));   // Q15
  }
  else if(alignment_fx<0) {
	  temp=negate(alignment_fx);
	  tmp_fx = negate(round32_16((Word32) L_sat32_40(divide_dp(temp, shl(currCW_fx.lag, 7), 2)))); // Q15
  }
  else {
	  temp=alignment_fx;
	  tmp_fx = round32_16((Word32) L_sat32_40(divide_dp(temp, shl(currCW_fx.lag, 7), 2))); // Q15
  }

  currCW_fx.phaseShift_fx(alignment_fx, currCW_fx.lag, S_fx, C_fx) ;   // Qmin
  curr_cw_fx->phaseShift_fx(alignment_fx, curr_cw_fx->lag, S_fx, C_fx);// Qmin

#ifdef WMOPS_FX
      sprintf(tmpstr,"WIsyn phaseShift after align_weight %d.....................",WIcallcount);
      WMP_fwc_fx(tmpstr);
#endif
  // Compute the cubic phase track and transform to 1-D signal
  cubicPhase_fx(*ph_offset_fx, tmp_fx, prevCW_fx.lag , currCW_fx.lag, N, phase_fx) ;


#ifdef WMOPS_FX
      sprintf(tmpstr,"WIsyn cubicPhase %d........................................",WIcallcount);
      WMP_fwc_fx(tmpstr);
#endif
  temp = shr(add(prevCW_fx.lag, currCW_fx.lag), 1);                // Q0
  prevCW_fx.transform_fx(currCW_fx, phase_fx, out_fx, (Word16)N) ;

#ifdef WMOPS_FX
      sprintf(tmpstr,"WIsyn transform call %d....................................",WIcallcount);
      WMP_fwc_fx(tmpstr);
#endif
  // Adjust the phase offset and wrap it between 0 and 2pi
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (flag==2) L_temp = L_shr(L_mult(tmp_fx, I), 1);   // Q15
  else L_temp = L_deposit_l(tmp_fx);                   // Q15
#ifdef WMOPS_FX
  test();
#endif
  while(L_temp<0) L_temp = L_add(L_temp, 0x8000L);
  L_temp = L_temp&0x7fff;                              // fraction part
  *ph_offset_fx = extract_l(L_temp);


  delete [] phase_fx ;

}


/*===================================================================*/
/* FUNCTION      :  ppp_extract_pitch_period_fx ()                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Update background noise estimate, signal energy  */
/*                  estimate, and band snrs                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) in :  residual input, Q0                          */
/*   _ (Word16) l :  pitch lag, Q0                                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) out :  pitch period prototype, Q0                 */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*===================================================================*/
void ppp_extract_pitch_period_fx(Word16 *in, Word16 *out, Word16 l)
{
  Word16 i;
  Word16 spike;
  Word16 max, x;
  Word16 range;
  const Word16 *ptr=in+FSIZE-l;
  Word32 tmp;
  Word40 Lacc, en1;
  Word16 n, scale;

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  spike=0;
  max=0;
  for (i=0, en1=0;i<l;i++) {
    x=abs_s(ptr[i]);
#ifdef WMOPS_FX
    test();
	move16();
	move16();
#endif
    if (sub(x,max)>0) {max=x;spike=i;}
    en1 =L_mac40(en1, ptr[i], ptr[i]); // Q1
  }
  
  range=mult_r(l,8192); // range=round(l*0.25)
  range=MAX_FX(range,6);

#ifdef WMOPS_FX
  test();
#endif
  if (sub(spike,range)<0) {
    for (i=0;i<add(l,sub(spike,range));i++) out[i]=ptr[i];
    // Grab Remaining From One Lag Before
    ptr-=l;
	for (;i<l;i++) {
#ifdef WMOPS_FX
  move16();
#endif
		out[i]=ptr[i];
	}
  }
  else if (sub(add(spike,range),l)>=0) {
    for (i=0;i<sub(spike,range);i++) out[i]=ptr[i];
    // Grab Remaining From One Lag Before
#ifdef WMOPS_FX
    test();
#endif
	if (ptr-l+i>=in) {
		for (ptr-=l;i<l;i++) {
#ifdef WMOPS_FX
  move16();
#endif
		out[i]=ptr[i];
		}
	}
	else {
		for (;i<l;i++) {
#ifdef WMOPS_FX
  move16();
#endif
			out[i]=ptr[i];
		}
	}
  }
  else {
	  for (i=0;i<l;i++) {
#ifdef WMOPS_FX
  move16();
#endif
		  out[i]=ptr[i];
	  }
  }

  //Energy adjustment added to eliminate artifacts at the end of voicing
  for (i=0, Lacc=0;i<l;i++) Lacc = L_mac40(Lacc,out[i],out[i]); // Q1
#ifdef WMOPS_FX
  test();
#endif
  if (L_sub40_40(en1,Lacc)<0) {
    tmp=sqrt_divide_dp(en1, Lacc, 0, &n);
    scale=round32_16(L_shl(tmp,sub(31,n))); // Q15
    for (i=0;i<l;i++) out[i] = mult_r(out[i],scale);
  }
  
}

/*===================================================================*/
/* FUNCTION      :  cubicPhase_fx ()                                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute cubic phase track for WI synthesis       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) ph1 :  initial phase, Q15 (normalized by 2pi)        */
/*   _ (Word16) ph2 :  final phase, Q15 (normalized by 2pi)          */
/*   _ (Word16) L1 :  previous pitch lag, Q0                         */
/*   _ (Word16) L2 :  current pitch lag, Q0                          */
/*   _ (Word16) N :  length of phase track, Q0                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) phOut :  phase track, Q15 (normalized by 2pi)     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: This function outputs phase in (-1,1)                       */
/*===================================================================*/
void cubicPhase_fx(Word16 ph1, Word16 ph2, Word16 L1, Word16 L2,
		   Word16 N, Word16 *phOut)
{
#define OVERFLOW_FIX
  Word16 n;
  Word16 n2, n3l, n3h;
  Word16 f1, f2;  // invert of L1, L2, Q19
  Word16 factor, temp,Q_Ltemp2=0;
  Word16 c0, c1, c2, c3; // cubic polynomial coefficients
                         // Q33, Q27, Q19, Q15 respectively
  Word32 Ltemp1, Ltemp2, Ltemp3,n2_32;
  Word40 Lacc;

#ifdef OVERFLOW_FIX
  Word16 templ, temph;
#endif

  N=sub(N,L2);

  Ltemp1=invert_dp(L1, 4, &n); // Ltemp1=1/L1 in Q(61-n)
  f1=round32_16(L_shl(Ltemp1,sub(n,26))); // f1=1/L1, Q19
  Ltemp1=invert_dp(L2, 4, &n); // Ltemp1=1/L2 in Q(61-n)
  f2=round32_16(L_shl(Ltemp1,sub(n,26))); // f2=1/L2, Q19
  
  c3=ph1; // c3 in Q15
  c2=f1;  // c2 in Q19

  Ltemp1=L_sub(ph1,ph2); // Ltemp1=ph1-ph2, Q15
  Ltemp2=L_add(f1,f2); // Ltemp2=0.5*(f1+f2), Q20
  temp=extract_l(Ltemp2);
  
  Ltemp2=(Word32)L_mult_su(N,temp); // Ltemp2=0.5*N*(f1+f2), Q20
  Ltemp1=L_add(L_shl(Ltemp1,5),Ltemp2); // Ltemp1=ph1 - ph2 + 0.5*N*(f2+f1), Q16, Q20

  factor=round32_16(L_shr(Ltemp1,4));  // factor in Q0

  c1=sub(f2,f1);  // c1=f2-f1, Q19
  Ltemp1=L_sub(ph2,ph1); // Q15
  Ltemp2=L_mult(N,f1); // Ltemp2=N*f1, Q20
  Ltemp2=L_sub(L_shl(L_deposit_h(factor),4),Ltemp2); // Ltemp2=factor-N*f1, Q20
  Ltemp1=L_add(Ltemp2,L_shl(Ltemp1,5)); // Ltemp1 in Q20

  Ltemp2=L_shr(L_mult(N,N),1);


#ifdef OVERFLOW_FIX
  temph=extract_h(Ltemp2);
  templ=extract_l(Ltemp2);
  Ltemp2=(Word32) L_mult_su(N, templ);
  Ltemp2=L_add(Ltemp2,  L_shl(L_mult(N, temph), 15)); //Ltemp2=N^3

#else
  Ltemp2=L_shr(L_mult(N,(Word16)Ltemp2),1); // Ltemp2=N^3
#endif
  Ltemp3=L_mult(N,c1); // Q20
  Ltemp3=L_sub(Ltemp3,L_shl(Ltemp1,1)); // Ltemp3=N*c1-2*Ltemp1, Q20

#ifdef VOIP_DORA_VOICE
      if (L_abs(Ltemp3) >= L_shl(Ltemp2,8)) {
       if (Ltemp3 > 0) Lacc = MAX_40;
	   else Lacc = MIN_40;
       c0=extract_h((Word32)L_sat32_40(Lacc)); // c0 in Q33
	   }
  else
   {
    Lacc=divide_dp(Ltemp3, Ltemp2, 0); // Lacc=Ltemp3/Ltemp2, Q49
	Lacc=L_add40(Lacc,0x08000);
    c0=extract_h((Word32)L_sat32_40(Lacc)); // c0 in Q33
	}
#else
  Lacc=divide_dp(Ltemp3, Ltemp2, 0); // Lacc=Ltemp3/Ltemp2, Q49
  Lacc=L_add40(Lacc,0x08000);
  c0=extract_h((Word32)L_sat32_40(Lacc)); // c0 in Q33
#endif
 
  Ltemp1=L_mult(N,N); // Ltemp1=2*N*N
  Ltemp1=L_add(Ltemp1,L_shr(Ltemp1,1)); // Ltemp1=3*N*N, max is 3*140*140

#ifdef  VOIP_DORA_VOICE
  //patch added for time warping support, where N can be more than 140
  if (Ltemp1 > 65537){
  Word16 dbgshft;
  dbgshft= norm_l(Ltemp1);
  Ltemp1= L_shl(Ltemp1,dbgshft);
  temp = extract_h(Ltemp1);
  Ltemp1=(Word32)L_shl(L_mult_su(c0,temp), (16-dbgshft)); // Ltemp1=3*N*N*c0, Q33
  //Patch end
  }
  else{
  temp=extract_l(Ltemp1);
  Ltemp1=(Word32)L_mult_su(c0,temp); // Ltemp1=3*N*N*c0, Q33
  }
#else
  temp=extract_l(Ltemp1);
  Ltemp1=(Word32)L_mult_su(c0,temp); // Ltemp1=3*N*N*c0, Q33
#endif

  

  Ltemp1=L_sub(L_shr(L_deposit_h(c1),2),Ltemp1); // Ltemp1=c1-3*N*N*c0, Q33

  Lacc=divide_dp(Ltemp1, N, -20); // Lacc=(c1-3*N*N*c0)/2/N, Q43
  Lacc=L_add40(Lacc,0x08000);
  c1=extract_h((Word32)L_sat32_40(Lacc)); // c1 in Q27
  
  // Computation of the phase value at each sample point
  // ph[n]=  c0*n^3+c1*n^2+c2*n+c3, Q15
#ifdef WMOPS_FX
  move16();
#endif
  phOut[0] = ph1 ;
  for (n=1; n<N; n++) {
    //    phOut[n] = _POLY3(n,coef) ;
#ifdef OVERFLOW_FIX
    Ltemp1 = L_shr(L_mult(n,n), 1);
    n2_32=Ltemp1; //Q0
    temph = extract_h(Ltemp1);
    templ = extract_l(Ltemp1);
    Ltemp1 = (Word32) L_mult_su(n, templ);
    Ltemp1 = L_add(Ltemp1, L_shl(L_mult(n, temph), 15));
#else
    n2=(Word16) L_shr(L_mult(n,n),1); // n2=n^2
    Ltemp1=L_shr(L_mult(n2,n),1); // Ltemp1=n^3
#endif
    n3h=extract_h(Ltemp1);
    n3l=extract_l(Ltemp1);
    Ltemp1=(Word32)L_mult_su(c0,n3l);
    Ltemp1=L_add(L_shr(Ltemp1,16),L_shr(L_mult(c0,n3h),1)); // Ltemp1=c0*n^3, Q17
 
#ifdef OVERFLOW_FIX
    temph=extract_h(n2_32); //Q(-16)
    templ=extract_l(n2_32); // Q0
    Ltemp2 = (Word32) L_mult_su(c1,templ); //Q27

    if (temph==0)
    {
    Ltemp2=L_shl(Ltemp2,1); //Q28
    Q_Ltemp2=28;
    }
    else
    {
    Ltemp2 = L_add(L_shr(Ltemp2,15),L_mult(temph,c1)); //Q12
    Q_Ltemp2=12; 
    }     
#else
    Ltemp2=L_mult(c1,n2); // Ltemp2=c1*n^2, Q28
    Q_Ltemp2=28;
#endif
    Ltemp3=L_mult(c2,n);  // Ltemp3=c2*n, Q20
    Ltemp3=L_add(L_shr(Ltemp3,5),c3);
    Ltemp3=L_add(L_shr(Ltemp2,Q_Ltemp2-15),Ltemp3);
    Ltemp3=L_add(L_shr(Ltemp1,2),Ltemp3); // Ltemp3=ph[n], Q15
    // Need to take modulo of Ltemp3 and keep the fractional part only
    phOut[n]=Ltemp3&0x7fff; // extract the fractional
#ifdef WMOPS_FX
    test();
#endif
    if (Ltemp3<0) phOut[n]=add(phOut[n],0x8000); // phOut[n]-=1, Q15
  }

  // last L2 phase values
  f2=shift_r(f2,-4);  // shift right and round, make it Q15
  for (; n<add(N,L2);n++) {
    Ltemp1=L_add(phOut[n-1],f2);
    phOut[n]=Ltemp1&0x7fff; // extract the fractional
#ifdef WMOPS_FX
    test();
#endif
    if (Ltemp1<0) phOut[n]=add(phOut[n],0x8000); // phOut[n]-=1, Q15
  }
}

/*===================================================================*/
/* FUNCTION      :  erb_slot_fx ()                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Allocate harmonics in ERB bins                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lag :  pitch lag, Q0                                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) out : number of harmonics in the ERB bins, Q0     */
/*   _ (Word16 []) mfreq : frequency bounds of the ERB bins, Q15     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: Frequency is normalized by 8000, i.e. 1=8000Hz              */
/*===================================================================*/
void erb_slot_fx(Word16 lag, Word16 *out, Word16 *mfreq)
{
  Word16 i, j, n;
  Word16 diff;
  Word32 mf[NUM_ERB];
  Word32 Ltemp;

  for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    out[i] = 0 ;
    mf[i] = 0 ;
  }

  Ltemp=invert_dp(lag, 4, &n); // Ltemp=1/lag, Q(61-n1)
  diff=round32_16(L_shl(Ltemp,sub(n,26))); // diff=1/lag, Q19
  
  n=shr(lag,1);

  for (i=j=0 ; i<=n; i++) {
    Ltemp=L_mult(diff,i); // Ltemp=i*diff, Q20
    //    freq=round32_16(L_shl(Ltemp,11)); // freq=i*diff, Q15
    Ltemp=MIN_FX(Ltemp,0x080000); // 0x80000=0.5 in Q20 (4000Hz)
    for ( ; j<NUM_ERB; j++) {
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub(Ltemp,L_shl(erb_fx[j+1],5))<0) {
	mf[j] = L_add(mf[j],Ltemp);
	out[j]=add(out[j],1);
	break ;
      }
    }
  }
  for (j=0; j<NUM_ERB; j++) {
    mfreq[j]=round32_16(L_shl(mf[j],11)); // Q15
#ifdef WMOPS_FX
    test();
#endif
    if (sub(out[j],1)>0) {
      Ltemp=(Word32)divide_dp(mf[j], out[j], -18); // Ltemp=mf[j]/out[j], Q31
      mfreq[j]=round32_16(Ltemp); // Q15
    }
  }
}

/*===================================================================*/
/* FUNCTION      :  LPCPowSpect_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute LPC power spectrum                       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) freq :  ERB frequency bounds, Q15                 */
/*   _ (Word16 []) LPC :  LPC coefficients, Q12                      */
/*   _ (Word16)  Nf:  number of ERB bins, Q0                         */
/*   _ (Word16) Np :  order of LPC, Q0                               */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) out : LPC power spectrum, Q7                      */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
/* NOTE: Frequency is normalized by 8000, i.e. 1=8000Hz              */
/*===================================================================*/
void LPCPowSpect_fx(Word16 *freq, Word16 *LPC, Word16 Nf, Word16 Np,
		    Word16 *out)
{
  Word16 i, k, n;
  Word16 w; // Q9
  Word16 t1, t2, dt;
  Word16 dh, dl;
  Word32 Re, Im;  // Q27
  Word32 Ltemp, Lw;
  Word40 Lacc;

  for (k=0; k<Nf; k++) {
#ifdef WMOPS_FX
  move32();
  move32();
  move32();
#endif
    Re=0x8000000; // Re=1.0, Q27
    Im=0;
    Lw=freq[k]; // Q15
    for (i=0;i<Np;i++) {
      Ltemp=L_shl(Lw,10); // Ltemp in Q25
      w=extract_h(Ltemp); // w in Q9
      dl=extract_l(Ltemp); // dl has 6 bits left-over
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      t1=cos_table[w%512];
      t2=cos_table[add(w,1)%512];
      dt=sub(t2,t1); // dt=t2-t1, Q15
      Ltemp=(Word32)L_mult_su(dt,dl); // Ltemp in Q31
      t1=add(t1,(Word16)L_shr(Ltemp,16)); // t1 is interpolated cos(w)
      Ltemp=L_shr(L_mult(LPC[i],t1),1); // Ltemp in Q27
      Re=L_sub(Re, Ltemp); // Re=1-sum(LPC[i]*cos(Lw));
      Ltemp=L_add(Lw,0x6000); // add 0.75, which is 3pi/2 to convert sin to cos
      Ltemp=L_shl(Ltemp,10); // Q25
      w=extract_h(Ltemp); // w is equivalent cos index
      dl=extract_l(Ltemp); // dl is 6 bit left-over for interpolation
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      t1=cos_table[w%512];
      t2=cos_table[add(w,1)%512];
      dt=sub(t2,t1); // dt=t2-t1, Q15
      Ltemp=(Word32)L_mult_su(dt,dl); // Ltemp in Q31
      t1=add(t1,(Word16)L_shr(Ltemp,16)); // t1 is interpolated cos(w)
      Ltemp=L_shr(L_mult(LPC[i],t1),1); // Ltemp in Q27
      Im=L_add(Im,Ltemp); // Im=sum(LPC[i]*sin(Lw))

      Lw=L_add(Lw,freq[k]); // Lw=(i+1)*freq[k]
    }
    /* If necessary, we can block-normalize Re and Im to improve precision */
    dh=extract_h(Re);
    dl=extract_l(Re);
    Lacc=L_mult_su(dh,dl);
    Lacc=L_add40(L_shr40(Lacc,15),L_shr(L_mult(dh,dh),1)); // Lacc=Re*Re
    dh=extract_h(Im);
    dl=extract_l(Im);
    Ltemp=(Word32)L_mult_su(dh,dl);
    Lacc=L_add40(Lacc,L_shr(Ltemp,15));
    Lacc=L_add40(Lacc,L_shr(L_mult(dh,dh),1)); // Lacc=Re^2+Im^2, Q22
    
    Ltemp=invert_dp(Lacc, 4, &n); // Ltemp in Q(39-n)
    out[k]=round32_16(L_shl(Ltemp,sub(n,16))); // output in Q7
  }
}

/*===================================================================*/
/* FUNCTION      :  GetSinCosTab_fx ()                               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Compute sine & cosine table given pitch lag,     */
/*                  by interpolating the 512-entry cosine table.     */
/*                  sin(2pi/4L*n) & cos(2pi/4L*n) for n=0,1,... 4L-1 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) L :  Pitch lag, Q0                                   */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) sinTab, Q15 : sin(2pi/4L*n), n=0,1,...,4L-1       */
/*   _ (Word16 []) cosTab, Q15 : cos(2pi/4L*n), n=0,1,...,4L-1       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: This function interpolates cos_table for better accuracy    */
/*===================================================================*/
void GetSinCosTab_fx(Word16 L, Word16 *sinTab, Word16 *cosTab)
{
  Word16 i, n, L4;
  Word16 dl, t1, t2, w, dt;
  Word32 invL; // 1/4L in Q25
  Word32 Ltemp, Lw;

  Ltemp=invert_dp(L,4,&n); // Ltemp=1/4L in Q(63-n)
  invL=L_shl(Ltemp,sub(n,38)); // 1/4L in Q25

  L4=shl(L,2);
  for (i=0, Lw=0;i<L4;i++) {
    w=extract_h(Lw); // w in Q9
    dl=extract_l(Lw); // dl has 16 bits left-over
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    t1=cos_table[w%512];
    t2=cos_table[add(w,1)%512];
    dt=sub(t2,t1); // Q15
    Ltemp=(Word32)L_mult_su(dt,dl); // Ltemp in Q31
    cosTab[i]=add(t1,(Word16)L_shr(Ltemp,16)); // interpolated cos(i*2pi/4L)
    
    Ltemp=L_add(Lw,0x1800000); // add 0.75 in Q25, which is 3pi/2 to convert sin to cos
    w=extract_h(Ltemp); // w is equivalent cos index
    dl=extract_l(Ltemp); // dl is 16 bit left-over for interpolation
#ifdef WMOPS_FX
  move16();
  move16();
#endif
    t1=cos_table[w%512];
    t2=cos_table[add(w,1)%512];
    dt=sub(t2,t1); // dt=t2-t1, Q15
    Ltemp=(Word32)L_mult_su(dt,dl); // Ltemp in Q31
    sinTab[i]=add(t1,(Word16)L_shr(Ltemp,16)); // t1 is interpolated cos(w)

    Lw=L_add(Lw,invL); // Lw=(i+1)/4L, Q25
  }
}

/*===================================================================*/
/* FUNCTION      :  erb_add_fx ()                                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Reconstruct current erb amplitude for QPPP       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) pl :  previous pitch lag, Q0                         */
/*   _ (Word16) l :   current pitch lag, Q0                          */
/*   _ (Word16 []) prev_erb : Previous erb amplitude, Q13            */
/*   _ (Word16 []) index: quantized differential erb index           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void erb_add_fx(Word16 pl, Word16 l, Word16 *index, Word16 *prev_erb,
		Word16 *curr_erb)
{
  Word16 i;
  Word16 pslot[NUM_ERB], cslot[NUM_ERB], t_prev_erb[NUM_ERB];
  Word16 tmp;

  erb_slot_fx(l, cslot, t_prev_erb);
  erb_slot_fx(pl,pslot,t_prev_erb);
  
  for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    t_prev_erb[i] = prev_erb[i] ;
  }

#ifdef WMOPS_FX
  test();
#endif
  if (sub(pl,l)>0) {
#ifdef WMOPS_FX
  move16();
#endif
    tmp = t_prev_erb[0] ; 
    for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
      test();
#endif
      if (pslot[i]!=0)
	tmp = t_prev_erb[i] ;
      else
	t_prev_erb[i] = tmp ;
    }
  } else if (sub(l,pl)>0) {
#ifdef WMOPS_FX
  move16();
#endif
    tmp = t_prev_erb[NUM_ERB-1] ;
    for (i=NUM_ERB-1; i>=0; i--) {
#ifdef WMOPS_FX
      test();
#endif
      if (pslot[i]!=0)
	tmp = t_prev_erb[i] ;
      else
	t_prev_erb[i] = tmp ;
    }
  }

  tmp=add(shl(index[0],3),shl(index[0],1)); // tmp=10*index[0]
  for (i=1; i<11; i++) { 
#ifdef WMOPS_FX
    test();
#endif
    if (cslot[i]!=0) {
      curr_erb[i] = add(AmpCB1_fx[tmp+i-1],t_prev_erb[i]) ; // Q13
      curr_erb[i] = MAX_FX(0, curr_erb[i]);
    } else
      curr_erb[i] = 0;
  }

  tmp=add(shl(index[1],3),index[1]); // tmp=9*index[1]
  for (i=11; i<20; i++) { 
#ifdef WMOPS_FX
    test();
#endif
    if (cslot[i]!=0) {
      curr_erb[i] = add(AmpCB2_fx[tmp+i-11],t_prev_erb[i]) ;
       curr_erb[i] = MAX_FX(0, curr_erb[i]);
    } else
      curr_erb[i] = 0;
  }
}

/*===================================================================*/
/* FUNCTION      :  erb_diff_search_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  erb amplitude VQ search for QPPP                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) prev_erb : Previous erb amplitude, Q13            */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*   _ (Word16 []) dif_erb: erb differential, Q13                    */
/*   _ (Word16 []) pow_spec : LPC power spectrum, Q7                 */
/*   _ (Word16 [][]) cb_fx : differential erb codebook, Q13          */
/*   _ (Word16) cb_size :  codebook size                             */
/*   _ (Word16) cb_dim :  codebook dimension                         */
/*   _ (Word16) offset :  index to current segment of erb array      */
/*                        for quantization                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) index: best codebook index                           */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
/* NOTE: This function is only used by the next function             */
/*===================================================================*/
Word16 erb_diff_search_fx(Word16 *prev_erb, Word16 *curr_erb, Word16 *dif_erb,
			  Word16 *pow_spec, Word16 *cb_fx,
			  Word16 cb_size, Word16 cb_dim, Word16 offset)
{
  Word16 i, j, mmseindex ;
  Word16 dh, dl;
  Word32 mmse;
  Word32 Ltemp1;
  Word40 Lacc;

#ifdef WMOPS_FX
  move32();
  move16();
#endif
  mmse=LW_MAX;
  mmseindex=-1;
  for (j=0; j<cb_size; j++) {
#ifdef WMOPS_FX
  move32();
#endif
    Lacc=0;
    for (i=0; i<cb_dim; i++) {
#ifdef WMOPS_FX
      test();
#endif
	if (add(cb_fx[j*cb_dim+i],prev_erb[i+offset])<0) {
	  Ltemp1=L_mult(curr_erb[i+offset],curr_erb[i+offset]); // Q27
	  dh=extract_h(Ltemp1);
	  dl=extract_l(Ltemp1);
	  Ltemp1=(Word32)L_mult_su(pow_spec[i+offset],dl);
	  Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(pow_spec[i+offset],dh));
	}
	else {
	  dh=sub(dif_erb[i+offset],cb_fx[j*cb_dim+i]); // Q13
	  Ltemp1=L_mult(dh,dh); // Q27
	  dh=extract_h(Ltemp1);
	  dl=extract_l(Ltemp1);
	  Ltemp1=(Word32)L_mult_su(pow_spec[i+offset],dl); // Q33
	  Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(pow_spec[i+offset],dh)); // Q18
	}
#ifdef WMOPS_FX
    test();
#endif
	if (sub(cb_fx[j*cb_dim+i],dif_erb[i+offset])<0) {
	  dh=extract_h(Ltemp1);
	  dl=extract_l(Ltemp1);
	  Ltemp1=(Word32)L_mult_su(29491,dl); // 29491=0.9 in Q15
	  Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(dh,29491));
	}
	Lacc=L_add40(Lacc,Ltemp1); // Q18
    }
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub40(Lacc,mmse)<0) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
      mmse = (Word32) Lacc ;
      mmseindex = j ;
    }
  }

  return (mmseindex);
  
}


/*===================================================================*/
/* FUNCTION      :  erb_diff_fx ()                                   */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Quantize erb amplitude for QPPP                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) pl :  previous pitch lag, Q0                         */
/*   _ (Word16) l :   current pitch lag, Q0                          */
/*   _ (Word16 []) prev_erb : Previous erb amplitude, Q13            */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*   _ (Word16 []) curr_lpc : LPC coefficients, Q12                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) index: quantized differential erb index           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
void erb_diff_fx(Word16 pl, Word16 l, Word16 *prev_erb, Word16 *curr_erb,
		 Word16 *curr_lpc, Word16 *index)
{
  Word16 i, pslot[NUM_ERB], cslot[NUM_ERB];
  Word16 t_prev_erb[NUM_ERB], lpc[ORDER], mfreq[NUM_ERB], PowSpect[NUM_ERB];
  Word16 dif_erb[NUM_ERB];
  Word16 tmp;
  
  erb_slot_fx(l,cslot,mfreq);
  erb_slot_fx(pl,pslot,t_prev_erb);
  
  for (i=0;i<ORDER;i++) {
    lpc[i]=mult_r(curr_lpc[i],pwf78_fx[i]);
  }
  
  LPCPowSpect_fx(mfreq, lpc, NUM_ERB, ORDER, PowSpect); // PowSpect in Q7

  for (i=0;i<NUM_ERB;i++) {
#ifdef WMOPS_FX
    test();
#endif
    if (cslot[i]==0) {
#ifdef WMOPS_FX
  move16();
#endif
      PowSpect[i]=0;
    }
  }

  for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
  move16();
#endif
    t_prev_erb[i] = prev_erb[i] ;
  }
  
#ifdef WMOPS_FX
  test();
#endif
  if (sub(pl,l)>0) {
#ifdef WMOPS_FX
  move16();
#endif
    tmp = t_prev_erb[0] ; 
    for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
      test();
#endif
      if (pslot[i]!=0)
	tmp = t_prev_erb[i] ;
      else
	t_prev_erb[i] = tmp ;
    }
  } else if (sub(l,pl)>0) {
#ifdef WMOPS_FX
  move16();
#endif
    tmp = t_prev_erb[NUM_ERB-1] ;
    for (i=NUM_ERB-1; i>=0; i--) {
#ifdef WMOPS_FX
      test();
#endif
      if (pslot[i]!=0)
	tmp = t_prev_erb[i] ;
      else
	t_prev_erb[i] = tmp ;
    }
  }

  for (i=0; i<NUM_ERB; i++)
    dif_erb[i] = sub(curr_erb[i],t_prev_erb[i]) ;
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant erb_diff LPC Pow Spect ...............");
#endif

  //First Band Amplitude Search

  index[0] = erb_diff_search_fx(t_prev_erb, curr_erb, dif_erb,
			  PowSpect, AmpCB1_fx,
			  ERB_CBSIZE1, 10, 1) ;

#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant erb_diff Search first band ...........");
#endif
  //Second Band Amplitude Search
  index[1] = erb_diff_search_fx(t_prev_erb, curr_erb, dif_erb,
			  PowSpect, AmpCB2_fx,
			  ERB_CBSIZE2, 9, 11) ;
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant erb_diff Search second band ..........");
#endif
}

/*===================================================================*/
/* FUNCTION      :  erb_quant_search_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  erb amplitude VQ search for FPPP                 */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*   _ (Word16 []) pow_spec : LPC power spectrum, Q7                 */
/*   _ (Word16 [][]) cb_fx : erb codebook, Q13                       */
/*   _ (Word16) cb_size :  codebook size                             */
/*   _ (Word16) cb_dim :  codebook dimension                         */
/*   _ (Word16) offset :  index to current segment of erb array      */
/*                        for quantization                           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) index: best codebook index                           */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
/* NOTE: This function is only used by the next function             */
/*===================================================================*/
Word16 erb_quant_search_fx(Word16 *curr_erb, Word16 *pow_spec, Word16 *cb_fx,
			   Word16 cb_size, Word16 cb_dim, Word16 offset)
{
  Word16 i, j, mmseindex ;
  Word16 dh, dl, dt;
  Word32 mmse;
  Word32 Ltemp1;
  Word40 Lacc;

#ifdef WMOPS_FX
  move32();
  move16();
#endif
  mmse=LW_MAX;
  mmseindex=-1;
  for (j=0; j<cb_size; j++) {
#ifdef WMOPS_FX
  move32();
#endif
    Lacc=0;
    for (i=0; i<cb_dim; i++) {
	dt=sub(cb_fx[j*cb_dim+i],curr_erb[i+offset]); // Q13
	Ltemp1=L_mult(mult_r(dt,dt),pow_spec[i+offset]);
#ifdef WMOPS_FX
    test();
#endif
	if (dt<0) {
	  dh=extract_h(Ltemp1);
	  dl=extract_l(Ltemp1);
	  Ltemp1=(Word32)L_mult_su(29491,dl); // 29491=0.9 in Q15
	  Ltemp1=L_add(L_shr(Ltemp1,15),L_mult(dh,29491));
	}
	Lacc=L_add40(Lacc,Ltemp1); // Q19
    }
    
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub40(Lacc,mmse)<0) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
      mmse = (Word32) Lacc ;
      mmseindex = j ;
    }
  }
  return (mmseindex);
}

/*===================================================================*/
/* FUNCTION      :  erb_quant_fx ()                                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Quantize erb amplitude for FPPP                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) l :   current pitch lag, Q0                          */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*   _ (Word16 []) curr_lpc : LPC coefficients, Q12                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) index: quantized erb index, 3 words               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
void erb_quant_fx(Word16 l, Word16 *curr_erb, Word16 *curr_lpc,
		  Word16 *index)
{
  Word16 i, cslot[NUM_ERB];
  Word16 lpc[ORDER], mfreq[NUM_ERB], PowSpect[NUM_ERB];

  erb_slot_fx(l,cslot,mfreq);
  
  for (i=0;i<ORDER;i++) {
    lpc[i]=mult_r(curr_lpc[i],pwf78_fx[i]);
  }
  
  LPCPowSpect_fx(mfreq, lpc, NUM_ERB, ORDER, PowSpect); // PowSpect in Q7

  for (i=0;i<NUM_ERB;i++) {
#ifdef WMOPS_FX
    test();
#endif
    if (cslot[i]==0) {
#ifdef WMOPS_FX
  move16();
#endif
      PowSpect[i]=0;
    }
  }
  index[0]=erb_quant_search_fx(curr_erb, PowSpect, A_AmpCB0_fx,
			       A_ERB_CBSIZE0, 5, 1);
  index[1]=erb_quant_search_fx(curr_erb, PowSpect, A_AmpCB1_fx,
			       A_ERB_CBSIZE1, 5, 6);
  index[2]=erb_quant_search_fx(curr_erb, PowSpect, A_AmpCB2_fx,
			       A_ERB_CBSIZE2, 9, 11);
}

/*===================================================================*/
/* FUNCTION      :  erb_dequant_fx ()                                */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Reconstruct erb amplitude for FPPP               */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) l :   current pitch lag, Q0                          */
/*   _ (Word16 []) index: quantized erb index                        */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) curr_erb : Current erb amplitude, Q13             */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void erb_dequant_fx(Word16 l, Word16 *index, Word16 *curr_erb)
{
  Word16 i, cslot[NUM_ERB] ;
  Word16 temp[NUM_ERB] ;
  Word16 offset;

  erb_slot_fx(l,cslot,temp);

  offset=extract_l(L_shr(L_mult(index[0],5),1)); // offset=5*index[0]
  offset=sub(offset,1);
  for (i=1; i<6; i++) { 
#ifdef WMOPS_FX
    test();
	move16();
#endif
    if (cslot[i]!=0) 
      curr_erb[i] = A_AmpCB0_fx[offset+i];
    else
      curr_erb[i] = 0;
  }
  
  offset=extract_l(L_shr(L_mult(index[1],5),1)); // offset=5*index[1]
  offset=sub(offset,6);
  for (i=6; i<11; i++) { 
#ifdef WMOPS_FX
    test();
	move16();
#endif
    if (cslot[i]!=0)
      curr_erb[i] = A_AmpCB1_fx[offset+i];
    else
      curr_erb[i] = 0;
  }
  
  offset=extract_l(L_shr(L_mult(index[2],9),1)); // offset=9*index[2]
  offset=sub(offset,11);
  for (i=11; i<20; i++) { 
#ifdef WMOPS_FX
    test();
	move16();
#endif
    if (cslot[i]!=0) 
      curr_erb[i] = A_AmpCB2_fx[offset+i];
    else
      curr_erb[i] = 0;
  }
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx:to_erb_fx ()                             */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convert harmonics to erb bands                   */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude, normalized               */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16 []) erb_out : erb output, Q13                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void DTFS_fx::to_erb_fx(Word16 *erb_out)
{
  Word16 i, j, n;
  Word16 count[NUM_ERB] ;
  Word16 diff ;
  Word32 sum_a[NUM_ERB];
  Word32 Ltemp;

  for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
  move16();
  move32();
#endif
    count[i] = 0 ;
    sum_a[i] = 0 ;
  }

  Ltemp=invert_dp(lag, 4, &n); // Ltemp=1/lag, Q(61-n)
  diff=round32_16(L_shl(Ltemp,sub(n,26))); // diff=1/lag, Q19
  
  n=shr(lag,1);

  for (i=j=0 ; i<=n; i++) {
    Ltemp=L_mult(diff,i); // Ltemp=i*diff, Q20
    for ( ; j<NUM_ERB; j++) {
#ifdef WMOPS_FX
      test();
#endif
      if (L_sub(Ltemp,L_shl(erb_fx[j+1],5))<0) {
	sum_a[j] = L_add(sum_a[j],L_deposit_l(a[i])); // Q
	count[j]=add(count[j],1);
	break ;
      }
    }
  }
  /* Make output in Q13  */
  n=sub(29,Q);
  j=negate(Q);
  for (i=0; i<NUM_ERB; i++) {
    erb_out[i]=round32_16(L_shl(sum_a[i],n)); // Q13
#ifdef WMOPS_FX
    test();
#endif
    if (sub(count[i],1)>0) {
      Ltemp=(Word32)divide_dp(sum_a[i], count[i], j); // Ltemp=sum_a[i]/count[i], Q29
      erb_out[i]=round32_16(Ltemp); // Q13
    }
  }
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx:erb_inv_fx ()                            */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Convert erb into harmonics                       */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) erb_in : erb output, Q13                          */
/*   _ (Word16 []) slot : number of harmonics in the ERB bins, Q0    */
/*   _ (Word16 []) mfreq : frequency bounds of the ERB bins, Q15     */
/*   _ (DTFS_fx) (Word16) lag: length of prototype in time domain    */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) : prototype in polar domain                         */
/*                (Word16 []) a: amplitude, normalized               */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void DTFS_fx::erb_inv_fx(Word16 *erb_in, Word16 *slot, Word16 *mfreq)
{
  Word16 i, j, m, n, HalfLag;
  Word16 diff; // 1/lag, Q19
  Word16 d1,d2, q[MAXLAG_WI], min_q;
  Word16 d1h, d1l, d2h, d2l;
  Word16 freq, f[NUM_ERB+2], amp[NUM_ERB+2] ;
  Word32 Ltemp, Ltemp2;
  Word40 Lacc;
  
  m=0;
  f[m]=0;
  amp[m]=0;
  m=add(m,1);

  for (i=0; i<NUM_ERB; i++) {
#ifdef WMOPS_FX
    test();
#endif
    if (slot[i] != 0) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
      f[m]=mfreq[i];
      amp[m]=erb_in[i];
      m=add(m,1);
    }
  }
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  f[m]=0x4000;  // 0.5 in Q15
  amp[m]=0;
  m=add(m,1);

  Ltemp=invert_dp(lag, 4, &n); // Ltemp=1/lag, Q(61-n)
  diff=round32_16(L_shl(Ltemp,sub(n,26))); // diff=1/lag, Q19
#ifdef WMOPS_FX
  move16();
  move16();
#endif
  min_q=SW_MAX;
  a[0]=0;
  HalfLag=shr(lag,1);
  for (i=1, j=1; i<=HalfLag; i++) {
    Ltemp=L_mult(diff,i);  // Ltemp=i*diff, Q20
    freq=round32_16(L_shl(Ltemp,11)); // Q15
    freq=MIN_FX(freq,0x4000); // 0x4000 is 0.5 in Q15
    for ( ; j<m; j++) {
      d1=sub(f[j],freq);
#ifdef WMOPS_FX
      test();
#endif
      if (d1>=0) {
	d2=sub(freq,f[j-1]);
	Ltemp=L_add(L_mult(amp[j],d2),L_mult(amp[j-1],d1)); // Q29
	d2=sub(f[j],f[j-1]); // Q15
	Ltemp2=invert_dp(d2, 4, &n); // Ltemp2=1/d2, Q(61-15-n)
	d1h=extract_h(Ltemp);
	d1l=extract_l(Ltemp);
	d2h=extract_h(Ltemp2);
	d2l=extract_l(Ltemp2);
	Ltemp=(Word32)L_mult_su(d1h,d2l);
	Lacc=L_mac40_su(Ltemp,d2h,d1l);
	Ltemp=L_add((Word32)L_shr40(Lacc,15),L_mult(d1h,d2h)); // 46-n+29-31
	d2h=norm_l(Ltemp); // d2h is 0 if Ltemp=0
	d2h=(Ltemp==0)?31:d2h; // make sure if Ltemp=0, it gets the largest Q
	a[i]=round32_16(L_shl(Ltemp,d2h)); // Q(28-n+d2h)
	q[i]=add(sub(28,n),d2h);
	min_q=MIN_FX(min_q,q[i]);
	break ;
      }
    }
  }
  // block normalize a[i]
  for (i=1;i<=HalfLag; i++) {
    a[i]=shl(a[i],sub(min_q,q[i]));
  }
#ifdef WMOPS_FX
  move16();
#endif
  Q=min_q;
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::car2pol_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Cartesian to polar representation                */
/*                  returning amplitudes and 0 phases                */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X :  prototype in Cartesian domain                  */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a,b: re/im of harmonics, normalized    */
/*                (Word16) Q: norm factor of a/b                     */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16 []) b: phase of harmonics,cleared to 0     */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: output X.b (angle) is not computed and should be ignored    */
/*       When phases are needed, such as in QPPP, instead uses the   */
/*       Cartesian representation to avoid computing phases by arctan*/
/*===================================================================*/
void DTFS_fx::car2pol_fx()
{
  Word16 k, HalfLag;
  Word32 Ltemp;
  Word40 Lacc;

  HalfLag = shr(sub(lag,1),1 );
  for ( k=1 ; k<=HalfLag; k++ ) {
    Lacc=L_mult(a[k],a[k]); // a[k]^2, 2Q+1
    Lacc=L_mac40(Lacc,b[k],b[k]); // a[k]^2+b[k]^2, 2Q+1

    Lacc=L_shr40(Lacc,3); // Lacc=(a[k]^2+b[k]^2)/4, 2Q
    Ltemp=dsp_sqrt_lut((Word32) Lacc, 0x8000); // Q+16
    a[k]=extract_h(Ltemp); // Q
#ifdef WMOPS_FX
  move16();
#endif
    b[k]=0; // clear b[k]
  }

#ifdef WMOPS_FX
  test();
#endif
  if (lag%2==0) {
    Lacc=L_mult(a[k],a[k]); // a[k]^2, 2Q+1
    Lacc=L_mac40(Lacc,b[k],b[k]); // a[k]^2+b[k]^2, 2Q+1
    Ltemp=(Word32)L_sat32_40(L_shr40(Lacc,1)); // Ltemp=(a[k]^2+b[k]^2), 2Q
    Ltemp=dsp_sqrt_lut(Ltemp, 0x8000); // Q+16
    a[k]=extract_h(Ltemp); // Q
#ifdef WMOPS_FX
  move16();
#endif
    b[k]=0; // clear b[k]
  }
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::pol2car_fx ()                           */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  polar to Cartesian representation                */
/*                  assuming all phases being 0                      */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16 []) b: phase of harmonics, assumed 0       */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) X :  prototype in Cartesian domain                  */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a,b: re/im of harmonics, normalized    */
/*                (Word16) Q: norm factor of a/b                     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* X.a[k]=2.0 * X.a[k] ;  because all phases are assumed to be 0     */
/* X.b[k]=0 ;                                                        */
/*===================================================================*/
void DTFS_fx::pol2car_fx ()
{
  Word16 k, HalfLag ;
  

  HalfLag = shr(sub(lag,1),1 );
  for ( k=1 ; k<=HalfLag; k++ ) {
#ifdef WMOPS_FX
  move16();
#endif
    b[k] =0;
  }
#ifdef WMOPS_FX
  test();
#endif
  if (lag%2==0) {
#ifdef WMOPS_FX
  move16();
#endif
    b[k] =0 ;
    a[k] = shr(a[k],1);
  }
  Q=sub(Q,1); // equivalent to a[k]=2*a[k] by adjusting Q factor 
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::copy_phase_fx ()                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Retain the amplitudes of a prototype X2, but copy*/
/*                  the phases of another prototype X1 of same length*/
/*                  over to make a new prototype                     */  
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X1 :  prototype in Cartesian domain                 */
/*                (Word16) X1.lag: length of prototype in time domain*/
/*                (Word16 []) X1.a,b: re/im of harmonics, normalized */
/*                (Word16) X1.Q: norm factor of X2.a/b               */
/*   _ (DTFS_fx) X2 :  prototype in polar domain                     */
/*                (Word16) X2.lag: should be same as X1.lag          */
/*                (Word16 []) X2.a:amplitude of harmonics, normalized*/
/*                (Word16 []) X2.b: phase of harmonics, don't care   */
/*                (Word16) X2.Q: norm factor of X2.a                 */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (DTFS_fx) X :  prototype in Cartesian domain                  */
/*                    The amplitudes of this prototype are from X2   */
/*                    and the phases are from X1                     */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* X.a[k]=2*X2.a[k]/sqrt(X1.a[k]^2+X1.b[k]^2)*X1.a[k]                */
/* X.b[k]=2*X2.a[k]/sqrt(X1.a[k]^2+X1.b[k]^2)*X1.b[k]                */
/*===================================================================*/
DTFS_fx DTFS_fx::copy_phase_fx(DTFS_fx X2)
{
  DTFS_fx X;
  Word16 k, n=0, q, sn, cn, HalfLag;
  Word16 d1h, d1l;
  Word32 Ltemp;
  Word40 Lacc;

#ifdef WMOPS_FX
  move16();
#endif
  X.lag=lag;
  X.Q=sub(X2.Q,1); // equivalent to 2x
  HalfLag=shr(lag,1);
  for ( k=1 ; k<=HalfLag; k++ ) {
    Lacc=L_mult(a[k],a[k]);
    Lacc=L_mac40(Lacc,b[k],b[k]); // 2*Q+1
    Ltemp=invert_dp(Lacc, 4, &n); // (61-2Q-1-n)
    Ltemp=L_shr(Ltemp,1); // 59-2Q-n
    q=sub(sub(59,shl(Q,1)),n); // q=59-2Q-n
    Ltemp=dsp_sqrt_lut(Ltemp, 0);

#ifdef WMOPS_FX
    test();
	logic16();
#endif
    if (q & 1)
      Ltemp=(Word32)Mpy_32_16(Ltemp, 23170); // 23170 is 1/sqrt(2) in Q15
    q=shr(q,1); // Ltemp in Q(q+16)
    
    d1h=extract_h(Ltemp);
    d1l=extract_l(Ltemp);
    Ltemp=(Word32)L_mult_su(b[k],d1l);
    Ltemp=L_add(L_shr(Ltemp,15),L_mult(b[k],d1h)); // sin(w) in Q(q+16+Q-15)
    sn=round32_16(L_shl(Ltemp,sub(30,add(q,Q)))); // Q15
    X.b[k]=round32_16(L_mult(X2.a[k],sn)); // X2.Q
    
    Ltemp=(Word32)L_mult_su(a[k],d1l);
    Ltemp=L_add(L_shr(Ltemp,15),L_mult(a[k],d1h)); // cos(w) in Q(q+Q+1)
    cn=round32_16(L_shl(Ltemp,sub(30,add(q,Q)))); // Q15
    X.a[k]=round32_16(L_mult(X2.a[k],cn)); // X2.Q
  }
  k=sub(k,1);
#ifdef WMOPS_FX
  test();
#endif
  if (lag%2==0) {
    X.a[k] = shr(X.a[k],1);
    X.b[k] = shr(X.b[k],1);
  }
  
  return X;
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::getSpEngyFromResAmp_fx ()               */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Get band energy                                  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (Word16) lband: lower frequency bound, Q15                    */
/*   _ (Word16) hband: upper frequency bound, Q15                    */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word32) en: energy of the specified frequency band,          */
/*                  Q factor is 2Q-13                                */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
Word32 DTFS_fx::getSpEngyFromResAmp_fx(Word16 lband, Word16 hband,
				       Word16 *curr_lpc, Word16 *sin_tab, 
				       Word16 *cos_tab)
{
  Word16 i, k, k4, n, M, HalfLag;
  Word16 fdiff, freq;
  Word32 Ltemp;
  Word40 Lacc;
  Word32 Re, Im;  // Q27
  Word32 en;

#ifdef WMOPS_FX
  move32();
#endif
  en=0;
#ifdef WMOPS_FX
  test();
  move16();
#endif
  if (sub(hband,0x4000)==0) hband = 0x4004; // 4001.0/8000 in Q15
  M=shl(lag,2); // M=4*lag
  Ltemp=invert_dp(lag, 4, &n); // Ltemp=1/lag, Q(61-n)
  fdiff=round32_16(L_shl(Ltemp,sub(n,26))); // fdiff=1/lag, Q19

  HalfLag = shr(lag,1);
  for (k=0; k<=HalfLag; k++) {
    Ltemp=L_mult(fdiff,k); // Ltemp=k*fdiff, Q20
    freq=extract_h(L_shl(Ltemp,11)); // k*fdiff in Q15
#ifdef WMOPS_FX
    test();
#endif
    if(sub(freq,hband)<0 && sub(freq,lband)>=0) {
#ifdef WMOPS_FX
  move32();
#endif
      Lacc=0x10000000; // Re=1.0, Q28
      k4=shl(k,2); // k4=4*k
#ifdef WMOPS_FX
  move16();
#endif
      n=k4;
      for (i=0; i<ORDER; i++) { // Compute Re
	Lacc=L_msu40(Lacc,curr_lpc[i],cos_tab[n%M]); // Q28
	n=add(n,k4); // n=4*i*k
      }
      Re=(Word32)L_sat32_40(L_shr40(Lacc,1)); // Q27
#ifdef WMOPS_FX
  move16();
  move32();
#endif
      n=k4;
      Lacc=0;
      for (i=0; i<ORDER; i++) { // Compute Im
	Lacc=L_mac40(Lacc,curr_lpc[i],sin_tab[n%M]); // Q28
	n=add(n,k4); // n=4*i*k
      }
      Im=(Word32)L_sat32_40(L_shr40(Lacc,1)); // Q27
      Lacc=L_add40(L_mult_ll(Re,Re),(Word32)L_mult_ll(Im,Im)); // Lacc=Re^2+Im^2 in Q23
      Ltemp=L_mult(a[k],a[k]); // 2*a[k]^2 in 2Q
      Ltemp=(Word32)L_sat32_40(divide_dp(Ltemp,Lacc,-19)); // Ltemp in Q(2Q-13)
      
#ifdef WMOPS_FX
      test();
	  logic16();
#endif
      if (lag%2==0 && sub(k,shr(lag,1))==0)
	en=L_add(en,L_shr(Ltemp,1));
      else
	en=L_add(en,Ltemp); // en in 2Q-13
    }
    
  }
  return (en); // en in 2Q-13
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::dequant_cw_fx ()                        */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Dequantize QPPP prototype                        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) pl: previous lag                                     */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices, 2 words                  */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*   _ (Word16[]) curr_erb: Quantized current ERB, Q13               */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (Word16[]) lasterb: ERB history for differential              */
/*                quantization, Q13                                  */
/*   _ (Word16) Lgain: low band power history, log domain, Q11       */
/*   _ (Word16) Hgain: high band power history, log domain, Q11      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void DTFS_fx::dequant_cw_fx(Word16 pl, Word16 p_idx, Word16 *a_idx,
			    Word16 *lasterb, Word16& Lgain, Word16& Hgain,
			    Word16 *curr_erb)
{
  Word16 tmp, mfreq[NUM_ERB];
  Word16 slot[NUM_ERB] ;
  Word16 n;
  Word16 Ql, Qh;
  Word32 logLag;
  Word32 Ltemp;
  Word40 Lacc;
#ifdef WMOPS_FX
      WMP_fwc_fx("");
#endif

  //Amplitude Dequantization
  erb_add_fx(pl, lag, a_idx, lasterb, curr_erb);
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant ERB add ......................");
#endif
  curr_erb[0] = mult_r(curr_erb[1],9830);
  curr_erb[20] = mult_r(curr_erb[19],9830);
#ifdef WMOPS_FX
  move16();
#endif
  curr_erb[21] = 0;
  erb_slot_fx(lag, slot, mfreq);

#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant ERB slot .....................");
#endif
  this->erb_inv_fx(curr_erb, slot, mfreq);  // erb->amplitude
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant ERB inv  ....................");
#endif
  /* Normalize band energy to 1.0 */
  this->setEngyHarm_fx(377, 4524, 0, 4524, 1, 0, &Ql); // Ql is norm factor of low band a[]
  this->setEngyHarm_fx(4524, 13517, 4524, 16384, 1, 0, &Qh);
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant setEngyHarm ..................");
#endif
  /* Need to unify the Q factors of both bands */
  Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
  n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
  if (n<0) this->rshiftHarmBand_fx(4524, 16384,n);
  else if (n>0) this->rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));

  this->to_erb_fx(lasterb); // output in Q13

#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant to_erb .......................");
#endif
  //Power Dequantization
  tmp=shl(p_idx,1); // tmp=2*p_idx
  Lgain =add(Lgain, PowerCB_fx[tmp]) ; // Q11
  Hgain =add(Hgain, PowerCB_fx[tmp+1]); // Q11

  Ltemp=log10_lut(lag); // Ltemp=10*log10(lag), Q23
  logLag=(Word32)Mpy_32_16(Ltemp,0x6666); // logLag=log10(lag), Q26

  Ltemp=L_sub(L_shr(L_deposit_h(Lgain),1),logLag); // Ltemp=Lgain-log10(lag), Q26
  
  Lacc=dsp_pow10(Ltemp); // Lacc=10^Lgain/lag, Q15

  n=norm32_l40(Lacc); 
  Ltemp=(Word32)L_shl40(Lacc,n); // Ltemp in Q(15+n)
  this->setEngyHarm_fx(377, 4524, 0, 4524, Ltemp, add(15,n), &Ql);

  Ltemp=L_sub(L_shr(L_deposit_h(Hgain),1),logLag); // Ltemp=Hgain-log10(lag), Q26
  Lacc=dsp_pow10(Ltemp); // Lacc=10^Hgain/lag, Q15
  n=norm32_l40(Lacc); 
  Ltemp=(Word32)L_shl40(Lacc,n); // Ltemp in Q(15+n)
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant Power Dequant ................");
#endif
  this->setEngyHarm_fx(4524, 13517, 4524, 16384, Ltemp, add(15,n), &Qh);
  /* Need to unify the Q factors of both bands */
  Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
  n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
  if (n<0) this->rshiftHarmBand_fx(4524, 16384,n);
  else if (n>0) this->rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant Set power and finish .........");
#endif
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx:: rshiftHarmBand_fx()                    */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Right-shift harmonics in band to align Q factor  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) lband: lower band boundary, Q15                      */
/*   _ (Word16) hband: upper band boundary, Q15                      */
/*   _ (Word16) shift: right shift value, Q0 (must be <0)            */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
/* NOTE: This function should be called after two consecutive calls  */
/*       to setEngyHarm_fx, because the norm factor outputs from     */
/*       the two previous calls might be different                   */
/*===================================================================*/
void DTFS_fx::rshiftHarmBand_fx(Word16 lband, Word16 hband, Word16 shift)
{
  Word16 k, HalfLag;
  Word16 low, high;

  low=mult(lband,lag);  // low=lband*lag, Q0
  high=mult(hband,lag); // high=hband*lag, Q0
  HalfLag = shr(sub(lag, 1), 1);
  for (k=low+1; k<=high; k++) {
      a[k]=shift_r(a[k],shift);  // right shift and round
  }
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::quant_target()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Get weighting and target for power quantization  */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16[]) w: Weighting for power quantization, Q15           */
/*   _ (Word16[]) target: Power of 2 bands for quantization, Q11     */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
/* NOTE: This function is used by quant_cw_fx and quant_cw_memless_fx*/
/*===================================================================*/
void DTFS_fx::quant_target(Word16 *curr_lpc, Word16 *w, Word16 *target,
			   Word16 *sin_tab, Word16 *cos_tab)
{
  Word16 tmp, n;
  Word16 Ql, Qh;
  Word32 Ltemp, Ltemp1, Ltemp2, logLag, Ltemp3;
  
  tmp=sub(shl(Q,1),13); // tmp=2Q-13, Q factor of getSpEngy... output
  Ltemp3=L_shl(L_mult(tmp,24660),9); // Ltemp3=(2Q-13)*10log10(2), Q23, 24660=10log10(2) in Q13
  Ltemp1=log10_lut(this->getSpEngyFromResAmp_fx(0, 4524,curr_lpc, sin_tab, cos_tab)); // Ltemp1=10log10(eng_lb), Q23, need to adjust for Q factor of energy (2Q-13)
  // subtract 10log10(2)*(2Q-13), Q23
  Ltemp1=L_sub(Ltemp1,Ltemp3); 
  Ltemp1=MAX_FX(0,Ltemp1);

  Ltemp2=log10_lut(this->getSpEngyFromResAmp_fx(4524,16384,curr_lpc, sin_tab, cos_tab)); // Ltemp1=10log10(eng_hb), Q23, need to adjust for Q factor of energy (2Q-13)
  Ltemp2=L_sub(Ltemp2,Ltemp3); // Ltemp2 in Q23

  Ltemp2=MAX_FX(0,Ltemp2);

  //Getting the Speech Domain Energy LOG Ratio
  Ltemp=invert_dp(L_add40(Ltemp1,Ltemp2), 4, &n); // Ltemp=1/(eng_lb+eng_hb), Q(61-23-n)

  tmp=round32_16(Ltemp); // tmp in Q(22-n)
  Ltemp1=(Word32)Mpy_32_16(Ltemp1,tmp); // Q(30-n)
  n=add(n,1);
  w[0]=round32_16(L_shl(Ltemp1,n)); // w[0] in Q15
  Ltemp2=(Word32)Mpy_32_16(Ltemp2,tmp); 
  w[1]=round32_16(L_shl(Ltemp2,n)); // w[1] in Q15

  logLag=log10_lut(lag); // logLag=10*log10(lag), Q23
  Ltemp3=L_shl(L_mult(shl(Q,1),24660),9); // Ltemp3=2Q*10log10(2), Q23
  /* Process low band */
  Ltemp=this->setEngyHarm_fx(377, 4524, 0, 4524, 1, 0, &Ql); // Ql is norm factor of low band a[], Ltemp is energy in 2Q
  /* Compensate for Q factor of energy to get log10(lag*eng)  */
  Ltemp=log10_lut(Ltemp); // Ltemp=10log10(eng), Q23
  Ltemp=L_add(L_sub(Ltemp,Ltemp3),logLag);  // Ltemp=10*log10(lag*eng), Q23
  
  target[0]=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11
  
  /* Process high band */
  Ltemp=this->setEngyHarm_fx(4524, 13517, 4524, 16384, 1, 0, &Qh);
  Ltemp=log10_lut(Ltemp);
  Ltemp=L_add(L_sub(Ltemp,Ltemp3),logLag); // Ltemp=10*log10(lag*eng), Q23
  target[1]=round32_16(L_shl((Word32)Mpy_32_16(Ltemp,0x6666),1)); // Q11

  /* Need to unify the Q factors of both bands */
  Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
  n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
  if (n<0) this->rshiftHarmBand_fx(4524, 16384,n);
  else if (n>0) this->rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::quant_cw_fx ()                          */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Quantize QPPP prototype                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) pl: previous lag                                     */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices                           */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*   _ (Word16[]) lasterb: ERB history for differential              */
/*                quantization, Q13                                  */
/*   _ (Word16) Lgain: low band power history, log domain, Q11       */
/*   _ (Word16) Hgain: high band power history, log domain, Q11      */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*   _ (Word16) flag: flag indicating success/failure (TRUE/FALSE)   */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
Word16 DTFS_fx::quant_cw_fx(Word16 pl, Word16 *curr_lpc,
			    Word16& p_idx, Word16 *a_idx,
			    Word16 *sin_tab, Word16 *cos_tab)
{
  Word16 mfreq[NUM_ERB], curr_erb[NUM_ERB] ;
  Word16 erb_uq[NUM_ERB];
  Word16 tmp, n, j, slot[NUM_ERB], d1, d2, flag;
  Word16 w[2]; // Q15
  Word16 target[2]; // Q11
  Word32 Ltemp;
  Word32 minerror;
  Word40 Lacc;

#ifdef WMOPS_FX
      WMP_fwc_fx("");
#endif
  // Get weighting and target
  this->quant_target(curr_lpc, w, target, sin_tab, cos_tab);
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Target .....................................");
#endif
  //Power Quantization in log domain
  target[0]=sub(target[0],lastLgainE_fx);
  target[1]=sub(target[1],lastHgainE_fx);
  
  /* Power quantization search */
#ifdef WMOPS_FX
  move32();
  move16();
#endif
  minerror=LW_MAX;
  p_idx=0;
  for (j=0,n=0; n<P_CBSIZE*2; n+=2,j++) {
    //    n=shl(j,1); // n=offset to current codebook entry
    d1=sub(target[0],PowerCB_fx[n]);
    d2=sub(target[1],PowerCB_fx[n+1]);
    Ltemp=L_mult(w[0],abs_s(d1));
    Ltemp=L_mac(Ltemp,w[1],abs_s(d2)); // Ltemp=error
#ifdef WMOPS_FX
    test();
#endif
    if (d1>=0 && d2>=0) Ltemp=(Word32)Mpy_32_16(Ltemp,0x6666); // *=0.8
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub(Ltemp, minerror)<0) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
      minerror = Ltemp ;
      p_idx = j ;
    }
  }

#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Power Quant ................................");
#endif
  //Amplitude Quantization
  this->to_erb_fx(curr_erb); // output in Q13
  for (j=0; j<NUM_ERB; j++) erb_uq[j] = curr_erb[j]; // save it for bump-up decision
  erb_slot_fx(lag, slot, mfreq);
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant To ERB and Slot ............................");
#endif
  erb_diff_fx(pl, lag, lasterbE_fx, curr_erb, curr_lpc, a_idx);
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant ERB Diff Quant .......................");
#endif

  // Dequantization of prototype
  this->dequant_cw_fx(pl, p_idx, a_idx, lasterbE_fx, lastLgainE_fx, lastHgainE_fx, curr_erb);

#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Quant Dequant ..............................");
#endif
  //Determine if the amplitude quantization is good enough
  flag=TRUE;
  
  for (j=1, Ltemp=0, n=0;j<10;j++) {
#ifdef WMOPS_FX
    test();
#endif
    if (slot[j]!=0) {
      Ltemp=L_add(Ltemp,abs_s(sub(erb_uq[j],curr_erb[j]))); // Q13
      n=add(n,1); // n++
    }
  }
  Lacc=divide_dp(Ltemp,n,-11); // Lacc in Q31
  tmp=round32_16((Word32)L_sat32_40(Lacc)); // tmp in Q15
#ifdef WMOPS_FX
  test();
#endif
  if (sub(tmp,0x3C29)>0 && add(target[0],819)>0) flag = FALSE ; //Bumping up
  
#ifdef WMOPS_FX
      WMP_fwc_fx("QPPP AmpQuant Finish Quant ...............................");
#endif
  return flag;
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::dequant_cw_memless_fx()                 */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Dequantize FPPP prototype                        */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices, 3 words                  */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype                  */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS : _ None.                                        */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX/RX                                               */
/*===================================================================*/
void DTFS_fx::dequant_cw_memless_fx(Word16 p_idx, Word16 *a_idx)
{
  Word16 mfreq[NUM_ERB], curr_erb[NUM_ERB];
  Word16 slot[NUM_ERB] ;
  Word16 n, tmp;
  Word16 Ql, Qh;
  Word32 logLag;
  Word32 Ltemp;
  Word40 Lacc;

  //Amplitude Dequantization
  erb_dequant_fx(lag, a_idx, curr_erb);
  curr_erb[0] = mult_r(curr_erb[1],9830);  // *0.3
  curr_erb[20] = mult_r(curr_erb[19],9830);
  curr_erb[21] = 0;
  erb_slot_fx(lag, slot, mfreq);
  this->erb_inv_fx(curr_erb, slot, mfreq);  // erb->amplitude
  
  tmp=shl(p_idx,1);  // 2*p_idx for 2-split

  Ltemp=log10_lut(lag); // Ltemp=10*log10(lag), Q23
  logLag=(Word32)Mpy_32_16(Ltemp,0x6666); // logLag=log10(lag), Q26

  Ltemp=L_sub(L_shr(L_deposit_h(A_PowerCB_fx[tmp]),1),logLag); // Ltemp=Lgain-log10(lag), Q26
  Lacc=dsp_pow10(Ltemp); // input in Q26, Lacc=10^Lgain/lag, Q15
  n=norm32_l40(Lacc); 
  Ltemp=(Word32)L_shl40(Lacc,n); // Ltemp in Q(15+n)
  // restore low band energy
  this->setEngyHarm_fx(377, 4524, 0, 4524, Ltemp, add(n,15), &Ql);

  Ltemp=L_sub(L_shr(L_deposit_h(A_PowerCB_fx[tmp+1]),1),logLag); // Ltemp=Hgain-log10(lag), Q26
  Lacc=dsp_pow10(Ltemp); // input in Q26, Lacc=10^Hgain/lag, Q15
  n=norm32_l40(Lacc); 
  Ltemp=(Word32)L_shl40(Lacc,n); // Ltemp in Q(15+n)
  // restore high band energy
  this->setEngyHarm_fx(4524, 13517, 4524, 16384, Ltemp, add(n,15), &Qh);
  /* Need to unify the Q factors of both bands */
  Q=MIN_FX(Ql,Qh); // set Q factor to be the smaller one
  n=sub(Ql,Qh); // compare band Q factors
#ifdef WMOPS_FX
  test();
#endif
  if (n<0) this->rshiftHarmBand_fx(4524, 16384,n);
  else if (n>0) this->rshiftHarmBand_fx(0, 4524, sub(Qh,Ql));
}

/*===================================================================*/
/* FUNCTION      :  DTFS_fx::quant_cw_memless_fx ()                  */
/*-------------------------------------------------------------------*/
/* PURPOSE       :  Quantize FPPP prototype                          */
/*-------------------------------------------------------------------*/
/* INPUT ARGUMENTS  :                                                */
/*   _ (Word16 []) curr_lpc: LPC coefficients, Q12                   */
/*   _ (Word16 []) sin_tab: sine table based on lag, Q15             */
/*   _ (Word16 []) cos_tab: cosine table based on lag, Q15           */
/*-------------------------------------------------------------------*/
/* OUTPUT ARGUMENTS :                                                */
/*   _ (Word16) p_idx: Power index                                   */
/*   _ (Word16[]) a_idx: Amplitude indices, 3 words                  */
/*-------------------------------------------------------------------*/
/* INPUT/OUTPUT ARGUMENTS :                                          */
/*   _ (DTFS_fx) X :  prototype in polar domain                      */
/*                (Word16) lag: length of prototype in time domain   */
/*                (Word16 []) a: amplitude of harmonics, normalized  */
/*                (Word16) Q: norm factor of a                       */
/*-------------------------------------------------------------------*/
/* RETURN ARGUMENTS :                                                */
/*                    _ None                                         */
/*-------------------------------------------------------------------*/
/* CALLED FROM : TX                                                  */
/*===================================================================*/
void DTFS_fx::quant_cw_memless_fx(Word16 *curr_lpc,
				  Word16& p_idx, Word16 *a_idx,
				  Word16 *sin_tab, Word16 *cos_tab)
{
  Word16 mfreq[NUM_ERB], curr_erb[NUM_ERB] ;
  Word16 n, j, slot[NUM_ERB], d1, d2;
  Word16 w[2]; // Q15
  
  Word16 target[2]; // Q11
  Word32 Ltemp;
  Word32 minerror;
 

  // Get weighting and target
  this->quant_target(curr_lpc, w, target, sin_tab, cos_tab);
  
  /* Power quantization search */
#ifdef WMOPS_FX
  move32();
  move16();
#endif
  minerror=LW_MAX;
  p_idx=0;
  for (n=0, j=0; n<A_P_CBSIZE*2; n+=2, j++) {
    //    n=shl(j,1); // n=offset to current codebook entry
    d1=sub(target[0],A_PowerCB_fx[n]);
    d2=sub(target[1],A_PowerCB_fx[n+1]);
    Ltemp=L_mult(w[0],abs_s(d1));
    Ltemp=L_mac(Ltemp,w[1],abs_s(d2)); // Ltemp=error
#ifdef WMOPS_FX
    test();
#endif
    if (d1>=0 && d2>=0) Ltemp=(Word32)Mpy_32_16(Ltemp,0x6666); // *=0.8
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub(Ltemp, minerror)<0) {
#ifdef WMOPS_FX
  move32();
  move16();
#endif
      minerror = Ltemp ;
      p_idx = j ;
    }
  }

  //Amplitude Quantization
  this->to_erb_fx(curr_erb); // output in Q13
  erb_slot_fx(lag, slot, mfreq);
  erb_quant_fx(lag, curr_erb, curr_lpc, a_idx);
  // Prototype dequantization
  this->dequant_cw_memless_fx(p_idx, a_idx);
  
}

/*
 * The decimation-in-time complex FFT/IFFT is implemented below.
 * The input complex numbers are presented as real part followed by
 * imaginary part for each sample.  The counters are therefore
 * incremented by two to access the complex valued samples.
 */
static void c_fft(Word16 * farray_ptr, Word16 size, Word16 stage, Word16 isign)
{

	Word16 i, j, k, ii, jj, kk, ji, kj;
	Word32 ftmp, ftmp_real, ftmp_imag;
	Word16 tmp, tmp1, tmp2;
	Word16 n_2, K;
	Word16 ii_table[7];

#ifdef WMOPS_FX
  move16();
  move16();
#endif
	K=0;
	k=256;
#ifdef WMOPS_FX
  test();
#endif
	while (k>0) {
	  K=add(K,1); // K=512/size
	  k=sub(k,size);
	}
	n_2=shr(size,1);
	for (i=1;i<=stage;i++) ii_table[i-1]=shr(size,i);
/* Rearrange the input array in bit reversed order */
	for (i = 0, j = 0; i < size - 2; i = i + 2)
	{
#ifdef WMOPS_FX
        test();
		Nmove16(6);
#endif
		if (j > i)
		{
			ftmp = *(farray_ptr + i);
			*(farray_ptr + i) = *(farray_ptr + j);
			*(farray_ptr + j) = ftmp;

			ftmp = *(farray_ptr + i + 1);
			*(farray_ptr + i + 1) = *(farray_ptr + j + 1);
			*(farray_ptr + j + 1) = ftmp;
		}

#ifdef WMOPS_FX
  test();
  move16();
#endif
		k = n_2;
		while (j >= k)
		{
			j = sub(j, k);
			k = shr(k, 1);
		}
		j += k;
	}

/* The FFT part */
#ifdef WMOPS_FX
    test();
#endif
	if (isign == 1)
	{
		for (i = 0; i < stage; i++)
		{						/* i is stage counter */
			jj = shl(2, i);		/* FFT size */
			kk = shl(jj, 1);	/* 2 * FFT size */
#ifdef WMOPS_FX
  move16();
#endif
			ii = ii_table[i];	/* 2 * number of FFT's */

			for (j = 0; j < jj; j = j + 2)
			{					/* j is sample counter */
				ji = j * ii;	/* ji is phase table index */

				for (k = j; k < size; k = k + kk)
				{				/* k is butterfly top */
					kj = add(k, jj);	/* kj is butterfly bottom */

					/* Butterfly computations */
					ftmp_real = L_sub(L_mult(*(farray_ptr + kj), cos_table[ji*K]),
									  L_mult(*(farray_ptr + kj + 1), cos_table[(ji*K+384)%512]));
					ftmp_imag = L_add(L_mult(*(farray_ptr + kj + 1), cos_table[ji*K]),
									  L_mult(*(farray_ptr + kj), cos_table[(ji*K+384)%512]));

					tmp1 = round32_16(ftmp_real);
					tmp2 = round32_16(ftmp_imag);

					tmp = sub(*(farray_ptr + k), tmp1);
					*(farray_ptr + kj) = shr(tmp, 1);

					tmp = sub(*(farray_ptr + k + 1), tmp2);
					*(farray_ptr + kj + 1) = shr(tmp, 1);

					tmp = add(*(farray_ptr + k), tmp1);
					*(farray_ptr + k) = shr(tmp, 1);

					tmp = add(*(farray_ptr + k + 1), tmp2);
					*(farray_ptr + k + 1) = shr(tmp, 1);
				}
			}
		}

/* The IFFT part */
	}
	else
	{
		for (i = 0; i < stage; i++)
		{						/* i is stage counter */
			jj = shl(2, i);		/* FFT size */
			kk = shl(jj, 1);	/* 2 * FFT size */
			ii = ii_table[i];	/* 2 * number of FFT's */

			for (j = 0; j < jj; j = j + 2)
			{					/* j is sample counter */
				ji = j * ii;	/* ji is phase table index */

				for (k = j; k < size; k = k + kk)
				{				/* k is butterfly top */
					kj = add(k, jj);	/* kj is butterfly bottom */

					/* Butterfly computations */
					ftmp_real = L_add(L_mult(*(farray_ptr + kj), cos_table[ji*K]),
									  L_mult(*(farray_ptr + kj + 1), cos_table[(ji*K+384)%512]));
					ftmp_imag = L_sub(L_mult(*(farray_ptr + kj + 1), cos_table[ji*K]),
									  L_mult(*(farray_ptr + kj), cos_table[(ji*K+384)%512]));

					tmp1 = round32_16(ftmp_real);
					tmp2 = round32_16(ftmp_imag);

					*(farray_ptr + kj) = sub(*(farray_ptr + k), tmp1);
					*(farray_ptr + kj + 1) = sub(*(farray_ptr + k + 1), tmp2);
					*(farray_ptr + k) = add(*(farray_ptr + k), tmp1);
					*(farray_ptr + k + 1) = add(*(farray_ptr + k + 1), tmp2);
				}
			}
		}
	}

}								/* end of c_fft () */

static void r_fft(Word16 * farray_ptr, Word16 size, Word16 stage, Word16 isign)
{

	Word16 ftmp1_real, ftmp1_imag, ftmp2_real, ftmp2_imag;
	Word32 Lftmp1_real, Lftmp1_imag, Lftmp2_real, Lftmp2_imag;
	Word16 i, j;
	Word32 Ltmp1, Ltmp2;
	Word16 n_2, k, K;

	n_2=shr(size,1);
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	K=0;
	k=256;
	while (k>0) {
	  K=add(K,1); // K=512/size
	  k=sub(k,size);
	}
/* The FFT part */
#ifdef WMOPS_FX
    test();
	move16();
	move16();
#endif
	if (isign == 1)
	{
		/* Perform the complex FFT */
		c_fft(farray_ptr, size, stage, isign);

		/* First, handle the DC and foldover frequencies */
		ftmp1_real = *farray_ptr;
		ftmp2_real = *(farray_ptr + 1);
		*farray_ptr = add(ftmp1_real, ftmp2_real);
		*(farray_ptr + 1) = sub(ftmp1_real, ftmp2_real);

		/* Now, handle the remaining positive frequencies */
		for (i = 2, j = size - i; i <= n_2; i = i + 2, j = size - i)
		{
			ftmp1_real = add(*(farray_ptr + i), *(farray_ptr + j));
			ftmp1_imag = sub(*(farray_ptr + i + 1), *(farray_ptr + j + 1));
			ftmp2_real = add(*(farray_ptr + i + 1), *(farray_ptr + j + 1));
			ftmp2_imag = sub(*(farray_ptr + j), *(farray_ptr + i));

			Lftmp1_real = L_deposit_h(ftmp1_real);
			Lftmp1_imag = L_deposit_h(ftmp1_imag);
			Lftmp2_real = L_deposit_h(ftmp2_real);
			Lftmp2_imag = L_deposit_h(ftmp2_imag);

			Ltmp1 = L_sub(L_mult(ftmp2_real, cos_table[i*K]), L_mult(ftmp2_imag, cos_table[(i*K+384)%512]));
			*(farray_ptr + i) = round32_16(L_shr(L_add(Lftmp1_real, Ltmp1), 1));

			Ltmp1 = L_add(L_mult(ftmp2_imag, cos_table[i*K]), L_mult(ftmp2_real, cos_table[(i*K+384)%512]));
			*(farray_ptr + i + 1) = round32_16(L_shr(L_add(Lftmp1_imag, Ltmp1), 1));

			Ltmp1 = L_add(L_mult(ftmp2_real, cos_table[j*K]), L_mult(ftmp2_imag, cos_table[(j*K+384)%512]));
			*(farray_ptr + j) = round32_16(L_shr(L_add(Lftmp1_real, Ltmp1), 1));

			Ltmp1 = L_add(L_negate(L_mult(ftmp2_imag, cos_table[j*K])), L_mult(ftmp2_real, cos_table[(j*K+384)%512]));
			Ltmp2 = L_add(L_negate(Lftmp1_imag), Ltmp1);
			*(farray_ptr + j + 1) = round32_16(L_shr(Ltmp2, 1));
		}

	}
	else
	{

		/* First, handle the DC and foldover frequencies */
#ifdef WMOPS_FX
  move16();
  move16();
#endif
		ftmp1_real = *farray_ptr;
		ftmp2_real = *(farray_ptr + 1);
		*farray_ptr = shr(add(ftmp1_real, ftmp2_real), 1);
		*(farray_ptr + 1) = shr(sub(ftmp1_real, ftmp2_real), 1);

		/* Now, handle the remaining positive frequencies */
		for (i = 2, j = size - i; i <= n_2; i = i + 2, j = size - i)
		{
			ftmp1_real = add(*(farray_ptr + i), *(farray_ptr + j));
			ftmp1_imag = sub(*(farray_ptr + i + 1), *(farray_ptr + j + 1));
			ftmp2_real = negate(add(*(farray_ptr + j + 1), *(farray_ptr + i + 1)));
			ftmp2_imag = negate(sub(*(farray_ptr + j), *(farray_ptr + i)));

			Lftmp1_real = L_deposit_h(ftmp1_real);
			Lftmp1_imag = L_deposit_h(ftmp1_imag);
			Lftmp2_real = L_deposit_h(ftmp2_real);
			Lftmp2_imag = L_deposit_h(ftmp2_imag);

			Ltmp1 = L_add(L_mult(ftmp2_real, cos_table[i*K]), L_mult(ftmp2_imag, cos_table[(i*K+384)%512]));
			*(farray_ptr + i) = round32_16(L_shr(L_add(Lftmp1_real, Ltmp1), 1));

			Ltmp1 = L_sub(L_mult(ftmp2_imag, cos_table[i*K]), L_mult(ftmp2_real, cos_table[(i*K+384)%512]));
			*(farray_ptr + i + 1) = round32_16(L_shr(L_add(Lftmp1_imag, Ltmp1), 1));

			Ltmp1 = L_sub(L_mult(ftmp2_real, cos_table[j*K]), L_mult(ftmp2_imag, cos_table[(j*K+384)%512]));
			*(farray_ptr + j) = round32_16(L_shr(L_add(Lftmp1_real, Ltmp1), 1));

			Ltmp1 = L_negate(L_add(L_mult(ftmp2_imag, cos_table[j*K]), L_mult(ftmp2_real, cos_table[(j*K+384)%512])));
			Ltmp2 = L_add(L_negate(Lftmp1_imag), Ltmp1);
			*(farray_ptr + j + 1) = round32_16(L_shr(Ltmp2, 1));
		}

		/* Perform the complex IFFT */
		c_fft(farray_ptr, size, stage, isign);

	}
	
}

Word16 DTFS_fx::glbl_alignment_FPPP_fx(DTFS_fx X2)
{
  Word16 j, k, idx=0 ;
  Word16 x1[128], x2[256];
  Word40 maxcorr, corr;
  DTFS_fx X1 = *this;

  X1.fast_fs_inv_fx(x1,128,7) ;
  X2.fast_fs_inv_fx(x2,128,7) ;

#ifdef WMOPS_FX
  move32();
#endif
  maxcorr = LW_MIN;

  for (j=0; j<128; j++) {
    for (k=0,corr=0; k<128; k++) {
      corr=L_mac40(corr,x1[k],x2[(k-j+128)%128]);
    }
#ifdef WMOPS_FX
    test();
#endif
    if (L_sub40_40(corr,maxcorr)>0) {
#ifdef WMOPS_FX
  move16();
  move32();
#endif
      idx = j ;
      maxcorr = corr ;
    } 
  }
  return idx;
}


void DTFS_fx::fast_fs_inv_fx(Word16 *out, Word16 N, Word16 LOG2N)
{
  Word16 i, M_2, N_2, s;
  Word16 dbuf[256];

  M_2=shr(lag,1);
  N_2=shr(N,1);
  s=negate(Q);

#ifdef WMOPS_FX
  move16();
  move16();
#endif
  dbuf[0]=a[0];
  dbuf[1]=0;

  for (i=1;i<=M_2;i++) { dbuf[2*i]=shift_r(a[i],s); dbuf[2*i+1]=shift_r(b[i],s);}


  for ( ;i<N_2; i++) {
#ifdef WMOPS_FX
  move16();
  move16();
#endif
	  dbuf[2*i]=dbuf[2*i+1]=0;
  }

  // do IFFT
  r_fft(dbuf, N, sub(LOG2N,1), -1);
  
  for (i=0;i<N;i++) {
#ifdef WMOPS_FX
  move16();
#endif
	  out[i]=dbuf[i];
  }
}
