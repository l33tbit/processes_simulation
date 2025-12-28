#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>  // Add this at the top of your file

#include "../../lib/structs/execution_queue.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/simulator.h"
#include "../../lib/structs/ressource_manager.h"

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
            self->statistics->total_temps_attente += *temp_attente;

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

PCB* op_sched_get_ready_queue_head(ORDONNANCEUR* self) {
    return self->simulator->simul_get_ready_queue_head(self->simulator);
}

sched_ressources_return op_check_ressources(ORDONNANCEUR* self, PCB* exec_proc) {  // Changed parameter to PCB*

    INSTRUCTION* next_instruct = exec_proc->instructions_head;  // Get instructions from PCB
    RESSOURCE ressources[ressource_number] = {0};
    int ressource_count = 0;

    while (next_instruct != NULL && ressource_count < ressource_number) {

        RESSOURCE ressource_needed = next_instruct->type;
        bool already = false;

        // Check if resource is already in the array
        for (int i = 0; i < ressource_count; i++) {
            if (ressources[i] == ressource_needed) {
                already = true;
                break;  // Exit loop early if found
            }
        }

        // If not already in array, add it
        if (!already) {
            ressources[ressource_count] = ressource_needed;
            ressource_count++;
        }

        next_instruct = next_instruct->next;
    }

    // Check all resources first
    bool resources_available = true;
    for (int i = 0; i < ressource_count; i++) {
        if (self->check_ressource_disponibility(self, ressources[i]) == false) {
            resources_available = false;
            break;  // Exit early if any resource is unavailable
        }
    }

    // If any resource is unavailable, block the process and continue to next iteration
    if (!resources_available) {
        if (self->sched_push_to_blocked_queue(self, exec_proc) == PUSHED) {  // Use exec_proc parameter
            return PROCESS_BLOCKED;  // This will skip execution and go to next while loop iteration
        } else {
            fprintf(stderr, "ERROR ON: sched_push_to_blocked_queue returned PUSH_ERROR\n");
            return PROCESS_ERROR;
        }
    }
    
    return RESSOURCES_AVAILABLE;
}

bool op_sched_update_process_manager(struct ORDONNANCEUR* self, float temps) {

    return self->simulator->simul_update_process_manager(self->simulator, NULL, &temps); // NULL because of file buffer we wont use it here
}

WORK_RETURN select_rr(ORDONNANCEUR* self, float quantum) {

    float max_arrival_time = self->get_max_arrival_time(self);

    float temps = 0;
    float proc_temps = 0;

    printf("hiiiiiit select_rr\n\n\n");

    do {

        printf("\n----temps %f\n", temps);
        printf("----temps %f\n", proc_temps);
    
        self->exec_proc = self->sched_ask_for_next_ready_element(self, self->exec_proc); // get the next element

        print_pcb(self->exec_proc);

        if (self->exec_proc  != NULL) {

            // check ressources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }

            if (self->execution_queue->execute_rr((self->exec_proc->remaining_time < quantum) ? self->exec_proc->remaining_time : quantum) != WORK_DONE) { // if remaining time is less than the quantum then execute for remaining time not quantum else execute for quantum
                return WORK_ERROR;
            }

            time_t daba;
            time_t* temps_fin_ptr = NULL;
            float time_used; // Variable to track actual time used

            if (self->exec_proc->remaining_time < quantum) {

                temps += self->exec_proc->remaining_time;


                float n_quantum = self->exec_proc->remaining_time;
                time_used = n_quantum; // Use the actual remaining time
                
                time(&daba);
                temps_fin_ptr = &daba;

                // Pass n_quantum instead of quantum
                process_update update = self->update_process(self, self->exec_proc, temps_fin_ptr, &n_quantum);
                
                if (update != UPDATED) {
                    return UPDATE_ERROR;
                }

                if (
                    self->update_schedular_statistics(self, &n_quantum, &self->exec_proc->burst_time, &self->exec_proc->statistics->temps_attente, true) != true // changed not good
                ) {
                    return UPDATE_ERROR;
                }
                
            } else {

                temps += quantum;

                time_used = quantum; // Use the full quantum
                
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

        }

        // should update temps arrive

        if ((float)(int)temps > proc_temps) {// get floor like 1.2365 -> 1.0000

            proc_temps = (float)(int)temps;

            printf("ppppppppppppp %d", proc_temps);

            if (self->sched_update_process_manager(self, proc_temps) == false) {

                fprintf(stderr, "ERROR ON: sched_update_process_manager failed\n");
                return UPDATE_ERROR;
            
            } else {

                printf("########## hit update readu_queue\n");

                if (self->update_ready_queue(self, true) == false) {
                    fprintf(stderr, "ERROR ON: update_ready_queue failed\n");
                    return UPDATE_ERROR;
                }
            }

        }

    } while (self->exec_proc != NULL);

    if (temps >= max_arrival_time) {
        printf("Scheduler terminated: All processes completed\n");
    } else {
        printf("Scheduler terminated early (time: %.1f, max arrival: %.1f)\n", 
        temps, max_arrival_time);
    }

    return WORK_DONE;
}


push_return op_sched_push_to_blocked_queue(ORDONNANCEUR* self, PCB* pcb) {

    return self->simulator->simul_push_to_blocked_queue(self->simulator, pcb);
}

float op_sched_get_max_arrival_time(ORDONNANCEUR* self) {
    return self->simulator->get_max_arrival_time(self->simulator);
}

bool op_sched_update_ready_queue(ORDONNANCEUR* self, bool circular) {
    return self->simulator->update_ready_queue(self->simulator, circular);
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
    self->check_ressources = op_check_ressources;
    self->update_process = op_update_process;
    self->kill = sched_kill;
    self->sched_push_to_blocked_queue = op_sched_push_to_blocked_queue;
    self->sched_get_ready_queue_head = op_sched_get_ready_queue_head;
    self->check_ressources = op_check_ressources;
    self->sched_update_process_manager = op_sched_update_process_manager;
    self->get_max_arrival_time = op_sched_get_max_arrival_time;
    self->update_ready_queue = op_sched_update_ready_queue;
    
    switch (options->algorithm) {
        case 0:
            self->select = select_rr;
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

    return self;
}
