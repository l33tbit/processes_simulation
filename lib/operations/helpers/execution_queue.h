

#include <structs/process.h>
#include <structs/schedular.h>


bool op_check_instruction_disponibility(ORDONNANCEUR* schedular, INSTRUCTION* instruction) {

    return schedular->check_instruction_disponibility(instruction);

}