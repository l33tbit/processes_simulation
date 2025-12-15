#pragma once

#include <time.h>
#include <stdbool.h>

#include "structs/process.h"


bool op_update_temps_creation(PCB* self) {
    if (self == NULL) {
        return NULL;
    }
    struct tm temps_arrive;
    time_t now = mktime(&temps_arrive);

    self->statistics->temps_creation = now;
    return true;
}

bool op_update_temps_arrive(PCB* self, struct tm temps_arrive) {
    if (pcb == NULL || pcd->statistics == NULL) {
        printf("ERROR ON: op_update_temps_arrive (NULL value)\n");
        return false;
    }
    time_t arrive = mkdtime(&temps_arrive);
    pcb->statistics->temps_arrive = arrive;
    return true;
}

bool op_update_temps_fin(PCB* self, struct tm temps_fin) { // updating temps_fin = market_terminated = update_turnround
    if (pcb == NULL || pcb->statistics == NULL) {
        printf("ERROR ON: op_update_temps_fin (NULL value)\n");
        return false;
    }

    time_t fin = mktime(&temps_fin);
    pcb->statistics->temps_fin = fin;
    
    // the soustraction

    pcb->statistics->tournround = difftime(pcb->statistics->temps_fin, pcb->statistics->arrive);
    pcb->etat = TERMINATED;
    return true;
}

bool op_cpu_time_used(PCB* self, float cpu_temps_used) { // updating cpu_temps_used = updating_remaining_time
    if (pcb == NULL) {
        printf("ERROR ON: op_cpu_time_used (NULL value)\n");
        return false;
    }
    pcb->cpu_temps_used += cpu_temps_used;
    return true;
}

bool op_update_cpu_usage(PCB* self, int cpu_usage) {
    if (pcb == NULL || cpu_usage == NULL) {
        printf("ERROR ON: op_update_cpu_usage (NULL value)\n");
        return false;
    }
    pcb->cpu_usage += cpu_usage;
    return true;
}   

bool op_update_temps_attente(PCB* self, float temps_attente) {
    if (pcb == NULL || temps_attente == NULL) {
        printf("ERROR ON: op_update_waiting_time (NULL value)\n");
        return false;
    }
    pcb->statistics->temps_attente += temps_attente;
    return true;
}

PCB* op_pcb_define_next(PCB* self, PCB* next) {
    if (self == NULL || next == NULL)
        return NULL;
    
    self->pid_sibling_next = next;
    return self;
}

PCB* op_pcb_define_previous(PCB* self, PCB* previous) {
    if (self == NULL || previous == NULL)
        return NULL;
    self->pid_sibling_previous = previous;
    return self;
}

PCB* op_mark_instruction_terminated(PCB* self, INSTRUCTION* instruction) {
    if (self == NULL || instruction == NULL)
        return NULL;

    self->current_instruction->state = COMPLETED;
    return self;
}