/**Egy olyan program irasa a cel, amely meghatározza, hogy egy csillagával adott naprendszer bolygói 
 közt van-e olyan, aminek a nap-év aránya nagyobb a naprendszer bármely másik bolygójának a tömegének
  és a bolygó körül keringő összes hold össztömegének arányánál.
A program a csillag nevét a standard inputon kapja, a holdak és a bolygók fájlnevei
parancssori argumentumok. A választ a standard outputon adja, és kiírja oda egy megfelelő bolygó nevét.**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    MAX_NAME = 15
}; //Felsorolt típus a sztringek méretének könnyebb megadásához
typedef struct m_data
{ //A holdak felhasználandó adatainak tárolására alkalmas adattípus
    char name[MAX_NAME];
    char planet[MAX_NAME];
    double mass;
} m_data;
typedef struct moon
{ //A hold adatokat tároló listaelemtípus
    m_data d;
    struct moon *next;
} moon;
typedef struct p_data
{ //A bolygók felhasználandó adatainak tárolására alkalmas adattípus
    char name[MAX_NAME];
    char star[MAX_NAME];
    double mass;
    double T;
    double Tr;
} p_data;
typedef struct planet
{ //A bolygó adatokat tároló listaelemtípus
    p_data d;
    moon *moons;
    struct planet *next;
} planet;

moon *create_m_list()
{ //A holdak strázsás láncolt listáját létrehozó függvény, visszatérési értéke a létrehozott lista feje
    moon *p = malloc(sizeof(moon));
    double d = -1.0;
    if (p == NULL)
    {
        fprintf(stderr, "Memory allocation unsuccesful\n");
        return p;
    }
    p->next = malloc(sizeof(moon));
    p->next->next = NULL;
    return p;
}
planet *create_p_list()
{
    planet *p = malloc(sizeof(planet)); //A bolygók strázsás láncolt listáját létrehozó függvény, visszatérési értéke
    if (p == NULL)
    {
        fprintf(stderr, "Memory allocation unsuccesful\n");
        return p;
    }
    p->next = malloc(sizeof(planet));
    p->next->next = NULL;
    p->next->moons = NULL;
    p->moons = NULL;
    return p;
}
int fill_moon_list(moon *head, char *f)
{                             //A holdak lácolt listáját feltöltő függvény: egy moon* listacímet és egy char*f fájlnevet kap

    FILE *fm = fopen(f, "r"); //paraméterként, a fájlból egyszerre egy hold adatait olvassa ki. Amennyiben sikeres volt az
    if (fm == NULL)
    {                                                         //olvasás, akkor memóriát foglal egy listaelemnek, belemásolja az adatokat és beilleszti a listába
        fprintf(stderr, "Could not open file containing moon data\n"); //A hibaüzeneteket a standard error outputra írja, ha sikeresen
        return 0;                                             //lefutott 1-gyel tér vissza, ha hiba történt akkor 0-val
    }

    while (1)
    {
        m_data d;
        moon *m;
        if (fscanf(fm, "Nev: %[^\n]\nTomeg:%lf\nBolygoja:%[^\n]\n", d.name, &d.mass, d.planet) == EOF)
            break;
        m = malloc(sizeof(moon));
        if (m == NULL)
        {
            fprintf(stderr, "Memory allocation unsuccesful\n");
            return 0;
        }
        m->d = d;
        m->next = head->next;
        head->next = m;
    }

    if (fclose(fm) != 0)
    {
        fprintf(stderr, "Could not close file containing moon data\n");
        return 0;
    }
    return 1;
}
void attach_moons(planet *phead, moon *mhead) //Ez a függvény bolygók és holdak fésűs listáját hozza létre oly módon, hogy a
{
    planet *p = phead->next; //paraméterként kapott planet típusú strázsás listán végigszaladva minden bolygó
    moon *m, *mprev;         //moons * moon pointerére felfűzi a szintén paraméterként kapott moons típusú
    for (; p->next != NULL; p = p->next)
    {                    //strázsás lista megfelelő elemeit. Ez az átalakítás azért hasznos, mert megkönnyíti
        m = mhead->next; // a sum_moon_mass függvény működését, ugyanakkor a megmaradt listában még maradhatnak
        mprev = mhead;   //holdak, amelyek egyik listabeli bolygóhoz sem tartoznak.
        while (m->next != NULL)
        {

            if (strcmp(p->d.name, m->d.planet) == 0)
            {
                mprev->next = m->next;
                m->next = p->moons;
                p->moons = m;
                m = mprev->next;
            }
            else
            {
                m = m->next;
                mprev = mprev->next;
            }
        }
    }
}
int fill_planet_list(planet *head, char *f)
{                             //A bolygók láncolt listáját feltöltő függvényegy planet* listacímet és egy char*f fájlnevet kap                         //paraméterként, a fájlból egyszerre egy hold adatait olvassa ki. Amennyiben sikeres volt az
                              //paraméterként, a fájlból egyszerre egy bolygó adatait olvassa ki. Amennyiben sikeres volt az
    FILE *fp = fopen(f, "r"); //olvasás, akkor memóriát foglal egy listaelemnek, belemásolja az adatokat és beilleszti a listába
    if (fp == NULL)
    {                                                           //A hibaüzeneteket a standard error outputra írja, ha sikeresen
        fprintf(stderr, "Could not open file containing planet data\n"); //lefutott 1-gyel tér vissza, ha hiba történt akkor 0-val
        return 0;
    }
    while (1)
    {
        p_data d;
        planet *p;
        if (fscanf(fp, "Nev: %[^\n]\nTomeg:%lf\nCsillaga:%[^\n]\nKeringesi ideje: %lf\n", d.name, &d.mass, d.star, &d.T) == EOF)
            break;
        fscanf(fp, "Sziderikus forgasido:%lf\n", &d.Tr);
        p = malloc(sizeof(planet));
        if (p == NULL)
        {
            fprintf(stderr, "Memory allocation unsuccesful\n");
            return 0;
        }
        p->d = d;
        p->next = head->next;
        head->next = p;
        p->moons = NULL;
    }
    if (fclose(fp) != 0)
    {
        fprintf(stderr, "Could not close file containing planet data\n");
        return 0;
    }
    return 1;
}

void delete_m_list(moon *head)
{ //Moon típusú láncolt listát törlő függvény, egy moon típusú lista fejét kapja paraméterként
    moon *p;
    while (head->next != NULL)
    {
        p = head;
        head = head->next;
        free(p);
    }
    free(head);
}
void delete_p_list(planet *head)
{ //Planet típusú láncolt listát törlő függvény, egy planet típusú lista fejét kapja paraméterként
    planet *p;
    while (head->next != NULL)
    {
        p = head;
        if (p->moons != NULL)
        {
            delete_m_list(p->moons);
        }
        head = head->next;
        free(p);
    }
    free(head);
}
double sum_moon_mass(planet *p)
{                                              //Egy bolygóhoz tartozó hold típusú lista elemeinek tömegét összeadó függvény
    double sum = -1.0;                    //Egy planet típusú listaelemre mutató pointert kap paraméterként, double-ben adja vissza a
    moon *m;                                   //holdak össztömegék. Amennyiben az adott bolygó nem rendelkezik holdakkal -1.0-val tér vissza
    for (m = p->moons; m != NULL; m = m->next) //az egyszerűbb esetekre bontás érdekében
        sum = sum + m->d.mass;
    return sum;
}
int is_there(planet *p, char *s)
{                        //A program főfüggvénye, egy planet típusú lista fejét és a felhasználó által kijelölt
    planet *q;           //csillag nevét kapja paraméterként. Iterálva halad végig a listán, és amennyiben
    double d_year_ratio; //az adott bolygó csillaga megfelelő, keres egy másik ahhoz a csillaghoz tarozó bolygót
    for (; p->next != NULL; p = p->next)
    { //Ha talál egy ilyen párt, a sum_moon_mass függvényt megállapítja, hogy az első nap/év
        if (strcmp(s, p->d.star) != 0)
            continue; //aránya nagyobb-e mint a második bolygó esetén  a bolygó tömegének/holdjainak össztömegének aránya.
        for (q = p; q->next != NULL; q = q->next)
        { //Az első teljesülés esetén kiírja az eredményt és a megfelelő bolygó nevét, majd kilép.
            double moons_mass;
            if (strcmp(s, q->d.star) != 0)
                continue;                    //Ha nem talál ilyet akkor azt írja a standard outputra. Visszatérési értéke a keresés
            d_year_ratio = p->d.T / p->d.Tr; //sikeressége esetén 1, különben 0.
            moons_mass = sum_moon_mass(q);
            if (moons_mass == -1.0)
                continue;
            if ((d_year_ratio) > (q->d.mass / moons_mass))
            {
                printf("Yes, planet %s, from %s's solar system matches the requirements.", p->d.name, s);
                return 1;
            }
        }
    }
    printf("No, there's no such planet in the database.");
    return 0;
}

int main(int argc, char **argv)
{
    moon *mhead;
    planet *phead;
    int a, b;
    char s[MAX_NAME];

    mhead = create_m_list();
    phead = create_p_list();
    if (mhead == NULL)
    {
        fprintf(stderr, "Problem creating empty moon list");
        return 1;
    }
    if (phead == NULL)
    {
        fprintf(stderr, "Problem creating empty planet list");
        return 1;
    }
    a = fill_moon_list(mhead, argv[1]);
    b = fill_planet_list(phead, argv[2]);

    if (a != 1 || b != 1)
    {
        delete_m_list(mhead);
        delete_p_list(phead);
        return 1;
    };
    attach_moons(phead, mhead);
    printf("Please enter the name of the star of the chosen solar system:");
    scanf("%[^\n]", s);
    is_there(phead, s);
    delete_p_list(phead);
    delete_m_list(mhead);

    return 0;
}
