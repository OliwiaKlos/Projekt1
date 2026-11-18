#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "matrix_io.h"

typedef macierz (*dzialanie)(macierz, macierz);

/* foldery z plikami (mozna zmienic przy kompilacji, np. -DDANE=\"x/\") */
#ifndef DANE
#define DANE "Dane/"
#endif
#ifndef OCZEKIWANE
#define OCZEKIWANE "Oczekiwane/"
#endif

#ifdef _WIN32
#define WYCISZ ">NUL 2>&1"
#else
#define WYCISZ ">/dev/null 2>&1"
#endif

static const char *program; /* nazwa tego programu (argv[0]) */
static int razem = 0, ok = 0;

static void sprawdz(int warunek, const char *opis) {
    razem++;
    ok += warunek;
    printf("[%s] %s\n", warunek ? " OK " : "FAIL", opis);
}

/* wczytuje plik z folderu DANE */
static macierz plik(const char *nazwa) {
    char sciezka[256];
    snprintf(sciezka, sizeof(sciezka), "%s%s", DANE, nazwa);
    return wczytaj(sciezka);
}

/* wczytuje plik podany z pelna sciezka (np. oczekiwany wynik) */
static macierz z_sciezki(const char *nazwa) {
    return wczytaj((char *)nazwa);
}

static int rowne(macierz a, macierz b) {
    int i, j;
    if (a.w != b.w || a.k != b.k) return 0;
    for (i = 0; i < a.w; i++)
        for (j = 0; j < a.k; j++)
            if (fabs(a.wartosci[i][j] - b.wartosci[i][j]) > 1e-9) return 0;
    return 1;
}

/* Funkcje konczace sie exit() uruchamiamy w osobnym procesie: ten sam program
   wywolany z 3 argumentami (tryb potomny, patrz main) i zwracamy jego kod
   wyjscia. Gdy f == NULL, tylko wczytujemy plik a. */
static int kod_wyjscia(dzialanie f, const char *a, const char *b) {
    char polecenie[512];
    char op = f == dodaj ? 'd' : f == odejmij ? 'o' : f == mnoz ? 'm' : 'w';
    int wynik;

    snprintf(polecenie, sizeof(polecenie), "%s %c %s %s " WYCISZ,
             program, op, a, b ? b : "-");
    fflush(stdout);
    wynik = system(polecenie);
#ifdef _WIN32
    return wynik;
#else
    return (wynik >> 8) & 0xFF;
#endif
}

int main(int argc, char *argv[]) {
    char opis[128];
    int i;

    /* tryb potomny: wykonaj jedna operacje (moze zakonczyc sie przez exit) */
    if (argc == 4) {
        switch (argv[1][0]) {
            case 'd': dodaj(plik(argv[2]), plik(argv[3])); break;
            case 'o': odejmij(plik(argv[2]), plik(argv[3])); break;
            case 'm': mnoz(plik(argv[2]), plik(argv[3])); break;
            default:  plik(argv[2]); break;
        }
        return 0;
    }

    program = argv[0];
#ifdef _WIN32
    {   /* cmd.exe nie lubi '/' w sciezce programu */
        static char kopia[256];
        char *p;
        snprintf(kopia, sizeof(kopia), "%s", argv[0]);
        for (p = kopia; *p; p++)
            if (*p == '/') *p = '\\';
        program = kopia;
    }
#endif

    struct { dzialanie f; const char *a, *b, *oczekiwany; } wyniki[] = {
        {dodaj,  "A.txt", "B.txt", OCZEKIWANE "A_plus_B.txt"},
        {dodaj,  "R.txt", "S.txt", OCZEKIWANE "R_plus_S.txt"},
        {odejmij, "A.txt", "B.txt", OCZEKIWANE "A_minus_B.txt"},
        {odejmij, "R.txt", "S.txt", OCZEKIWANE "R_minus_S.txt"},
        {mnoz,   "A.txt", "B.txt", OCZEKIWANE "A_razy_B.txt"},
        {mnoz,   "P.txt", "Q.txt", OCZEKIWANE "P_razy_Q.txt"},
        {mnoz,   "Q.txt", "P.txt", OCZEKIWANE "Q_razy_P.txt"},
        {mnoz,   "jeden_a.txt", "jeden_b.txt", OCZEKIWANE "jeden_razy.txt"},
        {mnoz,   "A.txt", "I.txt", DANE "A.txt"},
        {mnoz,   "I.txt", "A.txt", DANE "A.txt"},
    };

    struct { dzialanie f; const char *a, *b; } niezgodne[] = {
        {dodaj, "A.txt", "P.txt"}, {odejmij, "A.txt", "P.txt"},
        {mnoz, "P.txt", "P.txt"},  {mnoz, "R.txt", "S.txt"},
    };

    struct { const char *plik; int kod; } wejscia[] = {
        {"A.txt", 0}, {"nadmiar.txt", 0},
        {"nie_istnieje.txt", 1},
        {"pusty.txt", 2}, {"wymiar_ulamkowy.txt", 2},
        {"tekst.txt", 3}, {"uciety.txt", 3}, {"przecinek.txt", 3},
        {"zero_wierszy.txt", 4}, {"za_duzy.txt", 4},
    };

    for (i = 0; i < (int)(sizeof(wyniki) / sizeof(wyniki[0])); i++) {
        macierz w = wyniki[i].f(plik(wyniki[i].a), plik(wyniki[i].b));
        snprintf(opis, sizeof(opis), "%s, %s -> %s",
                 wyniki[i].a, wyniki[i].b, wyniki[i].oczekiwany);
        sprawdz(rowne(w, z_sciezki(wyniki[i].oczekiwany)), opis);
    }

    for (i = 0; i < (int)(sizeof(niezgodne) / sizeof(niezgodne[0])); i++) {
        int kod = kod_wyjscia(niezgodne[i].f, niezgodne[i].a, niezgodne[i].b);
        snprintf(opis, sizeof(opis), "niezgodne wymiary %s, %s -> exit(2)",
                 niezgodne[i].a, niezgodne[i].b);
        sprawdz(kod == 2, opis);
    }

    for (i = 0; i < (int)(sizeof(wejscia) / sizeof(wejscia[0])); i++) {
        int kod = kod_wyjscia(NULL, wejscia[i].plik, NULL);
        snprintf(opis, sizeof(opis), "wczytaj(%s) -> exit(%d)",
                 wejscia[i].plik, wejscia[i].kod);
        sprawdz(kod == wejscia[i].kod, opis);
    }

    zapisz("test_matrix_tmp.txt", plik("R.txt"));
    sprawdz(rowne(z_sciezki("test_matrix_tmp.txt"), plik("R.txt")), "zapisz + wczytaj");
    remove("test_matrix_tmp.txt");

    printf("\nwynik: %d/%d testow zaliczonych\n", ok, razem);
    return ok == razem ? 0 : 1;
}
