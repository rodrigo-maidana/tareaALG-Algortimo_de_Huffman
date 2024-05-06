#include "../include/huffman.h"
#include "../include/pq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct NodoHuffman {
    char caracter;
    unsigned int frecuencia;
    struct NodoHuffman *izquierda, *derecha;
} NodoHuffman;

// Prototipos de funciones auxiliares
NodoHuffman* nuevo_nodo(char caracter, unsigned int frecuencia);
void calcular_frecuencias(const char *filename, unsigned int *frecuencias);
NodoHuffman* construir_arbol_huffman(unsigned int *frecuencias);
void generar_codigos_huffman(NodoHuffman *nodo, char *codigo, int top, char **codigos);
void liberar_arbol_huffman(NodoHuffman *nodo);
void escribir_arbol(NodoHuffman *nodo, FILE *output);
NodoHuffman* leer_arbol(FILE *input);
void escribir_codificado(const char *filename, char **codigos, FILE *output);
void decodificar_archivo(NodoHuffman *raiz, FILE *input, FILE *output);

void escribir_arbol(NodoHuffman *nodo, FILE *output) {
    if (!nodo) return;
    if (nodo->izquierda || nodo->derecha) {
        fputc('0', output); // Indica un nodo interno
        escribir_arbol(nodo->izquierda, output);
        escribir_arbol(nodo->derecha, output);
    } else {
        fputc('1', output); // Indica un nodo hoja
        fputc(nodo->caracter, output); // Escribe el carácter
    }
}

NodoHuffman* leer_arbol(FILE *input) {
    int bit = fgetc(input);
    if (bit == '0') { // Nodo interno
        NodoHuffman *left = leer_arbol(input);
        NodoHuffman *right = leer_arbol(input);
        NodoHuffman *nodo = nuevo_nodo('\0', 0);
        nodo->izquierda = left;
        nodo->derecha = right;
        return nodo;
    } else if (bit == '1') { // Nodo hoja
        char caracter = fgetc(input);
        return nuevo_nodo(caracter, 0);
    }
    return NULL;
}

void escribir_codificado(const char *filename, char **codigos, FILE *output) {
    FILE *input = fopen(filename, "rb");
    if (!input) {
        perror("Error abriendo el archivo de entrada para codificar");
        return;
    }

    uint8_t buffer = 0; // Almacena los bits que vamos a escribir
    int bit_count = 0; // Cuenta cuántos bits tenemos en el buffer

    int c;
    while ((c = fgetc(input)) != EOF) {
        char *codigo = codigos[c]; // Obtén el código Huffman para el caracter
        for (int i = 0; codigo[i] != '\0'; i++) {
            // Coloca cada bit en el buffer
            buffer = (buffer << 1) | (codigo[i] == '1'); // Desplaza el buffer un bit a la izquierda, añade el bit actual
            bit_count++; // Incrementa el contador de bits

            if (bit_count == 8) { // Si el buffer está lleno
                fwrite(&buffer, 1, 1, output); // Escribe el buffer en el archivo
                buffer = 0; // Resetea el buffer
                bit_count = 0; // Resetea el contador de bits
            }
        }
    }

    // Si queda algún bit en el buffer que no se haya escrito
    if (bit_count > 0) {
        buffer <<= (8 - bit_count); // Desplaza los bits a la izquierda
        fwrite(&buffer, 1, 1, output); // Escribe los últimos bits
    }

    fclose(input);
}

void decodificar_archivo(NodoHuffman *raiz, FILE *input, FILE *output) {
    NodoHuffman *current = raiz;
    uint8_t buffer;
    int bit_count = 0;
    int bit;

    while (fread(&buffer, 1, 1, input) == 1) { // Lee un byte del archivo
        for (bit_count = 0; bit_count < 8; bit_count++) { // Procesa cada bit del byte
            bit = (buffer >> (7 - bit_count)) & 1; // Extrae el bit actual
            current = (bit == 0) ? current->izquierda : current->derecha; // Navega en el árbol de Huffman
            
            if (!current->izquierda && !current->derecha) { // Si es un nodo hoja
                fputc(current->caracter, output); // Escribe el caracter correspondiente al archivo
                current = raiz; // Reinicia al nodo raíz para el próximo carácter
            }
        }
    }
}


