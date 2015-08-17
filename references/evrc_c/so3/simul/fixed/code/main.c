/**********************************************************************
Each of the companies; Motorola, Lucent, Nokia, Qualcomm, (hereinafter 
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
all Normative portions of the EVRC Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
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
/*  Memory Usage:                           */
/*      ROM:                            */
/*      Static/Global RAM:                      */
/*      Stack/Local RAM:                    */
/*----------------------------------------------------------------------*/

/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "typedefs.h"
#include "mathevrc.h"
#include "macro.h"
#include "bqiir.h"
#include "decode.h"
#include "encode.h"
#include "ns127.h"
#include "rda.h"

/*======================================================================*/
/*         ..Defines.                                                   */
/*----------------------------------------------------------------------*/

#define PROGRAM_VERSION "1.0"
#define PROGRAM_DATE    "30 Mar 1998"

#define  SPEECH_BUFFER_LEN        160
#define  LOOKAHEAD_LEN             80
#define  BITSTREAM_BUFFER_LEN      12	/*  data + 1 word for rate  */

/*======================================================================*/
/*         ..Structs.                                                   */
/*----------------------------------------------------------------------*/
typedef struct
{
	char *input_filename;
	char *output_filename;
	Shortword max_frames;
	Shortword encode_only;
	Shortword decode_only;
	Shortword max_rate;
	Shortword max_rate_default;
	Shortword min_rate;
	Shortword min_rate_default;
	Shortword post_filter;
	Shortword post_filter_default;
	Shortword noise_suppression;
	Shortword noise_suppression_default;
	Shortword ibuf_len;
	Shortword obuf_len;
}
EvrcArgs;

typedef struct
{
    Longword max_ops;
    Longword max_fnum;
    Longword min_ops;
    Longword min_fnum;
    float    avg_ops;
    char     name[80];
} op_struct;

/*======================================================================*/
/*         ..Print banner.                                              */
/*----------------------------------------------------------------------*/
void banner(FILE * fp)
{
	/*....execute.... */
	fprintf(fp, "\n\
---------------------------------------------------------------------\n\
>>>>>>>>>>>>>>>>> CDMA Enhanced Variable-Rate Codec <<<<<<<<<<<<<<<<<\n\
<<<<<<<<<<<<<<<<<<<<<< Fixed-point C simulation >>>>>>>>>>>>>>>>>>>>>\n\
>>>>>>>>>>>>>>>>>>> - Version %s, %s - <<<<<<<<<<<<<<<<<<<\n\
---------------------------------------------------------------------\n",
			PROGRAM_VERSION,
			PROGRAM_DATE);
}

/*======================================================================*/
/*         ..Usage.                                                     */
/*----------------------------------------------------------------------*/
void usage(	  FILE * fp,
			  char *prog_name)
{
	/*....execute.... */
	banner(fp);
	fprintf(fp, "\
 usage:\n\
       %s  <required_args>  [optional_args]\n\
\n\
   required_args:\n\
       -i <filename>  Input filename.\n\
       -o <filename>  Output filename.\n\
\n\
   optional_args:\n\
       -e             Encode only (infile=speech, outfile = bitstream).\n\
       -d             Decode only (infile=bitstream, outfile = speech).\n\
       -h <rate>      Maximum rate, 1(eighth),3(half), or 4(full); (default=4).\n\
       -l <rate>      Minimum rate, 1(eighth),3(half), or 4(full); (default=1).\n\
       -p <1 or 0>    Enable (1)/disable (0) post filter, (default=1).\n\
       -n <1 or 0>    Enable (1)/disable (0) noise suppression, (default=1).\n\
       -f <maxframe>  Set maximum number of frames to process.\n\
\n",
			prog_name
		);
}

