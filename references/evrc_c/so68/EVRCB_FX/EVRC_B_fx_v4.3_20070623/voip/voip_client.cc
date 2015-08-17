/**********************************************************************
Each of the companies; Motorola, and Lucent, and Qualcomm, and Nokia (hereinafter 
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
// dummy.cpp : Defines the entry point for the DLL application.
//
#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include "macro_fx.h"
#include "proto_fx.h"
#include "globs_fx.h"

#include "tty.h"

#include "jitter_buffer.h"

//#include "struct.h" //For 4GV only

#define MAX_ENCODED_ELEMENTS	100
//#define	FSIZE					160

extern int		num_pkts_since_warped;

//FGV_MEM			FGV_A;
//EvrcArgs*		Eargs;
EvrcArgs		VocoderParameters;

short buf16 [FSIZE * 2];
int16 buf_fx [FSIZE * 2];

extern int		num_pkts_since_warped;
extern int		num_pkts_since_expanded;

struct encoded_data_element
{
	int rate;
	int seq_num;
	char data [22];
	int length;
	float arrival_time;
};

struct	    encoded_data_element encoded_data_array [MAX_ENCODED_ELEMENTS];

static bool g_initDone;
int	    num_encoded_elements;

double	    Current_Time;
FILE	    *fp_out;

void xmain();

void FGVCodec_Init()
{

  // Load default codec configuration
    
  bool bHighPassFilter = true;
  bool bNoiseSuppression = true;
  long OperatingPoint = 0;
  bool bSaveTxFrames = false;
  bool bSaveRxFrames = false;

  // Initializing Vocoder configuration
  Eargs = &VocoderParameters;
  EvrcArgs *eargs = &VocoderParameters;
  eargs->input_filename    = NULL;
  eargs->output_filename   = NULL;
  eargs->max_rate          = 4;    eargs->max_rate_default          = 1;
  eargs->min_rate          = 1;    eargs->min_rate_default          = 1;
  eargs->post_filter       = 1;    eargs->post_filter_default       = 1;
  eargs->noise_suppression = bNoiseSuppression;    eargs->noise_suppression_default = 1;
  eargs->ibuf_len          = SPEECH_BUFFER_LEN;
  eargs->obuf_len          = SPEECH_BUFFER_LEN;

  eargs->highpass_filter=bHighPassFilter;
  eargs->olr_calibration=0;

  eargs->unquantized_lsp=0;
  eargs-> partial_file_processing=0;
  eargs-> starting_frame_num=0;
  eargs-> num_frames=0;
  eargs-> form_res_out=0;
  eargs-> qform_res_out=0;
  eargs-> mform_res_out=0;
  eargs-> target_speech_out=0;
  eargs-> rate_out=0;
  eargs-> lag_out=0;
  eargs-> nacf_out=0;
  eargs-> unquantized_prototype=0;
  eargs-> unquantized_zero_rate=0;
  eargs-> unquantized_quarter_rate=0;
  eargs-> unquantized_half_rate=0;
  eargs-> unquantized_full_rate=0;
  eargs-> rfileP=NULL;
  eargs-> packet_out=0;
  eargs-> vad_out=0;
  eargs-> forced_count=0;
  eargs-> accshift_out=0;
  eargs-> avg_rate_target=5300;
  eargs-> avg_rate_control=0;
  eargs-> PPP_to_CELP_threshold=-150.0;
  eargs-> ratewin=600;		// [DJS] CHANGE FROM FOURGV_FL
#ifdef VOIP_DORA
  eargs->operating_point=1;
#else
  eargs->operating_point=2;
#endif

  
  //eargs->operating_point=OperatingPoint;
  strcpy(eargs->pattern,"QQF");
  eargs->erasure_file=NULL;
  eargs->signalling_file=NULL;
  eargs->verbose=NO;
  eargs->dtx=0;
#ifdef VOIP_DORA
  //Added for Support of Phase Matching/Warping
  eargs->phase_matching = 0;
  eargs->double_erasures_pm = 0;
  eargs->time_warping = 0;
#ifdef	VOIO_DBG
  eargs->dec_parfile = NULL;
#endif
  //End:Added for Support of Phase Matching/Warping
#endif

  eargs->encode_only=0; 
  eargs->decode_only=1;

  /* 
   * [DJS] FX initializations from FL main() in main.cc 
   */
  Init_globs_fx();
  InitEncoder_fx();
  InitDecoder_fx();
  init_tty_dec();

  for (int k=0;k<FSIZE;k++) buf_fx[k]=0;  // maintain an integer copy of input
}

