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
FILE:       SmartBlanking.cpp

SERVICES:  

GENERmAL DESCRIPTION:  Implementation of the Smart Blanking Algorithm

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
/*======================================================================*/
/*         Include files                         */
/*----------------------------------------------------------------------*/
#include <memory.h>
#include <stdlib.h>

#include "SmartBlanking.h"
//#include "../../Tracer/Tracer.h"     // for registry management utilities

/*======================================================================*/
/*         ..Smart 1/8 Rate Constants                         */
/*----------------------------------------------------------------------*/

#define FILTER_CONSTANT                     5// preferred 5 over 10 --> smoother
#define LOW_LSP_40P_THRESHOLD               71
#define HIGH_LSP_40P_THRESHOLD              79

#define EVRC_POWER_1DB                      11
#define EVRC_POWER_THRESHOLD                EVRC_POWER_1DB * 1

//#define FGV_POWER_1DB                       9
#define FGV_POWER_1DB                       6
#define FGV_POWER_THRESHOLD                 FGV_POWER_1DB * 1

#define N_TRANSITORY_FRAMES                 5
#define N_ERASURE_TIME_OUT                  5


#define EVRC_POWER_CODEBOOK_SIZE            256
#define FGV_POWER_CODEBOOK_SIZE             64

#define DTX_MAX                  (5 * 50)    // in frames (20ms per frame)
#define DTX_MIN					 8        // in frames 

#define INT_MAX 0x7fffffffL

/*======================================================================*/
/*         Definind where the codebook entries are located              */
/*         Depending of the endianess of the handwaree                  */
/*----------------------------------------------------------------------*/
#define LSPS_BYTE_POS		1		// Little Endian 
#define ENERGY_BYTE_POS		0		// (Intel processors) 

//#define LSPS_BYTE_POS		0		// Big Endian 
//#define ENERGY_BYTE_POS		1		// Most DSPs and is also network endianness 

/*======================================================================*/
/*         Scaled EVRC-B average codebook entry power                     */
/*----------------------------------------------------------------------*/
unsigned char FGVEigthRatePower[FGV_POWER_CODEBOOK_SIZE]= {
0,125,127,129,131,133,136,138,140,142,144,146,148,150,152,154,156,159,161,163,
165,167,169,171,173,175,177,180,182,184,186,188,190,192,194,196,198,201,203,205,
207,209,211,213,215,217,219,221,224,226,228,230,232,234,236,238,240,242,245,247,
249,251,253,255
};

/* Old table having some issues with lower volume ranges
unsigned char FGVEigthRatePower[FGV_POWER_CODEBOOK_SIZE]= {
0, 40, 44, 47, 51, 54, 58, 61, 65, 68, 72, 75, 78, 82, 85, 89,
92, 96, 99, 103, 106, 110, 113, 117, 120, 123, 127, 130, 134, 137, 141, 144,
148, 151, 155, 158, 162, 165, 168, 172, 175, 179, 182, 186, 189, 193, 196, 200,
203, 207, 210, 213, 217, 220, 224, 227, 231, 234, 238, 241, 245, 248, 252, 255
};
*/

/*======================================================================*/
/*         Scaled EVRC average codebook entry power                     */
/*----------------------------------------------------------------------*/
unsigned char EVRCEigthRatePower[EVRC_POWER_CODEBOOK_SIZE]= {
27, 137, 80, 180, 57, 161, 103, 196, 53, 154, 107, 189, 67, 165, 133, 211,
45, 146, 93, 184, 62, 169, 116, 199, 58, 160, 131, 193, 70, 173, 138, 225,
49, 149, 88, 185, 62, 167, 122, 205, 57, 157, 130, 194, 66, 172, 150, 221,
52, 150, 104, 188, 66, 173, 124, 203, 63, 164, 162, 207, 81, 179, 172, 233,
0, 143, 89, 182, 61, 164, 116, 201, 59, 156, 124, 195, 70, 169, 148, 218,
50, 150, 97, 190, 65, 174, 120, 201, 60, 161, 176, 198, 75, 182, 160, 236,
56, 149, 94, 187, 66, 169, 130, 217, 63, 160, 144, 196, 75, 174, 159, 217,
55, 156, 110, 192, 66, 181, 133, 205, 65, 168, 163, 211, 85, 183, 192, 240,
22, 139, 83, 184, 58, 163, 105, 198, 49, 157, 118, 193, 69, 167, 134, 216,
49, 151, 95, 187, 64, 173, 120, 202, 59, 164, 177, 186, 76, 176, 150, 233,
175, 150, 93, 188, 64, 170, 125, 211, 60, 160, 182, 195, 72, 174, 153, 185,
53, 153, 110, 191, 74, 177, 130, 204, 63, 165, 180, 211, 84, 185, 175, 238,
172, 145, 90, 185, 61, 167, 176, 206, 58, 160, 130, 198, 73, 169, 179, 220,
53, 155, 103, 192, 68, 174, 124, 209, 62, 162, 177, 202, 77, 189, 170, 246,
63, 154, 98, 192, 68, 171, 180, 234, 177, 163, 182, 198, 77, 178, 183, 226,
57, 158, 180, 195, 72, 182, 183, 210, 68, 171, 184, 220, 89, 187, 208, 255
};

