#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib/structs/output.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/schedular.h"

EXECUTION_SEGMENT* create_execution_segment(int pid, float start_time) {
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

TASK log_execution_start(EXECUTION_SEGMENT** head, EXECUTION_SEGMENT** current, int pid, float start_time) {
    EXECUTION_SEGMENT* new_segment = create_execution_segment(pid, start_time);
    if (new_segment == NULL) return TASK_ERR;
    
    if (*head == NULL) {
        *head = new_segment;
    } else {
        EXECUTION_SEGMENT* tail = *head;
        while (tail->next != NULL) tail = tail->next;
        tail->next = new_segment;
    }
    *current = new_segment;
    return TASK_SUCC;
}

TASK log_execution_end(EXECUTION_SEGMENT** current, float end_time, const char* reason) {
    if (*current == NULL) return TASK_ERR;
    (*current)->end_time = end_time;
    strcpy((*current)->reason, reason);
    *current = NULL;
    return TASK_SUCC;
}

void print_gantt_chart(EXECUTION_SEGMENT* head) {
    FILE* file = fopen("/home/zeus/projects/processus_simulation/src/python_ui/outputs/gantt_chart.txt", "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening gantt_chart.txt\n");
        return;
    }
    EXECUTION_SEGMENT* current = head;
    while (current != NULL) {
        fprintf(file, "Time %.2f-%.2f: [P%d] (%s)\n", current->start_time, current->end_time, current->pid, current->reason);
        current = current->next;
    }
    fclose(file);
}

void calculate_performance_summary(PERFORMANCE_SUMMARY* ps, float total_time, ORDONNANCEUR_STATISTICS* stats) {
    ps->total_simulation_time = total_time;
    ps->cpu_utilization_percent = (stats->cpu_total_temps_usage / total_time) * 100.0f;
    if (stats->processus_termine_count > 0) {
        ps->avg_turnaround_time = stats->total_turnround / stats->processus_termine_count;
        ps->avg_waiting_time = stats->total_temps_attente / stats->processus_termine_count;
    } else {
        ps->avg_turnaround_time = 0.0f;
        ps->avg_waiting_time = 0.0f;
    }
    ps->throughput = stats->troughtput;
    ps->context_switches = stats->context_switch;
    ps->preemptions = 0; // TODO: count from segments
    ps->priority_inversions = 0;
    ps->starved_processes = 0;
}

void print_performance_summary(PERFORMANCE_SUMMARY* summary) {
    FILE* file = fopen("/home/zeus/projects/processus_simulation/src/python_ui/outputs/performance_summary.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening performance_summary.txt\n");
        return;
    }
    fprintf(file, "Total Simulation Time: %.2f\n", summary->total_simulation_time);
    fprintf(file, "CPU Utilization: %.2f%%\n", summary->cpu_utilization_percent);
    fprintf(file, "Average Turnaround Time: %.2f\n", summary->avg_turnaround_time);
    fprintf(file, "Average Waiting Time: %.2f\n", summary->avg_waiting_time);
    fprintf(file, "Throughput: %.4f\n", summary->throughput);
    fprintf(file, "Context Switches: %d\n", summary->context_switches);
    fclose(file);
}

void print_process_details(PCB* process_table_head) {
    FILE* file = fopen("/home/zeus/projects/processus_simulation/src/python_ui/outputs/process_details.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening process_details.txt\n");
        return;
    }
    PCB* current = process_table_head;
    while (current != NULL) {
        float turnaround = current->statistics->temps_fin - current->statistics->temps_arrive;
        float wait = turnaround - current->burst_time;
        fprintf(file, "PID %d: Arrival=%.2f, Burst=%.2f, Completion=%.2f, Wait=%.2f, Turnaround=%.2f\n",
               current->pid,
               current->statistics->temps_arrive,
               current->burst_time,
               current->statistics->temps_fin,
               wait,
               turnaround);
        current = current->pid_sibling_next;
    }
    fclose(file);
}

void print_algorithm_output(EXECUTION_SEGMENT* head, PERFORMANCE_SUMMARY* ps, PCB* process_head, float total_time, ORDONNANCEUR_STATISTICS* stats) {
    calculate_performance_summary(ps, total_time, stats);
    print_gantt_chart(head);
    print_performance_summary(ps);
    print_process_details(process_head);
}