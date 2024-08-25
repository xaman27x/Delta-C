typedef struct connection {
    char* source;
    char** destinations;
} connection;

typedef struct airport_location {
    char* code;
    float latitude;
    float longitude;
} airport_location;

void getConnections(char* src, char** destination);
void getCoordinates(char* airport);
float calculateDistance(float lat1, float long1, float lat2, float long2);