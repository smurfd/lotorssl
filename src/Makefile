CC := gcc -O3 -ffast-math -Wall -Wextra -pedantic -std=c99 -march=native
BUILD := .build
SUBMODULES := lotormath
MODULES := mkbuilddir checksubmodules cryp hash ciph ec crpt crpt_srv crpt_cli

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

crpt:
	${CC} -c crpt.c -o ${BUILD}/crpt.o

crpt_cli:
	${CC} -o ${BUILD}/crpt_cli crpt_cli.c ${BUILD}/ciph.o ${BUILD}/crpt.o ${BUILD}/hash.o ${BUILD}/lotormath.o ${BUILD}/bmec.o -lm -lpthread

crpt_srv:
	${CC} -o ${BUILD}/crpt_srv crpt_srv.c ${BUILD}/ciph.o ${BUILD}/crpt.o ${BUILD}/hash.o ${BUILD}/lotormath.o ${BUILD}/bmec.o -lm -lpthread

test:
	${MAKE} -Ctests

test_local:
	${MAKE} -Ctests local
