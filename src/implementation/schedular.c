#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "../../lib/structs/execution_queue.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/simulator.h"

#include "../../src/implementation/execution_queue.c"

ORDONNANCEUR_STATISTICS* op_create_statistics() {   

    ORDONNANCEUR_STATISTICS* statistics = (ORDONNANCEUR_STATISTICS*)malloc(sizeof(ORDONNANCEUR_STATISTICS)); // init the statistics

    if (statistics == NULL) {
        fprintf(stderr, "ERROR ON: op_create_statistics: Failed to allocate memory for statistics\n");
        exit(1);
    }

    return statistics;
}

EXECUTION_QUEUE* op_create_execution_queue() {
    EXECUTION_QUEUE* execution_queue = (EXECUTION_QUEUE*)malloc(sizeof(EXECUTION_QUEUE));

    if (execution_queue == NULL) {
        fprintf(stderr, "ERROR ON: op_create_execution_queue: Failed to allocate memory for execution_queue\n");
        exit(1);
    }

    return execution_queue;
}


// ordonnanceur to simulator
bool op_need_ressources(ORDONNANCEUR* self, RESSOURCE_ELEMENT* ressource_needed) {
    bool response = self->need_ressources(ressource_needed);

    return response;
}

bool op_ressource_is_free(ORDONNANCEUR* self, RESSOURCE ressource) {
    bool response = self->simulator->signal_ressource_is_free(self->simulator, ressource);

    return response;
}

bool op_update_cpu_time_used(ORDONNANCEUR* self, float inc) {
    // bool response = process->update_cpu_time_used(process, inc);
    bool response = self->simulator->update_cpu_time_used(self->exec_proc, inc);


    return response;
}

bool op_update_process(ORDONNANCEUR* self,PCB* process, float *temps_fin, float *tournround) {
    
    return self->simulator->update_process(self->simulator, process, temps_fin, tournround);
    
}

bool op_ask_sort_rt(ORDONNANCEUR* schedular) {

    bool response = schedular->ask_sort_rt(schedular);

    return response;
}

bool op_ask_sort_priority(SIMULATOR* simulator) {

    bool response = simulator->ask_sort_priority(simulator);

    return response;
}

PCB* op_sched_ask_for_next_ready_element(ORDONNANCEUR* self, PCB* current_pcb) {

    PCB* response = self->simulator->simul_ask_for_next_ready_element(self->simulator, current_pcb); 

    return response;
}


bool op_check_ressource_disponibility(ORDONNANCEUR* self, RESSOURCE ressource) {

    bool result = self->simulator->check_ressource_disponibility(self->simulator, ressource);

    return result;
}

// // process_manager & schedular related function
// bool op_check_ressource_disponibility(SIMULATOR* self, RESSOURCE ressource_needed) {

//     bool response = self->ressource_manager->check_if_ressource_available(ressource_needed);

//     return response;
// }


// update statistics
bool op_update_schedular_statistics(ORDONNANCEUR* self, float* exec_time, float* burst, float* temp_attente, bool finished) { // must check nullty

    if (finished == true) {
        
            self->statistics->processus_termine_count++;
            self->statistics->cpu_total_temps_usage += *exec_time;
            self->statistics->total_turnround += *burst;
            self->statistics->troughtput = (float)self->statistics->processus_termine_count / self->statistics->total_turnround;
            self->statistics->total_temps_attente += self->exec_proc->statistics->temps_attente;

    } else {

            self->statistics->context_switch++;
            self->statistics->cpu_total_temps_usage += *exec_time;
    }
    
    return true;
}


WORK_RETURN sched_kill(ORDONNANCEUR* self) {

    printf("execution_queue hit killed\n\n\n");


    free(self->statistics);
    
    if (self->execution_queue->kill(self->execution_queue) != WORK_DONE) {

        printf("execution_queue error killed\n\n\n");
    
        return WORK_ERROR;
    }

    free(self);

    printf("schedular killed\n\n\n");

    return WORK_DONE;
}

#include "../../src/unit_testing/unit_tester.c"

WORK_RETURN select_rr(ORDONNANCEUR* self, float quantum) {

    printf("hiiiiiit select_rr\n\n\n");

    do {
    
        self->exec_proc = self->sched_ask_for_next_ready_element(self, self->exec_proc); // get the next element


        print_pcb(self->exec_proc);


        if (self->execution_queue->execute_rr((self->exec_proc->remaining_time < quantum) ? self->exec_proc->remaining_time : quantum) != WORK_DONE) { // if remaining time is less than the quantum then execute for remaining time not quantum else execute for quantum
            return WORK_ERROR;
        }

        time_t daba;

        time_t* temps_fin_ptr = NULL;


        if (self->exec_proc->remaining_time < quantum) {

            float n_quantum = self->exec_proc->remaining_time;
            
            time(&daba);
            temps_fin_ptr = &daba;

            process_update update = self->update_process(self, self->exec_proc, temps_fin_ptr, &quantum);
            
            if (update != UPDATED) {
                return UPDATE_ERROR;
            }

            if (
                self->update_schedular_statistics(self, &n_quantum, &self->exec_proc->burst_time, &self->exec_proc->statistics->temps_attente, true) != true // changed not good
            ) {
                return UPDATE_ERROR;
            }
            
        } else {

            process_update update = self->update_process(self, self->exec_proc, temps_fin_ptr, &quantum);

            if (update != UPDATED) {
                return UPDATE_ERROR;
            }

            if (
                self->update_schedular_statistics(self, &quantum, NULL, NULL, false) != true // changed not good
            ) {
                return UPDATE_ERROR;
            }

        }

    } while (self->exec_proc != NULL);

    printf("all processes are terminated\n");


    return WORK_DONE;
}

ORDONNANCEUR* op_sched_init(ORDONNANCEUR* self, SIMULATOR* simulator, OPTIONS* options) {

    // function assigning
    self->create_execution_queue = op_create_execution_queue;
    self->create_statistics = op_create_statistics;
    self->need_ressources = op_need_ressources;
    self->ressource_is_free = op_ressource_is_free;
    self->ask_sort_priority = op_ask_sort_priority;
    self->ask_sort_rt = op_ask_sort_rt;
    self->sched_ask_for_next_ready_element = op_sched_ask_for_next_ready_element;
    self->update_schedular_statistics = op_update_schedular_statistics;
    self->check_ressource_disponibility = op_check_ressource_disponibility;
    self->update_process = op_update_process;
    self->kill = sched_kill;
    
    switch (options->algorithm) {
        case 0:
            self->select = select_rr;
            break;
    }


    self->simulator = simulator;

    self->algorithm = options->algorithm;

    self->quantum = options->quantum;

    self->statistics = self->create_statistics(); // create statistics and assign it 

    self->statistics->cpu_total_temps_usage = 0;
    self->statistics->context_switch = 0;
    self->statistics->processus_termine_count = 0;
    self->statistics->total_turnround = 0;
    self->statistics->troughtput = 0;
    self->statistics->total_temps_attente = 0;

    self->execution_queue = self->create_execution_queue(); // create execution queue and assign it

    self->execution_queue->init = ex_init;

    self->execution_queue->init(self->execution_queue);

    return self;
}
