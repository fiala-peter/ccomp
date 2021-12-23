/**
 * @file nagyh.c
 * @author Gali Regina Anna (neptun k�d: AV1BDS)
 * @brief A program c�lja a k�t forr�sf�jl beolvas�sa ut�n, a felhaszn�l� �ltal megadott �vben sz�letett �s adott v�rosban j�tsz� j�t�kosok kilist�z�sa. Tov�bb� azon j�t�kosok kilist�z�sa, akik a felhaszn�l� �ltal megadott j�t�kos ellen j�tszhattak, ha a csapatokat nem utaztatt�k.
 * @version 0.1
 * @date 2020-11-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif
/**
 * @brief A j�t�kos strukt�ra defin�ci�ja.\n
 * A j�t�kos nev�t, nem�t, egyes�let�t sztringekben t�rolja, a sz�let�si �vet pedig eg�szben.
 */
typedef struct jatekos
{
    char nev[50 + 1];
    char nem[4 + 1];
    int dob;
    char egyesulet_neve[100 + 1];
} jatekos, *jatekos_ptr;
/**
 * @brief A j�t�kosokb�l �ll� l�colt lista defin�ci�ja.\n
 * Egy j�t�kos strukt�r�b�l �s a k�vetkez� listatagra mutat� pointerb�l �ll.
 */
typedef struct jatekos_lista
{
    jatekos j;
    struct jatekos_lista *next;
} jatekos_lista, *jat_list_ptr;
/**
 * @brief Az egyes�let strukt�ra defin�ci�ja.\n
 * Az egyes�let nev�t �s sz�khely�t is egy-egy sztringben t�rolja.
 */
typedef struct egyesulet
{
    char nev[100 + 1];
    char szekhely[50 + 1];
} egyesulet, *egyesulet_ptr;

/**
 * @brief Az egyes�letekb�l �ll� l�colt lista defin�ci�ja.\n
 * Egy egyes�let strukt�ra elemb�l �s a l�ncolt lista k�vetkez� elem�re mutat� pointerb�l �ll.
 */
typedef struct egyesulet_lista
{
    egyesulet e;
    struct egyesulet_lista *next;
} egyesulet_lista, *egy_list_ptr;

/**
 * @brief A sztringekben l�v� "\n" karaktert lecser�li egy "\0"-ra, hogy ne legyen felesleges karakter a sztringben.\n
 * Csak a lez�r� nulla karakterig n�zi a sztring elemeit.
 * @param string a sztring, amiben esetleg le kell cser�lnie az �jsor karaktert
 */
void lezaro(char *string)
{
    int i = 0;
    while (string[i] != '\0')
    {
        if (string[i] == '\n')
        {
            string[i] = '\0';
            break;
        }
        i++;
    }
}

/**
 * @brief K�tstr�zs�s �reslist�t k�sz�t a j�t�kosok l�colt list�j�hoz.\n
 * Ehhez dinamikusan foglal mem�ri�t a k�t elemnek, ha valamelyik foglal�s sikertelen, akkor NULL-pointert ad vissza. A h�ts� str�zsa next pointer�t NULL-ra �ll�tja, az el�ls� str�zsa next tagja pedig r�mutat a h�ts� str�zs�ra.
 * @return jatekos_lista* A lista fej�t adja vissza, ha mindk�t foglal�s sikeres, egy�bk�nt NULL-pointert.
 */
