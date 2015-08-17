/*======================================================================*/
/*     Enhanced Variable Rate Codec - Master C code Specification       */
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

This README file contains:
    1. Directory Tree Structure and Naming Conventions
    2. Subjective Test Summary
    3. Fixed Point Bit-Exact Test Vectors Description and Instructions

Note : All files are in 'PC' format.


************************************************************************
*                                                                      *
*        Directory Tree Structure and Naming Conventions               *
*                                                                      *
************************************************************************

+--- simul
|       +--- master
|       |       +-- code
|       |       |       +-- makefile
|       |       |       `-- *.c
|       |       +-- include
|       |       |       `-- *.h	*.dat
|       |       +-- bin
|       |       |       
|       |       `-- test
|       |               +-- mstr_ref.pcm           Reference PCM File
|       |               +-- mstr_ref.pkt           Reference EVRC Packet
|       |               `-- mstr_ref.dec           Reference EVRC Decoded Speech
|       +--- fixed
|               +-- code
|               |       +-- makefile
|               |       `-- *.c
|               +-- include
|               |       `-- *.h	*.dat
|               +-- bin
|               |       
|               +-- dspmath
|               |        `-- *.c *.h makefile 
|               |
|               `-- test
|                       +-- fixed31         31 Bit Long Multiply Reference Files
|                       |      +-- *.pkt        Reference EVRC Packet
|                       |      `-- *.dec        Reference EVRC Decoded Speech
|                       +-- fixed32         32 Bit Long Multiply Reference Files
|                       |      +-- *.pkt        Reference EVRC Packet
|                       |      `-- *.dec        Reference EVRC Decoded Speech
|                       `-- source          Source Reference Files
|                              `-- *.pcm        Reference PCM File
+-- subjctv
|       +-- exp1                        Experiment 1
|       |       +-- source                  Source PCM Files
|       |       |       +-- *.s12               High Audio Level    -12 dB
|       |       |       +-- *.s22               Clean Nominal Level -22 dB
|       |       |       `-- *.s32               Low Audio Level     -32 dB
|       |       +-- ref                     Reference PCM Files
|       |       |       +-- *.q05               MNRU  5 dBQ
|       |       |       +-- *.q15               MNRU 15 dBQ
|       |       |       +-- *.q20               MNRU 20 dBQ
|       |       |       +-- *.q25               MNRU 25 dBQ
|       |       |       +-- *.728               G.728 Processed
|       |       |       +-- *.qc1               IS-96A Clean Speech
|       |       |       +-- *.qc2               IS-96A High Level
|       |       |       +-- *.qc3               IS-96A Low Level
|       |       |       +-- *.qf3               IS-96A 3% Forward Link FER
|       |       |       +-- *.qr3               IS-96A 3% Reverse Link FER
|       |       |       `-- *.qc5               IS-96A 1/2 Rate Max
|       |       +-- m_pkt                   EVRC Master Encoded Packets
|       |       |       +-- *.p12               High Audio Level    -12 dB    
|       |       |       +-- *.p22               Clean Nominal Level -22 dB
|       |       |       +-- *.p32               Low Audio Level     -32 dB
|       |       |       +-- *.pf3               3% Forward Link FER
|       |       |       +-- *.pr3               3% Reverse Link FER
|       |       |       `-- *.phr               Half Rate Max
|       |       +-- m_m                     EVRC Master/Master Decoded Speech
|       |       |       +-- *.mm1               Clean Nominal Level -22 dB
|       |       |       +-- *.mm2               High Audio Level    -12 dB
|       |       |       +-- *.mm3               Low Audio Level     -32 dB
|       |       |       +-- *.mm4               3% Forward Link FER
|       |       |       +-- *.tm4               3% Reverse Link FER
|       |       |       `-- *.mm5               Half Rate Max
|       |       `-- data                    Play Lists      
|       |               +-- play1.lst
|       |               +-- play2.lst
|       |               +-- play3.lst
|       |               +-- play4.lst
|       |               +-- play5.lst
|       |               +-- play6.lst
|       |               +-- play7.lst
|       |               `-- play8.lst
|       |
|       `-- exp2                        Experiment 2
|               +-- source                  Source PCM Files
|               |       +-- *.s22               Clean Speech        @ -22 dB
|               |       +-- *.car               Car Noise (IRS)     @  15 dB SNR
|               |       +-- *.str               Street Noise (flat) @  12 dB SNR
|               |       `-- *.bab               Office Babble (flat)@  20 dB SNR
|               +-- ref                     Reference PCM Files
|               |       +-- *.q05               MNRU  5 dBQ
|               |       +-- *.q15               MNRU 15 dBQ
|               |       +-- *.q20               MNRU 20 dBQ
|               |       +-- *.q25               MNRU 25 dBQ
|               |       +-- *.728               G.728 Processed
|               |       +-- *.qc1               IS-96A Clean Speech
|               |       +-- *.qc2               IS-96A Car Noise
|               |       +-- *.qc3               IS-96A Street Noise
|               |       +-- *.qc4               IS-96A Office Babble
|               |       `-- *.qc5               IS-96A Tandem
|               +-- m_pkt                   EVRC Master Encoded Packets
|               |       +-- *.p22               Clean Speech
|               |       +-- *.pc                Car Noise
|               |       +-- *.ps                Street Noise
|               |       `-- *.pb                Office Babble
|               +-- m_m                     EVRC Master/Master Decoded Speech
|               |       +-- *.mm1               Clean Speech
|               |       +-- *.mm2               Car Noise
|               |       +-- *.mm3               Street Noise
|               |       +-- *.mm4               Office Babble
|               |       `-- *.mm5               Tandem
|               `-- data                    Play Lists
|                       +-- play1.lst
|                       +-- play2.lst
|                       +-- play3.lst
|                       +-- play4.lst
|                       +-- play5.lst
|                       +-- play6.lst
|                       +-- play7.lst
|                       `-- play8.lst
+-- objctv                                  Objective Test Source Files
|       `-- *.s                                 Avg. Rate Test Source Files
+-- cal                                     Calibration Test Source File
|       `-- cos1004.290                         1004 Hz. Tone @ -17.9 dBm0
+-- tools
|       +-- makefile
|       +-- bin
|       +-- avg_rate
|       |       +-- makefile
|       |       `-- avg_rate.c
|       +-- l_mu_l
|       |       +-- makefile
|       |       +-- *.c
|       |       `-- *.h
|       `-- sv56
|               +-- makefile
|               +-- *.c
|               `-- *.h
|
+-- testvec                             Bit-Exact Test Vectors
        +-- fixed31                         31 Bit Long Multiply Test Vectors
        |       +-- *.pkt                       Encoded Packets  
        |       `-- *.dec                       Decoded Speech
        +-- fixed32                         32 Bit Long Multiply Test Vectors
        |       +-- *.pkt                       Encoded Packets  
        |       `-- *.dec                       Decoded Speech
        +-- source                          Source Test Vectors
                +-- *.pcm                       Original PCM Speech Files                       
                +-- *_1.pkt                     Encoded packets w/ 1% FER
                +-- *_2.pkt                     Encoded packets w/ 2% FER
                `-- *_3.pkt                     Encoded packets w/ 3% FER



