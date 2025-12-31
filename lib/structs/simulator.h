#pragma once

#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource_manager.h"
#include "../../lib/structs/ressource.h"


typedef struct SIMULATOR {
    ORDONNANCEUR* schedular;            // 8 bytes: pointer 
    PROCESS_MANAGER* process_manager;   // 8 bytes: pointer 
    RESSOURCE_MANAGER* ressource_manager; // 8 bytes: pointer 
    struct SIMULATOR* simulator;        // 8 bytes: pointer to self
    OPTIONS* options;                   // 8 bytes: pointer 
    
    int simulation_time;                // 4 bytes: inereger the time of all simulation
    bool runing;                        // 1 byte: boolean
    
    char csv_path[20];                  // 20 bytes: chaine caracter 20 char
    
    // setters
    
    SETTER (*set_schedular)(struct SIMULATOR* self, ORDONNANCEUR* schedular);
    SETTER (*set_process_manager)(struct SIMULATOR* self, PROCESS_MANAGER* process_manager);
    SETTER (*set_ressource_manager)(struct SIMULATOR* self, RESSOURCE_MANAGER* ressource_manager);
    SETTER (*set_simulator)(struct SIMULATOR* self, struct SIMULATOR* simulator);
    SETTER (*set_options)(struct SIMULATOR* self, OPTIONS* options);
    
    SETTER (*set_simulation_time)(struct SIMULATOR* self, int time);
    SETTER (*set_runing)(struct SIMULATOR* self, bool runing);
    SETTER (*set_csv_path)(struct SIMULATOR* self, const char* path);
    
    // getters
    
    ORDONNANCEUR* (*get_schedular)(struct SIMULATOR* self);
    PROCESS_MANAGER* (*get_process_manager)(struct SIMULATOR* self);
    RESSOURCE_MANAGER* (*get_ressource_manager)(struct SIMULATOR* self);
    struct SIMULATOR* (*get_simulator)(struct SIMULATOR* self);
    OPTIONS* (*get_options)(struct SIMULATOR* self);
    
    int (*get_simulation_time)(struct SIMULATOR* self);
    bool (*get_runing)(struct SIMULATOR* self);
    char* (*get_csv_path)(struct SIMULATOR* self);
    
    // methods
    
    // pricipale ones
    struct SIMULATOR* (*run_simulator)(struct SIMULATOR* self, char* path);
    WORK_RETURN (*work)(struct SIMULATOR* self, OPTIONS* options);
    WORK_RETURN (*init)(struct SIMULATOR* self, FILE* buffer);
    WORK_RETURN (*stop)(struct SIMULATOR* self);
    
    // init 
    OPTIONS* (*ask_for_options)(void);
    PROCESS_MANAGER* (*create_process_manager)(void);
    RESSOURCE_MANAGER* (*create_ressource_manager)(void);
    ORDONNANCEUR* (*create_schedular)(Algorithms algorithm, int quantum);
    PROCESS_MANAGER* (*start_process_manager)(struct SIMULATOR* self, FILE* csv_buffer);
    RESSOURCE_MANAGER* (*start_ressource_manager)(struct SIMULATOR* self);
    ORDONNANCEUR* (*start_schedular)( struct SIMULATOR* self, Algorithms algorithm, int quantum);
    
    // file related
    FILE* (*load_processus)(char* file_name);
    
    // resource_manager related
    TASK (*signal_ressource_is_free)(struct SIMULATOR* self, RESSOURCE ressource);
    TASK (*check_ressource_disponibility)(struct SIMULATOR* self, RESSOURCE ressource);
    
    // process_manager related
    TASK (*update_cpu_time_used)(PCB* process, float inc);
    PROCESS_UPDATE (*update_process)(struct SIMULATOR* self, PCB* pcb, time_t* temps_fin, float* cpu_temps_used);
    
    // queues related
    PCB* (*simul_ask_for_next_ready_element)(struct SIMULATOR* self, PCB* current_pcb);
    push_return (*simul_push_to_blocked_queue)(struct SIMULATOR* self, PCB* pcb);
    TASK (*remove_from_ready_queue)(struct SIMULATOR* self, PCB* pcb, float completion_time);
    
    // sorting related
    TASK (*ask_sort_rt)(struct SIMULATOR* self);
    TASK (*ask_sort_priority)(struct SIMULATOR* self);
    TASK (*ask_sort_sjf)(struct SIMULATOR* self);
    
    // statistics related
    PCB* (*simul_get_ready_queue_head)(struct SIMULATOR* self);
    TASK (*simul_update_process_manager)(struct SIMULATOR* self, FILE* processus_buffer, float* temps, float* runed);
    float (*get_max_arrival_time)(struct SIMULATOR* self);
    TASK (*update_ready_queue)(struct SIMULATOR* self, bool circular);
    
} SIMULATOR;