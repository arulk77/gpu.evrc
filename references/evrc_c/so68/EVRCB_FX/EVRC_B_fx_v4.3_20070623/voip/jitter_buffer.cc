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
#include "jitter_buffer.h"

int		first_received;	//First Packet is Being Added to Jitter Buffer
int		first_packet;	//First Packet has been played out or not
double	required_jitter_length; //current value of reqd jitter length

double	new_jitter_length; //newly calculated value of jitter length


// these parameters are only for algorithm 5
int		num_underflows; //keeps track of number of underflows
int		num_packets_reached;
double	underflow_rate;
double	last_underflow_rate;
// end these parameters are only for algorithm 5

struct jitter_element
{
	int		seq_num;
	long		timestamp; //Added to keep track of length of silence within a sentence
	double		rcvd_time;
	int		rate;
}; //element of jitter buffer

struct	jitter_element jitter_buffer[100]; //jitter buffer array
int		jitter_buffer_num_elements; //number of elements in jitter buffer


int		consec_drops; //keeps track of consecutive drops: used to determine if past and future packets of a dropped packet are received

int		next_to_play; //next packet to play

#ifdef PHASE_MATCHING_ENABLED
int		next_to_play_before_erasure; // For Phase Matching
#endif

int		consec_underflow_count; //used for non drop option: amoung of extra jitter than can be introduced in a talkspurt due to late packets


double	last_updated_time;

double	avg_buffer_num_elements;

int		underflow_array [SIZE_UNDERFLOW_ARRAY];
int		num_elements_underflow_array;

int		silenceIntervalInProgress;

float	ALPHA;
int		CYCLE_LENGTH;
int		num_pkts_since_warped;
int		num_pkts_since_expanded;

float stored_PERs[10];
int num_stored_PERs = 0;


// Variables for adaptive filtering
double 		LastPacketArrivalTime;
int		BundleSize;
float		FilteredBundlingSize;
float		FilteredDejitterSize;


//Variables for maintaining intra-sentence separation of utterances
double 		last_packet_playing_time;
long		last_packet_timestamp;
int		first_utterance_in_sentence_passed = 0;
int 		num_consecutive_silence = 0;


double	MAXF (double a, double b)
{
	if (a > b)
		return a;
	else
		return b;
}

double	MINF (double a, double b)
{
	if (a < b)
		return a;
	else
		return b;
}

void	Init_Jitter_Buffer ()
{
	first_packet = 1;
	first_received = 0;

	if (TARGET_PKT_UNDERFLOW > 0.0085)
	{
		ALPHA = 0.9;
		CYCLE_LENGTH = 11;
	}
	else if (TARGET_PKT_UNDERFLOW > 0.0065)
	{
		ALPHA = 0.83;
		CYCLE_LENGTH = 27;
	}
	else if (TARGET_PKT_UNDERFLOW > 0.004)
	{
		ALPHA = 0.99;
		CYCLE_LENGTH = 26;
	}
	else if (TARGET_PKT_UNDERFLOW > 0.0015)
	{
		ALPHA = 0.99;
		CYCLE_LENGTH = 52;
	}
	else
	{
		ALPHA = 0.99;
		CYCLE_LENGTH = 25;
	}
		
	//Initializations


	required_jitter_length = DEFAULT_JITTER; //current value of reqd jitter length
	new_jitter_length = DEFAULT_JITTER; //newly calculated value of jitter length

	num_underflows = 0; //keeps track of number of underflows
			
	num_packets_reached = 1;
	underflow_rate = TARGET_PKT_UNDERFLOW;
	last_underflow_rate = 0;

	num_pkts_since_warped = 0;
	num_pkts_since_expanded = 0;

	jitter_buffer_num_elements = 0; //number of elements in jitter buffer

	consec_drops = 0; //keeps track of consecutive drops: used to determine if past and future packets of a dropped packet are received

	next_to_play = 0; //next packet to play

	if (NON_DROP_OPTION > 0)
	{
		consec_underflow_count = 0; //used for non drop option: amoung of extra jitter than can be introduced in a talkspurt due to late packets
	}

	last_updated_time = 0;

	avg_buffer_num_elements = 2;

	num_elements_underflow_array = 0;
	silenceIntervalInProgress = 1;


	// Initialization for adaptive filtering
    	LastPacketArrivalTime = 0.0;
    	BundleSize = 0;
    	FilteredBundlingSize = 1;
    	FilteredDejitterSize = 3;
}

