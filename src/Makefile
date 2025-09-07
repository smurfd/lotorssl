CC := gcc -O3 -ffast-math -Wall -pedantic -std=c99
BUILD := .build
SUBMODULES := lotormath

all: mkbuilddir checksubmodules cryp hash ciph ec test
local: mkbuilddir checksubmodules cryp hash ciph ec test_local

mkbuilddir:
	mkdir -p ${BUILD}

checksubmodules:
	for s in ${SUBMODULES} ; do \
		if [ -d $$s ]; then \
			echo "Deleting $$s ..."; \
			rm -rf $$s; \
		fi;\
		git submodule add --force https://github.com/smurfd/$$s; \
		git submodule init; \
		git submodule update; \
	done

cryp:
	${CC} -c cryp.c -o ${BUILD}/cryp.o

hash:
	${CC} -c hash.c -o ${BUILD}/hash.o

ciph:
	${CC} -c ciph.c -o ${BUILD}/ciph.o

ec:
	${CC} -c lotormath/src/lotormath.c -o ${BUILD}/lotormath.o
	${CC} -c bmec.c -o ${BUILD}/bmec.o

test:
	make -Ctests

test_local:
	make -Ctests local
