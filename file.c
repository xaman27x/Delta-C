#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fileOps/airdata.c"

int main() {
    char* destinations[100];  
    connection* node = (connection*) malloc(sizeof(connection));
    node->source = "PNQ";
    node->destinations = destinations;
    getConnections(node->source, node->destinations);
    getCoordinates("PNQ");
    printf("%s\n", node->destinations[0]);
    printf("%f\n", calculateDistance(18.5204, 73.8567, 21.1458, 79.0882));
    return 0;
}
