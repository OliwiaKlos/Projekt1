#ifndef MATRIX_H
#define MATRIX_H

#define MAX 20

enum kody_bledow {
    OK = 0,
    ERR_BRAK_NAZWY = 1,
    ERR_OTWARCIE_PLIKU = 2,
    ERR_NAGLOWEK = 3,
    ERR_WYMIARY_ZAKRES = 4,
    ERR_BRAK_ELEMENTU = 5,
    ERR_ELEMENT_NIELICZBOWY = 6,
    ERR_WYMIARY_DODAWANIE = 7,
    ERR_WYMIARY_MNOZENIE = 8,
    ERR_OPERACJA = 9,
    ERR_ZAPIS_OTWARCIE = 10,
    ERR_ZAPIS = 11
};

typedef struct macierz_s {
    int w;
    int k;
    double wartosci[MAX][MAX];
} macierz;

int dodaj(const macierz *lewa, const macierz *prawa, macierz *wynik);
int odejmij(const macierz *lewa, const macierz *prawa, macierz *wynik);
int mnoz(const macierz *lewa, const macierz *prawa, macierz *wynik);

const char *komunikat_bledu(int kod);

#endif
