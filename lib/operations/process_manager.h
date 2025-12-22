#pragma once
#include <stdbool.h>

#include "structs/process_manager.h"
#include "operations/process_manager.h"
#include "structs/process.h"
#include "operations/helpers/process_manager.h"
#include "operations/process.h"

// initialization
PCB* op_create_process_table(FILE* buffer) {
    // read_buffer and get elements list and count
    // create process table
    // iterate and add to process table

    // heap allocated buffer
    size_t size = 10 * 1024 * 1024; // 10mbytes

    char* buffera = (char*)malloc(size);

    if (!buffera) {
        fprintf(stderr, "ERROR ON: error while allocating the buffer before reading csv\n");
        exit(1);
    }

    FILE* csv_buffer = fopen("/home/zeus/projects/processus_simulation/unit_testing/csv_testin_parocess_manager_parsing.csv", "r");

    if (!csv_buffer) {
        perror("ERROR ON: error while reading csv\n");
        exit(1);
    }

    PCB* pcb_head = extract_from_buffer(csv_buffer);

    while (pcb_head == NULL) {
        fprintf(stderr, "ERROR ON: extract_from_buffer failed has returned NULL in allocating pcb_head\n");
    }

    return pcb_head; 

}


PCB* op_create_ready_queue(bool circular, PCB* pcb) {  // we dont pass the algo because it initialized before the schedular
    if (pcb == NULL) {
        fprintf(stderr, "ERROR ON: op_create_ready_queue failed has pcb is NULL\n");
        exit(1);
    }

    // first pcb in the ready queue
    PCB* ready_queue_head = (PCB*)malloc(sizeof(PCB));
    if (ready_queue_head == NULL) {
        fprintf(stderr, "ERROR ON: op_create_ready_queue , readu_queue_head allocation returned NULL\n");
        exit(1);
    }

    // Perform a deep copy of the PCB
    *ready_queue_head = *pcb;
    // Allocate new memory for statistics and copy the data
    ready_queue_head->statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS));
    if (ready_queue_head->statistics == NULL) {
        fprintf(stderr, "ERROR ON: op_create_ready_queue, ready_queue_head->statistics allocation returned NULL\n");
        free(ready_queue_head);
        exit(1);
    }
    if (pcb->statistics != NULL) {
        *(ready_queue_head->statistics) = *(pcb->statistics);
    } else {
        // Handle case where original statistics is NULL, maybe just zero it out
        memset(ready_queue_head->statistics, 0, sizeof(PROCESS_STATISTICS));
    }

    PCB* list_pcb_head_next = pcb->pid_sibling_next; // because we already have the first lement
    PCB* ready_pcb_head = ready_queue_head;

    // copy all nodes
    while (list_pcb_head_next != NULL) {
        // allocate a node
        PCB* next_node = (PCB*)malloc(sizeof(PCB));
        if (next_node == NULL) {
            // if allocation failed
            fprintf(stderr, "ERROR ON: op_create_ready_queue , next_node allocation returned NULL\n");
            // free the chaine first
            PCB* temp = ready_queue_head;
            while (temp != NULL) {
                PCB* next = temp->pid_sibling_next;
                free(temp);
                temp = next;
            }
            exit(1);
        }

        // Perform a deep copy of the PCB
        *next_node = *list_pcb_head_next;
        // Allocate new memory for statistics and copy the data
        next_node->statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS));
        if (next_node->statistics == NULL) {
            fprintf(stderr, "ERROR ON: op_create_ready_queue, next_node->statistics allocation returned NULL\n");
            // Proper cleanup needed here
            exit(1);
        }
        if (list_pcb_head_next->statistics != NULL) {
            *(next_node->statistics) = *(list_pcb_head_next->statistics);
        } else {
            memset(next_node->statistics, 0, sizeof(PROCESS_STATISTICS));
        }


        // chained it
        ready_pcb_head->pid_sibling_next = next_node;

        // move to next
        ready_pcb_head = ready_pcb_head->pid_sibling_next;
        list_pcb_head_next = list_pcb_head_next->pid_sibling_next;
        }

        if (circular == false) {
            // make the last point to null
            PCB* current = ready_queue_head; // for iteration
            
            if (current != NULL) {
                while (current->pid_sibling_next != NULL) {
                    current = current->pid_sibling_next;
                }
                current->pid_sibling_next = NULL;  // null in the end        }
        } else {
            // make last node pointe to the head
            if (ready_queue_head != NULL) {
                PCB* current = ready_queue_head; // for the iteration
                
                // find last
                while (current->pid_sibling_next != NULL && 
                    current->pid_sibling_next != ready_queue_head) {
                    current = current->pid_sibling_next;
                }
                
                // last will pointe to head for circular
                current->pid_sibling_next = ready_queue_head;
            }
        }

        return ready_queue_head;
    }
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

