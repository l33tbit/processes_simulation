#pragma once

#include <time.h>
#include <stdio.h>

#include "../../lib/structs/process.h" // for PCB struct
#include "../../lib/structs/ressource.h" // for RESSOURCE_ELEMENT
#include "../../lib/structs/execution_queue.h" // for WORK_RETURN


typedef struct PROCESS_MANAGER {

    struct PCB* process_table_head; // pointeur vers process table
    int process_count; // n processes
    struct PCB* ready_queue_head; // pointer to ready chaine can be circular for RR
    struct PCB* blocked_queue_head; // pointer to blocked
    FILE* processus_buffer;
    // RESSOURCE* ressources; // in the retrieving should retrieve ressources needed also if a ressource doesn't match the enumeration throw an error (ressource anavailable)

    // functions
    // on startypedef struct PROCESS_MANAGER
    struct PCB* (*create_process_table)(FILE* buffer); // need to be assigned to process_table field and update the process_count field// create a chaine circulaire ou non circular
    struct PCB* (*create_ready_queue)(PCB* process_table_head, bool circular); // size which is process count field)
    // READY_QUEUE_ELEMENT* (*create_ready_queue, PCB* pcb_head)(bool circular); // size which is process count field
    struct PCB* (*create_blocked_queue)(); // will initialize by size 0 i think

    // process table related  (when creating the pcb pcd_statistics should also be created)
    PCB* (*get_all_processus)(FILE* buffer); // should count while retrieving return struct that has first PCB* and size we'll get all process append them to a listn then assign pid,after that we ll push them into process list  *maybe*[ should check the ressources of each process compare to enumeration,]
    struct PCB* (*sort_by_fc)(struct PROCESS_MANAGER* self, bool circular); // process_list created by get_all_processus
    struct PCB* (*sort_by_rt)(struct PROCESS_MANAGER* self); // process_list by get_all_processus
    struct PCB* (*sort_by_priority)(struct PROCESS_MANAGER* self); // process_list by get_all_processus // same
    struct PCB* (*sort_by_burst)(struct PROCESS_MANAGER* self); // same
    struct PCB* (*push_all_to_process_table)(struct PCB* process_table_head, PCB* pcbs_head);  // list got by the sorting function

    //pcb related
    process_update (*update_process)(struct PROCESS_MANAGER* self, PCB* pcb, time_t* temps_fin, float* cpu_temps_used); // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time

    //ready queue related
    struct PCB* (*push_to_ready_queue)(struct PCB* ready_queue_head, struct PCB* pcb); // LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue
    struct PCB* (*delete_from_ready_queue)(struct PCB* ready_queue_head, PCB* pcb); // the chaine node should be freed
    struct PCB* (*move_process_to_ready)(struct PCB* ready_queue_head, PCB* pcb);
    struct PCB* (*get_next_ready_element) (struct PROCESS_MANAGER* self, PCB* current_pcb);

    // bloqued queue related
    struct PCB* (*add_process_to_blocked_queue)(struct PCB* blocked_queue_head, PCB* pcb); // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it
    struct PCB* (*delete_from_blocked_queue)(struct PCB* blocked_queue_head, PCB* pcb); // return the element 
    struct PCB* (*get_blocked_queue_element)(struct PCB* blocked_queue_head, PCB* pcb); // return the element with the ressource needed from pcb
    
    // assign function to the pcb
    PCB* (*assign_functions_to_pcb)(PCB* pcb);

    bool (*init)(struct PROCESS_MANAGER* self, FILE* buffer, int algorithm);

    WORK_RETURN (*kill) (struct PROCESS_MANAGER* self);

    bool (*free_process_table)(struct PCB* process_table_head);
    
} PROCESS_MANAGER;