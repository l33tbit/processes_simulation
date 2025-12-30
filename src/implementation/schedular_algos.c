#include "../../lib/structs/schedular.h"
#include <math.h>
#include <stdbool.h>



WORK_RETURN select_sjf(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);


    float temps = 0;

    if (quantum == -1)
        quantum = 1.0f;

    float runed = 0;
    int last_checked_time = -1;  // NEW: Track last checked arrival time
    
    printf("Starting SJF scheduler\n");

    int previous_pid_sjf = -1;  // Track previous process for context switch counting
    
    do {
        // Check if we've passed an integer boundary
        int current_int_time = (int)temps;
        
        // FIXED: Only check each integer time ONCE
        if (current_int_time > last_checked_time && 
            current_int_time <= max_arrival_time) {
            
            printf("\n=== Time %.0f: Checking for new arrivals ===\n", (float)current_int_time);

            printf("dddddddddddddddebg runed =: %f", runed);
            
            if (self->sched_update_process_manager(self, (float)current_int_time, &runed) == TASK_ERR) {
                fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                return UPDATE_ERROR;
            }

            // Update ready queue with new arrivals
            if (self->update_ready_queue(self, false) == TASK_ERR) {
                fprintf(stderr, "ERROR: update_ready_queue failed\n");
                return UPDATE_ERROR;
            }

            // Sort by burst time (SJF)
            if (self->ask_sort_sjf(self) == TASK_ERR) {
                fprintf(stderr, "ERROR: ask_sort_sjf failed\n");
                return UPDATE_ERROR;
            }
            
            last_checked_time = current_int_time;  // Update last checked time
        }
        
        // Get process with shortest burst time
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
        // Count context switch if switching to a different process
        // Note: First process (previous_pid_sjf == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid_sjf != -1 && self->exec_proc->pid != previous_pid_sjf) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: SJF context switch from PID %d to PID %d\n", previous_pid_sjf, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid_sjf = self->exec_proc->pid;
        }
        
        if (self->exec_proc != NULL) {
            // For SJF, we need to wait until the process arrives
            if (self->exec_proc->statistics->temps_arrive > temps) {
                // Process hasn't arrived yet, advance time to its arrival
                temps = self->exec_proc->statistics->temps_arrive;
                printf("Waiting for PCB %d to arrive at time %f\n", 
                       self->exec_proc->pid, temps);
                continue;
            }
            
            printf("\nRunning PCB %d at time %f (burst: %f)\n",
                   self->exec_proc->pid, temps, self->exec_proc->burst_time);
            
            // Check resources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    printf("PCB %d blocked, skipping\n", self->exec_proc->pid);
                    // Remove from ready queue and re-sort
                    if (self->ask_sort_sjf(self) == TASK_ERR) {
                        fprintf(stderr, "ERROR: ask_sort_sjf failed after blocking\n");
                        return UPDATE_ERROR;
                    }
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }
            
            // SJF is non-preemptive: run the process to completion
            float run_time = self->exec_proc->burst_time;
            printf("Executing for %f time units (full burst)\n", run_time);
 
            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            // Execute
            if (self->execution_queue->execute_sjf(run_time) != WORK_DONE) { 
                printf("ERROR: execute_sjf failed!\n");
                fflush(stdout);
                return WORK_ERROR;
            }

            runed = run_time;

            // Update time
            temps += run_time;
            
            log_execution_end(&self->current_segment, temps, "completed");
            
            // Save PID before updating
            int current_pid = self->exec_proc->pid;
            
            // Calculate stats before updating (which might free the PCB)
            float burst = self->exec_proc->burst_time;
            float arrival = self->exec_proc->statistics->temps_arrive;
            float turnaround = temps - arrival;
            float waiting = turnaround - burst;
            

            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        &temps, &run_time);

            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            // Update statistics
            if (self->update_schedular_statistics(self, &run_time, 
                &turnaround,
                &waiting,
                true) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            printf("PCB %d completed at time %f\n", current_pid, temps);
            
            // Update previous_pid for next iteration (when we get the next process, it will count as context switch)
            previous_pid_sjf = current_pid;
            // Clear current execution pointer
            self->exec_proc = NULL;
            
        } else {
            // No ready process
            if (temps < max_arrival_time) {
                // Advance to next integer time
                int next_int_time = (int)temps + 1;
                temps = (float)next_int_time;
                printf("No ready processes, advancing to time %f\n", temps);
            } else {
                break;  // No more arrivals expected
            }
        }
        
    } while (1);
    
    printf("\nSJF scheduler finished at time %f\n", temps);
    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 5);
    return WORK_DONE;
}


