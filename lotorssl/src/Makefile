CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD := .build
SUBMODULES := lotormath

all: mkbuilddir checksubmodules cryp hash keys ciph ec test
local: mkbuilddir checksubmodules cryp hash keys ciph ec test_local

mkbuilddir:
	mkdir -p ${BUILD}

checksubmodules:
	for s in ${SUBMODULES} ; do \
		if [ -d $$s ]; then \
			if [ -f "$$s/README.md" ]; then \
				echo $$s;\
			else \
				git submodule init; \
				git submodule update; \
			fi; \
		else \
			git submodule init; \
			git submodule update; \
		fi;\
	done

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

test:
	make -Ctests

test_local:
	make -Ctests local
