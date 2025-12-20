#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "structs/execution_queue.h"
#include "structs/schedular.h"
#include "structs/ressource.h"
#include "structs/process.h"




typedef enum {

    SUCCESS,      // Instruction executed successfully
    EXEC_TERMINATED,   // Process terminated
    EXEC_QUANTUM_EXPIRED, // Time slice expired
    EXEC_ERROR         // General error

} EXECUT_RESPONSE;

typedef struct EXECUTION_RESULT {

    INSTRUCTION* instruction;  // The instruction that was executed
    EXECUT_RESPONSE response;  // Result of execution
    int cycles_taken;          // How many CPU cycles it took
    time_t completion_time;    // When it completed

} EXECUTION_RESULT;

typedef struct EXECUTION_QUEUE {

    int id; // l id du composant en train d'executer
    char process_name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process

    //function
    EXECUTION_RESULT* (*execute_instruction) (INSTRUCTION* instruction);

} EXECUTION_QUEUE;

// on start
EXECUTION_QUEUE* op_create_execution_queue(SIMULATOR* simulator, Algorithms algorithm) {

    EXECUTION_QUEUE* execution_queue = (EXECUTION_QUEUE*)malloc(sizeof(EXECUTION_QUEUE)); // init the execution queue
    
    if (execution_queue == NULL) {
        fprintf(stderr, "ERROR ON: op_create_execution_queue: Failed to allocate memory for execution queue\n");
        exit(1);
    }
    
    return execution_queue;
}

typedef enum {
    RR, SRTF, PPP, FCFS, SJF
} Algorithms;

typedef struct {

    float cpu_total_temps_usage; // somme temps cpu occup total de tout process 
    float cpu_temps_unoccuped; // temps total ou cpu n etait pas utilisé
    int context_switch; // nombre total de changement de processus
    float total_temps_attente; // somme temps waiting de tout processus

    // when process is terminated
    float total_turnround; // somme de tout processus (temps termine - temps arrive)
    int processus_termine_count; // processus terminé
    float troughtput; // processus terminé / total temps en ms

    PCB* current_processus; // processus en cours d execution

} ORDONNANCEUR_STATISTICS;

ORDONNANCEUR_STATISTICS* op_create_statistics() {   

    ORDONNANCEUR_STATISTICS* statistics = (ORDONNANCEUR_STATISTICS*)malloc(sizeof(ORDONNANCEUR_STATISTICS)); // init the statistics

    if (statistics == NULL) {
        fprintf(stderr, "ERROR ON: op_create_statistics: Failed to allocate memory for statistics\n");
        exit(1);
    }

    return statistics;

}

// ordonnanceur to simulator
bool op_need_ressources(EXECUTION_QUEUE* execution_queue, RESSOURCE_ELEMENT* ressource_needed) {

}

bool op_ressource_is_free(RESSOURCE_ELEMENT* ressource) {

}

bool op_update_cpu_time_used(PCB* process, float inc) {

}

bool op_update_process(PCB* process, float temps_fin, float tournround, float temps_attente) {

}

bool op_ask_sort_rt() {

}

bool op_ask_sort_priority() {

}

PCB* op_ask_for_next_ready_element(PCB* current_pcb) {

}

// ordonnanceur to execution queue
EXECUTION_QUEUE_RESPONSE* op_execute_instruction(INSTRUCTION* instruction) {

} 

// update statistics
bool op_update_schedular_statistics(ORDONNANCEUR_STATISTICS* schedular, float cpu_total_temps_usage, float cpu_temps_unoccupied, int context_switch, float total_temps_attente, float process_termine_count, float throughtput) { // must check nullty

}