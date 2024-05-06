#include "../include/pq.h"
#include <stdlib.h>

struct PriorityQueue {
    PrioValue* arr;
    int size;
    int cap;
};

PQ pq_create() {
    PQ pq = (PQ)malloc(sizeof(struct PriorityQueue));
    if (!pq) {
        return NULL;
    }
    pq->cap = 10;
    pq->size = 0;
    pq->arr = (PrioValue*)malloc((pq->cap + 1) * sizeof(PrioValue));
    if (pq->arr == NULL) {
        free(pq);
        return NULL;
    }
    return pq;
}

BOOLEAN pq_add(PQ pq, void* valor, int prioridad) {
    if (pq->size == pq->cap) {
        pq->cap *= 2;
        PrioValue* newArr = (PrioValue*)realloc(pq->arr, (pq->cap + 1) * sizeof(struct _PrioValue));
        if (newArr == NULL) {
            return FALSE;
        }
        pq->arr = newArr;
    }

    PrioValue pv = (PrioValue)malloc(sizeof(struct _PrioValue));
    if (!pv) return FALSE;
    pv->prio = prioridad;
    pv->value = valor;

    pq->size++;
    int i = pq->size;
    while (i > 1 && pq->arr[i / 2]->prio > pv->prio) {
        pq->arr[i] = pq->arr[i / 2];
        i /= 2;
    }
    pq->arr[i] = pv;

    return TRUE;
}

BOOLEAN pq_remove(PQ pq, void** retVal) {
    if (pq->size == 0) return FALSE;

    *retVal = pq->arr[1]->value;
    free(pq->arr[1]);

    pq->arr[1] = pq->arr[pq->size];
    pq->size--;

    int i = 1;
    while (TRUE) {
        int left = 2 * i;
        int right = 2 * i + 1;
        int smallest = i;

        if (left <= pq->size && pq->arr[left]->prio < pq->arr[smallest]->prio) {
            smallest = left;
        }
        if (right <= pq->size && pq->arr[right]->prio < pq->arr[smallest]->prio) {
            smallest = right;
        }

        if (smallest != i) {
            PrioValue temp = pq->arr[i];
            pq->arr[i] = pq->arr[smallest];
            pq->arr[smallest] = temp;
            i = smallest;
        } else {
            break;
        }
    }

    return TRUE;
}

int pq_size(PQ pq) {
    return pq->size;
}

BOOLEAN pq_destroy(PQ pq) {
    if (!pq) return FALSE;
    for (int i = 1; i <= pq->size; i++) {
        free(pq->arr[i]);
    }
    free(pq->arr);
    free(pq);
    return TRUE;
}
