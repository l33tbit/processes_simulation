#pragma once


#include "structs/ressource.h" 

typedef enum {
    READY, BLOCKED, EXECUTION, TERMINATED
} E_etat;


typedef enum {
    NOT_STARTED, EXECUTING, BLOCKED, COMPLETED
} INSTRUCTION_STATE;

typedef struct PCB PCB; //forward declaration because pcb is inside instruction and instruction is inside pcb

typedef struct {
    uint32_t* instruct_id; // the id of instruction because process can stop at it if need ressources, pointer because it can be too long
    PCB* process; // the id of the process owner

    float time_remaining; // in nano seconds
    RESSOURCE_ELEMENT* type; // type of instruction which is ressource needed
    INSTRUCTION_STATE* state; // state of instruction 
    INSTRUCTION* next;
} INSTRUCTION;

typedef struct {
    struct tm temps_arrive; // located in time.h
    struct tm temps_creation;
    struct tm temps_fin;
    
    float temps_attente; // gap entre temps arrive & temps exec
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
    long programme_compteur; // max 9,223,372,036,854,775,807 instruction
    int memoire_necessaire; // en MB
    INSTRUCTION* current_instruction; // l'instruction en train de s executer

    int burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    float cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    int cpu_usage; // initialized as 0

    PROCESS_STATISTICS* statistics; 

    int* pid_childrens;
    PCB* pid_sibling_next; // pointeur vers next sib
    PCB* pid_sibling_previous; // pointeur vers previous sib
} PCB;