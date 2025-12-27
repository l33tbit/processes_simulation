#pragma once
// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>

// #include "operations/helpers/process_manager.h"
// #include "operations/ressource_manager.h"
// #include "structs/process.h"

// #include "structs/simulator.h"
// #include "structs/ressource_manager.h"
// #include "structs/ressource.h"
// #include "structs/process_manager.h"
// #include "structs/ressource.h"
  
#include "../../lib/structs/simulator.h"
#include "../../src/implementation/simulator.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>


void print_pcb(PCB* pcb) {
    if (pcb == NULL) {
        printf("PCB is NULL\n");
        return;
    }
    printf("process_id: %d\n", pcb->pid);
    printf("process_name: %s\n", pcb->process_name);
    printf("user_id: %s\n", pcb->user_id);
    printf("priority: %d\n", pcb->prioritie);
    printf("programe_counter: %d\n", pcb->programme_compteur);
    printf("memoire: %d\n", pcb->memoire_necessaire);
    printf("burst time: %f\n", pcb->burst_time);
    printf("remaining time: %f\n", pcb->remaining_time);
    printf("temps d arriver: %f\n", pcb->statistics->temps_arrive);
    
    if (pcb->instructions_head) {
        printf("first instruction id: %d\n", pcb->instructions_head->instruct_id);
    }
    
    if (pcb->statistics) {
        time_t creation_time = (time_t)pcb->statistics->temps_creation;
        printf("process %s\n", ctime(&creation_time)); // to format
    }
    // INSTRUCTION* instructions_head
    INSTRUCTION* TEMP = pcb->instructions_head;
    while (TEMP != NULL) { // enum are intigers
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


void print_pcb_chaine(PCB* pcb_head) {

    PCB* temp = pcb_head;
    bool circular = false;

    printf("hiiiit print_pcb_chaine");

    if (pcb_head == NULL) {
        printf("PCB is NULL\n");
        return;
    }

    while (temp != NULL) {
        if (circular == true && temp == pcb_head) {
            break;
        }
        circular = true;
        print_pcb(temp);
        temp = temp->pid_sibling_next;
        fflush(stdout); // Ensure output is printed before any potential crash
    }


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


// // testing the ressource creator
// RESSOURCE_ELEMENT* testing_ressource_creator() {

//     RESSOURCE_ELEMENT* head = op_create_ressource_list();

//     if (head == NULL) {
//         fprintf(stderr, "ERROR ON: error while allocating the ressources_head\n");
//         exit(1);
//     }

//     RESSOURCE_ELEMENT* ressources_head = head;
    
//     while (ressources_head != NULL) {
//         printf("ressource name: %s\n", ressources_head->ressource_name);
//         ressources_head = ressources_head->next_ressource;
//     }

//     return head;
// }

// // test execute process
// void testing_execute_process() {

//     PCB* head = testing_the_csv_parsing();

//     SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));

//     simulator->ressource_manager = (RESSOURCE_MANAGER*)malloc(sizeof(RESSOURCE_MANAGER));

//     simulator->process_manager = (PROCESS_MANAGER*)malloc(sizeof(PROCESS_MANAGER));

//     simulator->schedular = (ORDONNANCEUR*)malloc(sizeof(ORDONNANCEUR));

//     if (simulator == NULL || simulator->process_manager == NULL || simulator->schedular == NULL || simulator->ressource_manager == NULL) {
//         fprintf(stderr, "ERROR ON: error while allocating the simulator\n");
//         exit(1);
//     }

    

// }


/*
gcc -Wall -Wextra -std=c11 \
    -I./lib \
    -I./lib/implementation \
    -I./lib/implementation/helpers \
    -I./lib/structs \
    "unit_testing/unit_tester.c" \
    "lib/implementation/helpers/process_manager.c" \
    "lib/implementation/process_manager.c" \
    "lib/implementation/ressource_manager.c" \
    -o unit_test
  
*/


void print_ressource(RESSOURCE_ELEMENT* ressource) {
    printf("ressource name: %s\n", ressource->ressource_name);
    switch (ressource->ressource)
    {
    case 0:
        printf("ressource type: AAA\n");
        break;
    
    case 1:
        printf("ressource type: BBB\n");
        break;

    
    case 2:
        printf("ressource type: CCC\n");
        break;

    
    case 3:
        printf("ressource type: DDD\n");
        break;

    
    case 4:
        printf("ressource type: EEE\n");
        break;
    
    case 5:
        printf("ressource type: FFF\n");
        break;
    default:
        break;
    }
    printf("ressource availability: %d\n", ressource->disponibilite);
}

void print_ressources_list(RESSOURCE_MANAGER* ressource_manager) {

    RESSOURCE_ELEMENT* ressources_head = ressource_manager->ressources;

    while (ressources_head != NULL) {
        print_ressource(ressources_head);
        ressources_head = ressources_head->next_ressource;
    }
}


void test_runing() {

    SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));
    
    printf("hit main\n\n\n");

    FILE* buffer = fopen("data.csv", "r");
    if (buffer == NULL) {
        perror("ERROR: Failed to open data.csv");
        exit(1);
    }

    simulator->init = op_simul_init;

    simulator->init(simulator, buffer);

    simulator->stop(simulator);

    fclose(buffer);

}

void testing_parser() {

    FILE* buffer = fopen("/home/zeus/projects/processus_simulation/src/unit_testing/data_testing.csv", "r");

    if (!buffer) {
        perror("fopen");
        exit(1);
    }

    PCB* pcb_head = extract_from_buffer(buffer);

    if (pcb_head == NULL) {
        fprintf(stderr, "ERROR ON: extract_from_buffer failed has returned NULL\n");
        exit(1);
    }

    print_pcb_chaine(pcb_head);

    fclose(buffer);

}


// testing the  op_pro_init  : initialization of process manger
void test_process_manager_initialization() { 


    FILE* f = fopen("/home/zeus/projects/processus_simulation/src/unit_testing/data_testing.csv", "r");

    if (!f) {
        perror("fopen");
        exit(1);
    }

    PROCESS_MANAGER* process_manager = (PROCESS_MANAGER*)malloc(sizeof(PROCESS_MANAGER));
    
    if (process_manager == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the process_manager\n");
        exit(1);
    }
    process_manager->init = op_pro_init; // assign the initializer function
    
    printf("finished  process_manager->init = op_pro_init\n");

    process_manager->init(process_manager, f, 0);
    
    printf("finished process_manager->init\n");

    print_pcb_chaine(process_manager->process_table_head);

    fclose(f);

}


// test ressource manager initialization
void test_ressource_manager_initialization() {

    RESSOURCE_MANAGER* ressource_manager = (RESSOURCE_MANAGER*)malloc(sizeof(RESSOURCE_MANAGER));

    if (ressource_manager == NULL) {
        fprintf(stderr, "ERROR ON: error while allocating the ressource_manager\n");
        exit(1);
    }

    ressource_manager->init = op_rm_init; // assign the initializer function

    ressource_manager->init(ressource_manager);

    print_ressources_list(ressource_manager);
   
}


// initialization and kill are tested and work weally fine
void test_simulator_initialization() { 

   SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));

    
    printf("hit main\n\n\n");

    FILE* buffer = fopen("/home/zeus/projects/processus_simulation/src/unit_testing/data_testing.csv", "r");
    if (buffer == NULL) {
        perror("ERROR: Failed to open data.csv");
        exit(1);
    }

    simulator->init = op_simul_init;


    simulator->init(simulator, buffer);

    printf("hit print_pcb_chaine\n\n\n");
    fflush(stdout);
    print_pcb_chaine(simulator->process_manager->ready_queue_head); 

    if (simulator->ressource_manager && simulator->ressource_manager->ressources) {
        print_ressources_list(simulator->ressource_manager);
    }

    simulator->stop(simulator);

    fclose(buffer);

    return 0;
}


// testing work
// rr is working mozafak
void test_simulator_work() { 

   SIMULATOR* simulator = (SIMULATOR*)malloc(sizeof(SIMULATOR));

    
    printf("hit main\n\n\n");

    FILE* buffer = fopen("/home/zeus/projects/processus_simulation/src/unit_testing/data_testing.csv", "r");
    if (buffer == NULL) {
        perror("ERROR: Failed to open data.csv");
        exit(1);
    }

    simulator->init = op_simul_init;

    simulator->init(simulator, buffer);

    fflush(stdout);

    simulator->work(simulator, simulator->options);

    simulator->stop(simulator);

    fclose(buffer);

    return 0;
}



int main() {
    // testing_process_table_creation_and_ready_queue();
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

    // test_runing();

    test_simulator_work();

    // testing_parser();

    return 0;
}