COMP=clang
OPTIONS=-Wall -Wextra -Werror -O2
DEBUG=-fsanitize=address,undefined -g3

SOURCES=sweep.c
OUTPUT=sweep
LIBS=-lraylib

OPTIONS_WEB=-I$(RAYLIB_SRC) -L$(RAYLIB_SRC) -sUSE_GLFW=3 -sGL_ENABLE_GET_PROC_ADDRESS -DPLATFORM_WEB -sALLOW_MEMORY_GROWTH
SHELL_FILE=shell.html
OUTPUT_WEB=index.html

final:
	${COMP} ${OPTIONS} ${SOURCES} ${LIBS} -o ${OUTPUT}

debug:
	${COMP} ${OPTIONS} ${DEBUG} ${SOURCES} ${LIBS} -o ${OUTPUT}


web:
	emcc $(OPTIONS) $(SOURCES) $(RAYLIB_SRC)/libraylib.a $(OPTIONS_WEB) -o $(OUTPUT_WEB) --shell-file ${SHELL_FILE}
