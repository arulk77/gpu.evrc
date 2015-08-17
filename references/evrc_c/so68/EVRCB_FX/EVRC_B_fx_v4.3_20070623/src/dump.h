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
/******************************************************************************
 * *                                                                            *
 * *     M O T O R O L A   C O N F I D E N T I A L   I N F O R M A T I O N      *
 * *                                                                            *
 * *                   Copyright 1994 - 2005, Motorola, Inc.                    *
 * *                                                                            *
 * *            This software has been provided to QUALCOMM under a             *
 * *           Source Code Loan Agreement effective December 5, 2002.           *
 * *                                                                            *
 * *****************************************************************************/

/* ##### Package date: Wed Sep 14 16:50:00 CDT 2005 ##### */ 

#/* $Id: dump.h,v 1.2 2006/12/07 00:02:24 apadmana Exp $ */
#/* $Name:  $ */
#/***********************************************************************
# *
# *   File Name: $CVSfile: $
# *              cvs name/path: $Source: /prj/vocoder/data15/FOURGV/FOURGV_FX/src/dump.h,v $
# *
# *   PURPOSE:
# *     
# *     
# *   RCS Data:
# *             File Version                  = $Revision: 1.2 $
# *             Archived Date (put/delta)     = $Date: 2006/12/07 00:02:24 $
# *             Archive Extraction Date (get) = N/A
# *
# *     
# *   KEYWORDS: 
# *     
# ***********************************************************************/
/*
 * dump.h
 *
 * Dump to file MATLAB interface rountine h file
 *
 * Copyright 1998, Motorola, Inc.  All Rights Reserved.
 *      Written by : James P. Ashley - 9/17/98
 *
 */

#ifndef	_DUMP_H_
#define	_DUMP_H_

void dump (

	void *		data,	/* (i) void pointer to the data to be dumped */
	const int	num,	/* (i) number of elements in data array */
	const char *	type,	/* (i) data type: "short", "long", "float", "double" */
	const char *	label	/* (i) data label to be logged */
);

#endif
