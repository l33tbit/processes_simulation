#include "../../lib/structs/execution_queue.h"


TASK op_update_schedular_statistics(ORDONNANCEUR* self, float* exec_time, float* turnaround, float* temp_attente, bool finished) { // must check nullty

    if (self == NULL) {
        fprintf(stderr, "ERROR: self is NULL in op_update_schedular_statistics\n");
        return TASK_ERR;
    }
    
    if (self->statistics == NULL) {
        fprintf(stderr, "ERROR: statistics is NULL in op_update_schedular_statistics\n");
        return TASK_ERR;
    }

    if (exec_time == NULL) {
        fprintf(stderr, "ERROR: exec_time is NULL in op_update_schedular_statistics\n");
        return TASK_ERR;
    }

    if (finished == true) {
        // When finished, turnaround and temp_attente must be provided
        if (turnaround == NULL || temp_attente == NULL) {
            fprintf(stderr, "ERROR: turnaround or temp_attente is NULL when finished=true\n");
            return TASK_ERR;
        }
        
        // Safeguard: ensure waiting time is not negative
        float waiting_time = *temp_attente;
        if (waiting_time < 0.0f) {
            fprintf(stderr, "WARNING: Negative waiting time detected (%.2f), setting to 0\n", waiting_time);
            waiting_time = 0.0f;
        }
        
        fprintf(stderr, "DEBUG: Updating statistics - exec_time=%.2f, turnaround=%.2f, waiting=%.2f\n",
               *exec_time, *turnaround, waiting_time);
        
        self->statistics->processus_termine_count++;
        self->statistics->cpu_total_temps_usage += *exec_time;
        self->statistics->total_turnround += *turnaround;
        self->statistics->total_temps_attente += waiting_time;
        
        fprintf(stderr, "DEBUG: After update - count=%d, cpu_usage=%.2f, total_turnround=%.2f, total_waiting=%.2f\n",
               self->statistics->processus_termine_count, self->statistics->cpu_total_temps_usage,
               self->statistics->total_turnround, self->statistics->total_temps_attente);

    } else {
        // Note: Context switches are counted when we actually switch to a different process
        // in the scheduling algorithms, not here (to avoid double-counting)
        self->statistics->cpu_total_temps_usage += *exec_time;
    }
    
    return TASK_SUCC;
}

EXECUTION_SEGMENT* op_create_execution_segment(int pid, float start_time) {
    EXECUTION_SEGMENT* segment = (EXECUTION_SEGMENT*)malloc(sizeof(EXECUTION_SEGMENT));
    if (segment == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate EXECUTION_SEGMENT\n");
        return NULL;
    }
    segment->pid = pid;
    segment->start_time = start_time;
    segment->end_time = 0.0f;
    strcpy(segment->reason, "running");
    segment->next = NULL;
    return segment;
}

TASK op_log_execution_start(ORDONNANCEUR* self, int pid, float start_time) {
    EXECUTION_SEGMENT* new_segment = create_execution_segment(pid, start_time);
    if (new_segment == NULL) return TASK_ERR;
    
    if (self->execution_segments_head == NULL) {
        self->execution_segments_head = new_segment;
    } else {
        EXECUTION_SEGMENT* tail = self->execution_segments_head;
        while (tail->next != NULL) tail = tail->next;
        tail->next = new_segment;
    }
    self->current_segment = new_segment;
    return TASK_SUCC;
}

TASK op_log_execution_end(ORDONNANCEUR* self, float end_time, const char* reason) {
    if (self->current_segment == NULL) return TASK_ERR;
    self->current_segment->end_time = end_time;
    strcpy(self->current_segment->reason, reason);
    self->current_segment = NULL;
    return TASK_SUCC;
}

void op_print_gantt_chart(EXECUTION_SEGMENT* head) {
    printf("\n=== Gantt Chart ===\n");
    EXECUTION_SEGMENT* current = head;
    while (current != NULL) {
        printf("Time %.2f-%.2f: [P%d] (%s)\n", current->start_time, current->end_time, current->pid, current->reason);
        current = current->next;
    }
}

void op_calculate_performance_summary(ORDONNANCEUR* self, float total_time) {
    if (self->performance_summary == NULL) {
        self->performance_summary = (PERFORMANCE_SUMMARY*)malloc(sizeof(PERFORMANCE_SUMMARY));
        if (self->performance_summary == NULL) {
            fprintf(stderr, "ERROR: Failed to allocate PERFORMANCE_SUMMARY\n");
            return;
        }
    }
    PERFORMANCE_SUMMARY* ps = self->performance_summary;
    ps->algorithm = self->algorithm;
    ps->total_simulation_time = total_time;
    ps->cpu_utilization_percent = (self->statistics->cpu_total_temps_usage / total_time) * 100.0f;
    if (self->statistics->processus_termine_count > 0) {
        ps->avg_turnaround_time = self->statistics->total_turnround / self->statistics->processus_termine_count;
        ps->avg_waiting_time = self->statistics->total_temps_attente / self->statistics->processus_termine_count;
    } else {
        ps->avg_turnaround_time = 0.0f;
        ps->avg_waiting_time = 0.0f;
    }
    if (total_time > 0) {
        ps->throughput = (float)self->statistics->processus_termine_count / total_time;
    } else {
        ps->throughput = 0.0f;
    }
    ps->context_switches = self->statistics->context_switch;
    ps->preemptions = 0; // TODO: count from segments
    ps->priority_inversions = 0;
    ps->starved_processes = 0;
}

void op_print_performance_summary(PERFORMANCE_SUMMARY* summary, int algo) {
    printf("\n=== Performance Summary ===\n");
    printf("Total Simulation Time: %.2f\n", summary->total_simulation_time);
    printf("CPU Utilization: %.2f%%\n", summary->cpu_utilization_percent);
    printf("Average Turnaround Time: %.2f\n", summary->avg_turnaround_time);
    printf("Average Waiting Time: %.2f\n", summary->avg_waiting_time);
    printf("Throughput: %.4f processes/unit\n", summary->throughput);
    printf("Context Switches: %d\n", summary->context_switches);
}

void op_print_process_details(PCB* process_table_head) {
    printf("\n=== Process Details ===\n");
    PCB* current = process_table_head;
    while (current != NULL) {
        printf("PID %d: Arrival=%.2f, Burst=%.2f, Completion=%.2f, Wait=%.2f, Turnaround=%.2f\n",
               current->pid,
               current->statistics->temps_arrive,
               current->burst_time,
               current->statistics->temps_fin,
               current->statistics->temps_attente,
               current->statistics->tournround);
        current = current->pid_sibling_next;
    }
}