#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>

typedef struct {
    int index;
    double x;
    double y;
    double score;
} Vertex;

typedef struct {
    std::vector<Vertex> path;
    double max_length;
    double total_trip_length;
} Trip;

typedef struct 
{
    std::vector<Trip> trips;
    double max_tour_length;
    double total_tour_length;
} Tour;

void readFile(
    std::vector<Vertex>* vertices,
    Tour* base_tour, 
    char* input_file, 
    int* num_hotels, 
    int* num_points, 
    int* num_trips
) {
    std::ifstream input(input_file);

    input >> (*num_points) >> (*num_hotels) >> (*num_trips);
    (*num_points) -= 2;
    (*num_hotels) += 2;

    input >> base_tour->max_tour_length;

    for(int i = 0; i < (*num_trips); i++) {
        Trip trip;
        input >> trip.max_length;
        trip.total_trip_length = 0;
        base_tour->trips.push_back(trip);
    }

    for(int i = 0; i < (*num_hotels) + (*num_points); i++) {
        Vertex vertex;
        vertex.index = i;
        input >> vertex.x >> vertex.y >> vertex.score;
        vertices->push_back(vertex);
    }
}

void printInfo(
    std::vector<Vertex> vertices,
    Tour base_tour,
    int num_hotels, 
    int num_points, 
    int num_trips) 
{
    int count = 0;
    std::cout << "// INPUT INFOS ================================================" << std::endl;
    std::cout << "// GENERAL ====================================================" << std::endl;
    std::cout << "Number of hotels: " << num_hotels << std::endl;
    std::cout << "Number of points of interest: " << num_points << std::endl;
    std::cout << "Number of trips: " << num_trips << std::endl;
    std::cout << "Max tour length: " << base_tour.max_tour_length << std::endl;

    std::cout << "// TRIPS ======================================================" << std::endl;
    for(auto trip : base_tour.trips) {
        std::cout << "Max trip lenght for trip " << count << ": " << trip.max_length << std::endl;
        count++;
    }

    std::cout << "// HOTELS =====================================================" << std::endl;
    std::cout << std::setw(10) << std::left << "INDEX" << std::setw(7) << std::left << "X" << std::setw(7) << std::left << "Y" << "SCORE" << std::endl;
    for(int i = 0; i < num_hotels; i++) {
        std::cout << std::setw(10) << std::left << vertices[i].index << std::setw(7) << std::left << vertices[i].x << std::setw(7) << std::left << vertices[i].y << vertices[i].score << std::endl;
    }

    std::cout << "// POINTS OF INTEREST =========================================" << std::endl;
    std::cout << std::setw(10) << std::left << "INDEX" << std::setw(7) << std::left << "X" << std::setw(7) << std::left << "Y" << "SCORE" << std::endl;
    for(int i = num_hotels; i < num_hotels + num_points; i++) {
        std::cout << std::setw(10) << std::left << vertices[i].index << std::setw(7) << std::left << vertices[i].x << std::setw(7) << std::left << vertices[i].y << vertices[i].score << std::endl;
    }
}

double calcDistance(Vertex v1, Vertex v2) {
    return sqrt(pow((v1.x - v2.x), 2) + pow((v1.y - v2.y), 2));
}

void fillDistanceMatrix(double** distance_matrix, std::vector<Vertex> vertices) {
    for(int i = 0; i < vertices.size(); i++) {
        for(int j = 0; j < vertices.size(); j++) {
            if(i == j) distance_matrix[i][j] = 0;
            else distance_matrix[i][j] = calcDistance(vertices[i], vertices[j]);
        }
    }
}

int main(int argc, char *argv[])
{
    std::vector<Vertex> vertices;
    Tour base_tour;
    int num_hotels;
    int num_points;
    int num_trips;
    double** distance_matrix;
    char input_file[100] = "Instances\\SET1 1-2\\T3-105-1-2.ophs";

    // READING INPUT ============================================================
    readFile(&vertices, &base_tour, input_file, &num_hotels, &num_points, &num_trips);

    // PRINTING INFO FROM INPUT =================================================
    printInfo(vertices, base_tour, num_hotels, num_points, num_trips);

    // CREATING DISTANCE MATRIX =================================================
    distance_matrix = (double**)malloc(sizeof(double*) * (num_hotels + num_points));
    for(int i = 0; i < num_hotels + num_points; i++)
        distance_matrix[i] = (double*)malloc(sizeof(double) * (num_hotels + num_points));

    // FILLING DISTANCE MATRIX ==================================================
    fillDistanceMatrix(distance_matrix, vertices);

    // MEMORY RELEASING =========================================================
    for(int i = 0; i < num_hotels + num_points; i++) free(distance_matrix[i]);
    free(distance_matrix);
}

