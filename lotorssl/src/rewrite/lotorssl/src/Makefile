CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD := .build

all: mkbuilddir hash ecdsa ec bmec test testec testbm testfield
local: mkbuilddir hash ecdsa ec bmec test_local

mkbuilddir:
	mkdir -p ${BUILD}

ecdsa:
	${CC} -c ecdsa.c -o .build/ecdsa.o -lm

ec:
	${CC} -c ec.c -o .build/ec.o -lm

hash:
	${CC} -c ../../../hash.c -o .build/hash.o -lm

bmec:
	${CC} -c bm.c -o .build/bm.o -lm
	${CC} -c bmec.c -o .build/bmec.o -lm
	${CC} -c bmcurve.c -o .build/bmcurve.o -lm
#bmfield:
#	${CC} -c bmfield.c -o .build/bmfield.o

test:
	make -Ctests

test_local:
	make -Ctests local

testec:
	make -Ctests ec

testbm:
	make -Ctests bm

testfield:
	make -Ctests field

