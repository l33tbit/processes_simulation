#pragma once

#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource_manager.h"
#include "../../lib/structs/ressource.h"






typedef struct SIMULATOR {
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
    WORK_RETURN (*stop)(struct SIMULATOR* self); // same thing here
    struct SIMULATOR* (*run_simulator)(struct SIMULATOR* self, char* path); // same
    FILE* (*load_processus)(char* file_name); // will read a file so it's return type is FILE*

    // functions
    // initialize the managers
    PROCESS_MANAGER* (*start_process_manager)(struct SIMULATOR* self, FILE* buffer); // process manager need the csv buffer to create the process table
    RESSOURCE_MANAGER* (*start_ressource_manager)(struct SIMULATOR* self); // passing void to non argument function is a common use :) // i think it needs nothing because all the ressource are defined in the enum and should only toggle the disponibility or availability 
    ORDONNANCEUR* (*start_schedular)(Algorithms algorithm, int quantum, struct SIMULATOR* self); // we should pass the simulator itself's pointer to the function 

    // process_manager & schedular related function
    bool (*signal_ressource_is_free)(struct SIMULATOR* self, RESSOURCE ressource); // can be void but just to be sure

    // process_manager & schedular related functions
    bool (*update_cpu_time_used)(PCB* process, float inc);

    // check instruction disponibility
    bool (*check_ressource_disponibility)(struct SIMULATOR* self, RESSOURCE ressource); // pointer or can be changed to ressource name to the ressource needed if disponible return 1 else 0

    PCB* (*simul_ask_for_next_ready_element) (struct SIMULATOR* self, PCB* current_pcb);

    bool (*ask_sort_rt)(struct SIMULATOR* self); // ask simulator to tell process manager to sort by remaining time ; pour srtf
    bool (*ask_sort_priority)(struct SIMULATOR* self); // ask simulator to tell process manager to sort by priority ; pour ppp


    PROCESS_MANAGER* (*create_process_manager)(void);
    RESSOURCE_MANAGER* (*create_ressource_manager)(void);
    ORDONNANCEUR* (*create_schedular)(Algorithms algorithm, int quantum);

    WORK_RETURN (*init) (struct SIMULATOR* simulator, FILE* buffer);

    OPTIONS (*ask_for_options)(void);

    WORK_RETURN (*run)(struct SIMULATOR* self,OPTIONS options); // will be pointing on one of 5 functions depens on the algorithm

    process_update (*update_process)(struct SIMULATOR* self, PCB* pcb, time_t* temps_fin, float* cpu_temps_used); // with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time    

} SIMULATOR;