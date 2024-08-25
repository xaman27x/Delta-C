#include <stdio.h>
#include <string.h>
#include <math.h>
#include "airdata.h"
#define MAX_SIZE 1024
#define PI 3.1415626

void getConnections(char *src, char **destination) {
    FILE* fd;
    char ch;
    char buffer[MAX_SIZE];
    int len = 0, dest_len = 0;
    destination[dest_len] = NULL;
    fd = fopen("datasets/routes.csv", "r");
    if (fd == NULL) {
        perror("Failed to open file");
        return;
    }
    while ((ch = fgetc(fd)) != EOF) {
        if(ch == '\n' || ch == '\r' || ch == '\t') {
            buffer[len] = '\0';
            char* token = strtok(buffer, ",");
            while (token != NULL) {
                token = strtok(NULL, ",");
                token = strtok(NULL, ",");
                if(strcmp(token, src) == 0) {
                    token = strtok(NULL, ",");
                    token = strtok(NULL, ",");
                    printf("%s -> %s\n", src, token);
                    destination[dest_len] = (char*) malloc(sizeof(token));
                    strcpy(destination[dest_len++], token);
                };
                break;
            }
            len = 0;
        } else {
            if (len < MAX_SIZE - 1) {  // Ensure we don't overflow the buffer
                buffer[len++] = ch;
            }
        }
    }

    fclose(fd);
}

void getCoordinates(char* airport) {
    FILE* fd;
    char ch, buffer[MAX_SIZE];
    int len = 0;
    float latitude;
    float longitude;

    fd = fopen("datasets/airports.csv", "r");
    if (fd == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fd) != NULL) {
        char* token = strtok(buffer, ",");
        while (token != NULL) {
            if (strcmp(token, airport) == 0) {
                for (int i = 0; i < 13; i++) {
                    token = strtok(NULL, ",");
                    if (token == NULL) {
                        fclose(fd);
                        return;
                    }
                }
                latitude = strtof(token, NULL);
                token = strtok(NULL, ",");
                if (token != NULL) {
                    longitude = strtof(token, NULL);
                    printf("Latitude: %f, Longitude: %f\n", latitude, longitude);
                } else {
                    printf("Error: Longitude data missing.\n");
                }
                fclose(fd);
                return;
            }
            token = strtok(NULL, ",");
        }
    }

    fclose(fd);
}

float calculateDistance(float lat1, float long1, float lat2, float long2) {
    double Lat = (lat2 - lat1) * PI / 180.0;
    double Lon = (long2 - long1) * PI / 180.0;
 
        // convert to radians
        lat1 = (lat1) * PI / 180.0;
        lat2 = (lat2) * PI / 180.0;
 
        // apply formulae
        float a = pow(sin(Lat / 2), 2) + pow(sin(Lon / 2), 2) * cos(lat1) * cos(lat2);
        float rad = 6371;
        float c = 2 * asin(sqrt(a));
        return rad * c;
};