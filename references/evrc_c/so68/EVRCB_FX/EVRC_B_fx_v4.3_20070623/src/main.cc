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

static char const rcsid[]="$Id: main.cc,v 1.83 2007/06/24 11:45:12 apadmana Exp $";

/*======================================================================*/
/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for           */
/*  Wideband Spread Spectrum Digital System                             */
/*  C Source Code Simulation                                            */
/*                                                                      */
/*  Copyright (C) 2004-05 Qualcomm Incorporated. All rights             */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "proto_fx.h"
#include "globs_fx.h"
#include "basic_op40.h"
#include "basic_op.h"
#include "dsp_math.h"


#ifdef WMOPS_FX
#include "lib_wmp_fx.h"
#endif


#include "tty.h"
#include "dtmf.h"

#define PROGRAM_VERSION "4.3"
#define PROGRAM_DATE    "13 Jun 2007"

#ifdef WMOPS_FX
int WIcallcount=0;
#endif //WMOPS_FX

#ifdef VOIP_PARSE
typedef struct {
	int phase_offset;
	int run_length;
	float twf;
	int  frame_rate;
	short int buf16[11];
	} iostruct ;

 iostruct decpar_struct;
#endif



/* Global variable */

 int parse_info=0;
 
 FILE *parfileP;

//EvrcArgs*  Eargs;
/*======================================================================*/
/*         ..Print .                                              */
/*----------------------------------------------------------------------*/
void  banner (FILE* fp)
{
  /*....execute....*/
  fprintf(fp,"\n---------------------------------------------------------------------\n>>>>>>>>>>>>>>>>>    CDMA Enhanced Variable Rate Coder - B   <<<<<<<<<<<<<<<<<\n>>>>>>>>>>>>>>>>>>>> - Version %s, %s - <<<<<<<<<<<<<<<<<<<\n---------------------------------------------------------------------\n",PROGRAM_VERSION,PROGRAM_DATE);
}

/*======================================================================*/
/*         ..Usage_ed.                                                  */
/*----------------------------------------------------------------------*/
void  usage_ed (FILE*  fp,char*  prog_name)
{
	/*....execute....*/
  banner(fp);
  fprintf(fp,"usage:\n%s  <required_args>  [optional_args]\n\n",prog_name);
  fprintf(fp,"required_args:\n");
  fprintf(fp,"\t-i <filename>   Input filename.\n");
  fprintf(fp,"\t-o <filename>   Output filename.\n");
  fprintf(fp,"optional_args:\n");
  fprintf(fp,"\t-h              Print this message.\n");
  fprintf(fp,"\t-e              Encode only (infile=speech, outfile = bitstream).\n");
  fprintf(fp,"\t-d              Decode only (infile=bitstream, outfile = speech).\n");
  fprintf(fp,"\t-M <rate>       Maximum rate, 1(1/8),2(1/4),3(1/2) or 4; (default=4).\n");
  fprintf(fp,"\t-m <rate>       Minimum rate, 1(1/8),2(1/4),3(1/2) or 4; (default=1).\n");
  fprintf(fp,"\t-p <1 or 0>     Enable (1)/disable (0) post filter, (default=1).\n");
  fprintf(fp,"\t-n <1 or 0>     Enable (1)/disable (0) noise suppression, (default=1).\n");
  fprintf(fp,"\t-H              Enable front end IIR high-pass filter\n");
  //fprintf(fp,"\t-c              Do open-loop rate calibration only, no coding\n");
  //fprintf(fp,"\t-u              Keep LSP unquantized.\n");
  fprintf(fp,"\t-f <#>          Stop after processing # frames\n");
  fprintf(fp,"\t-s <signalling file>  Use signalling file \n");
  //fprintf(fp,"\t-s <#>          Starting frame number, default 0\n");
  //fprintf(fp,"\t-r              Write formant residual to stdout\n");
  //fprintf(fp,"\t-a              Write quantized formant residual to stdout\n");
  //fprintf(fp,"\t-R              Write modified residual to stdout\n");
  //fprintf(fp,"\t-E              Write target speech at the encoder to stdout\n");
  //fprintf(fp,"\t-G              Write rate to stdout\n");
  //fprintf(fp,"\t-O              Write lag to stdout\n");
  //fprintf(fp,"\t-N              Write current frame NACF to stdout\n");
  //fprintf(fp,"\t-P              Unquantized prototype in half rate\n");
  //fprintf(fp,"\t-C <1|2|3|4>    Unquantized 1/8, 1/4, 1/2 or full rate  frames\n");
  //fprintf(fp,"\t-I <rate file>  Use external rate file to overwrite rate decision\n");
  //fprintf(fp,"\t-k              Write packet to stdout\n");
  //fprintf(fp,"\t-v              Write VAD to stdout \n");
  //fprintf(fp,"\t-A <[F|f]#1<R|r>#1[<F|f>#2<R|r>#2 ...]> Force frame <#1> to rate <#1>,\n\t                up to 10 frames\n");
  //fprintf(fp,"\t-F              Control PPP threshold to keep avg rate fixed\n");
  //fprintf(fp,"\t-T <threshold>  The value of the above threshold (Default:-150.0dB)\n");
  fprintf(fp,"\t-W <avg_rate>   Try to keep active speech average channel rate within \n\t                 a range of the specified number in bps \n");
  //fprintf(fp,"\t-w <rate win>   Use this number of active frames over which to compute\n\t                average rate (default 100)\n");
  fprintf(fp,"\t-X OP=<options> Anchor Operating point <options> are 0, 1, or 2\n");
  fprintf(fp,"\t-X Target_ADR_file=<target ADR file> Use a target ADR binary file which has an ADR entry (in bps) for every frame\n");
#ifdef VOIP_DORA
   //Added for Support of Phase Matching/Warping
  fprintf(fp,"\t-Y <1 or 0>     Enable (1)/disable (0) Phase Matching\n");
  fprintf(fp,"\t-Z <1 or 0>     Enable (1)/disable (0) concealing double erasures using Phase Matching\n");
  fprintf(fp,"\t-V <1 or 0>     Enable (1)/disable (0) Time Warping\n");
  fprintf(fp,"\t-x <filename>   DecVOIP I/O Parameter filename \n" );
  //End: Added for Support of Phase Matching/Warping
#endif
  fprintf(fp,"\t-D <1 or 0>     Enable (1)/disable (0) DTX\n");
  
  fprintf(fp,"\n");
}

/*======================================================================*/
/*         ..Usage_e.                                                   */
/*----------------------------------------------------------------------*/
void  usage_e (FILE*  fp,char*  prog_name)
{
	/*....execute....*/
  banner(fp);
  fprintf(fp,"usage:\n%s  <required_args>  [optional_args]\n\n",prog_name);
  fprintf(fp,"required_args:\n");
  fprintf(fp,"\t <filename>   Input speech filename.\n");
  fprintf(fp,"\t <filename>   Output bitstream filename.\n");
  fprintf(fp,"\t-m M  operating point 0, or 1, or 2: Default is OP2\n");
  fprintf(fp,"optional_args:\n");
  fprintf(fp,"\t-f <mode file>  Use external file for external override switching modes\n");
  fprintf(fp,"\t-s <signalling file>  Use signalling file \n");
  fprintf(fp,"\t-v To run executable in verbose mode \n");
  fprintf(fp,"\n");
}

/*======================================================================*/
/*         ..Usage_d.                                                   */
/*----------------------------------------------------------------------*/
void  usage_d (FILE*  fp,char*  prog_name)
{
	/*....execute....*/
  banner(fp);
  fprintf(fp,"usage:\n%s  <input_bitstream_file>  <output_speech_file> [-v]\n\n",prog_name);
  fprintf(fp,"\t-v To run executable in verbose mode \n");
  fprintf(fp,"\n");
}

