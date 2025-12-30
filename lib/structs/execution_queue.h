#pragma once

#include "../../lib/structs/process.h"
#include "../../lib/structs/schedular.h"
#include "../../lib/structs/ressource.h"

typedef enum {

    EXEC_SUCCESS,    
    NEED_RESSOURCE,    
    EXEC_ERROR         

} EXECUT_RESPONSE;

typedef struct EXECUTION_QUEUE {
    
    int id;                             // 4 bytes: integer
    char name[10];                      // 10 bytes: chaine caracter 10 bytes
    
    // state
    struct INSTRUCTION* current_instruction;    // 8 bytes: pointer
    struct PCB* current_process;                // 8 bytes: pointer
    int process_id;                             // 4 bytes: integer
    
    // parameters    
    float quantum;                      // 4 bytes: float
    
    // link
    struct ORDONNANCEUR* schedular;     // 8 bytes: pointer
    
    // initialization & cleaning
    INITIALIZATION (*init)(struct EXECUTION_QUEUE* self);
    WORK_RETURN (*kill)(struct EXECUTION_QUEUE* self);
    
    // instruction related
    struct INSTRUCTION* (*next_instruction)(struct PCB* pcb);          
    EXECUT_RESPONSE* (*execute_instruction)(struct INSTRUCTION* instruction); 
    
    // ressource management
    TASK (*check_ressource_disponibility)(RESSOURCE ressource);         
    TASK (*mark_ressource_unavailable)(RESSOURCE ressource);      
    
    // execution algorithms
    WORK_RETURN (*execute_rr)(float quantum);     
    WORK_RETURN (*execute_srtf)(float quantum);   
    WORK_RETURN (*execute_sjf)(float quantum);     
    WORK_RETURN (*execute_fcfs)(float quantum);    
    WORK_RETURN (*execute_ppp)(float quantum);     
    WORK_RETURN (*execute_ppn)(float quantum); 
    
} EXECUTION_QUEUE;