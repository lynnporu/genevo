CC = gcc
CFLAGS = -fPIC -Wall -Wextra -O3 -g
LDFLAGS = -shared
RM = rm -f
TARGET_LIB = bin/genevo.so
PREPROCESSED_LIB = temp/genevo.c

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)

.PHONY: preprocess
preprocess:
	$(CC) ${SRCS} -E > ${PREPROCESSED_LIB}
