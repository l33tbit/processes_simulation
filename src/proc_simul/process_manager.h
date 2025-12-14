#pragma once

#include "process.h" // for PCB struct
#include "ressource_manager.h" // for RESSOURCE_ELEMENT

// structures nedded par les fonctions du process_manager
typedef struct { // used by process manager: many iteraction over process list but obe contact with ready queue for time reducing
    PCB* first_element;
    int size;
} pcb_list; // stand for pcb first came

typedef struct {
    int pid; // l identifier du processus
    PCB* pcb; // l id du pcb du processus
    bool circular;
} PROCESS_TABLE;

typedef struct {
    int pid; // processus id
    READY_QUEUE_ELEMENT* next; // pointeur vers element suivant
    READY_QUEUE_ELEMENT* previous; // pointeur vers element precedent
} READY_QUEUE_ELEMENT;

typedef struct { // ordred chaine
    READY_QUEUE_ELEMENT* head; // pointeur vers premier element du chaine
    READY_QUEUE_ELEMENT* tail; // pointeur vers last element
    int size; // nombre du nodes
} READY_QUEUE;


typedef struct {
    int pid; // l id processus
    RESSOURCE_ELEMENT* ressource; // ressource needed to execute the instruction
} BLOCKED_QUEUE_ELEMENT;

typedef struct { // circular chaine
    BLOCKED_QUEUE_ELEMENT* head; // first elme
    BLOCKED_QUEUE_ELEMENT* tail; // last one
    int size; // how many elements
} BLOCKED_QUEUE;

// get_all_processus & sort function helper
typedef struct {
    PCB* first;
    int size;
} pcbs_and_size

typedef struct {
    PROCESS_TABLE* process_table; // pointeur vers process table
    int process_count; // n processes
    READY_QUEUE* ready_queue; // pointer to ready chaine
    BLOCKED_QUEUE* blocked_queue; // pointer to blocked

    // functions
    // on start
    PROCESS_TABLE* (*create_process_table)(bool circular); // need to be assigned to process_table field and update the process_count field// create a chaine circulaire ou non circular
    READY_QUEUE* (*create_ready_queue)(int size); // size which is process count field
    BLOCKED_QUEUE* (*create_blocked_queue)(int size); // will initialize by size 0 i think

    // process table related
    pcbs_and_size* (*get_all_processus)(FILE* buffer); // should count while retrieving return struct that has first PCB* and size we'll get all process append them to a listn then assign pid,after that we ll push them into process list
    pcbs_and_size* (*sort_by_f_c)(pcbs_and_size* process_list); // process_list created by get_all_processus
    pcbs_and_size* (*sort_by_r_time)(pcbs_and_size* process_list); // process_list by get_all_processus
    pcbs_and_size* (*sort_by_priority)(pcbs_and_size* process_list);  // same
    pcbs_and_size* (*sort_by_burst)(pcbs_and_size* process_list); // same
    bool (*push_all_to_process_table)(READY_QUEUE* ready_queue, pcbs_and_size* pcb_list);  // list got by the sorting function

    //pcb related
    bool (*mark_process_terminate_update_temps_fin)(PCB* pcb);
    bool (*update_temps_arrive)(PCB* pcb);
    bool (*update_cpu_usage_and_remaining_time)(PCB* pcb);
    bool (*update_pcb_all)(PCB* pcb);

    //ready queue related
    



} PROCESS_MANAGER;