WORK_RETURN select_ppn(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    int last_checked_time = -1;
    int previous_pid_ppn = -1;  // Track previous process for context switch counting
    
    printf("Starting PPN (Priority Non-Preemptive) scheduler\n");

    do {
        // Check for new arrivals at integer times
        int current_int_time = (int)temps;
        
        if (current_int_time > last_checked_time && 
            current_int_time <= max_arrival_time) {
            
            printf("\n=== Time %.0f: Checking for new arrivals ===\n", (float)current_int_time);
            fflush(stdout);
            
            if (self->sched_update_process_manager(self, (float)current_int_time, NULL) == TASK_ERR) {
                fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                return UPDATE_ERROR;
            }

            // Update ready queue with new arrivals
            if (self->update_ready_queue(self, false) == TASK_ERR) {
                fprintf(stderr, "ERROR: update_ready_queue failed\n");
                return UPDATE_ERROR;
            }

            // CORRECTED: Sort by PRIORITY (not burst time)
            if (self->ask_sort_priority(self) == TASK_ERR) {
                fprintf(stderr, "ERROR: ask_sort_priority failed\n");
                return UPDATE_ERROR;
            }
            
            last_checked_time = current_int_time;
        }
        
        // Get process with HIGHEST PRIORITY (lowest priority number)
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
        // Count context switch if switching to a different process
        // Note: First process (previous_pid_ppn == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid_ppn != -1 && self->exec_proc->pid != previous_pid_ppn) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: PPN context switch from PID %d to PID %d\n", previous_pid_ppn, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid_ppn = self->exec_proc->pid;
        }
        
        if (self->exec_proc != NULL) {
            printf("\nRunning PCB %d at time %f (priority: %d, burst: %f)\n",
                   self->exec_proc->pid, temps, 
                   self->exec_proc->prioritie,  // ADD PRIORITY HERE
                   self->exec_proc->burst_time);
            
            // Check resources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    printf("PCB %d blocked, removing from queue\n", self->exec_proc->pid);
                    if (self->remove_from_ready_queue(self, self->exec_proc, temps) == TASK_ERR) {
                        fprintf(stderr, "ERROR: remove_from_ready_queue failed\n");
                        return UPDATE_ERROR;
                    }
                    // Re-sort after removal
                    if (self->ask_sort_priority(self) == TASK_ERR) {
                        fprintf(stderr, "ERROR: ask_sort_priority failed\n");
                        return UPDATE_ERROR;
                    }
                    self->exec_proc = NULL;
                    continue;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }
            
            // NON-PREEMPTIVE: Run to completion
            float run_time = self->exec_proc->remaining_time;
            printf("Executing PCB %d (priority %d) for %f time units\n", 
                   self->exec_proc->pid, self->exec_proc->prioritie, run_time);
 
            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            // Execute
            if (self->execution_queue->execute_ppn(run_time) != WORK_DONE) {
                fprintf(stderr, "ERROR: execute_ppn failed\n");
                return WORK_ERROR;
            }

            // Update time
            temps += run_time;
            
            log_execution_end(&self->current_segment, temps, "completed");
            
            // Save PID before updating
            int current_pid = self->exec_proc->pid;
            int current_priority = self->exec_proc->prioritie;

            // Calculate stats before updating
            float burst = self->exec_proc->burst_time;
            float arrival = self->exec_proc->statistics->temps_arrive;
            float turnaround = temps - arrival;
            float waiting = turnaround - burst;
            
            // Update process (mark as completed)
            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, &temps, &run_time);
            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }

            // Update statistics (PPN was missing this!)
            if (self->update_schedular_statistics(self, &run_time, &turnaround, &waiting, true) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            printf("PCB %d (priority %d) completed at time %f\n", 
                   current_pid, current_priority, temps);
            
            // Update previous_pid for next iteration (when we get the next process, it will count as context switch)
            previous_pid_ppn = current_pid;
            // Clear current execution pointer
            self->exec_proc = NULL;
            
            // Check for new arrivals that came during execution
            if (temps <= max_arrival_time) {
                // Update at current time to catch any arrivals
                if (self->sched_update_process_manager(self, temps, NULL) == TASK_ERR) {
                    fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                    return UPDATE_ERROR;
                }
                
                if (self->update_ready_queue(self, false) == TASK_ERR) {
                    fprintf(stderr, "ERROR: update_ready_queue failed\n");
                    return UPDATE_ERROR;
                }
                
                if (self->ask_sort_priority(self) == TASK_ERR) {
                    fprintf(stderr, "ERROR: ask_sort_priority failed\n");
                    return UPDATE_ERROR;
                }
            }
            
        } else {
            // No ready process
            if (temps < max_arrival_time) {
                // Advance to next integer time
                int next_int_time = (int)temps + 1;
                temps = (float)next_int_time;
                printf("No ready processes, advancing to time %f\n", temps);
            } else {
                break;
            }
        }
        
    } while (1);
    
    printf("\nPPN scheduler finished at time %f\n", temps);
    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 4);
    return WORK_DONE;
}



