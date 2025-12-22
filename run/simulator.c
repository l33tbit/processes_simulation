// #include "structs/simulator.h"
// #include "structs/process_manager.h"
// #include "structs/ressource_manager.h"
// #include "structs/schedular.h"
// #include "operations/simulator.h"
// #include "operations/process_manager.h"
// #include "operations/ressource_manager.h"
// #include "operations/schedular.h"
// #include "operations/execution_queue.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <time.h>
// #include <unistd.h>

// // Initialize and assign all function pointers for simulator
// SIMULATOR* op_initialize_simulator(char* csv_path, Algorithms algorithm, int quantum) {
//     SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));
    
//     if (simulator == NULL) {
//         fprintf(stderr, "ERROR: Failed to allocate simulator\n");
//         exit(1);
//     }
    
//     // Initialize fields
//     simulator->simulation_time = 0;
//     simulator->runing = false;
//     strncpy(simulator->csv_path, csv_path, sizeof(simulator->csv_path) - 1);
//     simulator->csv_path[sizeof(simulator->csv_path) - 1] = '\0';
    
//     // Assign function pointers
//     simulator->start = op_start;
//     simulator->stop = op_stop;
//     simulator->run_simulator = op_load_simulator;
//     simulator->load_processus = op_load_processus;
//     simulator->start_process_manager = op_start_process_manager;
//     simulator->start_ressource_manager = op_start_ressource_manager;
//     simulator->start_schedular = op_start_schedular;
//     simulator->check_ressource_disponibility = op_simul_check_instruction_disponibility;
//     simulator->signal_ressource_is_free = op_signal_ressource_is_free;
//     simulator->signal_ressource_free = op_signal_ressource_free;
//     simulator->ask_for_next_ready_element = op_ask_for_next_ready_element;
//     simulator->ask_sort_rt = op_ask_sort_rt;
//     simulator->ask_sort_priority = op_ask_sort_priority;
    
//     // Initialize managers
//     printf("Initializing Resource Manager...\n");
//     simulator->ressource_manager = simulator->start_ressource_manager(simulator);
//     simulator->ressource_manager->ressources = op_create_ressource_list();
//     simulator->ressource_manager->ressource_count = 6;
    
//     // Assign resource manager function pointers
//     simulator->ressource_manager->create_ressource_list = op_create_ressource_list;
//     simulator->ressource_manager->look_for_ressource_in_list = op_look_for_ressource_in_list;
//     simulator->ressource_manager->mark_ressource_available = op_mark_ressource_available;
//     simulator->ressource_manager->mark_ressource_unavailable = op_mark_ressource_unavailable;
//     simulator->ressource_manager->check_if_ressource_available = op_check_if_ressource_available;
    
//     printf("Loading processes from CSV...\n");
//     FILE* buffer = simulator->load_processus(simulator);
    
//     printf("Initializing Process Manager...\n");
//     simulator->process_manager = simulator->start_process_manager(simulator, buffer);
//     simulator->process_manager->process_table_head = op_create_process_table(buffer);
    
//     // Assign process manager function pointers
//     simulator->process_manager->create_process_table = op_create_process_table;
//     simulator->process_manager->create_blocked_queue = op_create_blocked_queue;
//     simulator->process_manager->sort_by_fc = op_sort_ready_by_fc;
//     simulator->process_manager->sort_by_rt = op_sort_ready_by_rt;
//     simulator->process_manager->sort_by_priority = op_sort_ready_by_priority;
//     simulator->process_manager->sort_by_burst = op_sort_ready_by_burst;
//     simulator->process_manager->update_process = op_update_process;
//     simulator->process_manager->push_to_ready_queue = op_push_to_ready_queue;
//     simulator->process_manager->delete_from_ready_queue = op_delete_from_ready_queue;
//     simulator->process_manager->add_process_to_blocked_queue = op_add_process_to_blocked_queue;
//     simulator->process_manager->delete_from_blocked_queue = op_delete_from_blocked_queue;
//     simulator->process_manager->get_blocked_queue_element = op_get_blocked_queue_element;
//     simulator->process_manager->get_next_ready_element = op_get_next_ready_element;
//     simulator->process_manager->assign_functions_to_pcb = op_assign_functions_to_pcb;
    
