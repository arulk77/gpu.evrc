
CCTARGET=./libdtmf.a
CCINCDIR=-I. -I../lib -I../src -I../tty
CC= g++
#CCFLAGS= -O2 -Wall -ansi
#CCFLAGS= -g -Wall -ansi
CCFLAGS= -g3 -Wall 

DTMF_HDRS=\
	agc.h \
	check.h \
	dtmf.h \
	dtmf20ms.h \
	dtmf_det.h \
	goertzel.h \
	init.h \
	tables.h \
	../lib/basic_op.h \
	../lib/basic_op40.h \
	../lib/typedef_fx.h

DTMF_OBJS=\
	agc.o \
	check.o \
	dtmf20ms.o \
	dtmf_det.o \
	goertzel.o \
	init.o \
        dtmf_gen.o\
	tables.o\
      dtmf_glob.o

.c.o:
	${CC} ${CCFLAGS} ${CCINCDIR} -c $*.c

${CCTARGET}: ${DTMF_OBJS} ${DTMF_HDRS}
	ar rv ${CCTARGET} ${DTMF_OBJS}
	ranlib ${CCTARGET}

${DTMF_OBJS}: ${DTMF_HDRS}

clean:
	rm -f *.o ${CCTARGET}
