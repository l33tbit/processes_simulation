#pragma once

#include "structs/process.h"

typedef enum {

    EXEC_SUCCESS,      // Instruction executed successfully
    EXEC_TERMINATED,   // Process terminated
    EXEC_QUANTUM_EXPIRED, // Time slice expired
    EXEC_ERROR         // General error

} EXECUT_RESPONSE;

typedef struct EXECUTION_RESULT {

    INSTRUCTION* instruction;  // The instruction that was executed
    EXECUT_RESPONSE response;  // Result of execution
    time_t completion_time;    // When it completed

} EXECUTION_RESULT;

typedef struct EXECUTION_QUEUE {

    int id; // l id du composant en train d'executer
    char name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process
    float quantum;

    //function
    INSTRUCTION* (*next_instruction) (PCB* pcb); // retrieve the next instruction to pass it to execute instruction
    EXECUTION_RESULT* (*execute_instruction) (INSTRUCTION* instruction, float quantum); // execute it and return the result
    bool (*check_instruction_disponibility) (INSTRUCTION* instruction); // check if instruction is disponible

} EXECUTION_QUEUE;