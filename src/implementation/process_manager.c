#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../../lib/structs/process_manager.h"
#include "../../lib/structs/process.h"
#include "../../lib/structs/simulator.h"

#include "../../src/implementation/helpers/process_manager.c"
#include "../../src/implementation/process.c"


// initialization
PCB* op_create_process_table(PROCESS_MANAGER* self) {
    // read_buffer and get elements list and count
    // create process table
    // iterate and add to process table

    PCB* pcb_head = extract_from_buffer(self); // the function create the chaine then return the head

    if (pcb_head == NULL) {
        fprintf(stderr, "ERROR ON: extract_from_buffer failed has returned NULL in allocating pcb_head\n");
        exit(1);
    }

    return pcb_head; 

}


PCB* op_get_next_process_table(PROCESS_MANAGER* self, PCB* current_pcb) {
    if (current_pcb == NULL) {
        return NULL;
    }
    
    // return the next pcb, because the current_pcb is a process table element
    return current_pcb->pid_sibling_next;
}

PCB* op_insert_after_ready(PROCESS_MANAGER* self, PCB* after_pcb, PCB* pcb_to_insert) { // if return the ready queue head then it's inserted if NULL then it's not inserted

    if (pcb_to_insert == NULL) {  // nothing to insert
        fprintf(stderr, "ERROR ON: op_push_after pcb_to_insert is null\n");
        
        
        return self->ready_queue_head;  // return the head
    }
    
    if (after_pcb == NULL) {
        // inserting in the begining
        pcb_to_insert->pid_sibling_next = self->ready_queue_head;
        self->ready_queue_head = pcb_to_insert;

        return self->ready_queue_head;
    }
    
    // inserting after after_pcb
    PCB* current = self->ready_queue_head;
    
    while (current != NULL) {
        if (current == after_pcb) { // found the pcb that need to be insert after it
            pcb_to_insert->pid_sibling_next = current->pid_sibling_next;
            current->pid_sibling_next = pcb_to_insert;
            return self->ready_queue_head;
        }
        current = current->pid_sibling_next;
    }
    
    
    fprintf(stderr, "ERROR ON: op_push_after after_pcb not found in ready queue\n"); // pcb not found
    
    // inserting in the end
    if (self->ready_queue_head == NULL) {
        self->ready_queue_head = pcb_to_insert;
        pcb_to_insert->pid_sibling_next = NULL;

    } else {
        PCB* last = self->ready_queue_head;
        
        while (last->pid_sibling_next != NULL) {
            last = last->pid_sibling_next;
        
        }
        last->pid_sibling_next = pcb_to_insert;
        
        pcb_to_insert->pid_sibling_next = NULL;
    }
    
    return self->ready_queue_head;
}

void op_free_ready_queue(PCB* head) {
    PCB* current = head;
    PCB* next;
    
    while (current != NULL) {
        next = current->pid_sibling_next;
        
        // free pcb statistics
        if (current->statistics != NULL) {
            free(current->statistics);
        }
        
        // free the pcb
        free(current);
        current = next;
    }
}


PCB* op_create_ready_queue(PROCESS_MANAGER* self, bool circular) {  // we dont pass the algo because it initialized before the schedular

    PCB* process_table_node = self->process_table_head;
    PCB* ready_queue_head = NULL;
    PCB* last_element = NULL;

    while (process_table_node != NULL) { //starting from the head obviously
        
        if (process_table_node->statistics->temps_arrive == self->temps) {
            
            PCB* new_pcb = (PCB*)malloc(sizeof(PCB));

            if (new_pcb == NULL) { // checking if allocation failed
                fprintf(stderr, "ERROR ON: op_create_ready_queue allocation failed\n");

                // if failed free then exit
                self->free_ready_queue(ready_queue_head);
                exit(1);
            }
            
            // copying field per field
            new_pcb->pid = process_table_node->pid;
            strncpy(new_pcb->process_name, process_table_node->process_name, sizeof(new_pcb->process_name)); // strncpy copy byte per byte so char by char to make sure
            strncpy(new_pcb->user_id, process_table_node->user_id, sizeof(new_pcb->user_id)); // strncpy copy byte per byte so char by char to make sure

            new_pcb->ppid = process_table_node->ppid;
            new_pcb->etat = process_table_node->etat;
            new_pcb->prioritie = process_table_node->prioritie;
            new_pcb->programme_compteur = process_table_node->programme_compteur;
            new_pcb->memoire_necessaire = process_table_node->memoire_necessaire;
            new_pcb->burst_time = process_table_node->burst_time;
            new_pcb->cpu_time_used = process_table_node->cpu_time_used;
            new_pcb->remaining_time = process_table_node->remaining_time;
            
            // allocating the pcb statistics
            new_pcb->statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS));
            if (new_pcb->statistics == NULL) {
                fprintf(stderr, "ERROR ON: statistics allocation failed\n");
                free(new_pcb);
                self->free_ready_queue(ready_queue_head);
                exit(1);
            }
            memcpy(new_pcb->statistics, process_table_node->statistics, sizeof(PROCESS_STATISTICS)); // this also copy the memory data with fixed size to make sure and because it made me tired of bugs
            
            // copy the head of instruction then copy field per field using this because of bugs occured
            INSTRUCTION* src_instr = process_table_node->instructions_head;

            INSTRUCTION* n_head = NULL;
            INSTRUCTION* n_tail = NULL;

            while (src_instr != NULL) {
            
                INSTRUCTION* new_instr = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
            
                memcpy(new_instr, src_instr, sizeof(INSTRUCTION));
            
                new_instr->process = new_pcb; // Update owner
                new_instr->next = NULL;

                if (n_head == NULL) { // s it's the first instruct
                    n_head = new_instr;  
                } 
                else // need to be chained in the end
                    n_tail->next = new_instr;
                
                n_tail = new_instr;
                src_instr = src_instr->next;
            }

            new_pcb->instructions_head = n_head; // to make sure because of bugs
            new_pcb->current_instruction = n_head; // same
            

            // init the next as NULL
            new_pcb->pid_sibling_next = NULL;
            
            // push the pcb to ready queue
            if (ready_queue_head == NULL) {
                ready_queue_head = new_pcb;
                last_element = new_pcb;
            } else {
                last_element->pid_sibling_next = new_pcb;
                last_element = new_pcb;
            }
            
            process_table_node->etat = READY_QUEUE; // seting state as ready because it moved from process table to ready and for interuptions it check this
        }
        

        process_table_node = process_table_node->pid_sibling_next;
    }
    
    // for circular queue
    if (circular && ready_queue_head != NULL && last_element != NULL) {
        
        last_element->pid_sibling_next = ready_queue_head;
    }
    
    self->ready_queue_head = ready_queue_head;
    return ready_queue_head;
}


