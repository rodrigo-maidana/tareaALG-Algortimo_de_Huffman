
#include "huffman.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbol.h"
#include "pq.h"
#include "bitstream.h"
#include "confirm.h"

/*====================================================
     Constantes
  ====================================================*/

#define NUM_CHARS 256

/*
estructura para almacenar valores de un nodo de un arbol, 
c es el caracter
frec es la frecuencia
*/
typedef struct _keyvaluepair {
    char c;
    int frec;

} keyvaluepair;

/*====================================================
     Campo de bits.. agrega funciones si quieres
     para facilitar el procesamiento de bits.
  ====================================================*/

typedef struct _campobits {
    unsigned int bits;
    int tamano;
} campobits;

/* Esto utiliza aritmetica de bits para agregar un
   bit a un campo.
   
   Supongamos que bits->bits inicialmene es (en binario):
   
      000110001000
      
   y le quiero agregar un 1 en vez del segundo 0 (desde izq).
   Entonces, creo una "mascara" de la siguiente forma:
   
      1 << 11   me da 0100000000000

   Y entonces si juntamos los dos (utilizando OR binario):      
      000110001000
    | 0100000000000
    ----------------
      010110001000

    Esta funcion utiliza bits->tamano para decidir donde colocar
    el siguiente bit.
    
    Nota: asume que bits->bits esta inicialmente inicializado a 0,
    entonces agregar un 0, no requiere mas que incrementar bits->tamano.
*/
      
static void bits_agregar(campobits* bits, int bit) {
    CONFIRM_RETURN(bits);
    CONFIRM_RETURN((unsigned int)bits->tamano < 8*sizeof(bits->bits));
    bits->tamano++;
    if (bit) {
        bits->bits = bits->bits | ( 0x1 << (bits->tamano-1));
    } 
}
/*
    funcion de utilidad para leer un bit dentro de campobits dado el indice pos
    pos = 0, primer bit 
    pos = 1, segundo bit
    pos = 2, tercer bit
    pos = k, k bit

*/
static int bits_leer(campobits* bits, int pos) {
    CONFIRM_TRUE(bits,0);
    CONFIRM_TRUE(!(pos < 0 || pos > bits->tamano),0);
    // para saber si campobits tiene un 1 o 0 en la posicion dada 
    // recorro bits usando shift << y >>
    int bit = (bits->bits & (0x1 << (pos))) >> (pos);
    return bit;
}



static void testCampobitsBitstream() {
    BitStream bs = NULL;
    BitStream bsIn = NULL;
    char* testbitstreamtxt = "testbitsteam.txt";
    int i = 0;
    // crear un campobits
    campobits* b = (campobits*)malloc(sizeof(struct _campobits));
    CONFIRM_RETURN(b);
    b->bits = 0;
    b->tamano = 0;
    // ej quiero codificar 00101 
    bits_agregar(b, 0);
    bits_agregar(b, 0);
    bits_agregar(b, 1);
    bits_agregar(b, 0);
    bits_agregar(b, 1);
    // crear un archivo y escribir bit a bit
    bs = OpenBitStream(testbitstreamtxt, "w");
    // para escribir en un archivo PutBit agrega bits 
    // ej recorro el campobits y agrego bit a bit
    for (i = 0; i < b->tamano; i++) {
        int bit = bits_leer(b, i);
        PutBit(bs, bit);
    }
    // PutByte para escribir 1 byte completo, 
    // ej agrego un caracter
    PutByte(bs, 'z');
    // no olvidar cerrar el archvio
    CloseBitStream(bs);
    // y liberar memoria utilizada
    free(b);
    // Mi archivo entonces contiene: 00101z
    // si quiero leer el mismo
    bsIn = OpenBitStream(testbitstreamtxt, "r");
    // leer bit a bit
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    printf("%d", GetBit(bsIn));
    // leer un byte
    printf("%c\n", GetByte(bsIn));
    CloseBitStream(bsIn);



}
static void imprimirNodoEjemplo(Arbol nodo) {
    CONFIRM_RETURN(nodo);
    keyvaluepair* val = (keyvaluepair*)arbol_valor(nodo);
    printf("%d,%c\n", val->frec, val->c);
}
static void testArbol() {

    /* si quiero crear un arbol que contiene:
    char freq
    a    4
    s    2
    entonces tengo un nodo padre
    con freq 6 y dos hijos, con freq 2  y 4 con sus caracteres correspondientes
    */
    keyvaluepair* v1 = malloc(sizeof(struct _keyvaluepair));
    keyvaluepair* v2 = malloc(sizeof(struct _keyvaluepair));
    keyvaluepair* v3 = malloc(sizeof(struct _keyvaluepair));
    Arbol n1;
    Arbol n2;
    Arbol n3;
    CONFIRM_RETURN(v1);
    CONFIRM_RETURN(v2);
    CONFIRM_RETURN(v3);
    v1->c = 'a';
    v1->frec = 4;
    v2->c = 's';
    v2->frec = 2;
    v3->c = ' ';
    v3->frec = 6;
    n1 = arbol_crear(v1);
    n2 = arbol_crear(v2);
    n3 = arbol_crear(v3);
    arbol_agregarIzq(n3, n2);
    arbol_agregarDer(n3, n1);
    //ejemplo de recorrer el arbol e imprimir nodos con valor keyvaluepair
    arbol_imprimir(n3, imprimirNodoEjemplo);
    arbol_destruir(n3);
    free(v1);
    free(v2);
    free(v3);
}


