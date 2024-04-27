COMP=clang
OPTIONS=-Wall -Wextra -Werror -O2
DEBUG=-fsanitize=address,undefined -g3

SOURCES=sweep.c
OUTPUT=sweep
LIBS=-lraylib

final:
	${COMP} ${OPTIONS} ${SOURCES} ${LIBS} -o ${OUTPUT}

debug:
	${COMP} ${OPTIONS} ${DEBUG} ${SOURCES} ${LIBS} -o ${OUTPUT}
