typedef struct SIMULATOR SIMULATOR;
#include "../lib/structs/simulator.h"
#include "../src/implementation/simulator.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int global_algorithm = 4;
float global_quantum = 0.0f;
char* global_file_path = "/home/zeus/projects/final/processus_simulation/src/unit_testing/data.csv";

void simulator_work(int algorithm, float quantum, char* file_path) { 

   SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));

    
    FILE* buffer = fopen(file_path, "r");
    if (buffer == NULL) {
        perror("ERROR: Failed to open data.csv");
        exit(1);
    }

    simulator->init = op_simul_init;

    OPTIONS* options = (OPTIONS*)malloc(sizeof(OPTIONS));

    if ( options == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the options\n");
        exit(1);
    }

    options->algorithm = algorithm;
    options->quantum = quantum;

    simulator->init(simulator, buffer);

    fflush(stdout);

    simulator->work(simulator, simulator->options);

    // Print process details for verification
    PCB* current = simulator->process_manager->process_table_head;
    printf("\n=== Process Details ===\n");
    while (current != NULL) {
        printf("PID: %d, Arrival: %.2f, Completion: %.2f, Turnaround: %.2f, Waiting: %.2f\n",
               current->pid,
               current->statistics->temps_arrive,
               current->statistics->temps_fin,
               current->statistics->tournround,
               current->statistics->temps_attente);
        current = current->pid_sibling_next;
    }

    if (simulator->stop(simulator) != WORK_DONE) {
        perror("ERROR: Failed AT simulator->stop(simulator)");
        exit(1);
    }

    fclose(buffer);

}



int main(int argc, char *argv[]) {

    if (argc > 1) {
        global_algorithm = atoi(argv[1]);
    }
    if (argc > 2) {
        global_quantum = atof(argv[2]);
    } else {
        global_quantum = -1; 
    } 
    if (argc > 3) {
        global_file_path = argv[3];
    }

    simulator_work(global_algorithm, global_quantum, global_file_path);

    return 0;
}