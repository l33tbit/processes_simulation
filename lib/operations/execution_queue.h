#pragma once

#include "structs/execution_queue.h"
#include "operations/helpers/execution_queue.h"

INSTRUCTION* op_next_instruction(PCB* pcb) {
    
}


EXECUTION_RESULT* op_execute_instruction(EXECUTION_QUEUE* self, INSTRUCTION* instruction) {
    self->check_instruction_disponibility(instruction);
}
