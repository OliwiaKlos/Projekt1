#define _POSIX_C_SOURCE 200112L   /* dla opendir/readdir przy -std=c90 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <float.h>
#include <dirent.h>
#include "matrix.h"
#include "matrix_io.h"

#define DL_TOKENU 128

/* Czyta kolejny token oddzielony bialymi znakami; 1 = ok, 0 = koniec pliku. */
static int czytaj_token(FILE *f, char *bufor) {
    return fscanf(f, "%127s", bufor) == 1;
}

static int parsuj_wymiar(const char *token, int *wartosc) {
    char *koniec;
    long v;

    if (token[strspn(token, "0123456789+-")] != '\0')
        return ERR_NAGLOWEK;

    errno = 0;
    v = strtol(token, &koniec, 10);
    if (koniec == token || *koniec != '\0')
        return ERR_NAGLOWEK;
    if (errno == ERANGE || v < 1 || v > MAX)
        return ERR_WYMIARY_ZAKRES;

    *wartosc = (int)v;
    return OK;
}

static int parsuj_liczbe(const char *token, double *wartosc) {
    char *koniec;
    double v;

    /* dozwolone tylko znaki liczby dziesietnej: odrzuca nan, inf, hex itp. */
    if (token[strspn(token, "0123456789+-.eE")] != '\0')
        return ERR_ELEMENT_NIELICZBOWY;

    errno = 0;
    v = strtod(token, &koniec);
    if (koniec == token || *koniec != '\0')
        return ERR_ELEMENT_NIELICZBOWY;
    if (v > DBL_MAX || v < -DBL_MAX)   /* przepelnienie (np. 1e400) */
        return ERR_ELEMENT_NIELICZBOWY;

    *wartosc = v;
    return OK;
}

static int wczytaj_z_pliku(FILE *f, macierz *m) {
    char token[DL_TOKENU];
    int i, j, kod;

    if (!czytaj_token(f, token))
        return ERR_NAGLOWEK;
    kod = parsuj_wymiar(token, &m->w);
    if (kod != OK)
        return kod;

    if (!czytaj_token(f, token))
        return ERR_NAGLOWEK;
    kod = parsuj_wymiar(token, &m->k);
    if (kod != OK)
        return kod;

    for (i = 0; i < m->w; i++) {
        for (j = 0; j < m->k; j++) {
            if (!czytaj_token(f, token))
                return ERR_BRAK_ELEMENTU;
            kod = parsuj_liczbe(token, &m->wartosci[i][j]);
            if (kod != OK)
                return kod;
        }
    }

    return OK;
}

int wczytaj(const char *nazwa_pliku, macierz *m) {
    FILE *f;
    macierz tmp;
    int kod;

    f = fopen(nazwa_pliku, "r");
    if (f == NULL)
        return ERR_OTWARCIE_PLIKU;

    kod = wczytaj_z_pliku(f, &tmp);
    fclose(f);

    if (kod == OK)
        *m = tmp;
    return kod;
}

int zapisz(const char *nazwa_pliku, const macierz *m) {
    FILE *f;
    int i, j, blad = 0, istnial = 0;

    f = fopen(nazwa_pliku, "r");
    if (f != NULL) {
        istnial = 1;
        fclose(f);
    }

    f = fopen(nazwa_pliku, "w");
    if (f == NULL)
        return ERR_ZAPIS_OTWARCIE;

    if (fprintf(f, "%d %d\n", m->w, m->k) < 0)
        blad = 1;

    for (i = 0; i < m->w; i++) {
        for (j = 0; j < m->k; j++) {
            if (!blad && fprintf(f, j ? " %.2f" : "%.2f", m->wartosci[i][j]) < 0)
                blad = 1;
        }
        if (!blad && fprintf(f, "\n") < 0)
            blad = 1;
    }

    if (fclose(f) != 0)   /* tu wychodza np. bledy "dysk pelny" */
        blad = 1;

    if (blad) {
        if (!istnial)
            remove(nazwa_pliku);
        return ERR_ZAPIS;
    }
    return OK;
}

static int porownaj_nazwy(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

int lista_plikow(const char *folder, char nazwy[][DL_NAZWY_PLIKU], int max) {
    DIR *d;
    struct dirent *wpis;
    int n = 0;
    size_t dl;

    d = opendir(folder);
    if (d == NULL)
        return -1;

    while (n < max && (wpis = readdir(d)) != NULL) {
        dl = strlen(wpis->d_name);
        if (dl < 5 || dl >= DL_NAZWY_PLIKU)
            continue;
        if (strcmp(wpis->d_name + dl - 4, ".txt") != 0)
            continue;
        strcpy(nazwy[n], wpis->d_name);
        n++;
    }
    closedir(d);

    qsort(nazwy, (size_t)n, DL_NAZWY_PLIKU, porownaj_nazwy);
    return n;
}

void wypisz(const macierz *m) {
    int i, j;

    printf("\n");
    for (i = 0; i < m->w; i++) {
        printf("  [ ");
        for (j = 0; j < m->k; j++)
            printf("%.2f ", m->wartosci[i][j]);
        printf("]\n");
    }
    printf("\n");
}
