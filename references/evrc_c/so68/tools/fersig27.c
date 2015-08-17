/*------------------------------------------------------------------------------------------------------------------------------------*
  Revision fersig v. 2.4
      Date:         sept 16, 2003:
      Author:       Philippe Gournay, Universite de Sherbrooke/VoiceAge corp.,
      Contribution: C11-20030915-006-Nokia-Fersig-Change-Request.doc
      Modification: For the Wideband codec (option -WBC), the program verifies whether it is running on a platform with Byte-swapping
                    of integer words. If this is the case, the packet payload (excluding the 1st word with rate information) is Byte
                    swapped before processing and is swapped again into original form after the processing. This was necessary for
                    correct processing for WBC which packs/unpacks the bitstream Byte per Byte.

  Revision fersig v. 2.5
      Date:         sept 17, 2003:
      Author:       Milan Jelinek, Universite de Sherbrooke/VoiceAge corp.,
      Contribution: C11-20030915-006R1-Nokia-Fersig-Change-Request.doc
      Modification: The platform test is now performed for unsigned short to be compatible with the "packet" array format.
                    If Byte swapping is done, also the first word (rate information) is byte swapped

  Revision fersig v. 2.6
      Date:         March 31, 2004:
      Author:       Milan Jelinek, Universite de Sherbrooke/VoiceAge corp.,
      Contribution: 
      Modification: Adding RS 1 extension of WBC (-WBC1 option)

  Revision fersig v. 2.7
      Date:         December 23, 2005:
      Author:       Ananth Kandhadai, Qualcomm Inc
      Contribution: 
      Modification: Adding support for EVRC-B
 *------------------------------------------------------------------------------------------------------------------------------------*/
               


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char prog_opts[]="c:e:s:b:r:h";
extern char *optarg;
extern int optind;
int main(int argc,char *argv[])
{
  int i,required_arg=0,optional_arg1=0,optional_arg2=0,optional_arg3=0;
  int optional_arg4=0;
  char coder[40];
  char infile[200],outfile[200],errfile[200],sigfile[200],logfile[200];
  FILE *fin,*fout,*ferr,*fsig,*f_log;
  static long seed1=1,seed2=1,seed3=1; /* Do not set to 0 ! */
  double badpct=0,onepct=0.01,threepct=0.03;
  unsigned short packet[18];
  unsigned char FEP=0,SIG=0;
  unsigned long FC=0,HC=0,QC=0,EC=0,BC=0,ERRC=0,DIMC=0,SIGC=0;
  unsigned long j=0,n;
  int FULL=171,HALF=80,QUART=40,EIGHTH=16,BLANK=0,ERASURE=0; // IS-96A,IS-127
  int FM=0xffe0,HM=0xffff,QM=0xff00,EM=0xffff; // Last word masks:IS-96A,IS-127
  int WORDS=12;
  int FW=11,HW=5,QW=3,EW=1; // Word to apply mask to for IS-96A,IS-127,SMV,EVRC-B
  long zdq,M0P;
  int fl,bad;
  int rateoffset;
  static long A=16807,M=2147483647,Q=127773,R=2836;
  static long M01=2126008810,M03=2083059137; /* 0.99*M and 0.97*M */
  static int rateset[4]={1,2,3,4};
  static int PW[4]={1,3,5,11},PM[4]={0xffff,0xff00,0xffff,0xffe0};     
  char allzerosflag=1, allonesflag=1;

  /* For WBC only */

  unsigned short endian;
  int swap_flag;
  unsigned char *c_ptr, tmp;

  while((i=getopt(argc,argv,prog_opts))!=EOF) switch (i) {
  case 'c':
    strcpy(coder,argv[optind-1]);
    required_arg=2;
    break;
  case 'e':
    if ((ferr=fopen(argv[optind-1],"rb"))==NULL) {
      fprintf(stderr,"Cannot open frame error pattern file %s\n",argv[optind-1]);
      exit(1);
    }
    optional_arg1=2;
    break;
  case 's':
    if ((fsig=fopen(argv[optind-1],"rb"))==NULL) {
      fprintf(stderr,"Cannot open signalling pattern file %s\n",argv[optind-1]);
      exit(1);
    }
    optional_arg2=2;
    break;
  case 'b':
    sscanf(argv[optind-1],"%lf",&badpct);
    if (badpct!=onepct && badpct!=threepct) {
      fprintf(stderr,"Bad Packet Percentage should be 0.01 or 0.03\n");
      exit(1);
    }
    optional_arg3=2;
    break;
  case 'r':
    sscanf(argv[optind-1],"%ld",&seed1);
    optind++;
    sscanf(argv[optind-1],"%ld",&seed2);
    optind++;
    sscanf(argv[optind-1],"%ld",&seed3);
    optional_arg4=3;
    break;
  case 'h':
    fprintf(stderr,"usage: %s -c coder(IS-96A,IS-733,IS-127,SMV,WBC,WBC1,EVRC-B) [-b 0.01/0.03] [-r badpacketseed randrateseed randbitsseed] [-e Frame_Error_Pattern_File] [-s Signalling_Pattern_File] [-h] input_packet_file output_packet_file\n",argv[0]);
    return(1);
  }
  
  if (required_arg==0 || argc<(required_arg+optional_arg1+optional_arg2+optional_arg3+optional_arg4)+3) {
    fprintf(stderr,"Too few arguments\n");
    fprintf(stderr,"usage: %s -c coder(IS-96A,IS-733,IS-127,SMV,WBC,WBC1,EVRC-B) [-b 0.01/0.03] [-r badpacketseed randrateseed randbitsseed] [-e Frame_Error_Pattern_File] [-s Signalling_Pattern_File] [-h] input_packet_file output_packet_file\n",argv[0]);
    exit(-1);
  }
  else {
    if (!strcmp(argv[argc-2],"-")) fin=stdin;
    else {
      if ((fin=fopen(argv[argc-2],"rb"))==NULL) {
        fprintf(stderr,"Cannot open input file %s\n",argv[argc-2]);
        exit(1);
      }
    }
    if (!strcmp(argv[argc-1],"-")) fout=stdout;
    else {
      if ((fout=fopen(argv[argc-1],"wb"))==NULL) {
        fprintf(stderr,"Cannot open output file %s\n",argv[argc-1]);
        exit(1);
      }
    }
  }
  
  if (!strcmp(coder,"SMV")) {
    FULL=170;
    FM=0xffc0;
    PM[3]=FM;
  }
  if ((!strcmp(coder,"IS-733")) || (!strcmp(coder,"WBC"))) {
    WORDS=18;FULL=266;HALF=124;QUART=54;EIGHTH=20;
    FM=0xffc0;HM=0xfff0;QM=0xfc00;EM=0xf000;
    FW=17;HW=8;QW=4;EW=2;
    PW[0]=EW;PW[1]=QW;PW[2]=HW;PW[3]=FW;
    PM[0]=EM;PM[1]=QM;PM[2]=HM;PM[3]=FM;
  }
  
  /* Determine endianess */

  swap_flag = 0;
  if ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) )
  {
    endian = 1;
    if(*(char *)&endian == 1) swap_flag = 1;
  }
  
  while (!feof(fin)) {
    if ((n=fread(packet,sizeof(unsigned short),WORDS,fin))<WORDS) {
      if (n) fprintf(stderr,"Incomplete packet read for frame number %d\n",j);
      break;
    }

    if (swap_flag) /* swap bytes for WBC on little endian platforms */
    {

      c_ptr = (unsigned char *)&packet[0];

      for (i=0;i<WORDS;i++)
      {
        tmp        = *c_ptr;
        *c_ptr     = *(c_ptr+1);
        *(c_ptr+1) = tmp;
        c_ptr     += 2;
      }

    }

    //Check for all zeros packet, and all ones eighth-rate packet
    allzerosflag=1;
    allonesflag=1;
    switch (packet[0]) {
    case 4:
      for (i=1;i<FW;i++) if (packet[i]!=0x0000) allzerosflag=0; 
      if ((packet[FW]&FM)!=0x0000) allzerosflag=0; 
      if (allzerosflag==1 && ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) ) ) {
        packet[0]=0xE;
        fprintf(stderr,"\n WARNING: Frame %ld - Found All Zeros Full Rate Packet \n",j);
      }
      break;
    case 3:
      for (i=1;i<HW;i++) if (packet[i]!=0x0000) allzerosflag=0; 
      if ((packet[HW]&HM)!=0x0000) allzerosflag=0; 
      if (allzerosflag==1 && ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) ) ) {
        packet[0]=0xE;
        fprintf(stderr,"\n WARNING: Frame %ld - Found All Zeros Half Rate Packet \n",j);
      }
      break;
    case 2:
      for (i=1;i<QW;i++) if (packet[i]!=0x0000) allzerosflag=0; 
      if ((packet[QW]&QM)!=0x0000) allzerosflag=0; 
      if (allzerosflag==1 && ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) ) ) {
        packet[0]=0xE;
        fprintf(stderr,"\n WARNING: Frame %ld - Found All Zeros Quarter Rate Packet \n",j);
      }
      break;
    case 1:
      for (i=1;i<EW;i++) if (packet[i]!=0x0000) allzerosflag=0; 
      if ((packet[EW]&EM)!=0x0000) allzerosflag=0; 
      if (allzerosflag==1 && ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) ) ) {
        packet[0]=0xE;
        fprintf(stderr,"\n WARNING: Frame %ld - Found All Zeros Eighth Rate Packet \n",j);
      }
      for (i=1;i<EW;i++) if (packet[i]!=0xFFFF) allonesflag=0; 
      if ((packet[EW]&EM)!=EM) allonesflag=0; 
      if (allonesflag==1 && ( (!strcmp(coder,"WBC")) || (!strcmp(coder,"WBC1")) ) ) {
        packet[0]=0xE;
        fprintf(stderr,"\n WARNING: Frame %ld - Found All Ones Eighth Rate Packet \n",j);
      }
      break;
    }

    switch (packet[0]) {
    case 4:
      packet[FW]&=FM;
      FC++;
      break;
    case 3:
      packet[HW]&=HM; for (i=HW+1;i<WORDS;i++) packet[i]=0;
      HC++;
      break;
    case 2:
      packet[QW]&=QM; for (i=QW+1;i<WORDS;i++) packet[i]=0;
      QC++;
      break;
    case 1:
      packet[EW]&=EM; for (i=EW+1;i<WORDS;i++) packet[i]=0;
      EC++;
      break;
    case 0:
      for (i=1;i<WORDS;i++) packet[i]=0;
      BC++;
      break;
    case 0xE:
      for (i=1;i<WORDS;i++) packet[i]=0;
      ERRC++;
      break;
    default:
      fprintf(stderr,"Bad Rate %d detected \n",packet[0]);
      exit(-1);
    } 

    if (optional_arg1==2) {
      if (fread(&FEP,sizeof(unsigned char),1,ferr)<1) {
        fprintf(stderr,"Frame error pattern string shorter than input\n");
        switch (packet[0]) {// Not going to include this packet for rate/output
        case 4:FC--;break;case 3:HC--;break;case 2:QC--;break;
        case 1:EC--;break;case 0:BC--;break;case 0xE:ERRC--;break;
        }
        break;
      }
      
      if (FEP==1) {
        //      switch (packet[0]) {// Decrement the count of the current rate
        //      case 4:FC--;break;case 3:HC--;break;case 2:QC--;break;
        //      case 1:EC--;break;case 0:BC--;break;case 0xE:ERRC--;break;
        //      }
        ERRC++;            // Increment the count of erasures
        for (packet[0]=0xE,i=1;i<WORDS;i++) packet[i]=0;
      }
    }
    if (optional_arg2==2) {
      if (fread(&SIG,sizeof(unsigned char),1,fsig)<1) {
        fprintf(stderr,"Signalling pattern string shorter than input\n");
        switch (packet[0]) {// Not going to include this packet for rate/output
        case 4:FC--;break;case 3:HC--;break;case 2:QC--;break;
        case 1:EC--;break;case 0:BC--;break;case 0xE:ERRC--;break;
        }
        break;
      }
      
      if (SIG==1) {
        if (packet[0]==4) {// Only affects full rate packets, others are <=half
          fprintf(stderr,"\n WARNING: Frame %ld - Found full-rate in Signalling Frame\n",j);
          //      FC--;HC++; // Decrement full count and increment half count
          for (packet[0]=3,i=HW+1;i<WORDS;i++) packet[i]=0;
          DIMC++;          // Increment count of dimmed frames
          packet[HW]&=HM;
        }
        SIGC++;            // Increment signalling count
      }
    }
    if (optional_arg3==2) {
      rateoffset=packet[0]-1;
      if (badpct==0.01) M0P=M01; if (badpct==0.03) M0P=M03;
      if (seed1<M0P/*prev_frame_not_bad_packet*/) fl=1; else fl=0;
      zdq=seed1/Q;seed1=A*(seed1-zdq*Q)-zdq*R;seed1+=(seed1<0?M:0);
      if (seed1>=M0P && fl==1 && FEP==0) bad=1; else bad=0;
      if (bad) {
        zdq=seed2/Q;seed2=A*(seed2-zdq*Q)-zdq*R;seed2+=(seed2<0?M:0);
        if (seed2<715827882) packet[0]=rateset[(rateoffset+1)%4];
        else if (seed2<1431655764) packet[0]=rateset[(rateoffset+2)%4];
        else packet[0]=rateset[(rateoffset+3)%4];
        for (i=1;i<=PW[packet[0]-1];i++) {
          zdq=seed3/Q;seed3=A*(seed3-zdq*Q)-zdq*R;seed3+=(seed3<0?M:0);
     /* "seed3" is uniformly distributed between 1 and M-1 (=0x7FFFFFFE) */
     /* -> keep the 16 most significant "active" bits of seed3           */
          packet[i]=(seed3>>(8*(sizeof(long)/sizeof(short))-1))&0xffff;
        }
        packet[PW[packet[0]-1]]&=PM[packet[0]-1];
        for (i=PW[packet[0]-1]+1;i<WORDS;i++) packet[i]=0;
      }
    }

    if (swap_flag) /* swap bytes for WBC on little endian platforms */
    {

      c_ptr = (unsigned char *)&packet[0];

      for (i=0;i<WORDS;i++)
      {
        tmp        = *c_ptr;
        *c_ptr     = *(c_ptr+1);
        *(c_ptr+1) = tmp;
        c_ptr     += 2;
      }

    }

    fwrite(packet,sizeof(unsigned short),WORDS,fout);
    j++;
  }

  if (fout==stdout) {
    if ((f_log=fopen("rda.log","w"))==NULL) {
      fprintf(stderr,"Cannot open rda.log for writing rate statistics\n");
      exit(-1);
    }
  }
  else {
    sprintf(logfile,"%s.rda.log",argv[argc-1]);
    if ((f_log=fopen(logfile,"w"))==NULL) {
     fprintf(stderr,"Cannot open %s for writing rate statistics\n",logfile);
     exit(-1);
    }
  }

  fprintf(f_log,"%s Version 2.4\n",argv[0]);
  fprintf(f_log,"Rate, Erasure and Signalling Statistics\n\n");
  fprintf(f_log,"Full    Rates:\t%6.2f%%\n",FC*100.0/j);
  fprintf(f_log,"Half    Rates:\t%6.2f%%\n",HC*100.0/j);
  fprintf(f_log,"Quarter Rates:\t%6.2f%%\n",QC*100.0/j);
  fprintf(f_log,"Eighth  Rates:\t%6.2f%%\n",EC*100.0/j);
  fprintf(f_log,"\n");
  fprintf(f_log,"Blank      Frames:\t%6.2f%%\n",BC*100.0/j);
  fprintf(f_log,"Dimmed     Frames:\t%6.2f%%\n",DIMC*100.0/j);
  fprintf(f_log,"Signalling Frames:\t%6.2f%%\n",SIGC*100.0/j);
  fprintf(f_log,"Erased     Frames:\t%6.2f%%\n",ERRC*100.0/j);
  fprintf(f_log,"\n");
  if ((!strcmp(coder,"IS-733")) || (!strcmp(coder,"WBC")))
    fprintf(f_log,"Average Channel Rate (kbps):\t\t\t%6.4f\n",(FC*14.4+HC*7.2+QC*3.6+EC*1.8)/j);
  else
    fprintf(f_log,"Average Channel Rate (kbps):\t\t\t%6.4f\n",(FC*9.6+HC*4.8+QC*2.4+EC*1.2)/j);
  if ((!strcmp(coder,"IS-733")) || (!strcmp(coder,"WBC")))
    fprintf(f_log,"Average Active Channel Rate (kbps):\t\t%6.4f\n",(FC*14.4+HC*7.2+QC*3.6)/(j-EC-BC-ERRC));
  else
    fprintf(f_log,"Average Active Channel Rate (kbps):\t\t%6.4f\n",(FC*9.6+HC*4.8+QC*2.4)/(j-EC-BC-ERRC));
  if ((!strcmp(coder,"IS-733")) || (!strcmp(coder,"WBC")))
    fprintf(f_log,"Average Source Rate (kbps):\t\t\t%6.4f\n",(FC*13.3+HC*6.2+QC*2.7+EC*1.0)/j);
  else
    fprintf(f_log,"Average Source Rate (kbps):\t\t\t%6.4f\n",(FC*8.55+HC*4.0+QC*2.0+EC*0.8)/j);
  if ((!strcmp(coder,"IS-733")) || (!strcmp(coder,"WBC")))
    fprintf(f_log,"Average Active Source Rate (kbps):\t\t%6.4f\n",(FC*13.3+HC*6.2+QC*2.7)/(j-EC-BC-ERRC));
  else
    fprintf(f_log,"Average Active Source Rate (kbps):\t\t%6.4f\n",(FC*8.55+HC*4.0+QC*2.0)/(j-EC-BC-ERRC));
  fclose(f_log);
     
  fclose(fin);fclose(fout);
  if (optional_arg1==1) fclose(ferr); if (optional_arg2==1) fclose(fsig);

  
  return(0);
      
}
