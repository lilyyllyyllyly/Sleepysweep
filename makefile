COMP=clang
OPTIONS=-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -O2
DEBUG=-fsanitize=address,undefined -g3

SOURCES=sweep.c
OUTPUT=sweep
LIBS=-lraylib

final:
	${COMP} ${OPTIONS} ${SOURCES} ${LIBS} -o ${OUTPUT}

debug:
	${COMP} ${OPTIONS} ${DEBUG} ${SOURCES} ${LIBS} -o ${OUTPUT}
