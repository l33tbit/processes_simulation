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

PCB* op_simul_ask_for_next_ready_element(SIMULATOR* simulator, PCB* process) {
    PCB* response = simulator->process_manager->get_next_ready_element(process);

    return response;
}

bool op_simul_ask_sort_rt(SIMULATOR* simulator) {
    bool response = simulator->process_manager->sort_by_rt(simulator->process_manager);

    return response;
}

bool op_simul_ask_sort_priority(SIMULATOR* simulator) {
    bool response = simulator->process_manager->sort_by_priority(simulator->process_manager);

    return response;
}

WORK_RETURN op_run(SIMULATOR* self, OPTIONS options) {

    self->schedular->select(self->schedular, self->schedular->quantum);

    return WORK_DONE;
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


OPTIONS op_ask_for_options() {
    int algorithm;
    float quantum;

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

    OPTIONS return_value;
    return_value.algorithm = algorithm;
    return_value.quantum = quantum;
    
    return return_value;
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



WORK_RETURN op_simul_init(SIMULATOR* self, FILE* buffer) {

    self->run = op_run;
    self->update_process = op_sched_update_process;
    self->check_ressource_disponibility = op_simul_check_instruction_disponibility;
    self->signal_ressource_is_free = op_signal_ressource_is_free;
    self->stop = op_simul_stop;
    self->ask_for_options = op_ask_for_options;
    self->create_process_manager = op_create_process_manager;
    self->create_schedular = op_create_schedular;
    self->create_ressource_manager = op_create_ressource_manager;

    
    // ---------- process manager
    printf("hit op_simul_init\n\n\n");

    OPTIONS options = self->ask_for_options();

    printf("hit finished ask_for_options\n\n\n");
    
    self->process_manager = self->create_process_manager(); // create process manager

    self->process_manager->init = op_pro_init; // assign the initializer function

    printf("finished process_manager init\n\n\n");

    self->process_manager->init(self->process_manager, buffer, options.algorithm);

    // ---------- ressource list
    printf("hit create_ressource_manager\n\n\n");

    self->ressource_manager = self->create_ressource_manager(); // create ressource manager

    self->ressource_manager->init = op_rm_init; // assign the initializer function

    self->ressource_manager->init(self->ressource_manager);

    // ---------- schedular
    printf("hit create_schedular\n\n\n");

    self->schedular = self->create_schedular(options.algorithm, options.quantum); // create schedular    

    self->schedular->init = op_sched_init; // assign the initialization function to schedular

    self->schedular->init(self->schedular, self, options); // then init it

    // ------- simulator

    return WORK_DONE;
}
