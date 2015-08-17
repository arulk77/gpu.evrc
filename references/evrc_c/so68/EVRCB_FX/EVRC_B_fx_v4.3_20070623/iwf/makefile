
EXEC =./fourgv_iwf.exe
CC= g++
CCFLAGS = -O3 -Wno-deprecated 
SRC_DIR = ../src/

OBJS =${SRC_DIR}packet.o 

${EXEC}: fourgv_iwf.cc
	${CC} -static ${CCFLAGS} -o ${EXEC} ${OBJS} fourgv_iwf.cc

clean:
	rm -f ${EXEC}
