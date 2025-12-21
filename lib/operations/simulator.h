#pragma once

#include "structs/process.h"
#include "structs/simulator.h"
#include "structs/process_manager.h"
#include "structs/ressource_manager.h"
#include "structs/schedular.h"
#include "structs/ressource.h"




// requiring functions
void op_start(SIMULATOR* self, char* path) {
    

}

void op_stop(SIMULATOR* self, char* path) {

}

void op_load_simulator(SIMULATOR* self, char* path) {

}

FILE* op_load_processus(SIMULATOR* self) {

}

//initialize managers
PROCESS_MANAGER* op_start_process_manager(FILE* buffer) {

}

RESSOURCE_MANAGER* op_start_ressource_manager() {

}

ORDONNANCEUR* op_start_schedular(Algorithms algorithm, int quantum, SIMULATOR* self) {
    
}


// process_manager & schedular related function
bool op_check_ressource_disponibility(RESSOURCE_ELEMENT* ressource_needed) {

}

bool op_signal_ressource_is_free(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    bool response = ressource_manager->mark_ressource_available(ressource);

    return response;
}

// process_manager & schedular related functions
bool op_update_cpu_time_used(PCB* process, float inc) {

}


bool op_sched_check_instruction_disponibility(INSTRUCTION* instruction) {

}

bool op_simul_check_instruction_disponibility(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    bool result = ressource_manager->check_if_ressource_available(ressource);

    return result;
}

bool signal_ressource_free(RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource) {
    bool response = ressource_manager->mark_ressource_available(ressource);

    return response;
}