void	Add_To_Jitter_Buffer (int sn, long ts, double rcvd_time, int rate)
{
	int index;

	if (first_packet == 1)
	{
		if (sn < next_to_play || first_received == 0)
			next_to_play = sn;

		first_received = 1;
	}

	if (rate != 1)
		num_packets_reached++;


	if  (FLAG_BUNDLING_HANDLING == 1)// Dynamic filtering
    	{
       		if (fabs(LastPacketArrivalTime - rcvd_time) < 0.003)  // anything delta of more than 2 ms are considered unbundle
       		{
        	   	BundleSize++;
       		}
       		else
       		{
           		// Update Filter
     			FilteredBundlingSize = (9 * FilteredBundlingSize + BundleSize)/10; // Filter constant of 10 same than default evaluation period
           		BundleSize = 0;
#if CRD_STDOUT
  			printf("Filter = %f\n", FilteredBundlingSize);
#endif
       		}

       		LastPacketArrivalTime = rcvd_time;
	}

	
	while (1)
	{
		//update underflow count
		index = -1;

		for (int g = 0; g < num_elements_underflow_array; g++)
		{
			if (underflow_array [g] == sn)
			{
				index = g;
				if (rate != 1)
				{
					num_underflows++;
#if CRD_STDOUT
					printf ("\n Underflow Found %d", sn);
#endif

				}
				break;
			}
		}

		if (index != -1)
		{
			for (int g = index; g < num_elements_underflow_array - 1; g++)
			{
				underflow_array [g] = underflow_array [g+1];
			}

			num_elements_underflow_array --;
		}
		else
			break;
	}


	if ( (sn >= next_to_play && sn < next_to_play + 1000) || sn < next_to_play - 1000)
	{
		Add_Packet_To_Buffer (sn, ts, rcvd_time, rate);

		if ((num_packets_reached >= CYCLE_LENGTH /*|| current_playing_time - last_updated_time > 0.4*/))
		{
			Algorithm5 (rcvd_time);
		}
	}
}

void	Add_Packet_To_Buffer (int sn, long ts, double rcvd_time, int rate)
{
	int index, i, insert_index;

	insert_index = jitter_buffer_num_elements;
	for (index = 0; index < jitter_buffer_num_elements; index++)
	{
		if (sn < jitter_buffer[index].seq_num && sn > jitter_buffer[index].seq_num - 1000)
		{
			insert_index = index;
			break;
		}
	}

	for (i = jitter_buffer_num_elements; i > insert_index; i--)
	{
		jitter_buffer[i].seq_num = jitter_buffer[i-1].seq_num;
		jitter_buffer[i].timestamp = jitter_buffer[i-1].timestamp;
		jitter_buffer[i].rate = jitter_buffer[i-1].rate;
		jitter_buffer[i].rcvd_time = jitter_buffer[i-1].rcvd_time;
	}

	jitter_buffer[insert_index].seq_num = sn;
	jitter_buffer[insert_index].timestamp = ts;
	jitter_buffer[insert_index].rate = rate;
	jitter_buffer[insert_index].rcvd_time = rcvd_time;

#if CRD_STDOUT
	printf ("\n\nreceiving at %f checking %d seq_num %d timestamp %d %d nums", rcvd_time, jitter_buffer_num_elements, sn, ts, insert_index);
#endif

	jitter_buffer_num_elements++;
}

