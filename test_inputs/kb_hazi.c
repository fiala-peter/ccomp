#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* A <math.h> ceil() és floor() függvényeit a fordító valamiért nem ismerte fel, és a kódon belül ezt nem tudtam megoldani, csak terminálból -
ezért inkább megírtam őket magamnak (így a math.h-ra már nincs is szükség), hogy tudjak debuggolni rendesen.*/

int lekerekit(double a)
{
    int b = a;
    if (b <= a)
        return b;
    else
        return b - 1;
}
int felkerekit(double a)
{
    int b = a;
    if (b >= a)
        return b;
    else
        return b + 1;
}

typedef enum homogen_hallgatoi_csoport
{
    A_UZIN = 0, // alap, üzemmérnök-informatikus
    A_INFO = 1, // alap, mérnökinformatikus
    A_VILL = 2, // alap, villamosmérnök
    M_INFO = 3, // mester, mérnökinformatikus
    M_VILL = 4, // mester, villamosmérnök
    M_EUGY = 5, // mester, egészségügyi mérnök
    M_GAZD = 6  // mester, gazdaságinformatikus
} hcsop;

typedef struct diak
{
    char neptun[7];
    char nev[40];
    char koltseg; // A: állami ösztöndíjas vagy K: önköltséges
    char kepzes;  // A: alap, M: mester
    char szak[30];
    int felev;
    int aktiv; // 1: aktív félév, 0: passzív félév
    double kreditindex;
    hcsop csoport;
} diak;

typedef struct neptun_atlag
{
    char neptun[7];
    double kreditindex;
} neptun_atlag;

typedef struct listaelem
{
    diak adat;
    struct listaelem *kov;
} listaelem;

typedef struct listaelem2
{
    char nev[30];
    double kreditindex;
    int osztondij;
    struct listaelem2 *kov;
} listaelem2;

int beolvas(FILE *f, listaelem *kezd) // beolvassa az adatokat a láncolt listába, és visszatér azzal, hogy hány sort olvasott be sikeresen
{
    int db = 0, i;
    listaelem *akt = kezd; // lépkedő pointer a listánkhoz
    diak d;
    *(d.neptun + 6) = '\0'; // lezáró nulla a neptun-kód végére
    for (i = 0; i < 30; i++)
    {
        *(d.nev + i) = '\0'; // lenullázzuk a név mezőt
    }

    while (fscanf(f, "%[^,],%[^,],%c,%c,%[^,],%d,%d\n", d.neptun, d.nev, &d.koltseg, &d.kepzes, d.szak, &d.felev, &d.aktiv) >= 7) // nem tudjuk a név hosszát
    {
        listaelem *uj = (listaelem *)malloc(sizeof(listaelem));
        listaelem *temp;

        strcpy(uj->adat.neptun, d.neptun);
        strcpy(uj->adat.nev, d.nev);
        uj->adat.koltseg = d.koltseg;
        uj->adat.kepzes = d.kepzes;
        strcpy(uj->adat.szak, d.szak);
        uj->adat.felev = d.felev;
        uj->adat.aktiv = d.aktiv;

        temp = akt->kov;
        akt->kov = uj;
        uj->kov = temp;
        akt = akt->kov; // a lista végére másoljuk mindig az új adatokat

        db++;
    }
    return db;
}

