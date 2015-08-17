#/*======================================================================*/
#/*  EVRC-B - Enhanced Variable Rate Coder - B Speech Service Option for */
#/*  Wideband Spread Spectrum Digital System                             */
#/*  C Source Code Simulation                                            */
#/*                                                                      */
#/*  Copyright (C) 2004 Qualcomm Incorporated. All rights                */
#/*  reserved.                                                           */
#/*----------------------------------------------------------------------*/

.PRECIOUS:	%

CC	= g++
#CFLAGS	= -g3 -Wno-deprecated -DMOT_SUPPRESSED_EIGHTH_RATE=1 #-DWMOPS_FX
#CFLAGS	= -g3 -Wno-deprecated -DMOT_SUPPRESSED_EIGHTH_RATE=1 #-DENABLE_IN_BAND_SIGNALLING_FOR_DTMF #-DWMOPS_FX
CFLAGS	= -g3 -Wno-deprecated #-DENABLE_IN_BAND_SIGNALLING_FOR_DTMF #-DWMOPS_FX
DEPEND	= -MM -Wno-deprecated
EXEC	= EvrcB_fx
CREATE_NCIM_INPUT = create_ncim_input
VOIP_TEST = EvrcB_ncim
LDFLAGS	= -lm
PURE	= /pkg/pure/purify/purify

FIXED_DIR = ../lib/
LIBDMATH = ${FIXED_DIR}/libdmath.a
DSP_DIR = ../dsp_lib/
TAB_DIR = ../tab/
SRC_DIR = ../src/
EXE_DIR = ../build/
TTY_DIR = ../tty/
LIBTTY = ${TTY_DIR}/libtty.a
DTMFDIR = ../dtmf
LIBDTMF = ${DTMFDIR}/libdtmf.a
LIBVOIP = EvrcB.a
VOIP_DIR = ../voip

MORE_DIR = -I${FIXED_DIR} -I${DSP_DIR} -I${TAB_DIR} -I${SRC_DIR} -I${TTY_DIR} -I${DTMFDIR}

LIBDIR= -L$(FIXED_DIR) -L$(DSP_DIR) -L$(TTY_DIR) -L$(DTMFDIR) 
LIBFILES = LIBDMATH LIBTTY LIBDTMF 
LIBS =  -ldmath -ltty -ldtmf -lm

OBJS =${SRC_DIR}main.o \
	${SRC_DIR}hpf80_fx.o \
	${SRC_DIR}r_fft.o \
	${SRC_DIR}ns127.o \
	${SRC_DIR}nacfap_fx.o \
	${SRC_DIR}rda_fx.o \
	${SRC_DIR}new_mode_fx.o \
	${SRC_DIR}fndppf_fx.o \
	${SRC_DIR}filt_fx.o \
	${SRC_DIR}globs_fx.o \
	${SRC_DIR}lpc_utils_fx.o \
	${SRC_DIR}lpcana_fx.o \
	${SRC_DIR}lsp_vq_fx.o \
	${SRC_DIR}encode_fx.o \
	${SRC_DIR}rcelp_fx.o \
	${SRC_DIR}cod3_10jcelp_fx.o \
	${SRC_DIR}dec3_10jcelp_fx.o \
	${SRC_DIR}cod7_35_fx.o \
	${SRC_DIR}factorial_pack_fx.o \
	${SRC_DIR}celp_fx.o \
	${SRC_DIR}nelp_fx.o \
	${SRC_DIR}uvgq_fx.o \
	${SRC_DIR}silence_fx.o \
	${SRC_DIR}voiced_fx.o \
	${SRC_DIR}WI_fx.o \
	${SRC_DIR}ppp_fx.o \
	${SRC_DIR}packet.o \
	${SRC_DIR}decode_fx.o \
	${SRC_DIR}fer_fx.o \
	${SRC_DIR}pf_fx.o \
	${SRC_DIR}bad_rate_fx.o \
	${SRC_DIR}blind_bg.o \
	${SRC_DIR}r_fft_float.o \
	${SRC_DIR}Init_globs.o \
	${SRC_DIR}SmartBlanking_fx.o \
	${SRC_DIR}dump.o

