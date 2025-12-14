#pragma once

#include "structs/process.h"
#include "structs/simulator.h"
#include "structs/process_manager.h"
#include "structs/ressource_manager.h"
#include "structs/schedular.h"
#include "structs/ressource.h"

// process_manager & schedular related functions
bool (*update_cpu_time_used)(PCB* process, float inc);


// requiring functions
void op_start(SIMULATOR* self, char* path) {

}

void op_stop(SIMULATOR* self, char* path) {

}

void load_simulator(SIMULATOR* self, char* path) {

}

FILE* load_processus(SIMULATOR* self) {

}

//initialize managers
PROCESS_MANAGER* start_process_manager(FILE* buffer) {

}

RESSOURCE_MANAGER* start_ressource_manager() {

}

ORDONNANCEUR* start_schedular(Algorithms algorithm, int quantum, SIMULATOR* self) {

}


// process_manager & schedular related function
bool check_ressource_disponibility(RESSOURCE_ELEMENT* ressource_needed) {

}

bool signal_ressource_is_free(RESSOURCE_ELEMENT* ressource) {

}