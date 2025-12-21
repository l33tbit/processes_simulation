#pragma once

#include "structs/process.h"
#include "structs/schedular.h"

typedef enum {

    EXEC_SUCCESS,      // Instruction executed successfully
    NEED_RESSOURCE,
    EXEC_ERROR         // General error

} EXECUT_RESPONSE;


typedef struct EXECUTION_QUEUE {

    int id; // l id du composant en train d'executer
    char name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process
    float quantum;

    ORDONNANCEUR* schedular;

    //function
    INSTRUCTION* (*next_instruction) (PCB* pcb); // retrieve the next instruction to pass it to execute instruction
    EXECUT_RESPONSE* (*execute_instruction) (INSTRUCTION* instruction); // execute it and return the result
    bool (*check_ressource_disponibility) (RESSOURCE ressource); // check if instruction is disponible
    bool (*mark_ressource_unavailable) (RESSOURCE ressource); // mark ressource indisponible

} EXECUTION_QUEUE;