CFLAGS=-Wall -DUSE_ACCELERATE
LDFLAGS=-framework Accelerate
test: test.o libhue.o