jatekos_lista *j_ureslista()
{
    jat_list_ptr head, tail;

    head = malloc(sizeof(jatekos_lista));
    if (head == NULL)
        return NULL;

    tail = malloc(sizeof(jatekos_lista));
    if (tail == NULL)
        return NULL;

    head->next = tail;
    tail->next = NULL;
    return head;
}
/**
 * @brief A jatekosok.txt f�jlb�l beolvasott adatoknak mem�ri�t foglal �s bef�zi �ket a l�ncolt list�ba.\n
 * A f�ggv�ny dinamikusan mem�ri�t foglal egy ::jatekos_lista t�pus� �j elemnek, amire az "ujelem" pointer mutat �s bef�zi ezt egy el�l-h�tul str�zs�s l�ncolt list�ba. A kapott adatok k�z�l a sztringeket a strcpy f�gv�nnyel m�solja �t az "ujelem" pointer "j" tagj�ba. Az �j elemet a lista elej�re, a str�zsa m�g� sz�rja be; egy "fuzo" nev� seg�dpointer fogja meg a str�zsa ut�ni elemet a besz�r�s el�tt.
 * 
 * @param head a l�colt lista els� elem�re mutat� pointer
 * @param jnev a j�t�kos nev�t tartalmaz� sztring
 * @param jnem a j�t�kos nem�t tartalmaz� sztring
 * @param jdob a j�t�kos sz�let�si �ve
 * @param jegyesulet a jat�kos egyes�lete
 * @return int -1-et ad vissza, ha nem siker�lt mem�ri�t foglalni a l�ncolt lista k�vetkez� elem�nek (az "ujelem" NULL-pointer), egy�bk�nt 0-t ad vissza.
 *
 */

int j_fuzo(jat_list_ptr head, char *jnev, char *jnem, int jdob, char *jegyesulet)
{
    jat_list_ptr fuzo;
    jat_list_ptr ujelem = malloc(sizeof(jatekos_lista));
    if (ujelem == NULL)
        return -1;

    ujelem->j.dob = jdob; //itt m�solja �t a j�t�kos adatait egy j�t�kos strukt�r�ba
    strcpy(ujelem->j.nev, jnev);
    strcpy(ujelem->j.nem, jnem);
    strcpy(ujelem->j.egyesulet_neve, jegyesulet);

    fuzo = head->next;
    head->next = ujelem;
    ujelem->next = fuzo;
    return 0;
}
/**
 * @brief Beolvassa a sz�veges f�jlt, ami a j�t�kosok adatait tartalmazza.\n
 * Minden beolvasott sort (a sorv�gi "\n"-nel egy�tt) egy-egy sztringben t�rol el. Ha a f�jl kinyit�sa nem sikeres, akkor ezt jelzi a felhaszn�l� fel� �s NULL-pointert visszaadva kil�p a f�ggv�nyb�l. Ha a f�jl kinyit�sa sikeres volt, akkor megh�vja a ::j_ureslista f�ggv�nyt. Ezek ut�n elkezdi beolvasni a f�jlt soronk�nt az fgets f�gv�ny seg�ts�g�vel, addig olvas, am�g tud. Mind a n�gy beolvasott adatot egy-egy sztringben t�rolja el. N�gy sort olvas be egym�s ut�n, a n�vre, nemre, egyes�letre megh�vja a ::lezaro f�ggv�nyt, a sz�let�si �vet tartalmaz� sztringre pedig az atoi f�ggv�nyt, ami a sztringb�l eg�szet csin�l. Ezut�n megh�vja a ::j_fuzo f�ggv�nyt, ami ha -1-et ad visszat�r�si �rt�kk�nt, akkor kil�p a f�ggv�ny a beolvas� ciklusb�l. Miut�n kil�pett a beolvas� ciklusb�l bez�rja a f�jlt. Ha m�r az els� elemn�l sikertelen volt a mem�riafoglal�s, akkor visszat�r�s el�tt felszabad�tja a k�t str�zs�t.
 * @param fajlnev a f�jl neve
 * @return jatekos_lista* A j�t�kosokb�l �ll� l�ncolt lista elej�re mutat� pointert adja vissza vagy NULL pointert, amennyiben a f�jl kinyit�sa sikertelen volt, nem siker�lt a k�t str�zs�nak mem�ri�t foglalni vagy ha m�r az els� listatagnak se tudott mem�ri�t foglalni.
 */
