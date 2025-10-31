

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../incs/lruCache.h"



/*
 * Comprueba que un carácter es una letra mayúscula A-Z.
 * Parámetro: c - carácter a validar.
 * Retorno: true si es A-Z, false en caso contrario.
 */
bool lru_is_valid(char c) {
    return (c >= 'A' && c <= 'Z');
}

/*
 * 
 * Reserva e inicializa un nodo para almacenar un único carácter.
 * Parámetro: dato - letra mayúscula.
 * Retorno: puntero al nodo o NULL si dato inválido o malloc falla.
 */
lru_node_t *node_create(char dato) {
    if (!lru_is_valid(dato)) 
        return NULL;   // validar entrada
    lru_node_t *n = malloc(sizeof(lru_node_t));
    if (!n) 
        return NULL;          // manejo de fallo de malloc
    n->data = dato;
    n->prev = n->next = NULL;
    return n;
}

/*
 * Busca un nodo que contenga 'dato' recorriendo desde head (MRU).
 * Parámetros: cache - puntero al cache 
 *             dato - letra a buscar.
 * Retorno: puntero al nodo encontrado o NULL si no existe / error.
 */
lru_node_t *node_find(const lru_cache_t *cache, char dato) {
    if (!cache) 
        return NULL;
    lru_node_t *p = cache->head;
    while (p) {
        if (p->data == dato) 
            return p;
        p = p->next;
    }
    return NULL;
}

/*
 * Desconecta y devuelve el nodo LRU (tail) sin liberarlo.
 * Parámetro: cache - puntero al caché.
 * Retorno: nodo extraído o NULL si la lista está vacía / error.
 */
lru_node_t *remove_tail(lru_cache_t *cache) {
    if (!cache || !cache->tail) 
        return NULL;
    lru_node_t *old = cache->tail;
    if (old->prev) {
        cache->tail = old->prev;
        cache->tail->next = NULL;
    } else {
        // solo un elemento -> la lista queda vacía
        cache->head = cache->tail = NULL;
    }
    old->prev = old->next = NULL;

    if (cache->size > 0) 
        cache->size--;
    return old;
}


/*
 * Mueve un nodo existente a la cabeza (MRU).
 * Parámetros: cache - puntero al caché
 *             node - nodo a mover.
 * Retorno: ninguno.
 */
void move_to_head(lru_cache_t *cache, lru_node_t *node) {
    if (!cache || !node || cache->head == node) 
        return;

    // desconectar node de su posición actual
    if (node->prev) 
        node->prev->next = node->next;

    if (node->next) 
        node->next->prev = node->prev;

    // si node era tail, actualizar tail
    if (cache->tail == node) 
        cache->tail = node->prev;

    // insertar node al frente
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) 
        cache->head->prev = node;
    cache->head = node;

    // si la lista quedó sin tail (lista vacía antes), asegurar tail 
    if (!cache->tail) 
        cache->tail = node;
}

/*
 * Libera la memoria de un nodo
 * Parámetro: node - puntero al nodo a liberar
 */
void node_free(lru_node_t *node) {
    if (!node) 
        return;
    node->prev = node->next = NULL;
    free(node);
}

/*
 * Crea e inicializa una estructura de caché LRU vacía.
 * Parámetro: capacity - capacidad máxima (>= MIN_CACHE_SIZE).
 * Retorno: puntero al caché o NULL si capacity inválido o malloc falla.
 */
lru_cache_t *lru_create(size_t capacity) {
    // Validar parámetro mínimo 
    if (capacity < MIN_CACHE_SIZE)
        return NULL;                // capacidad no permitida 

    // Reservar memoria para la estructura del caché 
    lru_cache_t *cache = malloc(sizeof(lru_cache_t));
    if (!cache)
        return NULL;                // fallo de asignación 

    // Inicializar 
    cache->capacity = capacity;     // capacidad máxima 
    cache->size = 0;                // inicialmente vacío 
    cache->head = cache->tail = NULL; // lista doblemente enlazada vacía 

    // Devolver caché listo para usar
    return cache;
}

/*
 * Libera todos los nodos y la estructura del caché.
 * Parámetro: cache - puntero al caché (puede ser NULL).
 */