WORK_RETURN select_rr(ORDONNANCEUR* self, float quantum) {

    float max_arrival_time = self->get_max_arrival_time(self);

    float temps = 0;
    float proc_temps = 0;

    printf("hiiiiiit select_rr\n\n\n");

    int previous_pid = -1;  // Use PID instead of pointer to avoid stale pointer issues
    
    do {

        printf("\n----temps %f\n", temps);
        printf("----temps %f\n", proc_temps);
    
        self->exec_proc = self->sched_ask_for_next_ready_element(self, self->exec_proc); // get the next element

        // Count context switch if we're switching to a different process
        // Note: First process (previous_pid == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid != -1 && self->exec_proc->pid != previous_pid) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: RR context switch from PID %d to PID %d\n", previous_pid, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid = self->exec_proc->pid;
        } else {
            // No process available, reset previous_pid
            previous_pid = -1;
        }

        print_pcb(self->exec_proc);
        fflush(stdout);

        if (self->exec_proc  != NULL) {

            // check ressources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    // Process blocked - reset previous_pid since we'll skip this process
                    previous_pid = -1;
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
            }

            // Save current PID before execution (in case process is freed)
            int current_exec_pid = self->exec_proc->pid;
            
            float time_to_execute;
            if (self->exec_proc->remaining_time < quantum) {
                if (self->exec_proc->remaining_time < 0.00001f) 
                    time_to_execute = 0.00001f;            
                else
                    time_to_execute = self->exec_proc->remaining_time;
            } else {
                time_to_execute = quantum;
            }

            // Ensure minimum execution time
            if (time_to_execute < 0.00001f) {
                time_to_execute = 0.00001f;
            }

            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            if (self->execution_queue->execute_rr(time_to_execute) != WORK_DONE) { // if remaining time is less than the quantum then execute for remaining time not quantum else execute for quantum
                return WORK_ERROR;
            }

            time_t daba;
            time_t* temps_fin_ptr = NULL;
            float time_used; // Variable to track actual time used
            if (time_to_execute < quantum) {

                temps += time_to_execute;


                float n_quantum = time_to_execute;
                time_used = n_quantum; // Use the actual remaining time

                // Calculate stats before update_process frees the PCB
                int completed_pid = self->exec_proc->pid;  // Save PID before process is freed
                float burst = self->exec_proc->burst_time;
                float arrival = self->exec_proc->statistics->temps_arrive;
                float turnaround = temps - arrival;
                float waiting = turnaround - burst;

                // Pass n_quantum instead of quantum
                PROCESS_UPDATE update = self->update_process(self, self->exec_proc, &temps, &n_quantum);
                
                // After update_process, the process may be freed, so maintain previous_pid
                // The next process will be different, so context switch will be counted in next iteration
                previous_pid = completed_pid;
                
                if (update != UPDATED) {
                    return WORK_ERROR;
                }

                if (
                    self->update_schedular_statistics(self, &n_quantum, &turnaround, &waiting, true) != TASK_SUCC 
                ) {
                    return WORK_ERROR;
                }
                
                log_execution_end(&self->current_segment, temps, "completed");
                // Process completed - next iteration will get a new process (context switch counted there)
                
            } else {

                temps += quantum;

                time_used = quantum; // se the full quantum
                
                PROCESS_UPDATE update = self->update_process(self, self->exec_proc, temps_fin_ptr, &quantum);

                if (update != UPDATED) {
                    return WORK_ERROR;
                }

                if (
                    self->update_schedular_statistics(self, &quantum, NULL, NULL, false) != TASK_SUCC
                ) {
                    return WORK_ERROR;
                }

                log_execution_end(&self->current_segment, temps, "preempted");
                // Process preempted - next iteration will get next process (context switch counted there)
                // Note: previous_pid is maintained, so if next process is different, it will be counted

            }

        }

        // should update temps arrive

        if ((float)(int)temps > proc_temps) {// get floor like 1.2365 -> 1.0000

            proc_temps = (float)(int)temps;


            if (self->sched_update_process_manager(self, proc_temps, NULL) == TASK_ERR) {

                fprintf(stderr, "ERROR ON: sched_update_process_manager failed\n");
                return WORK_ERROR;
            
            } else {

                printf("########## hit update readu_queue\n");
                if (proc_temps <= max_arrival_time) { // update only if max remaining time is inferior or equal to max arrival time, for time consuming
                    if (self->update_ready_queue(self, true) == TASK_ERR) {
                        fprintf(stderr, "ERROR ON: update_ready_queue failed\n");
                        return WORK_ERROR;
                    }
                }
            }
        }

    } while (self->exec_proc != NULL);

    // print_pcb_chaine(self->simulator->process_manager->process_table_head);

    if (temps >= max_arrival_time) {
        printf("Scheduler terminated: All processes completed\n");
    } else {
        printf("Scheduler terminated early (time: %.1f, max arrival: %.1f)\n", 
        temps, max_arrival_time);
    }

    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 0);
    return WORK_DONE;
}


