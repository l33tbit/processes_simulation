#include "../../lib/structs/schedular.h"



WORK_RETURN select_sjf(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);


    float temps = 0;

    if (quantum == -1)
        quantum = 1.0f;

    float runed = 0;
    int last_checked_time = -1;  // NEW: Track last checked arrival time
    
    printf("Starting SJF scheduler\n");

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
            

            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        &temps, &run_time);

            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            // Update statistics
            if (self->update_schedular_statistics(self, &run_time, 
                &self->exec_proc->burst_time,
                &self->exec_proc->statistics->temps_attente,
                true) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            printf("PCB %d completed at time %f\n", current_pid, temps);
            
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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}


WORK_RETURN select_ppn(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    int last_checked_time = -1;
    
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
            
            // Update process (mark as completed)
            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, &temps, &run_time);
            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            printf("PCB %d (priority %d) completed at time %f\n", 
                   current_pid, current_priority, temps);
            
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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}



WORK_RETURN select_rr(ORDONNANCEUR* self, float quantum) {

    float max_arrival_time = self->get_max_arrival_time(self);

    float temps = 0;
    float proc_temps = 0;

    printf("hiiiiiit select_rr\n\n\n");

    do {

        printf("\n----temps %f\n", temps);
        printf("----temps %f\n", proc_temps);
    
        self->exec_proc = self->sched_ask_for_next_ready_element(self, self->exec_proc); // get the next element

        print_pcb(self->exec_proc);
        fflush(stdout);

        if (self->exec_proc  != NULL) {

            // check ressources
            switch (self->check_ressources(self, self->exec_proc)) {
                case PROCESS_BLOCKED:
                    continue;
                case PROCESS_ERROR:
                    return WORK_ERROR;
                case RESSOURCES_AVAILABLE:
                    break;
            }

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

                // Pass n_quantum instead of quantum
                PROCESS_UPDATE update = self->update_process(self, self->exec_proc, &temps, &n_quantum);
                
                if (update != UPDATED) {
                    return WORK_ERROR;
                }

                if (
                    self->update_schedular_statistics(self, &n_quantum, &self->exec_proc->burst_time, &self->exec_proc->statistics->temps_attente, true) != TASK_SUCC // changed not good
                ) {
                    return WORK_ERROR;
                }
                
                log_execution_end(&self->current_segment, temps, "completed");
                
            } else {

                temps += quantum;

                time_used = quantum; // se the full quantum
                
                PROCESS_UPDATE update = self->update_process(self, self->exec_proc, temps_fin_ptr, &quantum);

                if (update != UPDATED) {
                    return WORK_ERROR;
                }

                if (
                    self->update_schedular_statistics(self, &quantum, NULL, NULL, false) != WORK_DONE // changed not good
                ) {
                    return WORK_ERROR;
                }

                log_execution_end(&self->current_segment, temps, "preempted");

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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}


WORK_RETURN select_srtf(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    float proc_temps = 0;  // Last integer time we processed

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
            float burst_time = 0;
            float temps_attente = 0;
            if (completed) {
                burst_time = self->exec_proc->burst_time;
                // Calculate temps_attente manually as we can't access PCB after free
                float temps_arrive = self->exec_proc->statistics->temps_arrive;
                temps_attente = (temps - temps_arrive) - burst_time;
            }


            
            // Update process. Only pass &daba (temps_fin) if completed.
            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        completed ? &temps : NULL, &run_time);
            if (update != UPDATED) {
                return UPDATE_ERROR;
            }
            
            // Update statistics
            if (self->update_schedular_statistics(self, &run_time, 
                completed ? &burst_time : NULL,
                completed ? &temps_attente : NULL,
                completed) != TASK_SUCC) {
                return UPDATE_ERROR;
            }
            
            if (completed) {
                printf("PCB %d completed at time %f\n", current_pid, temps);
                // When process completes, it's removed from ready queue
                // We need to re-sort for the next iteration
                self->exec_proc = NULL;  // Clear current execution pointer
            } else {
                // Process not completed - remaining_time changed, need to re-sort

                if (self->ask_sort_rt(self) == TASK_ERR) {
                    fprintf(stderr, "ERROR: ask_sort_rt failed after partial execution\n");
                    return UPDATE_ERROR;
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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}

WORK_RETURN select_ppp(ORDONNANCEUR* self, float quantum) {
    float max_arrival_time = self->get_max_arrival_time(self);
    float temps = 0;
    float proc_temps = 0;  // Last integer time we processed

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
            float burst_time = 0;
            float temps_attente = 0;
            if (completed) {
                burst_time = self->exec_proc->burst_time;
                // Calculate wait time manually
                float temps_arrive = self->exec_proc->statistics->temps_arrive;
                temps_attente = (temps - temps_arrive) - burst_time;
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
                completed ? &burst_time : NULL,
                completed ? &temps_attente : NULL,
                completed) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            if (completed) {
                printf("PCB %d (priority: %d) completed at time %f\n", 
                       current_pid, current_priority, temps);
                
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
                    self->exec_proc = NULL;  // Will pick new process next iteration
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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}

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

    while (1) {
        // Get the first process in ready queue (FIFO)
        self->exec_proc = self->sched_get_ready_queue_head(self);
        
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

            PROCESS_UPDATE update = self->update_process(self, self->exec_proc, 
                                                        &temps, &run_time);
            if (update != UPDATED) {
                fprintf(stderr, "ERROR: update_process failed for PCB %d\n", current_pid);
                return UPDATE_ERROR;
            }
            
            if (self->update_schedular_statistics(self, &run_time, 
                &self->exec_proc->burst_time,
                &self->exec_proc->statistics->temps_attente,
                true) != TASK_SUCC) {
                fprintf(stderr, "ERROR: update_schedular_statistics failed\n");
                return UPDATE_ERROR;
            }
            
            printf("PCB %d completed at time %f\n", current_pid, temps);
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
    print_algorithm_output(self->execution_segments_head, &ps, self->simulator->process_manager->process_table_head, temps, self->statistics);
    return WORK_DONE;
}
