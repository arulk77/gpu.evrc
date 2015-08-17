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

#include "ehwutl.h"

static nfirst;
double sFrct[32 + 8];
double dFrct[32 + 8];
static double maxSfrct;
static double minSfrct;
static int msb ={15};
static int lsb ={0};
char line[200];

static void init()
{
	int n;
	nfirst = 1;
	/*printf("%d %d signed max\n",INT_MAX,INT_MIN);
	 * printf("%U unsigned max\n",UINT_MAX);
	 * if (UINT_MAX < 4294967295U){
	 * printf("you need to convert ints to long ints \n");
	 * exit(1);
	 * }
	 */
	for (n = 0; n < 16; n++)
	{
		sFrct[n] = pow(2.0, -((double) 15.0 - (double) n));
	}
	maxSfrct = 1.0 - sFrct[0];
	minSfrct = -1.0;

	for (n = 0; n < 32; n++)
	{
		dFrct[n] = pow(2.0, -((double) 31.0 - (double) n));
	}
	for (n = 31; n < 40; n++)
	{
		dFrct[n] = pow(2.0, -(31.0 - (double) n));
	}
}

static void pHex(char *in, int num)
{
	/* The first num characters are checked for ' ' and
	 * replaced with '0' */
	int i;
	for (i = 0; i < num; i++)
	{
		if (line[i] == ' ')
			printf("0");
		else
			printf("%c", line[i]);
	}
}
void xDispSw(Shortword in)
{
	printf("0x");
	sprintf(line, "%4x", in);
	pHex(line, 4);
	printf(" ");
}
void xDispSns(struct NormSw snsIn)
{

	printf("0x");
	sprintf(line, "%4x", snsIn.man);
	pHex(line, 4);
	printf(" ");

	sprintf(line, "%2d", snsIn.sh);
	pHex(line, 2);
	printf(" ");

}
void xDispLw(Longword in)
{

	printf("0x");
	sprintf(line, "%4x", (in >> 16) & 0xffff);
	pHex(line, 4);
	printf(" ");
	sprintf(line, "%4x", in & 0xffff);
	pHex(line, 4);
	printf(" ");

}
double toFloatLw(Longword lwIn)
{
	/* convert signed fractional number to a double */

	double output;
	int i;
	unsigned long mask;
	int negative;

	if (!nfirst)
		init();

	output = 0.0;
	negative = 0;

	if (lwIn < 0)
	{
		/* negative */
		negative = 1;
		lwIn = -lwIn;
	}
	if (lwIn >= 0)
	{
		/* now positive */
		for (mask = 0x1, i = 0; i < 32; i++)
		{
			if (mask & lwIn)
				output += dFrct[i];
			mask <<= 1;
		}
		if (negative)
			output *= -1.0;
	}
	else
	{
		output = -1.0;
	}
	return (output);
}
double toFloatSw(Shortword swIn)
{
	return (toFloatLw((Longword) swIn * (Longword) 0x10000));
}
double toFloatSns(struct NormSw snsIn)
{
	double dOut;
	dOut = toFloatSw(snsIn.man);
	if (snsIn.sh == 0)
		return (dOut);
	else if (snsIn.sh < 0)
		return (dOut * pow(2.0, -snsIn.sh));
	else
		return (dOut / pow(2.0, snsIn.sh));
}
void fDispSw(Shortword swIn)
{
	printf("%11.8f ", toFloatSw(swIn));
}
void fDispLw(Longword lwIn)
{
	printf("%11.8f ", toFloatLw(lwIn));
}
void fDispSns(struct NormSw snsIn)
{
	printf("%11.8f ", toFloatSns(snsIn));
}
Longword toLwFloat(double dIn)
{
	long l1;
	long int liExt, liInt;
	unsigned long ulExt = 0;
	Longword L_Out;

	if (dIn > -(double) (1 << 0) / 2147483648.0 && dIn < 0)
	{
		L_Out = 0;
	}
	else
	{
		liInt = dIn;			/* integer portion */

		if (liInt != 0)
		{
			if (liInt < 0)
			{
				L_Out = LW_MIN;
			}
			else
			{
				L_Out = LW_MAX;
			}
		}
		else
		{

			/*
			 *  shift up, perform limiting and convergent rounding
			 */

			dIn *= 2147483648.0;	/* 2**31 */

			/* round */
			dIn += (dIn >= 0.0) ? 0.5 : -0.5;
			l1 = dIn;
			if (dIn == (double) l1)
			{
				l1 -= l1 % 2;
			}
			/* assign the fractional portion to frac */
			L_Out = l1;
		}
	}
	return (L_Out);
}

Shortword toSwFloat(double dIn)
{

	return (round(toLwFloat(dIn)));
}
