
#include <time.h>

enum {
    READY, BLOCKED, EXECUTION, TERMINATED
} E_etat;

typedef struct {
    char ressource_name[10];
    bool disponibilite;
} RESSOURCE;

typedef struct {
    struct tm temps_arrive; // located in time.h
    struct tm temps_creation;
    struct tm temps_fin;
    
    int temps_attente; // gap entre temps arrive & temps exec
    int tournround; // temps terminer - temps arrive
} PROCESS_STATISTICS;

typedef struct {
    int pcb_id;
    char process_name[20];
    char user_id[20];
    int ppid;
    E_etat etat; // one of the values in the enum e_etat
    int prioritie; // priorities from 1 to 5 ; 1-critical

    char* instructions;
    long programme_compteur; // max 9,223,372,036,854,775,807 instruction
    int memoire_necessaire; // en MB

    int burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    int cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    long cpu_usage; // 

    PROCESS_STATISTICS statistics;

    int* pid_childrens;
    PCB* pid_sibling_next; // pointeur vers next sib
    PCB* pid_sibling_previous; // pointeur vers previous sib
} PCB;

typedef struct {
    int pid; // l identifier du processus
    int pcb_id; // l id du pcb du processus
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
    char ressource_name[10];
    bool disponibilite;
} RESSOURCES_ELEMENT;

typedef struct {
    int pid; // l id processus
    RESSOURCES_ELEMENT* ressource; // ressource needed to execute the instruction
} BLOCKED_QUEUE_ELEMENT;

typedef struct { // circular chaine
    BLOCKED_QUEUE_ELEMENT* head; 
    BLOCKED_QUEUE_ELEMENT* tail;
    int size;
} BLOCKED_QUEUE;

enum {
    RR, SRTF, PPP, FCFS, SJF
} Algorithms;

typedef struct {
    Algorithms algorithm;
    READY_QUEUE* ready_queue;
    BLOCKED_QUEUE* blocked_queue; 
    PCB* exec_proc; // processus en train de s'executer
    
    int quantum; // quantum de time pour RR
    struct tm start;
    struct tm end;
    int cpu_time_used; // en ms: end - start
    
    RESSOURCES_ELEMENT* ressources; // again using malloc to allocate N ressources
} ORDONNANCEUR;















/*
// scheduling.h
#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "process.h"

// Déclaration des types de fonctions
typedef Process* (*SelectNextFunc)(void* context);
typedef void (*OnArrivalFunc)(void* context, Process* p);
typedef void (*OnCompletionFunc)(void* context, Process* p);
typedef void (*OnQuantumExpiryFunc)(void* context);
typedef void (*OnIOCompletionFunc)(void* context, Process* p);

typedef struct SchedulingAlgorithm {
    char name[20];
    
    // Pointeurs de fonction
    SelectNextFunc select_next;
    OnArrivalFunc on_process_arrival;
    OnCompletionFunc on_process_completion;
    OnQuantumExpiryFunc on_quantum_expiry;
    OnIOCompletionFunc on_io_completion;
    
    void* algorithm_data;
    
} SchedulingAlgorithm;

// Fonctions de création
SchedulingAlgorithm* create_fcfs_algorithm();
SchedulingAlgorithm* create_rr_algorithm(int quantum);
SchedulingAlgorithm* create_sjf_algorithm();

#endif
*/