/*======================================================================*/
/*         ..Print command line arguments.                              */
/*----------------------------------------------------------------------*/
void print_eargs(	FILE *fp,
					EvrcArgs *e)
{
	/*....execute.... */
	banner(fp);
	if (e->encode_only && !e->decode_only)
	{
		e->ibuf_len = SPEECH_BUFFER_LEN;
		e->obuf_len = BITSTREAM_BUFFER_LEN - 1;
		fprintf(fp, "          input_speechfile == \"%s\"\n", e->input_filename);
		fprintf(fp, "          output_bitstream => \"%s\"\n", e->output_filename);
	}
	else if (e->decode_only && !e->encode_only)
	{
		e->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
		e->obuf_len = SPEECH_BUFFER_LEN;
		fprintf(fp, "           input_bitstream => \"%s\"\n", e->input_filename);
		fprintf(fp, "         output_speechfile == \"%s\"\n", e->output_filename);
	}
	else if (!e->encode_only && !e->decode_only)
	{
		e->ibuf_len = SPEECH_BUFFER_LEN;
		e->obuf_len = SPEECH_BUFFER_LEN;
		fprintf(fp, "          input_speechfile == \"%s\"\n", e->input_filename);
		fprintf(fp, "         output_speechfile == \"%s\"\n", e->output_filename);
	}
	if (e->max_rate_default)
	{
		fprintf(fp, "                  max_rate == %d\n", e->max_rate);
	}
	else
	{
		fprintf(fp, "                  max_rate => %d\n", e->max_rate);
	}
	if (e->min_rate_default)
	{
		fprintf(fp, "                  min_rate == %d\n", e->min_rate);
	}
	else
	{
		fprintf(fp, "                  min_rate => %d\n", e->min_rate);
	}
	if (e->post_filter)
	{
		if (e->post_filter_default)
		{
			fprintf(fp, "               post_filter == ON\n");
		}
		else
		{
			fprintf(fp, "               post_filter => ON\n");
		}
	}
	else
	{
		if (e->post_filter_default)
		{
			fprintf(fp, "               post_filter == OFF\n");
		}
		else
		{
			fprintf(fp, "               post_filter => OFF\n");
		}
	}
	if (e->noise_suppression)
	{
		if (e->noise_suppression_default)
		{
			fprintf(fp, "         noise_suppression == ON\n");
		}
		else
		{
			fprintf(fp, "         noise_suppression => ON\n");
		}
	}
	else
	{
		if (e->noise_suppression_default)
		{
			fprintf(fp, "         noise_suppression == OFF\n");
		}
		else
		{
			fprintf(fp, "         noise_suppression => OFF\n");
		}
	}
	fprintf(fp, "---------------------------------------------------------------------\n");
}

/*======================================================================*/
/*         ..Get command line arguments.                                */
/*----------------------------------------------------------------------*/
EvrcArgs *get_eargs(
					   int argc,
					   char *argv[]
)
{
	/*....(local) variables.... */
	EvrcArgs *eargs;
	Shortword option;

	/*....execute.... */
	if (argc < 5)
	{
		eargs = NULL;
	}
	else
	{
		if ((eargs = (EvrcArgs *) malloc(sizeof(EvrcArgs))) == NULL)
		{
			fprintf(stderr, "%s:  ERROR - Unable to malloc arg memory.\n", argv[0]);
			exit(-1);
		}
		eargs->input_filename = NULL;
		eargs->output_filename = NULL;
		eargs->encode_only = 0;
		eargs->decode_only = 0;
		eargs->max_rate = 4;
		eargs->max_rate_default = 4;
		eargs->min_rate = 1;
		eargs->min_rate_default = 1;
		eargs->post_filter = 1;
		eargs->post_filter_default = 1;
		eargs->noise_suppression = 1;
		eargs->noise_suppression_default = 1;
		eargs->ibuf_len = SPEECH_BUFFER_LEN;
		eargs->obuf_len = SPEECH_BUFFER_LEN;
		eargs->max_frames = 0x7fff;

        while ((option = getopt(argc, argv, "i:o:redh:l:p:n:c:f:")) != EOF)
		{
			switch (option)
			{
			case 'i':
				eargs->input_filename = argv[optind - 1];
				break;
			case 'o':
				eargs->output_filename = argv[optind - 1];
				break;
			case 'e':
				eargs->encode_only = 1;
				if (eargs->decode_only)
				{
					fprintf(stderr, "%s:  ERROR - Encode with decode.\n", argv[0]);
					exit(-1);
				}
				break;
			case 'd':
				eargs->decode_only = 1;
				if (eargs->encode_only)
				{
					fprintf(stderr, "%s:  ERROR - Decode with encode.\n", argv[0]);
					exit(-1);
				}
				break;
			case 'h':
				eargs->max_rate = (Shortword) atoi(argv[optind - 1]);
				if (eargs->max_rate > 4)
					eargs->max_rate = 4;
				if (eargs->max_rate < 1)
					eargs->max_rate = 1;
				eargs->max_rate_default = 0;
				break;
			case 'l':
				eargs->min_rate = (Shortword) atoi(argv[optind - 1]);
				if (eargs->min_rate > 4)
					eargs->min_rate = 4;
				if (eargs->min_rate < 1)
					eargs->min_rate = 1;
				eargs->min_rate_default = 0;
				break;
			case 'p':
				eargs->post_filter = (Shortword) atoi(argv[optind - 1]);
				if (eargs->post_filter != 0)
					eargs->post_filter = 1;
				eargs->post_filter_default = 0;
				break;
			case 'n':
				eargs->noise_suppression = (Shortword) atoi(argv[optind - 1]);
				if (eargs->noise_suppression != 0)
					eargs->noise_suppression = 1;
				eargs->noise_suppression_default = 0;
				break;
			case 'f':
				eargs->max_frames = (Shortword) atoi(argv[optind - 1]);
				break;
            }
		}
		if (eargs->min_rate > eargs->max_rate)
		{
			eargs->min_rate = eargs->max_rate;
		}
	}
	return (eargs);
}