PCB* op_sort_ready_by_fc(PROCESS_MANAGER* process_manager, bool circular) {

    PCB* ready_queue_head = process_manager->ready_queue_head;

    if (ready_queue_head == NULL) {
        fprintf(stderr, "ERROR ON: op_sort_ready_by_fc , ready_queue_head is NULL\n");
        exit(1);
    }
    PCB* sorted_head = NULL;
    PCB* current = ready_queue_head; // node that will contain the proces arrived before current

    // find the process with the first arrival tile
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
    
        // insert the current into the sorted
        if (sorted_head == NULL || current->statistics->temps_arrive < sorted_head->statistics->temps_arrive) {
            // insert in the start
            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {
            // find the pointer where it inserted
            PCB* search = sorted_head;
            while (search->pid_sibling_next != NULL && // if the search has reach the end
                    search->pid_sibling_next->statistics->temps_arrive < current->statistics->temps_arrive) { // or if the searched value's temps arrive is < that current 
                search = search->pid_sibling_next;
            }

            current->pid_sibling_next = search->pid_sibling_next; // make the current'e next is searched's ext
            search->pid_sibling_next = current; // and the searched next to current
        }
        current = next;
    }

    return sorted_head;
}

PCB* op_sort_ready_by_priority(PROCESS_MANAGER* self) {
    PCB* ready_queue_head = self->ready_queue_head;

    if (ready_queue_head == NULL) {
        // if null then ready queue is empty so schedular finished
        return NULL;
    }
    
    // check if ready is circular to make the break 3dbatni :>
    PCB* end = ready_queue_head;

    bool circular = false;
    
    // check circular and find end
    while (end->pid_sibling_next != NULL && 
           end->pid_sibling_next != ready_queue_head
           ) {
            
        end = end->pid_sibling_next;
    }
    
        
    if (end->pid_sibling_next == ready_queue_head) { // it's circular so break
        circular = true;
        end->pid_sibling_next = NULL;
        printf("DEBUG: circular lise, breaking circle\n");
    }
    
    // sorting
    PCB* sorted_head = NULL;
    PCB* current = ready_queue_head;
    
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
    
        // insert to sorted list
        if (sorted_head == NULL || current->prioritie < sorted_head->prioritie) {

            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {

            PCB* search = sorted_head;
            
            while (search->pid_sibling_next != NULL &&
            
                search->pid_sibling_next->prioritie < current->prioritie) {
                search = search->pid_sibling_next;
            }
            current->pid_sibling_next = search->pid_sibling_next;
            search->pid_sibling_next = current;
        }
        current = next;
    }
    
    
    if (circular && sorted_head != NULL) { 
    
        PCB* new_tail = sorted_head; // remake the list circular
        while (new_tail->pid_sibling_next != NULL) { // find the last element
            new_tail = new_tail->pid_sibling_next;

        }
        new_tail->pid_sibling_next = sorted_head; // then push it
    }
    
    return sorted_head;
}

PCB* op_sort_ready_by_rt(PROCESS_MANAGER* self) {
    PCB* ready_queue_head = self->ready_queue_head;

    if (ready_queue_head == NULL) {
        // if null then ready queue is empty so schedular finished
        return NULL;
    }
    
    // check if ready is circular to make the break 3dbatni :>
    PCB* end = ready_queue_head;

    bool circular = false;
    
    // check circular and find end
    while (end->pid_sibling_next != NULL && 
           end->pid_sibling_next != ready_queue_head
           ) {
            
        end = end->pid_sibling_next;
    }
    
        
    if (end->pid_sibling_next == ready_queue_head) { // it's circular so break
        circular = true;
        end->pid_sibling_next = NULL;
        printf("DEBUG: circular lise, breaking circle\n");
    }
    
    // sorting
    PCB* sorted_head = NULL;
    PCB* current = ready_queue_head;
    
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
    
        // insert to sorted list
        if (sorted_head == NULL || current->remaining_time < sorted_head->remaining_time) {

            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {

            PCB* search = sorted_head;
            
            while (search->pid_sibling_next != NULL &&
            
                search->pid_sibling_next->prioritie < current->prioritie) {
                search = search->pid_sibling_next;
            }
            current->pid_sibling_next = search->pid_sibling_next;
            search->pid_sibling_next = current;
        }
        current = next;
    }
    
    
    if (circular && sorted_head != NULL) { 
    
        PCB* new_tail = sorted_head; // remake the list circular
        while (new_tail->pid_sibling_next != NULL) { // find the last element
            new_tail = new_tail->pid_sibling_next;

        }
        new_tail->pid_sibling_next = sorted_head; // then push it
    }
    
    return sorted_head;
}


