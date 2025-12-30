#pragma once

#include "../../lib/structs/process.h" // for pcb
#include "../../lib/structs/simulator.h" // for SIMULATOR

typedef enum {
    RR, SRTF, PPP, FCFS, SJF
} Algorithms;

#include "../../lib/structs/execution_queue.h" // for the exec queue
#include "../../lib/structs/ressource.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/output.h"

#include <unistd.h> // for time wait in second



typedef enum {
    UPDATED, UPDATE_ERROR
} PROCESS_UPDATE; // moved here for the compiler

typedef enum {
    PROCESS_BLOCKED, RESSOURCES_AVAILABLE, PROCESS_ERROR
} sched_ressources_return;

typedef enum {
    WORK_DONE, WORK_ERROR
} WORK_RETURN;

typedef enum {

    FINISHED,     
    RESSOURCE_NEEDED,
    QUANTUM_EXPIRED,
    PROCESS_ERRORE        

}  process_return;

typedef enum {
    PUSHED, PUSH_ERROR
} push_return;

typedef struct OPTIONS {

    int algorithm;
    float quantum;

} OPTIONS; // moved here because of compiler

typedef struct ORDONNANCEUR_STATISTICS {
    float cpu_total_temps_usage;        // 4 bytes: total time cpu was occuped
    int context_switch;                 // 4 bytes: integer how many process changement
    float total_turnround;              // 4 bytes: float somme of all processes tournround
    int processus_termine_count;        // 4 bytes: integer termined process count
    float troughtput;                   // 4 bytes: float process termined / temps
    float total_temps_attente;          // 4 bytes: float sum of all temps attente
    
    // setters
    SETTER (*set_cpu_total_temps_usage)(struct ORDONNANCEUR_STATISTICS* self, float usage);
    SETTER (*set_context_switch)(struct ORDONNANCEUR_STATISTICS* self, int switches);
    SETTER (*set_total_turnround)(struct ORDONNANCEUR_STATISTICS* self, float turnround);
    SETTER (*set_processus_termine_count)(struct ORDONNANCEUR_STATISTICS* self, int count);
    SETTER (*set_troughtput)(struct ORDONNANCEUR_STATISTICS* self, float throughput);
    SETTER (*set_total_temps_attente)(struct ORDONNANCEUR_STATISTICS* self, float attente);
    
    // getters
    float (*get_cpu_total_temps_usage)(struct ORDONNANCEUR_STATISTICS* self);
    int (*get_context_switch)(struct ORDONNANCEUR_STATISTICS* self);
    float (*get_total_turnround)(struct ORDONNANCEUR_STATISTICS* self);
    int (*get_processus_termine_count)(struct ORDONNANCEUR_STATISTICS* self);
    float (*get_troughtput)(struct ORDONNANCEUR_STATISTICS* self);
    float (*get_total_temps_attente)(struct ORDONNANCEUR_STATISTICS* self);
    
} ORDONNANCEUR_STATISTICS;

