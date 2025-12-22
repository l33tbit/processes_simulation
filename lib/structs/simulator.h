#pragma once

#include "structs/process_manager.h"
#include "structs/schedular.h"
#include "structs/ressource_manager.h"
#include "structs/ressource.h"

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
    EXECUTION_QUEUE* (*create_execution_queue)(void);
    ORDONNANCEUR_STATISTICS* (*create_statistics)(void);

    // ordonnanceur to simulator (using bool for simplicity)
    bool (*need_ressources)(RESSOURCE_ELEMENT* ressource_needed); // return 1 if ressource is available marked unavailable
    bool (*ressource_is_free)(RESSOURCE_ELEMENT* ressource); // return 1 if ressource succesfully free (for error handling)
    bool (*update_cpu_time_used)(PCB* process, float inc); // shoudld declancher calcul remaining time inc the value to add to time, because can only increasing not decreasing
    bool (*update_process)(PCB* process, float temps_fin, float tournround, float temps_attente); // when updating temps fin mark process terminated
    bool (*ask_sort_rt)(); // ask simulator to tell process manager to sort by remaining time ; pour srtf
    bool (*ask_sort_priority)(); // ask simulator to tell process manager to sort by priority ; pour ppp
    PCB* (*ask_for_next_ready_element)(PCB* current_pcb);

    // update statistics
    bool (*update_schedular_statistics) (ORDONNANCEUR_STATISTICS* schedular, float cpu_total_temps_usage, float cpu_temps_unoccupied, int context_switch, float total_temps_attente, float process_termine_count, float throughtput); // must check nullty

} ORDONNANCEUR;

typedef struct {
    ORDONNANCEUR* schedular; // pointeur vers lordonnanceur
    PROCESS_MANAGER* process_manager; // pointeur to process manaer
    RESSOURCE_MANAGER* ressource_manager; // pointeur vers ressource
    struct SIMULATOR* simulator;
    
    int simulation_time;
    bool runing;

    char csv_path[20];


    // functions
    // requiring functions
    struct SIMULATOR* (*start)(struct SIMULATOR* self, char* path); // so void is the return type, (*start) is the function pointer [we need to create or define the function outside then assign the fucntion created to that pointer], then after the arguments
    void (*stop)(struct SIMULATOR* self, char* path); // same thing here
    SIMULATOR* (*run_simulator)(struct SIMULATOR* self, char* path); // same
    FILE* (*load_processus)(char* file_name); // will read a file so it's return type is FILE*

    // functions
    // initialize the managers
    PROCESS_MANAGER* (*start_process_manager)(SIMULATOR* self, FILE* buffer); // process manager need the csv buffer to create the process table
    RESSOURCE_MANAGER* (*start_ressource_manager)(SIMULATOR* self); // passing void to non argument function is a common use :) // i think it needs nothing because all the ressource are defined in the enum and should only toggle the disponibility or availability 
    ORDONNANCEUR* (*start_schedular)(Algorithms algorithm, int quantum, SIMULATOR* self); // we should pass the simulator itself's pointer to the function 

    // process_manager & schedular related function
    bool (*check_ressource_disponibility)(RESSOURCE_ELEMENT* ressource_needed); // pointer or can be changed to ressource name to the ressource needed if disponible return 1 else 0
    bool (*signal_ressource_is_free)(RESSOURCE_ELEMENT* ressource); // can be void but just to be sure

    // process_manager & schedular related functions
    bool (*update_cpu_time_used)(PCB* process, float inc);

    // check instruction disponibility
    bool (*check_ressource_disponibility) (RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource);

    bool (*signal_ressource_free) (RESSOURCE_MANAGER* ressource_manager, RESSOURCE ressource);

    PCB* (*ask_for_next_ready_element) (SIMULATOR* simulator, PCB* current_pcb);

    bool (*ask_sort_rt)(SIMULATOR* self); // ask simulator to tell process manager to sort by remaining time ; pour srtf
    bool (*ask_sort_priority)(SIMULATOR* self); // ask simulator to tell process manager to sort by priority ; pour ppp

} SIMULATOR;