void meghat_hcsop(listaelem *kezd, int csop_tagszam[]) // homogén hallgatói csoport meghatározása
{
    listaelem *akt;          // lépkedő pointer
    akt = kezd->kov;         // 1. elemre mutat
    while (akt->kov != NULL) // amíg nem a lezáró strázsára mutat
    {
        /*meghatározza a megfelelő csoportot,
        beírja a diák struktúra megfelelő mezőjébe az ahhoz tartozó hcsop típust,
        és a csop_tagszám tömb megfelelő indexű elemét 1-el növeli, így számolva, hogy melyik csoportban hányan vannak -
        a megfelelő indexet tudjuk onnan, hogy az enum típus milyen intként van tárolva*/
        if (akt->adat.kepzes == 'A')
        {
            if (strcmp(akt->adat.szak, "villamosmérnök") == 0)
            {
                akt->adat.csoport = A_VILL;
                csop_tagszam[A_VILL] = csop_tagszam[A_VILL] + 1;
            }
            if (strcmp(akt->adat.szak, "mérnökinformatikus") == 0)
            {
                akt->adat.csoport = A_INFO;
                csop_tagszam[A_INFO] = csop_tagszam[A_INFO] + 1;
            }
            if (strcmp(akt->adat.szak, "üzemmérnök-informatikus") == 0)
            {
                akt->adat.csoport = A_UZIN;
                csop_tagszam[A_UZIN] = csop_tagszam[A_UZIN] + 1;
            }
        }
        else if (akt->adat.kepzes == 'M')
        {
            if (strcmp(akt->adat.szak, "villamosmérnök") == 0)
            {
                akt->adat.csoport = M_VILL;
                csop_tagszam[M_VILL] = csop_tagszam[M_VILL] + 1;
            }
            else if (strcmp(akt->adat.szak, "mérnökinformatikus") == 0)
            {
                akt->adat.csoport = M_INFO;
                csop_tagszam[M_INFO] = csop_tagszam[M_INFO] + 1;
            }
            else if (strcmp(akt->adat.szak, "egészségügyi mérnök") == 0)
            {
                akt->adat.csoport = M_EUGY;
                csop_tagszam[M_EUGY] = csop_tagszam[M_EUGY] + 1;
            }
            else if (strcmp(akt->adat.szak, "gazdaságinformatikus") == 0)
            {
                akt->adat.csoport = M_GAZD;
                csop_tagszam[M_GAZD] = csop_tagszam[M_GAZD] + 1;
            }
        }
        akt = akt->kov;
    }
}

void kiszam_krediti(FILE *f, neptun_atlag kreditindexek[], int n, int *tdb) // feltölti a kreditindexek tömböt, tdb-ba beírja, hogy hány sort olvasott be
{
    /*a neptun kódot beírja a tömbbe, majd kiszámolja a kreditindexet, és azt is beírja a tömbbe*/
    /* TVSz: "Az ösztöndíj megállapítására szolgáló ösztöndíjindex számítási módja: a félévben teljesített tárgyak
    érdemjegyének és kreditpontjának szorzatából képzett összeget [...] 30 kreditponttal kell osztani."*/
    int kiszam_db = 0, i;
    double kr, jegy;

    for (i = 0; i < n && fscanf(f, "%[^,]", kreditindexek[i].neptun) != 0 && *(kreditindexek[i].neptun) != '*'; i++)
    {
        double osztando = 0.0;
        char vegjel_e;

        *(kreditindexek[i].neptun + 6) = '\0'; // lezáró nulla a neptun-kód végére
        while (fscanf(f, ",%lf,%lf", &jegy, &kr) == 2)
        {
            if (jegy != 1)
                osztando += kr * jegy;
        }
        kreditindexek[i].kreditindex = osztando / 30.0;
        kiszam_db++;
        fscanf(f, "%c", &vegjel_e);
        if (vegjel_e == '*')
            break;
    }
    *tdb = kiszam_db;
}

void krediti_listaba(listaelem *kezd, neptun_atlag kreditindexek[], int n) // a listába írja a kreditindexeket a tömbből
{
    /*beírja a megfelelő neptun-kódú diák struktúra kreditindex mezőjébe a hozzá tartozó kreditindexet*/
    listaelem *akt;
    for (akt = kezd->kov; akt->kov != NULL; akt = akt->kov)
    {
        int i;
        for (i = 0; i < n; i++)
        {
            if (strcmp(kreditindexek[i].neptun, akt->adat.neptun) == 0)
            {
                akt->adat.kreditindex = kreditindexek[i].kreditindex;
                break;
            }
        }
    }
}

