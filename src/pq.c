#include <stdlib.h>

#include "../include/pq.h"


/* Crea la cola de prioridad PQ e inicializa sus atributos
retorna un puntero a la cola de prioridad 
retorna NULL si hubo error*/
PQ pq_create() {
    PQ pq = (PQ)malloc(sizeof(PQ)); // Asignación de memoria para la estructura PQ
    if (pq == NULL) {
        return NULL; // Falla al asignar memoria para PQ
    }
    pq->cap = 10; // Capacidad inicial arbitraria
    pq->size = 0; // Tamaño inicial del montículo
    pq->arr = (PrioValue*)malloc((pq->cap + 1) * sizeof(PrioValue)); // +1 por el elemento dummy en índice 0
    if (pq->arr == NULL) {
        free(pq); // Libera memoria de pq si falla la asignación para arr
        return NULL;
    }
    return pq;
}

/*
Agrega un valor a la cola con la prioridad dada

retorna TRUE si tuvo exito, FALSE si no
*/
BOOLEAN pq_add(PQ pq, void* valor, int prioridad) {
    if (pq->size == pq->cap) { // Verifica si el montículo está lleno
        pq->cap *= 2; // Duplica la capacidad
        PrioValue* newArr = (PrioValue*)realloc(pq->arr, (pq->cap + 1) * sizeof(PrioValue));
        if (newArr == NULL) {
            return FALSE; // Falla al intentar expandir el arreglo
        }
        pq->arr = newArr;
    }

    // Crea el nuevo PrioValue
    PrioValue pv = (PrioValue)malloc(sizeof(struct _PrioValue));
    if (!pv) return FALSE; // Falla al asignar memoria para el nuevo elemento
    pv->prio = prioridad;
    pv->value = valor;

    // Inserta el nuevo elemento en la siguiente posición disponible
    pq->size++;
    int i = pq->size; // Índice donde se insertará el nuevo elemento
    while (i > 1 && pq->arr[i / 2]->prio > pv->prio) { // Compara con el padre
        pq->arr[i] = pq->arr[i / 2]; // Mueve el padre hacia abajo
        i /= 2;
    }
    pq->arr[i] = pv;

    return TRUE;
}


/* 
  Saca el valor de menor prioridad (cima del monticulo) y lo guarda en la posicion retVal (paso por referencia)
  retorna FALSE si tiene un error
  retorna TRUE si tuvo EXITO
*/
BOOLEAN pq_remove(PQ pq, void** retVal) {
    if (pq->size == 0) return FALSE; // Montículo vacío

    *retVal = pq->arr[1]->value; // Guarda el valor de la cima del montículo para retornar

    // Libera el PrioValue de la cima
    free(pq->arr[1]);

    // Mueve el último elemento a la cima
    pq->arr[1] = pq->arr[pq->size];
    pq->size--; // Reduce el tamaño del montículo

    // Reorganiza el montículo comenzando desde la nueva cima
    int i = 1; // Comienza desde la nueva cima
    while (TRUE) {
        int left = 2 * i; // Índice del hijo izquierdo
        int right = 2 * i + 1; // Índice del hijo derecho
        int smallest = i;

        // Encuentra el menor entre el nodo actual y sus hijos
        if (left <= pq->size && pq->arr[left]->prio < pq->arr[smallest]->prio) {
            smallest = left;
        }
        if (right <= pq->size && pq->arr[right]->prio < pq->arr[smallest]->prio) {
            smallest = right;
        }

        // Si el menor es uno de los hijos, intercambia y sigue hundiendo
        if (smallest != i) {
            PrioValue temp = pq->arr[i];
            pq->arr[i] = pq->arr[smallest];
            pq->arr[smallest] = temp;
            i = smallest; // Continúa con el siguiente nivel
        } else {
            break; // Si el nodo actual es menor que ambos hijos, termina
        }
    }

    return TRUE;
}

/* retorna el tama�o de la cola de prioridad, 
   retorna 0 si hubo error 
 */
int pq_size(PQ pq) {
    return pq->size;
}

/* Destruye la cola de prioridad, 
retorna TRUE si tuvo exito
retorna FALSE si tuvo error*/
BOOLEAN pq_destroy(PQ pq) {
    if (!pq) return FALSE; // Verifica si pq es NULL
    for (int i = 1; i <= pq->size; i++) {
        free(pq->arr[i]); // Libera cada PrioValue
    }
    free(pq->arr); // Libera el arreglo de PrioValues
    free(pq); // Libera la estructura PQ
    return TRUE;
}