WORK_RETURN select_srtf(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    float proc_temps = 0;  // Last integer time we processed
    int previous_pid_srtf = -1;  // Track previous process for context switch counting

    quantum = 1.0f;
    
    printf("Starting SRTF scheduler\n");
    fflush(stdout);

    do {
        // Check if we've passed an integer boundary
        if ((int)temps > (int)proc_temps && temps <= max_arrival_time) {
            proc_temps = (float)(int)temps;
            
            printf("\n=== Time %.0f: Checking for new arrivals ===\n", proc_temps);
            
            if (self->sched_update_process_manager(self, proc_temps, NULL) == TASK_ERR) {
                fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                return UPDATE_ERROR;
            }

            // Update ready queue with new arrivals
            if (self->update_ready_queue(self, false) == TASK_ERR) {
                fprintf(stderr, "ERROR: update_ready_queue failed\n");
                return UPDATE_ERROR;
            }

            printf("aaaaaaaaaaaaaaaaaaa");
            fflush(stdout);
            // CRITICAL: Sort by remaining time after new arrivals
            if (self->ask_sort_rt(self) == TASK_ERR) {
                fprintf(stderr, "ERROR: ask_sort_rt failed\n");
                return UPDATE_ERROR;
            }
        }
        
        // Get process with shortest remaining time
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
        // Count context switch if switching to a different process
        // Note: First process (previous_pid_srtf == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid_srtf != -1 && self->exec_proc->pid != previous_pid_srtf) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: SRTF context switch from PID %d to PID %d\n", previous_pid_srtf, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid_srtf = self->exec_proc->pid;
        }
        
        if (self->exec_proc != NULL) {
            printf("\nRunning PCB %d at time %f (remaining: %f)\n",
                   self->exec_proc->pid, temps, self->exec_proc->remaining_time);
            
            // Check resources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    printf("PCB %d blocked, skipping\n", self->exec_proc->pid);
                    // Remove blocked process from ready queue
                    if (self->ask_sort_rt(self) == TASK_ERR) {  // Re-sort after removal
                        fprintf(stderr, "ERROR: ask_sort_rt failed after blocking\n");
                        return UPDATE_ERROR;
                    }
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }
            
            // Calculate run time: min(quantum, remaining, time_to_next_integer)
            float time_to_next_int = ceilf(temps) - temps;
            float run_time = self->exec_proc->remaining_time;
            
            if (run_time > quantum) run_time = quantum;
            if (run_time > time_to_next_int && time_to_next_int > 0.0001f) {
                run_time = time_to_next_int;
            }
            
            printf("Executing for %f time units\n", run_time);
            
            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            // Execute
            if (self->execution_queue->execute_srtf(run_time) != WORK_DONE) {
                return WORK_ERROR;
            }

            // Update time
            temps += run_time;
            
            // Save PID before updating (in case process is freed)
            int current_pid = self->exec_proc->pid;
            
            // Check if process completed
            float new_remaining = self->exec_proc->remaining_time - run_time;
            bool completed = (new_remaining <= 0.00001f);

            // Save stats for statistics update if completed (because PCB will be freed)
            float turnaround = 0;
            float temps_attente = 0;
            bool valid_stats = false;
            if (completed) {
                float burst_time = self->exec_proc->burst_time;
                float temps_arrive = self->exec_proc->statistics->temps_arrive;
                
                // Validate burst_time - it should be positive and reasonable
                if (burst_time <= 0.0f || burst_time > 10000.0f || !isfinite(burst_time)) {
                    fprintf(stderr, "WARNING: Invalid burst_time %.2f for PID %d, trying to recover\n", 
                           burst_time, current_pid);
                    // Try to recover from remaining_time + cpu_time_used (original burst)
                    float recovered_burst = self->exec_proc->remaining_time + self->exec_proc->cpu_time_used;
                    if (recovered_burst > 0.0f && recovered_burst <= 10000.0f && isfinite(recovered_burst)) {
                        burst_time = recovered_burst;
                        fprintf(stderr, "INFO: Recovered burst_time %.2f for PID %d\n", burst_time, current_pid);
                    } else {
                        fprintf(stderr, "ERROR: Cannot recover valid burst_time for PID %d, skipping statistics\n", current_pid);
                        burst_time = 0.0f;
                        valid_stats = false;
                    }
                } else {
                    valid_stats = true;
                }
                
                // Validate temps_arrive
                if (temps_arrive < 0.0f || temps_arrive > 10000.0f || !isfinite(temps_arrive)) {
                    fprintf(stderr, "ERROR: Invalid temps_arrive %.2f for PID %d\n", temps_arrive, current_pid);
                    valid_stats = false;
                }
                
                if (valid_stats) {
                    // Calculate temps_attente manually as we can't access PCB after free
                    turnaround = temps - temps_arrive;
                    temps_attente = turnaround - burst_time;
                    
                    // Final validation of calculated values
                    if (!isfinite(turnaround) || !isfinite(temps_attente)) {
                        fprintf(stderr, "ERROR: Non-finite values in calculation for PID %d\n", current_pid);
                        valid_stats = false;
                    } else if (temps_attente < -0.01f) {
                        // Small negative values might be due to rounding, clamp to 0
                        temps_attente = 0.0f;
                    } else if (temps_attente > 100000.0f) {
                        fprintf(stderr, "ERROR: Suspiciously large waiting time %.2f for PID %d (turnaround=%.2f, burst=%.2f)\n",
                               temps_attente, current_pid, turnaround, burst_time);
                        valid_stats = false;
                    }
                }
            }


            
            // Update process. Only pass &daba (temps_fin) if completed.
            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        completed ? &temps : NULL, &run_time);
            if (update != UPDATED) {
                return UPDATE_ERROR;
            }
            
            // Update statistics only if we have valid stats
            if (completed && valid_stats) {
                if (self->update_schedular_statistics(self, &run_time, 
                    &turnaround,
                    &temps_attente,
                    true) != TASK_SUCC) {
                    return UPDATE_ERROR;
                }
            } else if (completed && !valid_stats) {
                // Still update CPU time even if stats are invalid
                if (self->update_schedular_statistics(self, &run_time, 
                    NULL,
                    NULL,
                    false) != TASK_SUCC) {
                    return UPDATE_ERROR;
                }
            } else if (!completed) {
                // Process not completed, just update context switch
                if (self->update_schedular_statistics(self, &run_time, 
                    NULL,
                    NULL,
                    false) != TASK_SUCC) {
                    return UPDATE_ERROR;
                }
            }
            
            if (completed) {
                printf("PCB %d completed at time %f\n", current_pid, temps);
                // When process completes, it's removed from ready queue
                // Update previous_pid_srtf for next iteration (when we get the next process, it will count as context switch)
                previous_pid_srtf = current_pid;
                // We need to re-sort for the next iteration
                self->exec_proc = NULL;  // Clear current execution pointer
            } else {
                // Process not completed - remaining_time changed, need to re-sort
                if (self->ask_sort_rt(self) == TASK_ERR) {
                    fprintf(stderr, "ERROR: ask_sort_rt failed after partial execution\n");
                    return UPDATE_ERROR;
                }
                
                // Check if we need to preempt (another process now has shorter remaining time)
                PCB* new_shortest = self->sched_get_ready_queue_head(self);
                if (new_shortest != NULL && new_shortest->pid != current_pid) {
                    // Preempt: update previous_pid so next iteration counts context switch
                    previous_pid_srtf = current_pid;
                    // Will switch to new process in next iteration (context switch counted there)
                    self->exec_proc = NULL;  // Will pick new process next iteration
                } else {
                    // No preemption, continue with same process
                    previous_pid_srtf = current_pid;
                }
            }
            
            if (completed) {
                log_execution_end(&self->current_segment, temps, "completed");
            } else {
                log_execution_end(&self->current_segment, temps, "preempted");
            }
            
        } else {
            // No ready process, advance to next integer time
            int next_int_time = (int)temps + 1;
            if (next_int_time <= max_arrival_time) {
                temps = (float)next_int_time;
                printf("No ready processes, advancing to time %f\n", temps);
            } else {
                break;  // No more arrivals expected
            }
        }
        
    } while (1);  // Changed to infinite loop, break when done
    
    printf("\nSRTF scheduler finished at time %f\n", temps);
    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 2);
    return WORK_DONE;
}

