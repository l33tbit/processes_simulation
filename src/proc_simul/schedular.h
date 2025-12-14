#pragma once

#include "process.h" // for pcb
#include "simulator.h" // for SIMULATOR

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
    int id; // l id du composant en train d'executer
    char name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process
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
    EXECUTION_QUEUE* in_execution_queue; // pointeur vers queue d'execution

    ORDONNANCEUR_STATISTICS* statistics; // pointeur vers les statistics du schedular









} ORDONNANCEUR;
