#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Struct to hold algorithm results
typedef struct {
    char name[10];
    float avg_waiting_time;
    float avg_turnaround_time;
    float throughput;
} AlgoResult;

// Function to run simulation for a specific algorithm
void run_simulation_for_algo(int algo_code) {
    char command[50];
    sprintf(command, "./unit_tester %d", algo_code);
    system(command);
}

// Function to parse performance from file
void parse_performance(AlgoResult* result, const char* algo_name) {
    FILE* file = fopen("/home/zeus/projects/processus_simulation/src/python_ui/outputs/performance_summary.txt", "r");
    if (!file) {
        printf("Error opening performance.txt\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Average Waiting Time:")) {
            sscanf(line, "Average Waiting Time: %f", &result->avg_waiting_time);
        } else if (strstr(line, "Average Turnaround Time:")) {
            sscanf(line, "Average Turnaround Time: %f", &result->avg_turnaround_time);
        } else if (strstr(line, "Throughput:")) {
            sscanf(line, "Throughput: %f", &result->throughput);
        }
    }
    fclose(file);
    strcpy(result->name, algo_name);
}

// Comparison function for qsort
int compare_algorithms(const void* a, const void* b) {
    const AlgoResult* algoA = (const AlgoResult*)a;
    const AlgoResult* algoB = (const AlgoResult*)b;

    // Primary: avg_waiting_time ascending
    if (algoA->avg_waiting_time < algoB->avg_waiting_time) return -1;
    if (algoA->avg_waiting_time > algoB->avg_waiting_time) return 1;

    // Secondary: avg_turnaround_time ascending
    if (algoA->avg_turnaround_time < algoB->avg_turnaround_time) return -1;
    if (algoA->avg_turnaround_time > algoB->avg_turnaround_time) return 1;

    // Tertiary: throughput descending
    if (algoA->throughput > algoB->throughput) return -1;
    if (algoA->throughput < algoB->throughput) return 1;

    return 0;
}

// Main function to compare algorithms
int main() {
    const char* algos[] = {"FCFS", "SJF", "SRTF", "RR", "PPP", "PPN"};
    int algo_codes[] = {1, 2, 3, 4, 5, 6};
    int num_algos = 6;
    AlgoResult results[6];

    // Run simulations for each algorithm
    for (int i = 0; i < num_algos; i++) {
        printf("Running simulation for %s...\n", algos[i]);
        run_simulation_for_algo(algo_codes[i]);
        parse_performance(&results[i], algos[i]);
    }

    // Sort the results
    qsort(results, num_algos, sizeof(AlgoResult), compare_algorithms);

    // Print the ranking
    printf("\nAlgorithm Ranking (best to worst):\n");
    printf("Rank | Algorithm | Avg Waiting Time | Avg Turnaround Time | Throughput\n");
    printf("-----|-----------|------------------|----------------------|------------\n");
    for (int i = 0; i < num_algos; i++) {
        printf("%-4d | %-9s | %-16.2f | %-20.2f | %.2f\n",
               i+1, results[i].name, results[i].avg_waiting_time,
               results[i].avg_turnaround_time, results[i].throughput);
    }

    return 0;
}