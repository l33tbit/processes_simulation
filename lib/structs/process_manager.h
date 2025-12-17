#pragma once

#include <time.h>
#include <stdio.h>

#include "structs/process.h" // for PCB struct
#include "structs/ressource.h" // for RESSOURCE_ELEMENT

typedef struct PROCESS_TABLE_ELEMENT {
    int pid; // l identifier du processus
    PCB* pcb; // l id du pcb du processus
    struct PROCESS_TABLE_ELEMENT* next;
} PROCESS_TABLE_ELEMENT;

typedef struct READY_QUEUE_ELEMENT {
    PCB* pcb; // pointeur du process
    struct READY_QUEUE_ELEMENT* next; // pointeur vers element suivant
    struct READY_QUEUE_ELEMENT* previous; // pointeur vers element precedent
} READY_QUEUE_ELEMENT;


typedef struct BLOCKED_QUEUE_ELEMENT {
    PCB* pcb; // l pointeur du processus
    RESSOURCE_ELEMENT* ressource; // ressource needed to execute the instruction
    struct BLOCKED_QUEUE_ELEMENT* next;
} BLOCKED_QUEUE_ELEMENT;

// get_all_processus & sort function helper
typedef struct pcbs_and_size {
    PCB* first;
    int size;
} pcbs_and_size;

typedef struct PROCESS_MANAGER {
    PROCESS_TABLE_ELEMENT* process_table_head; // pointeur vers process table
    int process_count; // n processes
    READY_QUEUE_ELEMENT* ready_queue_head; // pointer to ready chaine can be circular for RR
    BLOCKED_QUEUE_ELEMENT* blocked_queue_head; // pointer to blocked
    FILE* processus_buffer;
    // RESSOURCE* ressources; // in the retrieving should retrieve ressources needed also if a ressource doesn't match the enumeration throw an error (ressource anavailable)

    // functions
    // on start
    PROCESS_TABLE_ELEMENT* (*create_process_table)(FILE* buffer); // need to be assigned to process_table field and update the process_count field// create a chaine circulaire ou non circular
    READY_QUEUE_ELEMENT* (*create_ready_queue)(bool circular); // size which is process count field
    BLOCKED_QUEUE_ELEMENT* (*create_blocked_queue)(void); // will initialize by size 0 i think

    // process table related  (when creating the pcb pcd_statistics should also be created)
    PCB* (*get_all_processus)(FILE* buffer); // should count while retrieving return struct that has first PCB* and size we'll get all process append them to a listn then assign pid,after that we ll push them into process list  *maybe*[ should check the ressources of each process compare to enumeration,]
    READY_QUEUE_ELEMENT* (*sort_by_fc)(READY_QUEUE_ELEMENT* ready_queue_head, bool circular); // process_list created by get_all_processus
    READY_QUEUE_ELEMENT* (*sort_by_rt)(READY_QUEUE_ELEMENT* ready_queue_head); // process_list by get_all_processus
    READY_QUEUE_ELEMENT* (*sort_by_priority)(READY_QUEUE_ELEMENT* ready_queue_head);  // same
    READY_QUEUE_ELEMENT* (*sort_by_burst)(READY_QUEUE_ELEMENT* ready_queue_head); // same
    PROCESS_TABLE_ELEMENT* (*push_all_to_process_table)(PROCESS_TABLE_ELEMENT* process_table_head, PCB* pcbs_head);  // list got by the sorting function

    //pcb related
    PCB* (*update_process)(PCB* pcb, struct tm temps_arrive, struct tm temps_fin, float cpu_temps_used, float temps_attente, int cpu_usage); // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time

    //ready queue related
    READY_QUEUE_ELEMENT* (*push_to_ready_queue)(READY_QUEUE_ELEMENT* ready_queue_head); // LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue
    READY_QUEUE_ELEMENT* (*delete_from_ready_queue)(READY_QUEUE_ELEMENT* ready_queue_head, PCB* pcb); // the chaine node should be freed
    READY_QUEUE_ELEMENT* (*move_process_to_ready)(READY_QUEUE_ELEMENT* ready_queue_head, PCB* pcb);

    // bloqued queue related
    BLOCKED_QUEUE_ELEMENT* (*add_process_to_blocked_queue)(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb); // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it
    BLOCKED_QUEUE_ELEMENT* (*delete_from_blocked_queue)(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb); // return the element 
    BLOCKED_QUEUE_ELEMENT* (*get_blocked_queue_element)(BLOCKED_QUEUE_ELEMENT* blocked_queue_head, PCB* pcb); // return the element with the ressource needed from pcb
    
    // assign function to the pcb
    PCB* (*assign_functions_to_pcb)(PCB* pcb);
    
} PROCESS_MANAGER;