/*======================================================================*/
/*        Scaled EVRC "Low frequency LSPs" spectral distance            */
/*----------------------------------------------------------------------*/
unsigned char EVRCLowLSPsDistance[LSP_CODEBOOK_SIZE][LSP_CODEBOOK_SIZE]= {
0,99,53,93,54,82,90,198,41,162,72,153,78,138,124,255,
99,0,81,87,100,97,80,129,87,90,41,137,125,128,66,173,
53,81,0,75,53,68,61,183,33,148,52,138,84,123,109,240,
93,87,75,0,72,58,55,109,56,80,49,63,61,53,56,166,
54,100,53,72,0,71,78,180,42,145,67,135,58,121,106,238,
82,97,68,58,71,0,70,116,41,99,56,93,58,56,86,173,
90,80,61,55,78,70,0,132,61,116,51,95,81,89,58,189,
198,129,183,109,180,116,132,0,157,63,131,62,122,66,74,57,
41,87,33,56,42,41,61,157,0,121,46,111,53,97,84,214,
162,90,148,80,145,99,116,63,121,0,96,77,112,93,71,99,
72,41,52,49,67,56,51,131,46,96,0,99,88,91,57,188,
153,137,138,63,135,93,95,62,111,77,99,0,78,66,74,102,
78,125,84,61,58,58,81,122,53,112,88,78,0,64,71,179,
138,128,123,53,121,56,89,66,97,93,91,66,64,0,75,117,
124,66,109,56,106,86,58,74,84,71,57,74,71,75,0,131,
255,173,240,166,238,173,189,57,214,99,188,102,179,117,131,0
};

/*======================================================================*/
/*        Scaled EVRC "High frequency LSPs" spectral distance           */
/*----------------------------------------------------------------------*/
unsigned char EVRCHighLSPsDistance[LSP_CODEBOOK_SIZE][LSP_CODEBOOK_SIZE]= {
0,136,129,117,114,158,207,133,92,121,141,143,102,155,191,203,
136,0,54,73,46,56,164,125,80,54,64,115,78,97,114,199,
129,54,0,73,58,74,110,131,71,69,47,113,60,103,79,206,
117,73,73,0,88,60,144,69,48,54,44,54,73,77,118,144,
114,46,58,88,0,89,152,157,95,48,73,139,45,129,137,232,
158,56,74,60,89,0,159,68,66,50,51,95,74,40,96,143,
207,164,110,144,152,159,0,179,162,152,111,130,112,144,84,205,
133,125,131,69,157,68,179,0,62,109,89,65,112,43,117,75,
92,80,71,48,95,66,162,62,0,60,64,70,75,66,130,137,
121,54,69,54,48,50,152,109,60,0,50,96,52,81,128,184,
141,64,47,44,73,51,111,89,64,50,0,73,49,70,88,163,
143,115,113,54,139,95,130,65,70,96,73,0,94,64,96,93,
102,78,60,73,45,74,112,112,75,52,49,94,0,84,97,186,
155,97,103,77,129,40,144,43,66,81,70,64,84,0,77,103,
191,114,79,118,137,96,84,117,130,128,88,96,97,77,0,145,
203,199,206,144,232,143,205,75,137,184,163,93,186,103,145,0
};

