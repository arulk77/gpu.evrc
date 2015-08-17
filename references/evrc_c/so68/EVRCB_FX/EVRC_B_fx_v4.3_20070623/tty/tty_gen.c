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
#include <math.h>
#include "tty.h"
#include "basic_op.h"

#define STOP_BIT_MASK       0x00C0
#define DATA_BIT_MASK       0x001F

#define STOP_BIT_NUM        6
#define MARK_HOLD_BIT_NUM   7

#if TTY_HALF_DUPLEX_FIX
#define SILENCE_HANGOVER_LEN       512    /* 64 ms. */
#define SILENCE_HANGOVER_BIT_NUM     8
#endif

Word16    bit_num;
Word16    bit_size;
Word16    current_counter;
Word16    current_char;

#if TTY_CONT_PHASE_FIX
extern short    tty_sin_tab_idx;    /* used by tty_continuous_phase_tone_gen() */
#else
Word16    prev_bit;
Word16    tone_param[3];
#endif

Word16   stop_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */
Word16   data_bit_len[NUM_TTY_RATES];    /* initialized in init_tty_dec() & init_tty_rxtx() */

/*------------------------------------------------------------------
*                       init_tty_gen()
*-------------------------------------------------------------------*/
void init_tty_gen(Word16 counter, Word16 tty_char, Word16 tty_baud_rate)
{

    bit_size = data_bit_len[tty_baud_rate];                 
    bit_num = 0;                                            
    current_counter = counter;                              
    current_char = shl((tty_char & DATA_BIT_MASK),1) | STOP_BIT_MASK;      

#if TTY_CONT_PHASE_FIX
    if( (counter & (NON_TTY|TTY_SILENCE)) != 0 )
    {
        /* Reset index if there is a break between characters */
        tty_sin_tab_idx = 16;       /* used in tty_continuous_phase_tone_gen() */
    }
#else
    prev_bit = MARK_FREQ;
#endif
}

