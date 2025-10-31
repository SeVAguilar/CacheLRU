

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
    if (capacity < MIN_CACHE_SIZE) 
        return NULL;
    lru_cache_t *cache = malloc(sizeof(lru_cache_t));
    if (!cache) 
        return NULL;
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = cache->tail = NULL;
    return cache;
}

/*
 * Libera todos los nodos y la estructura del caché.
 * Parámetro: cache - puntero al caché (puede ser NULL).
 */
void lru_destroy(lru_cache_t *cache) {
    if (!cache) return;
    lru_node_t *p = cache->head;
    while (p) {
        lru_node_t *sig = p->next;
        free(p);
        p = sig;
    }
    free(cache);
}

/*
 * Inserta o marca como usada la letra 'data'.
 * Parámetros: cache - puntero al caché
 *             data - letra mayúscula.
 * Retorno: 0 en éxito, -1 en error (cache NULL, dato inválido, malloc falla).
 */
int lru_add(lru_cache_t *cache, char data) {
    if (!cache || !lru_is_valid(data)) return -1;

    /* Si ya existe, lo usamos -> mover a MRU */
    lru_node_t *exist = node_find(cache, data);
    if (exist) {
        move_to_head(cache, exist);
        return 0;
    }

    /* Si está lleno, eliminar LRU (tail) */
    if (cache->size >= cache->capacity) {
        lru_node_t *borrar = remove_tail(cache);
        if (borrar) free(borrar);
    }

    /* Crear e insertar nuevo nodo como head (MRU) */
    lru_node_t *n = node_create(data);
    if (!n) return -1;
    n->next = cache->head;
    n->prev = NULL;
    if (cache->head) cache->head->prev = n;
    cache->head = n;
    if (!cache->tail) cache->tail = n;
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
    if (!cache || !lru_is_valid(data)) 
        return -1;
    lru_node_t *n = node_find(cache, data);
    if (!n) 
        return -1;
    move_to_head(cache, n);
    return 0;
}

/*
 * Obtiene la posición de 'data' sin cambiar prioridades.
 * Parámetros: cache - puntero al caché (const), data - letra a buscar.
 * Retorno: índice >=0 (0 = MRU) si se encuentra, -1 si no existe o error.
 */
int lru_search(const lru_cache_t *cache, char data) {
    if (!cache || !lru_is_valid(data)) return -1;
    lru_node_t *p = cache->head;
    int idx = 0;
    while (p) {
        if (p->data == data) return idx;
        p = p->next;
        idx++;
    }
    return -1;
}

/*
 * Imprimir el contenido del caché en orden MRU -> LRU.
 * Parámetro: cache - puntero al caché (const).
 */
void lru_print_all(const lru_cache_t *cache) {
    if (!cache) return;
    lru_node_t *p = cache->head;
    printf("Contenido del caché: ");
    if (!p) {
        printf("(vacío)\n");
        return;
    }
    while (p) {
        printf("%c", p->data);
        if (p->next) printf(" - ");
        p = p->next;
    }
    printf("\n");
}
// 