************************************************************************
*                                                                      *
*                  SUBJECTIVE TEST SUMMARY                             *
*                                                                      *
************************************************************************


Condition              | Experiment 1          | Experiment 2
========================================================================
Type of test           | MOS                   | MOS
-----------------------+-----------------------+------------------------
Number of talkers      | 4 males, 4 females    | 4 males, 4 females
-----------------------+-----------------------+------------------------
Background noise       | none (ambient)        | ambient and specified
                       |                       | test conditions
-----------------------+-----------------------+------------------------
Audio Input Level      | -22 dB (except for    | -22 dB
                       | high/low input        |
                       | condition)            |
-----------------------+-----------------------+------------------------
Filter Characteristics | IRS                   | Flat voice
-----------------------+-----------------------+------------------------
Reference Conditions   | u-law source          | u-law source
                       | 5,15,20,25 dBQ        | 5,15,20,25 dBQ
                       | G.728                 | G.728
-----------------------+-----------------------+------------------------
Test Conditions        | 1. Clean              | 1. Clean
                       | 2. High Audio Input   | 2. Car Noise (IRS)
                       |    Level -12 dB       |    @ 15 dB SNR
                       | 3. Low Audio Input    | 3. Street Noise (flat)
                       |    Level -32 dB       |    @ 12 dB SNR
                       | 4. 3% FER             | 4. Office Babble (flat)
                       |                       |    @ 20 dB SNR
                       | 5. 1/2 Rate Max       | 5. Tandem
-----------------------+-----------------------+------------------------
Number of Codecs       | (5) M/M, T/M, M/T,    | (5) M/M, T/M, M/T,
                       |     T/T, IS-96A       |     T/T, IS-96A
-----------------------+-----------------------+------------------------
Encoding Stages        | single                | tandem
------------------------------------------------------------------------



************************************************************************
*                                                                      *
*  FIXED POINT BIT-EXACT TEST VECTORS DESCRIPTION AND INSTRUCTIONS     *
*                                                                      *
************************************************************************

File Naming Convention

   source speech:   *.pcm
   encoder output:  *.pkt
   decoder output:  *.dec

   The directory "fixed31" corresponds to files processed using the
   31 bit long multiply fixed point DSP math library.  Likewise, the
   directory "fixed32" corresponds to files processed using the 32 bit
   long multiply fixed point DSP math library


