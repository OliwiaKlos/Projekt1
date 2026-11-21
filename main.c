#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "matrix_io.h"

#define DL_NAZWY 256
#define FOLDER_DANYCH "Dane"

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

/* Pobiera nazwe pliku wejsciowego: numer z listy albo nazwa/sciezka. */
static int pobierz_plik(const char *zacheta, char *sciezka,
                        char lista[][DL_NAZWY_PLIKU], int ile) {
    char token[DL_NAZWY];
    char *koniec;
    long numer;

    if (!pobierz_nazwe(zacheta, token))
        return 0;

    if (token[strspn(token, "0123456789")] == '\0') {
        numer = strtol(token, &koniec, 10);
        if (numer >= 1 && numer <= ile) {
            sprintf(sciezka, "%s/%s", FOLDER_DANYCH, lista[numer - 1]);
            return 1;
        }
    }
    strcpy(sciezka, token);
    return 1;
}

int main(void) {
    macierz a, b, wynik;
    char plik_a[DL_NAZWY], plik_b[DL_NAZWY], plik_wy[DL_NAZWY];
    char lista[MAX_PLIKOW][DL_NAZWY_PLIKU];
    char token_op[32], *koniec;
    long op;
    int kod, ile, i;

    ile = lista_plikow(FOLDER_DANYCH, lista, MAX_PLIKOW);
    if (ile > 0) {
        printf("pliki w folderze %s/ (wpisz numer albo wlasna nazwe/sciezke):\n", FOLDER_DANYCH);
        for (i = 0; i < ile; i++)
            printf("  %2d) %s\n", i + 1, lista[i]);
        printf("\n");
    } else {
        ile = 0;
        printf("(brak plikow .txt w folderze %s/ - podaj nazwe lub sciezke pliku)\n\n", FOLDER_DANYCH);
    }

    if (!pobierz_plik("podaj plik macierzy A: ", plik_a, lista, ile))
        return zglos_blad(ERR_BRAK_NAZWY, "macierz A");
    if (!pobierz_plik("podaj plik macierzy B: ", plik_b, lista, ile))
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