/*======================================================================*/
/*         Helper Function                                              */
/*----------------------------------------------------------------------*/
short RS1_LengthToRate(short Length)
{
	switch (Length)
	{
		case 0:
			return BLANK_RATE;
		case 2:
			return EIGHTH_RATE;
		case 5:
			return QUARTER_RATE;
		case 10:
			return HALF_RATE;
		case 22:
			return FULL_RATE;
		default:
			return ERASED_FRAME;
	}
}

short RS1_RateToLength(short Rate)
{
	switch (Rate)
	{
		case EIGHTH_RATE:
			return 2;
		case QUARTER_RATE:
			return 5;
		case HALF_RATE:
			return 10;
		case FULL_RATE:
			return 22;
		default:    // 0 bytes for Erasure, Blank or "error"
			return 0;
	}
}


/*======================================================================*/
/*         Smart 1/8 Rate blanking helper functions                     */
/*----------------------------------------------------------------------*/
void UpdateMemory(unsigned char LSPs, unsigned int *Histogram)
{
    int i;

    for (i=0; i < LSP_CODEBOOK_SIZE; i++)    // 16 codebooks
    {
        if (i == LSPs)
        {
            Histogram[i] = (Histogram[i] * (100 - FILTER_CONSTANT) + 100 * FILTER_CONSTANT)/100;
        }
        else
        {
            Histogram[i] =  (Histogram[i] * (100 - FILTER_CONSTANT))/100;
        }
    }
}




unsigned char GetMostPopular(unsigned int *Histogram)
{
    int i;
    unsigned char Index = 0;
    

    for (i=0; i <LSP_CODEBOOK_SIZE; i++)   // 16 codebooks
    {
        if (Histogram[i] >Histogram[Index])
        {
            Index = i;
        }
    }
    return Index;
}

/*======================================================================*/
/*         Smart blanking contructor and destructors                    */
/*----------------------------------------------------------------------*/
SBEncoder::SBEncoder()
{
    FirstTime = 1;
    InSilence = 0;							
    //SetVocoderType(EVRC_VOCODER);     // EVRC is assumed by default
    SetVocoderType(FGV_VOCODER);

    // little ugly butvery efficient...  Initializing memory
    memset(LowLSPsHist, 0, (16 * sizeof(int)));
    memset(HighLSPsHist, 0, (16 * sizeof(int)));

    FilteredLowLSPsDeltas = 0;
    FilteredHighLSPsDeltas = 0;
    NFramesSinceLastSent = 0;
}


SBDecoder::SBDecoder()
{
  //PrototypePlaybackProbability = (short)GetRegKeyLong(HKEY_CURRENT_USER, "Software\\Cisco Systems\\CCNMediaTerm\\SmartBlanking", "PrototypePlaybackProbability", 80);
    PrototypePlaybackProbability = 80; 
    FirstTime = 1;
    InSilence = 1;	// Decoder is considered to be in silence since it can take a while for it to get
					// good data (received data)
    EigthRatePrototype[ENERGY_BYTE_POS] = 0x00;   // Prototype should be initialized
    EigthRatePrototype[LSPS_BYTE_POS] = 0x05;   // to anything "quiet" and valid
}

/*======================================================================*/
/*         Smart blanking public helper funtions                   */
/*----------------------------------------------------------------------*/
void SBEncoder::SetVocoderType(int type)
{
    Vocoder = type;

    switch (Vocoder)
    {
    case EVRC_VOCODER :
        PowerThreshold = EVRC_POWER_THRESHOLD;
        break;
    case FGV_VOCODER :
        PowerThreshold = FGV_POWER_THRESHOLD;
        break;

    }
}

void SBEncoder::UpdateFilteredPower(unsigned char Power)
{
    switch (Vocoder)
    {
    case EVRC_VOCODER :
        FilteredPower = (FilteredPower * 90 +  EVRCEigthRatePower[Power] * 10)/100;
        break;
    case FGV_VOCODER :
      //FilteredPower = (FilteredPower * 90 +  FGVEigthRatePower[Power>>2] * 10)/100;
	FilteredPower = (FilteredPower * 80 +  FGVEigthRatePower[Power>>2] * 20)/100;
        break;
    }

}

