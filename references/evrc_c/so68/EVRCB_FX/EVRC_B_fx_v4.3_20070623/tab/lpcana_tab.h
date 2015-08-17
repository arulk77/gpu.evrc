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
/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
/*  reserved.                                                           */
/*----------------------------------------------------------------------*/
/*  Tables for LPC analysis such as windowing, autocorr,                */
/*======================================================================*/

static Word16 lsp_init_tab[10]={1573, 3146, 4719, 6291, 7864, 9437, 11010,
12583, 14156, 15729}; // Q15 for initialization
static Word16 lsp_int_tab[3]={0x1556, 0x4000, 0x6aaa}; // Q15

static Word16 gamma50_tab[10]={0x4000, 0x2000, 0x1000, 0x0800, 0x0400,
    0x0200, 0x0100, 0x0080, 0x0040, 0x0020}; // Q15, 0.5^i
    
static Word16 gamma90_tab[10]={0x7333, 0x67ae, 0x5d50, 0x53fb, 0x4b95, 
    0x4406, 0x3d39, 0x371a, 0x3197, 0x2ca1}; // Q15, 0.9^i

static Word16 gamma994_tab[10]={32571, 32376, 32182, 31989, 31797, 31606,
    31416, 31228, 31040, 30854}; // Q15, 0.994^i

static Word16 gamma9821_tab[10]={32182,31606,31040,30485,29940,29404,28878,28361,27854,27355}; // Q15, 0.9821^i
//bandwidth expansion table for fac = 0.9821 (0.994^3)

//static Word16 gamma9762_tab[10]={31989,31228,30485,29760,29052,28361,27687,27028,26385,25758}; // Q15, 0.9762^i
   

static Word32 w_corr[17]={ // Q31
64425,  // actual w_corr(0)-1.0
2147218688,
2146424192,
2145100544,
2143248896,
2140870400,
2137967104,
2134540928,
2130594432,
2126130688,
2121152768,
2115664384,
2109669632,
2103172992,
2096178944,
2088692736,
2080719744
};

static Word16 lpc_window[320]= { // Q16 unsigned
0x0,
0x0,
0x0,
0x0,
0x0,
0x1,
0x1,
0x1,
0x2,
0x2,
0x3,
0x4,
0x4,
0x5,
0x6,
0x7,
0x9,
0xa,
0xb,
0xd,
0xf,
0x11,
0x13,
0x15,
0x17,
0x1a,
0x1d,
0x20,
0x23,
0x27,
0x2a,
0x2e,
0x33,
0x37,
0x3c,
0x41,
0x47,
0x4c,
0x52,
0x59,
0x60,
0x67,
0x6f,
0x77,
0x7f,
0x88,
0x92,
0x9c,
0xa6,
0xb1,
0xbd,
0xc9,
0xd6,
0xe3,
0xf1,
0x100,
0x110,
0x120,
0x131,
0x143,
0x155,
0x168,
0x17d,
0x192,
0x1a8,
0x1bf,
0x1d7,
0x1f0,
0x20b,
0x226,
0x242,
0x260,
0x27f,
0x29f,
0x2c0,
0x2e3,
0x307,
0x32c,
0x353,
0x37c,
0x3a6,
0x3d2,
0x3ff,
0x42e,
0x45f,
0x491,
0x4c6,
0x4fc,
0x535,
0x56f,
0x5ab,
0x5ea,
0x62b,
0x66e,
0x6b3,
0x6fb,
0x745,
0x791,
0x7e1,
0x833,
0x887,
0x8de,
0x939,
0x996,
0x9f6,
0xa59,
0xabf,
0xb29,
0xb96,
0xc06,
0xc79,
0xcf1,
0xd6b,
0xdea,
0xe6c,
0xef2,
0xf7c,
0x100a,
0x109c,
0x1132,
0x11cc,
0x126b,
0x130f,
0x13b6,
0x1463,
0x1514,
0x15ca,
0x1685,
0x1745,
0x180a,
0x18d4,
0x19a3,
0x1a78,
0x1b52,
0x1c32,
0x1d17,
0x1e02,
0x1ef3,
0x1fe9,
0x20e6,
0x21e9,
0x22f1,
0x2401,
0x2516,
0x2632,
0x2754,
0x287d,
0x29ad,
0x2ae3,
0x2c20,
0x2d64,
0x2eaf,
0x3001,
0x315a,
0x32bb,
0x3422,
0x3591,
0x3707,
0x3885,
0x3a0a,
0x3b97,
0x3d2b,
0x3ec7,
0x406a,
0x4215,
0x43c8,
0x4583,
0x4745,
0x4910,
0x4ae2,
0x4cbb,
0x4e9d,
0x5086,
0x5277,
0x5470,
0x5671,
0x5879,
0x5a89,
0x5ca0,
0x5ebf,
0x60e6,
0x6314,
0x6549,
0x6786,
0x69c9,
0x6c14,
0x6e66,
0x70be,
0x731d,
0x7583,
0x77ef,
0x7a61,
0x7cd9,
0x7f57,
0x81db,
0x8464,
0x86f2,
0x8985,
0x8c1c,
0x8eb8,
0x9159,
0x93fd,
0x96a4,
0x994f,
0x9bfc,
0x9eac,
0xa15e,
0xa412,
0xa6c7,
0xa97d,
0xac33,
0xaeea,
0xb1a0,
0xb455,
0xb709,
0xb9ba,
0xbc69,
0xbf16,
0xc1be,
0xc463,
0xc702,
0xc99c,
0xcc30,
0xcebe,
0xd144,
0xd3c1,
0xd636,
0xd8a2,
0xdb03,
0xdd5a,
0xdfa4,
0xe1e2,
0xe412,
0xe635,
0xe848,
0xea4b,
0xec3e,
0xee1f,
0xefed,
0xf1a8,
0xf34e,
0xf4df,
0xf65a,
0xf7be,
0xf90a,
0xfa3d,
0xfb55,
0xfc53,
0xfd35,
0xfdf9,
0xfea0,
0xff28,
0xff90,
0xffd7,
0xfffd,
0xffff,
0xffdf,
0xff9a,
0xff30,
0xfe9f,
0xfde7,
0xfd07,
0xfbfe,
0xfacc,
0xf96f,
0xf7e8,
0xf635,
0xf456,
0xf24a,
0xf011,
0xedaa,
0xeb15,
0xe851,
0xe55f,
0xe23f,
0xdeef,
0xdb70,
0xd7c3,
0xd3e7,
0xcfdd,
0xcba5,
0xc73f,
0xc2ae,
0xbdf0,
0xb907,
0xb3f5,
0xaeba,
0xa958,
0xa3d1,
0x9e26,
0x985a,
0x926e,
0x8c64,
0x8641,
0x8006,
0x79b6,
0x7356,
0x6ce8,
0x6671,
0x5ff5,
0x5978,
0x52ff,
0x4c90,
0x4630,
0x3fe5,
0x39b5,
0x33a8,
0x2dc4,
0x2810,
0x2296,
0x1d5d,
0x186e,
0x13d4,
0xf98,
0xbc4,
0x864,
0x584,
0x32f,
0x174,
0x60
};


static Word16 pci_init_tab[ORDER]={-2390,500,-238,143,-99,74,-56,44,-34,23}; //Q12

