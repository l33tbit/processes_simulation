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

    // process_manager related functions
    bool (*start_process_manager)(PROCESS_MANAGER* process_manager, FILE* processus_buffer);






} SIMULATOR;