WORK_RETURN select_ppp(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    float proc_temps = 0;  // Last integer time we processed
    int previous_pid_ppp = -1;  // Track previous process for context switch counting

    if (quantum == -1)
        quantum = 1.0f;
    
    printf("Starting PPP (Priority Preemptive) scheduler\n");

    do {
        // Check if we've passed an integer boundary
        if ((int)temps > (int)proc_temps && temps <= max_arrival_time) {
            proc_temps = (float)(int)temps;
            
            printf("\n=== Time %.0f: Checking for new arrivals ===\n", proc_temps);
            
            if (self->sched_update_process_manager(self, proc_temps, NULL) == TASK_ERR) {
                fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                return UPDATE_ERROR;
            }

            // Update ready queue with new arrivals
            if (self->update_ready_queue(self, false) == TASK_ERR) {
                fprintf(stderr, "ERROR: update_ready_queue failed\n");
                return UPDATE_ERROR;
            }

            // CRITICAL: Sort by priority after new arrivals
            if (self->ask_sort_priority(self) == TASK_ERR) {
                fprintf(stderr, "ERROR: ask_sort_priority failed\n");
                return UPDATE_ERROR;
            }
        }
        
        // Get process with HIGHEST PRIORITY (lowest priority number)
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
        // Count context switch if switching to a different process
        // Note: First process (previous_pid_ppp == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid_ppp != -1 && self->exec_proc->pid != previous_pid_ppp) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: PPP context switch from PID %d to PID %d\n", previous_pid_ppp, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid_ppp = self->exec_proc->pid;
        }
        
        if (self->exec_proc != NULL) {
            // DEBUG: Print priority to verify
            printf("\nRunning PCB %d at time %f (priority: %d, remaining: %f)\n",
                   self->exec_proc->pid, temps, 
                   self->exec_proc->prioritie,  // ADD THIS
                   self->exec_proc->remaining_time);
            
            // Check resources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    printf("PCB %d (priority: %d) blocked, moving to blocked queue\n", 
                           self->exec_proc->pid, self->exec_proc->prioritie);
                    
                    // Remove blocked process from ready queue
                    if (self->remove_from_ready_queue(self, self->exec_proc, temps) == TASK_ERR) {
                        fprintf(stderr, "ERROR: remove_from_ready_queue failed\n");
                        return UPDATE_ERROR;
                    }
                    
                    // Re-sort after removal
                    if (self->ask_sort_priority(self) == TASK_ERR) {
                        fprintf(stderr, "ERROR: ask_sort_priority failed after blocking\n");
                        return UPDATE_ERROR;
                    }
                    
                    self->exec_proc = NULL;
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }
            
            // Calculate run time: min(quantum, remaining, time_to_next_integer)
            float time_to_next_int = ceilf(temps) - temps;
            float run_time = self->exec_proc->remaining_time;
            
            if (run_time > quantum) run_time = quantum;
            if (run_time > time_to_next_int && time_to_next_int > 0.0001f) {
                run_time = time_to_next_int;
            }
            
            printf("Executing PCB %d (priority: %d) for %f time units\n", 
                   self->exec_proc->pid, self->exec_proc->prioritie, run_time);
            
            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            // Execute
            if (self->execution_queue->execute_ppp(run_time) != WORK_DONE) {
                fprintf(stderr, "ERROR: execute_ppp failed\n");
                return WORK_ERROR;
            }

            // Update time
            temps += run_time;
            
            // Save PID and priority before updating (in case process is freed)
            int current_pid = self->exec_proc->pid;
            int current_priority = self->exec_proc->prioritie;
            
            // Check if process completed
            float new_remaining = self->exec_proc->remaining_time - run_time;
            bool completed = (new_remaining <= 0.00001f);

            // Save stats for statistics update if completed
            float turnaround = 0;
            float temps_attente = 0;
            if (completed) {
                float burst_time = self->exec_proc->burst_time;
                float temps_arrive = self->exec_proc->statistics->temps_arrive;
                
                // Validate burst_time
                if (burst_time <= 0 || burst_time > 10000.0f) {
                    fprintf(stderr, "ERROR: Invalid burst_time %.2f for PID %d in PPP\n", burst_time, current_pid);
                    burst_time = self->exec_proc->remaining_time + self->exec_proc->cpu_time_used;
                    if (burst_time <= 0 || burst_time > 10000.0f) {
                        fprintf(stderr, "ERROR: Cannot recover valid burst_time\n");
                        burst_time = 0.0f;
                    }
                }
                
                // Calculate wait time manually
                turnaround = temps - temps_arrive;
                temps_attente = turnaround - burst_time;
                
                // Validate waiting time
                if (temps_attente < -0.01f || temps_attente > 100000.0f) {
                    fprintf(stderr, "WARNING: Suspicious waiting time %.2f for PID %d\n", temps_attente, current_pid);
                    temps_attente = turnaround - burst_time;
                    if (temps_attente < 0.0f) temps_attente = 0.0f;
                }
            }
            
            // Update process. Only pass &temps if completed.
            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        completed ? &temps : NULL, &run_time);
            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            // Update statistics
            if (self->update_schedular_statistics(self, &run_time, 
                completed ? &turnaround : NULL,
                completed ? &temps_attente : NULL,
                completed) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            if (completed) {
                printf("PCB %d (priority: %d) completed at time %f\n", 
                       current_pid, current_priority, temps);
                
                // Update previous_pid_ppp for next iteration (when we get the next process, it will count as context switch)
                previous_pid_ppp = current_pid;
                // Process is removed from ready queue by update_process
                self->exec_proc = NULL;  // Clear current execution pointer
            } else {
                // Process not completed - need to re-sort by priority
                printf("PCB %d (priority: %d) preempted, remaining time: %f\n", 
                       current_pid, current_priority, new_remaining);
                
                // Update remaining time
                self->exec_proc->remaining_time = new_remaining;
                
                // Re-sort by priority (preemptive behavior)
                if (self->ask_sort_priority(self) == TASK_ERR) {
                    fprintf(stderr, "ERROR: ask_sort_priority failed after partial execution\n");
                    return UPDATE_ERROR;
                }
                
                // Check if we should preempt (another process now has higher priority)
                PCB* new_highest = self->sched_get_ready_queue_head(self);
                if (new_highest != NULL && new_highest->pid != current_pid) {
                    printf("Preempting PCB %d (priority=%d) for PCB %d (priority=%d)\n",
                           current_pid, current_priority,
                           new_highest->pid, new_highest->prioritie);
                    // Preempt: update previous_pid so next iteration counts context switch
                    previous_pid_ppp = current_pid;
                    // Will switch to new process in next iteration (context switch counted there)
                    self->exec_proc = NULL;  // Will pick new process next iteration
                } else {
                    // No preemption, continue with same process
                    previous_pid_ppp = current_pid;
                }
            }
            
            if (completed) {
                log_execution_end(&self->current_segment, temps, "completed");
            } else {
                log_execution_end(&self->current_segment, temps, "preempted");
            }
            
        } else {
            // No ready process, advance to next integer time
            int next_int_time = (int)temps + 1;
            if (next_int_time <= max_arrival_time) {
                temps = (float)next_int_time;
                printf("No ready processes, advancing to time %f\n", temps);
            } else {
                break;  // No more arrivals expected
            }
        }
        
    } while (1);
    
    printf("\nPPP scheduler finished at time %f\n", temps);
    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 3);
    return WORK_DONE;
}

