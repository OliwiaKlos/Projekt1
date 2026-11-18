#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

void zapisz(char *nazwa, macierz m);
macierz wczytaj(char *nazwa_pliku);
void wypisz(macierz m);

#endif