jatekos_lista *jatekos_olvaso(char *fajlnev)
{
    char nevek[51 + 1], nemek[6 + 1], egyesuletek[101 + 1], evek[6 + 1];
    int ev;
    FILE *fp = fopen(fajlnev, "r");
    jat_list_ptr head;
    if (fp == NULL)
    {
        printf("Nem siker�lt beolvasni a %s-t.", fajlnev);
        return NULL;
    }

    head = j_ureslista(); //l�trehozza a str�zs�s �reslist�t
    if (head == NULL)     //ha nem siker�lt a str�zs�knak se helyet foglalni
        return NULL;

    while ((fgets(nevek, 51, fp) != NULL && fgets(nemek, 6, fp) != NULL) && (fgets(evek, 6, fp) != NULL && fgets(egyesuletek, 100, fp) != NULL))
    {
        int siker;
        lezaro(nevek); //beolvas�sn�l a sorv�gi "\n"-eket is beolvassa a sztringbe, ezeket lev�gja a f�ggv�ny, hogy ne legyen felesleges karakter a t�mb v�g�n
        lezaro(nemek);
        lezaro(egyesuletek);
        ev = atoi(evek); //a sz�let�si �vet is egy sztringbe olvasta be - hogy ne kelljen k�l�n a sorv�gi '\n' karakter beolvas�s�val foglalkozni -, ez�rt ezt �t kell alak�tani eg�ssze (a strukt�ra defin�ci� miatt)
        siker = j_fuzo(head, nevek, nemek, ev, egyesuletek);
        if (siker == -1) //nem siker�lt a dinamikus foglal�s
        {
            printf("Nem siker�lt mem�ri�t foglalni a lista k�vetkez� elem�nek. (%s m�r nincs benne a list�ban, se az �t k�vet� emberek.) \n", nevek);
            break; //ekkor kil�p a ciklusb�l, mert val�sz�n�leg a k�vetkez� foglal�s is sikertelen lenne
        }
    }

    fclose(fp);

    if (head->next->next == NULL) //a j_fuzo m�r az els� elemnek se tudott mem�ri�t foglalni
    {
        free(head); //a k�t str�zsa felszabad�t�sa, hogy ne legyen mem�riasziv�rg�s
        return NULL;
    }

    return head;
}
/**
 * @brief Felszabad�tja a dinamikusan foglalt j�t�kosokb�l �ll� l�ncolt list�t.
 * 
 * @param head a j�t�kos lista elej�re mutat� pointer
 */
void j_free(jat_list_ptr head)
{
    jat_list_ptr fogo;
    while (head != NULL)
    {
        fogo = head->next; //a listafej ut�ni elemet fogja meg
        free(head);        //a listafej felszabad�t�sa
        head = fogo;
    }
}
/**
 * @brief K�tstr�zs�s �reslist�t hoz l�tre.
 * Dinamikusan foglal mem�ri�t a k�t ::egyesulet_lista elemnek (ezek lesznek a str�zs�k), ha valamelyik foglal�s sikertelen, akkor NULL-pointer ad vissza. A h�ts� str�zsa next pointer�t NULL-ra �ll�tja, az el�ls� str�zsa next tagja pedig r�mutat a h�ts� str�zs�ra.
 * @return egyesulet_lista* Visszaadja az egyes�let lista elej�re mutat� pointert, azaz az el�ls� str�zs�ra mutat� pointert, ha mindk�t foglal�s sikeres. Egy�bk�nt NULL-pointerrel t�r vissza.
 */
egyesulet_lista *e_ureslista()
{
    egy_list_ptr head, tail;
    
    head = malloc(sizeof(egyesulet_lista));
    if (head == NULL)
        return NULL;

    tail = malloc(sizeof(egyesulet_lista));
    if (tail == NULL)
        return NULL;

    head->next = tail;
    tail->next = NULL;
    return head;
}
/**
 * @brief Dinamikusan mem�ri�t foglal a l�ncolt lista k�vetkez� elem�nek �s bef�zi azt a lista elej�re.\n
 * A f�ggv�ny egy egyesulet_lista elemnek dinamikusan foglal mem�ri�t, erre az "ujelem" pointer mutat. Ha nem siker�lt a foglal�s, akkor az "ujelem" pointer NULL lesz. Ha siker�l a foglal�s, akkor a kapott adatokat az strcpy f�ggv�nnyel �tm�solja az "ujelem" pointer "e" tagj�ba �s bef�zi az "ujelemet" az el�ls� str�zsa m�g�. Egy "fuzo" nev� seg�dpointer fogja meg a str�zsa ut�ni elemet a besz�r�s el�tt.
 * @param head a l�ncolt lista feje
 * @param enev az egyes�let nev�t tartalmaz� sztring
 * @param varos az egyes�let sz�khely�t tartalmaz� sztring
 * @return int -1-et ad vissza, ha nem siker�lt dinamikusan mem�ri�t foglalnia a k�vetkez� listaelemnek; ha a foglal�s sikeres, akkor 0-�t ad vissza.
 */