/*====================================================
     Declaraciones de funciones 
  ====================================================*/

/* Puedes cambiar esto si quieres.. pero entiende bien lo que haces */
static int calcular_frecuencias(int* frecuencias, char* entrada);
static Arbol crear_huffman(int* frecuencias);
static int codificar(Arbol T, char* entrada, char* salida);
static void crear_tabla(campobits* tabla, Arbol T, unsigned int bits, int tamano);

static Arbol leer_arbol(BitStream bs);
static void decodificar(BitStream in, BitStream out, Arbol arbol);

static void imprimirNodo(Arbol nodo);

/*====================================================
     Implementacion de funciones publicas
  ====================================================*/



void campobitsDemo() {
    printf("***************CAMPOBITS DEMO*******************\n");
    testCampobitsBitstream();
    printf("***************ARBOL DEMO******************\n");
    testArbol();
    printf("***************FIN DEMO*****************\n");
}

/*
  Comprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int comprimir(char* entrada, char* salida) {
    
    /* 256 es el numero de caracteres ASCII.
       Asi podemos utilizar un unsigned char como indice.
     */
    int frecuencias[NUM_CHARS]; 
    Arbol arbol = NULL;
    /* Primer recorrido - calcular frecuencias */
    CONFIRM_TRUE(0 == calcular_frecuencias(frecuencias, entrada), 0);
            
    arbol = crear_huffman(frecuencias);
    arbol_imprimir(arbol, imprimirNodo); 

    /* Segundo recorrido - Codificar archivo */
    CONFIRM_TRUE(0 == codificar(arbol, entrada, salida), 0);
    
    arbol_destruir(arbol);
    
    return 0;
}


/*
  Descomprime archivo entrada y lo escriba a archivo salida.
  
  Retorna 0 si no hay errores.
*/
int descomprimir(char* entrada, char* salida) {

    BitStream in = 0;
    BitStream out = 0;
    Arbol arbol = NULL;
        
    /* Abrir archivo de entrada */
    in = OpenBitStream(entrada, "r");
    
    /* Leer Arbol de Huffman */
    arbol = leer_arbol(in);
    arbol_imprimir(arbol, imprimirNodo);

    /* Abrir archivo de salida */
    out = OpenBitStream(salida, "w");
    
    /* Decodificar archivo */
    decodificar(in, out, arbol);
    
    CloseBitStream(in);
    CloseBitStream(out);
    return 0;
}

/*====================================================
     Funciones privadas
  ====================================================*/

/* Devuelve 0 si no hay errores */
static int calcular_frecuencias(int* frecuencias, char* entrada) {
    // Inicializa todas las frecuencias a cero
    memset(frecuencias, 0, sizeof(int) * 256); // Asumimos que hay 256 caracteres posibles (ASCII extendido)

    // Recorre cada carácter en la cadena de entrada
    while (*entrada != '\0') {
        unsigned char index = *entrada; // Convertir a unsigned char para manejar caracteres extendidos
        frecuencias[index]++; // Incrementa la frecuencia del carácter correspondiente
        entrada++; // Mueve el puntero al siguiente carácter
    }

    return 0; // Retorna 0 indicando que no hubo errores
}