// PCB* op_sort_ready_by_rt(PROCESS_MANAGER* self) {
//     PCB* ready_queue_head = self->ready_queue_head;

//     if (ready_queue_head == NULL) {
//         // Empty queue is not an error
//         return NULL;
//     }
    
//     // First, check if the list is circular and break the circle if needed
//     PCB* tail = ready_queue_head;
//     int node_count = 0;
//     bool is_circular = false;
    
//     // Find tail and check for circularity
//     while (tail->pid_sibling_next != NULL && 
//            tail->pid_sibling_next != ready_queue_head && 
//            node_count < 100) {  // Safety limit
//         tail = tail->pid_sibling_next;
//         node_count++;
//     }
    
//     if (tail->pid_sibling_next == ready_queue_head) {
//         // List is circular - break the circle
//         is_circular = true;
//         tail->pid_sibling_next = NULL;
//         printf("DEBUG: Detected circular list, breaking circle\n");
//     }
    
//     // Now sort the list (now it's non-circular)
//     PCB* sorted_head = NULL;
//     PCB* current = ready_queue_head;
//     node_count = 0;
    
//     while (current != NULL && node_count < 100) {  // Safety limit
//         node_count++;
//         PCB* next = current->pid_sibling_next;
    
//         // Insert current into sorted list
//         if (sorted_head == NULL || current->remaining_time < sorted_head->remaining_time) {
//             current->pid_sibling_next = sorted_head;
//             sorted_head = current;
//         } else {
//             PCB* search = sorted_head;
//             while (search->pid_sibling_next != NULL &&
//                    search->pid_sibling_next->remaining_time < current->remaining_time) {
//                 search = search->pid_sibling_next;
//             }
//             current->pid_sibling_next = search->pid_sibling_next;
//             search->pid_sibling_next = current;
//         }
//         current = next;
//     }
    
//     if (node_count >= 100) {
//         fprintf(stderr, "ERROR: Sort function hit safety limit - possible infinite loop\n");
//         return NULL;
//     }
    
//     printf("Sort completed, sorted %d nodes\n", node_count);
    
//     // If the list was originally circular, make the sorted list circular
//     if (is_circular && sorted_head != NULL) {
//         PCB* new_tail = sorted_head;
//         while (new_tail->pid_sibling_next != NULL) {
//             new_tail = new_tail->pid_sibling_next;
//         }
//         new_tail->pid_sibling_next = sorted_head;
//         printf("DEBUG: Restored circularity after sorting\n");
//     }
    
//     return sorted_head;
// }

PCB* op_sort_ready_by_sjf(PROCESS_MANAGER* self) {
    PCB* ready_queue_head = self->ready_queue_head;
    
    if (ready_queue_head == NULL) {
        printf("SJF sort: Ready queue is empty\n");
        return NULL;
    }
    
    printf("SJF sort: start ready queue head is :%d\n", ready_queue_head->pid);
    
    // Check for cycles in the list
    PCB* slow = ready_queue_head;
    PCB* fast = ready_queue_head;
    int cycle_detected = 0;
    int cycle_count = 0;
    
    while (fast != NULL && fast->pid_sibling_next != NULL && cycle_count < 1000) {
        slow = slow->pid_sibling_next;
        fast = fast->pid_sibling_next->pid_sibling_next;
        cycle_count++;
        
        if (slow == fast) {
            printf("ERROR: Cycle detected in ready queue at iteration %d!\n", cycle_count);
            cycle_detected = 1;
            break;
        }
    }
    
    if (cycle_detected) {
        printf("Breaking cycle by setting last node's next to NULL\n");
        // Find the node before the cycle
        PCB* prev = ready_queue_head;
        while (prev->pid_sibling_next != slow) {
            prev = prev->pid_sibling_next;
        }
        prev->pid_sibling_next = NULL;
    }
    
    // remove the terminated process added to debug
    PCB* head = NULL;
    PCB* end = NULL;
    PCB* current = ready_queue_head;
    int active_count = 0;
    int traversal_count = 0;
    
    while (current != NULL && traversal_count < 1000) {
        traversal_count++;
        PCB* next = current->pid_sibling_next;
        
        if (active_count < 5) {
            printf("SJF sort: Node %d - PID %d, burst=%.2f, state=%d\n", 
                   active_count, current->pid, current->burst_time, current->etat);
        }
        
        // Only include non-terminated processes
        if (current->etat != TERMINATED) {
            // Detach current node from original list
            current->pid_sibling_next = NULL;
            
            if (head == NULL) {
                head = current;
                end = current;
            } else {
                end->pid_sibling_next = current;
                end = current;
            }
            active_count++;
        }
        current = next;
    }
    
    if (traversal_count >= 1000) {
        printf("ERROR: Traversal hit limit, possible cycle not detected\n");
    }
    
    printf("SJF sort: Found %d active processes\n", active_count);
    
    if (head == NULL) {
        printf("SJF sort: No active processes\n");
        return NULL;
    }
    
    
    int chang;
    int sort_iterations = 0;
    do { // very simple sort 
        chang = 0;
        PCB** ptr = &head;
        int inner_count = 0;
        
        while (*ptr != NULL && (*ptr)->pid_sibling_next != NULL && inner_count < 1000) {
            inner_count++;
            PCB* current_node = *ptr;
            PCB* next_node = current_node->pid_sibling_next;
            
            if (current_node->burst_time > next_node->burst_time) {
                // Swap nodes
                current_node->pid_sibling_next = next_node->pid_sibling_next;
                next_node->pid_sibling_next = current_node;
                *ptr = next_node;
                chang = 1;
            }
            ptr = &(*ptr)->pid_sibling_next;
        }
        sort_iterations++;
        if (sort_iterations > 1000) {
            printf("ERROR: Sort iterations exceeded limit\n");
            break;
        }
    } while (chang && sort_iterations < 1000);
    
    // Debug: Print sorted order  need_to_be_removed
    printf("SJF sort: Sorted order (first 5): ");
    current = head;
    for (int i = 0; i < 5 && current != NULL; i++) {
        printf("%d(%.2f) ", current->pid, current->burst_time);
        current = current->pid_sibling_next;
    }
    printf("\n");
    
    printf("SJF sort: Completed sorting %d processes\n", active_count);
    
    return head;
}



