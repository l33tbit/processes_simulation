#pragma once

#include <time.h>


typedef enum {
    READY, BLOCKED, EXECUTION, TERMINATED
} E_etat;

typedef enum {
    // ressources
} RESSOURCE;

typedef struct {
    char* ressource_name[10]; // nom de ressource
    RESSOURCE* ressource; // type ooubien l enum
    bool disponibilite; // dispo ou non 1 / 0
} RESSOURCES_ELEMENT;

typedef enum {
    // should define types
} INSTRUCTION_TYPE; // ressources

typedef enum {
    NOT_STARTED, EXECUTING, BLOCKED, COMPLETED
} INSTRUCTION_STATE;

typedef struct {
    uint32_t instruct_id; // the id of instruction because process can stop at it if need ressources 
    int process_it; // the id of the process owner

    float time_remaining; // in nano seconds
    INSTRUCTION_TYPE* type; // type of instruction which is ressource needed
    INSTRUCTION_STATE* state; // state of instruction 
} INSTRUCTION;

typedef struct {
    struct tm temps_arrive; // located in time.h
    struct tm temps_creation;
    struct tm temps_fin;
    
    int temps_attente; // gap entre temps arrive & temps exec
    int tournround; // temps terminer - temps arrive
} PROCESS_STATISTICS;

typedef struct {
    int pid;
    char process_name[20];
    char user_id[20];
    int ppid;
    E_etat etat; // one of the values in the enum e_etat
    int prioritie; // priorities from 1 to 5 ; 1-critical

    INSTRUCTION* instructions;
    long programme_compteur; // max 9,223,372,036,854,775,807 instruction
    int memoire_necessaire; // en MB
    INSTRUCTION* current_instruction; // l'instruction en train de s executer

    int burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    int cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    long cpu_usage; // 

    PROCESS_STATISTICS statistics;

    int* pid_childrens;
    PCB* pid_sibling_next; // pointeur vers next sib
    PCB* pid_sibling_previous; // pointeur vers previous sib
} PCB;

// structures nedded par les fonctions du process_manager
typedef struct { // used by process manager: many iteraction over process list but obe contact with ready queue for time reducing
    PCB* first_element;
    int size;
} pcb_list; // stand for pcb first came

typedef struct {
    int pid; // l identifier du processus
    PCB* pcb; // l id du pcb du processus
} PROCESS_TABLE;

typedef struct {
    int pid; // processus id
    READY_QUEUE_ELEMENT* next; // pointeur vers element suivant
    READY_QUEUE_ELEMENT* previous; // pointeur vers element precedent
} READY_QUEUE_ELEMENT;

typedef struct { // ordred chaine
    READY_QUEUE_ELEMENT* head; // pointeur vers premier element du chaine
    READY_QUEUE_ELEMENT* tail; // pointeur vers last element
    int size; // nombre du nodes
} READY_QUEUE;



typedef struct {
    int pid; // l id processus
    RESSOURCE_ELEMENT* ressource; // ressource needed to execute the instruction
} BLOCKED_QUEUE_ELEMENT;

typedef struct { // circular chaine
    BLOCKED_QUEUE_ELEMENT* head; // first elme
    BLOCKED_QUEUE_ELEMENT* tail; // last one
    int size; // how many elements
} BLOCKED_QUEUE;

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
    char

} EXECUTION_QUEUE;

typedef struct {
    Algorithms algorithm;
    PCB* exec_proc; // processus en train de s'executer
    
    int quantum; // quantum de time pour RR
    struct tm start;
    struct tm end;
    int cpu_time_used; // en ms: end - start

    float current_time;

    SIMULATOR* simulator; // pointeur vers simulator

    ORDONNANCEUR_STATISTICS statistics; // pointeur vers les statistics du schedular
} ORDONNANCEUR;

typedef struct {
    RESSOURCES_ELEMENT* ressources; // again using malloc to allocate N ressources
    int ressource_count; // n ressources
} RESSOURCE_MANAGER;

// fonctions du process manager
typedef


typedef struct {
    PROCESS_TABLE* process_table; // pointeur vers process table
    int process_count; // n processes
    READY_QUEUE* ready_queue; // pointer to ready chaine
    BLOCKED_QUEUE* blocked_queue; // pointer to blocked
} PROCESS_MANAGER;


typedef struct {
    ORDONNANCEUR* schedular; // pointeur vers lordonnanceur
    PROCESS_MANAGER* process_manager; // pointeur to process manaer
    RESSOURCE_MANAGER* ressource_manager; // pointeur vers ressource
    int simulation_time;
    bool runing;
    
} SIMULATOR;