int e_fuzo(egy_list_ptr head, char *enev, char *varos)
{
    egy_list_ptr fuzo;
    egy_list_ptr ujelem = malloc(sizeof(egyesulet_lista));
    if (ujelem == NULL) //nem siker�lt a mem�riafoglal�s
        return -1;

    strcpy(ujelem->e.nev, enev); //egyes�let adatainak m�sol�sa
    strcpy(ujelem->e.szekhely, varos);

    fuzo = head->next;
    head->next = ujelem;
    ujelem->next = fuzo;
    return 0;
}
/**
 * @brief Beolvassa a sz�veges f�jlt, ami az egyes�letek adatait tartalmazza. \n
 * Minden beolvasott sort (a sorv�gi "\n"-nel egy�tt) egy-egy sztringben t�rol el. Ha a f�jl kinyit�sa sikertelen, akkor ezt jelzi a felhaszn�l� fel� �s NULL-pointerrel t�r vissza. Ha a f�jl kinyit�sa sikeres volt, akkor megh�vja az ::e_ureslista f�ggv�nyt. Ezek ut�n elkezdi beolvasni a f�jlt soronk�nt az fgets f�ggv�ny seg�ts�g�vel, addig olvas, am�g tud. K�t sort olvas be egym�s ut�n, a nevet �s sz�khelyet tartalmaz� sztringekre megh�vja a ::lezaro f�ggv�nyt. Ezut�n megh�vja az ::e_fuzo-t, ha ez a f�ggv�ny -1-et ad visszat�r�si �rt�kk�nt, akkor kil�p a beolvas� ciklusb�l. Ha m�r az els� elemn�l sikertelen volt a mem�riafoglal�s, akkor visszat�r�s el�tt felszabad�tja a k�t str�zs�t.
 * @param fajlnev a f�jl neve
 * @return egyesulet_lista* Az egyes�letekb�l �ll� l�ncolt lista elej�re mutat� pointert adja vissza vagy NULL-pointert, ha a f�jl kinyit�sa sikertelen volt vagy akkor, ha csak a k�t str�zs�nak siker�lt mem�ri�t foglalni (vagy azoknak se).
 */

egyesulet_lista *egyesulet_olvaso(char *fajlnev)
{
    char nevek[101 + 1], varosok[51 + 1];
    egy_list_ptr head;
    FILE *fp = fopen(fajlnev, "r");
    if (fp == NULL) //ha nem siker�l kinyitnia a f�jlt, akkor NULL-pointerrel t�r vissza
    {
        printf("Nem siker�lt beolvasni az %s-t.", fajlnev);
        return NULL;
    }

    head = e_ureslista(); //l�trej�n a str�zs�s �reslista
    if (head == NULL)     //ha nem siker�lt helyet foglalni a str�zs�knak
        return NULL;

    while (fgets(nevek, 100, fp) != NULL && fgets(varosok, 50, fp) != NULL) //beolb�vas�s a f�jlb�l
    {
        int siker;
        lezaro(nevek); //beolvasott �jsor karakter cser�je '\0'-ra
        lezaro(varosok);
        siker = e_fuzo(head, nevek, varosok); //siker�lt-e mem�ri�t foglalni a k�vetkez� elemnek
        if (siker == -1)
        {
            printf("Nem siker�lt mem�ri�t foglalni a lista k�vetkez� elem�nek. (A %s m�r nincs benne a list�ban, se az �t k�vet� egyes�letek.)", nevek); //ha nem siker�lt, akkor kil�p a while ciklusb�l
            break;
        }
    }

    fclose(fp);

    if (head->next->next == NULL) //az e_fuzo m�r az els� elemnek se tudott mem�ri�t foglalni
    {
        free(head); //a k�t str�zsa felszabad�t�sa, hogy ne legyen mem�riasziv�rg�s
        return NULL;
    }

    return head;
}
/**
 * @brief Felszabad�tja a dinamikusan foglalt mem�ri�t.
 * 
 * @param head az egyes�letekb�l �ll� lista elej�re mutat� pointer
 */
