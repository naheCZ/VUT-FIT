/**
 * Kostra hlavickoveho souboru 3. projekt IZP 2015/16
 * a pro dokumentaci Javadoc.
 */

struct obj_t {
    /// ID objektu
    int id;
    /// Souřadnice X objektu
    float x;
    /// Souřadnice Y objektu
    float y;
};

struct cluster_t {
    /// Velikost shluku
    int size;
    /// Kapacita shluku (pro kolik objektů je alokovaná paměť)
    int capacity;
    /// Pole objektů ve shluku
    struct obj_t *obj;
};

/**
 *	@defgroup shluk Operace se shluky
 *	@addtogroup shluk
 *	@{
*/

/**
 *	Inicializace shluku
 *	@param c Shluk, který bude zinicializován
 *	@param cap Kapacita, pro kolik objektů se alokuje paměť shluku
 *  @pre c != NULL
 *  @pre cap >= 0
*/
void init_cluster(struct cluster_t *c, int cap);

/**
 *	Odstranění všech objektů shluku a inicializace na prázdný shluk
 *	@param c Shluk, ze kterého chceme odstranit objekty
 *	@post c = {0, 0, NULL}
*/
void clear_cluster(struct cluster_t *c);

/// Chunk of cluster objects. Value recommended for reallocation.
extern const int CLUSTER_CHUNK;

/**
 *	Změna kapacity shluku na novou velikost
 *	@param c Shluk, který chceme realokovat
 *	@param new_cap Nová kapacita, pro kterou se má shluk alokovat
 *  @pre c != NULL
 *  @pre c->capacity >= 0
 *  @pre new_cap >= 0
*/
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 *	Přidá objekt na konec shluku. Rozšíří shluk, pokud se do něj nevejde
 *	@param c Shluk, do kterého chceme přidat objekt
 *	@param obj Objekt, který chceme přidat do shluku
*/
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 *  Do shluku c1 přidá obejkty ze shkluku c2. Po přesunu objektů seřadí objekty ve shluku.
 *  @param c1 Shluk, do kterého chceme přidat objekty
 *  @param c2 Shluk, ze kteréhe chceme přidat objekty
 *  @pre c1 != NULL
 *  @pre c2 != NULL
*/
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 *  Odstraní shluk z pole shluků
 *  @param carr Pole shluku, ze kterého chceme shluk odstranit
 *  @param narr Počet shluků v poli
 *  @param idx Index, na kterém se nachází shluk k odstranění
 *  @pre idx < narr
 *  @pre narr > 0
 *  @post Nová velikost < narr
 *  @return Nový počet shluků v poli
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 *  Vypočítá Euklidovskou vzdálenost mezi dvěma objekty.
 *  @param o1 První objekt
 *  @param o2 Druhý objekt
 *  @pre o1 != NULL
 *  @pre o2 != NULL
 *  @return Výsledek Euklidovské vzdálenosti
*/
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 *  Na základě nejlbižšího souseda vypočítá vzdálenost dvou shluků.
 *  @param c1 První shluk
 *  @param c2 Druhý shluk
 *  @pre c1 != NULL
 *  @pre c1->size > 0
 *  @pre c2 != NULL
 *  @pre c2->size > 0
 *  @return Vzdálenost nejbližšího souseda
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/// @}

/**
 *  @defgroup pole Operace s polem shluků
 *  @addtogroup pole
 *  @{ 
*/


/**
 *  Najde dva nejbližší shluky v poli shluků pomocí metody nejbližšího souseda.
 *  @param carr Pole shluků
 *  @param narr Velikost pole shluků
 *  @param c1 První soused
 *  @param c2 Druhý soused
 *  @pre narr > 0
 *  @post 0 <= *c1 < *c2 < narr
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/// @}

/**
 *  @addtogroup shluk
 *  @{ 
*/

/**
 *  Seřadí obejkty ve shluku podle jejich id.
 *  @param c Shluk, který chceme seřadit
*/
void sort_cluster(struct cluster_t *c);

/**
 *  Na standartní výstup vytiskne shluk.
 *  @param c Shluk, který chceme vytisknout
*/
void print_cluster(struct cluster_t *c);

/// @}

/**
 *  @addtogroup pole
 *  @{ 
*/

/**
 *  Načte objekty ze souboru. Pro každý objekt vytvoří shluk a uloží jej do pole shluků.
 *  Alokuje místo pro pole shluků.
 *  @param filename Název souboru, ze kterého chceme načíst objekty.
 *  @param arr Ukazatel na pole shluků.
 *  @pre arr != NULL
 *  @return Počet načtených objektů
*/
int load_clusters(char *filename, struct cluster_t **arr);

/**
 *  Vytiskne pole shluků.
 *  @param carr Pole shluků
 *  @param narr Počet shluků v poli
*/
void print_clusters(struct cluster_t *carr, int narr);
/// @}