/* Crea el arbol huffman en base a las frecuencias dadas */
static Arbol crear_huffman(int* frecuencias) {
    PQ pq = pq_create();
    if (pq == NULL) {
        return NULL; // Error al crear la cola de prioridades
    }

    // Crear un nodo para cada carácter con frecuencia no nula y agregarlo a la cola de prioridades
    for (int i = 0; i < 256; i++) {
        if (frecuencias[i] > 0) {
            keyvaluepair* kvp = malloc(sizeof(keyvaluepair));
            if (kvp == NULL) {
                pq_destroy(pq);
                return NULL; // Error de memoria
            }
            kvp->c = (char)i;
            kvp->frec = frecuencias[i];
            Arbol nodo = arbol_crear(kvp);
            if (nodo == NULL) {
                free(kvp);
                pq_destroy(pq);
                return NULL; // Error al crear el nodo
            }
            pq_add(pq, nodo, frecuencias[i]);
        }
    }

    // Combinar nodos hasta que solo quede uno en la cola
    while (pq_size(pq) > 1) {
        Arbol a1, a2;
        pq_remove(pq, (void**)&a1);
        pq_remove(pq, (void**)&a2);

        // Combinar a1 y a2 en un nuevo árbol
        keyvaluepair* kvp = malloc(sizeof(keyvaluepair));
        if (kvp == NULL) {
            pq_destroy(pq);
            return NULL; // Error de memoria
        }
        kvp->c = 0; // Carácter nulo para nodos internos
        kvp->frec = ((keyvaluepair*)arbol_valor(a1))->frec + ((keyvaluepair*)arbol_valor(a2))->frec;

        Arbol nuevo = arbol_crear(kvp);
        if (nuevo == NULL) {
            free(kvp);
            pq_destroy(pq);
            return NULL; // Error al crear el árbol
        }

        arbol_agregarIzq(nuevo, a1);
        arbol_agregarDer(nuevo, a2);
        pq_add(pq, nuevo, kvp->frec);
    }

    // El último árbol en la cola es el árbol de Huffman
    Arbol huffman;
    if (!pq_remove(pq, (void**)&huffman)) {
        pq_destroy(pq);
        return NULL; // Error al sacar el último árbol
    }

    pq_destroy(pq);
    return huffman;
}

// Función auxiliar para crear la tabla de codificación
static void crear_tabla(campobits* tabla, Arbol T, unsigned int bits, int tamano) {
    if (T == NULL) return;

    // Si es un nodo hoja, asigna los bits y su tamaño
    if (arbol_izq(T) == NULL && arbol_der(T) == NULL) {
        keyvaluepair *kvp = (keyvaluepair *)arbol_valor(T);
        tabla[(unsigned char)kvp->c].bits = bits;
        tabla[(unsigned char)kvp->c].tamano = tamano;
    } else {
        // Recursivamente asigna bits a los nodos izquierdo y derecho
        crear_tabla(tabla, arbol_izq(T), (bits << 1) | 0, tamano + 1);
        crear_tabla(tabla, arbol_der(T), (bits << 1) | 1, tamano + 1);
    }
}

void PutBitStreamTree(Arbol T, BitStream bs) {
    if (T == NULL) return;

    // Si es una hoja, escribe bit 1 seguido por el carácter
    if (arbol_izq(T) == NULL && arbol_der(T) == NULL) {
        keyvaluepair *kvp = (keyvaluepair *)arbol_valor(T);
        PutBit(bs, 1); // Bit que indica hoja
        PutByte(bs, kvp->c); // Carácter
    } else {
        PutBit(bs, 0); // Bit que indica nodo interno
        PutBitStreamTree(arbol_izq(T), bs); // Recursividad a la izquierda
        PutBitStreamTree(arbol_der(T), bs); // Recursividad a la derecha
    }
}

static int codificar(Arbol T, char* entrada, char* salida) {
    FILE* in = fopen(entrada, "rb");
    BitStream out = OpenBitStream(salida, "wb");
    if (!in || !out) {
        if (in) fclose(in);
        if (out) CloseBitStream(out);
        return -1; // Error al abrir los archivos
    }

    campobits tabla[NUM_CHARS];
    memset(tabla, 0, NUM_CHARS * sizeof(campobits));

    // Crear tabla de codificación
    crear_tabla(tabla, T, 0, 0);

    // Escribir el árbol al archivo de salida
    arbol_preorden(T, (void (*)(Arbol, void*))PutBitStreamTree, out);

    // Escribir el texto codificado al archivo
    int c;
    while ((c = fgetc(in)) != EOF) {
        campobits cb = tabla[c];
        for (int i = 0; i < cb.tamano; i++) {
            int bit = (cb.bits >> i) & 1;
            PutBit(out, bit);
        }
    }

    // Limpieza
    fclose(in);
    CloseBitStream(out);

    return 0;
}
             
