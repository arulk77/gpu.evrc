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

#define UVG1_CBSIZE 32
#define UVG2_CBSIZE 64


Word16 UVG1CB_fx[UVG1_CBSIZE][2] = {
{15417, 15515, },
{16813, 16609, },
{12832, 12803, },
{14264, 14189, },
{2019, 3490, },
{11728, 11182, },
{21154, 20908, },
{16946, 14614, },
{7915, 7558, },
{10229, 12751, },
{9968, 9760, },
{19482, 19070, },
{5809, 4933, },
{1457, 18891, },
{1790, 15579, },
{11218, 5696, },
{-16726, 19245, },
{3541, 21064, },
{6515, 12913, },
{1961, 6799, },
{-16371, 251, },
{8341, 20065, },
{13964, 8713, },
{5234, 9150, },
{-15829, 6105, },
{11791, 16457, },
{18203, 17590, },
{14816, 12343, },
{-14570, 10025, },
{1800, 11492, },
{4820, 17314, },
{15921, 18995, }};


Word16 UVG2CB_fx[2][UVG2_CBSIZE][5] = {
{
{2819, 3078, 3184, 3679, 9058, },
{2808, 3077, 4251, 3664, 3214, },
{5234, 3762, 3312, 3410, 4088, },
{2372, 2534, 2547, 2499, 2390, },
{494, 498, 555, 789, 10873, },
{3373, 3474, 3549, 3363, 4601, },
{3293, 4272, 3416, 3063, 3076, },
{3713, 2984, 3056, 3229, 3163, },
{5079, 3587, 2611, 2131, 1966, },
{2293, 2510, 2762, 3445, 4003, },
{669, 659, 782, 1145, 6827, },
{3799, 3701, 3357, 4666, 3601, },
{5533, 4390, 4205, 4971, 4274, },
{1239, 1441, 12686, 5717, 2836, },
{596, 627, 1081, 6089, 3531, },
{2522, 2747, 3293, 3823, 6178, },
{5149, 4846, 5890, 4311, 4177, },
{4658, 3902, 4410, 3594, 3070, },
{4511, 4314, 4017, 4288, 5891, },
{790, 944, 6574, 3766, 2530, },
{6979, 4897, 4451, 3591, 3357, },
{1255, 1338, 1898, 9600, 4635, },
{3937, 3438, 4913, 4409, 4508, },
{1664, 6566, 3780, 2317, 2088, },
{21641, 7448, 3186, 2319, 2067, },
{658, 686, 923, 6696, 8391, },
{3297, 4372, 5299, 3923, 3493, },
{3935, 6249, 4972, 4941, 4192, },
{6090, 6611, 5243, 4634, 4683, },
{4997, 5734, 3979, 3607, 3336, },
{2696, 3057, 3684, 5305, 4428, },
{8207, 3893, 2565, 2103, 2094, },
{603, 920, 8298, 7167, 5636, },
{6832, 8102, 4500, 2887, 2444, },
{1563, 10810, 4410, 2223, 1682, },
{3806, 4927, 3976, 4069, 4415, },
{1131, 10203, 15451, 7077, 4084, },
{3358, 3543, 4484, 7768, 8218, },
{1467, 9446, 6863, 5609, 4567, },
{15209, 6763, 3501, 8404, 6680, },
{4137, 4032, 4996, 6170, 4321, },
{5292, 5347, 4972, 6221, 5886, },
{9020, 7103, 5850, 4521, 3676, },
{2832, 16209, 6252, 3112, 2375, },
{10057, 5331, 24665, 12383, 9150, },
{3632, 4485, 5996, 5253, 6222, },
{12026, 5505, 3209, 2603, 2348, },
{433, 13553, 5499, 12490, 2350, },
{2535, 2603, 2724, 3649, 14508, },
{6345, 5898, 7706, 6354, 5001, },
{11210, 14897, 7082, 3392, 2093, },
{16156, 7837, 3860, 3042, 2290, },
{18671, 7619, 9295, 9382, 8397, },
{21, 18, 136, 13190, 17345, },
{1838, 2015, 7596, 12202, 5397, },
{2774, 4965, 9124, 4442, 3038, },
{3968, 3420, 9872, 12579, 12946, },
{1349, 1279, 1840, 14924, 6127, },
{5121, 11182, 5389, 15253, 11825, },
{12448, 9349, 8714, 5182, 3661, },
{5536, 9374, 10928, 5015, 2782, },
{45, 261, 12636, 20771, 5843, },
{7751, 4904, 4270, 4906, 5475, },
{6153, 6765, 6225, 6617, 8502, }
},

{
{4636, 3328, 3077, 3342, 3587, },
{1182, 5321, 2852, 1886, 1792, },
{554, 567, 915, 11563, 7603, },
{3084, 2796, 2973, 2850, 2613, },
{535, 598, 998, 5900, 7294, },
{3473, 3458, 3388, 4489, 3408, },
{6277, 18484, 18748, 7937, 9090, },
{3484, 4263, 3425, 3123, 2965, },
{4353, 4332, 4162, 5136, 3765, },
{1420, 1678, 2190, 2693, 4724, },
{5009, 3907, 4446, 3688, 3236, },
{3399, 3281, 4625, 3498, 3317, },
{4172, 3907, 6047, 4945, 4641, },
{3776, 2583, 1959, 1684, 1637, },
{2978, 3261, 3169, 3358, 4176, },
{394, 421, 506, 768, 7354, },
{4727, 3728, 3050, 2502, 2224, },
{644, 946, 5976, 3314, 2197, },
{744, 828, 1595, 5461, 3143, },
{4045, 3585, 4088, 3828, 4823, },
{5551, 5077, 5424, 4243, 4083, },
{3216, 3364, 3802, 5655, 5224, },
{2110, 4269, 7134, 3417, 2692, },
{3083, 2836, 2833, 3179, 6811, },
{5631, 4204, 4080, 4798, 5168, },
{620, 625, 877, 8645, 4408, },
{7474, 6557, 4012, 2610, 2350, },
{446, 441, 430, 846, 11353, },
{22021, 15275, 12882, 6972, 6160, },
{160, 174, 9417, 27777, 14396, },
{4346, 5006, 3540, 3645, 4351, },
{6923, 3467, 2208, 1790, 1623, },
{18170, 9939, 18680, 9969, 19434, },
{8016, 5700, 5210, 4568, 4927, },
{3883, 4840, 4695, 4415, 5621, },
{1573, 8166, 4042, 2387, 2043, },
{20987, 10754, 13491, 14672, 5342, },
{561, 21485, 22246, 8693, 4231, },
{3582, 4990, 4903, 3870, 3719, },
{2209, 2523, 4262, 9062, 5897, },
{672, 1062, 6325, 5249, 6586, },
{13079, 6830, 4267, 3163, 2816, },
{4921, 5442, 3764, 3138, 2789, },
{5324, 5753, 5769, 5196, 6478, },
{635, 1463, 9385, 10960, 5582, },
{856, 15093, 8932, 6034, 4147, },
{10141, 4569, 2692, 2212, 2301, },
{3002, 3062, 8600, 6370, 4250, },
{5028, 6864, 4774, 4484, 4107, },
{1169, 1366, 2460, 7177, 11765, },
{2314, 3751, 3540, 15004, 10668, },
{4199, 4389, 5599, 7052, 7384, },
{8626, 11728, 6743, 4104, 3542, },
{3937, 4496, 4243, 4278, 9024, },
{704, 1095, 11229, 5658, 3742, },
{6659, 4483, 3714, 3793, 3538, },
{6141, 7180, 8549, 6449, 5624, },
{2377, 11641, 4901, 3353, 3357, },
{2209, 2426, 2491, 2878, 10889, },
{17130, 9793, 6656, 6951, 5571, },
{1237, 1143, 1279, 2015, 16239, },
{8905, 6606, 5510, 8163, 5937, },
{4675, 5150, 5191, 6144, 4775, },
{1165, 7904, 6263, 5640, 4454, }}
};

