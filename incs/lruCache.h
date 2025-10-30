
//Autor: Sebastian Vera


#ifndef LRU_FUNCIONES_H
#define LRU_FUNCIONES_H

#include <stddef.h>
#include <stdbool.h>

#define MIN_CACHE_SIZE 5  //tamaño mínimo permitido

// Nodo doblemente enlazado: cabeza = MRU, cola = LRU
typedef struct lru_node {
    char data;                // letra mayúscula almacenada
    struct lru_node *prev;
    struct lru_node *next;
} lru_node_t;

// Estructura principal del caché
typedef struct lru_cache {
    size_t capacity;          // capacidad máxima 
    size_t size;              // elementos actuales 
    lru_node_t *head;         // MRU (más reciente) 
    lru_node_t *tail;         // LRU (menos reciente) 
} lru_cache_t;

/* Crea un caché con la capacidad dada (>= MIN_CACHE_SIZE).
 * Devuelve puntero al caché o NULL en error. */
lru_cache_t *lru_create(size_t capacity);

/* Libera toda la memoria usada por el caché. */
void lru_destroy(lru_cache_t *cache);

/* Añade o marca como usada la letra 'data'.
 * - Si existe: se mueve a MRU.
 * - Si no existe y hay espacio: se inserta en MRU.
 * - Si está lleno: se elimina LRU y se inserta en MRU.
 * Retorna 0 en éxito o -1 en error. */
int lru_add(lru_cache_t *cache, char data);

/* Usa (accede) a 'data' si existe y la mueve a MRU.
 * Retorna 0 si se encontró y movió, -1 si no existe o en error. */
int lru_get(lru_cache_t *cache, char data);

/* Busca la posición de 'data' sin cambiar prioridades.
 * Posición 0 = MRU. Devuelve índice >=0 o -1 si no se encuentra. */
int lru_search(const lru_cache_t *cache, char data);

/* Imprime el contenido del caché de MRU a LRU (no modifica nada). */
void lru_print_all(const lru_cache_t *cache);

#endif 