/*======================================================================*/
/*         ..Returns number of frames in a binary file.                 */
/*----------------------------------------------------------------------*/

Shortword GetNumFrames(	  FILE * fp,
						  Shortword blocksize)
{
	/*....(local) variables.... */
	Shortword position;
	Shortword numFrames;

	/*....execute.... */
	position = ftell(fp);
	fseek(fp, 0L, 2);
	numFrames = ftell(fp) / blocksize;
	fseek(fp, position, 0);
	return (numFrames);
}

void reset_op_stats(op_struct *op_stat, char *name)
{
    op_stat->avg_ops=0;
    op_stat->max_ops=0;
    op_stat->min_ops=0x7fffffffL;
    strcpy(op_stat->name,name);
}

void update_op_stats(op_struct *op_stat, Longword ops, Longword fnum)
{
    op_stat->avg_ops+=ops;

    if (ops > op_stat->max_ops)
    {
        op_stat->max_ops=ops;
        op_stat->max_fnum=fnum;
    }
    if (ops < op_stat->min_ops)
    {
        op_stat->min_ops=ops;
        op_stat->min_fnum=fnum;
    }
}

void print_op_stats(FILE *fp, op_struct *op_stat, Longword fnum)
{
    float avg;
    float avg_mops;

    avg = op_stat->avg_ops/(float)fnum;
    avg_mops = avg*50.0*1.0e-6;

    fprintf(fp,"%s Average OPS/Frame : %6.0f\n",op_stat->name,avg);
    fprintf(fp,"%s Average MOPS      : %2.03f\n",op_stat->name,avg_mops);
    fprintf(fp,"%s Max OPS/Frame     : %ld\n",op_stat->name,op_stat->max_ops);
    fprintf(fp,"%s Max OPS at Frame #: %ld\n",op_stat->name,op_stat->max_fnum);
    fprintf(fp,"%s Min OPS/Frame     : %ld\n",op_stat->name,op_stat->min_ops);
    fprintf(fp,"%s Min OPS at Frame #: %ld\n\n",op_stat->name,op_stat->min_fnum);
}

/*======================================================================*/
/*         ..Speech encoder.                                            */
/*----------------------------------------------------------------------*/
void Encode(
			   Shortword rate,
			   Shortword * obuffer
)
{
	/*....execute.... */
	encode(rate, obuffer);
	post_encode();
}

/*======================================================================*/
/*         ..Speech decoder.                                            */
/*----------------------------------------------------------------------*/
void Decode(
			   Shortword * ibuffer,
			   Shortword rate,
			   Shortword post_filter,
			   Shortword * obuffer
)
{
	/*....execute.... */
	decode(ibuffer, rate, post_filter, obuffer);
}

