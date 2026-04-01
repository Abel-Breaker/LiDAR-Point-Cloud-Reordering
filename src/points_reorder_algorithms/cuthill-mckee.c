#include "cuthill-mckee.h"
#include "../utils/parse_args.h"
#include "../neighborhood_algorithms/knn_kd_tree.h"
#include "../utils/error_handler.h"
#include "../utils/auxiliar_structures/queue.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * TODO: Probar distintos criterios añadir los nodos a la cola: 
 *  - Por menor distancia euclídea
 *  - Por orden creciente en base al número de índice
 *  - Por menor distanca euclídea pero invirtiendo la permitación (equivalente a RCM)
 *  - Probar DFS (utilizar una pila/recursión en vez de una cola)
 */

// TODO: Es horrible la logica de la llamada de la funcion, tree queda inutilizado y libera points autometicamente
void reorder_points_cuthill_mckee(const KDTree *tree, Points *points){
    unsigned int number_of_points = (unsigned int)tree->pts->num_points;

    Queue *queue = createQueue(number_of_points);
    

    unsigned int *permutations;
    bool *visited;
    permutations = malloc(number_of_points * sizeof(*permutations));
    visited = calloc(number_of_points, sizeof(*visited));
    
    enqueue(queue, 0);

    unsigned int points_visited=0;

    while(is_queue_empty(queue) == false){
        unsigned int new_index = dequeue(queue);

        permutations[points_visited] = new_index;
        ++points_visited;

        visited[new_index] = true;

        size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, new_index, neighbours, neighbours_distances);

		for (unsigned int j = 0; j < K; ++j) {
             if (!visited[neighbours[j]]) {
                visited[neighbours[j]] = true;
                enqueue(queue, (unsigned int)neighbours[j]);
             }
			
		}
    }

    // TODO: 
    swap_points(points, permutations);


    destroyQueue(queue);
    free(permutations);
    free(visited);
}