int		Remove_Packet_From_Buffer (double current_playing_time, short *time_warp_fraction, int *global_sn, int *run_length, int *phase_offset)
{
	int		i;
	int		return_value = 14;
	int		return_rate = 4; //NEW ADDED
	int		store_sn = -1;

#ifdef PHASE_MATCHING_ENABLED
	int		store_pt = 1; //For Phase Matching
	
	*phase_offset = 10; //Phase Matching Disabled
	*run_length = consec_drops; //Run Length of Phase Matching
#endif
	
	if (NON_DROP_OPTION > 0)
	{
		if (jitter_buffer_num_elements > 0 && consec_underflow_count > 0)
		{
			for (i = 0; i <= consec_underflow_count; i++)
			{
				if (i > NON_DROP_OPTION)
					break;
				if (next_to_play + i == jitter_buffer[0].seq_num)
				{
					next_to_play = (next_to_play + i);
					consec_underflow_count = 0;
					break;
				}
			}
		}

		if (silenceIntervalInProgress == 1)
			consec_underflow_count = 0;
	}

	if (jitter_buffer_num_elements > 0)
    {
    	if (silenceIntervalInProgress == 1)
    	{
    		int check = 0;
			int check_index = 0;
			int num_non_eighth_packets = 0;

			for (int m = 0; m < jitter_buffer_num_elements; m++)
			{
				if (jitter_buffer[m].rate != 1)
				{
					check_index = m;
					break;
				}
			}
			
			for (int m = 0; m < jitter_buffer_num_elements; m++)
			{
				if (jitter_buffer[m].rate != 1)
				{
					num_non_eighth_packets++;
				}
			}

#if CRD_STDOUT
			printf ("\n Checking check_index=%d num_non_eighth_packets=%d", check_index, num_non_eighth_packets);
#endif
	
			if (num_non_eighth_packets > 0)
			{
#if CRD_STDOUT
				printf ("\nChecking_Times %f %f %f", current_playing_time, jitter_buffer[check_index].rcvd_time, required_jitter_length);
#endif

				if (current_playing_time >= jitter_buffer[check_index].rcvd_time + MAXF (required_jitter_length * BEGINNING_ENHANCEMENT_FACTOR, 0.02) - 0.000005)
		   		{
		   			check = 1;
		   		}

		   		if (num_non_eighth_packets >= (MAXF (required_jitter_length * BEGINNING_ENHANCEMENT_FACTOR, 0.02))/0.02)
		   		{
		   			//check = 1;
		   		}

				if (first_utterance_in_sentence_passed == 1)
				{
					double delay_relative_to_last_packet, delay_absolute;

					delay_absolute = (jitter_buffer[check_index].timestamp - last_packet_timestamp) * (0.02/160); 

					delay_relative_to_last_packet = (current_playing_time - last_packet_playing_time) - ((jitter_buffer[check_index].timestamp - last_packet_timestamp) * (0.02/160));

#if CRD_STDOUT
					printf ("\n Utterance Continuing %f %f %f %d %d check = %d delay = %f", delay_relative_to_last_packet, current_playing_time, last_packet_playing_time, jitter_buffer[check_index].timestamp, last_packet_timestamp, check, delay_absolute);
#endif
					if (FLAG_INTRA_TALKSPURT == 1)
					{
						if (delay_relative_to_last_packet < MINF (((-1 *  MAINTAIN_INTRA_TALKSPURT_SILENCE_COMPRESS * delay_absolute) - 0.01), -0.02) && check == 1)
						{
							check = 0;
#if CRD_STDOUT
							printf ("\n Utterance Postponing %f %f", current_playing_time, fabs(delay_relative_to_last_packet));
#endif
						}
						if (delay_relative_to_last_packet >= MAXF(((MAINTAIN_INTRA_TALKSPURT_SILENCE_EXPAND * delay_absolute) - 0.01), 0.02) && check == 0)
						{
							check = 1;
#if CRD_STDOUT
							printf ("\n Utterance Preponing %f %f", current_playing_time, fabs(delay_relative_to_last_packet));
#endif
						}
					}
				}
			}

			while (num_non_eighth_packets > 0 && check == 1)
			{
				if (jitter_buffer[0].rate != 1)
					break;

				else
				{
					for (i = 0; i < jitter_buffer_num_elements - 1; i++)
					{
						jitter_buffer[i].seq_num = jitter_buffer[i+1].seq_num;
						jitter_buffer[i].timestamp = jitter_buffer[i+1].timestamp;
						jitter_buffer[i].rate = jitter_buffer[i+1].rate;
						jitter_buffer[i].rcvd_time = jitter_buffer[i+1].rcvd_time;
					}
					
					jitter_buffer_num_elements--;
				}
			}
	
			if (jitter_buffer[0].rate == 1) //1/8th rate packet, silence
			{
#if CRD_STDOUT
				printf ("\n Silence Packet");
#endif
				next_to_play ++;

				store_sn = jitter_buffer[0].seq_num;
					
				for (i = 0; i < jitter_buffer_num_elements - 1; i++)
				{
					jitter_buffer[i].seq_num = jitter_buffer[i+1].seq_num;
					jitter_buffer[i].timestamp = jitter_buffer[i+1].timestamp;
					jitter_buffer[i].rate = jitter_buffer[i+1].rate;
					jitter_buffer[i].rcvd_time = jitter_buffer[i+1].rcvd_time;
				}
				jitter_buffer_num_elements--;

				silenceIntervalInProgress = 1;

#ifdef PHASE_MATCHING_ENABLED
				store_pt = jitter_buffer[0].rate; //For Phase Matching
#endif

				return_rate = 1;
		       	return_value = 4; //silent packet is a packet
			}
			else
			{		
       			if (check == 1)
       			{
       				next_to_play = jitter_buffer[0].seq_num + 1;
       				silenceIntervalInProgress = 0;
				
				consec_drops = 0;

#ifdef PHASE_MATCHING_ENABLED
				store_pt = jitter_buffer[0].rate; //For Phase Matching
#endif
						
				return_value = 4;
						
				store_sn = jitter_buffer[0].seq_num;
											
				for (i = 0; i < jitter_buffer_num_elements - 1; i++)
				{
					jitter_buffer[i].seq_num = jitter_buffer[i+1].seq_num;
					jitter_buffer[i].timestamp = jitter_buffer[i+1].timestamp;
					jitter_buffer[i].rate = jitter_buffer[i+1].rate;
					jitter_buffer[i].rcvd_time = jitter_buffer[i+1].rcvd_time;
				}

				jitter_buffer_num_elements--;
       			}
 	      		else if (check == 0)
       			{
       				return_value = 1; //silence
      	 		}
			}
   		}
		else if (silenceIntervalInProgress == 0)
		{
	   		if (jitter_buffer[0].seq_num == next_to_play)
			{	
				consec_drops = 0;

				last_packet_playing_time = current_playing_time;
				last_packet_timestamp = jitter_buffer[0].timestamp;
				first_utterance_in_sentence_passed = 1;
#if CRD_STDOUT
				printf ("\n Utterance Sentence %f %d", last_packet_playing_time, last_packet_timestamp);
#endif

				if (jitter_buffer[0].rate == 1) //1/8th rate packet, silence starts
		   		{
					next_to_play ++;

			   		store_sn = jitter_buffer[0].seq_num;
											
					for (i = 0; i < jitter_buffer_num_elements - 1; i++)
					{
						jitter_buffer[i].seq_num = jitter_buffer[i+1].seq_num;
						jitter_buffer[i].timestamp = jitter_buffer[i+1].timestamp;
						jitter_buffer[i].rate = jitter_buffer[i+1].rate;
						jitter_buffer[i].rcvd_time = jitter_buffer[i+1].rcvd_time;
					}
					jitter_buffer_num_elements--;

			   		silenceIntervalInProgress = 1;

#ifdef PHASE_MATCHING_ENABLED
					store_pt = jitter_buffer[0].rate; //For Phase Matching
#endif

					return_rate = 1;
					return_value = 4; //silent packet is a packet
				}
				else
				{
					next_to_play++;

					store_sn = jitter_buffer[0].seq_num;
										
					for (i = 0; i < jitter_buffer_num_elements - 1; i++)
					{
						jitter_buffer[i].seq_num = jitter_buffer[i+1].seq_num;
						jitter_buffer[i].timestamp = jitter_buffer[i+1].timestamp;
						jitter_buffer[i].rate = jitter_buffer[i+1].rate;
						jitter_buffer[i].rcvd_time = jitter_buffer[i+1].rcvd_time;
					}
					jitter_buffer_num_elements--;

#ifdef PHASE_MATCHING_ENABLED
					store_pt = jitter_buffer[0].rate; //For Phase Matching
#endif
					return_value = 4;
		  		}
		   	}
		   	else
	   		{
	   			if (num_elements_underflow_array == SIZE_UNDERFLOW_ARRAY)
	   			{
	   				for (i = 0; i < SIZE_UNDERFLOW_ARRAY - 1; i++)
	   					underflow_array [i] = underflow_array [i+1];
	   				num_elements_underflow_array--;
	   			}
						
	   			underflow_array [num_elements_underflow_array] = next_to_play;
	   			num_elements_underflow_array++;

	   			consec_drops++;

	   			return_value = 14; //erasure

				if (NON_DROP_OPTION > 0)
				{
	   				if (consec_underflow_count >= NON_DROP_OPTION)
	   					next_to_play = (next_to_play + 1);
					
	   				consec_underflow_count++;
	   				if (consec_underflow_count > NON_DROP_OPTION)
	   					consec_underflow_count = NON_DROP_OPTION;
				}			 
	
				if (consec_drops >= 3)
	   			{
	   				silenceIntervalInProgress = 1;
		       		
		   		}
			}
		}
	}
	else if (jitter_buffer_num_elements == 0)
	{
		if (silenceIntervalInProgress == 0)
		//buffer underflow
		{
			if (num_elements_underflow_array == SIZE_UNDERFLOW_ARRAY)
			{
				for (i = 0; i < SIZE_UNDERFLOW_ARRAY - 1; i++)
					underflow_array[i] = underflow_array[i+1];
				num_elements_underflow_array--;
			}
				
			underflow_array[num_elements_underflow_array] = next_to_play;
			num_elements_underflow_array++;

			consec_drops++;

			return_value = 14; //erasure

			if (NON_DROP_OPTION > 0)
			{
				if (consec_underflow_count >= NON_DROP_OPTION)
					next_to_play = (next_to_play + 1);
			
				consec_underflow_count++; 
				if (consec_underflow_count > NON_DROP_OPTION)
					consec_underflow_count = NON_DROP_OPTION;
			}
			if (consec_drops >= 3)
			{
				silenceIntervalInProgress = 1;
			}
		}

		else if (silenceIntervalInProgress == 1)
		{
			return_value = 1; //silence
		}
	}

	*time_warp_fraction = 0x4000;


	if (/* NEW ADDED for CTX*/silenceIntervalInProgress == 0)
	{
		avg_buffer_num_elements = avg_buffer_num_elements * (float(FilteredDejitterSize - 1)/FilteredDejitterSize) + jitter_buffer_num_elements * (1.00/FilteredDejitterSize);

#if CRD_STDOUT
		printf ("\n Avg_Buffer Value %f %d %f", avg_buffer_num_elements, jitter_buffer_num_elements, required_jitter_length );
#endif
	}

	int check_next_pkt = 0;

	if (jitter_buffer_num_elements > 0)
	{
		if (jitter_buffer[0].seq_num == next_to_play)
		{
			check_next_pkt = 1;
		}
	}


	if (TIME_WARP_OPTION)
	{	
		if (return_rate != 1 && return_value != 14 && return_value != 1 && ((jitter_buffer_num_elements + 1) * 0.02 <= 0.3 * required_jitter_length || check_next_pkt == 0))
		{
			if (check_next_pkt != 0 && num_pkts_since_expanded < TIME_WARP_SPACING_EXPAND)
			{
			  *time_warp_fraction = 0x4000; // 1.0 in Q14
#if CRD_STDOUT
				printf ("\n Not Expanded %d", num_pkts_since_expanded);
#endif
			}
			else
			  *time_warp_fraction = 0x6ccc; // 1.7 in Q14
		}

		else if (return_rate != 1 && return_value != 14 && return_value != 1 && ((jitter_buffer_num_elements + 1) * 0.02 <= 0.75 * required_jitter_length || check_next_pkt == 0))
		{
			if (check_next_pkt != 0 && num_pkts_since_expanded < TIME_WARP_SPACING_EXPAND)
			{
			  *time_warp_fraction = 0x4000; // 1.0 in Q14
#if CRD_STDOUT
				printf ("\n Not Expanded %d", num_pkts_since_expanded);
#endif
			}
			else
			  *time_warp_fraction = 0x6ccc; // 1.7 in Q14
		}

		else if (return_rate != 1 && return_value != 14 && return_value != 1 && (avg_buffer_num_elements + 1) * 0.02 > 1 * required_jitter_length && (jitter_buffer_num_elements + 1) * 0.02 > 1 * required_jitter_length && jitter_buffer_num_elements >= 1)
		{
			if (FLAG_BUNDLING_HANDLING == 1)
                   	{
                        	if (num_pkts_since_warped >= TIME_WARP_SPACING && (jitter_buffer_num_elements > FilteredBundlingSize))
				{
				  *time_warp_fraction = 0x3000; // 0.75 in Q14
				}
				else
				{
				  *time_warp_fraction = 0x4000; // 1.0 in Q14
				}
			}
			else
			{
				if (num_pkts_since_warped >= TIME_WARP_SPACING)
				{
					*time_warp_fraction = 0x3000; // 0.75 in Q14
				}
				else
				{
					*time_warp_fraction = 0x4000; // 1.0 in Q14
				}
			}
		}
	}


 	int last_pkt_talkspurt = 0;

	if (jitter_buffer_num_elements > 0)
	{
		for (i = 0; i < jitter_buffer_num_elements; i++)
		{
			if (jitter_buffer[i].rate == 1)
			{
				last_pkt_talkspurt = 1;
				break;
			}
		}
	}

#ifdef END_ENHANCEMENT
	int last_seq_num = store_sn;

	if (jitter_buffer_num_elements > 0 && last_pkt_talkspurt == 1)
	{
		for (i = 0; i < jitter_buffer_num_elements; i++)
		{
			if (jitter_buffer[i].rate == 1)
			{
				if (jitter_buffer[i].seq_num == (last_seq_num + 1))
					break;
				else
				{
					last_pkt_talkspurt = 0;
					break;
				}
			}
			else
			{
				if (jitter_buffer[i].seq_num != last_seq_num + 1)
				{
					last_pkt_talkspurt = 0;
					break;
				}
			}
			last_seq_num = jitter_buffer[i].seq_num;
		}
	}
#endif

#ifdef END_ENHANCEMENT
	if (return_rate != 1 && return_value != 14 && return_value != 1 && last_pkt_talkspurt == 1)
	{
		if (num_pkts_since_warped >= TIME_WARP_SPACING_END)
		{
#if CRD_STDOUT
			printf ("\n END_ENHANCED %d", num_pkts_since_warped);
#endif
			*time_warp_fraction = 0x3000; // 0.75 in Q14;
		}
		else
		{
			*time_warp_fraction = 0x4000; // 1.0 in Q14
		}
	}
#endif

	if (return_value == 4)
		first_packet = 0;

#if CRD_STDOUT
	printf ("\n Returning %d packet with sn = %d %d time_warp_fraction = %#x %f %f check_next = %d", return_value, store_sn, jitter_buffer_num_elements, *time_warp_fraction, required_jitter_length, avg_buffer_num_elements, check_next_pkt);
	printf ("\n Checking next number %d", next_to_play);
#endif

#ifdef PHASE_MATCHING_ENABLED

	//Code for Phase Matching
	if (store_pt == 4 && (*run_length > 0 && *run_length < 3))
	{
		*phase_offset = *run_length + 1 - (next_to_play - next_to_play_before_erasure);
#if CRD_STDOUT
		printf ("\n Phase Offset %d %d %d %d", *phase_offset, *run_length, next_to_play, next_to_play_before_erasure);
#endif

	if (PHASE_MATCHING == 0)
		*phase_offset = 10;
	}

	if (return_value != 14)
	{
		next_to_play_before_erasure = next_to_play;
	}
	//End: Code for Phase Matching

#endif

	*global_sn = store_sn;

	if (return_rate == 1 || silenceIntervalInProgress == 1)
		num_consecutive_silence++;
	else
		num_consecutive_silence = 0;

	if (num_consecutive_silence >= NUM_SILENCE_SENTENCE)
	{
#if CRD_STDOUT
		printf ("\n NEW SENTENCE %d", num_consecutive_silence);
#endif
		first_utterance_in_sentence_passed = 0;
	}
	
	return return_value;
}


