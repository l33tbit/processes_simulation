#pragma once


#include "process.h"


typedef enum {
    NEED_RESSOURCE, DONE
} EXECUT_RESPONSE;

typedef struct {
    INSTRUCTION* instruction;
    EXECUT_RESPONSE response;
} EXECUTION_QUEUE_RESPONSE;

typedef struct {
    int id; // l id du composant en train d'executer
    char name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process
} EXECUTION_QUEUE;