// Función auxiliar recursiva para leer un nodo del árbol
static Arbol leer_nodo(BitStream bs) {
    int bit = GetBit(bs);
    if (bit == -1) return NULL; // Error en la lectura o fin del stream

    if (bit == 1) {
        // Es una hoja, lee el byte del carácter
        unsigned char c = GetByte(bs);
        keyvaluepair* kvp = malloc(sizeof(keyvaluepair));
        if (!kvp) return NULL; // Fallo de memoria
        kvp->c = c;
        kvp->frec = 0; // La frecuencia no se reconstruye en la descompresión
        return arbol_crear(kvp);
    } else {
        // Es un nodo interno, lee recursivamente los subárboles izquierdo y derecho
        Arbol izq = leer_nodo(bs);
        Arbol der = leer_nodo(bs);
        if (!izq || !der) {
            arbol_destruir(izq);
            arbol_destruir(der);
            return NULL; // Manejo de errores, si algún subárbol falla
        }
        keyvaluepair* kvp = malloc(sizeof(keyvaluepair));
        if (!kvp) {
            arbol_destruir(izq);
            arbol_destruir(der);
            return NULL; // Fallo de memoria
        }
        kvp->c = 0; // Los nodos internos no tienen carácter
        kvp->frec = 0;
        Arbol nodo = arbol_crear(kvp);
        arbol_agregarIzq(nodo, izq);
        arbol_agregarDer(nodo, der);
        return nodo;
    }
}

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Para leer algo que esta guardado en preorden, hay que
   pensarlo un poquito.
   
   Pero basicamente la idea es que vamos a leer el archivo
   en secuencia. Inicialmente, el archivo probablemente va 
   a empezar con el bit 0 representando la raiz del arbol. 
   Luego, tenemos que leer recursivamente (utiliza otra funcion
   para ayudarte si lo necesitas) un nodo izquierdo y uno derecho.
   Si uno (o ambos) son hojas entonces tenemos que leer tambien su
   codigo ASCII. Hacemos esto hasta que todos los nodos tienen sus 
   hijos. (Si esta bien escrito el arbol el algoritmo terminara
   porque no hay mas nodos sin hijos)
*/
static Arbol leer_arbol(BitStream bs) {
    return leer_nodo(bs);
}

/* Esto se utiliza como parte de la descompresion (ver descomprimir())..
   
   Ahora lee todos los bits que quedan en in, y escribelos como bytes
   en out. Utiliza los bits para navegar por el arbol de huffman, y
   cuando llegues a una hoja escribe el codigo ASCII al out con PutByte()
   y vuelve a comenzar a procesar bits desde la raiz.
   
   Sigue con este proceso hasta que no hay mas bits en in.
*/   
static void decodificar(BitStream in, BitStream out, Arbol arbol) {
    if (arbol == NULL) return; // Asegúrate de que el árbol no es nulo

    Arbol nodo_actual = arbol;
    int bit;

    while ((bit = GetBit(in)) != -1) { // Lee bits hasta que no haya más
        if (bit == 0) {
            // Ir al subárbol izquierdo
            nodo_actual = arbol_izq(nodo_actual);
        } else if (bit == 1) {
            // Ir al subárbol derecho
            nodo_actual = arbol_der(nodo_actual);
        }

        // Comprobar si el nodo actual es una hoja
        if (arbol_izq(nodo_actual) == NULL && arbol_der(nodo_actual) == NULL) {
            // Es una hoja, obten el carácter
            keyvaluepair* kvp = (keyvaluepair*) arbol_valor(nodo_actual);
            PutByte(out, kvp->c); // Escribe el carácter al flujo de salida
            nodo_actual = arbol; // Vuelve a la raíz para comenzar la decodificación del siguiente carácter
        }
    }
}



/* Esto es para imprimir nodos..
   Tal vez tengas mas de uno de estas funciones debendiendo
   de como decidiste representar los valores del arbol durante
   la compresion y descompresion.
*/
static void imprimirNodo(Arbol nodo) {
    if (nodo == NULL) {
        printf("Nodo nulo\n");
        return;
    }

    keyvaluepair* kvp = (keyvaluepair*) arbol_valor(nodo);
    if (kvp->c != 0) {
        // Es una hoja
        printf("Hoja: Caracter = '%c', Frecuencia = %d\n", kvp->c, kvp->frec);
    } else {
        // Es un nodo interno
        printf("Nodo interno\n");
    }
}