void e_free(egy_list_ptr head)
{
    egy_list_ptr fogo; //ezzel fogja meg a head ut�ni elemet felszabad�t�skor
    while (head != NULL)
    {
        fogo = head->next;
        free(head);
        head = fogo;
    }
}
/**
 * @brief Megkeresi azokat a j�t�kosokat, akik az adott �vben sz�lettek �s adott v�rosban j�tszanak. Ezeket kilist�zza a standard outputra.\n
 * El�sz�r v�gigmegy a j�t�kosok list�j�n azt vizsg�lva, hogy a j�t�kos sz�let�si �ve megegyezik-e a kapott �vvel. Ha igen, akkor megkeresi az egyes�letek list�j�ban a j�t�kos egyes�let�t �s az strcmp f�ggv�nnyel �sszehasonl�tja az egyes�let sz�khely�t a kapott v�rossal. Ha megegyezik, akkor ki�rja a j�t�kos nev�t a standard outputra �s a "szamlalo" nev� eg�szt eggyel n�veli. Ha v�gigment a j�t�kosok list�j�n �s a "szamlalo" �rt�ke tov�bbra is 0, akkor jelzi a felhaszn�l� fel�, hogy nincs olyan j�t�kos, aki az adott �vben sz�letett �s az adott v�rosban j�tszik.
 *  
 * @param jhead a j�t�kos lista el�ls� str�zs�j�ra mutat� pointer
 * @param ehead az egyes�let lista el�ls� str�zs�j�ra mutat� pointer
 * @param ev a sz�let�si �v
 * @param varos a v�ros
 */
void jatekos_avarosban(jat_list_ptr jhead, egy_list_ptr ehead, int ev, char *varos)
{
    jat_list_ptr nyomtat = jhead->next; //str�zsa ut�ni listaelemre mutat� pointer
    jatekos print;
    egy_list_ptr egylet;
    egyesulet jo;
    int szamlalo = 0;

    while (nyomtat->next != NULL)
    {
        print = nyomtat->j;  //print j�t�kos a nyomtatnak a j�t�kosa
        if (print.dob == ev) //ha print sz�let�si �ve == a kapott �vvel
        {
            egylet = ehead->next; //str�zsa ut�ni elemre mutat az egylet pointer
            jo = egylet->e;       //v�gigmegy az egyes�leteken �s megn�zi, melyikben van print
            while (strcmp(print.egyesulet_neve, jo.nev) != 0 && egylet->next != NULL)
            {
                egylet = egylet->next;
                jo = egylet->e;
            }
            if (strcmp(jo.szekhely, varos) == 0) //megtal�lta az egyes�letet
            {
                if (szamlalo == 0)
                    printf("Ebben a v�rosban (%s) j�tsz� j�t�kosok, akik ekkor sz�lettek: %d\n", varos, ev);
                printf("%s\n", print.nev); //ha ugyanaz az egyesulet szekhelye mtint a varos akkor kiirjuk printe
                szamlalo++;
            }
        }
        nyomtat = nyomtat->next;
    }

    if (szamlalo == 0)
        printf("Nincs olyan j�t�kos ebben a v�rosban (%s), aki ebben az �vben (%d) sz�letett volna.", varos, ev);
}
/**
 * @brief Kilist�zza, hogy egy adott j�t�kos kik ellen j�tszhatott abban az esetben, ha nem utaztatj�k a csapatokat.
 * El�sz�r megkeresi a f�ggv�ny a kapott j�t�kos egyes�let�t (felt�telezz�k, hogy a j�t�kos egyes�lete benne van a list�ban). Ha megvan az egyes�let, akkor a sz�khelye az "egy" egyes�let t�pus� strukt�ra "szekhely" tagj�ban lesz t�rolva. Ezek ut�n v�gigmegy az egyes�letek list�j�n �s megkeresi azokat, amiknek ugyanaz a sz�khelye. Ha tal�l egy ilyet, akkor a j�t�kosokat n�zi v�gig, hogy van-e olyan, aki ann�l a csapatn�l j�tszik (felt�ve, hogy ez nem egyezik meg a kapott j�t�kos csapat�val), ugyanakkor sz�letett-e, mint a kapott j�t�kos �s ugyanaz-e a neme. Ha igen, akkor �t ki�rja a standard outputra. Ha az adott v�rosban csak egy csapat van, akkor ezt ki�rja a standard outputra.
 * @param player a j�t�kos
 * @param ehead az egyes�let lista feje
 * @param jhead a j�t�kos lista feje
 */
