/*
 * Soubor:  proj3.c
 * Datum:   2015/12/10
 * Autor:   Roman Nahalka, xnahal01@stud.fit.vubr.cz
 * Projekt: Jednoducha shlukova analyza, projekt c. 3 pro predmet IZP
 * Popis:   Program provadi jednoduchou shlukovou analyzu pomoci metody nejblizsiho
 *          souseda. Program zpracuje objekty ze souboru, ktery je zadan jako argument
 *          programu. Volitelny argument oznacuje do kolika shluku se maji objekty shlukovat.
 *          Neni-li tento argument zadan, takse pocet shluku rovna 1.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <float.h> // FLT_MAX
#include <errno.h> // Chybove hlaseni

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct objt_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    struct obj_t *obj = malloc(cap * sizeof(struct obj_t));

    if(obj == NULL)
        return;

    c->obj = obj;
    c->capacity = cap;
    c->size = 0;


}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->obj = NULL;
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */ 
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->capacity == c->size)
        resize_cluster(c, c->capacity + 1);
    
    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    int i;

    for(i=0; i<c2->size; i++)
        append_cluster(c1, c2->obj[i]);

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    int i;

    clear_cluster(&carr[idx]);

    for(i = idx; i<narr-1; i++)
        carr[i] = carr[i+1];

    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    return sqrtf(powf((o1->x - o2->x), 2) + powf((o1->y - o2->y), 2));
}

/*
 Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
 souseda.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    int i, j;
    float pomocna_vzdalenost = 0;
    float vzdalenost = FLT_MAX;

    for(i=0; i<c1->size; i++)
    {
        for(j=0; j<c2->size; j++)
        {
            pomocna_vzdalenost = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(pomocna_vzdalenost < vzdalenost)
                vzdalenost = pomocna_vzdalenost;
        }
    }

    return vzdalenost;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
 identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
 pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    int i, j;
    float pomocna_vzdalenost = 0;
    float vzdalenost = FLT_MAX;

    for(i=0; i<narr; i++)
    {
        for(j=i+1; j<narr; j++)
        {
            pomocna_vzdalenost = cluster_distance(&carr[i], &carr[j]);
            if(pomocna_vzdalenost < vzdalenost)
            {
                vzdalenost = pomocna_vzdalenost;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    int i, pocet = 0;
    struct obj_t obj;

    FILE *soubor;
    if((soubor = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Nelze otevrit zadany soubor!\n");
        return 0;
    }

    if(fscanf(soubor, "count=%d", &pocet) != 1)
    {
        fprintf(stderr, "Spatny format prvniho radku souboru!\n");
        fclose(soubor);
        return 0;
    }

    if(pocet <= 0)
    {
        fprintf(stderr, "Pocet nacitanych objektu musi byt vetsi jak 0!\n");
        fclose(soubor);
        return 0;
    }

    *arr = malloc(pocet * sizeof(struct cluster_t));
    
    for(i=0; i<pocet; i++)
    {
         if(fscanf(soubor, "%d %f %f ", &obj.id, &obj.x, &obj.y) != 3)
        {
            fprintf(stderr, "Spatny format souboru!\n");
            for(int j=0; j<i; j++)
                clear_cluster(&(*arr)[j]);
            fclose(soubor);
            return -1;
        }
        if(obj.id <= 0 || obj.x < 0 || obj.x > 1000 || obj.y < 0 || obj.y > 1000)
        {
            fprintf(stderr, "Jeden ze zadanych objektu je neplatny!\n");
            for(int j=0; j<i; j++)
                clear_cluster(&(*arr)[j]);
            fclose(soubor);
            return -1;
        }
        init_cluster(&(*arr)[i], 1);
        append_cluster(&(*arr)[i], obj);
    }

    fclose(soubor);

    return i;

}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    char* soubor;
    char* znak;
    int pocet = 1, skutecny_pocet, i;
    int prvni, druhy;

    if(argc < 2)
    {
        fprintf(stderr, "Zadal jsi malo argumentu!\n");
        return -1;
    }

    if(argc > 3)
    {
        fprintf(stderr, "Zadal jsi prilis mnoho argumentu!\n");
        return -1;
    }

    soubor = argv[1];
    if(argc == 3)
    {
        pocet = strtol(argv[2], &znak, 10);
        if(*znak != '\0' || pocet <= 0)
        {
            fprintf(stderr, "Nezadal jsi cislo ve spravnem formatu!\n");
            return -1;
        }
    }

    skutecny_pocet = load_clusters(soubor, &clusters);

    if(skutecny_pocet == 0)
        return -1;

    if(skutecny_pocet == -1)
    {
        free(clusters);
        return -1;
    }

    if(skutecny_pocet < pocet)
    {
        fprintf(stderr, "Pocet cilovych shluku je vice nez objektu!\n");
        for(i = 0; i<skutecny_pocet; i++)
        {
            clear_cluster(&clusters[i]);
        }

        free(clusters);
        return -1;
    }

    while(skutecny_pocet != pocet)
    {
        find_neighbours(clusters, skutecny_pocet, &prvni, &druhy);
        merge_clusters(&clusters[prvni], &clusters[druhy]);
        skutecny_pocet = remove_cluster(clusters, skutecny_pocet, druhy);
    }

    print_clusters(clusters, skutecny_pocet);

    for(i = 0; i<skutecny_pocet; i++)
    {
        clear_cluster(&clusters[i]);
    }

    free(clusters);

    return 0;
}
