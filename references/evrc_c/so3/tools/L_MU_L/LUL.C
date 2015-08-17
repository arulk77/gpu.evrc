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
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "l2m.h"
#include "m2l.h"

/*===========================================================================*/
/*         ..Compile flags.                                                  */
/*---------------------------------------------------------------------------*/
#define  MU_14         1        /* set to 1 for 14-bit left-shifted output   */

/*===========================================================================*/
/*         ..Defines.                                                        */
/*---------------------------------------------------------------------------*/
#define  FRAMESIZE     160
#define  MUSIZE      65536
#define  SCALE16_14 (8031.0/32257.0)
#define  SCALE14_16 (32257.0/8031.0)

#define  STRIP_N         0      /* number of samples to strip from beginning */
#define  APPEND_N        0      /* number of samples to add to end           */

/*===========================================================================*/
/*         ..Convert mu-law to 14-bit left-shifted linear.                   */
/*---------------------------------------------------------------------------*/
void  ulaw_expand (
	unsigned char*  u_buf,
	short*          lin_buf,
	int             items_read
)
{
	int    i;
	short  tmp;

	for (i=0; i<items_read; i++) {
#if MU_14
		lin_buf[i] = mulintbl[(int)u_buf[i]];   /* get CCITT value */
#else
		tmp = mulintbl[(int)u_buf[i]];          /* get CCITT value */
		lin_buf[i] = (short)(((float)(tmp>>2))*SCALE14_16); /* scale to 16 bits */
#endif
	}
}

/*===========================================================================*/
/*         ..Convert 16-bit linear to mu-law.                                */
/*---------------------------------------------------------------------------*/
void  ulaw_compress (
	short*          lin_buf,
	unsigned char*  u_buf,
	int             items_read
)
{
	int  i;

	for (i=0; i<items_read; i++) {
		u_buf[i] = lin_2_mu[(unsigned short)lin_buf[i]];
	}
}

/*===========================================================================*/
/*         ..Main.                                                           */
/*---------------------------------------------------------------------------*/
main (int argc, char *argv[])
{
	FILE*   inFp;
	FILE*   outFp;
	short*  lin_buf;
	short*  log_buf;
	unsigned char*  u_buf;
	long    items_read;
	int     i;
	int     j;
	short   zero_buf[APPEND_N];
	char    dummy[10];

	if (argc <3) {
#if MU_14
		fprintf(stderr,"16-bit linear to 8-bit mulaw, back to 14-bit left-shifted linear\n");
#else
		fprintf(stderr,"16-bit linear to 8-bit mulaw, back to 16-bit linear\n");
#endif
		fprintf(stderr,"usage:  %s  input_filename  output_filename\n",argv[0]);
		exit(1);
	}

	if ((lin_buf = (short *)calloc(sizeof(short),FRAMESIZE)) == NULL) {
		fprintf(stderr,"\nCannot allocate memory for linear PCM buffer.\n");
		exit(-1);
	}

	if ((u_buf = (unsigned char *)calloc(sizeof(unsigned char),FRAMESIZE)) == NULL) {
		fprintf(stderr,"\nCannot allocate memory for u-law buffer.\n");
		exit(-1);
	}

	if (APPEND_N) {
		/* fill buffer with 0's */
		for (i=0; i<APPEND_N; i++) {
			zero_buf[i]=0;
		}
	}

	if ((inFp=fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"ERROR:  Could not open input file \"%s\"\n",argv[1]);
		exit(1);
	}
	if ((outFp=fopen(argv[2],"w")) == NULL) {
		fprintf(stderr,"ERROR:  Could not open output file \"%s\"\n",argv[2]);
		exit(1);
	}

	if (STRIP_N) {
		/* strip STRIP_N samples from beginning of file */
		fread(lin_buf,sizeof(short),STRIP_N,inFp);
	}

	while ((items_read=fread(lin_buf,sizeof(short),FRAMESIZE,inFp)) == FRAMESIZE) {
		if (items_read>=FRAMESIZE) {
			ulaw_compress(lin_buf,u_buf,items_read);
			ulaw_expand(u_buf,lin_buf,items_read);
			fwrite (lin_buf,sizeof(short),items_read,outFp);
		}
	}

	if (APPEND_N) {
		/* append APPEND_N samples (zeros) to end of file */
		fwrite(zero_buf,sizeof(short),APPEND_N,outFp);
	}

	fclose(inFp);
	fclose(outFp);
}
