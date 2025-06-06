CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD := .build

all: mkbuilddir hash ecdsa ec bmec test testec testbm
local: mkbuilddir hash ecdsa ec bmec test_local

mkbuilddir:
	mkdir -p ${BUILD}

ecdsa:
	${CC} -c ecdsa.c -o .build/ecdsa.o

ec:
	${CC} -c ec.c -o .build/ec.o

hash:
	${CC} -c ../../../hash.c -o .build/hash.o

bmec:
	${CC} -c bm.c -o .build/bm.o
	${CC} -c bmcurve.c -o .build/bmcurve.o

test:
	make -Ctests

test_local:
	make -Ctests local

testec:
	make -Ctests ec

testbm:
	make -Ctests bm