float op_find_max_arrival_time(PROCESS_MANAGER* self) {
    float max_arrival = 0.0f;
    PCB* current = self->process_table_head;
    
    while (current != NULL) {
        if (current->statistics != NULL && 
            current->statistics->temps_arrive > max_arrival) {
            max_arrival = current->statistics->temps_arrive;
        }
        current = current->pid_sibling_next;
    }
    return max_arrival;
}


TASK op_update_self_temps(PROCESS_MANAGER* self, float temps, float* runed) {

    self->temps = temps;
    if (runed)
        self->last_runed = *runed;
    return TASK_SUCC;

}

PCB* op_sort_ready_by_burst(PROCESS_MANAGER* process_manager) {

    PCB* ready_queue_head = process_manager->ready_queue_head;

    if (ready_queue_head == NULL) {
        fprintf(stderr, "ERROR ON: op_sort_ready_by_priority , ready_queue_head is NULL\n");
        exit(1);
    }

    PCB* sorted_head = NULL;
    PCB* current = ready_queue_head; // node that will contain the proces arrived before current

    // find the process with the first arrival tile
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
    
        // insert the current into the sorted
        if (sorted_head == NULL || current->burst_time < sorted_head->burst_time) {
            // insert in the start
            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {
            // find the pointer where it inserted
            PCB* search = sorted_head;
            while (search->pid_sibling_next != NULL && // if the search has reach the end
                    search->pid_sibling_next->burst_time < current->burst_time) { // or if the searched value's temps arrive is < that current 
                search = search->pid_sibling_next;
            }

            current->pid_sibling_next = search->pid_sibling_next; // make the current'e next is searched's ext
            search->pid_sibling_next = current; // and the searched next to current
        }
        current = next;
    }

    return sorted_head;
}


PCB* op_create_blocked_queue() {
    // i think i just need to init it and when a process is blocked will be chained
    return NULL; // same a declaring pcb* pcb = null and return it
}

//--------------last one
TASK op_update_read_queue(PROCESS_MANAGER* self, bool circular) {
    
    PCB* current = self->process_table_head;
    int inserted = 0;
                   
    while (current != NULL) {
        
             
        // add if process state is PROCESS_NEW
        if (current->etat == PROCESS_NEW && current->statistics != NULL) {
            if (current->statistics->temps_arrive <= self->temps) {

                // skip if it's not new process
                if (current->etat != PROCESS_NEW) {

                    current = current->pid_sibling_next;
                    continue;
                }
                
                
                PCB* result = self->push_to_ready_queue(self, current, circular);
                if (result != NULL) {
                    current->etat = READY_QUEUE;  // MARK READY BECAUSE PUSHED
                    inserted++;
                }
                printf("ççççççççç");
                fflush(stdout);
            }
        }
        
        current = current->pid_sibling_next;
    }
    
    return TASK_SUCC;
}




//pcb related
// with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time
PROCESS_UPDATE op_pro_update_process(PROCESS_MANAGER* self, PCB* pcb, float *temps_fin, float *cpu_temps_used) {

    printf("DEBUG: op_pro_update_process called with temps_fin %p, cpu_temps_used %p\n", temps_fin, cpu_temps_used);
    fflush(stdout);

    if (pcb == NULL) {
        fprintf(stderr, "ERROR ON: op_update_process , pcb is NULL\n");
        return UPDATE_ERROR;
    }

    if (cpu_temps_used) {

        pcb->cpu_time_used += *cpu_temps_used; // because initialized to 0
        pcb->remaining_time = pcb->burst_time - pcb->cpu_time_used;
        
    }
    
    // updating the given fields
    if (temps_fin) { // updating temp fin = update tournround
        self->ready_queue_head = self->delete_from_ready_queue(self, pcb, *temps_fin); // delete the process from ready queue when terminated and the function return the head of the tready queue so capturing it and assigning it to ready_queue_head
    }

    return UPDATED;
}