void lru_destroy(lru_cache_t *cache) {
    if (!cache) 
        return;

    lru_node_t *p = cache->head;
    while (p) {
        lru_node_t *sig = p->next; // guardar siguiente antes de free 
        free(p);                   // liberar nodo actual 
        p = sig;                   // continuar con el siguiente 
    }

    free(cache); // liberar la estructura del cache
}

/*
 * Inserta o marca como usada la letra 'data'.
 * Parámetros: cache - puntero al caché
 *             data - letra mayúscula.
 * Retorno: 0 en éxito, -1 en error (cache NULL, dato inválido, malloc falla).
 */
int lru_add(lru_cache_t *cache, char data) {
    // Validaciones: cache existente y dato válido (A-Z). 
    if (!cache || !lru_is_valid(data)) 
        return -1;

    // Si ya existe, lo usamos -> mover a MRU 
    lru_node_t *exist = node_find(cache, data);
    if (exist) {
        move_to_head(cache, exist); // reubica el nodo como head 
        return 0;       // éxito: no se crean ni liberan nodos 
    }

    // Si está lleno, eliminar LRU (tail) 
    if (cache->size >= cache->capacity) {
        lru_node_t *borrar = remove_tail(cache); // devuelve nodo desconectado
        if (borrar) 
            free(borrar);        // liberar su memoria 
    }

    // Crear e insertar nuevo nodo como head (MRU) 
    lru_node_t *n = node_create(data);
    if (!n) 
        return -1; //fallo al crear nodo

    // Insertar el nuevo nodo al frente (head = MRU).
    n->next = cache->head; // siguiente del nuevo = antiguo head 
    n->prev = NULL;      // nuevo head no tiene prev 

    if (cache->head) 
        cache->head->prev = n;

    cache->head = n; //actualizar head al nuevo nodo 

    //Si la lista estaba vacía, tail también debe apuntar al nuevo nodo. 
    if (!cache->tail)
        cache->tail = n;

        // Actualiza contador de elementos
    cache->size++;
    return 0;
}

/*
 * Accesa a un dato (si existe) y lo promueve a MRU.
 * Parámetros: cache - puntero al caché
 *             data - letra a acceder.
 * Retorno: 0 si se encontró y movió, -1 si no existe o error.
 */
int lru_get(lru_cache_t *cache, char data) {
    // Validaciones: existe el cache o el dato es valido
    if (!cache || !lru_is_valid(data))
        return -1;

    //Buscar el nodo que contiene 'data' (recorre desde head).
    lru_node_t *n = node_find(cache, data);
    if (!n)
        return -1; 

    // Promover el nodo encontrado a MRU (head).
    move_to_head(cache, n);

    
    return 0;
}

/*
 * Obtiene la posición de 'data' sin cambiar prioridades.
 * Parámetros: cache - puntero al caché (const), data - letra a buscar.
 * Retorno: índice >=0 (0 = MRU) si se encuentra, -1 si no existe o error.
 */
int lru_search(const lru_cache_t *cache, char data) {
    if (!cache || !lru_is_valid(data)) 
        return -1; // validaciones

    lru_node_t *p = cache->head; // comenzar en MRU
    int idx = 0;

    while (p) {
        if (p->data == data)    // encontrado: devolver índice actual
            return idx;
        p = p->next;            // avanzar al siguiente (más antiguo)
        idx++;                  // incrementar posición 
    }

    return -1; 
}
/*
 * Imprimir el contenido del caché en orden MRU -> LRU.
 * Parámetro: cache - puntero al caché (const).
 */
void lru_print_all(const lru_cache_t *cache) {
    // Si no hay cache, no imprime. (sale)
    if (!cache)
        return;

    // Comenzar en el nodo más reciente (MRU). 
    lru_node_t *p = cache->head;

    
    printf("Contenido del caché: ");

    // Si la lista está vacía
    if (!p) {
        printf("(vacío)\n");
        return;
    }

    // Recorrer e imprimir cada elemento 
    while (p) {
        printf("%c", p->data);
        if (p->next) printf(" - ");
        p = p->next;
    
    }
    printf("\n");
}
