#pragma once

typedef struct SIMULATOR SIMULATOR;
#include "../../lib/structs/simulator.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/ressource_manager.h"
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/schedular.h"

#include "../../src/implementation/schedular.c"
#include "../../src/implementation/ressource_manager.c"
#include "../../src/implementation/process_manager.c"
#include "../../lib/structs/enums.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// extern globals from unit_tester
extern int global_algorithm;
extern float global_quantum;

// requiring functions
SIMULATOR* op_start(SIMULATOR* self, char* path) {
    
    PROCESS_MANAGER* process_manager = (PROCESS_MANAGER*)malloc(sizeof(PROCESS_MANAGER));
    
    RESSOURCE_MANAGER* ressource_manager = (RESSOURCE_MANAGER*)malloc(sizeof(RESSOURCE_MANAGER));

    ORDONNANCEUR* schedular = (ORDONNANCEUR*)malloc(sizeof(ORDONNANCEUR));

    if (process_manager == NULL || ressource_manager == NULL || schedular == NULL) {
        fprintf(stderr, "ERROR ON: op_start , allocation returned NULL\n");
        exit(1);
    }

    self->process_manager = process_manager;
    self->ressource_manager = ressource_manager;
    self->schedular = schedular;


    return self;
}

//initialize managers
PROCESS_MANAGER* op_start_process_manager(SIMULATOR* self, FILE* buffer) {

    PROCESS_MANAGER* process_manager = (PROCESS_MANAGER*)malloc(sizeof(PROCESS_MANAGER));
    
    if (process_manager == NULL) {
        fprintf(stderr, "ERROR ON: op_start_ressource_manager , process_manager allocation returned NULL\n");
        exit(1);
    }

    self->process_manager = process_manager;

    return process_manager;
}

RESSOURCE_MANAGER* op_start_ressource_manager(SIMULATOR* self) {
    
    RESSOURCE_MANAGER* ressource_manager = (RESSOURCE_MANAGER*)malloc(sizeof(RESSOURCE_MANAGER));

    if (ressource_manager == NULL) {
        fprintf(stderr, "ERROR ON: op_start_ressource_manager , ressource_manager allocation returned NULL\n");
        exit(1);
    }

    self->ressource_manager = ressource_manager;

    return ressource_manager;
}

ORDONNANCEUR* op_start_schedular(SIMULATOR* self, Algorithms algorithm, int quantum) {

    ORDONNANCEUR* schedular = (ORDONNANCEUR*)malloc(sizeof(ORDONNANCEUR));

    if (schedular == NULL) {
        fprintf(stderr, "ERROR ON: op_start_schedular , schedular allocation returned NULL\n");
        exit(1);
    }

    self->schedular = schedular;

    return schedular;
}

TASK op_simul_update_process_manager(SIMULATOR* self, FILE* processus_buffer, float* temps, float* runed) {

    bool updated = false;
    
    if (temps) {
        updated = self->process_manager->update_self_temps(self->process_manager, *temps, runed);
    }

    if (processus_buffer) {
        // logic of modifying buffer
    }

    return updated;
}


TASK op_signal_ressource_is_free(SIMULATOR* self, RESSOURCE ressource) {
    return self->ressource_manager->mark_ressource_available(self->ressource_manager, ressource);

}

TASK op_sched_check_instruction_disponibility(INSTRUCTION* instruction) {
    // need to add logic
}

TASK op_simul_check_instruction_disponibility(SIMULATOR* self, RESSOURCE ressource) {
    return self->ressource_manager->check_if_ressource_available(self->ressource_manager, ressource);
}

TASK op_signal_ressource_free(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    return ressource_manager->mark_ressource_available(ressource_manager, ressource);

}

PCB* op_simul_ask_for_next_ready_element(SIMULATOR* self, PCB* process) {
    return self->process_manager->get_next_ready_element(self->process_manager, process);

}

TASK op_simul_ask_sort_rt(SIMULATOR* self) {
    self->process_manager->ready_queue_head = self->process_manager->sort_by_rt(self->process_manager);

    return TASK_SUCC;
}

TASK op_simul_ask_sort_priority(SIMULATOR* self) {
    self->process_manager->ready_queue_head = self->process_manager->sort_by_priority(self->process_manager);

    return TASK_SUCC;
}

TASK op_simul_ask_sort_sjf(SIMULATOR* self) {
    self->process_manager->ready_queue_head = self->process_manager->sort_by_sjf(self->process_manager);

    return TASK_SUCC;

}

PROCESS_UPDATE op_sched_update_process(SIMULATOR* self, PCB* pcb, time_t* temps_fin, float* cpu_temps_used) { // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time
    // convert time_t* to float* for process_manager->update_process
    // note: time_t is typically long, need to convert to float
    float temps_fin_float_value;
    float* temps_fin_float = NULL;
    
    if (temps_fin != NULL) {
        temps_fin_float_value = (float)*temps_fin;
        temps_fin_float = &temps_fin_float_value;
    }
    
    return self->process_manager->update_process(self->process_manager, pcb, temps_fin_float, cpu_temps_used);
}

WORK_RETURN op_simul_stop(SIMULATOR* self) {

    
    WORK_RETURN stop_process_manager = self->process_manager->kill(self->process_manager);


    WORK_RETURN stop_schedular = self->schedular->kill(self->schedular);


    WORK_RETURN stop_ressource_manager = self->ressource_manager->kill(self->ressource_manager);


    if (stop_process_manager != WORK_DONE || stop_ressource_manager != WORK_DONE || stop_schedular != WORK_DONE) {
        printf("%d\n%d\n%d\n", stop_process_manager, stop_ressource_manager, stop_schedular);
        return WORK_ERROR;
    }

    return WORK_DONE;

}

