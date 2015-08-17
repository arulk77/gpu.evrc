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
#include <stdio.h>
#include <stdlib.h>

#if 1
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#else
unsigned short Swap16 (unsigned short inp)
{
	return ( (unsigned short) (((inp & 0xFF) << 8) | ((inp & 0xFF00) >> 8)));
}

unsigned int Swap32 (unsigned int inp)
{
	return ( (unsigned int) (((inp & 0xFF) << 24) | ((inp & 0xFF00) << 8) | ((inp & 0xFF0000) >> 8) | ((inp & 0xFF000000) >> 24)));
}
#endif

main (int argc, char **argv)
{
	FILE *fp1, *fp2, *fp3;

	int rtp_sn;
	short rtp_sn_BE;
	long rtp_ts, rtp_ts_BE;
	int smallest_rtp_sn = 10000000;
	double arrival_time;
	short last_rtp_sn = -1;

	long arrival_time_int;
	short rate;
	short speech_data [50];
	long size;
	int i;

	short rtp_header_field1 = 22;
	long rtp_header_field2 = 22;

	if (argc != 4) {
		fprintf(stderr,"Usage: %s Delay_Profile Encoded_pkt_file Output_file\n",argv[0]);
		exit(0);
	}
	fp1 = fopen (argv[1], "r");
	fp2 = fopen (argv[2], "rb");

	if ( (fp3 = fopen (argv[3], "wb")) == NULL)
	{
		printf ("\n Output File Not Opened");
	}

	while (!feof (fp1))
	{
		fscanf (fp1, "%d %*d %*lf", &rtp_sn);

		if (rtp_sn < smallest_rtp_sn)
			smallest_rtp_sn = rtp_sn;
	}

	fseek (fp1, 0, SEEK_SET);

	while (!feof (fp1) && !feof(fp2))
	{
		fscanf (fp1, "%d %d %lf", &rtp_sn, &rtp_ts, &arrival_time);
		printf ("\n RTP SN = %d", rtp_sn); fflush(stdout);

		if (last_rtp_sn == rtp_sn)
			break;

		last_rtp_sn = rtp_sn;

		arrival_time_int = (int) arrival_time;

		fseek (fp2, 24 * (rtp_sn - smallest_rtp_sn), SEEK_SET);
		if (fread (&rate, sizeof (short), 1, fp2) != 1) break;
		if (fread (speech_data, sizeof (short), 11, fp2) != 11) break;

		/* 
		 * NOTE: The quarter-rate (rate=2) vocoder frame size
		 * (5 bytes) fills six bytes on little-endian
		 * platforms.  Thus, RTP header (12 bytes) plus
		 * vocoder frame is 18 bytes.
		 */
		switch (rate)
		{
		case 1:
			size = 14; break;
		case 2:
			size = 18; break;
		case 3:
			size = 22; break;
		case 4:
			size = 34; break;
		}

		//Size and Packet Arrival Time
		fwrite (&size, sizeof (long), 1, fp3);
		fwrite (&arrival_time_int, sizeof (long), 1, fp3);

		//RTP Header
		fwrite (&rtp_header_field1, sizeof (short), 1, fp3);

		//rtp_sn_BE = Swap16 (rtp_sn);
		rtp_sn_BE = (htons (rtp_sn));
		fwrite (&rtp_sn_BE, sizeof (short), 1, fp3);

		//rtp_ts_BE = Swap32 (rtp_ts);
		rtp_ts_BE = htonl (rtp_ts);
		fwrite (&rtp_ts_BE, sizeof (long), 1, fp3);

		fwrite (&rtp_header_field2, sizeof (long), 1, fp3);

		//RTP Payload+
		fwrite (speech_data, sizeof (char), (size - 12), fp3);

		printf ("\n %d %d %d %d %d", rate, arrival_time_int, rtp_sn, smallest_rtp_sn, rtp_ts);
		fflush(stdout);
	}

	fclose (fp1);
	fclose (fp2);
	fclose (fp3);
}
