#pragma once


#include "../../lib/structs/process.h" 
#include <time.h>
#include <stdbool.h>
#include "../../lib/structs/ressource.h" // for ressource enum


void op_free_pcb_pcb(PCB* pcb_copy) {

    if (pcb_copy == NULL) return; // prevent bug of freeing null
    

    
    // free the instructions also
    INSTRUCTION* current_instr = pcb_copy->instructions_head;

    while (current_instr != NULL) {
        INSTRUCTION* next_instr = current_instr->next;
        free(current_instr);
        current_instr = next_instr;
    }
    
    // free also statistics
    if (pcb_copy->statistics != NULL) {
     
        free(pcb_copy->statistics);
    }
    
    free(pcb_copy);
}

