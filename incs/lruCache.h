
//Autor: Sebastian Vera


#ifndef LRU_FUNCIONES_H
#define LRU_FUNCIONES_H

#include <stddef.h>
#include <stdbool.h>

#define MIN_CACHE_SIZE 5  //tamaño mínimo permitido

// Nodo doblemente enlazado: cabeaza -> head = MRU, cola  -> tail = LRU
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

/*
 * Crea una nueva caché LRU con la capacidad indicada.
 * Parámetros:
 *  - capacity: Capacidad máxima de la caché (>= MIN_CACHE_SIZE)
 * Retorna:
 *  - Puntero a la caché creada, o NULL si ocurre un error.
 */
lru_cache_t *lru_create(size_t capacity);
/*
 * Elimina todos los nodos y libera la estructura lru_cache_t.
 * Parámetros:
 *   - cache: puntero al caché. Puede ser NULL (en ese caso no hace nada).
 * Retorno:
 *   - Nada.
 */
void lru_destroy(lru_cache_t *cache);
/* 
 *Inserta un dato en el caché o lo marca como usado si ya existe.
 * Parámetros:
 *   - cache: puntero al cache.
 *   - data: letra mayúscula a insertar. Se marca como usada.
 * Retorno:
 *   - 0 en éxito (insertado o movido).
 *   - -1 en error (cache NULL, dato inválido o fallo de memoria).
 */
int lru_add(lru_cache_t *cache, char data);
/*
 *Indica que un elemento fue usado sin eliminarlo, actualizar su prioridad.
 * Parámetros:
 *   - cache: puntero al caché.
 *   - data: letra mayúscula a buscar/usar.
 * Retorno:
 *   - 0 si el dato fue encontrado y movido a MRU.
 *   - -1 si no se encontró o en caso de error (cache NULL o dato inválido).
 */
int lru_get(lru_cache_t *cache, char data);
/* 
 *Localiza la posición de un elemento para inspección o verificación.
 * Parámetros:
 *   - cache: puntero al caché (const).
 *   - data: letra a buscar.
 * Retorno:
 *   - índice >= 0 (0 = MRU) si se encuentra.
 *   - -1 si no se encuentra o en caso de error.
 */
int lru_search(const lru_cache_t *cache, char data);
/* 
 *Muestra el estado actual del caché para depuración o verificación.
 * Parámetros:
 *   - cache: puntero al caché (const).
 * Retorno:
 *   - Ninguno.
 */
void lru_print_all(const lru_cache_t *cache);
/* 
 * Valida que un carácter sea una letra mayúscula A-Z.
 * Parámetros:
 *   - c: caracter a validar.
 * Retorno:
 *   - true si c está entre 'A' y 'Z', false en caso contrario.
 */
bool lru_is_valid(char c);
/* 
 *Crea un nodo nuevo con el dato dado.
 * Parámetros:
 *   - dato: letra mayúscula para almacenar.
 * Retorno:
 *   - puntero a lru_node_t recién asignado, o NULL si dato inválido o malloc falla.
 */
lru_node_t *node_create(char dato);
/*
 *Busca un nodo por su dato en el caché (sin modificar la lista).
 * Parámetros:
 *   - cache: puntero al caché.
 *   - dato: letra a buscar.
 * Retorno:
 *   - puntero al nodo si se encuentra, NULL si no existe o error.
 */
lru_node_t *node_find(const lru_cache_t *cache, char dato);
/*
 *Extrae el nodo tail (LRU) de la lista y actualiza cache->size.
 * Parámetros:
 *   - cache: puntero al caché.
 * Retorno:
 *   - puntero al nodo extraído o NULL si la lista está vacía o error.
 */
lru_node_t *remove_tail(lru_cache_t *cache);
/*
 *Mueve un nodo ya existente a la cabeza (MRU).
 * Parámetros:
 *   - cache: puntero al caché.
 *   - node: nodo que ya pertenece a la lista (debe ser no-NULL).
 * Retorno:
 *   - Ninguno.
 */
void move_to_head(lru_cache_t *cache, lru_node_t *node);
/*
 *Libera la memoria de un nodo creado por node_create o devuelto por remove_tail.
 * Parámetros:
 *   - node: puntero al nodo a liberar (puede ser NULL, en cuyo caso no hace nada).
 * Retorno:
 *   - Ninguno.
 */
void node_free(lru_node_t *node);


#endif 