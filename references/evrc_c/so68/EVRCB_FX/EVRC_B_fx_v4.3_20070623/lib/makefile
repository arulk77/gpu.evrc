#/*======================================================================*/
#/*  4GV - Fourth Generation Vocoder Speech Service Option for             */
#/*  Wideband Spread Spectrum Digital System                             */
#/*  C Source Code Simulation                                            */
#/*                                                                      */
#/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
#/*  reserved.                                                           */
#/*----------------------------------------------------------------------*/
CC	= g++
CFLAGS	= -g3 -I../src/ #-DWMOPS_FX
CCTARGET=libdmath.a

FIXED_OBJS= 	${FIXED_DIR}basic_op.o \
		${FIXED_DIR}basic_op40.o \
		${FIXED_DIR}math_ext32.o \
		${FIXED_DIR}math_adv.o \
		${FIXED_DIR}lib_wmp_fx.o

.c.o:
	${CC} ${CFLAGS}  -c $*.c

${CCTARGET}: ${FIXED_OBJS}
	ar cr ${CCTARGET} $(FIXED_OBJS)
	ranlib ${CCTARGET}

clean:
	rm -f *.o ${CCTARGET} core
