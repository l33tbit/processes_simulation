#pragma once


#include "../../lib/structs/process.h" 
#include <time.h>
#include <stdbool.h>
#include "../../lib/structs/ressource.h" // for ressource enum
#include "../../lib/structs/execution_queue.h" // for WORK_RETURN
#include "../../lib/structs/enums.h"

typedef enum {
    PROCESS_NEW, READY_QUEUE, BLOCKED, EXECUTION, TERMINATED
} E_etat;


typedef enum {
    NOT_STARTED, EXECUTING, STOPPED, COMPLETED
} INSTRUCTION_STATE;


typedef struct PCB PCB; // because instruction is called in pcb and pcb call instruction

typedef struct INSTRUCTION {
    int instruct_id;                    // 4 bytes: integer
    PCB* process;                       // 8 bytes: pointer 
    RESSOURCE type;                     // 4 bytes: enum because it s int
    INSTRUCTION_STATE state;            // 4 bytes: enum because it s int
    struct INSTRUCTION* next;           // 8 bytes: pointer 
    
    // setters
    SETTER (*set_instruct_id)(struct INSTRUCTION* self, int id);
    SETTER (*set_process)(struct INSTRUCTION* self, PCB* process);
    SETTER (*set_type)(struct INSTRUCTION* self, RESSOURCE type);
    SETTER (*set_state)(struct INSTRUCTION* self, INSTRUCTION_STATE state);
    SETTER (*set_next)(struct INSTRUCTION* self, struct INSTRUCTION* next);
    
    // getters
    int (*get_instruct_id)(struct INSTRUCTION* self);
    PCB* (*get_process)(struct INSTRUCTION* self);
    RESSOURCE (*get_type)(struct INSTRUCTION* self);
    INSTRUCTION_STATE (*get_state)(struct INSTRUCTION* self);
    struct INSTRUCTION* (*get_next)(struct INSTRUCTION* self);
    
} INSTRUCTION;


typedef struct PROCESS_STATISTICS {
    float temps_arrive;                 // 4 bytes: arrival time
    float temps_creation;               // 4 bytes: creation time
    float temps_fin;                    // 4 bytes: completion time
    float temps_attente;                // 4 bytes: waiting time
    float tournround;                   // 4 bytes: turnaround time
    
    // setters
    SETTER (*set_temps_arrive)(struct PROCESS_STATISTICS* self, float time);
    SETTER (*set_temps_creation)(struct PROCESS_STATISTICS* self, float time);
    SETTER (*set_temps_fin)(struct PROCESS_STATISTICS* self, float time);
    SETTER (*set_temps_attente)(struct PROCESS_STATISTICS* self, float time);
    SETTER (*set_tournround)(struct PROCESS_STATISTICS* self, float time);
    
    // getters
    float (*get_temps_arrive)(struct PROCESS_STATISTICS* self);
    float (*get_temps_creation)(struct PROCESS_STATISTICS* self);
    float (*get_temps_fin)(struct PROCESS_STATISTICS* self);
    float (*get_temps_attente)(struct PROCESS_STATISTICS* self);
    float (*get_tournround)(struct PROCESS_STATISTICS* self);
    
} PROCESS_STATISTICS;

typedef struct PCB {
    int pid;                            // 4 bytes: integer
    char process_name[20];              // 20 bytes: chaine caracter 20 bytes
    char user_id[20];                   // 20 bytes: same thing
    
    E_etat etat;                        // 4 bytes: enumeration is integer
    int prioritie;                      // 4 bytes: integer
    
    // instructions related
    INSTRUCTION* instructions_head;     // 8 bytes: pointer
    int programme_compteur;             // 4 bytes: integer
    INSTRUCTION* current_instruction;   // 8 bytes: pointer
    
    // i m not using this field in the entier code
    int memoire_necessaire;             // 4 bytes: integer
    
    float burst_time;                   // 4 bytes: float
    float cpu_time_used;                // 4 bytes: same
    float remaining_time;               // 4 bytes: same
    
    // statistics
    PROCESS_STATISTICS* statistics;     // 8 bytes: pointer
    
    // process relations
    struct PCB* pid_sibling_next;       // 8 bytes: pointer
    
    // setters
    SETTER (*set_pid)(struct PCB* self, int pid);
    SETTER (*set_process_name)(struct PCB* self, const char* name);
    SETTER (*set_user_id)(struct PCB* self, const char* user_id);
    SETTER (*set_ppid)(struct PCB* self, int ppid);
    
    SETTER (*set_etat)(struct PCB* self, E_etat etat);
    SETTER (*set_prioritie)(struct PCB* self, int priority);
    
    SETTER (*set_instructions_head)(struct PCB* self, INSTRUCTION* head);
    SETTER (*set_programme_compteur)(struct PCB* self, int counter);
    SETTER (*set_current_instruction)(struct PCB* self, INSTRUCTION* instr);
    
    SETTER (*set_memoire_necessaire)(struct PCB* self, int memory);
    
    SETTER (*set_burst_time)(struct PCB* self, float burst);
    SETTER (*set_cpu_time_used)(struct PCB* self, float cpu_time);
    SETTER (*set_remaining_time)(struct PCB* self, float remaining);
    
    SETTER (*set_statistics)(struct PCB* self, PROCESS_STATISTICS* stats);
    
    SETTER (*set_pid_childrens)(struct PCB* self, int* children);
    SETTER (*set_pid_sibling_next)(struct PCB* self, struct PCB* next);
    
    // getters
    
    int (*get_pid)(struct PCB* self);
    char* (*get_process_name)(struct PCB* self);
    char* (*get_user_id)(struct PCB* self);
    int (*get_ppid)(struct PCB* self);
    
    E_etat (*get_etat)(struct PCB* self);
    int (*get_prioritie)(struct PCB* self);
    
    INSTRUCTION* (*get_instructions_head)(struct PCB* self);
    int (*get_programme_compteur)(struct PCB* self);
    INSTRUCTION* (*get_current_instruction)(struct PCB* self);
    
    int (*get_memoire_necessaire)(struct PCB* self);
    
    float (*get_burst_time)(struct PCB* self);
    float (*get_cpu_time_used)(struct PCB* self);
    float (*get_remaining_time)(struct PCB* self);
    
    PROCESS_STATISTICS* (*get_statistics)(struct PCB* self);
    
    struct PCB*  (*get_pid_sibling_next)(struct PCB* self);
    
    // les methods
    
    // statistics realted
    TASK (*update_temps_attente)(struct PCB* self, float temps_attente);
    TASK (*update_temps_arrive)(struct PCB* self, struct tm temps_arrive);
    TASK (*update_temps_creation)(struct PCB* self, struct tm temps_creation);
    TASK (*update_temps_fin)(struct PCB* self, struct tm temps_fin);
    
    // instruction related
    TASK (*mark_instruction_terminated)(struct PCB* self, INSTRUCTION* instruction);
    
    // navigation related
    struct PCB* (*define_next)(struct PCB* self, struct PCB* next);
    struct PCB* (*define_previous)(struct PCB* self, struct PCB* previous);
    
    // init & cleaning
    INITIALIZATION (*init)(struct PCB* self);
    WORK_RETURN (*kill)(struct PCB* self);
    
} PCB;