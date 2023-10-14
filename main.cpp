#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <bits/stdc++.h> 
#include <random>

#define MAX_LS_ITER 50
#define MAX_GRASP_ITER 50

typedef struct {
    int index;
    double x;
    double y;
    double score;
    double temp_score;
    double min_dist_to_hotel;
    int closest_hotel_index;
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
    double total_tour_score;
} Tour;

void readFile(
    std::vector<Vertex>* vertices,
    Tour* base_solution, 
    char* input_file, 
    int* num_hotels, 
    int* num_points, 
    int* num_trips
) {
    std::ifstream input(input_file);

    input >> (*num_points) >> (*num_hotels) >> (*num_trips);
    (*num_points) -= 2;
    (*num_hotels) += 2;

    input >> base_solution->max_tour_length;

    for(int i = 0; i < (*num_trips); i++) {
        Trip trip;
        input >> trip.max_length;
        trip.total_trip_length = 0;
        base_solution->trips.push_back(trip);
    }

    for(int i = 0; i < (*num_hotels) + (*num_points); i++) {
        Vertex vertex;
        vertex.index = i;
        vertex.min_dist_to_hotel = 0;
        input >> vertex.x >> vertex.y >> vertex.score;
        vertices->push_back(vertex);
    }
}

void printInfo(
    std::vector<Vertex> vertices,
    Tour base_solution,
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
    std::cout << "Max tour length: " << base_solution.max_tour_length << std::endl;

    std::cout << "// TRIPS ======================================================" << std::endl;
    for(auto trip : base_solution.trips) {
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

void fillMinDistsToHotels(std::vector<Vertex>* vertices, double** distance_matrix, int num_hotels) {
    double min;
    int index;
    for(int i = num_hotels; i < vertices->size(); i++) {
        min = distance_matrix[i][0];
        index = 0;
        for(int j = 1; j < num_hotels; j++) {
            if(distance_matrix[i][j] < min) {
                index = j;
                min = distance_matrix[i][j];
            }
        }
        (*vertices)[i].min_dist_to_hotel = min;
        (*vertices)[i].closest_hotel_index = index;
    }
}

bool compareByTempScore(Vertex v1, Vertex v2) {
    return v1.temp_score > v2.temp_score;
}

bool compareByScore(Vertex v1, Vertex v2) {
    return v1.score > v2.score;
}

bool canBeAddedToPathEnd(Trip trip, Vertex vertex, double** distance_matrix, bool is_final_trip) {
    if(!is_final_trip)
        return (trip.total_trip_length + distance_matrix[trip.path[trip.path.size() - 1].index][vertex.index] + vertex.min_dist_to_hotel <= trip.max_length);
    else
        return (trip.total_trip_length + distance_matrix[trip.path[trip.path.size() - 1].index][vertex.index] + distance_matrix[1][vertex.index] <= trip.max_length);
}

bool canBeInsertedAfter(Trip trip, Vertex vertex, double** distance_matrix, int index) {
    double new_length = trip.total_trip_length - distance_matrix[trip.path[index].index][trip.path[index + 1].index] + distance_matrix[trip.path[index].index][vertex.index] + distance_matrix[vertex.index][trip.path[index + 1].index];

    if(new_length <= trip.max_length) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Can insert vertex " << vertex.index << " after vertex " << trip.path[index].index << std::endl;
        std::cout << "Old length: " << trip.total_trip_length << std::endl;
        std::cout << "Subtracted distance: " << distance_matrix[trip.path[index].index][trip.path[index + 1].index] << std::endl;
        std::cout << "Added distance: " << distance_matrix[trip.path[index].index][vertex.index] + distance_matrix[vertex.index][trip.path[index + 1].index] << std::endl;
        std::cout << "New distance: " << new_length << std::endl;
        std::cout << "Score increase: " << vertex.score << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }

    return (new_length <= trip.max_length);
}

std::vector<Vertex> updateTempScores(std::vector<Vertex> cndt_points, Trip trip, double** distance_matrix) {
    for(auto point = cndt_points.begin(); point != cndt_points.end(); point++) {
        point->temp_score = point->score / distance_matrix[trip.path[trip.path.size() - 1].index][point->index];
    }
    std::sort(cndt_points.begin(), cndt_points.end(), compareByTempScore);
    return cndt_points;
}

double calcTripScore(Trip trip) {
    double sum = 0;
    for(auto vertex : trip.path) {
        sum += vertex.score;
    }
    return sum;
}

double calcTripLength(Trip trip, double** distance_matrix) {
    double length = 0;
    for(int i = 0; i < trip.path.size() - 1; i++) {
        length += distance_matrix[trip.path[i].index][trip.path[i + 1].index];
    }
    return length;
}

double calcTourScore(Tour tour) {
    double sum = 0;
    for(auto trip : tour.trips) {
        sum += calcTripScore(trip);
    }
    return sum;
}

double calcTourLength(Tour tour) {
    double sum = 0;
    for(auto trip : tour.trips) {
        sum += trip.total_trip_length;
    }
    return sum;
}

std::vector<Vertex> insertVertexAfter(std::vector<Vertex> path, Vertex vertex, int insert_after) {
    path.insert(path.begin() + insert_after + 1, vertex);
    return path;
}

std::vector<Vertex> evaluateCandidates(std::vector<Vertex> cndt_points, Trip trip, double** distance_matrix, bool is_final_trip) {
    std::vector<Vertex> partial_cndt_points;
    for(int cndt_index = 0; cndt_index < cndt_points.size(); cndt_index++) {
            if(canBeAddedToPathEnd(trip, cndt_points[cndt_index], distance_matrix, is_final_trip)) {
                partial_cndt_points.push_back(cndt_points[cndt_index]);
        }
    }
    return partial_cndt_points;
}

std::vector<Vertex> eraseCandidateByIndex(std::vector<Vertex> cndt_points, int index) {
    int i = 0;
    while(cndt_points[i].index != index) i++;
    cndt_points.erase(cndt_points.begin() + i);
    return cndt_points;
}

Trip swapTripVertices(Trip trip, int p1, int p2, double** distance_matrix) {
    Trip new_trip = trip;
    std::iter_swap(new_trip.path.begin() + p1, new_trip.path.begin() + p2);
    new_trip.total_trip_length = calcTripLength(new_trip, distance_matrix);
    if(new_trip.total_trip_length < trip.max_length) {
        if(new_trip.total_trip_length < trip.total_trip_length) {
            std::cout << "===== SWAPPING =====" << std::endl;
            std::cout << "Old trip: ";
            for(auto point : trip.path) std::cout << point.index << " ";
            std::cout << std::endl;
            std::cout << "Old trip length: " << trip.total_trip_length << std::endl;
            std::cout << "New trip after swapping " << trip.path[p1].index << " and " << trip.path[p2].index << ": ";
            for(auto point : new_trip.path) std::cout << point.index << " ";
            std::cout << std::endl;
            std::cout << "New trip length: " << new_trip.total_trip_length << std::endl;
            std::cout << "====================" << std::endl;
            return new_trip;
            
        }
    }
    return trip;
}

// TODO: finalizar a construção
Tour constructInitialSolution(
    Tour base_solution,
    std::vector<Vertex> vertices, 
    int num_hotels, 
    int num_points, 
    double** distance_matrix,
    std::vector<Vertex>* not_visited_points
){
    std::vector<Vertex> cndt_hotels;
    std::vector<Vertex> cndt_points;
    std::vector<Vertex> partial_cndt_points;
    Tour initial_solution = base_solution;
    int index;
    double max_increase_cndt;

    for(int i = 0; i < num_hotels; i++) cndt_hotels.push_back(vertices[i]);
    for(int i = num_hotels; i < vertices.size(); i++) cndt_points.push_back(vertices[i]);

    // std::sort(cndt_points.begin(), cndt_points.end(), compareByScore);

    // Insere o hotel inicial no começo da primeira viagem
    initial_solution.trips[0].path.push_back(cndt_hotels[0]);

    for(int trip_index = 0; trip_index < initial_solution.trips.size(); trip_index++) {
        bool finished_trip = false;
        bool is_final_trip = (trip_index == initial_solution.trips.size() - 1);

        while(!finished_trip) {
            bool added_vertex = false;
            int cndt_index = 0;

            cndt_points = updateTempScores(cndt_points, initial_solution.trips[trip_index], distance_matrix);
            partial_cndt_points = evaluateCandidates(cndt_points, initial_solution.trips[trip_index], distance_matrix, is_final_trip);

            std::random_device rd;     // Only used once to initialise (seed) engine
            std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
            std::uniform_int_distribution<int> uni(0, floor((partial_cndt_points.size() - 1) * 0.333)); // Guaranteed unbiased

            auto random_integer = uni(rng);

            if(partial_cndt_points.size() != 0)
            {
                std::cout << "Vector range: 0 - " << partial_cndt_points.size() - 1 << std::endl;
                std::cout << "Range considered: 0 - " << floor((partial_cndt_points.size() - 1) * 0.333) << std::endl;
                std::cout << "Selected candidate at index " << random_integer << ": " << partial_cndt_points[random_integer].index << std::endl; 
            }
            std::cout << "Partial candidates: ";
            for(auto point : partial_cndt_points) {
                std::cout << point.index << " ";
            }
            std::cout << std::endl;

            if(partial_cndt_points.size() > 0) {
                initial_solution.trips[trip_index].total_trip_length += distance_matrix[initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 1].index][partial_cndt_points[random_integer].index];
                initial_solution.trips[trip_index].path.push_back(partial_cndt_points[random_integer]);
                added_vertex = true;
            }

            // for(cndt_index; cndt_index < cndt_points.size() && !added_vertex; cndt_index++) {
            //     if(canBeAddedToPathEnd(initial_solution.trips[trip_index], cndt_points[cndt_index], distance_matrix, is_final_trip)) {
            //         initial_solution.trips[trip_index].total_trip_length += distance_matrix[initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 1].index][cndt_points[cndt_index].index];
            //         initial_solution.trips[trip_index].path.push_back(cndt_points[cndt_index]);
            //         added_vertex = true;
            //     }
            // }

            if(!added_vertex) {
                
                if(!is_final_trip) {
                    initial_solution.trips[trip_index].path.push_back(vertices[initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 1].closest_hotel_index]);
                    initial_solution.trips[trip_index + 1].path.push_back(initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 1]);
                    initial_solution.trips[trip_index].total_trip_length += initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 2].min_dist_to_hotel;
                    finished_trip = true;
                }
                else {
                    initial_solution.trips[trip_index].path.push_back(cndt_hotels[1]);
                    initial_solution.trips[trip_index].total_trip_length += distance_matrix[1][initial_solution.trips[trip_index].path[initial_solution.trips[trip_index].path.size() - 2].index];
                    finished_trip = true;
                }
            } 
            else {
                cndt_points = eraseCandidateByIndex(cndt_points, partial_cndt_points[random_integer].index);
            };
        }   
    }

    (*not_visited_points) = cndt_points;

    return initial_solution;
}

