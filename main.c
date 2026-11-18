#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "matrix_io.h"

#define DL_NAZWY 256

int main() {
    macierz m1, m2, m3;
    char plik_we_A[DL_NAZWY], plik_we_B[DL_NAZWY], plik_wy[DL_NAZWY];

	printf("podaj plik macierzy A: ");
	if (scanf("%255s", plik_we_A) != 1) {
		fprintf(stderr, "nie podano nazwy pliku wejsciowego\n");
		return 1;
	}
	
	printf("podaj plik macierzy B: ");
	if (scanf("%255s", plik_we_B) != 1) {
		fprintf(stderr, "nie podano nazwy pliku wejsciowego\n");
		return 1;
	}

	printf("podaj plik wyniku: ");
	if (scanf("%255s", plik_wy) != 1) {
		fprintf(stderr, "nie podano nazwy pliku wyjsciowego\n");
		return 1;
	}
	
	m1 = wczytaj(plik_we_A);
	wypisz(m1);
	m2 = wczytaj(plik_we_B);
	wypisz(m2);

    m3 = dodaj(m2, m1);
    printf("wynik dodawania:\n");
    wypisz(m3);
    
    printf("zapisano wynik do pliku: %s\n", plik_wy);
    zapisz(plik_wy, m3);

    return 0;
}