1) Test vectors to exercise majority of bitstream slots.
        vec_01.pcm    15dB babble   7 females, 7 males
        vec_02.pcm    10dB car      7 females, 7 males
        vec_03.pcm    flat clean    7 females, 7 males
        vec_04.pcm    15dB street   7 females, 7 males
        vec_05.pcm    high level    4 females, 4 males
        vec_06.pcm    low level     4 females, 4 males
        vec_07.pcm    irs clean     4 females, 4 males
        vec_08.pcm    flat clean    4 females, 4 males
        vec_09.pcm    10dB car      4 females, 4 males
        vec_10.pcm    15dB babble   4 females, 4 males
        vec_11.pcm    12dB street   4 females, 4 males
        vec_12.pcm    mixed noise one-sided conversation
        vec_13.pcm    mixed noise one-sided conversation

2) Test vectors to exercise RCELP algorithm.
   NOTE: Must be set to run in full-rate only (only rate 4 allowed).
        shiftr.pcm    Frequency-sweeping sine waves
        shiftl.pcm    Frequency-sweeping sine waves
   
3) Test vectors w/ one-sided conversation to test rate determination.
        rda_test
        rda_mod
        rda_high 
        rda_low 

4) Test vectors to test frame erasures:
        vec_07_1.pkt         Encoded packet w/ 1% FER
        vec_07_2.pkt         Encoded packet w/ 2% FER
        vec_07_3.pkt         Encoded packet w/ 3% FER
        vec_08_1.pkt         Encoded packet w/ 1% FER
        vec_08_2.pkt         Encoded packet w/ 2% FER
        vec_08_3.pkt         Encoded packet w/ 3% FER
        vec_10_1.pkt         Encoded packet w/ 1% FER
        vec_10_2.pkt         Encoded packet w/ 2% FER
        vec_10_3.pkt         Encoded packet w/ 3% FER


PROCESS FILES AS FOLLOWS:

PCM source files to be processed in DEFAULT MODE (rates 1,3,4 allowed)
    Encode:     EvrcFix -e -i file.pcm -o file.pkt
    Decode:     EvrcFix -d -i file.pkt -o file.dec

    PCM Source File     Encoded Packet File     Decoded Speech File
    ===============     ===================     ===================
    rda_high.pcm        rda_high.pkt            rda_high.dec
    rda_low.pcm         rda_low.pkt             rda_low.dec
    rda_mod.pcm         rda_mod.pkt             rda_mod.dec
    rda_test.pcm        rda_test.pkt            rda_test.dec
    vec_01.pcm          vec_01.pkt              vec_01.dec
    vec_02.pcm          vec_02.pkt              vec_02.dec
    vec_03.pcm          vec_03.pkt              vec_03.dec
    vec_04.pcm          vec_04.pkt              vec_04.dec
    vec_05.pcm          vec_05.pkt              vec_05.dec
    vec_06.pcm          vec_06.pkt              vec_06.dec
    vec_07.pcm          vec_07.pkt              vec_07.dec
    vec_08.pcm          vec_08.pkt              vec_08.dec
    vec_09.pcm          vec_09.pkt              vec_09.dec
    vec_10.pcm          vec_10.pkt              vec_10.dec
    vec_11.pcm          vec_11.pkt              vec_11.dec
    vec_12.pcm          vec_12.pkt              vec_12.dec
    vec_13.pcm          vec_13.pkt              vec_13.dec


PCM source files to be processed in HALF RATE MAX MODE (rates 1,3 allowed)
    Encode:     EvrcFix -e -h 3 -i file.pcm -o file_h.pkt
    Decode:     EvrcFix -d -i file_h.pkt -o file_h.dec

    PCM Source File     Encoded Packet File     Decoded Speech File
    ===============     ===================     ===================
    vec_05.pcm          vec_05_h.pkt            vec_05_h.dec
    vec_06.pcm          vec_06_h.pkt            vec_06_h.dec
    vec_08.pcm          vec_08_h.pkt            vec_08_h.dec


PCM source files to be processed in FULL RATE ONLY (rate 4 only)
    Encode:     EvrcFix -e -l 4 -i file.pcm -o file.pkt
    Decode:     EvrcFix -d -i file.pkt -o file.dec

    PCM Source File     Encoded Packet File     Decoded Speech File
    ===============     ===================     ===================
    shiftl.pcm          shiftl.pkt              shiftl.dec
    shiftr.pcm          shiftr.pkt              shiftr.dec


Packets to be decoded
    Decode:     EvrcFix -d -i file.pkt -o file.dec

    Packet Source File  Decoded Speech File
    ==================  ===================
    vec_07_1.pkt        vec_07_1.dec
    vec_07_2.pkt        vec_07_2.dec
    vec_07_3.pkt        vec_07_3.dec
    vec_08_1.pkt        vec_08_1.dec
    vec_08_2.pkt        vec_08_2.dec
    vec_08_3.pkt        vec_08_3.dec
    vec_10_1.pkt        vec_10_1.dec
    vec_10_2.pkt        vec_10_2.dec
    vec_10_3.pkt        vec_10_3.dec