void	Algorithm5 (double rt)
{
	double test;
	double current_time = rt;

	test = (double) (num_underflows * 1.00/num_packets_reached);
	if (num_packets_reached == 0)
		test = 0;

#if CRD_STDOUT
	printf ("\n Algorithm5: %d %d %f %f %f", num_underflows, num_packets_reached, test, required_jitter_length, underflow_rate);
#endif
	if (test > 0)
		underflow_rate = ALPHA * underflow_rate + (1 - ALPHA) * test;
	else
		underflow_rate = ALPHA * underflow_rate + (1 - ALPHA) * test;
	
	test = underflow_rate;

	num_underflows = 0;
		
	if (test < TARGET_PKT_UNDERFLOW || (FLAG_DECREASING_BUFFER_DEPTH == 1 && num_stored_PERs > 0 && test < stored_PERs[num_stored_PERs-1]))
	{
		new_jitter_length -= JITTER_INCREMENT;

		if (FLAG_DECREASING_BUFFER_DEPTH == 1)
		{
			num_stored_PERs--;
			if (num_stored_PERs < 0)
				num_stored_PERs = 0;
		}
	}
	else if (test > TARGET_PKT_UNDERFLOW)
	{
		if (test > last_underflow_rate)
		{
			new_jitter_length += JITTER_INCREMENT;

			if (FLAG_DECREASING_BUFFER_DEPTH == 1)
			{
				if (new_jitter_length <= JITTER_UPPER_LIMIT)
				{
					stored_PERs[num_stored_PERs] = last_underflow_rate;
					num_stored_PERs++;
				}
			}
		}
	}

	test = underflow_rate;

	if (new_jitter_length < JITTER_LOWER_LIMIT)
		new_jitter_length = JITTER_LOWER_LIMIT;
	if (new_jitter_length > JITTER_UPPER_LIMIT)
		new_jitter_length = JITTER_UPPER_LIMIT;


	required_jitter_length = new_jitter_length;

	last_updated_time = current_time;

	last_underflow_rate = underflow_rate;
	num_packets_reached = 0;
}
