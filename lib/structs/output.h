#pragma once

// Forward declarations to avoid circular includes
typedef struct PCB PCB;
typedef struct ORDONNANCEUR_STATISTICS ORDONNANCEUR_STATISTICS;

typedef struct EXECUTION_SEGMENT {
    int pid;
    float start_time;
    float end_time;
    char reason[20]; // "completed", "preempted", "blocked"
    struct EXECUTION_SEGMENT* next;
} EXECUTION_SEGMENT;

typedef struct PERFORMANCE_SUMMARY {
    // Timing
    float total_simulation_time;
    float cpu_utilization_percent;
    
    // Process metrics
    float avg_turnaround_time;
    float avg_waiting_time;
    float avg_response_time; // for preemptive algorithms
    float throughput;
    
    // Algorithm-specific
    int context_switches;
    int preemptions; // for preemptive algorithms
    int priority_inversions; // for priority algorithms
    int starved_processes; // for priority/SJF
    
    // Completion order
    int completion_order[100]; // assuming max 100 processes
    
} PERFORMANCE_SUMMARY;

// Function declarations
EXECUTION_SEGMENT* create_execution_segment(int pid, float start_time);
TASK log_execution_start(EXECUTION_SEGMENT** head, EXECUTION_SEGMENT** current, int pid, float start_time);
TASK log_execution_end(EXECUTION_SEGMENT** current, float end_time, const char* reason);
void print_gantt_chart(EXECUTION_SEGMENT* head);
void calculate_performance_summary(PERFORMANCE_SUMMARY* ps, float total_time, ORDONNANCEUR_STATISTICS* stats);
void print_performance_summary(PERFORMANCE_SUMMARY* summary);
void print_process_details(PCB* process_table_head);
void print_algorithm_output(EXECUTION_SEGMENT* head, PERFORMANCE_SUMMARY* ps, PCB* process_head, float total_time, ORDONNANCEUR_STATISTICS* stats);