PCB* op_push_to_ready_queue(PROCESS_MANAGER* self, PCB* pcb, bool circular) {
    if (pcb == NULL) {
        return self->ready_queue_head;
    }
    
    // Create new PCB (copy)
    PCB* ready_pcb = (PCB*)malloc(sizeof(PCB));
    if (!ready_pcb) {
        printf("ERROR: Failed to allocate PCB copy\n");
        return self->ready_queue_head;
    }
    
    // Copy the PCB (simplified - you may need deep copy)
    memcpy(ready_pcb, pcb, sizeof(PCB));
    
    // Allocate new statistics for the copy
    PROCESS_STATISTICS* new_stats = (PROCESS_STATISTICS*)calloc(1, sizeof(PROCESS_STATISTICS));
    if (new_stats == NULL) {
        free(ready_pcb);
        return self->ready_queue_head;
    }
    new_stats->temps_arrive = ready_pcb->statistics->temps_arrive;
    new_stats->temps_creation = ready_pcb->statistics->temps_creation;
    new_stats->temps_fin = ready_pcb->statistics->temps_fin;
    new_stats->temps_attente = ready_pcb->statistics->temps_attente;
    new_stats->tournround = ready_pcb->statistics->tournround;
    ready_pcb->statistics = new_stats;
    
    // IMPORTANT: Reset the sibling pointer to avoid cycles
    ready_pcb->pid_sibling_next = NULL;
    ready_pcb->etat = READY_QUEUE;
    
    // Insert into ready queue
    if (self->ready_queue_head == NULL) {
        self->ready_queue_head = ready_pcb;
        if (circular) {
            ready_pcb->pid_sibling_next = ready_pcb; // Self-loop for circular
        }
    } else {
        // Find the last node SAFELY
        PCB* last = self->ready_queue_head;
        
        // Use visited tracking to detect cycles
        int visited[1000] = {0}; // Assuming max 1000 processes
        int count = 0;
        
        if (circular) {
            // For circular: find node before head
            while (last->pid_sibling_next != self->ready_queue_head) {
                last = last->pid_sibling_next;
                count++;
                if (count > 1000) {
                    printf("ERROR: Circular list too long or has cycle!\n");
                    break;
                }
            }
            last->pid_sibling_next = ready_pcb;
            ready_pcb->pid_sibling_next = self->ready_queue_head;
        } else {
            // For non-circular: find NULL
            while (last->pid_sibling_next != NULL) {
                // Track visited nodes to detect cycles
                if (count < 1000) {
                    visited[count] = last->pid;
                }
                
                // Check if we're revisiting a node
                for (int i = 0; i < count; i++) {
                    if (visited[i] == last->pid) {
                        printf("ERROR: Cycle detected at PID %d!\n", last->pid);
                        last->pid_sibling_next = NULL; // Break cycle
                        goto found_end;
                    }
                }
                
                last = last->pid_sibling_next;
                count++;
                
                if (count > 1000) {
                    printf("ERROR: List too long, possible infinite loop!\n");
                    last->pid_sibling_next = NULL; // Force end
                    break;
                }
            }
            
        found_end:
            last->pid_sibling_next = ready_pcb;
            ready_pcb->pid_sibling_next = NULL;
        }
    }
    
    printf("Added process %d to ready queue\n", ready_pcb->pid);
    return self->ready_queue_head;
}


// PCB* op_push_to_ready_queue(PROCESS_MANAGER* self, PCB* pcb, bool circular) {

//     if (pcb == NULL) {
//         return self->ready_queue_head;
//     }
    
//     // create the process that will be added
//     PCB* ready_pcb = (PCB*)malloc(sizeof(PCB));
//     if (!ready_pcb) {
//         printf("ERROR: Failed to allocate PCB copy\n");
//         return self->ready_queue_head;
//     }
    
//     // copy field by field
//     memcpy(ready_pcb, pcb, sizeof(PCB));
    
//     // copy the statistics
//     if (pcb->statistics != NULL) {

//         ready_pcb->statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS));

//         if (ready_pcb->statistics) {
//             memcpy(ready_pcb->statistics, pcb->statistics, sizeof(PROCESS_STATISTICS)); // copy the bytes for bugs
//         }
//     }

//     // copy the instruction
//     INSTRUCTION* o_instr = pcb->instructions_head;
//     INSTRUCTION* n_head = NULL;
//     INSTRUCTION* n_tail = NULL;

//     while (o_instr != NULL) {

//         INSTRUCTION* new_instr = (INSTRUCTION*)malloc(sizeof(INSTRUCTION));
//         memcpy(new_instr, o_instr, sizeof(INSTRUCTION));
        
//         new_instr->process = ready_pcb;
//         new_instr->next = NULL;

//         if (n_head == NULL) 
//             n_head = new_instr;
        
//         else 
//             n_tail->next = new_instr;
        
//         n_tail = new_instr;
//         o_instr = o_instr->next;
//     }
//     ready_pcb->instructions_head = n_head;
//     ready_pcb->current_instruction = n_head;

    
//     ready_pcb->pid_sibling_next = NULL; // assign the next as NULL

//     ready_pcb->etat = READY_QUEUE; // set the state
    
