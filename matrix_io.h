#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include "matrix.h"

int wczytaj(const char *nazwa_pliku, macierz *m);

int zapisz(const char *nazwa_pliku, const macierz *m);

#define MAX_PLIKOW 100
#define DL_NAZWY_PLIKU 128

int lista_plikow(const char *folder, char nazwy[][DL_NAZWY_PLIKU], int max);

void wypisz(const macierz *m);

#endif