//     // Create ready queue based on algorithm
//     printf("Creating ready queue for algorithm...\n");
//     bool circular = (algorithm == RR);
//     simulator->process_manager->ready_queue_head = op_create_ready_queue(circular, simulator->process_manager->process_table_head);
    
//     // Sort ready queue based on algorithm
//     if (algorithm == FCFS) {
//         simulator->process_manager->ready_queue_head = op_sort_ready_by_fc(simulator->process_manager, circular);
//     } else if (algorithm == SJF) {
//         simulator->process_manager->ready_queue_head = op_sort_ready_by_burst(simulator->process_manager);
//     } else if (algorithm == PPP) {
//         simulator->process_manager->ready_queue_head = op_sort_ready_by_priority(simulator->process_manager);
//     }
    
//     // Initialize blocked queue
//     simulator->process_manager->blocked_queue_head = op_create_blocked_queue(NULL);
    
//     printf("Initializing Scheduler...\n");
//     simulator->schedular = simulator->start_schedular(algorithm, quantum, simulator);
//     simulator->schedular->algorithm = algorithm;
//     simulator->schedular->quantum = quantum;
//     simulator->schedular->simulator = simulator;
//     simulator->schedular->current_time = 0.0f;
//     simulator->schedular->statistics = op_create_statistics();
    
//     // Initialize statistics
//     simulator->schedular->statistics->cpu_total_temps_usage = 0.0f;
//     simulator->schedular->statistics->cpu_temps_unoccuped = 0.0f;
//     simulator->schedular->statistics->context_switch = 0;
//     simulator->schedular->statistics->total_temps_attente = 0.0f;
//     simulator->schedular->statistics->total_turnround = 0.0f;
//     simulator->schedular->statistics->processus_termine_count = 0;
//     simulator->schedular->statistics->troughtput = 0.0f;
    
//     // Assign scheduler function pointers
//     simulator->schedular->create_statistics = op_create_statistics;
//     simulator->schedular->signal_execute_instruction = op_signal_execute_instruction;
//     simulator->schedular->ressource_is_free = op_ressource_is_free;
//     simulator->schedular->update_schedular_statistics = op_update_schedular_statistics;
//     simulator->schedular->check_ressource_disponibility = op_check_ressource_disponibility;
//     simulator->schedular->execute_process = op_execute_process;
//     simulator->schedular->ask_for_next_ready_element = op_ask_for_next_ready_element;
    
//     // Create execution queue
//     EXECUTION_QUEUE* exec_queue = (EXECUTION_QUEUE*)malloc(sizeof(EXECUTION_QUEUE));
//     if (exec_queue == NULL) {
//         fprintf(stderr, "ERROR: Failed to allocate execution queue\n");
//         exit(1);
//     }
//     exec_queue->id = 1;
//     strcpy(exec_queue->name, "CPU");
//     exec_queue->current_instruction = NULL;
//     exec_queue->current_process = NULL;
//     exec_queue->process_id = -1;
//     exec_queue->quantum = quantum;
//     exec_queue->schedular = simulator->schedular;
//     exec_queue->execute_instruction = op_execute_instruction;
    
//     simulator->schedular->in_execution_queue = exec_queue;
    
//     if (buffer) fclose(buffer);
    
//     printf("Simulator initialized successfully!\n");
//     return simulator;
// }

// // Load processes from CSV file
// FILE* op_load_processus(SIMULATOR* self) {
//     FILE* file = fopen(self->csv_path, "r");
//     if (file == NULL) {
//         fprintf(stderr, "ERROR: Could not open file %s\n", self->csv_path);
//         exit(1);
//     }
//     return file;
// }

// // Main simulation loop
// void op_run_simulation(SIMULATOR* simulator) {
//     printf("\n=== Starting Simulation ===\n");
//     printf("Algorithm: ");
    
//     switch(simulator->schedular->algorithm) {
//         case FCFS: printf("First Come First Served\n"); break;
//         case SJF: printf("Shortest Job First\n"); break;
//         case SRTF: printf("Shortest Remaining Time First\n"); break;
//         case RR: printf("Round Robin (Quantum: %.2f)\n", simulator->schedular->quantum); break;
//         case PPP: printf("Preemptive Priority\n"); break;
//     }
    
