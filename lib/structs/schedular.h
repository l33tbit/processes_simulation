#pragma once

#include "process.h" // for pcb
#include "simulator.h" // for SIMULATOR
#include "execution_queue.h" // for the exec queue

typedef enum {
    RR, SRTF, PPP, FCFS, SJF
} Algorithms;

typedef struct {
    float cpu_total_temps_usage; // somme temps cpu occup total de tout process 
    float cpu_temps_unoccuped; // temps total ou cpu n etait pas utilisé
    int context_switch; // nombre total de changement de processus
    float total_temps_attente; // somme temps waiting de tout processus

    // when process is terminated
    float total_turnround; // somme de tout processus (temps termine - temps arrive)
    int processus_termine_count; // processus terminé
    float troughtput; // processus terminé / total temps en ms

    PCB* current_processus; // processus en cours d execution
} ORDONNANCEUR_STATISTICS;


typedef struct {
    Algorithms algorithm;
    PCB* exec_proc; // processus en train de s'executer
    int current_pid; // pid du processus en cours d'exec
    
    int quantum; // quantum de time pour RR
    struct tm start;
    struct tm end;
    int cpu_time_used; // en ms: end - start

    float current_time;

    SIMULATOR* simulator; // pointeur vers simulator
    EXECUTION_QUEUE* in_execution_queue; // pointeur vers queue d'execution

    ORDONNANCEUR_STATISTICS* statistics; // pointeur vers les statistics du schedular


    // functions 
    // on start
    bool (*create_execution_queue)();
    bool (*create_statistics)();

    // ordonnanceur to simulator
    bool (*need_ressources)(RESSOURCE_ELEMENT* ressource_needed); // return 1 if ressource is available marked unavailable
    bool (*ressource_is_free)(RESSOURCE_ELEMENT* ressource); // return 1 if ressource succesfully free (for error handling)
    bool (*update_cpu_time_used)(PCB* process, float inc); // shoudld declancher calcul remaining time inc the value to add to time, because can only increasing not decreasing
    bool (*update_process)(PCB* process, float temps_fin, float tournround, float temps_attente); // when updating temps fin mark process terminated
    bool (*ask_sort_rt)(); // ask simulator to tell process manager to sort by remaining time ; pour srtf
    bool (*ask_sort_priority)(); // ask simulator to tell process manager to sort by priority ; pour ppp
    PCB* (*ask_for_next_ready_element)();

    // ordonnanceur to execution queue
    EXECUTION_QUEUE_RESPONSE* (*execute_instruction)(INSTRUCTION* instruction); // instruction to execute

    // update statistics
    bool (*update_schedular_statistics) (ORDONNANCEUR_STATISTICS* schedular, float cpu_total_temps_usage, float cpu_temps_unoccupied, int context_switch, float total_temps_attente, float process_termine_count, float throughtput); // must check nullty
    

} ORDONNANCEUR;
