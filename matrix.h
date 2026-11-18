#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

#define MAX 20

typedef struct _macierz {
    int w;
    int k;
    double wartosci[MAX][MAX];
} macierz;

macierz dodaj(macierz lewa, macierz prawa);
macierz odejmij(macierz lewa, macierz prawa);
macierz mnoz(macierz lewa, macierz prawa);

#endif