//     simulator->runing = true;
//     PCB* current_process = simulator->process_manager->ready_queue_head;
//     float current_time = 0.0f;
    
//     while (simulator->runing) {
//         // Check if all processes are terminated
//         bool all_terminated = true;
//         PCB* check = simulator->process_manager->ready_queue_head;
        
//         if (check != NULL) {
//             PCB* start = check;
//             do {
//                 if (check->etat != TERMINATED) {
//                     all_terminated = false;
//                     break;
//                 }
//                 check = check->pid_sibling_next;
//             } while (check != NULL && check != start);
//         }
        
//         // Check blocked queue
//         if (all_terminated && simulator->process_manager->blocked_queue_head != NULL) {
//             all_terminated = false;
//         }
        
//         if (all_terminated) {
//             printf("\nAll processes completed!\n");
//             break;
//         }
        
//         // Find next ready process
//         if (current_process == NULL || current_process->etat == TERMINATED) {
//             current_process = simulator->process_manager->ready_queue_head;
            
//             // Skip terminated processes
//             while (current_process != NULL && current_process->etat == TERMINATED) {
//                 current_process = current_process->pid_sibling_next;
//             }
            
//             if (current_process == NULL) {
//                 // No ready processes, check blocked queue
//                 if (simulator->process_manager->blocked_queue_head != NULL) {
//                     printf("All processes blocked, freeing resources...\n");
//                     // In real implementation, we'd wait for I/O or timeout
//                     current_time += 1.0f;
//                     simulator->schedular->statistics->cpu_temps_unoccuped += 1.0f;
//                 }
//                 continue;
//             }
//         }
        
//         // Skip processes that haven't arrived yet
//         if (current_process->statistics->temps_arrive > current_time) {
//             float wait_time = current_process->statistics->temps_arrive - current_time;
//             current_time += wait_time;
//             simulator->schedular->statistics->cpu_temps_unoccuped += wait_time;
//             continue;
//         }
        
//         if (current_process->etat == READY || current_process->etat == BLOCKED) {
//             current_process->etat = READY;
//         }
        
//         printf("\n[Time %.2f] Executing Process PID=%d (%s)\n", 
//                current_time, current_process->pid, current_process->process_name);
        
//         // Execute process
//         process_return result = simulator->schedular->execute_process(simulator->schedular, current_process);
        
//         // Update statistics
//         simulator->schedular->statistics->context_switch++;
        
//         if (result == FINISHED) {
//             printf("  Process PID=%d completed\n", current_process->pid);
//             current_process->etat = TERMINATED;
            
//             // Update completion time
//             struct tm temp_time;
//             time_t now = time(NULL);
//             localtime_r(&now, &temp_time);
//             current_process->update_temps_fin(current_process, temp_time);
            
//             simulator->schedular->statistics->processus_termine_count++;
//             simulator->schedular->statistics->total_turnround += current_process->statistics->tournround;
//             simulator->schedular->statistics->total_temps_attente += current_process->statistics->temps_attente;
            
//             current_time += current_process->burst_time - current_process->cpu_time_used;
//             simulator->schedular->statistics->cpu_total_temps_usage += current_process->burst_time;
            
//         } else if (result == RESSOURCE_NEEDED) {
//             printf("  Process PID=%d blocked (waiting for resource)\n", current_process->pid);
//             current_process->etat = BLOCKED;
            
//             // Move to blocked queue
//             simulator->process_manager->blocked_queue_head = 
//                 simulator->process_manager->add_process_to_blocked_queue(
//                     simulator->process_manager->blocked_queue_head, current_process);
            
//             current_time += 1.0f; // Small time increment for blocked operation
            
//         } else if (result == QUANTUM_EXPIRED) {
//             printf("  Process PID=%d quantum expired\n", current_process->pid);
//             current_process->etat = READY;
//             current_time += simulator->schedular->quantum;
//             simulator->schedular->statistics->cpu_total_temps_usage += simulator->schedular->quantum;
//         }
        
//         // Get next process
//         current_process = simulator->schedular->ask_for_next_ready_element(simulator, current_process);
        
