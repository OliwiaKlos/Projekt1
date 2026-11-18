CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c90 -O2

ifeq ($(OS),Windows_NT)
  EXE = .exe
  RM  = -del /Q /F
  DEVNULL = 2>NUL
else
  EXE =
  RM  = rm -f
  DEVNULL =
endif

TARGET  = matrix$(EXE)
TEST    = test_matrix$(EXE)
OBJS    = main.o matrix.o matrix_io.o

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c matrix.h matrix_io.h
	$(CC) $(CFLAGS) -c $<

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c $<

matrix_io.o: matrix_io.c matrix_io.h matrix.h
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET) $(IN) $(OUT)

$(TEST): test_matrix.c matrix.c matrix_io.c matrix.h matrix_io.h
	$(CC) $(CFLAGS) -o $@ test_matrix.c matrix.c matrix_io.c -lm

test: $(TEST)
	./$(TEST)

clean:
	$(RM) $(OBJS) $(TARGET) $(TEST) test_matrix_tmp.txt wynik.txt $(DEVNULL)