void szures(listaelem *kezd) // kiláncolja, aki biztosan nem kap ösztöndíjat
{
    /*kiszűri azokat, akik biztos nem kaphatnak ösztöndíjat - őket kiláncolja a listából, és felszabadítja azt a memóriaterületet*/
    listaelem *akt;    
    for (akt = kezd; akt != NULL && akt->kov != NULL && akt->kov->kov != NULL; akt = akt->kov)
    {
        int megnelepjunk = 1; // ez 1 lesz, ha a következő elemet vizsgálni kell, vagyis még ne léptessük az aktuális (akt) pointert
        while (akt->kov->kov != NULL && megnelepjunk == 1)
        {
            megnelepjunk = 0;
            if ((akt->kov->adat.felev == 1 && akt->kov->adat.kepzes == 'A') // elsőéves hallgató
                || (akt->kov->adat.kreditindex <= 3.0)                      // túl alacsony kreditindex
                || (akt->kov->adat.aktiv == 0)                              // passzív félév
                || (akt->kov->adat.koltseg == 'K')                          //önköltséges
                )                                                           // ha a következő diák ezek miatt nem kaphat ösztöndíjat
            {
                listaelem *torlendo = akt->kov;
                akt->kov = akt->kov->kov;
                free(torlendo);
                megnelepjunk = 1;
            }
        }
    }
}

void szur_legjobb_hhcsop(listaelem *kezd, int csop_tagszam[]) // szur_legjobb, de hhcsoportonként
{
    int j;
    for (j = 0; j < 7; j++)
    {
        int i;
        listaelem *p;
        int diakok = 0;
        double *legrosszabb_kredit, max;

        for (p = kezd->kov; p->kov != NULL; p = p->kov)
            if (p->adat.csoport == j)
                diakok++;

        if (csop_tagszam[j] >= diakok)
            continue;

        p = kezd->kov;
        legrosszabb_kredit = (double *)malloc((diakok - csop_tagszam[j]) * sizeof(double)); // ahány főt ki kell láncolni
        for (i = 0; p->kov != NULL && i < (diakok - csop_tagszam[j]); p = p->kov)
        {
            if (p->adat.csoport == j)
            {
                legrosszabb_kredit[i] = p->adat.kreditindex;
                i++;
            }
        }
        max = -1; // ennél biztosan csak nagyobb kreditindexek lesznek
        for (i = 0; i < (diakok - csop_tagszam[j]); i++)
            if (legrosszabb_kredit[i] > max)
                max = legrosszabb_kredit[i];
        while (p->kov != NULL)
        {
            if (p->adat.csoport == j && p->adat.kreditindex < max)
            {
                for (i = 0; i < (diakok - csop_tagszam[j]); i++)
                {
                    if (legrosszabb_kredit[i] == max)
                    {
                        legrosszabb_kredit[i] = p->adat.kreditindex;
                        max = legrosszabb_kredit[i];
                        break;
                    }
                }
                for (i = 0; i < (diakok - csop_tagszam[j]); i++)
                    if (legrosszabb_kredit[i] > max)
                        max = legrosszabb_kredit[i];
            }
            p = p->kov;
        }
        // létrejött egy tömbünk a legrosszabb kreditindexekkel, ezeket kiláncoljuk
        p = kezd;

        while (p->kov->kov != NULL)
        {
            listaelem *temp;
            int kell_leptetni = 1;
            for (i = 0; i < (diakok - csop_tagszam[j]); i++)
            {
                if (p->kov->adat.csoport == j && p->kov->adat.kreditindex == legrosszabb_kredit[i])
                {
                    kell_leptetni = 0;
                    *(legrosszabb_kredit + i) = -1;
                    temp = p->kov->kov;
                    free(p->kov);
                    p->kov = temp;
                    break;
                }
            }
            if (kell_leptetni)
                p = p->kov;
        }
        free(legrosszabb_kredit);
    }
}

void adatbekero(double *e, int *a, int *f) // bekéri és paraméterlistán visszaadja a-t, f-et, e-t: a minimális és maximális ösztöndíjat, ill. hogy a diákok hányszorosa kaphat max.
{
    double e_be, a_be, f_be;

    printf("Kérem, adja meg, hogy maximum a hallgatók számának hányszorosa kapjon ösztöndíjat: ");
    scanf("%lf", &e_be);
    while (e_be < 0 || e_be > 1)
    {
        printf("Helytelen értéket adott meg. Kérem, adja meg a 0 és 1 közötti számot: ");
        scanf("%lf", &e_be);
    }
    printf("Kérem, adja meg az öszöndíj minimális és maximális értékét: ");
    scanf("%lf %lf", &a_be, &f_be);
    while (a_be < 100 || f_be < a_be || felkerekit(a_be / 100) > lekerekit(f_be / 100))
    {
        printf("Helytelen értékeket adott meg. Kérem, adjon meg két pozitív számot, először az ösztöndíj\nminimális értékét, majd szóközzel elválasztva maximális értékét (a minimális érték legyen legalább 100Ft): ");
        scanf("%lf %lf", &a_be, &f_be);
    }
    *e = e_be;
    *a = felkerekit(a_be / 100) * 100;
    *f = lekerekit(f_be / 100) * 100;
}

