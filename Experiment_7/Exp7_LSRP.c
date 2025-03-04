#include <stdio.h>
#include <stdbool.h>

#define MAX_NODES 10 // Maximum number of nodes
#define INF 99999 // Represents infinity

int numNodes; // Number of nodes in the network
typedef struct{
    int cost;
    bool exists;
} Edge;
Edge graph[MAX_NODES][MAX_NODES]; // Adjacency matrix representation of the graph
bool visited[MAX_NODES]; // Array to keep track of visited nodes

// Function to perform Dijkstre'a Algorithm for shortest path
void dijkstra(int startNode){
    int distance[MAX_NODES]; // Stores shortest distance from startNode to each node
    int previous[MAX_NODES]; // Array to keep track of visited nodes

    // Initialize all distances as INF and visited[] as false
    for(int i = 0; i < numNodes; i++){
        distance[i] = INF;
        visited[i] = false;
        previous[i] = -1;
    }
    distance[startNode] = 0; // Distance to itself is 0

    //Loop to find shortest paths for all nodes
    for(int count = 0; count < numNodes - 1; count++){
        int minDistance = INF, minNode = -1;

        // FInd the unvisited node with the smallest distance
        for(int i = 0; i < numNodes; i++){
            if(!visited[i] && distance[i] < minDistance){
                minDistance = distance[i];
                minNode = i;
            }
        }
        if(minNode == -1){// No reachable unvisited nodes left
            break;
        }
        visited[minNode] = true; // Mark the node as visited
        
        // Update distances for neighbouring nodes
        for(int i = 0; i < numNodes; i++){
            if(!visited[i] && graph[minNode][i].exists && distance[minNode] + graph[minNode][i].cost < distance[i]){
                distance[i] = distance[minNode] + graph[minNode][i].cost;
                previous[i] = minNode;
            }
        }
    }
    // Print the Routing Table for the node
    printf("\nRouting Table for Node %d\n", startNode);
    printf("Destination\tNext Hop\tCost\n");
    for(int i = 0; i < numNodes; i++){
        if(i != startNode){
            printf("%d\t\t%d\t\t%d\n", i, previous[i] != -1 ? previous[i] : -1, distance[i]);
        }
    }
}

int main(){
    printf("Enter the number of nodes: ");
    scanf("%d", &numNodes);

    // Initialize graph with no links(infinite cost)
    for(int i = 0; i < numNodes; i++){
        for(int j = 0; j < numNodes; j++){
            graph[i][j].cost = (i==j) ? 0 : INF;
            graph[i][j].exists = false;
        }
    }

    int numLinks;
    printf("Enter the number of links: ");
    scanf("%d", &numLinks);
    
    // Read user input for graph connections
    for(int i = 0; i < numLinks; i++){
        int src, dest, cost;
        printf("Ender source, destination and cost of link %d: ", i+1);
        scanf("%d %d %d", &src, &dest, &cost);

        graph[src][dest].cost = cost;
        graph[dest][src].cost = cost;
        graph[src][dest].exists = true;
        graph[dest][src].exists = true;
    }

    // Run Dijkstra's Algorithm from each node
    for(int i = 0; i < numNodes; i++){
        dijkstra(i);
    }
    
    return 0;
}