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
#else
#define WYCISZ ">/dev/null 2>&1"
#endif

#define TMP "test_matrix_tmp.txt"
#define BRAK_FOLDERU "nie_ma_takiego_folderu/wynik.txt"

static const char *program; 
static int razem = 0, ok = 0;

static const char *wyniki[][4] = {
    {"d", "A.txt", "B.txt", OCZEKIWANE "A_plus_B.txt"},
    {"o", "A.txt", "B.txt", OCZEKIWANE "A_minus_B.txt"},
    {"m", "A.txt", "B.txt", OCZEKIWANE "A_razy_B.txt"},
    {"d", "R.txt", "S.txt", OCZEKIWANE "R_plus_S.txt"},
    {"o", "R.txt", "S.txt", OCZEKIWANE "R_minus_S.txt"},
    {"m", "P.txt", "Q.txt", OCZEKIWANE "P_razy_Q.txt"},
    {"m", "Q.txt", "P.txt", OCZEKIWANE "Q_razy_P.txt"},
    {"m", "A.txt", "I.txt", DANE "A.txt"},
    {"m", "I.txt", "A.txt", DANE "A.txt"},
    {"o", "A.txt", "A.txt", OCZEKIWANE "zero_2x2.txt"},
    {"m", "A.txt", "Z.txt", OCZEKIWANE "A_razy_Z.txt"},
    {"d", "A.txt", "Z.txt", DANE "A.txt"},
    {"d", "jeden_a.txt", "jeden_b.txt", OCZEKIWANE "jeden_plus.txt"},
    {"o", "jeden_a.txt", "jeden_b.txt", OCZEKIWANE "jeden_minus.txt"},
    {"m", "jeden_a.txt", "jeden_b.txt", OCZEKIWANE "jeden_razy.txt"},
    {"d", "ulamki_a.txt", "ulamki_b.txt", OCZEKIWANE "ulamki_plus.txt"},
    {"o", "ulamki_a.txt", "ulamki_b.txt", OCZEKIWANE "ulamki_minus.txt"},
    {"m", "ulamki_a.txt", "ulamki_b.txt", OCZEKIWANE "ulamki_razy.txt"},
    {"d", "d20.txt", "d20.txt", OCZEKIWANE "d20_plus_d20.txt"},
    {"o", "d20.txt", "I20.txt", OCZEKIWANE "d20_minus_I20.txt"},
    {"m", "d20.txt", "d20.txt", OCZEKIWANE "d20_razy_d20.txt"},
    {"m", "d20.txt", "I20.txt", DANE "d20.txt"},
    {"m", "wiersz20.txt", "kolumna20.txt", OCZEKIWANE "wiersz20_razy_kolumna20.txt"},
    {"m", "kolumna20.txt", "wiersz20.txt", OCZEKIWANE "kolumna20_razy_wiersz20.txt"},
};

static const char *niezgodne[][3] = {
    {"d", "A.txt", "P.txt"},
    {"o", "A.txt", "P.txt"},
    {"d", "R.txt", "Q.txt"},
    {"m", "P.txt", "P.txt"},
    {"m", "R.txt", "S.txt"},
    {"m", "wiersz20.txt", "wiersz20.txt"},
    {"d", "wiersz20.txt", "kolumna20.txt"},
};

static const char *kod0[] = {"A.txt", "nadmiar.txt", "wymiary_w_osobnych_liniach.txt", "d20.txt", "wiersz20.txt", "kolumna20.txt", NULL};
static const char *kod1[] = {"nie_istnieje.txt", NULL};
static const char *kod2[] = {"pusty.txt", "wymiar_ulamkowy.txt", NULL};
static const char *kod3[] = {"tekst.txt", "uciety.txt", "przecinek.txt", NULL};
static const char *kod4[] = {"zero_wierszy.txt", "zero_kolumn.txt", "ujemny_wymiar.txt",
                             "za_duzy.txt", "za_duzo_kolumn.txt", NULL};

static const char *zapisy[][2] = {
    {"R.txt",            "2 3\n1.00 -2.00 0.00\n3.00 4.00 -5.00\n"},
    {"jeden_b.txt",      "1 1\n5.00\n"},
    {"ulamki_a.txt",     "2 2\n0.10 0.20\n1.50 -2.25\n"},
    {"zaokraglenie.txt", "1 2\n2.35 -2.34\n"},
};

static void sprawdz(int warunek, const char *opis) {
    razem++;
    ok += warunek;
    printf("[%s] %s\n", warunek ? " OK " : "FAIL", opis);
}