Tour localSearch(Tour inital_solution, double** distance_matrix, std::vector<Vertex> not_visited_points) {
    Tour best_solution = inital_solution;
    Tour partial_solution= inital_solution;

    std::cout << "Initial not visited vector: ";
    for(auto point : not_visited_points) std::cout << point.index << " ";
    std::cout << std::endl;

    double best_score;
    int point_to_insert;
    int insert_after;

    //std::sort(not_visited_points.begin(), not_visited_points.end(), compareByScore);

    for(int i = 0; i < MAX_LS_ITER; i++) {
        if(not_visited_points.size() == 0) break;

        for(int trip_index = 0; trip_index < partial_solution.trips.size(); trip_index++) {
            if(not_visited_points.size() == 0) break;

            best_score = calcTripScore(partial_solution.trips[trip_index]);

            for(int cndt_index = 0; cndt_index < not_visited_points.size(); cndt_index++) {
                for(int points_index = 0; points_index < partial_solution.trips[trip_index].path.size() - 1; points_index++) {
                    if(canBeInsertedAfter(partial_solution.trips[trip_index], not_visited_points[cndt_index], distance_matrix, points_index)) {
                        if(best_score < partial_solution.total_tour_score + not_visited_points[cndt_index].score) {
                            best_score = partial_solution.total_tour_score + not_visited_points[cndt_index].score;
                            point_to_insert = cndt_index;
                            insert_after = points_index;
                        }
                    }
                }
            }

            if(best_score > calcTripScore(partial_solution.trips[trip_index])) {
                partial_solution.trips[trip_index].path = insertVertexAfter(partial_solution.trips[trip_index].path, not_visited_points[point_to_insert], insert_after);
                partial_solution.trips[trip_index].total_trip_length = calcTripLength(partial_solution.trips[trip_index], distance_matrix); 
                partial_solution.total_tour_score = calcTourScore(partial_solution);
                not_visited_points.erase(not_visited_points.begin() + point_to_insert);
            }
        }

        for(int trip_index = 0; trip_index < partial_solution.trips.size(); trip_index++) {
            for(int p1 = 1; p1 < partial_solution.trips[trip_index].path.size() - 1; p1++) {
                for(int p2 = p1 + 1; p2 < partial_solution.trips[trip_index].path.size() - 1; p2++) {
                    partial_solution.trips[trip_index] = swapTripVertices(partial_solution.trips[trip_index], p1, p2, distance_matrix);
                }
            }
        }
    }

    return partial_solution;
}