int comprimir(const char *input_file, const char *output_file) {
    unsigned int frecuencias[256] = {0};
    calcular_frecuencias(input_file, frecuencias);

    NodoHuffman *raiz = construir_arbol_huffman(frecuencias);
    if (!raiz) {
        fprintf(stderr, "Error al construir el árbol de Huffman\n");
        return 1;
    }

    char *codigos[256];
    char codigo[256];
    int i;
    for (i = 0; i < 256; i++) {
        codigos[i] = (char*)malloc(256 * sizeof(char));
        codigos[i][0] = '\0';
    }
    generar_codigos_huffman(raiz, codigo, 0, codigos);

    FILE *output = fopen(output_file, "wb");
    if (!output) {
        perror("No se pudo abrir el archivo de salida");
        return 1;
    }
    escribir_arbol(raiz, output);
    escribir_codificado(input_file, codigos, output);
    fclose(output);

    liberar_arbol_huffman(raiz);
    for (i = 0; i < 256; i++) free(codigos[i]);
    return 0;
}

int descomprimir(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "rb");
    if (!input) {
        perror("No se pudo abrir el archivo de entrada");
        return 1;
    }

    NodoHuffman *raiz = leer_arbol(input);
    if (!raiz) {
        fprintf(stderr, "Error al leer el árbol de Huffman\n");
        return 1;
    }

    FILE *output = fopen(output_file, "wb");
    if (!output) {
        perror("No se pudo abrir el archivo de salida");
        fclose(input);
        return 1;
    }

    decodificar_archivo(raiz, input, output);
    fclose(input);
    fclose(output);

    liberar_arbol_huffman(raiz);
    return 0;
}

void calcular_frecuencias(const char *filename, unsigned int *frecuencias) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error abriendo el archivo para calcular frecuencias");
        return;
    }
    int c;
    while ((c = fgetc(file)) != EOF) {
        frecuencias[c]++;
    }
    fclose(file);
}

NodoHuffman* construir_arbol_huffman(unsigned int *frecuencias) {
    PQ pq = pq_create();
    for (int i = 0; i < 256; i++) {
        if (frecuencias[i] > 0) {
            pq_add(pq, nuevo_nodo(i, frecuencias[i]), frecuencias[i]);
        }
    }
    NodoHuffman *left, *right, *parent;
    void* temp;
    while (pq_size(pq) > 1) {
        if (!pq_remove(pq, &temp)) {
            fprintf(stderr, "Error al remover de la cola de prioridades\n");
            return NULL;
        }
        left = (NodoHuffman*) temp;

        if (!pq_remove(pq, &temp)) {
            fprintf(stderr, "Error al remover de la cola de prioridades\n");
            return NULL;
        }
        right = (NodoHuffman*) temp;

        parent = nuevo_nodo('\0', left->frecuencia + right->frecuencia);
        parent->izquierda = left;
        parent->derecha = right;
        pq_add(pq, parent, parent->frecuencia);
    }
    if (!pq_remove(pq, &temp)) {
        fprintf(stderr, "Error al remover de la cola de prioridades\n");
        return NULL;
    }
    return (NodoHuffman*) temp;
}


NodoHuffman* nuevo_nodo(char caracter, unsigned int frecuencia) {
    NodoHuffman *nodo = (NodoHuffman *)malloc(sizeof(NodoHuffman));
    if (!nodo) return NULL;
    nodo->caracter = caracter;
    nodo->frecuencia = frecuencia;
    nodo->izquierda = nodo->derecha = NULL;
    return nodo;
}

void generar_codigos_huffman(NodoHuffman *nodo, char *codigo, int top, char **codigos) {
    if (nodo->izquierda) {
        codigo[top] = '0';
        generar_codigos_huffman(nodo->izquierda, codigo, top + 1, codigos);
    }
    if (nodo->derecha) {
        codigo[top] = '1';
        generar_codigos_huffman(nodo->derecha, codigo, top + 1, codigos);
    }
    if (!nodo->izquierda && !nodo->derecha) {
        codigo[top] = '\0';
        strcpy(codigos[(unsigned char)nodo->caracter], codigo);
    }
}

void liberar_arbol_huffman(NodoHuffman *nodo) {
    if (nodo != NULL) {
        liberar_arbol_huffman(nodo->izquierda);
        liberar_arbol_huffman(nodo->derecha);
        free(nodo);
    }
}

// Implementa aquí las funciones auxiliares...
