
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "operations/helpers/process_manager.h"
#include "operations/ressource_manager.h"
#include "structs/process.h"

void print_pcb(PCB* pcb) {
    printf("process_id: %d\n", pcb->pid);
    printf("process_name: %s\n", pcb->process_name);
    printf("user_id: %s\n", pcb->user_id);
    printf("priority: %d\n", pcb->prioritie);
    printf("programe_counter: %d\n", pcb->programme_compteur);
    printf("memoire: %d\n", pcb->memoire_necessaire);
    printf("burst time: %f\n", pcb->burst_time);
    printf("remaining time: %f\n", pcb->remaining_time);
    printf("temps d arriver: %f\n", pcb->statistics->temps_arrive);
    printf("first instruction id: %d\n", pcb->instructions_head->instruct_id);
    printf("process %s\n", ctime(&pcb->statistics->temps_creation)); // to format
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
    // kob lia hloben a wliidi raho khdem
}

// PCB* testing_the_csv_parsing() {
//     FILE* csv_buffer = fopen("/home/zeus/projects/processus_simulation/unit_testing/csv_testin_parocess_manager_parsing.csv", "r");

//     if (!csv_buffer) {
//         perror("ERROR ON: error while opening csv file\n");
//         exit(1);
//     }

//     PCB* pcb_head = extract_from_buffer(csv_buffer);
    
//     fclose(csv_buffer);  // Close the file

//     // Test printing all PCBs
//     PCB* current = pcb_head;
//     while (current != NULL) {
//         // print_pcb(current);  // Uncomment when you have this function
//         printf("PCB PID: %d, Name: %s\n", current->pid, current->process_name);
//         current = current->pid_sibling_next;
//     }

//     return pcb_head;  // Return the head of the list
// }

// // testing the process table creation and ready queue sorting fcfs
// void testing_process_table_creation_and_ready_queue() {

//     PCB* head = testing_the_csv_parsing();

//     if (head == NULL) {
//         fprintf(stderr, "ERROR ON: error while allocating the head before reading csv\n");
//         exit(1);
//     }

//     PCB* ready_queue = op_create_ready_queue(false, head);

//     if (ready_queue == NULL) {
//         fprintf(stderr, "ERROR ON: error while allocating the ready_queue before reading csv\n");
//         exit(1);
//     }



//     PCB* sorted_ready_queue = op_sort_ready_by_fc(ready_queue, false);

//     if (sorted_ready_queue == NULL) {
//         fprintf(stderr, "ERROR ON: error while allocating the sorted_ready_queue before reading csv\n");
//         exit(1);
//     }

//     // till here all is good


//     // PCB* sorted_rt = (PCB*)op_sort_ready_by_rt(ready_queue);
//     // printf("sort by rt\n");

//     // while (ready_queue != NULL) {
//     //     print_pcb(ready_queue);
//     //     ready_queue = ready_queue->pid_sibling_next;
//     // }

//     while (sorted_ready_queue != NULL) {
//         print_pcb(sorted_ready_queue);
//         sorted_ready_queue = sorted_ready_queue->pid_sibling_next;
//     }
// }


// testing the ressource creator
RESSOURCE_ELEMENT* testing_ressource_creator() {

    RESSOURCE_ELEMENT* head = op_create_ressource_list();

    if (head == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the ressources_head\n");
        exit(1);
    }

    RESSOURCE_ELEMENT* ressources_head = head;
    
    while (ressources_head != NULL) {
        printf("ressource name: %s\n", ressources_head->ressource_name);
        ressources_head = ressources_head->next_ressource;
    }

    return head;
}


/*
gcc -Wall -Wextra -std=c11 \
    -I./lib \
    -I./lib/operations \
    -I./lib/operations/helpers \
    -I./lib/structs \
    "unit_testing/unit_tester.c" \
    "lib/operations/helpers/process_manager.c" \
    "lib/operations/process_manager.c" \
    "lib/operations/ressource_manager.c" \
    -o unit_test
  
*/
  

int main() {
    // testing_process_table_creation_and_ready_queue(); // parsed is very vawy vawygood

    // PCB* pc = testing_the_csv_parsing();
    // while (pc != NULL) {
    //     print_pcb(pc);
    //     pc = pc->pid_sibling_next;
    // }

    // RESSOURCE_ELEMENT* ressources_head = testing_ressource_creator();

    // op_mark_ressource_unavailable(ressources_head, CCC);

    // printf("disponibility : %d\n", op_check_if_ressource_available(ressources_head, CCC));

    // op_mark_ressource_available(ressources_head, CCC);
    // printf("disponibility : %d\n", op_check_if_ressource_available(ressources_head, CCC));

    // op_mark_ressource_unavailable(ressources_head, CCC);

    // printf("disponibility : %d\n", op_check_if_ressource_available(ressources_head, CCC));



    return 0;
}