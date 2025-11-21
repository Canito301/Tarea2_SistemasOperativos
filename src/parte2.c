#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Usamos flags en los dos bits menos significativos
#define FLAG_VALIDA  (1U << 0)  // bit 0: página válida
#define FLAG_USO     (1U << 1)  // bit 1: bit de uso para reloj


//El puntero es para decir en que parte del algoritmo está, y que no inicie en 0 en cada llamada
uint32_t reemplazo_reloj(uint32_t *arreglo_marcos, uint32_t num_marcos, uint32_t *puntero, uint32_t *tabla_paginas) {
    while (1) {
        uint32_t npv = arreglo_marcos[*puntero];
        uint32_t entrada = tabla_paginas[npv];
        
        if (!(entrada & FLAG_USO)) {
            // Encontramos marco sin bit de uso
            uint32_t marco = *puntero;
            tabla_paginas[npv] = 0;  // se invalida 
            *puntero = (*puntero + 1) % num_marcos;
            return marco;
        }
        tabla_paginas[npv] = entrada & ~FLAG_USO;
        *puntero = (*puntero + 1) % num_marcos;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s Nmarcos tamaño_marco [--verbose] archivo_traza.txt\n", argv[0]);
        return 1;
    }

    uint32_t num_marcos = atoi(argv[1]);
    uint32_t tamano_pagina = atoi(argv[2]);
    int verbose = 0;
    char *archivo_traza;

    if (argc == 5 && strcmp(argv[3], "--verbose") == 0) {
        verbose = 1;
        archivo_traza = argv[4];
    } else {
        archivo_traza = argv[3];
    }

    // Calcular bits de desplazamiento
    uint32_t b = 0;
    uint32_t temp = tamano_pagina;
    while (temp > 1) {
        temp >>= 1;
        b++;
    }
    uint32_t mascara = (1U << b) - 1;

    // encontrar el NPV máximo para dimensionar la tabla (memoria)
    FILE *archivo = fopen(archivo_traza, "r");
    if (!archivo) {
        perror("Error al abrir archivo");
        return 1;
    }

    uint32_t npv_max = 0;
    uint32_t num_referencias = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), archivo)) {
        uint32_t dv;
        if (strncmp(linea, "0x", 2) == 0 || strncmp(linea, "0X", 2) == 0) {
            dv = strtoul(linea, NULL, 16);
        } else {
            dv = atoi(linea);
        }
        uint32_t npv = dv >> b;
        if (npv > npv_max) npv_max = npv;
        num_referencias++;
    }
    fclose(archivo);

    // Tabla de páginas con tamaño exacto
    uint32_t capacidad = npv_max + 1;
    uint32_t *tabla_paginas = calloc(capacidad, sizeof(uint32_t));
    
    // Arreglo de marcos para algoritmo reloj
    uint32_t *arreglo_marcos = malloc(num_marcos * sizeof(uint32_t));
    uint32_t proximo_libre = 0;
    uint32_t puntero_reloj = 0;

    // procesar la traza
    archivo = fopen(archivo_traza, "r");
    if (!archivo) {
        perror("Error al abrir archivo");
        return 1;
    }

    uint32_t referencias = 0;
    uint32_t fallos_pagina = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        uint32_t dv;
        if (strncmp(linea, "0x", 2) == 0 || strncmp(linea, "0X", 2) == 0) {
            dv = strtoul(linea, NULL, 16);
        } else {
            dv = atoi(linea);
        }

        uint32_t offset = dv & mascara;
        uint32_t npv = dv >> b;
        uint32_t acierto = 0;
        uint32_t marco;

        uint32_t entrada = tabla_paginas[npv];
        
        if (entrada & FLAG_VALIDA) {
            acierto = 1;
            marco = entrada >> 2;  // extraer marco corriendo 2 bits a la derecha
            tabla_paginas[npv] = entrada | FLAG_USO;  // bit de uso
        } else {
            fallos_pagina++;
            
            if (proximo_libre < num_marcos) {
                marco = proximo_libre;
                arreglo_marcos[marco] = npv;
                proximo_libre++;
            } else {
                marco = reemplazo_reloj(arreglo_marcos, num_marcos, &puntero_reloj, tabla_paginas);
                arreglo_marcos[marco] = npv;
            }

            // Guardar marco y flags, altos y bajos
            tabla_paginas[npv] = (marco << 2) | FLAG_VALIDA | FLAG_USO;
        }

        uint32_t df = (marco << b) | offset;
        referencias++;

        if (verbose) {
            printf("DV: 0x%08x, NPV: %u, Offset: 0x%x, %s, Marco: %u, DF: 0x%08x\n",
                   dv, npv, offset, acierto ? "HIT" : "FALLO", marco, df);
        }
    }
    fclose(archivo);

    printf("\n=== Resultados ===\n");
    printf("Referencias: %u\n", referencias);
    printf("Fallos de página: %u\n", fallos_pagina);
    printf("Tasa de fallos: %.2f%%\n", 
           referencias > 0 ? (100.0 * fallos_pagina / referencias) : 0.0);

    free(tabla_paginas);
    free(arreglo_marcos);

    return 0;
}
