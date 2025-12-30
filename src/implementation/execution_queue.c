
#include <unistd.h>

#include "../../lib/structs/schedular.h"
#include "../../lib/structs/process.h"


WORK_RETURN op_execute_rr(float quantum) {
    // sleep(quantum);
    return WORK_DONE;
}

WORK_RETURN op_execute_srtf(float quantum) {
    printf("=== execute_srtf CALLED with quantum=%f ===\n", quantum);

    // sleep(quantum);
    
    printf("=== execute_srtf RETURNING WORK_DONE ===\n");
    
    return WORK_DONE;
}

WORK_RETURN op_ex_kill(EXECUTION_QUEUE* self) {
    
    free(self);
    return WORK_DONE;
}

WORK_RETURN op_execute_sjf(float quantum) {
    printf("=== execute_sjf CALLED with quantum=%f ===\n", quantum);

    // sleep(quantum);
    
    printf("=== execute_sjf RETURNING WORK_DONE ===\n");
    
    return WORK_DONE;
}

WORK_RETURN op_execute_ppp(float run_time) {
    printf("=== execute_priority CALLED with time=%f ===\n", run_time);

    // sleep(quantum);

    printf("=== execute_priority RETURNING WORK_DONE ===\n");
    return WORK_DONE;
}

WORK_RETURN op_execute_ppn(float run_time) {
    printf("=== execute_priority_n CALLED with time=%f ===\n", run_time);

    sleep(0.01);

    printf("=== execute_priority_n RETURNING WORK_DONE ===\n");
    return WORK_DONE;
}


WORK_RETURN op_execute_fcfs(float quantum) {
    printf("=== execute_fcfs CALLED with quantum=%f ===\n", quantum);

    // sleep(quantum);
    
    printf("=== execute_fcfs RETURNING WORK_DONE ===\n");
    
    return WORK_DONE;
}

TASK ex_init(EXECUTION_QUEUE* self) {

    self->execute_rr = op_execute_rr;
    self->kill = op_ex_kill;
    self->execute_srtf = op_execute_srtf;
    self->execute_sjf = op_execute_sjf;
    self->execute_fcfs = op_execute_fcfs;
    self->execute_ppp = op_execute_ppp;
    self->execute_ppn = op_execute_ppn;

    return TASK_SUCC;
}


