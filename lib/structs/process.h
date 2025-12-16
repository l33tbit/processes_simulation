#pragma once


#include "structs/ressource.h" 
#include <time.h>
#include <stdbool.h>

typedef enum {
    READY, BLOCKED, EXECUTION, TERMINATED
} E_etat;


typedef enum {
    NOT_STARTED, EXECUTING, BLOCKED, COMPLETED
} INSTRUCTION_STATE;


typedef struct {
    int* instruct_id; // the id of instruction because process can stop at it if need ressources, pointer because it can be too long
    PCB* process; // the id of the process owner

    float time_remaining; // in nano seconds
    RESSOURCE_ELEMENT* type; // type of instruction which is ressource needed
    INSTRUCTION_STATE* state; // state of instruction 
    struct INSTRUCTION* next;
} INSTRUCTION;

typedef struct {
    time_t temps_arrive; // located in time.h
    time_t temps_creation;
    time_t temps_fin;
    
    float temps_attente; // gap entre temps arrive & temps exec // need to be init 0
    int tournround; // temps terminer - temps arrive
} PROCESS_STATISTICS;

typedef struct {
    int pid;
    char process_name[20];
    char user_id[20];
    int ppid;
    E_etat etat; // one of the values in the enum e_etat
    int prioritie; // priorities from 1 to 5 ; 1-critical

    INSTRUCTION* instructions_head; // change to a chained list
    int programme_compteur; // max 20000 instruction
    long memoire_necessaire; // en MB
    INSTRUCTION* current_instruction; // l'instruction en train de s executer

    float burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    float cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    int cpu_usage; // initialized as 0

    PROCESS_STATISTICS* statistics; 

    int* pid_childrens;
    struct PCB* pid_sibling_next; // pointeur vers next sib
    struct PCB* pid_sibling_previous; // pointeur vers previous sib

    float (*update_temps_attente)(struct PCB* self);
    float (*update_turnround) (struct PCB* self);
    bool (*mark_instruction_terminated) (struct PCB* self, INSTRUCTION* instruction);
    bool (*update_temps_arrive) (struct PCB* self, struct tm temps_arrive);
    bool (*update_temps_creation) (struct PCB* self, struct tm temps_creation);
    bool (*update_temps_fin) (struct PCB* self, struct tm temps_fin);
    struct PCB* (*get_next_pcb)(struct PCB* self);
    struct PCB* (*get_previous_pcb)(struct PCB* self);
    struct PCB* (*define_next)(struct PCB* self, struct PCB* next);
    struct PCB* (*define_previous)(struct PCB* self, struct PCB* previous);
} PCB;