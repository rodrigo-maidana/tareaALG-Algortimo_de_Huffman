#include "./include/pq.h"
#include <stdlib.h>


/* Crea la cola de prioridad PQ e inicializa sus atributos
retorna un puntero a la cola de prioridad 
retorna NULL si hubo error*/
PQ pq_create() {
   
	
	/* PQ pq = (PQ) malloc(sizeof(struct Heap)); */
	/* AGREGUE SU CODIGO AQUI */
	return NULL;
}

/*
Agrega un valor a la cola con la prioridad dada

retorna TRUE si tuvo exito, FALSE si no
*/
BOOLEAN pq_add(PQ pq, void* valor, int prioridad) {
   
	/* AGREGUE SU CODIGO AQUI */
   return TRUE;
}

/* 
  Saca el valor de menor prioridad (cima del monticulo) y lo guarda en la posicion retVal (paso por referencia)
  retorna FALSE si tiene un error
  retorna TRUE si tuvo EXITO
*/
BOOLEAN pq_remove(PQ pq, void** retVal) {

   /* AGREGUE SU CODIGO AQUI */
	*retVal = pq->arr[1]->value;
   return TRUE;
}

/* retorna el tama�o de la cola de prioridad, 
   retorna 0 si hubo error 
 */
int pq_size(PQ pq) {

   	/* AGREGUE SU CODIGO AQUI */
   return 0;
}

/* Destruye la cola de prioridad, 
retorna TRUE si tuvo exito
retorna FALSE si tuvo error*/
BOOLEAN pq_destroy(PQ pq) {
   
   /* AGREGUE SU CODIGO AQUI */
	return TRUE;
}