LIBVOIP_OBJS = \
	${VOIP_DIR}/jitter_buffer.o \
	${VOIP_DIR}/voip_client.o

CREATE_NCIM_INPUT_OBJS = ${VOIP_DIR}/create_ncim_input.o
VOIP_TEST_OBJS = ${VOIP_DIR}/voip_test.o

DSP_OBJ=${DSP_DIR}/dsp_math.o

all: ${EXEC} ${LIBVOIP} ${CREATE_NCIM_INPUT} ${VOIP_TEST}

${EXEC}: ${OBJS} ${DSP_OBJ} ${LIBTTY} ${LIBDTMF} ${LIBDMATH}
	${CC} -static ${CFLAGS} -o ${EXEC} ${OBJS}  ${DSP_OBJ} ${MORE_DIR} ${LIBDIR} $(LIBS)

${EXEC}_enc: ${EXEC}
	\rm -f ${EXEC}_enc;ln -s ${EXEC} ${EXEC}_enc

${EXEC}_dec: ${EXEC}
	\rm -f ${EXEC}_dec;ln -s ${EXEC} ${EXEC}_dec

pure: ${EXEC}_pure
${EXEC}_pure: ${OBJS} ${DATA}
	make -C ${FIXED_DIR}
	make -C ${TTY_DIR}
	make -C ${DTMFDIR}
	${PURE} ${CC} ${CFLAGS} -o ${EXEC}_pure ${OBJS} ${LDFLAGS}
	\rm -f ${EXEC}_enc_pure;ln -s ${EXEC}_pure ${EXEC}_enc_pure
	\rm -f ${EXEC}_dec_pure;ln -s ${EXEC}+pure ${EXEC}_dec_pure

${CREATE_NCIM_INPUT}: ${CREATE_NCIM_INPUT_OBJS}
	${CC} -o ${CREATE_NCIM_INPUT} ${CREATE_NCIM_INPUT_OBJS}

${VOIP_TEST}: ${VOIP_TEST_OBJS} ${LIBVOIP} ${CREATE_NCIM_INPUT}
	${CC} ${CFLAGS} -o ${VOIP_TEST} ${VOIP_TEST_OBJS} ${LIBVOIP} ${LIBDIR} ${LIBS}

${LIBVOIP}: ${OBJS} ${LIBVOIP_OBJS} ${DATA}
	ar rv ${LIBVOIP} ${OBJS} ${LIBVOIP_OBJS} ${DSP_OBJ} 
	ranlib ${LIBVOIP}

#$%.o:${SRC_DIR}%.cc
$(SRC_DIR)%.o:$(SRC_DIR)%.cc
	$(CC) $(CFLAGS) ${MORE_DIR} -c $< -o $@
$(DSP_DIR)%.o:$(DSP_DIR)%.c
	$(CC) $(CFLAGS) ${MORE_DIR} -c $< -o $@
#.cc.o:
#	${CC} ${CFLAGS} ${MORE_DIR} -c $*.cc
$(VOIP_DIR)%.o:$(VOIP_DIR)%.cc
	$(CC) $(CFLAGS) ${MORE_DIR} -c $< -o $@

${LIBDMATH}:
	make -C ${FIXED_DIR}

${LIBTTY}:
	make -C ${TTY_DIR}

${LIBDTMF}:
	make -C ${DTMFDIR}

clean:
	make -C ${FIXED_DIR} clean
	make -C ${TTY_DIR} clean
	make -C ${DTMFDIR} clean
	\rm -f ${DSP_DIR}*.o
	\rm -f ${SRC_DIR}*.o depend EvrcB_fx EvrcB_fx.exe EvrcB_fx_enc EvrcB_fx_dec
	\rm -f ${VOIP_DIR}/*.o
	\rm -f ${VOIP_DIR}*.o ${LIBVOIP} ${CREATE_NCIM_INPUT} ${VOIP_TEST}
