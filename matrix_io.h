#ifndef MATRIX_IO_H
#define MATRIX_IO_H

#include "matrix.h"

/* Wczytuje macierz z pliku tekstowego. Zwraca OK albo kod bledu
   (plik jest zawsze zamykany, *m zawiera sensowne dane tylko przy OK). */
int wczytaj(const char *nazwa_pliku, macierz *m);

/* Zapisuje macierz do pliku (dwie cyfry po kropce). Zwraca OK albo kod bledu.
   Przy bledzie zapisu nie zostaje niepelny plik wyniku (jesli nie istnial wczesniej). */
int zapisz(const char *nazwa_pliku, const macierz *m);

/* Wypisuje macierz na standardowe wyjscie. */
void wypisz(const macierz *m);

#endif
