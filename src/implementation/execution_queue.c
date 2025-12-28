
#include <unistd.h>

#include "../../lib/structs/schedular.h"
#include "../../lib/structs/process.h"


WORK_RETURN op_execute_rr(float quantum) {
    sleep(quantum);
    return WORK_DONE;
}

// WORK_RETURN op_execute_srtf(float quantum) {
//     printf("executing");
    
//     sleep(quantum);

//     printf("done");

//     return WORK_DONE;
// }

WORK_RETURN op_execute_srtf(float quantum) {
    printf("=== op_execute_srtf CALLED with quantum=%f ===\n", quantum);
    fflush(stdout);  // Force output
    sleep(quantum);
    
    // Don't sleep for now - just return immediately
    printf("=== op_execute_srtf RETURNING WORK_DONE ===\n");
    fflush(stdout);
    
    return WORK_DONE;
}

WORK_RETURN op_ex_kill(EXECUTION_QUEUE* self) {
    
    free(self);

    return WORK_DONE;
}



bool ex_init(EXECUTION_QUEUE* self) {

    self->execute_rr = op_execute_rr;
    self->kill = op_ex_kill;
    self->execute_srtf = op_execute_srtf;


    return true;
}