void versus(jatekos const *player, egy_list_ptr ehead, jat_list_ptr jhead)
{
    int szamlalo = 0;
    egyesulet egy;
    jatekos jat;
    egy_list_ptr q = ehead->next;
    jat_list_ptr p = jhead->next;

    jat = p->j;
    egy = q->e;
    while (strcmp(egy.nev, player->egyesulet_neve) != 0) //felt�telezz�k, hogy a j�t�kos egyes�lete benne van a list�ban
    {
        q = q->next;
        egy = q->e;
    }
    //megvan a V�ROS---> egy.szekhely
    q = ehead->next;
    while (q->next != NULL)
    {
        while (strcmp(egy.szekhely, q->e.szekhely) != 0 && q->next != NULL) //q v�gigmegy az egyes�leteken �s keresi azokat, amiknek a sz�khelye azonos a kapott j�t�kos csapat�nak a sz�khely�vel
            q = q->next;

        if (q->next == NULL)
            break;

        while (p->next != NULL)
        { //van-e olyan j�t�kos, aki itt j�tszik, de nem ugyanabban az egyes�letben van, mint a kapott j�t�kos; ezek ut�n m�g megn�zi, hogy megegyezik-e a nem�k �s a sz�let�si �v�k
            if ((strcmp(jat.egyesulet_neve, q->e.nev) == 0 && strcmp(jat.egyesulet_neve, player->egyesulet_neve) != 0) && (strcmp(jat.nem, player->nem) == 0 && player->dob == jat.dob))
            {
                if (szamlalo == 0)
                    printf("%s ellen�k j�tszhatott:\n", player->nev);

                printf("%s\n", jat.nev);
                szamlalo++;
            }
            p = p->next;
            jat = p->j;
        }
        p = jhead->next; //visszamegy a j�t�kos lista elej�re, hogy a k�vetkez� egyes�letn�l is az �sszes j�t�kost vizsg�lja
        q = q->next;
    }
    if (szamlalo == 0)
        printf("Ebben a v�rosban (%s) csak egy csapat van.", egy.szekhely);
}
/**
 * @brief Megn�zi, hogy van-e egy�ltal�n olyan j�t�kos, aki a felhaszn�l� �ltal megadott �vben sz�letett.\n
 * Ha nincs ilyen sz�let�si �v� j�t�kos a list�ban, akkor ezt k�zli a felhaszn�l�val a standard outputon kereszt�l.
 * @param head a j�t�kosokb�l �ll� l�ncolt lista elej�re mutat� pointer
 * @param ev a sz�let�si �v
 * @return int 1, ha van olyan, aki abban az �vben sz�letett; 0 ha nincs.
 */
int benne_van(jat_list_ptr head, int ev)
{
    jat_list_ptr h = head->next;
    while (h->next != NULL)
    {
        if (h->j.dob == ev)
            return 1;
        h = h->next;
    }
    printf("Nincs olyan j�t�kos, aki ebben az �vben (%d) sz�letett.\n", ev);
    return 0;
}
/**
 * @brief Megn�zi, hogy van-e egy�ltal�n olyan egyes�let, aminek a sz�khelye a felhaszn�l� �ltal megadott v�ros.
 * El�sz�r azt vizsg�lja, hogy nagybet�vel �rt�k-e a v�rosnevet. Ha nem akkor jelzi a felhaszn�l� fel�, hogy azzal k�ne �s nem n�zi v�gig az egyes�leteket tartalmaz� list�t, hiszen biztos, hogy nem lesz egyez�s. Ha nagybet�vel van �rva, de m�gsincs benne a list�ban, akkor ezt is jelzi a felhaszn�l� fel�. Az egyez�st a strcmp f�ggv�ny seg�ts�g�vel keresi.
 * @param head a l�ncolt lista feje
 * @param varos a keresett v�ros
 * @return int 1, ha benne van a list�ban �s 0, ha nincs.
 */
