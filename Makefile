LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =
LIBS = -l pthread

SRC_BL = bl.c bltpool.c
OBJ_BL = ${SRC_BL:.c=.o}
BL_LDFLAGS = ${LIBS}

SRC_TEST_BL = test_bl.c
OBJ_TEST_BL = ${SRC_TEST_BL:.c=.o}
SRC_TEST_BLTPOOL = test_bltpool.c
OBJ_TEST_BLTPOOL = ${SRC_TEST_BLTPOOL:.c=.o}
TEST_LDFLAGS = -L $(CURDIR) -l blqueue -Wl,-rpath,$(CURDIR)

CC = gcc
CFLAGS = -std=c99 -c -g -Wall -Werror -pie -fPIC -pedantic ${INCS}

all: libblqueue.so test_bl test_bltpool

.c.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libblqueue.so: ${OBJ_BL}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_BL} ${BL_LDFLAGS}

test_bl: ${OBJ_TEST_BL}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TEST_BL} ${TEST_LDFLAGS} 

test_bltpool: ${OBJ_TEST_BLTPOOL}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TEST_BLTPOOL} ${TEST_LDFLAGS}

clean:
		@echo Cleaning
		@rm -f ${OBJ_TEST_BL} ${OBJ_TEST_BLTPOOL} ${OBJ_BL}
		@rm -f libblqueue.so test_bl test_bltpool
