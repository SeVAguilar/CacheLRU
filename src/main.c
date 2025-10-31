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
        if (strcmp(cmd, "create") == 0) {
            char *arg = strtok(NULL, " \t");
            if (!arg) 
                continue;
            long n = strtol(arg, NULL, 10);
            if (n < MIN_CACHE_SIZE) 
                continue;
            if (cache) 
                lru_destroy(cache);
            cache = lru_create((size_t)n);
            continue;
        }

        // add <A>
        if (strcmp(cmd, "add") == 0) {
            char *arg = strtok(NULL, " \t");
            if (!arg || strlen(arg) != 1 || !cache) 
                continue;
            char c = (char)toupper((unsigned char)arg[0]);
            if (c < 'A' || c > 'Z') 
                continue;
            lru_add(cache, c);
            continue;
        }

        // get <A> 
        if (strcmp(cmd, "get") == 0) {
            char *arg = strtok(NULL, " \t");
            if (!arg || strlen(arg) != 1 || !cache) 
                continue;
            char c = (char)toupper((unsigned char)arg[0]);
            if (c < 'A' || c > 'Z') 
                continue;
            lru_get(cache, c);
            continue;
        }

        // search <A> 
        if (strcmp(cmd, "search") == 0) {
            char *arg = strtok(NULL, " \t");
            if (!arg || strlen(arg) != 1 || !cache) { 
                puts("-1"); 
                continue; 
            }
            char c = (char)toupper((unsigned char)arg[0]);
            if (c < 'A' || c > 'Z') { 
                puts("-1"); 
                continue; 
            }
            int pos = lru_search(cache, c);
            printf("%d\n", pos);
            continue;
        }

        // all 
        if (strcmp(cmd, "all") == 0) {
            if (!cache) 
                continue;
            lru_print_all(cache);
            continue;
        }

        // exit 
        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        
    }

    if (cache) 
        lru_destroy(cache);
    return 0;
}