int benne_vanv(egy_list_ptr head, char *varos)
{
    egy_list_ptr egyes = head->next; //a str�zsa ut�ni elemr�l indul
    if (varos[0] <= 'z' && varos[0] >= 'a')
    {
        printf("Pr�b�ld nagy kezd�bet�vel!\n");
        return 0;
    }
    lezaro(varos); // a kapott sztring v�g�r�l lev�gja az enter miatt keletkezett �jsor karaktert
    while (egyes->next != NULL)
    {
        if (strcmp(varos, egyes->e.szekhely) == 0)
            return 1;
        egyes = egyes->next; //v�gigmegy a list�n
    }
    printf("Nincs ebben a v�rosban (%s) sportegyes�let.\n", varos);
    return 0;
}

/**
 * @brief A jatekos_kereso m�dos�t�sa. Ha t�bb azonos nev� j�t�kos van, akkor keresett j�t�kos sz�let�si �v�t is bek�ri a f�program �s �tadja ennek a f�ggv�nynek. \n
 * Ha nem tal�lja az adott nev� j�t�kost a kapott sz�let�si �vvel, akkor ezt jelzi a felhaszn�l� fel�.
 * @param nev a j�t�kos neve
 * @param head a lista elej�re mutat� pointer
 * @param szulev a j�t�kos sz�let�si �ve
 * @return jat_list_ptr a j�t�kos lista megfelel� elem�re mutat� pointer vagy NULL, ha nincs ilyen j�t�kos.
 */
jat_list_ptr jatekos_kereso_mod(char *nev, jat_list_ptr head, int szulev)
{
    head = head->next; //str�zsa ut�ni elemr�l kezd
    lezaro(nev);       //a n�v v�g�r�l leszedi az entert, hogy a strcmp f�ggv�nnyel lehessen kereseni az egyez�st
    while (head->next != NULL)
    {
        if (strcmp(nev, head->j.nev) == 0 && szulev == head->j.dob)
            return head;

        head = head->next;
    }
    printf("Nincs %s nev� j�t�kos, aki ebben az �vben (%d) sz�letett volna.", nev, szulev);

    return NULL;
}

/**
 * @brief Megn�zi, szerepel-e az adott nev� j�t�kos a list�ban �s ha igen, akkor nincs-e t�bb ilyen nev� j�t�kos is. \n
 * Ha nincs ilyen nev� j�t�kos a list�ban, akkor ezt jelzi a felhaszn�l� fel� a standard outputon kereszt�l, valamint ha t�bb ilyen j�t�kos van, akkor azt is.
 * @param nev a j�t�kos neve
 * @param head a l�ncolt lista feje
 * @return jat_list_ptr Ha egy ilyen nev� j�t�kos van, akkor a r� mutat� pointert adja vissza. Ha nincs ilyen j�t�kos, akkor NULL pointert �s param�terlist�n 0-t ad vissza. Ha t�bb ilyen j�t�kos van, akkor az � sz�mukat param�terlist�n �s mell� egy NULL pointert ad vissza.
 */
jat_list_ptr jatekos_kereso(char *nev, jat_list_ptr head, int *szamlal)
{
    jat_list_ptr keresett;
    int szamlalo = 0;
    head = head->next;
    lezaro(nev);
    while (head->next != NULL)
    {
        if (strcmp(nev, head->j.nev) == 0)
        {
            szamlalo++;
            keresett = head;
        }
        head = head->next;
    }
    *szamlal = szamlalo; //ezt adja vissza param�terlist�n kereszt�l

    if (szamlalo == 1)
        return keresett; //ha pontosan egy ilyen j�t�kos van, akkor a listaelemre mutat� pointert adja vissza

    if (szamlalo == 0) //nincs ilyen nev� j�t�kos
    {
        printf("Nincs %s nev� j�t�kos a list�ban.", nev);
    }
    if (szamlalo != 0) //t�bb ilyen nev� j�t�kos van
    {
        printf("T�bb %s nev� j�t�kos is van a list�ban.", nev);
    }
    return NULL;
}
/**
 * @brief A f�program. \n
 * A felhaszn�l�val val� interakci� nagyr�szt itt t�rt�nik. A standard inputon �t �rkez� adatokat itt olvassa be a program. 
 * 
 * @param argc a parancssori param�terek sz�ma
 * @param argv sztringt�mb, ami a param�tereket tartalmazza
 * @return int Visszat�r�si �rt�ke 0, ha a program rendeltet�sszer�en futott le, k�l�nben -1. 
 */
