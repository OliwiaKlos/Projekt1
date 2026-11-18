CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99 -O2
TARGET  = matrix
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

# przyklad: make run IN=dane.txt OUT=wynik.txt
run: $(TARGET)
	./$(TARGET) $(IN) $(OUT)

# testy: pliki A.txt, B.txt, ... powinny lezec w biezacym katalogu
test_matrix: test_matrix.c matrix.c matrix_io.c matrix.h matrix_io.h
	$(CC) $(CFLAGS) -o $@ test_matrix.c matrix.c matrix_io.c -lm

test: test_matrix
	./test_matrix

clean:
	rm -f $(OBJS) $(TARGET) test_matrix test_matrix_tmp.txt
