CC := gcc -O3 -ffast-math -Wall -pedantic -std=c11 -march=native
BUILD := .build
SUBMODULES := lotormath
MODULES := mkbuilddir checksubmodules cryp hash ciph ec

all: ${MODULES} test
local: ${MODULES} test_local
build: ${MODULES}

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
	${MAKE} -Ctests

test_local:
	${MAKE} -Ctests local