//     // insert to ready queue
//     if (self->ready_queue_head == NULL) {
//         self->ready_queue_head = ready_pcb;

//         if (circular) {
//             ready_pcb->pid_sibling_next = ready_pcb;

//         } else {

//             ready_pcb->pid_sibling_next = NULL;
//         }

//     } else {
//         // find the last ready pcb
//         PCB* last = self->ready_queue_head;
        
//         if (circular) {
//             while (last->pid_sibling_next != self->ready_queue_head) {
//                 last = last->pid_sibling_next;

//             }
//             last->pid_sibling_next = ready_pcb;
//             ready_pcb->pid_sibling_next = self->ready_queue_head;
//         } else {
//             while (last->pid_sibling_next != NULL) {
//                 last = last->pid_sibling_next;
//  printf("aaaaaaaaaaaa\n");
//     fflush(stdout);
//             }
//             last->pid_sibling_next = ready_pcb;
//             ready_pcb->pid_sibling_next = NULL;
//         }
           
//     }
    
//     return self->ready_queue_head;
// }

TASK op_mark_process_table_pcb_terminated(PROCESS_MANAGER* self, PCB* pcb, float completion_time) {
    if (self == NULL || pcb == NULL) {
        fprintf(stderr, "ERROR ON: op_mark_process_table_pcb_terminated pcb or self are null\n");
        return TASK_ERR;
    }
    
    PCB* process_table_head = self->process_table_head;
    
    if (process_table_head == NULL) {
        fprintf(stderr, "ERROR ON: op_mark_process_table_pcb_terminated process table is empty\n");
        return TASK_ERR;
    }
    
    
    // finding  the pcb
    PCB* current = process_table_head;
    PCB* head = process_table_head;
    
    do {
        
        if (current->pid == pcb->pid) { // process found
            
            current->etat = TERMINATED; // mark it as terminated
            
            
            if (current->statistics != NULL) { // update termination time and check if the statistixs exist if not so it's a bug found

                // set temps fin as completion time
                current->statistics->temps_fin = completion_time;
                current->statistics->tournround = completion_time - current->statistics->temps_arrive;
                
                // Calculate waiting time: completion_time - arrival_time - burst_time
                float waiting_time = completion_time - 
                                    current->statistics->temps_arrive - 
                                    current->burst_time;
                if (waiting_time < 0) 
                    waiting_time = 0;
                
                    current->statistics->temps_attente = waiting_time;
                
            } else {
                fprintf(stderr, "ERROR ON: op_mark_process_table_pcb_terminated pcb is found but statistics are NULL\n");
                exit(1);
            }
            
            return TASK_SUCC;
        }
        
        current = current->pid_sibling_next;
        
    } while (current != NULL && current != head); // while the next is not null ans it's not a circular queue
    
    return TASK_ERR;
}

PCB* op_delete_from_ready_queue(PROCESS_MANAGER* self, PCB* pcb, float completion_time) {


    PCB* ready_queue_head = self->get_ready_queue_head(self);
    
    // check for NULL and it's not an error
    if (ready_queue_head == NULL) {
        return NULL;
    }
    
    if (pcb == NULL) {
        return ready_queue_head;
    }
    

    // if it's the head that need to be deleted
    if (ready_queue_head == pcb) {

        PCB* new_head = ready_queue_head->pid_sibling_next;
        
        // if it's a circular list and has one element
        if (ready_queue_head->pid_sibling_next == ready_queue_head) {

            new_head = NULL;

        } 
        // circular with multiple elements
        else if (ready_queue_head->pid_sibling_next != NULL) {
            // find tha last and update it's next

            PCB* last = ready_queue_head;
            while (last->pid_sibling_next != ready_queue_head && last->pid_sibling_next != NULL) {
                
                last = last->pid_sibling_next;
            }
            
            if (last->pid_sibling_next == ready_queue_head) {
                last->pid_sibling_next = new_head;

            }
        }
        

        if (self->mark_process_table_pcb_terminated(self, pcb, completion_time) == TASK_ERR) {
            fprintf(stderr, "ERROR ON: op_delete_from_ready_queue , mark_process_table_pcb_terminated has returned false\n");
            exit(1);
        }
        
        op_free_pcb_pcb(ready_queue_head);
        
        return new_head;
    }

    // deleting an internal pcb
    PCB* prev = ready_queue_head;
    PCB* current = ready_queue_head->pid_sibling_next;
    int found = 0;
    

    while (current != NULL && current != ready_queue_head) {  // iterating over the <ueue
        
        if (current == pcb) { // found
            found = 1;
            
            prev->pid_sibling_next = current->pid_sibling_next;
            

            if (self->mark_process_table_pcb_terminated(self, current, completion_time) == TASK_ERR) { // if not removed
                fprintf(stderr, "ERROR ON: op_delete_from_ready_queue , mark_process_table_pcb_terminated has returned false\n");
                exit(1);
            }
            op_free_pcb_pcb(current);
            
            break;
        }
        
        PCB* next_node = current->pid_sibling_next;
        prev = current;
        current = next_node;
        
    }
    
    if (!found) {
        printf("!!!! PCB %d wasnot found in ready queue\n", pcb->pid); // signal that it's not found
        return ready_queue_head;
    }
    
    return ready_queue_head;
}

PCB* op_get_ready_queue_head(PROCESS_MANAGER* self) {

    if (self->ready_queue_head == NULL) {
        fprintf(stderr, "ERROR ON: op_get_ready_queue_head, head is null");
    }

    return self->ready_queue_head;
}