void szur_legjobb(listaelem *kezd, int n) // kiszűri a legjobb kreditindexű n főt
{
    int i;
    listaelem *p;
    int diakok = 0;
    double *legrosszabb_kredit, max;

    for (p = kezd->kov; p->kov != NULL; p = p->kov)
        diakok++;

    if (n >= diakok)
        return;

    p = kezd->kov;
    legrosszabb_kredit = (double *)malloc((diakok - n) * sizeof(double)); // ahány főt ki kell láncolni
    for (i = 0; p->kov != NULL && i < (diakok - n); i++)
    {
        legrosszabb_kredit[i] = p->adat.kreditindex;
        p = p->kov;
    }
    max = -1; // ennél biztosan csak nagyobb kreditindexek lesznek
    for (i = 0; i < (diakok - n); i++)
        if (legrosszabb_kredit[i] > max)
            max = legrosszabb_kredit[i];
    while (p->kov != NULL)
    {
        if (p->adat.kreditindex < max)
        {
            for (i = 0; i < (diakok - n); i++)
            {
                if (legrosszabb_kredit[i] == max)
                {
                    legrosszabb_kredit[i] = p->adat.kreditindex;
                    max = legrosszabb_kredit[i];
                    break;
                }
            }
            for (i = 0; i < (diakok - n); i++)
                if (legrosszabb_kredit[i] > max)
                    max = legrosszabb_kredit[i];
        }
        p = p->kov;
    }
    // létrejött egy tömbünk a legrosszabb kreditindexekkel, az ebben lévőket most kiláncoljuk
    p = kezd;

    while (p->kov->kov != NULL)
    {
        listaelem *rossz;
        int kell_leptetni = 1;
        for (i = 0; i < (diakok - n); i++)
        {
            if (p->kov->adat.kreditindex == legrosszabb_kredit[i])
            {
                kell_leptetni = 0;
                *(legrosszabb_kredit + i) = -1;
                rossz = p->kov->kov;
                free(p->kov); //!!!
                p->kov = rossz;
                break;
            }
        }
        if (kell_leptetni)
            p = p->kov;
    }
    free(legrosszabb_kredit);
}

void rendezve_masol(listaelem *kezd, listaelem2 *kezd2, int csop_tagszam[]) // 1.lista elemeinek releváns információi a 2. listába, rendezve
{
    int i;
    listaelem *lep;
    /*hhcsop és kreditindex szerint rendezve bemásolja az 1. láncolt lista elemeit a 2.-ba, közben csop_tagszam-ba másolva,
    hogy melyik hhcsopnak hány diákja került a második listába*/

    // csop_tagszam lenullázása
    for (i = 0; i < 7; i++)
        csop_tagszam[i] = 0;

    lep = kezd->kov;
    while (lep->kov != NULL) // amíg nem a lezáró strázsára mutat
    {
        listaelem2 *lep2;
        listaelem2 *uj = (listaelem2 *)malloc(sizeof(listaelem2));
        listaelem2 *temp;
        hcsop akthcsop;        
        int eltolni_lep2t, lepesek;

        strcpy(uj->nev, lep->adat.nev);
        uj->kreditindex = lep->adat.kreditindex;
        uj->osztondij = 0;

        // beláncolás
        lep2 = kezd2;

        akthcsop = lep->adat.csoport;
        eltolni_lep2t = 0;
        for (i = 0; i < akthcsop; i++)
            eltolni_lep2t += csop_tagszam[i]; //összeadja a saját hcsopja előtti elemek számát
        for (i = 0; i < eltolni_lep2t; i++)
            lep2 = lep2->kov;
        lepesek = 0;
        for (; lepesek < csop_tagszam[akthcsop] && lep2->kov->kreditindex < uj->kreditindex; lep2 = lep2->kov)
            lepesek++;
        temp = lep2->kov;
        lep2->kov = uj;
        uj->kov = temp;
        *(csop_tagszam + akthcsop) += 1; // csop_tagszamban növeltük a megfelelő értéket

        lep = lep->kov;
    }
}

