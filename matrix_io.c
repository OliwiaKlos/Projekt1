#include "matrix.h"
#include "matrix_io.h"

void wypisz(macierz m) {
    int i, j;

    printf("\n");
    for (i = 0; i < m.w; i++) {
        printf("  [ ");
        for (j = 0; j < m.k; j++) {
            printf("%.2f ", m.wartosci[i][j]);
        }
        printf("]\n");
    }
    printf("\n");
}

macierz wczytaj(char *nazwa_pliku) {
    macierz m = {0};
    FILE *fin;
    int i, j;
	
    printf("bede probowac wczytac dane z pliku: %s\n", nazwa_pliku);

    fin = fopen(nazwa_pliku, "r");
    if (fin == NULL) {
        printf("nie udalo sie otworzyc pliku: %s\n", nazwa_pliku);
        exit(1);
    }

    if (fscanf(fin, "%d %d", &m.w, &m.k) != 2) {
        printf("bledny format pliku. podaj liczbe wierszy/kolumn w pliku: %s\n", nazwa_pliku);
        fclose(fin);
        exit(2);
    }

    if (m.w <= 0 || m.k <= 0 || m.w > MAX || m.k > MAX) {
        printf("niepoprawne wymiary macierzy: %d x %d\n", m.w, m.k);
        fclose(fin);
        exit(4);
    }

    for (i = 0; i < m.w; i++) {
        for (j = 0; j < m.k; j++) {
            if (fscanf(fin, "%lf", &m.wartosci[i][j]) != 1) {
                printf("bledne dane w pliku. wartosc wierszy i kolumn niezgodne z wartosciami macierzy w pliku: %s\n", nazwa_pliku);
                fclose(fin);
                exit(3);
            }
        }
    }

    fclose(fin);
    return m;
}

void zapisz(char *nazwa, macierz m) {
    FILE *fout;
    int i, j;

    fout = fopen(nazwa, "w");
    if (fout == NULL) {
        printf("nie udalo sie utworzyc pliku %s\n", nazwa);
        exit(1);
    }

    fprintf(fout, "%d\n", m.w);
    fprintf(fout, "%d\n", m.k);

    for (i = 0; i < m.w; i++) {
        for (j = 0; j < m.k; j++) {
            fprintf(fout, "%.2f ", m.wartosci[i][j]);
        }
        fprintf(fout, "\n");
    }

    fclose(fout);
}