int codecNoMorePackets ()
{
  if (num_encoded_elements == 0)
    return 1;
  else
    return 0;
}

void codecCreateInstance ()
{
  g_initDone = 0;
  num_encoded_elements = 0;
  Current_Time = 0;

  for (int k = 0; k < FSIZE * 2; k++)
    buf_fx[k] = 0;

  FGVCodec_Init();
  //SB.SetVocoderType (FGV_VOCODER);

  Init_Jitter_Buffer();

  fp_out = fopen ("Output_File.dat", "w+");
	
  return;
}

void codecReleaseInstance ()
{
  fclose (fp_out);
}

int codecInputPacket (const void *packet, unsigned int length,
                      unsigned long received)
{
  int rate;
	
  /* Parse RTP header for sequence number and transmitted timestamp. */
  const unsigned char *packetBytes = (const unsigned char *) packet;
  const int seq = ((int) packetBytes[2]) << 8
    | ((int) packetBytes[3]);
  const unsigned long sent = ((unsigned long) packetBytes[4]) << 24
    | ((unsigned long) packetBytes[5]) << 16
    | ((unsigned long) packetBytes[6]) << 8
    | ((unsigned long) packetBytes[7]);

#if CRD_STDOUT
  printf ("\n incoming data is %x %x", packetBytes[12], packetBytes[13]);
#endif

  /* Determine rate based on payload size. */
  /* 
   * NOTE: The quarter-rate (rate=2) vocoder frame size (5 bytes)
   * fills six bytes on little-endian platforms.  Thus, RTP header
   * (12 bytes) plus vocoder frame is 18 bytes.
   */
  switch (length)
    {
    case 34: rate = 4; break;
    case 22: rate = 3; break;
    case 18: rate = 2; break;
    case 14: rate = 1; break;
    default: return 0; /* bad packet */
    }

  /* Initialize the dejitter buffer etc if it is not yet initialized. */
  if (!g_initDone)
    {
      g_initDone = 1;
      Current_Time = received;
    }

  /* Cache the packet for use later when the packet is returned out of the
  ** dejitter buffer.
  */
  encoded_data_array [num_encoded_elements].rate = rate;
  encoded_data_array [num_encoded_elements].seq_num = seq;
  encoded_data_array [num_encoded_elements].length = length - 12;
  encoded_data_array [num_encoded_elements].arrival_time = received;
    
  memcpy (encoded_data_array [num_encoded_elements].data, packetBytes + 12, length - 12);
    
#if CRD_STDOUT
  printf ("\n %d DATA", length - 12);
  for (int i = 0; i < length - 12 + 1; i++)
    printf ("  %d", encoded_data_array [num_encoded_elements].data[i]);
  printf ("\n");
#endif

  num_encoded_elements++;
	
  /* Add packet seq_num to dejitter buffer. */
  Add_To_Jitter_Buffer (seq, sent, (received * 1.0)/1000, rate);

  static int count = 0;
#if CRD_STDOUT
  printf ("\n Packet #%d: %u bytes, arrival time = %lu, seq = %d, sent = %lu stored = %d",
	  count++, length, received, seq, sent, num_encoded_elements);
#endif

  return 1;
}

