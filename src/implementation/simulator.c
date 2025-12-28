#pragma once

#include "../../lib/structs/process.h"
#include "../../lib/structs/simulator.h"
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/ressource_manager.h"
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/schedular.h"

#include "../../src/implementation/schedular.c"
#include "../../src/implementation/ressource_manager.c"
#include "../../src/implementation/process_manager.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>




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

ORDONNANCEUR* op_start_schedular(Algorithms algorithm, int quantum, SIMULATOR* self) {

    ORDONNANCEUR* schedular = (ORDONNANCEUR*)malloc(sizeof(ORDONNANCEUR));

    if (schedular == NULL) {
        fprintf(stderr, "ERROR ON: op_start_schedular , schedular allocation returned NULL\n");
        exit(1);
    }

    self->schedular = schedular;

    return schedular;
}

bool op_simul_update_process_manager(SIMULATOR* self, FILE* processus_buffer, float* temps) {

    bool updated = false;
    
    if (temps) {
        updated = self->process_manager->update_self_temps(self->process_manager, *temps);
    }

    if (processus_buffer) {
        // logic of modifying buffer
    }

    return updated;
}


bool op_signal_ressource_is_free(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    bool response = ressource_manager->mark_ressource_available(ressource_manager, ressource);

    return response;
}

bool op_sched_check_instruction_disponibility(INSTRUCTION* instruction) {

}

bool op_simul_check_instruction_disponibility(SIMULATOR* self, RESSOURCE ressource) {
    bool result = self->ressource_manager->check_if_ressource_available(self->ressource_manager, ressource);

    return result;
}

bool op_signal_ressource_free(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    bool response = ressource_manager->mark_ressource_available(ressource_manager, ressource);

    return response;
}

PCB* op_simul_ask_for_next_ready_element(SIMULATOR* self, PCB* process) {
    PCB* response = self->process_manager->get_next_ready_element(self->process_manager, process);

    return response;
}

bool op_simul_ask_sort_rt(SIMULATOR* self) {
    
    self->process_manager->ready_queue_head = self->process_manager->sort_by_rt(self->process_manager);

    return true;
}

bool op_simul_ask_sort_priority(SIMULATOR* self) {
    self->process_manager->ready_queue_head = self->process_manager->sort_by_priority(self->process_manager);

    return true;
}

process_update op_sched_update_process(SIMULATOR* self, PCB* pcb, time_t* temps_fin, float* cpu_temps_used) { // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time

    return self->process_manager->update_process(self->process_manager, pcb, temps_fin, cpu_temps_used);
}

WORK_RETURN op_simul_stop(SIMULATOR* self) {

    WORK_RETURN stop_process_manager = self->process_manager->kill(self->process_manager);

    WORK_RETURN stop_schedular = self->schedular->kill(self->schedular);

    WORK_RETURN stop_ressource_manager = self->ressource_manager->kill(self->ressource_manager);

    if (stop_process_manager != WORK_DONE || stop_ressource_manager != WORK_DONE || stop_schedular != WORK_DONE) {
        return WORK_ERROR;
    }

    return WORK_DONE;

}

// ------------------------helpers


OPTIONS* op_ask_for_options() {
    int algorithm = -1; // init to -1, for validatoion and force user to choose
    float quantum = -1; // init to -1, if not rr prevent random value

    do {
        printf("Hello\n\twhich algorithm you wish to run :\n");
        printf("0: RR\n");
        printf("1: SRTF\n");
        printf("2: PPP\n");
        printf("3: FCFS\n");
        printf("4: SJF\n");
        scanf("%d", &algorithm);

        if (algorithm == 0) {
            printf("quantum (in ms)\n");
            scanf("%f", &quantum);
        }

    } while (algorithm != 0 && algorithm != 1 && algorithm != 2 && algorithm != 3 && algorithm != 4);

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

bool op_simul_update_ready_queue(SIMULATOR* self, bool circular) {
    
    printf("trying updaaaaaaaation");


    bool result = self->process_manager->update_read_queue(self->process_manager, circular);

    
    return result;
}


WORK_RETURN op_simul_work(SIMULATOR* self, OPTIONS* options) {

    WORK_RETURN sched_work = self->schedular->select(self->schedular, self->schedular->quantum);

    if (sched_work != WORK_DONE) {

        fprintf(stderr, "ERROR ON: op_simul_work returned WORK_ERROR\n");
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
    self->stop = op_simul_stop;
    self->ask_for_options = op_ask_for_options;
    self->create_process_manager = op_create_process_manager;
    self->create_schedular = op_create_schedular;
    self->create_ressource_manager = op_create_ressource_manager;
    self->simul_ask_for_next_ready_element = op_simul_ask_for_next_ready_element;
    self->simul_get_ready_queue_head = op_simul_get_ready_queue_head;
    self->simul_push_to_blocked_queue = op_simul_push_to_blocked_queue;
    self->simul_update_process_manager = op_simul_update_process_manager;
    self->get_max_arrival_time = op_simul_get_max_arrival_time;
    self->update_ready_queue = op_simul_update_ready_queue;
    self->ask_sort_rt = op_simul_ask_sort_rt;



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
