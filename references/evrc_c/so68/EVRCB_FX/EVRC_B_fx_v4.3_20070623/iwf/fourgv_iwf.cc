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
#if 0
#include <getopt.h>
#endif
extern void Bitpack(short in, unsigned short *TrWords, short NoOfBits, short *ptr);
extern void BitUnpack(short *out, unsigned short *RecWords, short NoOfBits, short *ptr);


#define WORDS 12
char prog_opts[]="s:i:o:h";
extern char *optarg;
extern int optind;
int main(int argc,char *argv[])
{
  int i, n, j=0;
  char infile[200],outfile[200],sigfile[200];
  FILE *fin,*fout,*fsig;
  unsigned short packet[WORDS];
  int HW=5;
  unsigned char SIG=0;
  unsigned long FC=0,DIMC=0,SIGC=0;
  int required_arg=0;

  /* unpacking variables */
  short delay_idx,mode_bit;
  short lsp_idx[4],acbg_idx[3],dmp[2];
  short PktPtr[2]={16,0};
  short f_rot_idx, a_amp_idx[3],a_power_idx;
  
  
  fprintf(stderr,"|******************************************************************************|\n");
  fprintf(stderr,"| EVRC-B - Enhanced Variable Rate Coder - B Packet Level Signaling InterWorking Function |\n");
  fprintf(stderr,"|       Version 1.0                                                            |\n");
  fprintf(stderr,"|       Converts EVRC-B full-rate packets to half-rate packets                    |\n");
  fprintf(stderr,"|******************************************************************************|\n");

  while((i=getopt(argc,argv,prog_opts))!=EOF) switch (i) {
  case 's':
    if ((fsig=fopen(argv[optind-1],"rb"))==NULL) {
      fprintf(stderr,"Cannot open signalling pattern file %s\n",argv[optind-1]);
      exit(1);
    }
    required_arg++;
    break;
  case 'i':
    if ((fin=fopen(argv[optind-1],"rb"))==NULL) {
      fprintf(stderr,"Cannot open input file %s\n",argv[optind-1]);
      exit(1);
    }
    required_arg++;
    break;
  case 'o':
    if ((fout=fopen(argv[optind-1],"wb"))==NULL) {
      fprintf(stderr,"Cannot open output file %s\n",argv[optind-1]);
      exit(1);
    }
    required_arg++;
    break;
  case 'h':
    fprintf(stderr,"usage: %s -s Signalling_Pattern_File [-h] input_packet_file output_packet_file\n",argv[0]);
    return(1);
  }
  
  if (required_arg!=3) {
    fprintf(stderr,"Wrong number of arguments\n");
    fprintf(stderr,"usage: %s -s Signalling_Pattern_File [-h] input_packet_file output_packet_file\n",argv[0]);
    exit(-1);
  }
  
  while (!feof(fin)) {
    if ((n=fread(packet,sizeof(unsigned short),WORDS,fin))<WORDS) {
      if (n) fprintf(stderr,"Incomplete packet read for frame number %d\n",j);
      break;
    }
    if (packet[0]==4) FC++; //increment full-rate count

    if (1) {
     if (fread(&SIG,sizeof(unsigned char),1,fsig)<1) {
	fprintf(stderr,"Signalling pattern string shorter than input\n");
	break;
      }
      
      if (SIG==1) {
	if (packet[0]==4) {// Only affects full rate packets, others are <=half
	  PktPtr[0]=16;
	  PktPtr[1]=0;
           BitUnpack(&mode_bit,packet+1,1,PktPtr);
	   if (mode_bit == 0) //FCELP - HCELP
	     {
	       // unpacking F-CELP information
	       BitUnpack(&delay_idx ,packet+1,7,PktPtr); 
	       BitUnpack(lsp_idx ,packet+1,6,PktPtr);
	       BitUnpack(lsp_idx+1 ,packet+1,6,PktPtr);
	       BitUnpack(lsp_idx+2 ,packet+1,9,PktPtr);
	       BitUnpack(lsp_idx+3 ,packet+1,7,PktPtr);
	
	       BitUnpack(dmp ,packet+1,5,PktPtr);
	       for(i=0;i<3;i++) BitUnpack(acbg_idx+i ,packet+1,3,PktPtr);
	   
	       //Packing spl. 1/2 CELP packet
	       PktPtr[0]=16;PktPtr[1]=0;
	       for (i=0; i<WORDS; i++) {
		 packet[i]=0;
	       }
            
	       Bitpack(0x6E ,packet+1,7,PktPtr); //110 in dec
               Bitpack(0,packet+1,2,PktPtr); //SHCELP,SHPPP =0, SQNELP=1
	       Bitpack(mode_bit ,packet+1,1,PktPtr);
	   
	       Bitpack(lsp_idx[0] ,packet+1,6,PktPtr);
	       Bitpack(lsp_idx[1] ,packet+1,6,PktPtr);
	       Bitpack(lsp_idx[2] ,packet+1,9,PktPtr);
	       Bitpack(lsp_idx[3] ,packet+1,7,PktPtr);
	   
	       for(i=0;i<3;i++) Bitpack(acbg_idx[i] ,packet+1,3,PktPtr);
	       Bitpack(delay_idx ,packet+1,7,PktPtr);
	       
	     }
	   if (mode_bit==1) //FPPP - HPPP
	     {
	      // unpacking F-PPP information
	       BitUnpack(&delay_idx ,packet+1,7,PktPtr); 
	       BitUnpack(lsp_idx ,packet+1,6,PktPtr);
	       BitUnpack(lsp_idx+1 ,packet+1,6,PktPtr);
	       BitUnpack(lsp_idx+2 ,packet+1,9,PktPtr);
	       BitUnpack(lsp_idx+3 ,packet+1,7,PktPtr);
	
	       BitUnpack(&f_rot_idx,packet+1,7,PktPtr);
	       BitUnpack(a_amp_idx,packet+1,6,PktPtr);
	       BitUnpack(a_amp_idx+1,packet+1,6,PktPtr);
	       BitUnpack(a_amp_idx+2,packet+1,8,PktPtr);
	       BitUnpack(&a_power_idx,packet+1,8,PktPtr);

	       //Packing spl. 1/2 rate PPP packet
	       PktPtr[0]=16;PktPtr[1]=0;
	       for (i=0; i<WORDS-1; i++) {
		 packet[i]=0;
	       }
	       
	       Bitpack(0x6E ,packet+1,7,PktPtr); //110 in dec
               Bitpack(0,packet+1,2,PktPtr);//SHCELP,SHPPP =0, SQNELP=1
	       Bitpack(mode_bit ,packet+1,1,PktPtr);
	       Bitpack(lsp_idx[0] ,packet+1,6,PktPtr);

	       Bitpack(lsp_idx[1] ,packet+1,6,PktPtr);
	       Bitpack(lsp_idx[2] ,packet+1,9,PktPtr);
	       Bitpack(lsp_idx[3] ,packet+1,7,PktPtr);
	   
	       Bitpack(f_rot_idx,packet+1,7,PktPtr);
	       Bitpack(a_amp_idx[0],packet+1,6,PktPtr);
	       Bitpack(a_amp_idx[1],packet+1,6,PktPtr);
	       Bitpack(a_amp_idx[2],packet+1,8,PktPtr);
	       Bitpack(a_power_idx,packet+1,8,PktPtr);
      
	       Bitpack(delay_idx ,packet+1,7,PktPtr);
                      
	     }
	   for (packet[0]=3,i=HW+1;i<WORDS;i++) packet[i]=0;
	  DIMC++;          // Increment count of dimmed frames
	}
	SIGC++;            // Increment signalling count
      }
    }
    fwrite(packet,sizeof(unsigned short),WORDS,fout);
    j++;
    
  }


  fprintf(stdout,"Dimmed Frames: %d out of %d full-rate frames (%.2f%%)\n",DIMC,FC,DIMC*100.0/FC);
     
  fclose(fin);fclose(fout);

  
  return(0);
      
}
