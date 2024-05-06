#ifndef PQ_H
#define PQ_H

typedef struct PriorityQueue* PQ;
typedef struct _PrioValue {
    void* value;
    int prio;
} *PrioValue;

typedef int BOOLEAN;

//define Flase y true  
#define FALSE 0
#define TRUE 1

PQ pq_create();
BOOLEAN pq_add(PQ pq, void* valor, int prioridad);
BOOLEAN pq_remove(PQ pq, void** retVal);
int pq_size(PQ pq);
BOOLEAN pq_destroy(PQ pq);

#endif