/*------------------------------------------------------------------
*                       tty_gen()
*-------------------------------------------------------------------*/
Word16 tty_gen(
    Word16   outbuf[],           /* (o): output buffer                   */
    Word16   length,             /* (i): length of buffer                */
    Word16   subframe,           /* (i): subframe counter                */
    Word16   num_subfr,          /* (i): number of subframes/frame       */
    Word16   counter_hist[],     /* (i/o): tty counter history           */
    Word16   char_hist[],        /* (i/o): tty character history         */
    Word16   tty_rate_hist[]     /* (i/o): tty baud rate history         */
)
{
    Word16   i;
    Word16   num;
    Word16   bit;
    Word16   *p_buf;
    Word16   write_flag;



    bit = 0;    /* eliminates compiler warning */

                                                            
    /* Change onset to silence */
    if( sub(counter_hist[CURRENT_FRAME],TTY_ONSET) == 0 )
    {
        counter_hist[CURRENT_FRAME] = TTY_SILENCE;          
        char_hist[CURRENT_FRAME] = TTY_SILENCE_CHAR;        
    }

                                                            
    /* If transition from silence to anything else */
    if( (current_counter & (TTY_SILENCE|NON_TTY)) != 0
        && sub(counter_hist[CURRENT_FRAME],TTY_SILENCE) != 0
      )
    {
        init_tty_gen( counter_hist[CURRENT_FRAME],
                      char_hist[CURRENT_FRAME],
                      tty_rate_hist[CURRENT_FRAME]);
    }

    /* else if transition from NON_TTY to TTY_SILENCE */

    else if( sub(current_counter,NON_TTY) == 0
             && sub(counter_hist[CURRENT_FRAME],TTY_SILENCE) == 0
           )
    {
                                                            
        init_tty_gen( TTY_SILENCE, TTY_SILENCE_CHAR, 0);
    }

    /* else if a new character is coming before the old one is finished */
    else if( subframe == 0
             && sub(current_counter,counter_hist[CURRENT_FRAME]) != 0
             && (counter_hist[CURRENT_FRAME] & COUNTER_BETWEEN_START_STOP) != 0
           )
    {

#if TTY_HALF_DUPLEX_FIX
        if( sub(bit_num,MARK_HOLD_BIT_NUM) >= 0 )
#else
        if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
#endif
        {
            /* Cut short the mark hold tone to start new character */
            init_tty_gen( counter_hist[CURRENT_FRAME],
                          char_hist[CURRENT_FRAME],
                          tty_rate_hist[CURRENT_FRAME]);
        }
        else if( sub(bit_num,STOP_BIT_NUM) == 0
                 && sub(bit_size,FRAMESIZE) >= 0
               )
        {
            /* Shorten the stop bit to 1 bit length if it will be done this frame */

#if TTY_GEN_50_BAUD_FIX
#if TTY_NOMINAL_BIT_LEN_FIX
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-3; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-3; 
#else
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-4; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-4; 
#endif /* TTY_NOMINAL_BIT_LEN_FIX */
#endif /* TTY_GEN_50_BAUD_FIX */
            bit_size = add(bit_size,data_bit_len[tty_rate_hist[CURRENT_FRAME]]);
            bit_size = sub(bit_size,stop_bit_len[tty_rate_hist[CURRENT_FRAME]]);

        }
        else if( sub(bit_num,STOP_BIT_NUM) < 0 )
        {
            /*
            *  Shorten to 1 stop bit if next character comes before
            *  generating stop bit of current character.
            */

#if TTY_GEN_50_BAUD_FIX
#if TTY_NOMINAL_BIT_LEN_FIX
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-3; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-3; 
#else
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD-4; 
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD-4; 
#endif /* TTY_NOMINAL_BIT_LEN_FIX */
#endif /* TTY_GEN_50_BAUD_FIX */
            stop_bit_len[tty_rate_hist[CURRENT_FRAME]] = data_bit_len[tty_rate_hist[CURRENT_FRAME]];
        }

    } /* end if( falling behind) */

    p_buf = outbuf;                                         
    num = length;                                           
                                                            
    if( sub(bit_size,length) < 0 )
    {
        num = bit_size;                                     
    }

#if TTY_HALF_DUPLEX_FIX
    write_flag = 1;
#else
    write_flag = 0;
#endif

    while( num > 0 )
    {

        /*-----------------------------------------------------------------------*/
        /*-----------------------------------------------------------------------*/
                                                            
        /* If current character non_tty, get out of loop */
        if( sub(current_counter,NON_TTY) == 0 )
        {
              init_tty_gen(NON_TTY,0,0);
#if TTY_HALF_DUPLEX_FIX
              write_flag = 0;
#endif
              break;
        }

        /* else if silence, generate silence */
        else if( sub(current_counter,TTY_SILENCE) == 0 )
        {
                                                            
            for( i=0 ; i < num ; i++ )
            {
                p_buf[i] = 0;                               
            }
            init_tty_gen(TTY_SILENCE,TTY_SILENCE_CHAR,0);         /* init in preparation for next char */
#if !TTY_HALF_DUPLEX_FIX
            write_flag = 1;
#endif
        }
        else
        {
#if TTY_HALF_DUPLEX_FIX
            if( bit_num == SILENCE_HANGOVER_BIT_NUM )
            {
                for( i=0 ; i < num ; i++ )
                {
                    p_buf[i] = 0;
                }
            }
            else
            {
                bit = current_char & shl(1,bit_num);            
                                                           
                if( bit == 0 )
                {
                    bit = SPACE_FREQ;
                }
                else
                {
                    bit = MARK_FREQ;
                }

#if TTY_CONT_PHASE_FIX
                tty_continuous_phase_tone_gen(p_buf,bit,num);
#else
                if( sub(bit,prev_bit) != 0 )
                {
                    init_tone_gen(tone_param,bit);
                }

                tone_gen(p_buf,bit,BAUDOT_GAIN,num,tone_param);
#endif /* TTY_CONT_PHASE_FIX */
            }
            write_flag = 2;

#else /* !TTY_HALF_DUPLEX_FIX */
            bit = current_char & shl(1,bit_num);            
                                                            
            if( bit == 0 )
            {
                bit = SPACE_FREQ;                           
            }
            else
            {
                bit = MARK_FREQ;                            
            }

#if TTY_CONT_PHASE_FIX
            tty_continuous_phase_tone_gen(p_buf,bit,num);
#else /* TTY_CONT_PHASE_FIX */
            if( sub(bit,prev_bit) != 0 )
            {
                init_tone_gen(tone_param,bit);
            }

            tone_gen(p_buf,bit,BAUDOT_GAIN,num,tone_param);
#endif /* TTY_CONT_PHASE_FIX */
            write_flag = 1;
#endif /* HALF_DUPLEX_FIX */

            /* Update for next iteration */

            p_buf += num; 
            bit_size = sub(bit_size,num);                                
                                                            
            if( sub(bit_size,1) < 0 )                 /* if end of bit */
            {
                bit_num = add(bit_num,1);                                  

                /*
                *  If a new character immediately follows the
                *  current character, eliminate the mark hold tone.
                */
#if TTY_HALF_DUPLEX_FIX
                if( sub(bit_num,MARK_HOLD_BIT_NUM) >= 0 )
#else
                if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
#endif
                {
                    /* Reset stop bit to 1.5 bits */

                    stop_bit_len[TTY_45_BAUD] = STOP_BIT_LEN_45_BAUD;
                    stop_bit_len[TTY_50_BAUD] = STOP_BIT_LEN_50_BAUD;

                    if( sub(current_counter,counter_hist[CURRENT_FRAME]) != 0
                        && (counter_hist[CURRENT_FRAME] & COUNTER_BETWEEN_START_STOP) != 0
                    )
                    {
#if TTY_HALF_DUPLEX_FIX
                        bit_num = SILENCE_HANGOVER_BIT_NUM+1;
#else
                        bit_num = add(bit_num,1);
#endif
                    }
#if TTY_HALF_DUPLEX_FIX
                    else
                    {
                        /* Remove current_counter from history */
                        for( i=CURRENT_FRAME-1 ; i >= 0 ; i-- )
                        {
                            if( current_counter == counter_hist[i] )
                            {
                                /* Replace with TTY_FER so that a vote will be taken */
                                counter_hist[i] = TTY_FER; 
                                char_hist[i] = 0; 
                            }
                            else
                            {
                                break;
                            }                                
                        }
                    }
#endif
                }

#if TTY_HALF_DUPLEX_FIX
                if( sub(bit_num,SILENCE_HANGOVER_BIT_NUM) > 0 )        /* if end of character */
#else
                if( sub(bit_num,MARK_HOLD_BIT_NUM) > 0 )               /* if end of character */
#endif
                {
                                                            
                    /* if start of silence */
                    if( sub(current_counter,counter_hist[CURRENT_FRAME]) == 0
                        || sub(counter_hist[CURRENT_FRAME],NON_TTY) == 0 )
                    {
                        counter_hist[CURRENT_FRAME] = TTY_SILENCE;      
                        char_hist[CURRENT_FRAME] = TTY_SILENCE_CHAR;    

                        init_tty_gen(TTY_SILENCE,TTY_SILENCE_CHAR,0);
                    }

                    /* else... next character begins right away. */
                    else                        /* else start next character */
                    {
                        init_tty_gen( counter_hist[CURRENT_FRAME],
                                      char_hist[CURRENT_FRAME],
                                      tty_rate_hist[CURRENT_FRAME]);
                    }
                }
                else                            /* start of next bit */
                {
                                                            
                    if( sub(bit_num,STOP_BIT_NUM) == 0 )
                    {
                        bit_size = stop_bit_len[tty_rate_hist[CURRENT_FRAME+1]];            
                    }
                    else if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
                    {
                                                            
                        bit_size = MARK_HOLD_LEN;           
                    }
#if TTY_HALF_DUPLEX_FIX
                    else if( sub(bit_num,SILENCE_HANGOVER_BIT_NUM) == 0 )
                    {
                        bit_size = SILENCE_HANGOVER_LEN;
                    }
#endif
                    else
                    {
                                                            
                        bit_size = data_bit_len[tty_rate_hist[CURRENT_FRAME+1]]; 
                    }

                }
            }
        } /* end if */


        /* Update for next iteration */
#if !TTY_CONT_PHASE_FIX
        prev_bit = bit;
#endif
        length = sub(length,num); 
        num = length;                                       
                                                            
        if( sub(bit_size,length) < 0 )
        {
            num = bit_size;                                 
        }

    } /* end while */


    /* Update history buffers for next iteration at the end of the frame */
                                                            
    if( sub(subframe,sub(num_subfr,1)) == 0 )
    {

        /* Put last generated character in history buffers */
                                                            
        if( sub(counter_hist[CURRENT_FRAME],current_counter) != 0 )
        {
            tty_rate_hist[CURRENT_FRAME] = tty_rate_hist[CURRENT_FRAME+1];  
        }
        counter_hist[CURRENT_FRAME] = current_counter;      
        char_hist[CURRENT_FRAME] = shr(current_char,1) & DATA_BIT_MASK;     

        for( i=TTY_BUF_SIZE-1 ; i > 0 ; i-- )
        {
            counter_hist[i] = counter_hist[i-1];            
            char_hist[i] = char_hist[i-1];                  
            tty_rate_hist[i] = tty_rate_hist[i-1];          
        }

        /* During the mark hold, allow the next character to be generated */
#if TTY_HALF_DUPLEX_FIX
        if( sub(bit_num,MARK_HOLD_BIT_NUM) >= 0 )
#else
        if( sub(bit_num,MARK_HOLD_BIT_NUM) == 0 )
#endif
        {
#if TTY_GEN_50_BAUD_FIX
            stop_bit_len[TTY_45_BAUD] = STOP_BIT_LEN_45_BAUD;
            data_bit_len[TTY_45_BAUD] = DATA_BIT_LEN_45_BAUD;
            stop_bit_len[TTY_50_BAUD] = STOP_BIT_LEN_50_BAUD;
            data_bit_len[TTY_50_BAUD] = DATA_BIT_LEN_50_BAUD;
#endif
            for( i=CURRENT_FRAME ; i >= 0 ; i-- )
            {
                                                            
                /* Remove current_counter from history */
                if( current_counter == counter_hist[i] )
                {                                                            
                    counter_hist[i] = TTY_FER;      
                    char_hist[i] = 0;    
                }
                else
                {
                    break;
                }                                
            }
        }

    }



    return(write_flag);
        
} /* end tty_gen() */