int codecOutputPCM (short *pcm)
{
  /* Loop until enough PCM is output. */
  short time_warp_fraction = 16384;  /* 1.0 in Q14 */
  int i, j;
  short obuf_len = 160;
  int run_length, phase_offset;
  float arrival_time;
  static int djs_cnt = 0;

  /* Remove packet from the dejitter buffer. */
  /* [DJS]
   * NOTE:  Here, <rate> is distinct from <frameRate> and has the
   *        following meaning:
   *             14:  Erasure during non-silence
   *              4:  Regular packet (silence, NELP, CELP)
   *              1:  Erasure during silence (DTX)
   */
  int seq = -1;
  int rate = Remove_Packet_From_Buffer ((Current_Time * 1.0)/1000, &time_warp_fraction, &seq, &run_length, &phase_offset);
#if CRD_STDOUT
  printf ("\n Packet Returned %d", seq);

  if (phase_offset != 10)
    printf ("\n PM= %d %d %d", phase_offset, run_length, seq);
#endif

  /* Decode the packet to PCM. */
  short frameRate;
    
  if ((rate == 1) || (rate == 14))
    {
#if CRD_STDOUT
      if (rate == 1)
	printf ("\n Rate1 Returned");
        
      printf ("\n blanking decoding %d", frameRate); fflush(stdout);

      printf ("\n decoding %d %x %x", frameRate, buf16[0], buf16[1]); fflush(stdout);
#endif

      frameRate = 0;

      data_packet.PACKET_RATE = frameRate;
      decode_fx (buf16, frameRate, 1, buf_fx, 0, 10, 1, &obuf_len);
#if CRD_STDOUT
      printf ("\n decoded %d %#x", obuf_len, time_warp_fraction); fflush(stdout);
#endif
    }
  else
    {
      djs_cnt++;
      int found = -1;

      for (j = 0; j < num_encoded_elements; j++)
	{
	  if (seq == encoded_data_array [j].seq_num)
	    {
	      found = j;
	      break;
	    }
	}

      frameRate = encoded_data_array [found].rate;
      arrival_time = encoded_data_array [found].arrival_time;

      //printf ("\n Encoded Found %d %d %d", found, seq, frameRate);

      memcpy (buf16, encoded_data_array [found].data, encoded_data_array [found].length);
		
      if (found >= 0)
	{
	  for (j = found; j < num_encoded_elements - 1; j++)
	    {
	      encoded_data_array [j].rate = encoded_data_array [j+1].rate;
	      encoded_data_array [j].seq_num = encoded_data_array [j+1].seq_num;
	      encoded_data_array [j].arrival_time = encoded_data_array [j+1].arrival_time;
	
	      for (i = 0; i < encoded_data_array [j+1].length; i++)
		encoded_data_array [j].data[i] = encoded_data_array [j+1].data[i];
				
	      encoded_data_array [j].length = encoded_data_array [j+1].length;
	    }
	  num_encoded_elements--;
	}

      //Remove older packets from Jitter Buffer

      while (1)
	{
	  found = -1;

	  for (j = 0; j < num_encoded_elements; j++)
	    {
	      if (seq > encoded_data_array [j].seq_num)
		{
		  found = j;
		  break;
		}
	    }

	  if (found >= 0)
	    {
	      for (j = found ; j < num_encoded_elements - 1; j++)
		{
		  encoded_data_array [j].rate = encoded_data_array [j+1].rate;
		  encoded_data_array [j].seq_num = encoded_data_array [j+1].seq_num;
		  encoded_data_array [j].arrival_time = encoded_data_array [j+1].arrival_time;

		  for (i = 0; i < encoded_data_array [j+1].length; i++)
		    encoded_data_array [j].data[i] = encoded_data_array [j+1].data[i];
			
		  encoded_data_array [j].length = encoded_data_array [j+1].length;
		}
	      num_encoded_elements--;
	    }
	  else
	    break;
	}
#if CRD_STDOUT
      printf ("\n decoding %d %x %x", frameRate, buf16[0], buf16[1]); fflush(stdout);
#endif

      data_packet.PACKET_RATE = frameRate;
      decode_fx (buf16, frameRate, 1, buf_fx, run_length, phase_offset, time_warp_fraction, &obuf_len);
#if CRD_STDOUT
      printf ("\n decoded %d %#x", obuf_len, time_warp_fraction); fflush(stdout);
#endif
    }

  /* [DJS] Not necessary to check bounds for fixed point */
//   for (int m = 0; m < obuf_len; m++)
//     {
//       if (buf [m] > 32767.0)
// 	buf16[m] = (short) 32767;
//       else if (buf [m] < -32767.0)
// 	buf16[m] = (short) -32767;
//       else
// 	buf16[m] = (short) buf[m];
//     }

  memcpy (pcm, buf_fx, obuf_len * 2);

  if (obuf_len == 160)
    num_pkts_since_warped++;
  else
    num_pkts_since_warped = 0;

  if (obuf_len > 160)
    num_pkts_since_expanded = 0;
  else
    num_pkts_since_expanded++;

  if (rate == 1 || frameRate == 1)
    {
      fprintf (fp_out, "\n S\t 160 0", Current_Time);
    }
  else if (rate == 14)
    {
      fprintf (fp_out, "\n E\t 160 0", Current_Time);
    }
  else
    {
      fprintf (fp_out, "\n %d\t %d %f", seq, obuf_len, Current_Time - arrival_time);
    }

  Current_Time += (20 * obuf_len * 1.0)/160; //Increment time by 20 msec


  //printf ("%u PCM samples output\n", length);
  return (obuf_len);
}
