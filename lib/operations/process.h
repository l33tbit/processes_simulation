#pragma once

#include <time.h>
#include <stdbool.h>
#include <stdio.h>

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
    if (self == NULL || self->statistics == NULL) {
        printf("ERROR ON: op_update_temps_arrive (NULL value)\n");
        return false;
    }
    time_t arrive = mktime(&temps_arrive);
    self->statistics->temps_arrive = arrive;
    return true;
}

bool op_update_temps_fin(PCB* self, struct tm temps_fin) { // updating temps_fin = market_terminated = update_turnround
    if (self == NULL || self->statistics == NULL) {
        printf("ERROR ON: op_update_temps_fin (NULL value)\n");
        return false;
    }

    time_t fin = mktime(&temps_fin);
    self->statistics->temps_fin = fin;
    
    // the soustraction

    self->statistics->tournround = difftime(self->statistics->temps_fin, self->statistics->temps_arrive);
    self->etat = TERMINATED;
    return true;
}

bool op_cpu_time_used(PCB* self, float cpu_temps_used) { // updating cpu_temps_used = updating_remaining_time
    if (self == NULL) {
        printf("ERROR ON: op_cpu_time_used (NULL value)\n");
        return false;
    }
    self->cpu_time_used += cpu_temps_used;
    return true;
}

bool op_update_cpu_usage(PCB* self, int cpu_usage) {
    if (self == NULL || cpu_usage == NULL) {
        printf("ERROR ON: op_update_cpu_usage (NULL value)\n");
        return false;
    }
    self->cpu_usage += cpu_usage;
    return true;
}   

bool op_update_temps_attente(PCB* self, float temps_attente) {
    if (self == NULL || temps_attente == 0) {
        printf("ERROR ON: op_update_waiting_time (NULL value)\n");
        return false;
    }
    self->statistics->temps_attente += temps_attente;
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

PCB* op_get_next_pcb(PCB* pcb) {
    return pcb->pid_sibling_next;
}

PCB* op_pid_sibling_previous(PCB* pcb) {
    return pcb->pid_sibling_previous;
}