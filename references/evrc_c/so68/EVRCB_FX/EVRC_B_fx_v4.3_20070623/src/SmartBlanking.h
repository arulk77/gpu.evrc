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
/*=============================================================================
FILE:       SmartBlanking.h

SERVICES:   

GENERAL DESCRIPTION:  Implementation of the Smart Blanking Algorithm (Headers)

INITIALIZATION AND SEQUENCING REQUIREMENTS:
none 

(c) COPYRIGHT 2005 QUALCOMM Incorporated.
All rights reserved.  QUALCOMM proprietary and confidential.

The party receiving this software directly from QUALCOMM (the "Recipient" )
may use this software and make copies thereof as reasonably necessary solely
for the purposes set forth in the agreement between the Recipient and
QUALCOMM (the "Agreement").  The software may be used in source code form
solely by the Recipient's employees.  The Recipient shall have no right to
sublicense, assign, transfer or otherwise provide the source code to any
third party. Subject to the terms and conditions set forth in the Agreement,
this software, in binary form only, may be distributed by the Recipient to
its customers. QUALCOMM retains all ownership rights in and to the software.

This notice shall supercede any other notices contained within the software.

=============================================================================*/
// SmartBlanking.h: interface for the SmartBlanking class.
//
//////////////////////////////////////////////////////////////////////

                                                                            
#ifndef SMART_BLANKING_H_
#define SMART_BLANKING_H_

/*======================================================================*/
/*         ..Smart 1/8 Rate Constants                         */
/*----------------------------------------------------------------------*/
#define LSP_CODEBOOK_SIZE                   16
#define EVRC_VOCODER                        0
#define FGV_VOCODER                         1
#define FGVWB_VOCODER						2

/*======================================================================*/
/*  Rate definition bellow MUST match with the one used by vocoder      */
/*            This values are used by ECVRC and EVRC-B c-SIMs              */
/*----------------------------------------------------------------------*/
#define FULL_RATE           4
#define HALF_RATE           3
#define QUARTER_RATE        2
#define EIGHTH_RATE         1
#define BLANK_RATE          0
#define ERASED_FRAME        0xe


    
class SBEncoder  
{
public:
	SBEncoder();
//	~SBEncoder();
    void SetVocoderType(int Vocoder);
    //virtual unsigned int Encode(short* Rate, short* IBuffer);
    // virtual unsigned int Encode(short* Rate, unsigned char* IBuffer);
    virtual unsigned int Encode(short* Rate, unsigned char* IBuffer,short refl_flag,short nsid);
    short RateToLength(short Rate);
    short LengthToRate(short Length);

private:
    void UpdateFilteredPower(unsigned char Power);
	int GetDeltaFromAverage(unsigned char Power);

    int FirstTime;
    unsigned int InSilence;
    int Vocoder;
    unsigned char EigthRatePrototype[2];
    unsigned char CandidateEigthRatePrototype[2];
    unsigned int LowLSPsHist[LSP_CODEBOOK_SIZE];  // 16 Codebooks
    unsigned int HighLSPsHist[LSP_CODEBOOK_SIZE]; // 16 Codebooks
    unsigned char ProtoLowLSPs;
    unsigned char ProtoHighLSPs;

    unsigned int FilteredPower;
    unsigned int PowerThreshold;
    unsigned int FilteredLowLSPsDeltas;
    unsigned int FilteredHighLSPsDeltas;
    unsigned int NFramesSinceLastSent;
};

class SBDecoder  
{
public:
	SBDecoder();
//	~SBDecoder();
    void SetVocoderType(int Vocoder);
	virtual unsigned int Decode(short* Rate, unsigned char* IBuffer);
    short RateToLength(short Rate);
    short LengthToRate(short Length);

private:    
    int FirstTime;
    unsigned int InSilence;
    int Vocoder;
    int ErasureRun;
    unsigned char EigthRatePrototype[2];

    short PrototypePlaybackProbability;
};


#endif // !defined(SMART_BLANKING_H_)
                                                                            
