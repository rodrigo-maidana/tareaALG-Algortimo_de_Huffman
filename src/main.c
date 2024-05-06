#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/huffman.h"

void forma_de_uso() {
    printf("\nUso: ./mi_programa [comprimir|descomprimir] <archivoEntrada> <archivoSalida>\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        forma_de_uso();
        return 1;
    }

    int result = 0;
    if (strcmp(argv[1], "comprimir") == 0) {
        result = comprimir(argv[2], argv[3]);
    } else if (strcmp(argv[1], "descomprimir") == 0) {
        result = descomprimir(argv[2], argv[3]);
    } else {
        forma_de_uso();
        return 1;
    }

    return result;
}
