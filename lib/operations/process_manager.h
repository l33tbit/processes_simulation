#pragma once
#include <stdbool.h>

#include "structs/process_manager.h"
#include "structs/process.h"
#include "operations/helpers/process_manager.h"
#include "operations/process.h"

// initialization
PROCESS_TABLE_ELEMENT* op_create_process_table(FILE* buffer) {
    // read_buffer and get elements list and count
    // create process table
    // iterate and add to process table


}

READY_QUEUE_ELEMENT* op_create_ready_queue(bool circular) {

}

BLOCKED_QUEUE_ELEMENT* op_create_blocked_queue(void) {

}


// process table related  (when creating the pcb pcd_statistics should also be created)
PCB* op_get_all_processus(FILE* buffer) {

}

READY_QUEUE_ELEMENT* op_sort_by_fc(READY_QUEUE_ELEMENT* ready_queue_head, bool circular) {

}

READY_QUEUE_ELEMENT* op_sort_by_rt(READY_QUEUE_ELEMENT* ready_queue_head) {

}

READY_QUEUE_ELEMENT* op_sort_by_priority(READY_QUEUE_ELEMENT* ready_queue_head) {

}

READY_QUEUE_ELEMENT* op_sort_by_burst(READY_QUEUE_ELEMENT* ready_queue_head) {

}

PROCESS_TABLE* op_push_all_to_process_table(PROCESS_TABLE* process_table, PCB* pcbs_head) { // need to assing pid

}

//pcb related
// with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time
PCB* op_update_process(PCB* pcb, struct tm temps_arrive, struct tm temps_fin, float cpu_temps_used, float temps_attente, int cpu_usage) {

}


// //ready queue related
READY_QUEUE_ELEMENT* op_push_to_ready_queue(READY_QUEUE_ELEMENT* ready_queue_head) {// LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue

}

READY_QUEUE_ELEMENT* op_delete_from_ready_queue(READY_QUEUE_ELEMENT* ready_queue_head, PCB* pcb) {// the chaine node should be freed

}

READY_QUEUE_ELEMENT* op_move_process_to_ready(READY_QUEUE_ELEMENT* ready_queue_head, PCB* pcb) {

} 

// bloqued queue related
BLOCKED_QUEUE_ELEMENT* op_add_process_to_blocked_queue(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb) { // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it

}

BLOCKED_QUEUE_ELEMENT* op_delete_from_blocked_queue(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb) {

}

BLOCKED_QUEUE_ELEMENT* op_get_blocked_queue_element(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb) {

}


PCB* op_assign_functions_to_pcb(PCB* pcb) {
    pcb->update_temps_attente = op_update_temps_attente;
    pcb->update_turnround = op_update_turnround;
    pcb->mark_instruction_terminated = op_mark_instruction_terminated;
    pcb->update_temps_arrive = op_update_temps_arrive;
    pcb->update_temps_creation
    pcb->update_temps_fin = op_update_temps_fin;
    pcb->get_next_pcb
    pcb->get_previous_pcb
    pcb->define_next
    pcb->define_previous

}
