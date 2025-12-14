#pragma once



#include "structs/execution_queue.h"
#include "structs/schedular.h"
#include "structs/ressource.h"
#include "structs/instruction.h"



// on start
EXECUTION_QUEUE* op_create_execution_queue() {

}

ORDONNANCEUR_STATISTICS* op_create_statistics() {

}

// ordonnanceur TO simulator
bool op_need_ressources(RESSOURCE_ELEMENT* ressource_needed) {

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