// bloqued queue related
PCB* op_add_process_to_blocked_queue(PROCESS_MANAGER* process_manager, PCB* pcb) { // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it

    PCB* blocked_queue_head = process_manager->blocked_queue_head;

    pcb->etat = BLOCKED;  // mark it as blocked
    pcb->pid_sibling_next = NULL; // clearing the pointer tpo next

    if (blocked_queue_head == NULL) { // if there is no process in the blocked the pcb will be the head
        process_manager->blocked_queue_head = pcb;  // so updating the head setting it as the pcb giving in the arguments
        return pcb;
    }

    PCB* iter = blocked_queue_head;

    while (iter->pid_sibling_next != NULL) {
        iter = iter->pid_sibling_next;
    }

    // ajouter pcb in the end
    iter->pid_sibling_next = pcb;

    // the head is the same so returning it
    return blocked_queue_head;
}


PCB* op_delete_from_blocked_queue(PROCESS_MANAGER* self, PCB* pcb) {

    PCB* blocked_queue_head = self->blocked_queue_head;

    if (blocked_queue_head == NULL) {
        return NULL;
    }

    if (pcb == NULL) {
        fprintf(stderr, "ERROR ON: op_delete_from_blocked_queue pcb passed is null");
        exit(1);    
    }

    if (blocked_queue_head == pcb) {
        return NULL;
    }

    PCB* prev = blocked_queue_head;
    PCB* current = blocked_queue_head->pid_sibling_next;

    while (current != NULL) {
        if (current == pcb) {
            prev->pid_sibling_next = current->pid_sibling_next;
            free(current);
            break;
        }
        prev = current;
        current = current->pid_sibling_next;
    }

    return blocked_queue_head;
}

PCB* op_get_blocked_queue_element(PROCESS_MANAGER* self, PCB* pcb) {

    PCB* blocked_queue_head = self->blocked_queue_head;

    if (blocked_queue_head == NULL || pcb == NULL) {
        return blocked_queue_head;
    }

    PCB* iter = blocked_queue_head;

    // if element is in the start
    if (iter == pcb) {
        return iter;
    }

    // seach and return
    while (iter != NULL) {
        if (iter == pcb) {
            return pcb;
        }
        iter = iter->pid_sibling_next;
    }

    return NULL;
}
// PCB* op_sort_ready_by_sjf(PROCESS_MANAGER* self) { // need to simplify the algo and test
//     PCB* ready_queue_head = self->ready_queue_head;
    
//     if (ready_queue_head == NULL) {
//         printf("SJF sort: Ready queue is empty\n");
//         return NULL;
//     }
    
//     printf("SJF sort: start ready queue head is :%d\n", ready_queue_head->pid);
    
//     PCB* slow = ready_queue_head;
//     PCB* fast = ready_queue_head;
    
//     // if circular break
//     while (fast != NULL && fast->pid_sibling_next != NULL) {
//         slow = slow->pid_sibling_next;
//         fast = fast->pid_sibling_next;

//         if (fast != NULL) {
//             fast = fast->pid_sibling_next;
//         }
        
//         if (slow == fast && slow != NULL) {
//             printf("ERROR: Circular linked list detected in ready queue at PID %d!\n", slow->pid);
//             printf("Breaking the cycle...\n");
            
//             PCB* the_head = ready_queue_head;
//             while (the_head != slow) {
//                 the_head = the_head->pid_sibling_next;
//                 slow = slow->pid_sibling_next;
//             }
            
//             // Now cycle_start is the start of the cycle
//             // Find the node before cycle_start
//             PCB* prev = ready_queue_head;
//             while (prev != NULL && prev->pid_sibling_next != the_head) {
//                 prev = prev->pid_sibling_next;
//             }
            
//             if (prev != NULL) {
//                 printf("Breaking cycle at node before PID %d\n", the_head->pid);
//                 prev->pid_sibling_next = NULL;  // Break the cycle
//             } else {
//                 // If we can't find prev, just set head to NULL
//                 printf("Could not find cycle start, setting head to NULL\n");
//                 self->ready_queue_head = NULL;
//                 return NULL;
//             }
//             break;
//         }
//     }
    
//     // Reset for second pass
//     slow = ready_queue_head;
    
//     // Second pass: count actual nodes (non-circular now)
//     while (slow != NULL) {
//         slow = slow->pid_sibling_next;
//     }
        
    
//     // First, filter out terminated processes
//     PCB* filtered_head = NULL;
//     PCB* filtered_tail = NULL;
//     PCB* current = ready_queue_head;
//     int filtered_count = 0;
    
//     while (current != NULL) {
//         PCB* next = current->pid_sibling_next;
        
//         // Debug print first few nodes
//         if (filtered_count < 5) {
//             printf("SJF sort: Node %d - PID %d, burst=%.2f, state=%d\n", 
//                    filtered_count, current->pid, current->burst_time, current->etat);
//         }
        
//         // Skip terminated processes
//         if (current->etat != TERMINATED) {
//             if (filtered_head == NULL) {
//                 filtered_head = current;
//                 filtered_tail = current;
//                 current->pid_sibling_next = NULL;
//             } else {
//                 filtered_tail->pid_sibling_next = current;
//                 filtered_tail = current;
//                 current->pid_sibling_next = NULL;
//             }
//             filtered_count++;
//         }
//         current = next;
//     }
    
