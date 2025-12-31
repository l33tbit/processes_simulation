#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>  // add this at the top of your file
#include <limits.h>  // for int_min, int_max

#include "../../lib/structs/execution_queue.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/simulator.h"
#include "../../lib/structs/ressource_manager.h"

#include "../../src/implementation/execution_queue.c"
#include "../../src/implementation/schedular_algos.c"
#include "../../src/implementation/schedular_out.c"

#include "../../src/implementation/output.c"

PCB* op_simul_get_ready_queue_head(struct SIMULATOR* self);

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
TASK op_need_ressources(ORDONNANCEUR* self, RESSOURCE_ELEMENT* ressource_needed) {
    
    // call through the function pointer - note this creates a circular dependency
    // the function pointer signature has been updated to match
    return self->need_ressources(self, ressource_needed);

}

TASK op_ressource_is_free(ORDONNANCEUR* self, RESSOURCE ressource) {
    
    return self->simulator->signal_ressource_is_free(self->simulator, ressource);

}

TASK op_update_cpu_time_used(ORDONNANCEUR* self, float inc) {

    return self->simulator->update_cpu_time_used(self->exec_proc, inc);

}

PROCESS_UPDATE op_update_process(ORDONNANCEUR* self,PCB* process, float *temps_fin, float *tournround) {
    
    printf("DEBUG: op_update_process called with temps_fin %p\n", temps_fin);
    fflush(stdout);
    return self->simulator->process_manager->update_process(self->simulator->process_manager, process, temps_fin, tournround);
    
}

TASK op_ask_sort_rt(ORDONNANCEUR* self) {

    return self->simulator->ask_sort_rt(self->simulator);

}

TASK op_ask_sort_priority(ORDONNANCEUR* self) {

    return self->simulator->ask_sort_priority(self->simulator);
    
}


TASK op_ask_sort_sjf(ORDONNANCEUR* self) {

    return self->simulator->ask_sort_sjf(self->simulator);
}

PCB* op_sched_ask_for_next_ready_element(ORDONNANCEUR* self, PCB* current_pcb) {

    PCB* response = self->simulator->simul_ask_for_next_ready_element(self->simulator, current_pcb); 

    return response;
}


TASK op_check_ressource_disponibility(ORDONNANCEUR* self, RESSOURCE ressource) {

    return self->simulator->check_ressource_disponibility(self->simulator, ressource);

}


// update statistics


WORK_RETURN sched_kill(ORDONNANCEUR* self) {

    // free execution segments
    EXECUTION_SEGMENT* current = self->execution_segments_head;
    while (current != NULL) {
        EXECUTION_SEGMENT* next = current->next;
        free(current);
        current = next;
    }

    free(self->statistics);

    
    if (self->execution_queue->kill(self->execution_queue) != WORK_DONE) {

        printf("execution_queue error killed\n\n\n");
    
        return WORK_ERROR;
    }

    free(self);

    printf("schedular killed\n\n\n");

    return WORK_DONE;
}



PCB* op_sched_get_ready_queue_head(ORDONNANCEUR* self) {
    return op_simul_get_ready_queue_head(self->simulator);
}

sched_ressources_return op_check_ressources(ORDONNANCEUR* self, PCB* exec_proc) {  // changed parameter to pcb*

    INSTRUCTION* next_instruct = exec_proc->instructions_head;  // get instructions from pcb
    RESSOURCE ressources[ressource_number] = {0};
    int ressource_count = 0;

    while (next_instruct != NULL && ressource_count < ressource_number) {

        RESSOURCE ressource_needed = next_instruct->type;
        bool already = false;

        // check if resource is already in the array
        for (int i = 0; i < ressource_count; i++) {
            if (ressources[i] == ressource_needed) {
                already = true;
                break;  // exit loop early if found
            }
        }

        // if not already in array, add it
        if (!already) {
            ressources[ressource_count] = ressource_needed;
            ressource_count++;
        }

        next_instruct = next_instruct->next;
    }

    // check all resources first
    bool resources_available = true;
    for (int i = 0; i < ressource_count; i++) {
        if (self->check_ressource_disponibility(self, ressources[i]) == TASK_ERR) {
            resources_available = false;
            break;  // exit early if any resource is unavailable
        }
    }

    // if any resource is unavailable, block the process and continue to next iteration
    if (!resources_available) {
        if (self->sched_push_to_blocked_queue(self, exec_proc) == PUSHED) {  // use exec_proc parameter
            return PROCESS_BLOCKED;  // this will skip execution and go to next while loop iteration
        } else {
            fprintf(stderr, "ERROR ON: sched_push_to_blocked_queue returned PUSH_ERROR\n");
            return PROCESS_ERROR;
        }
    }
    
    return RESSOURCES_AVAILABLE;
}

TASK op_sched_update_process_manager(struct ORDONNANCEUR* self, float temps, float* runed) {

    return self->simulator->simul_update_process_manager(self->simulator, NULL, &temps, runed); // null because of file buffer we wont use it here
}


TASK op_sched_remove_from_ready_queue(ORDONNANCEUR* self, PCB* pcb, float completion_time) {
    return self->simulator->remove_from_ready_queue(self->simulator, pcb, completion_time);

}


push_return op_sched_push_to_blocked_queue(ORDONNANCEUR* self, PCB* pcb) {

    return self->simulator->simul_push_to_blocked_queue(self->simulator, pcb);
}

float op_sched_get_max_arrival_time(ORDONNANCEUR* self) {
    return self->simulator->get_max_arrival_time(self->simulator);
}

TASK op_sched_update_ready_queue(ORDONNANCEUR* self, bool circular) {
    return self->simulator->update_ready_queue(self->simulator, circular);
}


INITIALIZATION op_sched_init(ORDONNANCEUR* self, SIMULATOR* simulator, OPTIONS* options) {

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
    self->check_ressources = op_check_ressources;
    self->update_process = op_update_process;
    self->kill = sched_kill;
    self->sched_push_to_blocked_queue = op_sched_push_to_blocked_queue;
    self->sched_get_ready_queue_head = op_sched_get_ready_queue_head;
    self->sched_update_process_manager = op_sched_update_process_manager;
    self->get_max_arrival_time = op_sched_get_max_arrival_time;
    self->update_ready_queue = op_sched_update_ready_queue;
    self->ask_sort_sjf = op_ask_sort_sjf;
    self->remove_from_ready_queue = op_sched_remove_from_ready_queue;
    
    switch (options->algorithm) {
        case 0:
            self->select = select_rr;
            break;
        case 1:
            self->select = select_fcfs;
            break;
        case 2:
            self->select = select_srtf;
            break;
        case 3:
            self->select = select_ppp;
            break;
        case 4:
            self->select = select_ppn;
            break;
        case 5:
            self->select = select_sjf;
            break;
    }


    self->simulator = simulator;

    self->algorithm = options->algorithm;

    self->quantum = options->quantum;

    self->exec_proc = NULL;

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

    self->execution_segments_head = NULL;
    self->current_segment = NULL;

    return INIT_SUCC;
}
