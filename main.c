#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "matrix_io.h"

#define DL_NAZWY 256

int main() {
	int op = 0;
    macierz m1, m2, m3;
    char plik_we_A[DL_NAZWY], plik_we_B[DL_NAZWY], plik_wy[DL_NAZWY];

	printf("podaj plik macierzy A: ");
	if (scanf("%255s", plik_we_A) != 1) {
		fprintf(stderr, "blad. nie podano nazwy pliku macierzy\n");
		return 1;
	}
	
	printf("podaj plik macierzy B: ");
	if (scanf("%255s", plik_we_B) != 1) {
		fprintf(stderr, "blad. nie podano nazwy pliku macierzy\n");
		return 1;
	}

	printf("podaj plik wyniku: ");
	if (scanf("%255s", plik_wy) != 1) {
		fprintf(stderr, "blad. nie podano nazwy pliku wyjsciowego\n");
		return 1;
	}
	
	m1 = wczytaj(plik_we_A);
	wypisz(m1);
	m2 = wczytaj(plik_we_B);
	wypisz(m2);
	
	printf("podaj numer operacji(1- dodawanie, 2- odejmowanie, 3- mnozenie): ");
	if (scanf("%d", &op) != 1) op = 0;
	printf("\n");
	
	switch(op){
		case 1:
			m3 = dodaj(m1, m2);
    		printf("wynik dodawania:\n");
    		wypisz(m3);
    		break;
    	case 2:
    		m3 = odejmij(m1, m2);
    		printf("wynik odejmowania:\n");
    		wypisz(m3);
    		break;
    	case 3:
    		m3 = mnoz(m1, m2);
    		printf("wynik mnozenia:\n");
    		wypisz(m3);
    		break;
    	default:
    		printf("blad. nieprawidlowy numer operacji\n");
    		return 1;
	}
	
    printf("zapisano wynik do pliku: %s\n", plik_wy);
    zapisz(plik_wy, m3);

    return 0;
}