/*======================================================================*/
/*         ..Print command line arguments.                              */
/*----------------------------------------------------------------------*/
void  print_eargs_ed (FILE* fp,EvrcArgs* e)
{
  int16 i;
  /*....execute....*/
  banner(fp);
#ifdef WMOPS_FX
  test();
  logic16();
  Nmove16(2);
#endif
  if (e->encode_only && !e->decode_only) {
    e->ibuf_len = SPEECH_BUFFER_LEN;
    e->obuf_len = BITSTREAM_BUFFER_LEN - 1;

    fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
    fprintf(fp,"         output_bitstream => \"%s\"\n",e->output_filename);
    
  }
  else
    if (e->decode_only && !e->encode_only) {
      e->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
      e->obuf_len = SPEECH_BUFFER_LEN;
      fprintf(fp,"          input_bitstream => \"%s\"\n",e->input_filename);
      fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
    }
    else
      if (!e->encode_only && !e->decode_only) {
	e->ibuf_len = SPEECH_BUFFER_LEN;
	e->obuf_len = SPEECH_BUFFER_LEN;
	fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
	fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
      }

#ifdef WMOPS_FX
  test();
#endif
  if (e->max_rate_default) {
    fprintf(fp,"                  max_rate == %d\n",    e->max_rate);
  }
  else {
    fprintf(fp,"                  max_rate => %d\n",    e->max_rate);
  }

#ifdef WMOPS_FX
  test();
#endif
  if (e->min_rate_default) {
    fprintf(fp,"                  min_rate == %d\n",    e->min_rate);
  }
  else {
    fprintf(fp,"                  min_rate => %d\n",    e->min_rate);
  }

#ifdef WMOPS_FX
  test();
#endif
  if (e->post_filter) {
#ifdef WMOPS_FX
  test();
#endif
    if (e->post_filter_default) {
      fprintf(fp,"               post_filter == ON\n");
    }
    else {
      fprintf(fp,"               post_filter => ON\n");
    }
  }
  else {
#ifdef WMOPS_FX
  test();
#endif
    if (e->post_filter_default) {
      fprintf(fp,"               post_filter == OFF\n");
    }
    else {
      fprintf(fp,"               post_filter => OFF\n");
    }
  }

#ifdef WMOPS_FX
  test();
#endif
  if (e->noise_suppression) {
#ifdef WMOPS_FX
  test();
#endif
    if (e->noise_suppression_default) {
      fprintf(fp,"         noise_suppression == ON\n");
    }
    else {
      fprintf(fp,"         noise_suppression => ON\n");
    }
  } else {
#ifdef WMOPS_FX
  test();
#endif
    if (e->noise_suppression_default) {      
      fprintf(fp,"         noise_suppression == OFF\n");
    }
    else {
      fprintf(fp,"         noise_suppression => OFF\n");
    }
  }

#ifdef WMOPS_FX
  test();
#endif
  if (e->avg_rate_control==0) 
    fprintf(fp,"\tUsing Anchor Operating points\n");
  else if (e->avg_rate_control==1)  
    fprintf(fp,"\tControlling Active Speech Average Rate to %f kbps\n",e->avg_rate_target/1000.0);
  else if (e->avg_rate_control==2)
    fprintf(fp,"\tDynamic (frame level) ADR control\n");
  if (e->highpass_filter)
    fprintf(fp,"\tFront-end highpass filter == ON\n");
  else
    fprintf(fp,"\tFront-end highpass filter == OFF\n");
    
  if (e->rfileP)
    fprintf(fp,"\tUse external rate file \"%s\"\n",e->rate_filename);
  if (e->unquantized_lsp)
    fprintf(fp,"\tLSP not quantized\n");
  if (e->starting_frame_num>0)
    fprintf(fp,"\tStart processing at frame %d\n",e->starting_frame_num);
  if(e-> partial_file_processing)
    fprintf(fp,"\tProcess only %d frames\n",e->num_frames);
  if(e-> form_res_out)
    fprintf(fp,"\tWriting original formant residual to stdout\n");
  if(e-> qform_res_out)
    fprintf(fp,"\tWriting quantized formant residual to stdout\n");
  if(e-> mform_res_out)
    fprintf(fp,"\tWriting RCELP residual to stdout\n");
  if(e-> target_speech_out)
    fprintf(fp,"\tWriting encoder target speech to stdout\n");
  if(e-> rate_out)
    fprintf(fp,"\tWriting rate to stdout\n");
  if(e-> olr_calibration)
    fprintf(fp,"\tDo open-loop rate calibration only, no coding\n");
  if(e-> lag_out)
    fprintf(fp,"\tWriting pitch to stdout\n");
  if(e-> nacf_out)
    fprintf(fp,"\tWriting current NACF to stdout\n");
  if(e-> packet_out)
    fprintf(fp,"\tWriting packet to stdout\n");
  if(e-> unquantized_prototype)
    fprintf(fp,"\tPPP prototype not quantized\n");
  if(e-> unquantized_zero_rate)
    fprintf(fp,"\tEighth rate frames not quantized\n");
  if(e-> unquantized_quarter_rate)
    fprintf(fp,"\tQuarter rate frames not quantized\n");
  if(e-> unquantized_half_rate)
    fprintf(fp,"\tHalf rate frames not quantized\n");
  if(e-> unquantized_full_rate)
    fprintf(fp,"\tFull rate frames not quantized\n");
  if(e-> vad_out)
    fprintf(fp,"\tWriting VAD to stdout \n");
  if(e-> forced_count>0)
    for (i=0;i<e->forced_count;i++)
      fprintf(fp,"\tFrame %d is forced to rate %d\n",e->forced_frame[i], e->forced_rate[i]);  
  /*  if(e-> accshift_out)
    fprintf(fp,"\tWrite accumulated shift to stdout\n");
    */
  fprintf(fp,"---------------------------------------------------------------------\n");
  if (e->avg_rate_control==0 && !e->decode_only) 
    fprintf(fp,"\tRunning in Achor Operating point, OP-%d\n",e->operating_point);
  //if (e->operating_point==2) fprintf(fp,"\t'%s' Pattern used\n",e->pattern);
  if (e->signalling_file!=NULL) fprintf(fp,"\tDimming being caused\n");
  if (e->erasure_file!=NULL) fprintf(fp,"\tErasures being inserted\n");
  if (e->dtx==1) fprintf(fp,"\tDTX enabled \n");
    
}

void print_eargs_e(FILE* fp,EvrcArgs* e)
{
   /*....execute....*/
  if (e->verbose) banner(fp);
  if (e->encode_only && !e->decode_only) {
    e->ibuf_len = SPEECH_BUFFER_LEN;
    e->obuf_len = BITSTREAM_BUFFER_LEN - 1;

    if (e->verbose) {
      fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
      fprintf(fp,"         output_bitstream => \"%s\"\n",e->output_filename);
    }
    
  }
  else
    if (e->decode_only && !e->encode_only) {
      e->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
      e->obuf_len = SPEECH_BUFFER_LEN;
      fprintf(fp,"          input_bitstream => \"%s\"\n",e->input_filename);
      fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
    }
    else
      if (!e->encode_only && !e->decode_only) {
	e->ibuf_len = SPEECH_BUFFER_LEN;
	e->obuf_len = SPEECH_BUFFER_LEN;
	fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
	fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
      }

  if (e->verbose) {
    fprintf(fp,"\tRunning in OP-%d\n",e->operating_point);
    if (e->signalling_file!=NULL) fprintf(fp,"\tDimming being caused\n");
  }
    
}

void print_eargs_d(FILE* fp,EvrcArgs* e)
{
   /*....execute....*/
  if (e->verbose) banner(fp);
  if (e->encode_only && !e->decode_only) {
    e->ibuf_len = SPEECH_BUFFER_LEN;
    e->obuf_len = BITSTREAM_BUFFER_LEN - 1;

    fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
    fprintf(fp,"         output_bitstream => \"%s\"\n",e->output_filename);
    
  }
  else
    if (e->decode_only && !e->encode_only) {
      e->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
      e->obuf_len = SPEECH_BUFFER_LEN;
      if (e->verbose) {
        fprintf(fp,"          input_bitstream => \"%s\"\n",e->input_filename);
        fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
      }
    }
    else
      if (!e->encode_only && !e->decode_only) {
	e->ibuf_len = SPEECH_BUFFER_LEN;
	e->obuf_len = SPEECH_BUFFER_LEN;
	fprintf(fp,"         input_speechfile == \"%s\"\n",e->input_filename);
	fprintf(fp,"        output_speechfile == \"%s\"\n",e->output_filename);
      }
  
    if (e->signalling_file!=NULL) 
      if (e->verbose) fprintf(fp,"\tDimming being caused\n");
}