/*======================================================================*/
/*         ..Main.                                                      */
/*----------------------------------------------------------------------*/
void main(int argc, char *argv[])
{
	/*....(local) variables.... */
	EvrcArgs *eargs;

    op_struct encode_ops;
    op_struct decode_ops;
    op_struct ns_ops;
    op_struct rda_ops;
    op_struct hpf_ops;
    op_struct pe_ops;

	FILE *ifileP;
	FILE *ofileP;

	Shortword buf[SPEECH_BUFFER_LEN];
    Shortword buf16[BITSTREAM_BUFFER_LEN];

	Shortword buf_count;
	Shortword ibuf_len;
	Shortword obuf_len;
    Shortword j;
	Shortword rate;
	Shortword beta;
    float rate_sum;
    float avg_rate;
	Longword R[17];

    Shortword k;

    /*....execute.... */
	/*...get arguments and check usage... */
	if ((eargs = get_eargs(argc, argv)) == NULL)
	{
		usage(stdout, argv[0]);
		exit(-1);
	}
	print_eargs(stdout, eargs);

	/*...open files... */
	if ((ifileP = fopen(eargs->input_filename, "rb")) == NULL)
	{
		fprintf(stderr, "%s:  ERROR - Unable to open input file \"%s\".\n",
				argv[0],
				eargs->input_filename
			);
		exit(-1);
	}
	if ((ofileP = fopen(eargs->output_filename, "wb")) == NULL)
	{
		fprintf(stderr, "%s:  ERROR - Unable to open output file \"%s\".\n",
				argv[0],
				eargs->output_filename
			);
		exit(-1);
	}

	/*...loop counter max... */
	if (eargs->decode_only)
	{
		buf_count = GetNumFrames(ifileP, sizeof(Shortword) * BITSTREAM_BUFFER_LEN);
	}
	else
	{
		buf_count = GetNumFrames(ifileP, sizeof(Shortword) * SPEECH_BUFFER_LEN);
	}


    /*...processing loop... */
    reset_op_stats(&encode_ops,"Encode\0");
    reset_op_stats(&decode_ops,"Decode\0");
    reset_op_stats(&pe_ops,"Pre-Encode\0");
    reset_op_stats(&ns_ops,"Noise Supression\0");
    reset_op_stats(&hpf_ops,"BQIIR\0");
    reset_op_stats(&rda_ops,"RDA\0");

	InitEncoder();
	InitDecoder();

	rate_sum = 0.0;
	avg_rate = 0.0;

	ibuf_len = eargs->ibuf_len;
	obuf_len = eargs->obuf_len;
	j = 1;

	if (!eargs->decode_only)
	{
        while (((fread(buf, sizeof(Shortword), ibuf_len, ifileP)) == ibuf_len)
				&& (j<=eargs->max_frames))
		{
		    OP_RESET;
		    bqiir(buf);
		    update_op_stats(&hpf_ops,op_counter,j);

			if (eargs->noise_suppression)
			{
				OP_RESET;
				noise_suprs(buf);
				noise_suprs(buf + ibuf_len / 2);
			} else {
				for (k=0; k<ibuf_len; k++) {
					buf[k] = shift_r(buf[k],-1);
				}
			}

            update_op_stats(&ns_ops,op_counter,j);
            OP_RESET;

			beta = pre_encode(buf, R);
            update_op_stats(&pe_ops,op_counter,j);
            OP_RESET;

            rate = select_rate(R, eargs->max_rate, eargs->min_rate, beta);
            update_op_stats(&rda_ops,op_counter,j);
            OP_RESET;

			switch (rate)
			{
			case 1:
				rate_sum += 1;
				break;
			case 3:
				rate_sum += 4;
				break;
			default:
				rate_sum += 8;
				break;
			}
			avg_rate = (rate_sum / (float) j) * 1.2;

            Encode(rate, buf16);
            update_op_stats(&encode_ops, op_counter, j);
            OP_RESET;

            if (eargs->encode_only)
			{
				fwrite(&rate, sizeof(Shortword), 1, ofileP);
                fwrite(buf16, sizeof(Shortword), obuf_len, ofileP);
				fprintf(stderr, "Encoding %d of %d   avg_rate= %6.2f\r", j, buf_count, avg_rate);
			}
			else
			{

                Decode(buf16, rate, eargs->post_filter, buf);
                update_op_stats(&decode_ops, op_counter, j);

                fwrite(buf, sizeof(Shortword), obuf_len, ofileP);
				fprintf(stderr, "Encoding/Decoding %d of %d   avg_rate= %6.2f\r", j, buf_count, avg_rate);
			}
			j++;
		}
	}
	else
	{
		while ((fread(&rate, sizeof(Shortword), 1, ifileP)) == 1)
		{
            if ((fread(buf16, sizeof(Shortword), ibuf_len, ifileP)) == ibuf_len)
			{
                OP_RESET;
                Decode(buf16, rate, eargs->post_filter, buf);
                update_op_stats(&decode_ops, op_counter, j);

                fprintf(stderr, "Decoding %d of %d\r", j, buf_count);
                fwrite(buf, sizeof(Shortword), obuf_len, ofileP);
				j++;
			}
		}
	}
	fprintf(stderr, "\n");
}

