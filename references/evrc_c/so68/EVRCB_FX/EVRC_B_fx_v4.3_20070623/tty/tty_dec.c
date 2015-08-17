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
/*===================================================================*/
/* Conexant Systems Inc., Ericsson Inc., Lucent Technologies Inc.,   */
/* Motorola Inc., Nokia Inc, Nortel Networks Corporation, and        */
/* Qualcomm Inc. do hereby grant a free, irrevocable license to      */
/* 3GPP2 and its Organizational Partners (OPs) to incorporate text   */
/* or other copyrightable material contained in the above-referenced */
/* document and any modifications thereof in the creation of 3GPP2   */
/* publications; to copyright and sell in Organizational Partner's   */
/* name, any Organizational Partner's standards publication even     */
/* though it may include portions of the text or other copyrightable */
/* material contained in such document; and at the Organizational    */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such text or other copyrightable material or the       */
/* resulting Organizational Partner's standards publication.         */
/* Conexant Systems Inc., Ericsson Inc, Lucent Technologies Inc.,    */
/* Motorola Inc., Nokia Inc, Nortel Networks Corporation, and        */
/* Qualcomm Inc. do hereby agree that it will grant licenses under   */
/* any copyrights of such undersigned party to third parties on      */
/* reasonable, non-discriminatory terms and conditions.              */
/*                                                                   */
/*-------------------------------------------------------------------*/
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel or otherwise, any license or right under    */
/* any existing or later issuing patent, whether or not the use of   */ 
/* information in the document necessarily employs an invention of   */
/* any existing or later issued patent, or copyright, but Conexant   */
/* Systems Inc., Ericsson Inc., Lucent Technologies Inc., Motorola   */
/* Inc., Nokia Inc, Nortel Networks Corporation, and Qualcomm Inc.   */
/* will make a copyright license available as hereinabove previously */
/* stated.                                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*===================================================================*/

#include <stdio.h>
#include "tty.h"
#include "basic_op.h"

Word16    counter_hist[TTY_BUF_SIZE];
Word16    char_hist[TTY_BUF_SIZE];
Word16    tty_rate_hist[TTY_BUF_SIZE];

/*Word16    debug_frame_counter = 0; */   /* for debugging only */
unsigned long	tty_dec_char_count = 0;		/* for debugging only */

/***********************************************************************
*   init_tty_dec()
************************************************************************/
void init_tty_dec()
{
    Word16   i;
    extern Word16    stop_bit_len[NUM_TTY_RATES];
    extern Word16    data_bit_len[NUM_TTY_RATES];

#if TTY_CONT_PHASE_FIX
   extern Word16 tty_sin_tab_idx;    /* used by tty_continuous_phase_tone_gen() */
   tty_sin_tab_idx = 16;
#endif

    data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD;
    stop_bit_len[TTY_45_BAUD] = STOP_BIT_LEN_45_BAUD;
    data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD;
    stop_bit_len[TTY_50_BAUD] = STOP_BIT_LEN_50_BAUD;

    for( i=0 ; i < TTY_BUF_SIZE ; i++ )
    {
        char_hist[i] = 0;
        counter_hist[i] = NON_TTY;
        tty_rate_hist[i] = DEFAULT_TTY_BAUD_RATE;
    }

    init_tty_gen(NON_TTY,0,0);

}