/*======================================================================*/
/*         ..Get command line arguments.                                */
/*----------------------------------------------------------------------*/
extern int optind;

EvrcArgs*  get_eargs (int    argc,char*  argv[])
{
  /*....(local) variables....*/
  EvrcArgs*  eargs;
  int16      option;

  char frame_rate[80];

  char *num;
  
  if (strstr(argv[0],"_enc")!=NULL) {
    if (argc<5 || argc>10) eargs=NULL;
    else {
      if ((eargs=(EvrcArgs*)malloc(sizeof(EvrcArgs))) == NULL) {
	fprintf(stderr,"%s:  ERROR - Unable to malloc arg memory.\n",argv[0]);
	exit(-1);
      }
      eargs->encode_only=1; eargs->decode_only=0;
    }
  }
  else if (strstr(argv[0],"_dec")!=NULL) {
    if (argc<3 || argc>4) eargs=NULL;
    else {
      if ((eargs=(EvrcArgs*)malloc(sizeof(EvrcArgs))) == NULL) {
	fprintf(stderr,"%s:  ERROR - Unable to malloc arg memory.\n",argv[0]);
	exit(-1);
      }
      eargs->encode_only=0; eargs->decode_only=1;
    }
  }
  else {
    if (argc<5) eargs=NULL;
    else {
      if ((eargs=(EvrcArgs*)malloc(sizeof(EvrcArgs))) == NULL) {
	fprintf(stderr,"%s:  ERROR - Unable to malloc arg memory.\n",argv[0]);
	exit(-1);
      }
      eargs->encode_only=0; eargs->decode_only=0;
    }
  }

  if (eargs!=NULL) {
    eargs->input_filename    = NULL;
    eargs->output_filename   = NULL;
    eargs->max_rate          = 4;    eargs->max_rate_default          = 1;
    eargs->min_rate          = 1;    eargs->min_rate_default          = 1;
    eargs->post_filter       = 1;    eargs->post_filter_default       = 1;
    eargs->noise_suppression = 1;    eargs->noise_suppression_default = 1;
    eargs->ibuf_len          = SPEECH_BUFFER_LEN;
    eargs->obuf_len          = SPEECH_BUFFER_LEN;
    
    eargs->highpass_filter=1;
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
    eargs-> ratewin=600;
#ifdef VOIP_DORA
    eargs->operating_point=1;
#else
    eargs->operating_point=2;
#endif
    
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
    if (strstr(argv[0],"_enc")!=NULL) {
      //int fileflag=0;
      while ((option=getopt(argc,argv,"m:f:s:v")) != EOF) {
	//while (optind<argc) {}
	// option=getopt(argc,argv,"m:f:s:v");
	switch (option) {
	case 'm':
	  eargs->operating_point = (int16)atoi(argv[optind-1]);
	  break;
	case 'f':
	  break;
	case 's':
	  if ((eargs->signalling_file=fopen(argv[optind-1],"rb"))==NULL) {
	    fprintf(stderr,"Cannot Open Signalling File %s - ",argv[optind-1]);
	    fprintf(stderr,"Running without dim and burst\n");
	  }
	  break;
        case 'v':
          eargs->verbose=YES;
          break; 
	default:
	  //if (fileflag==0) {eargs->input_filename=argv[optind];fileflag++;}
	  //else if(fileflag==1){eargs->output_filename=argv[optind];fileflag++;}
	  //else {printf("Illegal option, optind = %d\n", optind);}
	  //optind++;
	  printf("Illegal option, optind = %d, option = %s\n", optind, argv[optind-1]);
	  usage_e(stderr,argv[0]);
	  exit(-1);
	}
      }
      eargs->input_filename=argv[argc-2];
      eargs->output_filename=argv[argc-1];
      return eargs;
    }
    else if (strstr(argv[0],"_dec")!=NULL) {
      //int fileflag=0;
      //while (optind<argc) {}
      //option=getopt(argc,argv,"v");
      while ((option=getopt(argc,argv,"v")) != EOF) {
	switch (option) {
        case 'v':
          eargs->verbose=YES;
          break; 
	default:
	  //if (fileflag==0) {eargs->input_filename=argv[optind];fileflag++;}
	  //else if(fileflag==1){eargs->output_filename=argv[optind];fileflag++;}
	  //else {printf("Illegal option\n");}
	  //optind++;
	  printf("Illegal option, optind = %d, option = %s\n", optind, argv[optind-1]);
	  usage_e(stderr,argv[0]);
	  exit(-1);
	  //break;
	}
      }
      eargs->input_filename=argv[argc-2];
      eargs->output_filename=argv[argc-1];
      return eargs;
    }
    else {
      eargs->verbose=YES;
      while ((option=getopt(argc,argv,"X:aA:cC:dEef:FGhHi:I:kM:m:Oo:Pp:Nn:Rrs:T:uvW:x:Y:Z:V:D:")) != EOF) {
	switch (option) {
#ifdef VOIP_DORA
		//Added for Support of Phase Matching/Warping
	case 'Y':
      eargs->phase_matching = atoi(argv[optind-1]);
	  break;
	case 'Z':
      eargs->double_erasures_pm = atoi(argv[optind-1]);
	  break;
	case 'V':
      eargs->time_warping = atoi(argv[optind-1]);
	  break;
    //End:Added for Support of Phase Matching/Warping
#ifdef VOIP_PARSE
      case 'x':
	  parse_info = 1;
	  eargs->dec_parfile = argv[optind-1];
      
  
	  break;
#endif //VOIP_PARSE

#endif

	case 'X':
	  {
	    char arg[200],*sx=arg,name[20],value[200],*p;
	    strcpy(arg,argv[optind-1]);
	    int i=0,nc=0,nt=0;
	    do {
	      char *p1=p=strchr(sx,',');
	      if (p==NULL) nc=strlen(sx); else nc=p-sx;
	      p=strchr(sx,'=');
	      if (p==NULL) {
		fprintf(stderr,"Bad Format for option 'X'\n");
		exit(-1);
	      }
	      else nt=p-sx;
	      strncpy(name,sx,nt);name[nt]='\0';
	      strncpy(value,sx+nt+1,nc-nt-1);value[nc-nt-1]='\0';
	      if (!strcmp(name,"OP")) {
		sscanf(value,"%d",&eargs->operating_point);
		if (eargs->operating_point<0||eargs->operating_point>2) {
		  fprintf(stderr,"Bad Operating Point %s\n",value);
		  exit(-1);
		}
	      }
	      else if (!strcmp(name,"pattern")) {
		strcpy(eargs->pattern,value);
#ifdef  VOIP_DORA		
		if (strcmp(eargs->pattern,"QQF")&&strcmp(eargs->pattern,"FQF")&&strcmp(eargs->pattern,"QFF")&&strcmp(eargs->pattern,"FFF")&&strcmp(eargs->pattern,"QHQF")&&strcmp(eargs->pattern,"FQFQ")&&strcmp(eargs->pattern,"FFQQ")){
#else	
        if (strcmp(eargs->pattern,"QQF")&&strcmp(eargs->pattern,"QHQF")&&strcmp(eargs->pattern,"FQFQ")&&strcmp(eargs->pattern,"FFQQ")){
#endif
		  fprintf(stderr,"Bad Pattern %s\n",value);
		  exit(-1);
		}
	      }
	      else if (!strcmp(name,"erasure_file")) {
		if ((eargs->erasure_file=fopen(value,"rb"))==NULL) {
		  fprintf(stderr,"Cannot Open Erasure File %s - ",value);
		  fprintf(stderr,"Running without erasures\n");
		}
	      }
	      else if (!strcmp(name,"signalling_file")) {
		if ((eargs->signalling_file=fopen(value,"rb"))==NULL) {
		  fprintf(stderr,"Cannot Open Signalling File %s - ",value);
		  fprintf(stderr,"Running without dim and burst\n");
		}
	      }

		else if (!strcmp(name,"Target_ADR_file")) {
		  if ((eargs->tar_adr=fopen(value,"rb"))==NULL) {
		    fprintf(stderr,"Cannot Open Target ADR File %s - ",value);
		    fprintf(stderr,"Running without dynamic ADR switching\n");
		  }
		  else
		    eargs->avg_rate_control=2; //this is done only for display purposes
		  // the above is set to 1 inside the main loop
		  
		}
		
		if (p1==NULL) break;
	      sx+=nc+1;
	      i++;
	    } while (strlen(sx)!=0);
	  }
	  break;
	case 'h':
	  usage_ed(stderr,argv[0]);
	  exit(-1);
	  break;
	case 'i':
	  eargs->input_filename = argv[optind-1];
	  break;
	case 'o':
	  eargs->output_filename = argv[optind-1];
	  break;
	case 'e':
	  eargs->encode_only = 1;
	  if (eargs->decode_only) {
	    fprintf(stderr,"%s:  ERROR - Encode with decode.\n",argv[0]);
	    exit(-1);
	  }
	  break;
	case 'd':
	  eargs->decode_only = 1;
	  if (eargs->encode_only) {
	    fprintf(stderr,"%s:  ERROR - Decode with encode.\n",argv[0]);
	    exit(-1);
	  }
	  break;
	case 'M':
	  eargs->max_rate = (int16)atoi(argv[optind-1]);
	  if (eargs->max_rate > 8) eargs->max_rate = 8;
	  if (eargs->max_rate < 1) eargs->max_rate = 1;
	  eargs->max_rate_default = 0;
	  break;
	case 'm':
	  eargs->min_rate = (int16)atoi(argv[optind-1]);
	  if (eargs->min_rate > 8) eargs->min_rate = 8;
	  if (eargs->min_rate < 1) eargs->min_rate = 1;
	  eargs->min_rate_default = 0;
	  break;
	case 'p':
	  eargs->post_filter = (int16)atoi(argv[optind-1]);
	  if (eargs->post_filter != 0) eargs->post_filter = 1;
	  eargs->post_filter_default = 0;
	  break;
	case 'n':
	  eargs->noise_suppression = (int16)atoi(argv[optind-1]);
	  if (eargs->noise_suppression != 0) eargs->noise_suppression = 1;
	  eargs->noise_suppression_default = 0;
	  break;
	case 'u':
	  eargs->unquantized_lsp=1;
	  break;
	case 'f':
	  eargs->partial_file_processing=1;
	  eargs->num_frames=(int16)atoi(argv[optind-1]);
	  break;
	case 's':
	  if ((eargs->signalling_file=fopen(argv[optind-1],"rb"))==NULL) {
	    fprintf(stderr,"Cannot Open Signalling File %s - ",argv[optind-1]);
	    fprintf(stderr,"Running without dim and burst\n");
	  }
	  break;
	  //case 's':
	  //eargs->starting_frame_num=(int16)atoi(argv[optind-1]);
	  //break;
	case 'r':
	  eargs->form_res_out=1;
	  break;
	case 'a':
	  eargs->qform_res_out=1;
	  break;
	case 'R':
	  eargs->mform_res_out=1;
	  break;
	case 'E':
	  eargs->target_speech_out=1;
	  break;
	case 'G':
	  eargs->rate_out=1;
	  break;
	case 'O':
	  eargs->lag_out=1;
	  break;
	case 'P':
	  eargs->unquantized_prototype=1;
	  break;
	case 'C':
	  switch(atoi(argv[optind-1])) {
	  case 1: 
	    eargs->unquantized_zero_rate=1;
	    break;
	  case 2: 
	    eargs->unquantized_quarter_rate=1;
	    break;
	  case 3: 
	    eargs->unquantized_half_rate=1;
	    break;
	  case 4: 
	    eargs->unquantized_full_rate=1;
	    break;
	  default:
	    break;
	  }
	  break;
	case 'I':
	  eargs->rate_filename=argv[optind-1];
	  if ((eargs->rfileP=fopen(eargs->rate_filename,"rb")) == NULL) {
	    fprintf(stderr,"%s:  ERROR - Unable to open rate file \"%s\".\n", argv[0],	eargs->rate_filename);
	    exit(-1);
	  }
	  break;
	case 'k':
	  eargs->packet_out=1;
	  break;
	case 'N':
	  eargs->nacf_out=1;
	  break;
	case 'v':
	  eargs->vad_out=1;
	  break;
	case 'A':
	  sprintf(frame_rate,"%s",argv[optind-1]);
	  num=strtok(frame_rate," FfRr");
	  while (num !=NULL && eargs->forced_count<10) {
	    eargs->forced_frame[eargs->forced_count]=atoi(num);
	    num=strtok(NULL,"Ff");
	    if (num !=NULL) eargs->forced_rate[eargs->forced_count++]=atoi(num);
	    num=strtok(NULL,"Rr");
	  }
	  break;
	  /*
	    case 's':
	    eargs->accshift_out=1;
	    break;
	  */
	case 'H':
	  eargs->highpass_filter=1;
	  break;
	case 'F':
	  eargs->avg_rate_control=1;
	  break;
	case 'T':
	  eargs->PPP_to_CELP_threshold=(float)atof(argv[optind-1]);
	  break;
	case 'W':
	  eargs->avg_rate_target=(int16) atof(argv[optind-1]);
	  eargs->avg_rate_control=1;
	  break;
	case 'w':
	  eargs->ratewin=(int16)atoi(argv[optind-1]);
	  break;
	case 'c':
	  eargs->olr_calibration=1;
	  break;
	case 'D':
	  eargs->dtx=(int16)atoi(argv[optind-1]);
	  break;
	}
      }
      if (eargs->min_rate > eargs->max_rate) {
	eargs->min_rate = eargs->max_rate;
      }
      return(eargs);
    }
  }
  else return(eargs);
}

/*======================================================================*/
/*         ..Returns number of frames in a binary file.                 */
/*----------------------------------------------------------------------*/
int32  GetNumFrames (FILE*  fp,int16  blocksize)
{
  /*....(local) variables....*/
  int32  position;
  int32  numFrames;

  /*....execute....*/
  if (Eargs->starting_frame_num>0) {
    fseek(fp,blocksize*Eargs->starting_frame_num,SEEK_SET);
    if (Eargs->rfileP)
      fseek(Eargs->rfileP,sizeof(int16)*Eargs->starting_frame_num,SEEK_SET);
  }
  position = ftell(fp);
  fseek(fp,0L,2);
  numFrames = (ftell(fp)-position) / blocksize;
  fseek(fp,position,0);
  return(numFrames);
}

/*======================================================================*/
/*         ..Main.                                                      */
/*----------------------------------------------------------------------*/

int  main (int argc,char* argv[])
{
  /*....(local) variables....*/
   

  FILE*      ifileP;
  FILE*      ofileP;

#ifdef VOIP_DORA
  int16      buf16[SPEECH_BUFFER_LEN*2];
  Word16 buf_fx[SPEECH_BUFFER_LEN*2+LOOKAHEAD_LEN];
 
#else
  int16      buf16[SPEECH_BUFFER_LEN];
  Word16 buf_fx[SPEECH_BUFFER_LEN+LOOKAHEAD_LEN];
#endif
  int16*     buf16P;
  Word16     *bufP_fx;

  int32      buf_count;
  int16      ibuf_len;
  int16      obuf_len;
  int32      j;
  int16      k;
  int16      rate;

  float      rate_sum;
  float      avg_rate;

  int32      rate_cnt[4+12+1];
  int32   rate_cnt_dec[5+11+1];
  float      chan_rate, voc_rate;

  int16  evrc_rate;

  int curr_va=0;

  int prev_voiced=0, prev_mode=1;
  
  Word16 pitch2_fx[2];
  Word16 nacf_at_pitch_fx[5]={0, 0, 0, 0, 0};
  
  MODE_FX m_fx;
  Word16 curr_ns_snr_fx[2];
  Word16 next_ns_snr_fx[2][2];
  Word16 curr_snr_fx[2];

  Word32 R_fx[17];
  Word16 r_scale;
  ENCODER_MEM_fx  rate_mem_fx;

#ifdef WMOPS_FX
  Word40 frame_wmops_fx;
#endif

  Word32 Ltemp;
  
  void update_average_rate(int rate);
/*-------------------------------------------------------------------*/
/*                     Data tranmission variables                    */
/*-------------------------------------------------------------------*/

 Word16 last_header=-1;
 DTMF_DETECTOR   Dtmf[1];
 DTMF_DETECTOR*  DtmfP;
 Word16 dtmf_buffer[shl(FSIZE,1)];

  parse_info=0;  

#ifdef VOIP_DORA
  //Variables used for passing information about Phase Matching/Warping
  
  short run_length = 0;
  short phase_offset = 10;
  short time_warp_fraction =  16384;   /*  in Q14 */

  int16	erasure_count = 0;
  int32	store_ptr;
  int16	CHECK_POINT = 13;
 
 
  //End: Variables used for passing information about Phase Matching/Warping
#endif
 
  // Initialization
#ifdef WMOPS_FX
  Nmove16(2);
  Nmove16(5);
#endif
  for (k=0;k<2;k++) curr_ns_snr_fx[k]=0;
  for (k=0;k<5;k++) nacf_at_pitch_fx[k]=0;

  for(k=0;k<2;k++) {
    next_ns_snr_fx[k][0]=0;
    next_ns_snr_fx[k][1]=0;
  }
  
  for(k=0;k<2;k++) curr_snr_fx[k]=0;
  
  /*....execute....*/
  /*...get arguments and check usage...*/
  if ((Eargs=get_eargs(argc,argv)) == NULL) {
    if (strstr(argv[0],"_enc")!=NULL) usage_e(stderr,argv[0]);
    else if (strstr(argv[0],"_dec")!=NULL) usage_d(stderr,argv[0]);
    else usage_ed(stderr,argv[0]);
    exit(-1);
  }
  if (strstr(argv[0],"_enc")!=NULL) print_eargs_e(stderr,Eargs);
  else if (strstr(argv[0],"_dec")!=NULL) print_eargs_d(stderr,Eargs);
  else print_eargs_ed(stderr,Eargs);

  /*...open files...*/
  if ((ifileP=fopen(Eargs->input_filename,"rb")) == NULL) {
    fprintf(stderr,"%s:  ERROR - Unable to open input file \"%s\".\n", argv[0],	Eargs->input_filename);
    exit(-1);
  }
    
  if ((ofileP=fopen(Eargs->output_filename,"wb")) == NULL) {
    fprintf(stderr,"%s:  ERROR - Unable to open output file \"%s\".\n",	argv[0], Eargs->output_filename);
    exit(-1);
  }

#ifdef VOIP_PARSE
   if ((parfileP=fopen(Eargs->dec_parfile,"r")) == NULL) {
       fprintf(stderr,"%s:  ERROR - Unable to open DecoderParametr file \"%s\".\n", argv[0],	Eargs->dec_parfile);
	   printf("\n Missing Decoder Parameter File");
       exit(-1);
       }
#endif




  /*...loop counter max...*/
  if (Eargs->decode_only) {
    buf_count = GetNumFrames(ifileP,sizeof(int16)*BITSTREAM_BUFFER_LEN);
  }
  else {
    buf_count = GetNumFrames(ifileP,sizeof(int16)*SPEECH_BUFFER_LEN);
  }
  if (!Eargs->partial_file_processing)
    Eargs->num_frames = buf_count;

  /*...processing loop...*/
  encode_fcnt = 0; //moved outside of InitEncoder_fx 
  decode_fcnt=0;
  Init_globs_fx();
  
  InitEncoder_fx();
  InitDecoder_fx();
 

 /*-------------------------------------------------------------------*/
 /*                  Data transmission Initialization                 */
 /*-------------------------------------------------------------------*/

  init_tty_enc( &tty_enc_char, &tty_enc_header, &tty_enc_baud_rate);
  
  init_tty_dec();

  DtmfP = &Dtmf[0];
  DtmfInit(DtmfP);
  dtmf_digit_last = -1;
  for(j=0;j<shl(FSIZE,1);j++)
    dtmf_buffer[j]=0;
  
#ifdef WMOPS_FX
  WMP_init_lib_fx ();
#endif

#ifdef WMOPS_FX
  Nmove16(2);
#endif
  rate_sum = 0.0;
  avg_rate = 0.0;

  ibuf_len = Eargs->ibuf_len;
  obuf_len = Eargs->obuf_len;

#ifdef WMOPS_FX
  Nmove16(4+12+1);
  Nmove16(5+11+1);
  Nmove16(3);
#endif
  for (j=0;j<4+12+1;j++) rate_cnt[j]=0;
  for (j=0;j<5+11+1;j++) rate_cnt_dec[j]=0;
  voc_rate=0;
  chan_rate=0;
  j = 1;

  for (k=0;k<FSIZE;k++) buf_fx[k]=0;  // maintain an integer copy of input
  if (!Eargs->decode_only) {
    fread(buf16,sizeof(int16),40,ifileP); /*discard 5 ms to align with EVRC ,was 40 */
    
    while (((fread(buf16,sizeof(int16),ibuf_len,ifileP)) == (unsigned short) ibuf_len) &&
	   ((Eargs->partial_file_processing == 0) ||
	    (Eargs->partial_file_processing && (Eargs->num_frames >= j)))) {
      for (buf16P=buf16,bufP_fx=buf_fx+LOOKAHEAD_LEN,k=0; k<ibuf_len; k++,buf16P++,bufP_fx++) {
	*bufP_fx = *buf16P;  // copy input speech to buf_fx
	
      }
# if 1
      if (Eargs->tar_adr!=NULL) {
	short curr_frame_adr;
	fread(&curr_frame_adr,sizeof(short),1,Eargs->tar_adr);
	Eargs->avg_rate_control=1;
	Eargs->avg_rate_target=curr_frame_adr;
#if 1
	if (curr_frame_adr==4800) //half-rate max
	  {
	    Eargs->operating_point=0;
	    Eargs->max_rate=3;
	  }
	else
	  Eargs->max_rate=4;
	//printf("%d\n",curr_frame_adr);
#endif
	//printf("%d %d\n",Eargs->avg_rate_target,Eargs->max_rate);
      }

      
#endif    
      //setup Anchor operating points

	if (Eargs->avg_rate_control) {
		if (Eargs->avg_rate_target>7500) Eargs->operating_point=0;
		else if (Eargs->avg_rate_target>6600) Eargs->operating_point=1;
       	else if (Eargs->avg_rate_target>5700) Eargs->operating_point=2;
     
       }
      if (Eargs->avg_rate_control) {
      	extern Word16 pattern_m;
      	if (Eargs->avg_rate_target==10000) pattern_m=1000;
      }

      if (Eargs->verbose) fprintf(stderr,"Encoding %ld of %ld\r",j,buf_count);

      /* Initialize data_buf */
      for(k=0;k<DATA_SIZE;k++)
	data_buf[k]=0;
      /*---------------------------------------------------------------*/
      /*                          DTMF Detector                        */
      /*---------------------------------------------------------------*/
		 
	     for (k = 0; k < FSIZE; k++)
	       {
		 dtmf_buffer[k] = dtmf_buffer[k+FSIZE];
		 dtmf_buffer[FSIZE+k] = buf_fx[k+LOOKAHEAD_LEN];
	       }
  
	     
	     dtmf_enc_flag = Dtmf20ms(DtmfP, dtmf_buffer, &dtmf_digit);

#ifndef ENABLE_IN_BAND_SIGNALLING_FOR_DTMF
	     dtmf_digit=-1;dtmf_digit_last=-1;dtmf_enc_flag=0;

#endif	     
	     

	     if ((dtmf_digit_last >= 0) || (dtmf_digit >= 0)) 
	       {
		 data_buf[0] = DTMF_HEADER;
		 data_buf[1] = dtmf_digit;
	       }

      
      /*=====================================================================*/
      /*        ..Hi-pass filter with 80 Hz Cheby II.                        */
      /*---------------------------------------------------------------------*/
#ifdef WMOPS_FX
      WMP_reset_counter_WMOPS_fx();
#endif
#ifdef WMOPS_FX
      WMP_fwc_fx("");
#endif
     
	if (Eargs->highpass_filter) {
	  hpf80_fx(buf_fx+LOOKAHEAD_LEN, buf_fx+LOOKAHEAD_LEN, SPEECH_BUFFER_LEN);
	}
	
#ifdef WMOPS_FX
      WMP_fwc_fx("High Pass Filter .........................................");
#endif
      
    tty_enc_flag = tty_enc( &tty_enc_char, &tty_enc_header, &tty_enc_baud_rate, buf_fx+LOOKAHEAD_LEN,FrameSize);

    
	     if (tty_enc_flag > 0) 
	       {
		 data_buf[0] = TTY_HEADER;
		 data_buf[1] = tty_enc_header+(TTY_TYPE_Baudot<<8);
		 data_buf[2] = tty_enc_char;
		 data_buf[3] = tty_enc_baud_rate;
		 last_header = tty_enc_header;
	       }
      /*=====================================================================*/
      /*        ..Noise suppression on 10ms buffer.                          */
      /*---------------------------------------------------------------------*/
	        
      if (Eargs->noise_suppression) {
	
	noise_suprs_fx(buf_fx+LOOKAHEAD_LEN, next_ns_snr_fx[0]);
	noise_suprs_fx(buf_fx+LOOKAHEAD_LEN+ibuf_len/2, next_ns_snr_fx[1]);
	
	for (k=LOOKAHEAD_LEN;k<LOOKAHEAD_LEN+SPEECH_BUFFER_LEN;k++) buf_fx[k]=shl(buf_fx[k],1); // because ns scales output down by 2
      }
      
    
      //Do NS even for TTY   
#ifdef WMOPS_FX
      WMP_fwc_fx("Noise Suppression ........................................");
#endif
      
#ifdef WMOPS_FX      
      WMP_fwc_fx("");
#endif

      if ((tty_enc_flag == 0 ) && (dtmf_digit==-1) && (dtmf_digit_last==-1))    
   { 
      beta_fx=pre_encode_fx (buf_fx, R_fx, r_scale);
#ifdef WMOPS_FX      
      WMP_fwc_fx("pre_encode_fx ............................................");
#endif

      /* Use either vad decision from nsvad() or EVRC rate selection: */
#ifdef WMOPS_FX      
      WMP_fwc_fx("");
#endif
      evrc_rate=voice_activity_detection_fx(R_fx, &rate_mem_fx, r_scale,
				  beta_fx, Eargs->operating_point);
#ifdef WMOPS_FX      
      WMP_fwc_fx("VAD_fx ...................................................");
#endif
      
      //      pitch2[0]=(int)delay1; pitch2[1]=(int)delay;
#ifdef WMOPS_FX
  Nmove16(2);
#endif
      pitch2_fx[0]=delay1_fx;
      pitch2_fx[1]=delay_fx;

#ifdef WMOPS_FX      
      WMP_fwc_fx("");
#endif
     
     get_nacf_at_pitch_fx(residual_fx+GUARD,pitch2_fx,nacf_at_pitch_fx+2);
         	   
#ifdef WMOPS_FX      
      WMP_fwc_fx("NACF_fx ..................................................");
#endif
      // mode decision      
      curr_va=(sub(evrc_rate,1)==0)?0:1;
      for (k=0;k<2;k++) {
	Ltemp=L_sub(log10_lut(rate_mem_fx.band_power[k]),log10_lut(rate_mem_fx.band_noise_sm[k])); // Q23
	curr_snr_fx[k]=round32_16(L_shl(Ltemp,2)); // Q9
      }
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
      VAD_SNR_fx = curr_snr_fx[0];
#endif
#ifdef WMOPS_FX      
      WMP_fwc_fx("");
#endif
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
      m_fx=new_mode_decision_fx(curr_va, prev_voiced, prev_mode,
				nacf_at_pitch_fx,
				curr_snr_fx, buf_fx);
#else
      m_fx=new_mode_decision_fx(curr_va, prev_voiced, prev_mode,
				nacf_at_pitch_fx,curr_ns_snr_fx,
				curr_snr_fx, buf_fx);
#endif
     
#ifdef WMOPS_FX      
      WMP_fwc_fx("mode_decision_fx .........................................");
#endif

      if (sub(evrc_rate,3)==0)
	if (sub(m_fx.speech_type,4)==0 || sub(m_fx.speech_type,3)==0) {
	  //	if (m.speech_type==4) {
	  m_fx.speech_type=5;
	}
      m_fx.mode=m_fx.speech_type;

     if (Eargs->operating_point==0)
      if (m_fx.mode==3) m_fx.mode=4;

      if (sub(m_fx.mode,3)==0) {
	Ltemp=L_sub(L_deposit_h(MAX_FX(delay_fx, pdelay_fx)),L_shl(L_mult(MIN_FX(delay_fx, pdelay_fx),0x4CCD),1)); // MAX(delay,pdelay)-1.2*MIN(delay,pdelay)
	if (Ltemp>0) m_fx.mode=4;
	//	if (MAX(delay,pdelay)/MIN(delay,pdelay) >1.2) m.mode=4;
	if (sub(abs_s(sub(delay_fx,pdelay_fx)),15)>0) m_fx.mode=4;// Messes up 'Q' otherwise in PPP
      }
      
      // Back up variables
      prev_mode=m_fx.mode;
      for (k=0;k<2;k++) {
	curr_ns_snr_fx[k]=shr(add(next_ns_snr_fx[0][k],next_ns_snr_fx[1][k]),1);
      }	
      prev_voiced=m_fx.curr_voiced;
      for (k=0;k<2;k++) nacf_at_pitch_fx[k]=nacf_at_pitch_fx[k+2];     
      
      rate=m_fx.mode;
      // End new stuff from G*


  
      if (sub(rate,3)==0) {
	PPP_MODE_E_fx=-1;
	rcelp_half_rateE=0;
      }
      if (sub(rate,5)==0) {
	rate=3;
	rcelp_half_rateE=1;
      }
      if (sub(rate,6)==0) rate=4;

      if (Eargs->operating_point==0) {
	rate=evrc_rate;
	rcelp_half_rateE=1; //No PPP for OP-0 at 1/2 rate (really at all rates)
        if (Eargs->avg_rate_control && rate!=3 && m_fx.mode==2) rate = 2; 
      }
    if (Eargs->avg_rate_control) {
      if (Eargs->operating_point == 0) {
	  //extern Word16 ave_rate_bps;
        extern Word16 patterncount;
        extern Word16 pattern_m;
        if (rate==2) {
                patterncount += pattern_m;
                if (patterncount >= 1000) {
                        patterncount -= 1000;
                        rate = 4;
                }
        }
      }
    }
      // Pattern QPPP_FCELP-FCELP for OP-1
      if (Eargs->operating_point==1) {
	if (sub(rate,3)!=0) pppcountE=0;
	else if (sub(rcelp_half_rateE,1)!=0) {
#ifdef USE_VADSNR_INSTEAD_OF_NSSNR
	  if (sub(curr_snr_fx[0],SNR_THLD)<0) {  // Q9
#else
	  if (sub(curr_ns_snr_fx[0],25*512)<0) {  // Q9	  
#endif
	    pppcountE=add(pppcountE,1);
	    if (sub(pppcountE,1)==0) rate=3;
	    else if (sub(pppcountE,2)==0) {if (sub(lastrateE,3)==0) rate=4; else rate=3;}
	    else {rate=4;pppcountE=0;}
	  }
	  else {
	    pppcountE=add(pppcountE,1);
	    if (sub(pppcountE,1)==0) rate=4;
	    else if (sub(pppcountE,2)==0) {if (sub(lastrateE,3)==0) rate=4; else rate=3;}
	    else {rate=4;pppcountE=0;}
	  }
	}
      }
#ifndef USE_VADSNR_INSTEAD_OF_NSSNR
      NS_SNR_fx=curr_ns_snr_fx[0];
#endif
      PPP_BUMPUP=0; // Reset this flag before every encode to avoid spurious
                    // percentages of rate=4 bumpups

      
      //tone detection to over-ride vad and mode decision
#if 0
      if (stoporder30<=4) rate=4; //reached 30 db pred gain within order 4
      if (rate==3 && rcelp_half_rateE==1 && stoporder15<=4) rate=4;
#else
      if ((stoporder30<=4) || (TONE_DETECTED==1)) rate=4; //reached 30 db pred gain within order 4
      if ((rate==3 && rcelp_half_rateE==1 && stoporder15<=4) || (TONE_DETECTED==1)) rate=4;
 
#endif      
      	  
	  // Signalling Dim and Burst
      if (Eargs->signalling_file!=NULL) {
	char dim;
	fread(&dim,sizeof(char),1,Eargs->signalling_file);
	if (dim==1) {
         
	  if (rate==4) rate=3;
	  dim_and_burstE=1;
	  if (Eargs->operating_point==1) pppcountE=0; //Reset the pattern FQF in OP1
	}
	else dim_and_burstE=0;
      }

      if (rate > Eargs->max_rate) rate = Eargs->max_rate;
      if (rate < Eargs->min_rate) rate = Eargs->min_rate;

      if (Eargs->max_rate ==3)  rcelp_half_rateE=1;

      SPL_HNELP=0;
      
      if ((m_fx.mode == 2) && (Eargs->operating_point==0) && (Eargs->max_rate<4))
	{
          rate=2;
	  SPL_HNELP=1;
	}
      

#ifdef WMOPS_FX      
      WMP_fwc_fx("");
#endif
  
#if 1
      if ((rate_mem_fx.last_rate ==3) && ((rate==4) || ((rate==3) && (rcelp_half_rateE ==0)))) 
	    rate_mem_fx.last_rate =4;
#endif      

#if 0
      // if (encode_fcnt<=200)
	printf("%d %d %d\n",rate,stoporder30,stoporder15);

#endif       
      if (!Eargs->olr_calibration) encode_fx(rate,buf16); //CALL THE ENCODER
#ifdef WMOPS_FX      
      WMP_fwc_fx("encoder_fx ...............................................");
#endif

      update_average_rate(data_packet.PACKET_RATE);

      switch (rate) {
      case 1:
	rate_sum += 1;
	rate_cnt[0]++;
	voc_rate+=0.8;
	chan_rate+=1.2;
	break;
      case 2:
	rate_sum +=2;
	rate_cnt[1]++;
	voc_rate+=2.0;
	chan_rate+=2.4;
	break;
      case 3:
	if (rcelp_half_rateE||dim_and_burstE) {
	  rate_sum+=4;
	  rate_cnt[6]++;rate_cnt[2]++;
	  voc_rate+=4.0;
	  chan_rate+=4.8;
	}
	else switch (PPP_MODE_E_fx) {
	case 0:
	  rate_sum+=2;
	  rate_cnt[9]++;
	  voc_rate+=2.0;
	  chan_rate+=2.4;
	  break;
	case 1:
	  rate_sum+=8;
	  if (PPP_BUMPUP) rate_cnt[16]++;
	  rate_cnt[15]++;
	  voc_rate+=8.5;
	  chan_rate+=9.6;
	  break;
	}
	break;
      default:
	if (PPP_BUMPUP==1) rate_cnt[8]++;
	rate_sum += 8;
	rate_cnt[3]++;
	voc_rate+=8.5;
	chan_rate+=9.6;
	break;
      }
      avg_rate = (rate_sum / (float)j) * 1.2;
      if (Eargs->olr_calibration) {
	j++;
#ifdef WMOPS_FX
  Nmove16(FSIZE);
#endif
	for (k=0;k<FSIZE;k++) buf_fx[k]=buf_fx[k+FSIZE];

#ifdef WMOPS_FX
  Nmove16(LPCORDER);
#endif
	for (k = 0; k < LPCORDER; k++) Oldlsp_nq_fx[k] = lsp_nq_fx[k];
	pdelay_fx=delay_fx;
	continue;
      }
         
      if (Eargs->encode_only) {
	
	fwrite(&data_packet.PACKET_RATE,sizeof(int16),1,ofileP); 
	fwrite(buf16,sizeof(int16),obuf_len,ofileP); //pack speech
      }
      else {
	// Erasure Insertion
	if (Eargs->erasure_file!=NULL) {
	  char Err_Mask;
	  fread(&Err_Mask,sizeof(char),1,Eargs->erasure_file);
	  if (Err_Mask==1) rate=data_packet.PACKET_RATE=0xE;
	}
#ifdef VOIP_DORA	
    //printf("\n calling FX Decoder ************* \n");
	decode_fx(buf16,data_packet.PACKET_RATE,Eargs->post_filter,buf_fx,run_length, phase_offset, time_warp_fraction, &obuf_len);
#else
    decode_fx(buf16,data_packet.PACKET_RATE,Eargs->post_filter,buf_fx);
    
#if 0
    int i;
    if (decode_fcnt==152)
      for(i=0;i<160;i++)
	printf("%d\n",buf_fx[i]);
#endif
    
#endif
	for (buf16P=buf16,bufP_fx=buf_fx,k=0; k<obuf_len; k++,buf16P++,bufP_fx++) {
	  *buf16P = (int16)*bufP_fx;
	}
	fwrite(buf16,sizeof(int16),obuf_len,ofileP);
	switch (rate) {
	case 1:
	  rate_cnt_dec[0]++;
	  break;
	case 2:
	  rate_cnt_dec[1]++;
	  break;
	case 3:
	  if (rcelp_half_rateD) {
	    rate_cnt_dec[6]++;rate_cnt_dec[2]++;
	  }
	  else switch (PPP_MODE_D_fx) {
	  case 0:
	    rate_cnt_dec[9]++;
	    break;
	  case 1:
	    rate_cnt_dec[15]++;
	    break;
	  }
	  break;
	case 4:
	  rate_cnt_dec[3]++;
	  break;
	default:  // Erasure
	  rate_cnt_dec[4]++;
	break;
	}

      }
      j++;
      fflush(stdout);fflush(stderr);fflush(ofileP);
   } //if tty_enc_flag ==0
 else //TTY/DTMF detected
   {
     InitEncoder_fx();
     PktPtr[0]=16;
     PktPtr[1]=0; 
     for (k=0; k<PACKWDSNUM; k++) {
#ifdef WMOPS_FX
  move16();
#endif
	  TxPkt[k]=0;
       }
       data_packet.PACKET_RATE=3;     
       fwrite(&data_packet.PACKET_RATE,sizeof(int16),1,ofileP); //half-rate
       TTY_DTMF_pack(data_buf,TxPkt,PktPtr);
       fwrite(TxPkt,sizeof(int16),obuf_len,ofileP);
       encode_fcnt++;j++;
   }
    
      for (k=0;k<LOOKAHEAD_LEN;k++) {
	buf_fx[k]=buf_fx[k+FSIZE];
      }
      dtmf_digit_last = -1;
      if (dtmf_digit > 0)
	dtmf_digit_last = dtmf_digit;
      
#ifdef WMOPS_FX
      frame_wmops_fx = WMP_calcul_stat_fx (j-1);
#endif
      
    }// close of partial processing
    j--;
#ifdef WMOPS_FX 
    WMP_print_stat_fx ();	 
#endif
    
    if (strstr(argv[0],"_enc")!=NULL) {
      if (Eargs->verbose) fprintf(stderr,"Done encoding\n");
    }
    else {
      if (Eargs->encode_only) fprintf(stderr,"Done encoding\n");
       else fprintf(stderr,"Done encoding and decoding\n");
      fprintf(stderr,"\n");
      fprintf(stderr,"Encoder:\n");
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 2,2.0 ,2.4, rate_cnt[1]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 9,2.0 ,2.4, rate_cnt[9]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 3,4.0 ,4.8, rate_cnt[2]/(float)(j-rate_cnt[0])); 
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 6,4.0 ,4.8, rate_cnt[6]/(float)(j-rate_cnt[0])); 
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 12,4.0 ,4.8, rate_cnt[12]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 15,8.5 ,9.6, rate_cnt[15]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 4,8.5 ,9.6, rate_cnt[3]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2s:\t %.3f\t\t%6.3f\t\t%5.3f\n", "PC",8.5 ,9.6, rate_cnt[8]/(float)(j-rate_cnt[0]));
      fprintf(stderr,"Mode %2s:\t %.3f\t\t%6.3f\t\t%5.3f\n", "PB",8.5 ,9.6, rate_cnt[16]/(float)(j-rate_cnt[0]));
      if (!Eargs->encode_only) {
	fprintf(stderr,"\n");
	fprintf(stderr,"Decoder:\n");
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 2,2.0 ,2.4, rate_cnt_dec[1]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 9,2.0 ,2.4, rate_cnt_dec[9]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 3,4.0 ,4.8, rate_cnt_dec[2]/(float)(j-rate_cnt_dec[0])); 
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 6,4.0 ,4.8, rate_cnt_dec[6]/(float)(j-rate_cnt_dec[0])); 
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 12,4.0 ,4.8, rate_cnt_dec[12]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 15,8.5 ,9.6, rate_cnt_dec[15]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 4,8.5 ,9.6, rate_cnt_dec[3]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"Erasure:\t\t\t\t\t%5.3f\n", rate_cnt_dec[4]/(float)(j-rate_cnt_dec[0]));
	fprintf(stderr,"\n");
      
      fprintf(stderr,"A Total of %ld frames, Voice Activity=%.3f%%\n",j, 100-rate_cnt[0]*100.0/j);

      fprintf(stderr,"Overall Average Channel Rate=%.3f\n",chan_rate/j);
      fprintf(stderr,"Average Vocoder Rate for Active Speech=%.3f\n",(voc_rate-rate_cnt[0]*0.8)/(j-rate_cnt[0]));
      fprintf(stderr,"Average Channel Rate for Active Speech=%.3f\n",(chan_rate-rate_cnt[0]*1.2)/(j-rate_cnt[0]));
      }
    }
  } // end of !decode only loop
  
  else {
    
    if (parse_info){  //Get the input from parse file
	int i, res=0;
#ifdef VOIP_PARSE	
	 do {
    
	fscanf(parfileP,"%d",&(decpar_struct.phase_offset));
	fscanf(parfileP, "%d",&(decpar_struct.run_length));
	fscanf(parfileP,"%f",&(decpar_struct.twf));
	fscanf(parfileP,"%d", &(decpar_struct.frame_rate));

    for (i=0;i<11;i++)
		res = fscanf(parfileP,"%d", &(decpar_struct.buf16[i]));
        time_warp_fraction = (int)(decpar_struct.twf*16384); //Q14 Format
	    rate = decpar_struct.frame_rate;
        phase_offset = decpar_struct.phase_offset;
	    run_length = decpar_struct.run_length;
        for (i=0;i<11;i++)
	     buf16[i]= decpar_struct.buf16[i];


#ifdef WMOPS_FX
	  WMP_reset_counter_WMOPS_fx();
	  WMP_fwc_fx("");
#endif

#ifdef VOIP_DORA	
   
	decode_fx(buf16,rate,Eargs->post_filter,buf_fx, run_length, phase_offset, time_warp_fraction, &obuf_len);
#else
    decode_fx(buf16,rate,Eargs->post_filter,buf_fx);
#endif

#ifdef WMOPS_FX
	  WMP_fwc_fx("Decode remaining processing ................................");
#endif

	for (buf16P=buf16,bufP_fx=buf_fx,k=0; k<obuf_len; k++,buf16P++,bufP_fx++) {
	  *buf16P = (int16)*bufP_fx;
	}
	if (Eargs->verbose) fprintf(stderr,"Decoding %ld of %ld\r",j,buf_count);
	fwrite(buf16,sizeof(int16),obuf_len,ofileP);
	j++;
	switch (rate) {
	case 1:
	  rate_cnt_dec[0]++;
	  break;
	case 2:
	  rate_cnt_dec[1]++;
	  break;
	case 3:
	  rate_cnt_dec[2]++;
	  break;
	case 4:
	  rate_cnt_dec[3]++;
	  break;
	default:  // Erasure
	  rate_cnt_dec[4]++;
	break;
	}
        fflush(stdout);fflush(stderr);fflush(ofileP);
#ifdef WMOPS_FX
	frame_wmops_fx = WMP_calcul_stat_fx (j-1);
#endif
   
      }while (res!=EOF);

#endif
    }  //end parsing case

    else {    // get info from Input packet files

    while ((fread(&rate,sizeof(int16),1,ifileP)) == 1) {
      if ((fread(buf16,sizeof(int16),ibuf_len,ifileP)) == (unsigned short) ibuf_len) {

#ifdef VOIP_DORA
   	if (Eargs->phase_matching == 0)
		phase_offset = 10;

	if (Eargs->double_erasures_pm == 0)
	{
		if (phase_offset == -1)
		{
			phase_offset = 10;
			time_warp_fraction = 0x4000; /* 1 in Q14*/
		}
	}
	if (Eargs->time_warping == 0)
		time_warp_fraction = 0x4000; /* 1 in Q14*/
#endif

#ifdef WMOPS_FX
	  WMP_reset_counter_WMOPS_fx();
	  WMP_fwc_fx("");
#endif

#ifdef VOIP_DORA	
   
	decode_fx(buf16,rate,Eargs->post_filter,buf_fx, run_length, phase_offset, time_warp_fraction, &obuf_len);
#else
    decode_fx(buf16,rate,Eargs->post_filter,buf_fx);
#endif

#ifdef WMOPS_FX
	  WMP_fwc_fx("Decode remaining processing ................................");
#endif

	for (buf16P=buf16,bufP_fx=buf_fx,k=0; k<obuf_len; k++,buf16P++,bufP_fx++) {
	  *buf16P = (int16)*bufP_fx;
	}
	if (Eargs->verbose) fprintf(stderr,"Decoding %ld of %ld\r",j,buf_count);
	fwrite(buf16,sizeof(int16),obuf_len,ofileP);
	j++;
	switch (rate) {
	case 1:
	  rate_cnt_dec[0]++;
	  break;
	case 2:
	  rate_cnt_dec[1]++;
	  break;
	case 3:
	  rate_cnt_dec[2]++;
	  break;
	case 4:
	  rate_cnt_dec[3]++;
	  break;
	default:  // Erasure
	  rate_cnt_dec[4]++;
	break;
	}
        fflush(stdout);fflush(stderr);fflush(ofileP);
#ifdef WMOPS_FX
	frame_wmops_fx = WMP_calcul_stat_fx (j-1);
#endif
   
	        }

		 }
  
	}


#ifdef WMOPS_FX 
    WMP_print_stat_fx ();	 
#endif
    
    if (Eargs->verbose) {
      fprintf(stderr,"Done decoding\n");
    }
    if (strstr(argv[0],"_dec")==NULL) {
      fprintf(stderr,"\n");
      fprintf(stderr,"Decoder:\n");
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 2,1.8 ,2.4, rate_cnt_dec[1]/(float)(j-rate_cnt_dec[0]));
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 3,3.9 ,4.8, rate_cnt_dec[2]/(float)(j-rate_cnt_dec[0])); 
      fprintf(stderr,"Mode %2d:\t %.3f\t\t%6.3f\t\t%5.3f\n", 4,8.5 ,9.6, rate_cnt_dec[3]/(float)(j-rate_cnt_dec[0]));
      fprintf(stderr,"Erasure:\t\t\t\t\t%5.3f\n", rate_cnt_dec[4]/(float)(j-rate_cnt_dec[0]));
      fprintf(stderr,"\n");
    }
  }
  fprintf(stderr,"\n");
  return(0);
  
}


