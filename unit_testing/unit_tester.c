
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "operations/helpers/process_manager.h"
#include "structs/process.h"

void print_pcb(PCB* pcb) {
    printf("process_id: %d\n", pcb->pid);
    printf("process_name: %s\n", pcb->process_name);
    printf("user_id: %s\n", pcb->user_id);
    printf("priority: %d\n", pcb->prioritie);
    printf("programe_counter: %d\n", pcb->programme_compteur);
    printf("memoire: %d\n", pcb->memoire_necessaire);
    printf("burst time: %f\n", pcb->burst_time);
    printf("process %s\n", ctime(&pcb->statistics->temps_creation)); // to format
    printf("first instruction id: %d\n", pcb->instructions_head->instruct_id);
    // INSTRUCTION* instructions_head
    INSTRUCTION* TEMP = pcb->instructions_head;
    while (TEMP->next != NULL) { // enum are intigers
        // typedef enum {
        //     AAA, BBB, CCC, DDD, EEE, FFF
        // } RESSOURCE;
        
        switch (TEMP->type) {
            case 0:
                printf("instruct : AAA\n");
                break;
            case 1: 
                printf("instruct : BBB\n");
                break;
            case 2: 
                printf("instruct : CCC\n");
                break;
            case 3: 
                printf("instruct : DDD\n");
                break;
            case 4: 
                printf("instruct : EEE\n");
                break;
            case 5: 
                printf("instruct : FFF\n");
                break;
        }
        TEMP = TEMP->next;
    }
    free(TEMP);
    // kob lia hloben a wliidi raho khdem
    
}

PCB* testing_the_csv_parsing() {

    // heap allocated buffer
    size_t size = 10 * 1024 * 1024; // 10mbytes

    char* buffer = (char*)malloc(size);

    if (!buffer) {
        fprintf(stderr, "ERROR ON: error while allocating the buffer before reading csv\n");
        exit(1);
    }

    FILE* csv_buffer = fopen("/home/zeus/projects/processus_simulation/unit_testing/csv_testin_parocess_manager_parsing.csv", "r");

    if (!csv_buffer) {
        perror("ERROR ON: error while reading csv\n");
        exit(1);
    }

    PCB* pcb_head = extract_from_buffer(csv_buffer);

    // while (pcb_head != NULL) {
    //     print_pcb(pcb_head);
    //     pcb_head = pcb_head->pid_sibling_next;
    // }

    return pcb_head;

}

// testing the process table creation and ready queue sorting fcfs
void testing_process_table_creation_and_ready_queue() {

    PCB* head = testing_the_csv_parsing();

    if (head == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the head before reading csv\n");
        exit(1);
    }

    PCB* ready_queue = op_create_ready_queue(head);

    if (ready_queue == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the ready_queue before reading csv\n");
        exit(1);
    }

    PCB* sorted_ready_queue = op_sort_ready_by_fc(ready_queue);

    if (sorted_ready_queue == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the sorted_ready_queue before reading csv\n");
        exit(1);
    }

    while (ready_queue != NULL) {
        print_pcb(ready_queue);
        ready_queue = ready_queue->pid_sibling_next;
    }

    while (sorted_ready_queue != NULL) {
        print_pcb(sorted_ready_queue);
        sorted_ready_queue = sorted_ready_queue->pid_sibling_next;
    }
    // checked all good
}   

// testing process functions







/*
gcc -Wall -Wextra -std=c11 \
    -I./lib \
    -I./lib/operations \
    -I./lib/operations/helpers \
    -I./lib/structs \
    "unit_testing/unit_tester.c" \
    -o unit_test

*/

int main() {

    testing_the_csv_parsing();

    return 0;
}