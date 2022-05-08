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

.PHONY: diag_state_machine_test
diag_state_machine_test:
	cp ./tests/diag_state_machine_test.c .
	gcc diag_state_machine_test.c $(SRCS) \
		-lm \
		-o ./bin/diag_state_machine_test
	rm diag_state_machine_test.*

.PHONY: uniform_state_machine_test
uniform_state_machine_test:
	cp ./tests/uniform_state_machine_test.c .
	gcc uniform_state_machine_test.c $(SRCS) \
		-lm \
		-o ./bin/uniform_state_machine_test
	rm uniform_state_machine_test.*

.PHONY: rand_test
rand_test:
	cp ./tests/rand_test.c .
	gcc rand_test.c $(SRCS) \
		-lm \
		-o ./bin/rand_test
	rm rand_test.*

.PHONY: flip_bits_test
flip_bits_test:
	cp ./tests/flip_bits_test.c .
	gcc flip_bits_test.c $(SRCS) \
		-lm \
		-o ./bin/flip_bits_test
	rm flip_bits_test.*