/*************  ✨ Windsurf Command ⭐  *************/
/*******  945a767f-e2ca-4142-9dd1-3e057f79a7c5  *******/
WORK_RETURN select_fcfs(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    
    if (quantum == -1)
        quantum = 1.0f;
    
    float runed = 0;
    int last_checked_time = 0;
    
    printf("Starting FCFS scheduler\n");
    
    // Initial update at time 0 to load all processes that arrive at time 0
    printf("\n=== Time 0: Initial check for arrivals ===\n");
    if (self->sched_update_process_manager(self, 0.0f, &runed) == TASK_ERR) {
        fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
        return UPDATE_ERROR;
    }
    
    if (self->update_ready_queue(self, false) == TASK_ERR) {
        fprintf(stderr, "ERROR: update_ready_queue failed\n");
        return UPDATE_ERROR;
    }

    int previous_pid_fcfs = -1;  // Track previous process for context switch counting
    
    while (1) {
        // Get the first process in ready queue (FIFO)
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
        // Count context switch if switching to a different process
        // Note: First process (previous_pid_fcfs == -1) does NOT count as a context switch
        if (self->exec_proc != NULL) {
            if (previous_pid_fcfs != -1 && self->exec_proc->pid != previous_pid_fcfs) {
                self->statistics->context_switch++;
                fprintf(stderr, "DEBUG: FCFS context switch from PID %d to PID %d\n", previous_pid_fcfs, self->exec_proc->pid);
            }
            // Update previous_pid for next comparison (set initial on first process, update on subsequent)
            previous_pid_fcfs = self->exec_proc->pid;
        }
        
        if (self->exec_proc != NULL) {
            // For FCFS, if current time is before process arrival, advance time
            if (temps < self->exec_proc->statistics->temps_arrive) {
                float old_temps = temps;
                temps = self->exec_proc->statistics->temps_arrive;
                printf("Advanced time from %f to %f for PCB %d arrival\n", 
                       old_temps, temps, self->exec_proc->pid);
                       
                // Check for any arrivals between old_temps and temps
                int check_time = ((int)old_temps) + 1;
                while (check_time <= (int)temps && check_time <= max_arrival_time) {
                    printf("\n=== Time %d: Checking for new arrivals ===\n", check_time);
                    
                    if (self->sched_update_process_manager(self, (float)check_time, &runed) == TASK_ERR) {
                        fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                        return UPDATE_ERROR;
                    }
                    
                    if (self->update_ready_queue(self, false) == TASK_ERR) {
                        fprintf(stderr, "ERROR: update_ready_queue failed\n");
                        return UPDATE_ERROR;
                    }
                    
                    check_time++;
                }
                continue; // Re-get head after potential new arrivals
            }
            
            printf("\nRunning PCB %d at time %f (burst: %f, arrival: %f)\n",
                   self->exec_proc->pid, temps, 
                   self->exec_proc->burst_time,
                   self->exec_proc->statistics->temps_arrive);
            
            // Check resources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    printf("PCB %d blocked, moving to next process\n", self->exec_proc->pid);
                    if (self->remove_from_ready_queue(self, self->exec_proc, temps) == TASK_ERR) {
                        fprintf(stderr, "ERROR: remove_from_ready_queue failed\n");
                        return UPDATE_ERROR;
                    }
                    self->exec_proc = NULL;
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
                default:
                    return WORK_ERROR;
            }
            
            // Execute process to completion
            float run_time = self->exec_proc->burst_time;
            printf("Executing for %f time units (full burst)\n", run_time);

            log_execution_start(&self->execution_segments_head, &self->current_segment, self->exec_proc->pid, temps);

            if (self->execution_queue->execute_fcfs(run_time) != WORK_DONE) { 
                printf("ERROR: execute_fcfs failed!\n");
                return WORK_ERROR;
            }

            runed = run_time;
            temps += run_time;
            
            log_execution_end(&self->current_segment, temps, "completed");
            
            int current_pid = self->exec_proc->pid;
            printf("DEBUG: Process %d completed at simulation time %f\n", current_pid, temps);

            // Calculate stats before updating (which frees PCB)
            float burst = self->exec_proc->burst_time;
            float arrival = self->exec_proc->statistics->temps_arrive;
            
            // Validate burst_time
            if (burst <= 0 || burst > 10000.0f) {
                fprintf(stderr, "ERROR: Invalid burst_time %.2f for PID %d in FCFS\n", burst, current_pid);
                // Try to recover from remaining_time + cpu_time_used
                burst = self->exec_proc->remaining_time + self->exec_proc->cpu_time_used;
                if (burst <= 0 || burst > 10000.0f) {
                    fprintf(stderr, "ERROR: Cannot recover valid burst_time, using 0\n");
                    burst = 0.0f;
                }
            }
            
            float turnaround = temps - arrival;
            float waiting = turnaround - burst;
            
            // Validate waiting time
            if (waiting < -0.01f || waiting > 100000.0f) {
                fprintf(stderr, "WARNING: Suspicious waiting time %.2f for PID %d (turnaround=%.2f, burst=%.2f)\n",
                       waiting, current_pid, turnaround, burst);
                waiting = turnaround - burst;
                if (waiting < 0.0f) waiting = 0.0f;
            }

            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        &temps, &run_time);
            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            if (self->update_schedular_statistics(self, &run_time, 
                &turnaround,
                &waiting,
                true) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            printf("PCB %d completed at time %f\n", current_pid, temps);
            // Update previous_pid for next iteration (when we get the next process, it will count as context switch)
            previous_pid_fcfs = current_pid;
            self->exec_proc = NULL;
            
            // Check for new arrivals after completion
            int current_check = last_checked_time + 1;
            while (current_check <= (int)temps && current_check <= max_arrival_time) {
                printf("\n=== Time %d: Checking for arrivals after completion ===\n", current_check);
                
                if (self->sched_update_process_manager(self, (float)current_check, &runed) == TASK_ERR) {
                    fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                    return UPDATE_ERROR;
                }
                
                if (self->update_ready_queue(self, false) == TASK_ERR) {
                    fprintf(stderr, "ERROR: update_ready_queue failed\n");
                    return UPDATE_ERROR;
                }
                
                last_checked_time = current_check;
                current_check++;
            }
            
        } else {
            // No ready process - check if more arrivals expected
            if (temps < max_arrival_time) {
                int next_time = (int)temps + 1;
                temps = (float)next_time;
                printf("No ready processes, advancing to time %f\n", temps);
                
                if (self->sched_update_process_manager(self, temps, &runed) == TASK_ERR) {
                    fprintf(stderr, "ERROR: sched_update_process_manager failed\n");
                    return UPDATE_ERROR;
                }
                
                if (self->update_ready_queue(self, false) == TASK_ERR) {
                    fprintf(stderr, "ERROR: update_ready_queue failed\n");
                    return UPDATE_ERROR;
                }
                
                last_checked_time = next_time;
            } else {
                break;  // No more processes and no more arrivals
            }
        }
    }
    
    printf("\nFCFS scheduler finished at time %f\n", temps);
    PERFORMANCE_SUMMARY ps = {0};
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics, 1);
    return WORK_DONE;
}
