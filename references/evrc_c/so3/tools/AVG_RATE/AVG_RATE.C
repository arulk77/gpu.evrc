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
/*===========================================================================*/
/*         ..Includes.                                                       */
/*---------------------------------------------------------------------------*/
#include <stdio.h>


/*===========================================================================*/
/*         ..Defines.                                                        */
/*---------------------------------------------------------------------------*/
#define N_WORDS_PACKET	12


/*===========================================================================*/
/* round(): Generic round-off function                                       */
/*---------------------------------------------------------------------------*/
int round(float x)
{
	if (x < 0)
		return (int) (x - 0.5);

	else if (x > 0)
		return (int) (x + 0.5);
	else
		return 0;
}

  
/*===========================================================================*/
/* main(): Determines the average data rate of one or more EVRC packet files */
/*---------------------------------------------------------------------------*/
void main(int argc, char *argv[])
{

/*....(local) variables....*/
unsigned int packet_count = 0;
short packet[N_WORDS_PACKET];
short *pP;
short nfiles;
short j;
float Ravg;
float R;
float sum;
FILE* fp;

/*....execute....*/
if (argc < 2){
	fprintf(stderr,"Usage: %s filename_1 <filename_2> <filename_3>... <filename_n>\n",argv[0]);
	exit(0);
	}

nfiles = argc-1; 
pP = packet;
Ravg = 0; 

for (j=0; j<nfiles; j++){
	/*....open input file....*/
	if ((fp = fopen(argv[j+1], "rb")) == NULL){
		fprintf(stderr,"Cannot open file '%s'...\n",argv[j+1]);
		exit(0);
		}

	sum = 0;
	packet_count = 0;

	/*....read packet....*/
	while (fread(pP,sizeof(short),N_WORDS_PACKET,fp) == N_WORDS_PACKET){
		++packet_count;
		/*....*pP corresponds to the rate....*/
		switch (*pP){
			case 1:				/* Rate 1/8 */
				sum += 1200;
				break;
			case 3:				/* Rate 1/2 */
				sum += 4800;
				break;
			case 4:				/* Rate 1 */
				sum += 9600;
				break;
			default:	
				sum += 9600;	
				break;
			}

		}
		
		R = sum/packet_count;
		Ravg += R;
		fprintf(stdout,"%s:   %d packets   R = %d bps\n",argv[j+1],packet_count,round(R));
		fclose(fp);
	}

		Ravg = Ravg/nfiles;
		fprintf(stdout,"\nAverage rate for %d files = %d bps\n",nfiles,round(Ravg));
}