int main(int argc, char *argv[])
{
#ifdef _WIN32
    SetConsoleCP(1250);
    SetConsoleOutputCP(1250);
#endif

    int szamlalo, i, ev;
    char *jfajl = "default.txt";
    char *efajl = "default.txt";
    char varos[51 + 1], jatek[51 + 1]; //az enter miatt 51+1 �s nem 50+1
    jat_list_ptr ptr, jhead;
    egy_list_ptr ehead;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            printf("Haszn�lat: %s\n-j a j�t�kosokat tartalmaz� sz�veges f�jl neve a kiterjeszt�ssel egy�tt (pl.: -j jatekosok.txt) \n-e az egyes�leteket tartalmaz� sz�veges f�jl neve a kiterjeszt�ssel egy�tt", argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-j") == 0)
            jfajl = argv[++i];
        else if (strcmp(argv[i], "-e") == 0)
            efajl = argv[++i];
        else
        {
            printf("Ismeretlen param�ter: %s", argv[i]);
            return -1;
        }
    }
    if (strcmp(efajl, "default.txt") == 0 || strcmp(jfajl, "default.txt") == 0) //ha nem lett �tadva parancsori argumentumk�nt legal�bb az egyik f�jl neve, akkor befejez�dik a program
    {
        printf("Legal�bb az egyik f�jl neve nem volt a parancssori argumentumok k�z�tt.");
        return -1;
    }

    jhead = jatekos_olvaso(jfajl);
    if (jhead == NULL)
        return -1;
    ehead = egyesulet_olvaso(efajl);
    if (ehead == NULL)
    {
        j_free(jhead); //a m�sik list�t fel kell szabad�tani a return el�tt, hogy ne legyen mem�riasziv�rg�s
        return -1;
    }

    printf("Adj meg egy sz�let�si �vet: ");
    while (scanf("%d", &ev) == 0)
    {
        printf("Egy sz�mot adj meg: ");
        fflush(stdin); //nem mindig ugyanazt olvassa be, �gy nem ker�l v�gtelen ciklusba
    }
    fflush(stdin); //az enter miatt
    while (benne_van(jhead, ev) == 0)
    {
        printf("Adj meg egy �j sz�let�si �vet: ");
        scanf("%d", &ev);
        fflush(stdin); //az enter miatt
    }

    printf("Adj meg egy v�rost: ");
    fgets(varos, 50, stdin);
    while (benne_vanv(ehead, varos) == 0)
    {
        printf("Add meg a v�rost �jra: ");
        fgets(varos, 50, stdin);
    }
    jatekos_avarosban(jhead, ehead, ev, varos);

    printf("\nAdj meg egy j�t�kost csupa nagy bet�vel: ");
    fgets(jatek, 50, stdin);
    ptr = jatekos_kereso(jatek, jhead, &szamlalo);
    while (ptr == NULL && szamlalo == 0) // nincs ilyen nev� j�t�kos, ez�rt addig k�r be neveket, am�g nem kap egy olyat, ami szerepel a list�ban
    {
        printf("\nAdj meg egy m�sik j�t�kost: ");
        fgets(jatek, 50, stdin);
        ptr = jatekos_kereso(jatek, jhead, &szamlalo);
    }
    if (ptr == NULL && szamlalo != 0) //t�bb ilyen nev� j�t�kos is van
    {
        printf("\nAdd meg %s sz�let�si �v�t: ", jatek);
        scanf("%d", &ev);
        ptr = jatekos_kereso_mod(jatek, jhead, ev);
        while (ptr == NULL) //ilyen sz�let�si �vvel nem tal�lta meg az adott nev� j�t�kost, ez�rt addig k�r be �j �veket, am�g nem kap megfelel�t
        {
            printf("\nAdd meg %s sz�let�si �v�t �jra: ", jatek);
            scanf("%d", &ev);
            ptr = jatekos_kereso_mod(jatek, jhead, ev);
        }
    }
    versus(ptr->j, ehead, jhead);

    j_free(jhead);
    e_free(ehead);

    return 0;
}
