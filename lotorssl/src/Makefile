CC:=gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD:=.build

all: mkbuilddir cryp hash keys ciph ec test
local: mkbuilddir cryp hash keys ciph test_local

mkbuilddir:
	mkdir -p ${BUILD}

cryp:
	${CC} -c cryp.c -o ${BUILD}/cryp.o

hash:
	${CC} -c hash.c -o ${BUILD}/hash.o

keys:
	${CC} -c keys.c -o ${BUILD}/keys.o

ciph:
	${CC} -c ciph.c -o ${BUILD}/ciph.o

ec:
	${CC} -c lotormath/src/lotormath.c -o ${BUILD}/lotormath.o
	${CC} -c bmec.c -o ${BUILD}/bmec.o
	${CC} -o ${BUILD}/testbmec tests/testbm.c ${BUILD}/lotormath.o ${BUILD}/bmec.o
	${BUILD}/testbmec
test:
	make -Ctests

test_local:
	make -Ctests local
