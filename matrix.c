#include "matrix.h"

macierz dodaj(macierz lewa, macierz prawa) {
    macierz wynik = {0};
    int i, j;

    if (lewa.w != prawa.w || lewa.k != prawa.k) {
        printf("blad. niezgodne wymiary macierzy do operacji\n");
        exit(2);
    }

    wynik.w = lewa.w;
    wynik.k = lewa.k;

    for (i = 0; i < lewa.w; i++) {
        for (j = 0; j < lewa.k; j++) {
            wynik.wartosci[i][j] = lewa.wartosci[i][j] + prawa.wartosci[i][j];
        }
    }

    return wynik;
}

macierz odejmij(macierz lewa, macierz prawa) {
    macierz wynik = {0};
    int i, j;

    if (lewa.w != prawa.w || lewa.k != prawa.k) {
        printf("blad. niezgodne wymiary macierzy do operacji\n");
        exit(2);
    }

    wynik.w = lewa.w;
    wynik.k = lewa.k;

    for (i = 0; i < lewa.w; i++) {
        for (j = 0; j < lewa.k; j++) {
            wynik.wartosci[i][j] = lewa.wartosci[i][j] - prawa.wartosci[i][j];
        }
    }

    return wynik;
}

macierz mnoz(macierz lewa, macierz prawa) {
    macierz wynik = {0};
    int i, j, k;

    if (lewa.k != prawa.w) {
        printf("blad. niezgodne wymiary macierzy do operacji\n");
        exit(2);
    }

    wynik.w = lewa.w;
    wynik.k = prawa.k;

    for (i = 0; i < lewa.w; i++) {
        for (j = 0; j < prawa.k; j++) {
            for (k = 0; k < lewa.k; k++) {
                wynik.wartosci[i][j] += lewa.wartosci[i][k] * prawa.wartosci[k][j];
            }
        }
    }

    return wynik;
}
