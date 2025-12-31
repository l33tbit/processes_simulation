#pragma once

#include <time.h>
#include <stdio.h>

#include "../../lib/structs/process.h" // for PCB struct
#include "../../lib/structs/ressource.h" // for RESSOURCE_ELEMENT
#include "../../lib/structs/execution_queue.h" // for WORK_RETURN

#include "../../lib/structs/enums.h"

typedef struct PROCESS_MANAGER {

    struct PCB* process_table_head;     // pointer : 8bytes
    int process_count;                  // integer : 4bytes
    struct PCB* ready_queue_head;       // pointer : 8bytes
    struct PCB* blocked_queue_head;     // pointer : 8bytes
    FILE* processus_buffer;             // pointer : 8bytes
    float temps;                        // float   : 4bytes
    float last_runed;                   // float   : 4bytes
    float max_arrival_time;             // float   : 4bytes
    int ready_count;                    // integer : 4bytes
    int blocked_count;                  // integer : 4bytes

    // setters
    SETTER (*set_process_table_head)(struct PROCESS_MANAGER* self, struct PCB* process_table_head);
    SETTER (*set_inc_process_count)(struct PROCESS_MANAGER* self, int inc);
    SETTER (*set_ready_queue_head)(struct PROCESS_MANAGER* self, struct PCB* ready_queue_head);
    SETTER (*set_blocked_queue_head)(struct PROCESS_MANAGER* self, struct PCB* blocked_queue_head);
    SETTER (*set_processus_buffer)(struct PROCESS_MANAGER* self, FILE* processus_buffer);
    SETTER (*set_temps)(struct PROCESS_MANAGER* self, float temps);
    SETTER (*set_last_runed)(struct PROCESS_MANAGER* self, float last_runed);
    SETTER (*set_max_arrival_time)(struct PROCESS_MANAGER* self, float max_arrival_time);

    // getters
    struct PCB* (*get_process_table_head)(struct PROCESS_MANAGER* self);
    int (*get_process_count)(struct PROCESS_MANAGER* self);
    struct PCB* (*get_ready_queue_head)(struct PROCESS_MANAGER* self);
    struct PCB* (*get_blocked_queue_head)(struct PROCESS_MANAGER* self);
    FILE* (*get_processus_buffer)(struct PROCESS_MANAGER* self);
    float (*get_temps)(struct PROCESS_MANAGER* self);
    float (*get_last_runed)(struct PROCESS_MANAGER* self);
    float (*get_max_arrival_time)(struct PROCESS_MANAGER* self);

    // initialization & cleaning
    struct PCB* (*create_process_table)(struct PROCESS_MANAGER* self); 
    struct PCB* (*create_ready_queue)(struct PROCESS_MANAGER* self, bool circular);
    struct PCB* (*create_blocked_queue)();                      
    TASK (*init)(struct PROCESS_MANAGER* self, FILE* buffer, int algorithm); 
    WORK_RETURN (*kill)(struct PROCESS_MANAGER* self);         
    TASK (*free_process_table)(struct PROCESS_MANAGER* self); 
    void (*free_ready_queue)(PCB* head);                 

    // process table related
    PCB* (*get_all_processus)(FILE* buffer);                    
    struct PCB* (*push_all_to_process_table)(struct PCB* process_table_head, PCB* pcbs_head); 
    struct PCB* (*get_next_process_table)(struct PROCESS_MANAGER* self, struct PCB* current_pcb); 
    void (*make_process_table_linear)(struct PROCESS_MANAGER* self);

    // pcb management
    PROCESS_UPDATE (*update_process)(struct PROCESS_MANAGER* self, PCB* pcb, float* temps_fin, float* cpu_temps_used);
    PCB* (*assign_functions_to_pcb)(struct PROCESS_MANAGER* self, PCB* pcb); 
    TASK (*mark_process_table_pcb_terminated)(struct PROCESS_MANAGER* self, PCB* pcb, float completion_time);

    // ready queue related
    struct PCB* (*push_to_ready_queue)(struct PROCESS_MANAGER* self, struct PCB* pcb, bool circular);
    struct PCB* (*delete_from_ready_queue)(struct PROCESS_MANAGER* self, PCB* pcb, float completion_time);
    struct PCB* (*get_next_ready_element)(struct PROCESS_MANAGER* self, PCB* current_pcb);
    struct PCB* (*insert_after_ready)(struct PROCESS_MANAGER* self, PCB* after_pcb, PCB* pcb_to_insert);

    // blocked queue related
    push_return (*add_process_to_blocked_queue)(struct PROCESS_MANAGER* self, PCB* pcb);
    struct PCB* (*delete_from_blocked_queue)(struct PROCESS_MANAGER* self, PCB* pcb);
    struct PCB* (*get_blocked_queue_element)(struct PROCESS_MANAGER* self, PCB* pcb);

    // sorting
    struct PCB* (*sort_by_fc)(struct PROCESS_MANAGER* self, bool circular);    
    struct PCB* (*sort_by_rt)(struct PROCESS_MANAGER* self);                  
    struct PCB* (*sort_by_priority)(struct PROCESS_MANAGER* self);          
    struct PCB* (*sort_by_sjf)(struct PROCESS_MANAGER* self);               

    // updates
    TASK (*update_read_queue)(struct PROCESS_MANAGER* self, bool circular);    
    TASK (*update_self_temps)(struct PROCESS_MANAGER* self, float temps, float* runed);  

    // time related
    float (*find_max_arrival_time)(struct PROCESS_MANAGER* self);
    
} PROCESS_MANAGER;