int main(int argc, char *argv[])
{
    std::vector<Vertex> vertices;
    std::vector<Vertex> not_visited_points;
    Tour base_solution;
    Tour best_grasp_solution;
    Tour initial_solution;
    int best_grasp_score = -1;
    base_solution.total_tour_score = 0;
    int num_hotels;
    int num_points;
    int num_trips;
    double** distance_matrix;
    char input_file[100] = "Instances\\SET5 15-10\\100-150-15-10.ophs";

    // READING INPUT ============================================================
    readFile(&vertices, &base_solution, input_file, &num_hotels, &num_points, &num_trips);

    // PRINTING INFO FROM INPUT =================================================
    printInfo(vertices, base_solution, num_hotels, num_points, num_trips); 

    // CREATING DISTANCE MATRIX =================================================
    distance_matrix = (double**)malloc(sizeof(double*) * (num_hotels + num_points));
    for(int i = 0; i < num_hotels + num_points; i++)
        distance_matrix[i] = (double*)malloc(sizeof(double) * (num_hotels + num_points));

    // FILLING DISTANCE MATRIX ==================================================
    fillDistanceMatrix(distance_matrix, vertices);

    // FILLING MINIMUN DISTANCE TO A HOTEL FOR VERTICES =========================
    fillMinDistsToHotels(&vertices, distance_matrix, num_hotels);

    for(int i = 0; i < MAX_GRASP_ITER; i++) {

        // CONSTRUCTING THE INITIAL SOLUTION
        // Clearing initial_solution
        initial_solution = constructInitialSolution(base_solution, vertices, num_hotels, num_points, distance_matrix, &not_visited_points);
        initial_solution.total_tour_score = calcTourScore(initial_solution);
        initial_solution.total_tour_length = calcTourLength(initial_solution);
        std::cout << "// CONSTRUCTION PHASE ===============================" << std::endl;
        for(int i = 0; i < initial_solution.trips.size(); i++) {
            std::cout << "-----------------------------" << std::endl;
            std::cout << "Constructed trip " << i << ": ";
            for(auto vertex : initial_solution.trips[i].path) {
                std::cout << vertex.index << " ";
            }
            std::cout << std::endl;
            std::cout << "Trip total size: " << calcTripLength(initial_solution.trips[i], distance_matrix) << std::endl;
            std::cout << "Trip total score: " << calcTripScore(initial_solution.trips[i]) << std::endl;     
            std::cout << "-----------------------------" << std::endl;
        }
        std::cout << "Tour total length: " << initial_solution.total_tour_length << std::endl;
        std::cout << "Tour total score: " << initial_solution.total_tour_score << std::endl;

        std::cout << "Not visited points:" << std::endl;
        for(auto point : not_visited_points) {
            std::cout << point.index << " ";
        }
        std::cout << std::endl;
        std::cout << "==================================================" << std::endl;

        // EXECUTES THE LOCAL SEARCH
        std::cout << "// LOCAL SEARCH PHASE ============================" << std::endl;
        Tour best_solution = localSearch(initial_solution, distance_matrix, not_visited_points);
        best_solution.total_tour_score = calcTourScore(best_solution);
        best_solution.total_tour_length = calcTourLength(best_solution);
        
        for(int i = 0; i < best_solution.trips.size(); i++) {
            std::cout << "Local searched trip " << i << ": ";
            for(auto vertex : best_solution.trips[i].path) {
                std::cout << vertex.index << " ";
            }
            std::cout << std::endl;
            std::cout << "Trip total size: " << best_solution.trips[i].total_trip_length << std::endl;
            std::cout << "Trip total score: " << calcTripScore(best_solution.trips[i]) << std::endl;     
            std::cout << std::endl;
        }
        std::cout << "Tour total length: " << best_solution.total_tour_length << std::endl;
        std::cout << "Tour total score: " << best_solution.total_tour_score << std::endl;
        std::cout << "==================================================" << std::endl;
        

        if(best_solution.total_tour_score > best_grasp_score) {
            best_grasp_score = best_solution.total_tour_score;
            best_grasp_solution = best_solution;
        }
    }

    for(int i = 0; i < best_grasp_solution.trips.size(); i++) {
        std::cout << "Best solution found " << i << ": ";
        for(auto vertex : best_grasp_solution.trips[i].path) {
            std::cout << vertex.index << " ";
        }
        std::cout << std::endl;
        std::cout << "Trip total size: " << best_grasp_solution.trips[i].total_trip_length << std::endl;
        std::cout << "Trip total score: " << calcTripScore(best_grasp_solution.trips[i]) << std::endl;     
        std::cout << std::endl;
    }
    std::cout << "Tour total length: " << best_grasp_solution.total_tour_length << std::endl;
    std::cout << "Tour total score: " << best_grasp_solution.total_tour_score << std::endl;
    std::cout << "==================================================" << std::endl;

    // MEMORY RELEASING =========================================================
    for(int i = 0; i < num_hotels + num_points; i++) free(distance_matrix[i]);
    free(distance_matrix);
}

