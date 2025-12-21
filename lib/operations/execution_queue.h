#pragma once

#include "structs/execution_queue.h"
#include "structs/process.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


EXECUT_RESPONSE op_execute_instruction(INSTRUCTION* instruction) {

    instruction->state = COMPLETED;

    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 1; // sleep one nano sec

    nanosleep(&ts, NULL);

    return EXEC_SUCCESS;
}



