LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =
LIBS = -l pthread

SRC_QUEUE = block_list.c block_tpool.c
OBJ_QUEUE = ${SRC_QUEUE:.c=.o}

CC = gcc
CFLAGS = -std=c11 -c -g -Wall -Werror -pie -fPIC -pedantic ${INCS}
LDFLAGS = ${LIBS}

all: libblock_queue.so

.c.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libblock_queue.so: ${OBJ_QUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_QUEUE} ${LDFLAGS}

clean:
		@echo cleaning
		@rm -f ${OBJ_QUEUE}
		@rm -f test_block_list
		@rm -f test_block_tpool