/***********************************************************************
*   tty_dec()
************************************************************************/
Word16 tty_dec(
    Word16   buf[],
    Word16   acb_gain,
    Word16   tty_header,
    Word16   tty_char,
    Word16   tty_baud_rate,
    Word16   fer_flag,
    Word16   subframe,
    Word16   num_subfr,
    Word16   length
)
{
    Word16   counter;
    Word16   error;
    Word16   num;
    Word16   best_num;
    Word16   best_counter;
    Word16   best_char;
    Word16   best_rate;
    Word16   i;

    Word16   j;



  /* Convert the received header values */
  counter = tty_header;
  tty_header_in( &counter );

                                                    
  if( subframe == 0 )
  {

    if( sub(fer_flag,1) == 0 )
    {
        counter_hist[0] = TTY_FER;                          
        char_hist[0] = 0;                             	    
    }

    /* Detect if Baudot signal is being received */
    else if( acb_gain != 0 )
    {
                                                            
        counter_hist[0] = NON_TTY;                          
        char_hist[0] = 0;                                   
    }

    /* Sanity check the received information */
    else if( (sub(counter,TTY_SILENCE) == 0 && sub(tty_char,TTY_SILENCE_CHAR) != 0)
            || (sub(counter,TTY_ONSET) == 0 && sub(tty_char,TTY_ONSET_CHAR) != 0)
            || sub(counter,TTY_COUNTER_MAX) > 0
            || sub(counter,TTY_COUNTER_MIN) < 0
            || sub(tty_char,TTY_CHAR_MAX) > 0
            || sub(tty_char,TTY_CHAR_MIN) < 0 )
    {
                                                            
        counter_hist[0] = NON_TTY;                          
        char_hist[0] = 0;                                   
    }
    else
    {
                                                            
        counter_hist[0] = counter;                          
        char_hist[0] = tty_char;                            
        tty_rate_hist[0] = tty_baud_rate;                   
    }

    /*************************************************************
    *  The transition from NON_TTY to any TTY state must
    *  be voted on by the TTY FER handler.  Hence all transitions
    *  away from NON_TTY is treated as a FER
    *  This cuts down on false alarms.
    *************************************************************/
    if( (counter_hist[CURRENT_FRAME] & (TTY_SILENCE|TTY_ONSET|COUNTER_BETWEEN_START_STOP)) != 0
        && sub(counter_hist[CURRENT_FRAME+1],NON_TTY) == 0
      )
    {
        counter_hist[CURRENT_FRAME] = TTY_FER;              
        char_hist[CURRENT_FRAME] = 0;                       
    }

    /*--------------------------------------------------------------
    * Sanity check and correct FER in middle of character
    *---------------------------------------------------------------*/

    /*********************************************************************
    *  Check for the start of a new character detected or there is an FER
    *  at the beginning or end of a character.  If it is a new character,
    *  make sure the next 8 frames have the same counter and
    *  character value.  This corrects FERS in the middle of a
    *  character.  If there is an FER at the transition of a character,
    *  then use the lookahead frames to vote on the most likely character.
    ***********************************************************************/

    error = 0;                                          
                                                            
    if( ( ( sub(counter_hist[CURRENT_FRAME],counter_hist[CURRENT_FRAME+1]) != 0
            || sub(char_hist[CURRENT_FRAME],char_hist[CURRENT_FRAME+1]) != 0)
          && (counter_hist[CURRENT_FRAME] & COUNTER_BETWEEN_START_STOP) != 0 )
        || sub(counter_hist[CURRENT_FRAME],TTY_FER) == 0
      )
    {

        error = 1;                                          
    }
                                                            
        /*  If an error was detected above, correct it */
    if( error > 0 )
    {
        /* Look ahead 7 frames max for 50 baud */
        error = 7;                          
        if( tty_rate_hist[CURRENT_FRAME+1] == 0 )
        {
            /* Look ahead 8 frames max for 45.45 baud */
            error = 8;                      
        }

        best_num = 2;                                    
        best_counter = counter_hist[CURRENT_FRAME+1];    
        best_char = char_hist[CURRENT_FRAME+1];          
        best_rate = tty_rate_hist[CURRENT_FRAME+1];      
        for( i=CURRENT_FRAME+1 ; i > sub(CURRENT_FRAME,error) ; i-- )
        {
                                                         
            /* Exclude FERs from winning */
            if( sub(counter_hist[i],TTY_FER) != 0 )
            {
                num = 0;                                 
                for( j=CURRENT_FRAME+1 ; j > CURRENT_FRAME-error ; j-- )
                {
                                                         
                    if( sub(counter_hist[i],counter_hist[j]) == 0
                        && sub(char_hist[i],char_hist[j]) == 0
                        && sub(tty_rate_hist[i],tty_rate_hist[j]) == 0 )
                    {
                        num = add(num,1);                           
                    }
                }
                                                         
                if( sub(num,best_num) > 0 )
                {
                    best_num = num;                      
                    best_counter = counter_hist[i];      
                    best_char = char_hist[i];            
                    best_rate = tty_rate_hist[i];      
                }
            }
        }
                                                            
        /* If best guess is silence */
        if( ((best_counter & (TTY_SILENCE|NON_TTY|TTY_EIGHTH_RATE|TTY_ONSET)) != 0)
            || ( sub(best_counter,counter_hist[CURRENT_FRAME+1]) == 0
                 && sub(best_char,char_hist[CURRENT_FRAME+1]) == 0 )
          )
        {
            /* Fix only this FER */
            counter_hist[CURRENT_FRAME] = best_counter; 
            char_hist[CURRENT_FRAME] = best_char;       
            tty_rate_hist[CURRENT_FRAME] = best_rate;               
        }
        else
        {   /* else fix a character's worth of frames */
 
	        if( best_rate == 0 )
            {
                /* Correct min of 7 frames for 45.45 baud */
                error = sub(CURRENT_FRAME,7);           
            }
            else
            {
                /* Correct min of 6 frames 50 baud */
                error = sub(CURRENT_FRAME,6);
            }

            for( i=CURRENT_FRAME ; i >= 0 ; i-- )
            {
                                                            
                if( sub(i,error) > 0
                    || sub(counter_hist[i],best_counter) == 0
                    || (counter_hist[i] & (TTY_FER|NON_TTY)) != 0 )
                {
                    counter_hist[i] = best_counter;         
                    char_hist[i] = best_char;               
                    tty_rate_hist[i] = best_rate;           
                }
                else if( sub(counter_hist[i],best_counter) != 0
                     && (counter_hist[i] & COUNTER_BETWEEN_START_STOP) != 0 )
                {
                    break;
                }
            }
        }
    }

                                                            
    /* if eighth rate, keep doing what was done in the last frame */
    if( sub(counter_hist[CURRENT_FRAME],TTY_EIGHTH_RATE) == 0 )
    {
        counter_hist[CURRENT_FRAME] = counter_hist[CURRENT_FRAME+1];    
        char_hist[CURRENT_FRAME] = char_hist[CURRENT_FRAME+1];          
        tty_rate_hist[CURRENT_FRAME] = tty_rate_hist[CURRENT_FRAME+1];          
    }

    /*****************************************************************
    * If NON_TTY is coming and there is trailing silence, get rid
    * of the silence.  This helps return to NON_TTY if there was
    * a false alarm
    ******************************************************************/

                                                            
    if( sub(counter_hist[0],NON_TTY) == 0 )
    {
        for( i=1 ; i <= CURRENT_FRAME ; i++ )
        {
                                                            
            if( counter_hist[i] == TTY_SILENCE )
            {
                counter_hist[i] = NON_TTY;                  
            }
            else
            {
                break;
            }
        }
    }


  } /* end if(subframe == 0) */
  else
  {
                                                        
    /* If acb_gain is non-zero at any time in the frame, declare NON_TTY */
    if( acb_gain != 0 && !fer_flag )
    {
        counter_hist[0] = NON_TTY;                      
        char_hist[0] = 0;                               
    }
  }


  /* Generate the Baudot signal */
  i = tty_gen( buf,
               length,
               subframe,
               num_subfr,
               counter_hist,
               char_hist,
               tty_rate_hist );




  return(i);

} /* end tty_dec() */

