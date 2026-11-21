#include "matrix.h"

int dodaj(const macierz *lewa, const macierz *prawa, macierz *wynik) {
    int i, j;

    if (lewa->w != prawa->w || lewa->k != prawa->k)
        return ERR_WYMIARY_DODAWANIE;

    wynik->w = lewa->w;
    wynik->k = lewa->k;
    for (i = 0; i < lewa->w; i++)
        for (j = 0; j < lewa->k; j++)
            wynik->wartosci[i][j] = lewa->wartosci[i][j] + prawa->wartosci[i][j];

    return OK;
}

int odejmij(const macierz *lewa, const macierz *prawa, macierz *wynik) {
    int i, j;

    if (lewa->w != prawa->w || lewa->k != prawa->k)
        return ERR_WYMIARY_DODAWANIE;

    wynik->w = lewa->w;
    wynik->k = lewa->k;
    for (i = 0; i < lewa->w; i++)
        for (j = 0; j < lewa->k; j++)
            wynik->wartosci[i][j] = lewa->wartosci[i][j] - prawa->wartosci[i][j];

    return OK;
}

int mnoz(const macierz *lewa, const macierz *prawa, macierz *wynik) {
    int i, j, k;

    if (lewa->k != prawa->w)
        return ERR_WYMIARY_MNOZENIE;

    wynik->w = lewa->w;
    wynik->k = prawa->k;
    for (i = 0; i < lewa->w; i++) {
        for (j = 0; j < prawa->k; j++) {
            double suma = 0.0;
            for (k = 0; k < lewa->k; k++)
                suma += lewa->wartosci[i][k] * prawa->wartosci[k][j];
            wynik->wartosci[i][j] = suma;
        }
    }

    return OK;
}

const char *komunikat_bledu(int kod) {
    switch (kod) {
    case OK:                      return "brak bledu";
    case ERR_BRAK_NAZWY:          return "nie podano nazwy pliku";
    case ERR_OTWARCIE_PLIKU:      return "nie udalo sie otworzyc pliku do odczytu";
    case ERR_NAGLOWEK:            return "bledny naglowek: oczekiwano dwoch calkowitych wymiarow (wiersze kolumny)";
    case ERR_WYMIARY_ZAKRES:      return "wymiary macierzy musza byc z zakresu 1-20";
    case ERR_BRAK_ELEMENTU:       return "brakujacy element macierzy (za malo danych w pliku)";
    case ERR_ELEMENT_NIELICZBOWY: return "element macierzy nie jest poprawna liczba";
    case ERR_WYMIARY_DODAWANIE:   return "niezgodne wymiary: dodawanie i odejmowanie wymagaja takich samych wymiarow";
    case ERR_WYMIARY_MNOZENIE:    return "niezgodne wymiary: liczba kolumn A musi byc rowna liczbie wierszy B";
    case ERR_OPERACJA:            return "nieprawidlowy numer operacji (dozwolone 1, 2, 3)";
    case ERR_ZAPIS_OTWARCIE:      return "nie udalo sie utworzyc pliku wyniku";
    case ERR_ZAPIS:               return "blad podczas zapisu pliku wyniku";
    default:                      return "nieznany blad";
    }
}