typedef struct ORDONNANCEUR {
    Algorithms algorithm;               // 4 bytes: enum so its integer
    float quantum;                      // 4 bytes: float
    float current_time;                 // 4 bytes: float the current time like time where schedular arrive when accessing it
    
    struct PCB* exec_proc;              // 8 bytes: current exec process
    int current_pid;                    // 4 bytes: pid of current process

    // timing
    time_t start;                       // 8 bytes: start time
    time_t end;                         // 8 bytes: end time
    int cpu_time_used;                  // 4 bytes: integer
    
    // LINKS
    struct SIMULATOR* simulator;        // 8 bytes: pointer
    struct EXECUTION_QUEUE* execution_queue;  // 8 bytes: pointer 
    ORDONNANCEUR_STATISTICS* statistics; // 8 bytes: pointer 
    struct INSTRUCTION* current_instruction;  // 8 bytes: pointer
    EXECUTION_SEGMENT* execution_segments_head;
    EXECUTION_SEGMENT* current_segment;
    PERFORMANCE_SUMMARY* performance_summary;

    // setters
    // fields setters
    SETTER (*set_algorithm)(struct ORDONNANCEUR* self, Algorithms algorithm);
    SETTER (*set_exec_proc)(struct ORDONNANCEUR* self, struct PCB* exec_proc);
    SETTER (*set_current_pid)(struct ORDONNANCEUR* self, int current_pid);
    SETTER (*set_quantum)(struct ORDONNANCEUR* self, float quantum);
    SETTER (*set_start)(struct ORDONNANCEUR* self, time_t start);
    SETTER (*set_end)(struct ORDONNANCEUR* self, time_t end);
    SETTER (*set_cpu_time_used)(struct ORDONNANCEUR* self, int cpu_time_used);
    SETTER (*set_current_time)(struct ORDONNANCEUR* self, float current_time);
    
    // links setters
    SETTER (*set_simulator)(struct ORDONNANCEUR* self, struct SIMULATOR* simulator);
    SETTER (*set_execution_queue)(struct ORDONNANCEUR* self, struct EXECUTION_QUEUE* execution_queue);
    SETTER (*set_statistics)(struct ORDONNANCEUR* self, ORDONNANCEUR_STATISTICS* statistics);
    SETTER (*set_current_instruction)(struct ORDONNANCEUR* self, struct INSTRUCTION* current_instruction);
    
    // getters
    
    // fields getters
    Algorithms (*get_algorithm)(struct ORDONNANCEUR* self);
    struct PCB* (*get_exec_proc)(struct ORDONNANCEUR* self);
    int (*get_current_pid)(struct ORDONNANCEUR* self);
    float (*get_quantum)(struct ORDONNANCEUR* self);
    time_t (*get_start)(struct ORDONNANCEUR* self);
    time_t (*get_end)(struct ORDONNANCEUR* self);
    int (*get_cpu_time_used)(struct ORDONNANCEUR* self);
    float (*get_current_time)(struct ORDONNANCEUR* self);
    
    // link getters
    struct SIMULATOR* (*get_simulator)(struct ORDONNANCEUR* self);
    struct EXECUTION_QUEUE* (*get_execution_queue)(struct ORDONNANCEUR* self);
    struct ORDONNANCEUR_STATISTICS* (*get_statistics)(struct ORDONNANCEUR* self);
    struct INSTRUCTION* (*get_current_instruction)(struct ORDONNANCEUR* self);
    
    // methods
    
    // init & clean
    INITIALIZATION (*init)(struct ORDONNANCEUR* self, struct SIMULATOR* simulator, OPTIONS* option);
    struct EXECUTION_QUEUE* (*create_execution_queue)(void);
    ORDONNANCEUR_STATISTICS* (*create_statistics)(void);
    WORK_RETURN (*kill)(struct ORDONNANCEUR* self);
    
    // function used in sched algos
    WORK_RETURN (*select)(struct ORDONNANCEUR* self, float quantum);
    struct PCB* (*sched_ask_for_next_ready_element)(struct ORDONNANCEUR* self, struct PCB* current_pcb);
    struct PCB* (*sched_get_ready_queue_head)(struct ORDONNANCEUR* self);
    
    // process_managemer related
    PROCESS_UPDATE (*update_process)(struct ORDONNANCEUR* self, struct PCB* pcb, float* temps_fin, float* cpu_temps_used);
    TASK (*remove_from_ready_queue)(struct ORDONNANCEUR* self, struct PCB* pcb, float completion_time);
    push_return (*sched_push_to_blocked_queue)(struct ORDONNANCEUR* self, struct PCB* pcb);
    
    // resource_management related
    sched_ressources_return (*check_ressources)(struct ORDONNANCEUR* self, struct PCB* exec_proc);
    TASK (*check_ressource_disponibility)(struct ORDONNANCEUR* self, RESSOURCE ressource);
    TASK (*need_ressources)(RESSOURCE_ELEMENT* ressource_needed);
    TASK (*ressource_is_free)(struct ORDONNANCEUR* self, struct SIMULATOR* simulator, RESSOURCE ressource);
    
    // ready_queue related
    TASK (*ask_sort_rt)(struct ORDONNANCEUR* self);
    TASK (*ask_sort_priority)(struct ORDONNANCEUR* self);
    TASK (*ask_sort_sjf)(struct ORDONNANCEUR* self);
    TASK (*update_ready_queue)(struct ORDONNANCEUR* self, bool circular);
    TASK (*sched_update_process_manager)(struct ORDONNANCEUR* self, float temps, float* runed);
    
    // statistics related
    TASK (*update_schedular_statistics)(struct ORDONNANCEUR* self, float* exec_time, float* turnaround, float* temp_attente, bool finished);
    float (*get_max_arrival_time)(struct ORDONNANCEUR* self);
    
} ORDONNANCEUR;