// ------------------------helpers


OPTIONS* op_ask_for_options() {
    int algorithm = global_algorithm;
    float quantum = global_quantum;

    // if quantum not set, use default
    if (quantum <= 0) {
        quantum = 2.0f; // default quantum for rr
    }

    OPTIONS* options = (OPTIONS*)malloc(sizeof(OPTIONS));
    
    if (options == NULL) {
        fprintf(stderr, "ERROR ON: op_ask_for_options , options allocation returned NULL\n");
        exit(1);
    }

    options->algorithm = algorithm;
    options->quantum = quantum;
    
    return options;
}



PROCESS_MANAGER* op_create_process_manager() {
    
    PROCESS_MANAGER* process_manager = (PROCESS_MANAGER*)malloc(sizeof(PROCESS_MANAGER)); // allocate process manager
    
    if (process_manager == NULL) {
        fprintf(stderr, "ERROR ON: create_process_manager , process_manager allocation failed\n");
        exit(1);
    }

    return process_manager;
}

ORDONNANCEUR* op_create_schedular(Algorithms algorithm, int quantum) {

    ORDONNANCEUR* schedular = (ORDONNANCEUR*)malloc(sizeof(ORDONNANCEUR)); // init the schedular    

    if (schedular == NULL) {
        fprintf(stderr, "ERROR ON: op_create_schedular: Failed to allocate memory for schedular\n");
        exit(1);
    }

    schedular->algorithm = algorithm;
    schedular->quantum = quantum;

    return schedular;
}


RESSOURCE_MANAGER* op_create_ressource_manager() {

    RESSOURCE_MANAGER* ressource_manager = (RESSOURCE_MANAGER*)malloc(sizeof(RESSOURCE_MANAGER)); // init the schedular    

    if (ressource_manager == NULL) {
        fprintf(stderr, "ERROR ON: op_create_schedular: Failed to allocate memory for schedular\n");
        exit(1);
    }

    return ressource_manager;
}

TASK op_simul_update_ready_queue(SIMULATOR* self, bool circular) {
    

    return self->process_manager->update_read_queue(self->process_manager, circular);

}


WORK_RETURN op_simul_work(SIMULATOR* self, OPTIONS* options) {

    WORK_RETURN sched_work = self->schedular->select(self->schedular, self->schedular->quantum);
        printf("aaaaaaaaaaaa\n");
        fflush(stdout);
    if (sched_work != WORK_DONE) {
        fprintf(stderr, "ERROR ON: sched_work returned WORK_ERROR\n");
        return WORK_ERROR;
    
    }
    
    return WORK_DONE;
}

push_return op_simul_push_to_blocked_queue(SIMULATOR* self, PCB* pcb) {

    return self->process_manager->add_process_to_blocked_queue(self->process_manager, pcb);
}


PCB* op_simul_get_ready_queue_head(SIMULATOR* self) {
    return self->process_manager->get_ready_queue_head(self->process_manager);
}

float op_simul_get_max_arrival_time(SIMULATOR* self) {

    return self->process_manager->get_max_arrival_time(self->process_manager);
}

TASK op_simul_remove_from_ready_queue(SIMULATOR* self, PCB* pcb, float completion_time) {
    self->process_manager->ready_queue_head = self->process_manager->delete_from_ready_queue(self->process_manager, pcb, completion_time);
    
    return TASK_SUCC;
}


WORK_RETURN op_simul_init(SIMULATOR* self, FILE* buffer) {

    if (buffer == NULL) {
        fprintf(stderr, "ERROR ON: op_simul_init, buffer is NULL\n");
        return WORK_ERROR;
    }

    // ------- simulator

    self->work = op_simul_work;
    self->update_process = op_sched_update_process;
    self->check_ressource_disponibility = op_simul_check_instruction_disponibility;
    self->signal_ressource_is_free = op_signal_ressource_is_free;
    self->ask_for_options = op_ask_for_options;
    self->create_process_manager = op_create_process_manager;
    self->create_schedular = op_create_schedular;
    self->create_ressource_manager = op_create_ressource_manager;
    self->simul_ask_for_next_ready_element = op_simul_ask_for_next_ready_element;
    // self->simul_get_ready_queue_head = op_simul_get_ready_queue_head;
    self->simul_push_to_blocked_queue = op_simul_push_to_blocked_queue;
    self->simul_update_process_manager = op_simul_update_process_manager;
    self->get_max_arrival_time = op_simul_get_max_arrival_time;
    self->update_ready_queue = op_simul_update_ready_queue;
    self->ask_sort_rt = op_simul_ask_sort_rt;
    self->ask_sort_sjf = op_simul_ask_sort_sjf;
    self->ask_sort_priority = op_simul_ask_sort_priority;
    self->remove_from_ready_queue = op_simul_remove_from_ready_queue;
    self->stop = op_simul_stop;
    



    // ---------- process manager

    self->options = self->ask_for_options();
    
    self->process_manager = self->create_process_manager(); // create process manager

    self->process_manager->init = op_pro_init; // assign the initializer function
    
    self->process_manager->init(self->process_manager, buffer, self->options->algorithm);
    
    
    // ---------- ressource list

    self->ressource_manager = self->create_ressource_manager(); // create ressource manager

    self->ressource_manager->init = op_rm_init; // assign the initializer function

    self->ressource_manager->init(self->ressource_manager);


    // ---------- schedular

    self->schedular = self->create_schedular(self->options->algorithm, self->options->quantum); // create schedular    

    self->schedular->init = op_sched_init; // assign the initialization function to schedular

    self->schedular->init(self->schedular, self, self->options); // then init it


    return WORK_DONE;
}
