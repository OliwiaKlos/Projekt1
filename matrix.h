#ifndef MATRIX_H
#define MATRIX_H

#define MAX 20

/* Kody wyjscia programu. Kazdy blad ma wlasny, unikalny kod. */
enum kody_bledow {
    OK = 0,
    ERR_BRAK_NAZWY = 1,          /* nie podano nazwy pliku (koniec wejscia) */
    ERR_OTWARCIE_PLIKU = 2,      /* brak pliku wejsciowego / brak dostepu */
    ERR_NAGLOWEK = 3,            /* brak lub nieliczbowe wymiary w pliku */
    ERR_WYMIARY_ZAKRES = 4,      /* wymiar spoza zakresu 1-MAX */
    ERR_BRAK_ELEMENTU = 5,       /* za malo elementow w pliku */
    ERR_ELEMENT_NIELICZBOWY = 6, /* element nie jest liczba */
    ERR_WYMIARY_DODAWANIE = 7,   /* A+B i A-B: rozne wymiary */
    ERR_WYMIARY_MNOZENIE = 8,    /* A*B: kolumny A != wiersze B */
    ERR_OPERACJA = 9,            /* nieprawidlowy numer operacji */
    ERR_ZAPIS_OTWARCIE = 10,     /* nie mozna utworzyc pliku wyniku */
    ERR_ZAPIS = 11               /* blad podczas zapisu wyniku */
};

typedef struct macierz_s {
    int w;
    int k;
    double wartosci[MAX][MAX];
} macierz;

/* Funkcje zwracaja OK albo kod bledu; *wynik zmieniaja tylko przy OK. */
int dodaj(const macierz *lewa, const macierz *prawa, macierz *wynik);
int odejmij(const macierz *lewa, const macierz *prawa, macierz *wynik);
int mnoz(const macierz *lewa, const macierz *prawa, macierz *wynik);

/* Czytelny opis kodu bledu. */
const char *komunikat_bledu(int kod);

#endif