int SBEncoder::GetDeltaFromAverage(unsigned char Power)
{
    int DeltaFromAverage = INT_MAX;     // Very big number
    
    switch (Vocoder)
    {
    case EVRC_VOCODER :
        DeltaFromAverage = abs(FilteredPower - EVRCEigthRatePower[Power]);
        break;
    case FGV_VOCODER :
        DeltaFromAverage = abs(FilteredPower - FGVEigthRatePower[Power>>2]);
        break;
    }

    return DeltaFromAverage;
}


void SBDecoder::SetVocoderType(int type)
{
    Vocoder = type;     
}

short SBEncoder::RateToLength(short Rate)
{
    return RS1_RateToLength(Rate);
}

short SBEncoder::LengthToRate(short Length)
{
    return RS1_LengthToRate(Length);
}

short SBDecoder::RateToLength(short Rate)
{
    return RS1_RateToLength(Rate);
}

short SBDecoder::LengthToRate(short Length)
{
    return RS1_LengthToRate(Length);
}

void memcpy(short* tar, short* src, int N)
{
  int i;
  for(i=0;i<N;i++)
    tar[i]=src[i];
  
}



#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK1 23148373

float ran0_d(long *seed0)
{
  long    k;
  float   ans;
  
  *seed0 ^= MASK1;
  k = (*seed0) / IQ;
  *seed0 = IA * (*seed0 - k * IQ) - IR * k;
  if (*seed0 < 0) *seed0 += IM;
  ans = AM * (*seed0);
  *seed0 ^= MASK1;
  return (ans);
}

/*======================================================================*/
/*         Smart blanking encode and decode functions                   */
/*----------------------------------------------------------------------*/
unsigned int 
SBEncoder::Encode(short* Rate, unsigned char* IBuffer, short refl_flag,short nsid)
{
    
    unsigned char Current8Frame[2];
    unsigned char LowLSPs;
    unsigned char HighLSPs;
	unsigned int  PowerDelta;

    memcpy(Current8Frame, IBuffer, 2);
    if (*Rate > EIGHTH_RATE)    // It is assumed that encoder does not generate erasures
    {							// or it blank frames
        InSilence = 0;
    }
    else
    {
        InSilence++;    // warp arround after 21+ minutes (16 bits), good enough
        if (FirstTime)
        {
            memcpy(CandidateEigthRatePrototype, IBuffer, 2);
            memcpy(EigthRatePrototype, IBuffer, 2); 
            ProtoLowLSPs = EigthRatePrototype[LSPS_BYTE_POS] >> 4;
            ProtoHighLSPs = EigthRatePrototype[LSPS_BYTE_POS] & 0x0F;
            
            UpdateMemory(ProtoLowLSPs, LowLSPsHist);
            UpdateMemory(ProtoHighLSPs, HighLSPsHist);

          //  FilteredPower = EVRCEigthRatePower[EigthRatePrototype[ENERGY_BYTE_POS]];
            FilteredPower = FGVEigthRatePower[EigthRatePrototype[ENERGY_BYTE_POS]>>2];

            FirstTime = 0;
        }
    }

    // For interoperability with dumber  algorithm we want to consider
    // The following changes which forces sending the prototype to bring
    // noise floor down at the dumber receiver size (with out smart blanking)
    if (InSilence == 1)
    {
    // "Send prototype" frame after 1st sil frame
    memcpy(IBuffer, EigthRatePrototype, 2);
    }

    if (InSilence > 1)    // Number of frames to send

//    if (InSilence > 1)    // Number of frames to send
    {
        *Rate = BLANK_RATE;
        NFramesSinceLastSent++;
    }
    else
    {
        NFramesSinceLastSent = -1;  // a frame will be transmitted
    }

    
    if ((InSilence > N_TRANSITORY_FRAMES) || (refl_flag==1))  // number after which encoder is assumed stable, =5
    {
        // Update memory, filters
        UpdateFilteredPower(Current8Frame[ENERGY_BYTE_POS]);

        LowLSPs = Current8Frame[LSPS_BYTE_POS] >> 4;
        HighLSPs = Current8Frame[LSPS_BYTE_POS] & 0x0F;

        UpdateMemory(LowLSPs, LowLSPsHist);
        UpdateMemory(HighLSPs, HighLSPsHist);

		FilteredLowLSPsDeltas = (FilteredLowLSPsDeltas * (100 - FILTER_CONSTANT) +
                                 EVRCLowLSPsDistance[LowLSPs][ProtoLowLSPs] * FILTER_CONSTANT)/100;
        FilteredHighLSPsDeltas = (FilteredHighLSPsDeltas * (100 - FILTER_CONSTANT) + 
                                  EVRCHighLSPsDistance[HighLSPs][ProtoHighLSPs] * FILTER_CONSTANT)/100;

        // Update candidate prototype if required 
        if (GetDeltaFromAverage(CandidateEigthRatePrototype[ENERGY_BYTE_POS]) > GetDeltaFromAverage(Current8Frame[ENERGY_BYTE_POS]))
        {
            memcpy(CandidateEigthRatePrototype, Current8Frame, 2);  // Copy full frame even when not needed.
	    }
        
        PowerDelta = GetDeltaFromAverage(EigthRatePrototype[ENERGY_BYTE_POS]);


	if (nsid==0)
	 {
        // Send Prototype if background noise update is required
	    if (((PowerDelta > PowerThreshold) || (FilteredLowLSPsDeltas > LOW_LSP_40P_THRESHOLD) ||
		 (FilteredHighLSPsDeltas > HIGH_LSP_40P_THRESHOLD))
                && ((short) *CandidateEigthRatePrototype != (short) *EigthRatePrototype)
                && (NFramesSinceLastSent > DTX_MIN) || (refl_flag==1))
	      {
		// build new prototype 
		LowLSPs = GetMostPopular(LowLSPsHist);
		HighLSPs = GetMostPopular(HighLSPsHist);
		
		CandidateEigthRatePrototype[LSPS_BYTE_POS] = ((LowLSPs << 4) | HighLSPs);
		
		// We have a new prototype... update mamory and filters
		memcpy(EigthRatePrototype, CandidateEigthRatePrototype, 2);
		ProtoLowLSPs =  LowLSPs;
		ProtoHighLSPs = HighLSPs;
		FilteredLowLSPsDeltas = 0;
		FilteredHighLSPsDeltas = 0;
		
		// "Send prototype"
		memcpy(IBuffer, EigthRatePrototype, 2);
		*Rate = EIGHTH_RATE;
		NFramesSinceLastSent = 0;
	      }
	 }
        if (NFramesSinceLastSent >= DTX_MAX)
        {
            // "Send prototype"
            memcpy(IBuffer, EigthRatePrototype, 2);
            *Rate = EIGHTH_RATE;
            NFramesSinceLastSent = 0;
        }
    }

    return InSilence; 
}

