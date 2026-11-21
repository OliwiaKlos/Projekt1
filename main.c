#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "matrix_io.h"

#define DL_NAZWY 256

static int zglos_blad(int kod, const char *szczegol) {
    fprintf(stderr, "blad (kod %d): %s", kod, komunikat_bledu(kod));
    if (szczegol != NULL)
        fprintf(stderr, " [%s]", szczegol);
    fprintf(stderr, "\n");
    return kod;
}

static int pobierz_nazwe(const char *zacheta, char *nazwa) {
    printf("%s", zacheta);
    return scanf("%255s", nazwa) == 1;
}

int main(void) {
    macierz a, b, wynik;
    char plik_a[DL_NAZWY], plik_b[DL_NAZWY], plik_wy[DL_NAZWY];
    char token_op[32], *koniec;
    long op;
    int kod;

    if (!pobierz_nazwe("podaj plik macierzy A: ", plik_a))
        return zglos_blad(ERR_BRAK_NAZWY, "macierz A");
    if (!pobierz_nazwe("podaj plik macierzy B: ", plik_b))
        return zglos_blad(ERR_BRAK_NAZWY, "macierz B");
    if (!pobierz_nazwe("podaj plik wyniku: ", plik_wy))
        return zglos_blad(ERR_BRAK_NAZWY, "plik wyniku");

    kod = wczytaj(plik_a, &a);
    if (kod != OK)
        return zglos_blad(kod, plik_a);
    wypisz(&a);

    kod = wczytaj(plik_b, &b);
    if (kod != OK)
        return zglos_blad(kod, plik_b);
    wypisz(&b);

    printf("podaj numer operacji (1 - dodawanie, 2 - odejmowanie, 3 - mnozenie): ");
    if (scanf("%31s", token_op) != 1)
        return zglos_blad(ERR_OPERACJA, "nie podano numeru");
    op = strtol(token_op, &koniec, 10);
    if (*koniec != '\0')
        return zglos_blad(ERR_OPERACJA, "to nie jest liczba calkowita");
    printf("\n");

    switch (op) {
    case 1:
        kod = dodaj(&a, &b, &wynik);
        break;
    case 2:
        kod = odejmij(&a, &b, &wynik);
        break;
    case 3:
        kod = mnoz(&a, &b, &wynik);
        break;
    default:
        return zglos_blad(ERR_OPERACJA, "podano inna liczbe");
    }
    if (kod != OK)
        return zglos_blad(kod, NULL);

    printf("wynik:\n");
    wypisz(&wynik);

    kod = zapisz(plik_wy, &wynik);
    if (kod != OK)
        return zglos_blad(kod, plik_wy);

    printf("zapisano wynik do pliku: %s\n", plik_wy);
    return OK;
}
