#include <stdio.h>
// Strcture to store routing table information for each node
struct node
{
    int dist[20];
    int from[20];
} route[10];

int main(){
    int dm[20][20], no; // Distance matrix and number of nodes

    // Input number of nodes
    printf("Enter number of nodes: ");
    scanf("%d", &no);

    // Input distance matrix
    printf("Enter distance matrix: ");
    for(int i = 0; i < no; i++){
        for(int j = 0; j < no; j++){
            scanf("%d", &dm[i][j]);

            // Distance from node to itself should always be 0
            if(i == j){
                dm[i][j] = 0;
            }

            // Initialize routing table with direct distances
            route[i].dist[j] = dm[i][j];
            route[i].from[j] = j; // Initially, the next hop is the destination itself
        }
    }
    int flag;

    // Bellman-Ford Algorithm: Iteratively update routing table
    do{
        flag = 0; // Flag to track if any update happens
        for (int i = 0; i < no; i++){ // For each node as source
            for (int j = 0; j < no; j++){ // For each node as destination node
                for (int k = 0; k < no; k++){ // Check intermediate node k
                    // If a shorter path is found via k, update the routing table
                    if((route[i].dist[j] > (route[i].dist[k] + route[k].dist[j]))){
                        route[i].dist[j] = route[i].dist[k] + route[k].dist[j];
                        route[i].from[j] = route[i].from[k];
                        flag = 1; // Indicate that an update has occured
                    }
                }
            }                        
        }
    }while(flag); // Repeat till no updates occur
    // Print the final routing tables
    for(int i = 0; i < no ; i++){
        printf("\nRouter info for router : %c\n", (i+65));
        printf("Destination\tNext Hop\tDistance\n");
        for(int j = 0; j < no; j++){
            printf("%c\t\t%c\t\t%d\n", (j+65), (route[i].from[j]+65), route[i].dist[j]);
        }
    }
    return 0;
}