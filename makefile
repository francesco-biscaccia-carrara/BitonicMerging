CC = gcc
MPI = mpicc
CFLAGS = -I./include
SRCDIR = utils/src
INCDIR = utils/headers

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=%.o)
SEQ_OBJS = sequential_test.o $(OBJS)
PAR_OBJS = parallel_test.o $(OBJS)
DEPS = $(wildcard $(INCDIR)/*.h)

TARGET_SEQ = test_seq
TARGET_PAR = test_par

.PHONY: all clean

all: $(TARGET_SEQ) $(TARGET_PAR) clean

$(TARGET_SEQ): $(SEQ_OBJS)
	$(CC) $(SEQ_OBJS) -o $(TARGET_SEQ) -O3 

$(TARGET_PAR): $(PAR_OBJS)
	$(MPI) $(PAR_OBJS) -o $(TARGET_PAR) -O3 

sequential_test.o: sequential_test.c
	$(CC) -c sequential_test.c -o sequential_test.o

parallel_test.o: parallel_test.c
	$(MPI) $(CFLAGS) -c parallel_test.c -o parallel_test.o

%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SEQ_OBJS) $(PAR_OBJS)
