#pragma once

#include "../../lib/structs/process.h" // for pcb
#include "../../lib/structs/simulator.h" // for SIMULATOR
#include "../../lib/structs/execution_queue.h" // for the exec queue
#include "../../lib/structs/ressource.h"

#include <unistd.h> // for time wait in second

typedef enum {
    RR, SRTF, PPP, FCFS, SJF
} Algorithms;


typedef enum {
    UPDATED, UPDATE_ERROR
} process_update; // moved here for the compiler


typedef enum {
    WORK_DONE, WORK_ERROR
} WORK_RETURN;



typedef enum {

    FINISHED,      // Instruction executed successfully
    RESSOURCE_NEEDED,
    QUANTUM_EXPIRED,
    PROCESS_ERROR         // General error

}  process_return;


typedef struct OPTIONS {

    int algorithm;
    float quantum;

} OPTIONS; // moved here because of compiler

typedef struct ORDONNANCEUR_STATISTICS {

    float cpu_total_temps_usage; // somme temps cpu occup total de tout process
    int context_switch; // nombre total de changement de processus

    // when process is terminated
    float total_turnround; // somme de tout processus (temps termine - temps arrive)
    int processus_termine_count; // processus terminé
    float troughtput; // processus terminé / total temps en ms
    float total_temps_attente; // somme temps waiting de tout processus
    
} ORDONNANCEUR_STATISTICS;

typedef struct ORDONNANCEUR {

    Algorithms algorithm;
    struct PCB* exec_proc; // processus en train de s'executer
    int current_pid; // pid du processus en cours d'exec

    float quantum; // quantum de time pour RR
    time_t start;
    time_t end;
    int cpu_time_used; // en ms: end - start

    float current_time;

    struct SIMULATOR* simulator; // pointeur vers simulator
    struct EXECUTION_QUEUE* execution_queue; // pointeur vers queue d'execution

    ORDONNANCEUR_STATISTICS* statistics; // pointeur vers les statistics du schedular
    struct INSTRUCTION* current_instruction; // need to be init as head

    // functions
    // on start
    bool (*init)(struct ORDONNANCEUR* self, struct SIMULATOR* simulator, OPTIONS* option);

    struct EXECUTION_QUEUE* (*create_execution_queue)(void);
    ORDONNANCEUR_STATISTICS* (*create_statistics)(void);

    // ordonnanceur to simulator (using bool for simplicity)
    // EXECUT_RESPONSE (*signal_execute_instruction) (ORDONNANCEUR* self, EXECUTION_QUEUE* execution_queue, INSTRUCTION* instruction); // tell the execution queue to execute an instruction
    bool (*need_ressources)(RESSOURCE_ELEMENT* ressource_needed); // return 1 if ressource is available marked unavailable
    bool (*ressource_is_free)(struct SIMULATOR* simulator, RESSOURCE ressource); // return 1 if ressource succesfully free (for error handling)
    // bool (*update_cpu_time_used)(PCB* process, float inc); // shoudld declancher calcul remaining time inc the value to add to time, because can only increasing not decreasing
    bool (*ask_sort_rt)(struct ORDONNANCEUR* self); // ask simulator to tell process manager to sort by remaining time ; pour srtf
    bool (*ask_sort_priority)(struct ORDONNANCEUR* self); // ask simulator to tell process manager to sort by priority ; pour ppp
    struct PCB* (*sched_ask_for_next_ready_element)(struct ORDONNANCEUR* self, struct PCB* current_pcb);

    // update statistics
    bool (*update_schedular_statistics) (struct ORDONNANCEUR* self, float* exec_time, float* burst, float* temp_attente, bool finished); // must check nullty

    // check instruction disponibility
    bool (*check_ressource_disponibility) (struct ORDONNANCEUR* self, RESSOURCE ressource);
    
    
    process_update (*update_process)(struct ORDONNANCEUR* self, struct PCB* pcb, time_t* temps_fin, float* cpu_temps_used); // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time

    // execute process    
    
    WORK_RETURN (*select)(struct ORDONNANCEUR* self, float quantum); // will be pointing on one of 5 functions depens on the algorithm

    WORK_RETURN (*kill) (struct ORDONNANCEUR* self);

} ORDONNANCEUR;

// like n9dr n3ti l ordonnanceur process i3tih l execution_queue