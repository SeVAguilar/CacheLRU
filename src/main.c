#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../incs/lruCache.h"

#define LINE_SZ 128

// Muestra un menú breve con los comandos y su uso.
static void print_menu() {
    puts("Comandos disponibles:");
    puts("  create <N>    - Crear/reescribir caché con capacidad N (N >= 5)");
    puts("  add <A>       - Añadir o usar letra mayúscula A");
    puts("  get <A>       - Promover letra A a MRU si existe");
    puts("  search <A>    - Imprimir índice de A (0 = MRU) o -1 si no existe");
    puts("  all           - Mostrar contenido (MRU -> LRU)");
    puts("  tutorial      - Mostrar ejemplo de uso");
    puts("  exit          - Salir");
}

// Imprime un tutorial paso a paso 
static void print_tutorial() {
    puts("Tutorial - uso básico del CLI LRU");
    puts("");
    puts("1) Crear el caché:");
    puts("   Comando:  create 5");
    puts("   Descripción: crea un caché con capacidad 5 (mínimo permitido).");
    puts("");
    puts("2) Añadir elementos (cada 'add' pone el elemento como MRU):");
    puts("   Comandos:");
    puts("     add A");
    puts("     add B");
    puts("     add C");
    puts("   Descripción: tras los comandos, el orden MRU->LRU será C - B - A.");
    puts("");
    puts("3) Acceder a un elemento (get) mueve ese elemento a MRU:");
    puts("   Comando:  get B");
    puts("   Descripción: si B existe, pasa a ser MRU. Usar 'all' para ver el cambio.");
    puts("");
    puts("4) Insertar cuando está lleno expulsa el LRU:");
    puts("   Ejemplo:");
    puts("     create 3");
    puts("     add A");
    puts("     add B");
    puts("     add C");
    puts("     add D   # ahora C,B,A -> al añadir D, se expulsará A (LRU)");
    puts("   Descripción: siempre se elimina el elemento menos reciente (LRU).");
    puts("");
    puts("5) Buscar posición sin cambiar prioridades:");
    puts("   Comando:  search C");
    puts("   Salida: un número >=0 indicando la posición (0 = MRU) o -1 si no existe.");
    puts("");
    puts("6) Inspeccionar contenido:");
    puts("   Comando:  all");
    puts("   Descripción: imprime el contenido en orden MRU -> LRU.");
    puts("");
    puts("7) Salir:");
    puts("   Comando:  exit");
    puts("");
}

