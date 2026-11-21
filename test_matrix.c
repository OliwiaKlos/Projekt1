/* Testy: biblioteka (matrix.c, matrix_io.c) oraz caly program (kody wyjscia).
   Uruchamianie: ./test_matrix ./matrix   (z katalogu glownego projektu) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "matrix_io.h"

#define DANE "Dane/"
#define OCZEKIWANE "Oczekiwane/"

#ifdef _WIN32
#define WYCISZ ">NUL 2>&1"
#define PLIK_PELNY NULL
#else
#define WYCISZ ">/dev/null 2>&1"
#define PLIK_PELNY "/dev/full"
#endif

/* pliki oczekiwane maja 2 miejsca po kropce, wiec porownujemy z dokladnoscia do 0.005 */
#define TOLERANCJA 0.0050001

#define TMP_WE "test_matrix_in.txt"
#define TMP_WY "test_matrix_out.txt"
#define BRAK_FOLDERU "nie_ma_takiego_folderu/wynik.txt"

static const char *program = "./matrix";
static int razem = 0, ok = 0;

static const char *wyniki[][4] = {
    {"d", "A.txt", "B.txt", OCZEKIWANE "A_plus_B.txt"},
    {"o", "A.txt", "B.txt", OCZEKIWANE "A_minus_B.txt"},
    {"m", "A.txt", "B.txt", OCZEKIWANE "A_razy_B.txt"},
    {"o", "R.txt", "S.txt", OCZEKIWANE "R_minus_S.txt"},      /* prostokatne, ujemne */
    {"m", "P.txt", "Q.txt", OCZEKIWANE "P_razy_Q.txt"},       /* prostokatne */
    {"m", "A.txt", "Z.txt", OCZEKIWANE "A_razy_Z.txt"},       /* zera */
    {"m", "jeden_a.txt", "jeden_b.txt", OCZEKIWANE "jeden_razy.txt"},  /* 1 x 1 */
    {"m", "d20.txt", "d20.txt", OCZEKIWANE "d20_razy_d20.txt"},        /* 20 x 20 */
    {"m", "wiersz20.txt", "kolumna20.txt", OCZEKIWANE "wiersz20_razy_kolumna20.txt"},
};

/* niezgodne wymiary: operacja, A, B (kod wynika z operacji) */
static const char *niezgodne[][3] = {
    {"d", "A.txt", "P.txt"},
    {"o", "A.txt", "P.txt"},
    {"m", "P.txt", "P.txt"},
};

static struct { const char *plik; int kod; } wejscia[] = {
    {"A.txt", OK},
    {"nie_istnieje.txt", ERR_OTWARCIE_PLIKU},
    {"pusty.txt", ERR_NAGLOWEK},
    {"wymiar_ulamkowy.txt", ERR_NAGLOWEK},
    {"zero_wierszy.txt", ERR_WYMIARY_ZAKRES},
    {"za_duzy.txt", ERR_WYMIARY_ZAKRES},
    {"uciety.txt", ERR_BRAK_ELEMENTU},
    {"tekst.txt", ERR_ELEMENT_NIELICZBOWY},
    {"nan.txt", ERR_ELEMENT_NIELICZBOWY},
};

static const char *zapisy[][2] = {
    {"R.txt",            "2 3\n1.00 -2.00 0.00\n3.00 4.00 -5.00\n"},
    {"zaokraglenie.txt", "1 2\n2.35 -2.34\n"},
};

#define LICZBA(tab) ((int)(sizeof(tab) / sizeof((tab)[0])))

static void sprawdz(int warunek, const char *opis) {
    razem++;
    ok += warunek;
    printf("[%s] %s\n", warunek ? " OK " : "FAIL", opis);
}

static void sciezka(char *cel, const char *nazwa) {
    sprintf(cel, "%s%s", DANE, nazwa);
}

