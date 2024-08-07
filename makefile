CC = gcc
MPI = mpicc
CFLAGS = -I./include
SRCDIR = utils/src
INCDIR = utils/headers

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=%.o) 
OBJS += sequential_test.o
OBJS += parallel_test.o
DEPS = $(wildcard $(INCDIR)/*.h)
TARGET = test

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -O3 && rm -f $(OBJS)

seq_test.o: sequential_test.c
	$(CC) -c sequential_test.c -o sequential_test.o

parallel_test.o: parallel_test.c
	$(CC) -c parallel_test.c -o parallel_test.o

%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -f $(OBJS) 