//     printf("SJF sort: Filtered %d active processes (non-terminated)\n", filtered_count);
    
//     if (filtered_head == NULL) {
//         printf("SJF sort: No active processes after filtering\n");
//         return NULL;
//     }
    
//     // Now sort the filtered list by burst time (shortest first)
//     PCB* sorted_head = NULL;
//     current = filtered_head;
//     int sorted_count = 0;
    
//     while (current != NULL) {
//         PCB* next = current->pid_sibling_next;
        
//         if (sorted_head == NULL) {
//             // First node
//             current->pid_sibling_next = NULL;
//             sorted_head = current;
//         } else if (current->burst_time < sorted_head->burst_time) {
//             // Insert at head (shorter than current head)
//             current->pid_sibling_next = sorted_head;
//             sorted_head = current;
//         } else {
//             // Insert in middle or at tail
//             PCB* search = sorted_head;
//             while (search->pid_sibling_next != NULL &&
//                    search->pid_sibling_next->burst_time <= current->burst_time) {
//                 search = search->pid_sibling_next;
//             }
//             current->pid_sibling_next = search->pid_sibling_next;
//             search->pid_sibling_next = current;
//         }
        
//         sorted_count++;
//         current = next;
//     }
    
//     // Debug: Print sorted order
//     printf("SJF sort: Sorted order (first 5): ");
//     current = sorted_head;
//     for (int i = 0; i < 5 && current != NULL; i++) {
//         printf("%d(%.2f) ", current->pid, current->burst_time);
//         current = current->pid_sibling_next;
//     }
//     printf("\n");
    
//     printf("SJF sort: Completed sorting %d processes\n", sorted_count);

//     return sorted_head;
// }


PCB* op_get_next_ready_element(PROCESS_MANAGER* self, PCB* current_pcb) {

    if (current_pcb == NULL) {
        return self->ready_queue_head;
    }

    PCB* next = current_pcb->pid_sibling_next;

    if (next == current_pcb && current_pcb->remaining_time < 0.00001) {
        return NULL;
    }

    return next;
}


WORK_RETURN proc_kill(PROCESS_MANAGER* self) {

    if (self->free_process_table(self) != TASK_SUCC) {
        fprintf(stderr, "ERROR ON: proc_kill failed to free the process list");
        return WORK_ERROR;
    }


    free(self);

    return WORK_DONE;
}

TASK op_free_process_list(PROCESS_MANAGER* self) {
    if (self == NULL || self->process_table_head == NULL) {
        return TASK_SUCC;
    }
    
    PCB* current = self->process_table_head;
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
        free(current);
        current = next;
    }
    
    self->process_table_head = NULL;
    return TASK_SUCC;
}

float op_proc_get_max_arrival_time(PROCESS_MANAGER* self) {
    return self->max_arrival_time;
}





// ---------------getter 
FILE* op_get_processus_buffer(PROCESS_MANAGER* self) {
    return self->processus_buffer;
}


TASK op_pro_init(PROCESS_MANAGER* self, FILE* buffer, int algorithm) {

    if (buffer == NULL) {
        fprintf(stderr, "ERROR ON: op_pro_init, buffer is NULL\n");
        return TASK_ERR;
    }

    // --------- function assigning

    self->create_process_table = op_create_process_table;
    self->create_ready_queue = op_create_ready_queue;
    self->create_blocked_queue = op_create_blocked_queue;
    self->push_to_ready_queue = op_push_to_ready_queue;
    self->delete_from_ready_queue = op_delete_from_ready_queue;
    self->add_process_to_blocked_queue = op_add_process_to_blocked_queue;
    self->free_process_table = op_free_process_list;
    self->update_process = op_pro_update_process;
    self->kill = proc_kill;
    self->get_next_ready_element = op_get_next_ready_element;
    self->get_blocked_queue_element = op_get_blocked_queue_element;
    self->delete_from_blocked_queue = op_delete_from_blocked_queue;
    self->get_ready_queue_head = op_get_ready_queue_head;
    self->get_next_process_table = op_get_next_process_table;
    self->insert_after_ready = op_insert_after_ready;
    self->free_ready_queue = op_free_ready_queue;
    self->update_read_queue = op_update_read_queue;
    self->update_self_temps = op_update_self_temps;
    self->find_max_arrival_time = op_find_max_arrival_time;
    self->get_max_arrival_time = op_proc_get_max_arrival_time;
    self->sort_by_rt = op_sort_ready_by_rt;
    self->sort_by_sjf = op_sort_ready_by_sjf;
    self->sort_by_priority = op_sort_ready_by_priority;
    self->mark_process_table_pcb_terminated = op_mark_process_table_pcb_terminated;

    // --------------------------

    // --------------------getter
    self->get_processus_buffer = op_get_processus_buffer;



    self->ready_queue_head = NULL; 

    self->processus_buffer = buffer;

    self->temps = .0;

    self->last_runed = -1.0f;

    self->process_table_head = self->create_process_table(self); // return the first element in process table

    self->ready_queue_head = self->create_ready_queue(self, self->process_table_head, (algorithm == 0 ? true : false)); // if it's rr then circular

    self->blocked_queue_head = self->create_blocked_queue();

    self->max_arrival_time = self->find_max_arrival_time(self);

    return TASK_SUCC;
}
