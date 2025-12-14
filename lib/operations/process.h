

#include "structs/process.h"

bool op_update_process(PCB* pcb, struct tm temps_arrive, struct tm temps_fin, float cpu_temps_used, float temps_attente, int cpu_usage) {

}

bool op_update_temps_arrive(PCB* pcb, struct tm temps_arrive) {

}

bool op_update_temps_fin(PCB* pcb, struct tm temps_fin) { // updating temps_fin = market_terminated = update_turnround

}

bool op_cpu_time_used(PCB* pcb, float cpu_temps_used) { // updating cpu_temps_used = updating_remaining_time

}

bool op_update_cpu_usage(PCB* pcb, int cpu_usage) {

}

bool op_update_waiting_time(PCB* pcb, float temps_attente) {
    
}