static macierz plik(const char *nazwa) {
    char sciezka[256];
    sprintf(sciezka, "%s%s", DANE, nazwa);
    return wczytaj(sciezka);
}

static macierz licz(char op, macierz a, macierz b) {
    return op == 'd' ? dodaj(a, b) : op == 'o' ? odejmij(a, b) : mnoz(a, b);
}

static int rowne(macierz a, macierz b) {
    int i, j;
    if (a.w != b.w || a.k != b.k) return 0;
    for (i = 0; i < a.w; i++)
        for (j = 0; j < a.k; j++)
            if (fabs(a.wartosci[i][j] - b.wartosci[i][j]) > 1e-9) return 0;
    return 1;
}

static int tekst_rowny(const char *nazwa, const char *wzorzec) {
    char bufor[4096];
    FILE *f = fopen(nazwa, "r");
    size_t n;

    if (f == NULL) return 0;
    n = fread(bufor, 1, sizeof(bufor) - 1, f);
    bufor[n] = '\0';
    fclose(f);
    return strcmp(bufor, wzorzec) == 0;
}

static int kod_wyjscia(char op, const char *a, const char *b) {
    char polecenie[512];
    int wynik;

    sprintf(polecenie, "%s %c %s %s " WYCISZ, program, op, a, b ? b : "-");
    fflush(stdout);
    wynik = system(polecenie);
#ifdef _WIN32
    return wynik;
#else
    return (wynik >> 8) & 0xFF;
#endif
}

static void sprawdz_wejscia(const char *pliki[], int kod) {
    char opis[128];
    int i;

    for (i = 0; pliki[i] != NULL; i++) {
        sprintf(opis, "wczytaj(%s) -> exit(%d)", pliki[i], kod);
        sprawdz(kod_wyjscia('w', pliki[i], NULL) == kod, opis);
    }
}

#define LICZBA(tab) ((int)(sizeof(tab) / sizeof((tab)[0])))
int main(int argc, char *argv[]) {
    char opis[256];
    int i;

    if (argc == 4) {
        char op = argv[1][0];
        if (op == 'z') zapisz(argv[3], plik(argv[2]));
        else if (op == 'w') plik(argv[2]);
        else licz(op, plik(argv[2]), plik(argv[3]));
        return 0;
    }

    program = argv[0];
#ifdef _WIN32
    { 
        static char kopia[256];
        char *p;
        sprintf(kopia, "%s", argv[0]);
        for (p = kopia; *p; p++)
            if (*p == '/') *p = '\\';
        program = kopia;
    }
#endif

    for (i = 0; i < LICZBA(wyniki); i++) {
        macierz w = licz(wyniki[i][0][0], plik(wyniki[i][1]), plik(wyniki[i][2]));
        sprintf(opis, "%s %s %s -> %s",
                wyniki[i][1], wyniki[i][0], wyniki[i][2], wyniki[i][3]);
        sprawdz(rowne(w, wczytaj((char *)wyniki[i][3])), opis);
    }

    for (i = 0; i < LICZBA(niezgodne); i++) {
        sprintf(opis, "niezgodne wymiary: %s %s %s -> exit(2)",
                niezgodne[i][1], niezgodne[i][0], niezgodne[i][2]);
        sprawdz(kod_wyjscia(niezgodne[i][0][0], niezgodne[i][1], niezgodne[i][2]) == 2, opis);
    }

    sprawdz_wejscia(kod0, 0);
    sprawdz_wejscia(kod1, 1);
    sprawdz_wejscia(kod2, 2);
    sprawdz_wejscia(kod3, 3);
    sprawdz_wejscia(kod4, 4);

    for (i = 0; i < LICZBA(zapisy); i++) {
        zapisz(TMP, plik(zapisy[i][0]));
        sprintf(opis, "zapisz(%s) -> format pliku", zapisy[i][0]);
        sprawdz(tekst_rowny(TMP, zapisy[i][1]), opis);
    }

    zapisz(TMP, plik("d20.txt"));
    sprawdz(rowne(wczytaj(TMP), plik("d20.txt")), "zapisz + wczytaj 20 x 20");
    remove(TMP);

    sprawdz(kod_wyjscia('z', "R.txt", BRAK_FOLDERU) == 1,
            "zapisz do nieistniejacego folderu -> exit(1)");
    sprawdz(fopen(BRAK_FOLDERU, "r") == NULL, "po bledzie zapisu nie ma pliku wynikowego");

    printf("\nwynik: %d/%d testow zaliczonych\n", ok, razem);
    return ok == razem ? 0 : 1;
}