int main() {
    // Declara un buffer para leer cada línea de entrada del usuario.
    char line[LINE_SZ]; 
    // Puntero al caché
    // inicialmente no existe (NULL) hasta que se ejecute create.
    lru_cache_t *cache = NULL;

    // Muestra un menu al iniciar
    print_menu();


    while (1) { //Bucle principal que procesa líneas
                // hasta break/EOF.
        // Lee una línea con fgets. Si fgets falla (EOF), sale del bucle.
        if (!fgets(line, sizeof(line), stdin))
            break;
        // Elimina salto de línea final (strcspn).
        line[strcspn(line, "\r\n")] = '\0';

        //Tokeniza la línea con strtok(line, " \t") 
        //Si la línea está vacía se continúa a la siguiente iteración
        char *cmd = strtok(line, " \t");
        if (!cmd) 
            continue;

        if (strcmp(cmd, "menu") == 0 || strcmp(cmd, "help") == 0) {
            print_menu();
            continue;
        }
        if (strcmp(cmd, "tutorial") == 0) {
            print_tutorial();
            continue;
        }

        // create <N> 
        // Maneja el comando: create <N>. Crea el caché con capacidad N.
        if (strcmp(cmd, "create") == 0) {
        // Toma el token como argumento y verifica si es "create"

            char *arg = strtok(NULL, " \t");
            //Toma el siguiente token (el argumento N). 

            if (!arg){
            // Si falta el argumento, ignora la línea y sigue esperando comandos.
                puts("Uso: create <N>");
                continue;
            }

            // Convierte la cadena `arg` a un entero de tipo long en base 10.
            // Si `arg` no empieza por un número devuelve 0
            long n = strtol(arg, NULL,10);

            if (n < MIN_CACHE_SIZE){
            //Rechaza tamaños menores que el mínimo (MIN_CACHE_SIZE). 
            //Si n menor que el mínimo, ignora la entrada.
                printf("Error: el tamaño debe ser >= %d\n", MIN_CACHE_SIZE);
                continue;
            }

            // Crear primero y comprobar exito antes de destruir el anterior
            lru_cache_t *new_cache = lru_create((size_t)n);
            if (!new_cache) {
                puts("Error: no se pudo crear el caché (malloc fallo).");
                continue;
            }
            if (cache){
            // Si ya existe un caché previo, lo libera 
                lru_destroy(cache);
            }

            cache = new_cache;
            printf("Caché creado con capacidad %ld\n", n);

            continue;
            // Se procesa el comando y pasa a la siguiente iteracion
        }

        // add <A>
        // Maneja el comando: add <A>. Añade la letra A al cache o la marca como usada
        if (strcmp(cmd, "add") == 0) {
        // Toma el token como argumento y verifica si es "add"

            char *arg = strtok(NULL, " \t");
            //Toma el siguiente token (el argumento). 

            if (!arg || strlen(arg) != 1 || !cache){
            // Si no hay argumento, si el token no es exactamente 1 carácter, 
            //o si no existe caché, el comando se ignora.
                puts("Uso: add <A>  (una sola letra)");
                continue;
            }

            if (!cache) {
            // Comprueba si existe el cache
                puts("Primero cree el caché con 'create <N>'");
                continue;
            }

            char c = (char)toupper((unsigned char)arg[0]);
            // Convierte el caracter a mayuscula

            if (c < 'A' || c > 'Z'){
            // Valida que el caracter sea valido (A,Z)
                puts("Dato inválido: usar letra A-Z");
                continue;
            }

            lru_add(cache, c);
            // Agrega al cache y expulsa LRU si esta lleno

            continue;
            // Se procesa el comando y pasa a la siguiente iteracion

        }

        // get <A> 
        // Maneja el comando: get <A>.  Promueve el elemento a MRU si existe.
        if (strcmp(cmd, "get") == 0) {
        // Toma el token como argumento y verifica si es "get"

            char *arg = strtok(NULL, " \t");
            // Toma el siguiente token (el argumento esperado: una sola letra).

            if (!arg || strlen(arg) != 1 || !cache){
            // Si falta el argumento, si no es exactamente 1 carácter,
            // o si no hay caché creado, se ignora el comando.
                puts("Uso: get <A>  (una sola letra)");
                continue;
            }

            char c = (char)toupper((unsigned char)arg[0]);
            // Convierte el carácter a mayúscula

            if (c < 'A' || c > 'Z') {
            // Valida que el caracter sea valido (A,Z)
                puts("Dato inválido: usar letra A-Z");
                continue;
            }

            if (lru_get(cache, c) == 0) {
                printf("Dato %c promovido a MRU\n", c);
            } else {
                printf("Dato %c no encontrado\n", c);
            }

            continue;
            // Comando procesado, continuar con la siguiente iteracion.
        }

        // search <A> 
        // Maneja el comando: search <A>. Imprime la posición (0 = MRU) o -1 si no existe.
        if (strcmp(cmd, "search") == 0) {
            char *arg = strtok(NULL, " \t");
            // Toma el siguiente token (el argumento esperado: una sola letra).

            if (!arg || strlen(arg) != 1 || !cache){
            // Si falta el argumento, si no es exactamente 1 carácter,
            // o si no hay caché creado, se ignora el comando.
                puts("Uso: get <A>  (una sola letra)");
                continue;
            }

            char c = (char)toupper((unsigned char)arg[0]);
            // Convierte el carácter a mayúscula

            if (c < 'A' || c > 'Z') {
            // Valida que el caracter sea valido (A,Z)
                puts("Dato inválido: usar letra A-Z");
                continue;
            }

            // Llama a la función que devuelve la posición sin cambiar prioridades.
            int pos = lru_search(cache, c);
            // Imprime la posición (o -1 si no se encontró).
            printf("%d\n", pos);
            continue;
        }

         // all 
        // Maneja el comando: all. Imprime el contenido del caché en orden MRU -> LRU.
        if (strcmp(cmd, "all") == 0) {
            // Si no se ha creado el cache aun, no hay nada que imprimir.
            if (!cache) 
                continue;

            // Llama a la función que recorre e imprime todos los elementos
            // desde el más reciente (MRU) hasta el menos reciente (LRU).
            lru_print_all(cache);
            continue;
        }

        // exit 
        // Maneja el comando: exit. Sale del bucle principal para terminar el programa.
        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        
    }

    if (cache) 
    // Si el cache existe libera la memoria
        lru_destroy(cache);
    return 0;
}