static int licz(char op, const macierz *a, const macierz *b, macierz *w) {
    return op == 'd' ? dodaj(a, b, w) : op == 'o' ? odejmij(a, b, w) : mnoz(a, b, w);
}

static int rowne(const macierz *a, const macierz *b) {
    int i, j;
    if (a->w != b->w || a->k != b->k) return 0;
    for (i = 0; i < a->w; i++)
        for (j = 0; j < a->k; j++)
            if (fabs(a->wartosci[i][j] - b->wartosci[i][j]) > TOLERANCJA) return 0;
    return 1;
}

static int plik_istnieje(const char *nazwa) {
    FILE *f = fopen(nazwa, "r");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

static int wczytaj_tekst(const char *nazwa, char *bufor, size_t rozmiar) {
    FILE *f = fopen(nazwa, "r");
    size_t n;
    if (f == NULL) return 0;
    n = fread(bufor, 1, rozmiar - 1, f);
    bufor[n] = '\0';
    fclose(f);
    return 1;
}

static int plik_ma_tekst(const char *nazwa, const char *wzorzec) {
    char bufor[4096];
    return wczytaj_tekst(nazwa, bufor, sizeof(bufor)) && strcmp(bufor, wzorzec) == 0;
}

static int pliki_rowne_tekstowo(const char *a, const char *b) {
    char ta[4096], tb[4096];
    return wczytaj_tekst(a, ta, sizeof(ta)) && wczytaj_tekst(b, tb, sizeof(tb))
        && strcmp(ta, tb) == 0;
}

/* Uruchamia caly program, podajac mu na wejscie podany tekst.
   Zwraca kod wyjscia albo -1, gdy program zakonczyl sie awaria. */
static int uruchom(const char *wejscie) {
    char polecenie[1024];
    FILE *f;
    int status;

    f = fopen(TMP_WE, "w");
    if (f == NULL) return -1;
    fputs(wejscie, f);
    fclose(f);

    sprintf(polecenie, "%s < %s " WYCISZ, program, TMP_WE);
    fflush(stdout);
    status = system(polecenie);
#ifdef _WIN32
    return status;
#else
    if ((status & 0x7F) != 0) return -1;
    return (status >> 8) & 0xFF;
#endif
}

static int uruchom_pliki(const char *a, const char *b, const char *wy, const char *op) {
    char wejscie[768];
    sprintf(wejscie, "%s\n%s\n%s\n%s\n", a, b, wy, op);
    remove(TMP_WY);
    return uruchom(wejscie);
}

static int numer_operacji(char op) {
    return op == 'd' ? 1 : op == 'o' ? 2 : 3;
}

static void testy_biblioteki(void) {
    char opis[256], pa[256], pb[256], pe[256];
    macierz a, b, w, oczek, tmp;
    int i;

    printf("\n--- operacje (biblioteka) ---\n");
    for (i = 0; i < LICZBA(wyniki); i++) {
        sciezka(pa, wyniki[i][1]);
        sciezka(pb, wyniki[i][2]);
        sprintf(pe, "%s", wyniki[i][3]);
        sprintf(opis, "%s %s %s -> %s", wyniki[i][1], wyniki[i][0], wyniki[i][2], wyniki[i][3]);
        sprawdz(wczytaj(pa, &a) == OK && wczytaj(pb, &b) == OK
                && licz(wyniki[i][0][0], &a, &b, &w) == OK
                && wczytaj(pe, &oczek) == OK && rowne(&w, &oczek), opis);
    }

    printf("\n--- niezgodne wymiary (biblioteka) ---\n");
    for (i = 0; i < LICZBA(niezgodne); i++) {
        int oczekiwany = niezgodne[i][0][0] == 'm' ? ERR_WYMIARY_MNOZENIE : ERR_WYMIARY_DODAWANIE;
        sciezka(pa, niezgodne[i][1]);
        sciezka(pb, niezgodne[i][2]);
        wczytaj(pa, &a);
        wczytaj(pb, &b);
        sprintf(opis, "%s %s %s -> kod %d", niezgodne[i][1], niezgodne[i][0], niezgodne[i][2], oczekiwany);
        sprawdz(licz(niezgodne[i][0][0], &a, &b, &w) == oczekiwany, opis);
    }

    printf("\n--- odczyt: kody bledow (biblioteka) ---\n");
    for (i = 0; i < LICZBA(wejscia); i++) {
        sciezka(pa, wejscia[i].plik);
        tmp.w = 77;
        sprintf(opis, "wczytaj(%s) -> kod %d", wejscia[i].plik, wejscia[i].kod);
        sprawdz(wczytaj(pa, &tmp) == wejscia[i].kod
                && (wejscia[i].kod == OK || tmp.w == 77), opis);
    }

    printf("\n--- zapis (biblioteka) ---\n");
    for (i = 0; i < LICZBA(zapisy); i++) {
        sciezka(pa, zapisy[i][0]);
        wczytaj(pa, &a);
        remove(TMP_WY);
        sprintf(opis, "zapisz(%s) -> format pliku", zapisy[i][0]);
        sprawdz(zapisz(TMP_WY, &a) == OK && plik_ma_tekst(TMP_WY, zapisy[i][1]), opis);
    }
    sciezka(pa, "d20.txt");
    wczytaj(pa, &a);
    remove(TMP_WY);
    sprawdz(zapisz(TMP_WY, &a) == OK && wczytaj(TMP_WY, &b) == OK && rowne(&a, &b),
            "zapisz + wczytaj 20 x 20");
    remove(TMP_WY);

    sciezka(pa, "R.txt");
    wczytaj(pa, &a);
    sprawdz(zapisz(BRAK_FOLDERU, &a) == ERR_ZAPIS_OTWARCIE && !plik_istnieje(BRAK_FOLDERU),
            "zapisz do nieistniejacego folderu -> ERR_ZAPIS_OTWARCIE, brak pliku");
#ifndef _WIN32
    sprawdz(zapisz(PLIK_PELNY, &a) == ERR_ZAPIS,
            "zapisz do /dev/full -> ERR_ZAPIS (blad zapisu)");
#endif

    printf("\n--- kody bledow sa unikalne i opisane ---\n");
    {
        int kod, j, unikalne = 1, opisane = 1;
        for (kod = ERR_BRAK_NAZWY; kod <= ERR_ZAPIS; kod++) {
            if (strcmp(komunikat_bledu(kod), "nieznany blad") == 0) opisane = 0;
            for (j = kod + 1; j <= ERR_ZAPIS; j++)
                if (strcmp(komunikat_bledu(kod), komunikat_bledu(j)) == 0) unikalne = 0;
        }
        sprawdz(opisane && unikalne, "kazdy kod bledu ma unikalny komunikat");
    }
}

static void testy_programu(void) {
    char opis[256], pa[256], pb[256], op[2], wejscie[768];
    macierz wynik, oczek;
    int i, kod;

    printf("\n--- caly program: wyniki poprawne ---\n");
    for (i = 0; i < LICZBA(wyniki); i++) {
        sciezka(pa, wyniki[i][1]);
        sciezka(pb, wyniki[i][2]);
        sprintf(op, "%d", numer_operacji(wyniki[i][0][0]));
        kod = uruchom_pliki(pa, pb, TMP_WY, op);
        sprintf(opis, "program: %s %s %s -> kod 0 i poprawny plik", wyniki[i][1], wyniki[i][0], wyniki[i][2]);
        sprawdz(kod == OK
                && wczytaj(TMP_WY, &wynik) == OK
                && wczytaj(wyniki[i][3], &oczek) == OK
                && rowne(&wynik, &oczek)
                && (strncmp(wyniki[i][3], OCZEKIWANE, strlen(OCZEKIWANE)) != 0
                    || pliki_rowne_tekstowo(TMP_WY, wyniki[i][3])), opis);
    }

    printf("\n--- caly program: niezgodne wymiary ---\n");
    for (i = 0; i < LICZBA(niezgodne); i++) {
        int oczekiwany = niezgodne[i][0][0] == 'm' ? ERR_WYMIARY_MNOZENIE : ERR_WYMIARY_DODAWANIE;
        sciezka(pa, niezgodne[i][1]);
        sciezka(pb, niezgodne[i][2]);
        sprintf(op, "%d", numer_operacji(niezgodne[i][0][0]));
        kod = uruchom_pliki(pa, pb, TMP_WY, op);
        sprintf(opis, "program: %s %s %s -> kod %d, brak pliku wyniku",
                niezgodne[i][1], niezgodne[i][0], niezgodne[i][2], oczekiwany);
        sprawdz(kod == oczekiwany && !plik_istnieje(TMP_WY), opis);
    }

    printf("\n--- caly program: bledne pliki wejsciowe ---\n");
    sciezka(pb, "A.txt");
    for (i = 0; i < LICZBA(wejscia); i++) {
        if (wejscia[i].kod == OK) continue;
        sciezka(pa, wejscia[i].plik);
        kod = uruchom_pliki(pa, pb, TMP_WY, "1");
        sprintf(opis, "program: A=%s -> kod %d, brak pliku wyniku", wejscia[i].plik, wejscia[i].kod);
        sprawdz(kod == wejscia[i].kod && !plik_istnieje(TMP_WY), opis);
    }
    sciezka(pa, "nie_istnieje.txt");
    kod = uruchom_pliki(pb, pa, TMP_WY, "1");
    sprawdz(kod == ERR_OTWARCIE_PLIKU && !plik_istnieje(TMP_WY),
            "program: B=nie_istnieje.txt -> kod 2, brak pliku wyniku");

    printf("\n--- caly program: menu i zapis ---\n");
    sciezka(pa, "A.txt");
    sciezka(pb, "B.txt");
    {
        static const char *zle_op[] = {"0", "abc", NULL};
        for (i = 0; zle_op[i] != NULL; i++) {
            kod = uruchom_pliki(pa, pb, TMP_WY, zle_op[i]);
            sprintf(opis, "operacja \"%s\" -> ERR_OPERACJA, brak pliku wyniku", zle_op[i]);
            sprawdz(kod == ERR_OPERACJA && !plik_istnieje(TMP_WY), opis);
        }
    }
    sprawdz(uruchom_pliki(pa, pb, TMP_WY, "") == ERR_OPERACJA,
            "brak numeru operacji -> ERR_OPERACJA");

    sprawdz(uruchom("") == ERR_BRAK_NAZWY, "puste wejscie -> ERR_BRAK_NAZWY");
    sprintf(wejscie, "%s\n", pa);
    sprawdz(uruchom(wejscie) == ERR_BRAK_NAZWY, "brak nazwy B -> ERR_BRAK_NAZWY");

    kod = uruchom_pliki(pa, pb, BRAK_FOLDERU, "1");
    sprawdz(kod == ERR_ZAPIS_OTWARCIE && !plik_istnieje(BRAK_FOLDERU),
            "zapis do nieistniejacego folderu -> ERR_ZAPIS_OTWARCIE");
#ifndef _WIN32
    sprawdz(uruchom_pliki(pa, pb, PLIK_PELNY, "1") == ERR_ZAPIS,
            "zapis do /dev/full -> ERR_ZAPIS");
#endif
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        static char kopia[256];
        sprintf(kopia, "%s", argv[1]);
#ifdef _WIN32
        { char *p; for (p = kopia; *p; p++) if (*p == '/') *p = '\\'; }
#endif
        program = kopia;
    }

    testy_biblioteki();
    testy_programu();

    remove(TMP_WE);
    remove(TMP_WY);
    printf("\nwynik: %d/%d testow zaliczonych\n", ok, razem);
    return ok == razem ? 0 : 1;
}
