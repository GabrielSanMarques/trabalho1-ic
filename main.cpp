#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

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
    std::vector<Vertex>* points,
    std::vector<Vertex>* hotels,
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

    for(int i = 0; i < (*num_hotels); i++) {
        Vertex hotel;
        hotel.index = i;
        input >> hotel.x >> hotel.y >> hotel.score;
        hotels->push_back(hotel);
    }

    for(int i = 0; i < (*num_points); i++) {
        Vertex point;
        point.index = i;
        input >> point.x >> point.y >> point.score;
        points->push_back(point);
    }
}

void printInfo(
    std::vector<Vertex> points,
    std::vector<Vertex> hotels, 
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
    for(auto hotel : hotels) {
        std::cout << std::setw(10) << std::left << hotel.index << std::setw(7) << std::left << hotel.x << std::setw(7) << std::left << hotel.y << hotel.score << std::endl;
    }

    std::cout << "// POINTS OF INTEREST =========================================" << std::endl;
    std::cout << std::setw(10) << std::left << "INDEX" << std::setw(7) << std::left << "X" << std::setw(7) << std::left << "Y" << "SCORE" << std::endl;
    for(auto point : points) {
        std::cout << std::setw(10) << std::left << point.index << std::setw(7) << std::left << point.x << std::setw(7) << std::left << point.y << point.score << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::vector<Vertex> points;
    std::vector<Vertex> hotels;
    Tour base_tour;
    int num_hotels;
    int num_points;
    int num_trips;
    double** distance_matrix;
    char input_file[100] = "Instances\\SET1 1-2\\T3-105-1-2.ophs";

    // READING INPUT ============================================================
    readFile(&points, &hotels, &base_tour, input_file, &num_hotels, &num_points, &num_trips);

    // PRINTING INFO FROM INPUT =================================================
    //printInfo(points, hotels, base_tour, num_hotels, num_points, num_trips);

    // CREATING DISTANCE MATRIX =================================================

}

