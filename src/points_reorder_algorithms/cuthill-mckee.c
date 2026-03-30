#include "cuthill-mckee.h"
#include "structures/neighborhood_matrix.h"
#include "../utils/parse_args.h"
#include "../neighborhood_algorithms/knn_kd_tree.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * TODO: Probar distintos criterios añadir los nodos a la cola: 
 *  - Por menor distancia euclídea
 *  - Por orden creciente en base al número de índice
 *  - Por menor distanca euclídea pero invirtiendo la permitación (equivalente a RCM)
 *  - Probar DFS (utilizar una pila/recursión en vez de una cola)
 */

 // TODO: Optimizar la cola
typedef struct Node {
    unsigned int index;
    struct Node* next;
} Node;

// Cola
typedef struct {
    Node* front;
    Node* rear;
} Queue;

// Crear cola
void createQueue(Queue *q) {
    q->front = q->rear = nullptr;
}

// Encolar (push)
void enqueue(Queue* q, unsigned int index) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->index = index;
    newNode->next = nullptr;

    if (q->rear == nullptr) {
        q->front = q->rear = newNode;
        return;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}

// Desencolar (pop)
unsigned int dequeue(Queue* q) {
    if (q->front == nullptr) {
        printf("Cola vacía\n");
        return 0;
    }

    Node* temp = q->front;
    unsigned int value = temp->index;

    q->front = q->front->next;

    if (q->front == nullptr)
        q->rear = nullptr;

    free(temp);
    return value;
}

// TODO: Es horrible la logica de la llamada de la funcion, tree queda inutilizado y libera points autometicamente
void reorder_points_cuthill_mckee(const KDTree *tree, Points *points){
    unsigned int number_of_points = (unsigned int)tree->pts->num_points;

    Queue queue;
    createQueue(&queue);
    

    unsigned int *permutations;
    bool *visited;
    permutations = malloc(number_of_points * sizeof(*permutations));
    visited = calloc(number_of_points, sizeof(*visited));
    
    enqueue(&queue, 0);

    unsigned int points_visited=0;

    while(queue.front != nullptr){
        unsigned int new_index = dequeue(&queue);

        permutations[points_visited] = new_index;
        ++points_visited;

        visited[new_index] = true;

        size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, new_index, neighbours, neighbours_distances);

		for (unsigned int j = 0; j < K; ++j) {
             if (!visited[neighbours[j]]) {
                visited[neighbours[j]] = true;
                enqueue(&queue, (unsigned int)neighbours[j]);
             }
			
		}
    }

    // TODO: 
    swap_points(points, permutations);


    free(permutations);
    free(visited);
}