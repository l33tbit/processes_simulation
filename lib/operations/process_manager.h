#pragma once


#include "structs/process_manager.h"
#include "structs/process.h"

// initialization
PROCESS_TABLE* op_create_process_table(bool circular) {

}

READY_QUEUE* op_create_ready_queue(int size) {

}

BLOCKED_QUEUE* op_create_blocked_queue(int size) {

}


// process table related  (when creating the pcb pcd_statistics should also be created)

pcbs_and_size* op_get_all_processus(FILE* buffer) {

}

pcbs_and_size* op_sort_by_fc(pcbs_and_size* process_list) {

}

pcbs_and_size* op_sort_by_rt(pcbs_and_size* process_list) {

}

pcbs_and_size* op_sort_by_priority(pcbs_and_size* process_list) {

}

pcbs_and_size* op_sort_by_burst(pcbs_and_size* process_list) {

}

bool op_push_all_to_process_table(PROCESS_TABLE* process_table, pcbs_and_size* pcb_list) {

}

//pcb related
// with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time
bool op_update_process(PCB* pcb, struct tm temps_arrive, struct tm temps_fin, float cpu_temps_used, float temps_attente, int cpu_usage) {

}


// //ready queue related
READY_QUEUE* op_push_to_ready_queue(pcbs_and_size* sorted_list) {// LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue

}

bool op_delete_from_ready_queue(READY_QUEUE* ready_queue, PCB* pcb) {// the chaine node should be freed

}

bool op_move_process_to_ready(READY_QUEUE* ready_queue, PCB* pcb) {

} 

// bloqued queue related
bool op_add_process_to_blocked_queue(BLOCKED_QUEUE* blocked_queue, PCB* pcb) { // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it

}

BLOCKED_QUEUE_ELEMENT* op_delete_from_blocked_queue(BLOCKED_QUEUE* blocked_queue, PCB pcb) {

}

BLOCKED_QUEUE_ELEMENT* op_get_blocked_queue_element(BLOCKED_QUEUE* blocked_queue, PCB* pcb) {

}