void osztondij_oszto(listaelem2 *kezd2, int csop_tagszam[], int a_i, int f_i) // hhcsopinként kiszámítjuk minden diáknak az ösztöndíjat
{
    double a = (double)a_i;
    double f = (double)f_i;
    double c = (1 - 0.05 / 1.95) * f + (0.05 / 1.95) * a; // a töréspont: ennyi Ft-tól kezdve feleakkora meredekséggel nő az ösztöndíj
    int i;

    listaelem2 *p = kezd2->kov;

    double szelsoertekek[7][2]; // ebben tároljuk a hhcsoportokon belül a legjobb és a leggyengébb kreditindexeket a 2-es listából
    for (i = 0; p != NULL && p->kov != NULL && i < 7; i++)
    {
        if (csop_tagszam[i] != 0)
        {
            int j;
            szelsoertekek[i][0] = p->kreditindex; // leggyengébb
            for (j = 1; j < csop_tagszam[i]; j++)
                p = p->kov;
            szelsoertekek[i][1] = p->kreditindex; // legjobb
            p = p->kov;
        }
    }
    p = kezd2->kov;

    /*ki lehet számolni, hogy a specifikációban leírt ösztöndíjértéket ezekkel a képletekkel kapatjuk meg - a c konstanst
    a jobb átláthatóság kedvéért vezetjük be, illetve hogy ne kelljen minden iterációnál újra kiszámolni */

    for (i = 0; i < 7; i++)
    {
        int j;
        for (j = 1; p->kov != NULL && j <= csop_tagszam[i]; j++)
        {
            // a változó, amivel a képletek számolnak
            double x = 1;
            if (csop_tagszam[i] > 1)
                if (szelsoertekek[i][0] == szelsoertekek[i][1])
                    x = csop_tagszam[i];
                else
                    x = (p->kreditindex - szelsoertekek[i][0]) * csop_tagszam[i] / (szelsoertekek[i][1] - szelsoertekek[i][0]);

            // ha a tag 95%ban van
            if (j < csop_tagszam[i] * 0.95)
                p->osztondij = lekerekit((((c - a) * x) / (0.95 * csop_tagszam[i]) + a) / 100) * 100;
            // ha a tag a top 5%ban van
            else if (j > csop_tagszam[i] * 0.95)
                p->osztondij = lekerekit(((((f - c) * x) / (0.05 * csop_tagszam[i])) + (0.05 * csop_tagszam[i] * c - (f - c) * 0.95 * csop_tagszam[i]) / (0.05 * csop_tagszam[i])) / 100) * 100;

            p = p->kov;
        }
    }
}

void kiiras(listaelem2 *kezd2, int csop_tagszam[]) // kiírja az ösztöndíjasok nevét és ösztöndíját egy osztondijasok.txt fájlba
{
    FILE *kimenet = fopen("osztondijasok.txt", "wt");
    listaelem2 *p = kezd2->kov;
    int i;
    int csoptagszam_osszegek[7] = {0};
    for (i = 0; i < 7; i++)
    {
        int j;
        for (j = 0; j <= i; j++)
            csoptagszam_osszegek[i] += csop_tagszam[j];
    }
    i = 0;
    while (p->kov != NULL)
    {
        // kiírjuk, hogy melyik hhcsop-nál tartunk a kiírás alatt
        if (i == 0 && csop_tagszam[0] != 0)
            fprintf(kimenet, "Üzemmérnök-informatikus alapképzés:\n");
        if (i == csoptagszam_osszegek[0] && csop_tagszam[1] != 0)
            fprintf(kimenet, "Mérnökinformatikus alapképzés:\n");
        if (i == csoptagszam_osszegek[1] && csop_tagszam[2] != 0)
            fprintf(kimenet, "Villamosmérnök alapképzés:\n");
        if (i == csoptagszam_osszegek[2] && csop_tagszam[3] != 0)
            fprintf(kimenet, "Mérnökinformatikus mesterképzés:\n");
        if (i == csoptagszam_osszegek[3] && csop_tagszam[4] != 0)
            fprintf(kimenet, "Villamosmérnök mesterképzés:\n");
        if (i == csoptagszam_osszegek[4] && csop_tagszam[5] != 0)
            fprintf(kimenet, "Egészségügyi mérnök mesterképzés:\n");
        if (i == csoptagszam_osszegek[5] && csop_tagszam[6] != 0)
            fprintf(kimenet, "Gazdaságinformatikus mesterképzés:\n");

        fprintf(kimenet, "%s: %dFt\n", p->nev, p->osztondij);
        i++;
        p = p->kov;
    }
    fclose(kimenet);
}