PCB* op_sort_ready_by_rt(PROCESS_MANAGER* process_manager) { // copy past and change condition and error output

    PCB* ready_queue_head = process_manager->ready_queue_head;

    if (ready_queue_head == NULL) {
        fprintf(stderr, "ERROR ON: op_sort_ready_by_rt , ready_queue_head is NULL\n");
        exit(1);
    }
    PCB* sorted_head = NULL;
    PCB* current = ready_queue_head; // node that will contain the proces arrived before current

    // find the process with the first arrival tile
    while (current != NULL) {
        PCB* next = current->pid_sibling_next;
    
        // insert the current into the sorted
        if (sorted_head == NULL || current->remaining_time < sorted_head->remaining_time) {
            // insert in the start
            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {
            // find the pointer where it inserted
            PCB* search = sorted_head;
            while (search->pid_sibling_next != NULL && // if the search has reach the end
                    search->pid_sibling_next->remaining_time < current->remaining_time) { // or if the searched value's temps arrive is < that current 
                search = search->pid_sibling_next;
            }

            current->pid_sibling_next = search->pid_sibling_next; // make the current'e next is searched's ext
            search->pid_sibling_next = current; // and the searched next to current
        }
        current = next;
    }

    return sorted_head;
}

PCB* op_sort_ready_by_priority(PROCESS_MANAGER* process_manager) {

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
        if (sorted_head == NULL || current->prioritie < sorted_head->prioritie) {
            // insert in the start
            current->pid_sibling_next = sorted_head;
            sorted_head = current;
        } else {
            // find the pointer where it inserted
            PCB* search = sorted_head;
            while (search->pid_sibling_next != NULL && // if the search has reach the end
                    search->pid_sibling_next->prioritie < current->prioritie) { // or if the searched value's temps arrive is < that current 
                search = search->pid_sibling_next;
            }

            current->pid_sibling_next = search->pid_sibling_next; // make the current'e next is searched's ext
            search->pid_sibling_next = current; // and the searched next to current
        }
        current = next;
    }

    return sorted_head;
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

PCB* op_create_blocked_queue(PCB* blocked_queue_head) {
    // i think i just need to init it and when a process is blocked will be chained
    return NULL; // same a declaring pcb* pcb = null and return it
}


//pcb related
// with nullty check; updating temps_fin = market_terminated = update_turnround ; updating cpu_temps_used = updating_remaining_time
PCB* op_update_process(PCB* pcb, time_t *temps_fin, float *cpu_temps_used, float *temps_attente, int *cpu_usage) {
    if (pcb == NULL) {
        fprintf(stderr, "ERROR ON: op_update_process , pcb is NULL\n");
        return NULL;
    }

    // updating the given fields
    if (temps_fin) { // updating temp fin = update tournround
        pcb->statistics->temps_fin = *temps_fin;
        pcb->statistics->tournround = *temps_fin - pcb->statistics->temps_arrive;
    }
    
    if (cpu_temps_used) {
        pcb->cpu_time_used += *cpu_temps_used; // because initialized to 0
    }
    
    if (temps_attente) {
        pcb->statistics->temps_attente += *temps_attente; // because init to 0
    }
    
    // if (cpu_usage) {
    //     pcb->cpu_usage += cpu_usage; // because also init to 0
    // }

    return pcb;
}


// ready queue related
PCB* op_push_to_ready_queue(PCB* ready_queue_head, struct PCB* pcb) {// LIST CREATED NEED TO BEE FREE AFRTER ASSIGNING IT TO the proces_manager ready queue
    if (pcb == NULL) {
        return ready_queue_head;  // Nothing to add
    }

    if (ready_queue_head == NULL) {
        return pcb; // the new pcb will be the ready queue head but i think that the condition will not be true, because of the function of ready_queue
    }

    PCB* last = ready_queue_head; // the last in chaine

    // check it's circular with one element
    if (last->pid_sibling_next == ready_queue_head) {
        ready_queue_head->pid_sibling_next = pcb;
        pcb->pid_sibling_next = ready_queue_head; // keep it circular
    } else {
        // a simplke chaine or circular with multiple elements
        while (last->pid_sibling_next != NULL && last->pid_sibling_next != ready_queue_head) { // the second check is for circularity
            last = last->pid_sibling_next; // jump by one element
        }

        // insert in the end
        last->pid_sibling_next = pcb;
        pcb->pid_sibling_next = NULL;


        // if circular
        if (last->pid_sibling_next == ready_queue_head) {
            pcb->pid_sibling_next = ready_queue_head;
        }
    }
    
    return ready_queue_head;
}

PCB* op_delete_from_ready_queue(PCB* ready_queue_head, PCB* pcb) {// the chaine node should be freed

    if (ready_queue_head == NULL || pcb == NULL) {
        return ready_queue_head;
    }

    // if the head that need to be deleted
    if (ready_queue_head == pcb) {
        PCB* hold = ready_queue_head->pid_sibling_next;
        free(ready_queue_head);
        return hold;
    }

    // if not in the start
    PCB* prcd = ready_queue_head;
    PCB* current = ready_queue_head->pid_sibling_next;

    while (current != NULL) {
        if (current == pcb) { // found
            // jump it and free it
            prcd->pid_sibling_next = current->pid_sibling_next;
            free(current);
            break;
        }
        prcd = current;
        current = current->pid_sibling_next;
    }

    return ready_queue_head;
}

// bloqued queue related
PCB* op_add_process_to_blocked_queue(PCB* blocked_queue_head, PCB* pcb) { // should covert pcb to BLOCKED_QUEUE_ELEMENT then push it

    if (blocked_queue_head == NULL) { // if there is no process in the blocked the pcb will be the head
        return pcb;
    }

    PCB* iter = blocked_queue_head;

    while (iter->pid_sibling_next != NULL) {
        iter = iter->pid_sibling_next;
    }

    iter->pid_sibling_next = pcb;

    return blocked_queue_head;
}

PCB* op_delete_from_blocked_queue(PCB* blocked_queue_head, PCB* pcb) {

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
}

PCB* op_get_blocked_queue_element(PCB* blocked_queue_head, PCB* pcb) {

    if (blocked_queue_head == NULL || pcb == NULL) {
        return blocked_queue_head;
    }

    PCB* got = NULL;
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

PCB* op_get_next_ready_element(PCB* current_pcb) {
 
    if (current_pcb == NULL) {
        return NULL;
    }

    PCB* next = current_pcb->pid_sibling_next;

    return next;
}



PCB* op_assign_functions_to_pcb(PCB* pcb) {
    pcb->update_temps_attente = op_update_temps_attente;
    pcb->mark_instruction_terminated = op_mark_instruction_terminated;
    pcb->update_temps_arrive = op_update_temps_arrive;
    pcb->update_temps_creation = op_update_temps_creation;
    pcb->update_temps_fin = op_update_temps_fin;
    pcb->define_next = op_pcb_define_next;
    pcb->define_previous = op_pcb_define_previous;
}