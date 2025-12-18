#pragma once


#include "structs/process.h"

typedef enum {
    EXEC_SUCCESS,      // Instruction executed successfully
    EXEC_IO_REQUEST,   // Instruction needs I/O
    EXEC_MEMORY_FAULT, // Memory access violation
    EXEC_TERMINATED,   // Process terminated
    EXEC_QUANTUM_EXPIRED, // Time slice expired
    EXEC_ERROR         // General error
} EXECUT_RESPONSE;

typedef struct EXECUTION_RESULT {
    INSTRUCTION* instruction;  // The instruction that was executed
    EXECUT_RESPONSE response;  // Result of execution
    int cycles_taken;          // How many CPU cycles it took
    time_t completion_time;    // When it completed
} EXECUTION_RESULT;



typedef struct EXECUTION_QUEUE {
    int id; // l id du composant en train d'executer
    char name[10]; // name du composant qui execute

    INSTRUCTION* current_instruction; // l instruction en train de s'executer
    PCB* current_process; // process en train de s'executer
    int process_id; // l'pid du current process

    //function
    EXECUTION_RESULT* (*execute_instruction) (INSTRUCTION* instruction);

} EXECUTION_QUEUE;