//         // Re-sort if needed (for preemptive algorithms)
//         if (simulator->schedular->algorithm == SRTF) {
//             simulator->process_manager->ready_queue_head = 
//                 simulator->process_manager->sort_by_rt(simulator->process_manager);
//             current_process = simulator->process_manager->ready_queue_head;
//         } else if (simulator->schedular->algorithm == PPP) {
//             simulator->process_manager->ready_queue_head = 
//                 simulator->process_manager->sort_by_priority(simulator->process_manager);
//             current_process = simulator->process_manager->ready_queue_head;
//         }
//     }
    
//     simulator->runing = false;
    
//     // Calculate final statistics
//     if (current_time > 0) {
//         simulator->schedular->statistics->troughtput = 
//             simulator->schedular->statistics->processus_termine_count / current_time;
//     }
    
//     printf("\n=== Simulation Complete ===\n");
// }

// // Display final statistics
// void op_display_statistics(SIMULATOR* simulator) {
//     ORDONNANCEUR_STATISTICS* stats = simulator->schedular->statistics;
    
//     printf("\n");
//     printf("=============================================\n");
//     printf("          SIMULATION STATISTICS\n");
//     printf("=============================================\n");
//     printf("Total CPU Time Used:      %.2f ms\n", stats->cpu_total_temps_usage);
//     printf("CPU Idle Time:            %.2f ms\n", stats->cpu_temps_unoccuped);
//     printf("Context Switches:         %d\n", stats->context_switch);
//     printf("Processes Completed:      %.0f\n", stats->processus_termine_count);
//     printf("Total Turnaround Time:    %.2f ms\n", stats->total_turnround);
//     printf("Total Waiting Time:       %.2f ms\n", stats->total_temps_attente);
    
//     if (stats->processus_termine_count > 0) {
//         printf("Avg Turnaround Time:      %.2f ms\n", 
//                stats->total_turnround / stats->processus_termine_count);
//         printf("Avg Waiting Time:         %.2f ms\n", 
//                stats->total_temps_attente / stats->processus_termine_count);
//     }
    
//     printf("Throughput:               %.4f processes/ms\n", stats->troughtput);
    
//     float total_time = stats->cpu_total_temps_usage + stats->cpu_temps_unoccuped;
//     if (total_time > 0) {
//         printf("CPU Utilization:          %.2f%%\n", 
//                (stats->cpu_total_temps_usage / total_time) * 100);
//     }
    
//     printf("=============================================\n");
// }

// // Start simulation
// void op_start(SIMULATOR* self, char* path) {
//     printf("Starting simulation...\n");
//     self->runing = true;
//     op_run_simulation(self);
//     op_display_statistics(self);
// }

// // Stop simulation
// void op_stop(SIMULATOR* self, char* path) {
//     printf("Stopping simulation...\n");
//     self->runing = false;
// }

// // Load simulator state (for future implementation)
// void op_load_simulator(SIMULATOR* self, char* path) {
//     printf("Loading simulator state from: %s\n", path);
//     // Implementation for loading saved state
// }

// // Clean up simulator resources
// void op_cleanup_simulator(SIMULATOR* simulator) {
//     if (simulator == NULL) return;
    
//     // Free process manager resources
//     if (simulator->process_manager) {
//         // Free process table
//         PCB* current = simulator->process_manager->process_table_head;
//         while (current != NULL) {
//             PCB* next = current->pid_sibling_next;
//             if (current->statistics) free(current->statistics);
            
//             // Free instruction chain
//             INSTRUCTION* inst = current->instructions_head;
//             while (inst != NULL) {
//                 INSTRUCTION* next_inst = inst->next;
//                 free(inst);
//                 inst = next_inst;
//             }
            
//             free(current);
//             current = next;
//         }
        
//         free(simulator->process_manager);
//     }
    
//     // Free resource manager
//     if (simulator->ressource_manager) {
//         RESSOURCE_ELEMENT* res = simulator->ressource_manager->ressources;
//         while (res != NULL) {
//             RESSOURCE_ELEMENT* next = res->next_ressource;
//             free(res);
//             res = next;
//         }
//         free(simulator->ressource_manager);
//     }
    
//     // Free scheduler
//     if (simulator->schedular) {
//         if (simulator->schedular->statistics) {
//             free(simulator->schedular->statistics);
//         }
//         if (simulator->schedular->in_execution_queue) {
//             free(simulator->schedular->in_execution_queue);
//         }
//         free(simulator->schedular);
//     }
    
//     free(simulator);
// }