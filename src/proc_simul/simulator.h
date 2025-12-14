#pragma once

#include "process_manager.h"
#include "schedular.h"
#include "ressource_manager.h"


typedef struct {
    ORDONNANCEUR* schedular; // pointeur vers lordonnanceur
    PROCESS_MANAGER* process_manager; // pointeur to process manaer
    RESSOURCE_MANAGER* ressource_manager; // pointeur vers ressource
    
    int simulation_time;
    bool runing;

    char csv_path[20];


    // functions
    // requiring functions
    void (*start)(SIMULATOR* self, char* path); // so void is the return type, (*start) is the function pointer [we need to create or define the function outside then assign the fucntion created to that pointer], then after the arguments
    void (*stop)(SIMULATOR* self, char* path); // same thing here
    void (*run_simulator)(SIMULATOR* self, char* path); // same
    FILE* (*load_processus)(SIMULATOR* self); // will read a file so it's return type is FILE*

    // functions
    // initialize the managers
    PROCESS_MANAGER* (*start_process_manager)(FILE* buffer); // process manager need the csv buffer to create the process table
    RESSOURCE_MANAGER* (*start_ressource_manager)(); // i think it needs nothing because all the ressource are defined in the enum and should only toggle the disponibility or availability 
    ORDONNANCEUR* (*start_schedular)(Algorithms algorithm, int quantum, SIMULATOR* self); // we should pass the simulator itself's pointer to the function 

    // process_manager & schedular related function
    bool (*check_ressource_disponibility)(RESSOURCE_ELEMENT* ressource_needed); // pointer or can be changed to ressource name to the ressource needed if disponible return 1 else 0
    bool (*signal_ressource_is_free)(RESSOURCE_ELEMENT* ressource); // can be void but just to be sure


    // process_manager & schedular related functions
    



} SIMULATOR;