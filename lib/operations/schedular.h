#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "structs/execution_queue.h"
#include "structs/schedular.h"
#include "structs/ressource.h"
#include "structs/process.h"
#include "structs/simulator.h"

ORDONNANCEUR_STATISTICS* op_create_statistics() {   

    ORDONNANCEUR_STATISTICS* statistics = (ORDONNANCEUR_STATISTICS*)malloc(sizeof(ORDONNANCEUR_STATISTICS)); // init the statistics

    if (statistics == NULL) {
        fprintf(stderr, "ERROR ON: op_create_statistics: Failed to allocate memory for statistics\n");
        exit(1);
    }

    return statistics;
}

// ordonnanceur to simulator
bool op_need_ressources(ORDONNANCEUR* self, RESSOURCE_ELEMENT* ressource_needed) {
    bool response = self->need_ressources(ressource_needed);

    return response;
}

bool op_ressource_is_free(SIMULATOR* simulator, RESSOURCE ressource) {
    bool response = simulator->signal_ressource_free(simulator->ressource_manager, ressource);

    return response;
}

bool op_update_cpu_time_used(ORDONNANCEUR* self, float inc) {
    // bool response = process->update_cpu_time_used(process, inc);
    bool response = self->simulator->update_cpu_time_used(self->exec_proc, inc);


    return response;
}

bool op_update_process(SIMULATOR* simulator,PCB* process, float *temps_fin, float *tournround, float *temps_attente) {
    if (process == NULL) {
        fprintf(stderr, "ERROR ON: op_update_process , process is NULL\n");
        return false;
    }

    if (temps_fin) { // updating temp fin = update tournround

    }    

    if (tournround) {
        process->statistics->tournround = *tournround;
    }

    if (temps_attente) {
        process->statistics->temps_attente = *temps_attente;
    }

    return true;
}

bool op_ask_sort_rt(ORDONNANCEUR* schedular) {

    bool response = schedular->ask_sort_rt(schedular);

    return response;
}

bool op_ask_sort_priority(SIMULATOR* simulator) {

    bool response = simulator->ask_sort_priority(simulator);

    return response;
}

PCB* op_ask_for_next_ready_element(SIMULATOR* simulator, PCB* current_pcb) {

    PCB* response = simulator->ask_for_next_ready_element(simulator, current_pcb); 

    return response;
}

// ordonnanceur to execution queue
EXECUT_RESPONSE op_signal_execute_instruction(ORDONNANCEUR* self, EXECUTION_QUEUE* execution_queue, INSTRUCTION* instruction) {
    
    bool disponibility = self->check_ressource_disponibility(instruction->type);

    EXECUT_RESPONSE response;
    
    if (disponibility == true) {
        
        execution_queue->execute_instruction(instruction); // if it's disponible then exeute it
        instruction->state = COMPLETED; // mark instruction completed
        response = EXEC_SUCCESS; // if disponible return success
        
    } else if (disponibility == false) {

        response = NEED_RESSOURCE; // if not disponible return need ressource
    
    } else {

        response = EXEC_ERROR; // if error return error
    }

    return response;
}

// update statistics
bool op_update_schedular_statistics(ORDONNANCEUR* schedular, float* cpu_total_temps_usage, float* cpu_temps_unoccupied, int* context_switch, float* total_temps_attente, float* process_termine_count, float* throughtput) { // must check nullty
    
    if (cpu_total_temps_usage != NULL)
        schedular->statistics->cpu_total_temps_usage = *cpu_total_temps_usage;
    
    if (cpu_temps_unoccupied != NULL)
        schedular->statistics->cpu_temps_unoccuped = *cpu_temps_unoccupied;
    
    if (context_switch != NULL)
        schedular->statistics->context_switch = *context_switch;

    if (total_temps_attente != NULL)
        schedular->statistics->total_temps_attente = *total_temps_attente;
    
    if (total_temps_attente != NULL)
        schedular->statistics->processus_termine_count = *process_termine_count;
    
    if (throughtput != NULL)
        schedular->statistics->troughtput = *throughtput;

    return true;
}

bool op_check_ressource_disponibility(SIMULATOR* simulator, RESSOURCE ressource) {

    bool result = simulator->check_ressource_disponibility(simulator->ressource_manager, ressource);

    return result;
}

process_return op_execute_process(ORDONNANCEUR* self, PCB* process) {

    self->exec_proc = process; // define the process en cours d'execution

    process->etat = EXECUTION; // change the state

    if ( process->current_instruction == NULL) // if it's the first time executing the process ------ need to be init as 0
        process->current_instruction = process->instructions_head; // define the current instruction

    if (process->etat == READY)
        process->current_instruction = process->instructions_head;

    process_return response;

    while (process->current_instruction != NULL) {

        EXECUT_RESPONSE exec_instruction = self->signal_execute_instruction(self, self->in_execution_queue, process->current_instruction); // can be changed to take 5ms
        
        if (exec_instruction == EXEC_SUCCESS) { // if instruction succedded

            if (process->current_instruction->next == NULL) {

                process->etat = TERMINATED;

                response = FINISHED;

                break;
            }

            process->current_instruction = process->current_instruction->next;

        } else if (exec_instruction == NEED_RESSOURCE) { // if it need's ressources

            process->etat = BLOCKED;

            response = RESSOURCE_NEEDED;

            break;

        } else {

            response = ERROR;

            break;
        }
    }

    return response;
}