int main(void)
{
    //üres strázsás lista létrehozása
    FILE *kepzes, *jegyek;
    int db, tdb, i;
    int csop_tagszam[7] = {0};
    listaelem *kezd = (listaelem *)malloc(sizeof(listaelem));
    listaelem *veg = (listaelem *)malloc(sizeof(listaelem));
    neptun_atlag *kreditindexek;
    double e;
    int a, f;
    listaelem2 *kezd2;
    listaelem2 *veg2;
    listaelem *torlo;
    listaelem2 *torlo2;

    kezd->kov = veg;
    veg->kov = NULL;

    // adatok beolvasása
    kepzes = fopen("kepzes.txt", "rt");
    if (kepzes == NULL)
    {
        printf("Nem létezik a kepzes.txt fájl.\n");
        return 1;
    }
    db = beolvas(kepzes, kezd);
    fclose(kepzes);

    // hallgatói csoportok meghatározása
    meghat_hcsop(kezd, csop_tagszam);

    // kreditindex mezők kitöltése
    jegyek = fopen("jegyek.txt", "rt");
    if (jegyek == NULL)
    {
        printf("Nem létezik a jegyek.txt fájl.\n");
        return 1;
    }
    kreditindexek = (neptun_atlag *)malloc(db * sizeof(neptun_atlag));
    if (kreditindexek == NULL)
        return 1;
    tdb = 0; // ezzel számoljuk, hogy hány diák kreditindexét olvastuk be a tömbbe.
    kiszam_krediti(jegyek, kreditindexek, db, &tdb);
    krediti_listaba(kezd, kreditindexek, tdb);
    free(kreditindexek);
    fclose(jegyek);

    // kiláncoljuk a listából azokat, akik biztos nem kaphatnak ösztöndíjat
    szures(kezd);

    // a csop_tagszam tömb elemeit megváltoztatjuk annyira, amennyi fő (maximum) az adott hhcsoportból ösztöndíjat kaphat.
    for (i = 0; i < 7; i++)
        csop_tagszam[i] = lekerekit(csop_tagszam[i] / 2);

    // kreditindex alapján kiszűrjük a legjobb csop_tagszam[i] főt hhcsoportonként
    szur_legjobb_hhcsop(kezd, csop_tagszam);

    // bekérjük e-t, vagyis hogy max hányszorosa kaphat a hallgatóknak ösztöndíjat, a-t és f-et, az öszöndíj alsó és felső korlátját
    adatbekero(&e, &a, &f);

    // e-től függően a maradékból kiszűrni a legjobb lekerekit(db*e) főt
    szur_legjobb(kezd, lekerekit(db * e));

    //új láncolt lista, rendezve hhcsop és kreditindex szerint + csop_tagszam update-elés
    kezd2 = (listaelem2 *)malloc(sizeof(listaelem2));
    veg2 = (listaelem2 *)malloc(sizeof(listaelem2));
    kezd2->kov = veg2;
    veg2->kov = NULL;
    rendezve_masol(kezd, kezd2, csop_tagszam);

    // 1. láncolt lista törlése
    torlo = kezd;
    while (kezd != NULL)
    {
        kezd = kezd->kov;
        free(torlo);
        torlo = kezd;
    }
    veg = NULL;

    // kiosztja az ösztöndíjat hhcsoportonként
    osztondij_oszto(kezd2, csop_tagszam, a, f);

    // kiírni egy új fájlba
    kiiras(kezd2, csop_tagszam);

    // 2. láncolt lista törlése
    torlo2 = kezd2;
    while (kezd2 != NULL)
    {
        kezd2 = kezd2->kov;
        free(torlo2);
        torlo2 = kezd2;
    }
    veg2 = NULL;

    return 0;
}
