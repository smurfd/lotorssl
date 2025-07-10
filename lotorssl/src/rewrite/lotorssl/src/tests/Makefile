CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD := ../.build

all: build runner
local: build runner_local
ec: buildec runnerec
bm: buildbm runnerbm
field: buildfield runnerfield

build:
	${CC} -o ${BUILD}/tests tests.c ${BUILD}/ecdsa.o ${BUILD}/hash.o ${BUILD}/bm.o ${BUILD}/bmcurve.o -lpthread -lm

buildec:
	${CC} -o ${BUILD}/testsec testec.c ${BUILD}/ec.o ${BUILD}/hash.o -lpthread -lm

buildbm:
	${CC} -o ${BUILD}/testsbm testbm.c ${BUILD}/bm.o ${BUILD}/bmec.o -lpthread -lm

buildfield:
	${CC} -o ${BUILD}/testsfield testfield.c ${BUILD}/bm.o ${BUILD}/bmcurve.o -lpthread -lm

runner:
	${BUILD}/tests

runnerec:
	${BUILD}/testsec

runnerbm:
	${BUILD}/testsbm

runnerfield:
	${BUILD}/testsfield

runner_local:
	${BUILD}/tests local
