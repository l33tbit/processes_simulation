#pragma once

#include <time.h>

#include "structs/process.h" // for PCB struct
#include "structs/ressource.h" // for RESSOURCE_ELEMENT

// structures nedded par les fonctions du process_manager
typedef struct { // used by process manager: many iteraction over process list but obe contact with ready queue for time reducing
    PCB* first_element;
} pcb_list; // stand for pcb first came

typedef struct {
    int pid; // l identifier du processus
    PCB* pcb; // l id du pcb du processus
    PROCESS_TABLE_ELEMENT* next;
} PROCESS_TABLE_ELEMENT;

typedef struct {
    PROCESS_TABLE_ELEMENT* head; // first element
} PROCESS_TABLE;

typedef struct {
    PCB* pcb; // pointeur du process
    READY_QUEUE_ELEMENT* next; // pointeur vers element suivant
    READY_QUEUE_ELEMENT* previous; // pointeur vers element precedent
} READY_QUEUE_ELEMENT;

typedef struct { // ordred chaine
    READY_QUEUE_ELEMENT* head; // pointeur vers premier element du chaine
} READY_QUEUE;


typedef struct {
    PCB* pcb; // l pointeur du processus
    RESSOURCE_ELEMENT* ressource; // ressource needed to execute the instruction
} BLOCKED_QUEUE_ELEMENT;

typedef struct { // circular chaine
    BLOCKED_QUEUE_ELEMENT* head; // first elme
    // BLOCKED_QUEUE_ELEMENT* tail; // last one
    int size; // how many elements
} BLOCKED_QUEUE;

// get_all_processus & sort function helper
typedef struct {
    PCB* first;
    int size;
} pcbs_and_size;

typedef struct {
    PROCESS_TABLE* process_table; // pointeur vers process table
    int process_count; // n processes
    READY_QUEUE* ready_queue; // pointer to ready chaine
    BLOCKED_QUEUE* blocked_queue; // pointer to blocked
    FILE* processus_buffer;
    // RESSOURCE* ressources; // in the retrieving should retrieve ressources needed also if a ressource doesn't match the enumeration throw an error (ressource anavailable)

    // functions
    // on start
    PROCESS_TABLE* (*create_process_table)(FILE* buffer); // need to be assigned to process_table field and update the process_count field// create a chaine circulaire ou non circular
    READY_QUEUE* (*create_ready_queue)(); // size which is process count field
    BLOCKED_QUEUE* (*create_blocked_queue)(); // will initialize by size 0 i think

    // process table related  (when creating the pcb pcd_statistics should also be created)
    pcbs_and_size* (*get_all_processus)(FILE* buffer); // should count while retrieving return struct that has first PCB* and size we'll get all process append them to a listn then assign pid,after that we ll push them into process list  *maybe*[ should check the ressources of each process compare to enumeration,]
    pcbs_and_size* (*sort_by_fc)(pcbs_and_size* process_list); // process_list created by get_all_processus
    pcbs_and_size* (*sort_by_rt)(pcbs_and_size* process_list); // process_list by get_all_processus
    pcbs_and_size* (*sort_by_priority)(pcbs_and_size* process_list);  // same
    pcbs_and_size* (*sort_by_burst)(pcbs_and_size* process_list); // same
    bool (*push_all_to_process_table)(PROCESS_TABLE* process_table, pcbs_and_size* pcb_list);  // list got by the sorting function

    //pcb related
    bool (*update_process)(PCB* pcb, struct tm temps_arrive, struct tm temps_fin, float cpu_temps_used, float temps_attente, int cpu_usage); // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time

    //ready queue related
    READY_QUEUE* (*push_to_ready_queue)(pcbs_and_size* sorted_list); // LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue
    bool (*delete_from_ready_queue)(READY_QUEUE* ready_queue, PCB* pcb); // the chaine node should be freed
    bool (*move_process_to_ready)(READY_QUEUE* ready_queue, PCB* pcb);

    // bloqued queue related
    bool (*add_process_to_blocked_queue)(BLOCKED_QUEUE* blocked_queue, PCB* pcb); // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it
    BLOCKED_QUEUE_ELEMENT* (*delete_from_blocked_queue)(BLOCKED_QUEUE* blocked_queue, PCB* pcb); // return the element 
    BLOCKED_QUEUE_ELEMENT* (*get_blocked_queue_element)(BLOCKED_QUEUE* blocked_queue, PCB* pcb); // return the element with the ressource needed from pcb 
    
} PROCESS_MANAGER;