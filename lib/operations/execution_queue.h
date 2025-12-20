#pragma once

#include "structs/execution_queue.h"
#include "structs/process.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>



typedef struct EXECUTION_RESULT {

    INSTRUCTION* instruction;  // The instruction that was executed
    EXECUT_RESPONSE response;  // Result of execution
    time_t completion_time;    // When it completed

} EXECUTION_RESULT;


typedef struct INSTRUCTION{
    int instruct_id; // the id of instruction because process can stop at it if need ressources, pointer because it can be too long
    PCB* process; // the id of the process owner

    float time_remaining; // in ms
    RESSOURCE type; // type of instruction which is ressource needed
    INSTRUCTION_STATE state; // state of instruction 
    struct INSTRUCTION* next;
} INSTRUCTION;


EXECUTION_RESULT* op_execute_instruction(INSTRUCTION* instruction, float quantum) {


    instruction->state = COMPLETED;

    EXECUTION_RESULT* response = (EXECUTION_RESULT*)malloc(sizeof(EXECUTION_RESULT)); 

    if (response == NULL) {
        fprintf(stderr, "ERROR ON: op_execute_instruction: Failed to allocate memory for response\n");
        exit(1);
    }

    response->instruction = instruction;
    response->response = EXEC_SUCCESS;
    response->completion_time = time(NULL);


    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 1; // sleep one nano sec

    nanosleep(&ts, NULL);

    return response;
}