unsigned int 
SBDecoder::Decode(short* Rate, unsigned char* IBuffer)
{

  static long seed=0;
//    if ((*Rate != EIGHTH_RATE) && (*Rate != ERASED_FRAME))
    if (*Rate > EIGHTH_RATE)	// It is assumed that erasure is fed as blank frame
    {
        InSilence= 0;
        ErasureRun = 0;
    }
    else // We have a 1/8 rate frame or an erasure
    {
        if (*Rate == EIGHTH_RATE)  // If we got a 1/8 rate frame.. we transition to silence
        {
            if (FirstTime)
            {
            // Asume that first received 1/8 Rate is the prototype
	      memcpy(EigthRatePrototype, IBuffer, 2);
	      
            FirstTime = 0;
            }
            InSilence++;
        }
        else    // we have an erasure or blank frame (same for the algorithm)   
        {
            if (InSilence || (ErasureRun++ >= N_ERASURE_TIME_OUT))
            {
                InSilence++;    // warp arround after 21+ minutes (16 bits), good enough 
            }
        }
    }

    if (InSilence > 1)  // first 1/8 rate gets played as is. It is considered "speech"
    {
        if (*Rate == EIGHTH_RATE)   // a new prototype has arrived
        {                                                    
            memcpy(EigthRatePrototype, IBuffer, 2);          
        }                                                    
        else    // play stored prototype
        {
	  /*if ((InSilence == 2)  ||	//Always play prototype after first received 1/8 rate frame 
				     (ran0_d(&seed) < 0.5)) // randomize after that
				     {*/
                memcpy(IBuffer, EigthRatePrototype, 2);
                *Rate = EIGHTH_RATE;
		//}
        }
    }
    
    return InSilence; 
}

