#pragma once

#include "lib/structs/process.h"
#include "lib/structs/process_manager.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h> // for INT_MAX AND INT_MIN


// need_to_be_changed
char instructions_list[6][4] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF"};
int instruction_list_len = 6;

// ------------structs
typedef struct buffer_return {
    PCB* pcb;
    int size;
} buffer_return;


// process_name,user_id,ppid,priority,[instruction],n_instruction,memoire,burst,
typedef struct parser_return {
    char process_name[20];
    char user_id[20];
    int priority;
    INSTRUCTION* instructions_head;
    int instructions_count;
    int memoire;
    float burst;
    float temps_creation;
    float temps_arrive;
    bool unvalid_process_csv_check;
} parser_return;

typedef struct insruction_parser_return {
    INSTRUCTION* instructions_head;
    INSTRUCTION* instructions_fin; //adding it for time consuming
    int count;
} insruction_parser_return;


// -----------------helpers 
PCB* pcb_chaine(PCB* pcb, PCB* pcb_end) {
    if (pcb_end->pid_sibling_next != NULL) {
        fprintf(stderr, "ERROR ON: parser function pcb_chaine pcb_end's next in not null\n");
        exit(1);
    }
    pcb->pid_sibling_next = NULL;
    pcb_end->pid_sibling_next = pcb; // last pcb's next is the created pcb
    return pcb;
}

bool check_known_ressource(char ressource[]) {
    int flag = 0;
    for (int i = 0; i < 6; i++) {
        if (strcmp(instructions_list[i], ressource) == 0) {
            flag = 1;
            break;
        }
    }

    if (flag == 1) 
        return true;
    else
        return false;

}

void free_instructions_chaine(INSTRUCTION* instruct_head) {
    if (instruct_head == NULL) {
        fprintf(stderr, "ERROR ON: instructionfree_instructions_chaine attemp of freeing a null\n");
    } else {
        INSTRUCTION* temp;
        while (instruct_head != NULL) {
            temp = instruct_head;
            instruct_head = instruct_head->next;
            free(temp);
        }
    }
}

// btw this list is for testing purpose char instructions_list[6][4] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF"};
INSTRUCTION* add_instruction_type(int count, INSTRUCTION* instruct, char instruction[]) {
    bool check = false; // init as false and if conditio nmet make true then check and raise error
    if (strcmp(instruction, "AAA") == 0) { instruct->type = AAA; check = true;}
    else if (strcmp(instruction, "BBB") == 0) { instruct->type = BBB;  check = true;} 
    else if (strcmp(instruction, "CCC") == 0) { instruct->type = CCC;  check = true;} 
    else if (strcmp(instruction, "DDD") == 0) { instruct->type = DDD;  check = true;} 
    else if (strcmp(instruction, "EEE") == 0) { instruct->type = EEE;  check = true;} 
    else if (strcmp(instruction, "FFF") == 0) { instruct->type = FFF;  check = true;}
    
    if (check == false) {
        fprintf(stderr, "ERROR ON: function add_instruction not condition met\n");
        exit(1);
    }


    // make count ad id
    instruct->instruct_id = count;
    
    return instruct; 
}

// handle error where the instruction_fin pointer doesnt pointe to last node
INSTRUCTION* returned_instructions_fin_not_end(INSTRUCTION* fin) {
    while(fin->next != NULL) {
        fin = fin->next;
    }
    return fin;
}

void free_parsed_buffer(parser_return* paresed_buffer) {
    free_instructions_chaine(paresed_buffer->instructions_head);
}

// ---------------pricipale functions

// prototypes latb9a yaati error d return type unmatched like (char*)* chi haja hkk
parser_return* parser_func(char* line);
insruction_parser_return* instruction_parser(char* value);




PCB* extract_from_buffer(FILE* csv_buffer) {

    PCB* pcb_chaine_head = NULL; // decalring and null for random val
    PCB* pcb_chaine_end = NULL;  // same

    int pcb_flag = 0;
    //buffer

    size_t size = 128; // 
    char* line_pcb = (char*)malloc(size);
    if (line_pcb == NULL) {
        fprintf(stderr, "ERROR ON: extract_from_buffer failed to allocate line buffer\n");
        exit(1);
    }
    
    size_t char_count = 0; // using size_t
    int char_got;
    int process_count = 0;

    while((char_got = fgetc(csv_buffer)) != EOF) { // get one char // dont forget temps creation

        if (char_got == '\n') { // if the end of line
            process_count++;
            
            line_pcb[char_count] = '\0'; // end of string
            
            // if line is empty
            if (char_count == 0) {
                // Reset for next line
                char_count = 0;
                continue;
            }
            
            parser_return* paresed_buffer = parser_func(line_pcb); // intializing the returned struct after parsing a line

            
            // BULLTY return from parser_func vide or space
            if (paresed_buffer == NULL) {
                // Skip this line, reset buffer
                char_count = 0;
                continue;
            }
            
            PCB* pcb = (PCB*)malloc(sizeof(PCB)); // initializing the pcb
            PROCESS_STATISTICS* statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS)); // allocate statistics for the pcb

            
            if (paresed_buffer == NULL) {
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer parser_func returned NULL for non-empty line\n");
                free(line_pcb);
                exit(1);
            }

            if (pcb == NULL) {
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer failed allocation pcb\n");
                free_parsed_buffer(paresed_buffer);
                free(line_pcb);
                exit(1);
            }

            if (statistics == NULL) {
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer failed allocation statistics\n");
                free(pcb);
                free_parsed_buffer(paresed_buffer);
                free(line_pcb);
                exit(1);
            }

            // declaring PCB fields
            memset(pcb, 0, sizeof(PCB));
            memset(statistics, 0, sizeof(PROCESS_STATISTICS));
            

            // copy the process name - do this BEFORE checking length
            if (strlen(paresed_buffer->process_name) > 20) { // if the process name from parsed buffer > 20 (the allowed on pcb struct)
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's process name has exceed 20 char %s\n", paresed_buffer->process_name);
                free(pcb);
                free(statistics);
                free_parsed_buffer(paresed_buffer);
                free(line_pcb);
                exit(1);
            }
            strncpy(pcb->process_name, paresed_buffer->process_name, sizeof(pcb->process_name) - 1); // copy just the size of process_name 
            pcb->process_name[sizeof(pcb->process_name) - 1] = '\0'; // add null terminator

            // copy the user_id
            if (strlen(paresed_buffer->user_id) > 20) { // if the process name from parsed buffer > 20 (the allowed on pcb struct)
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's user_id has exceed 20 char %s\n", paresed_buffer->user_id);
                free(pcb);
                free(statistics);
                free_parsed_buffer(paresed_buffer);
                free(line_pcb);
                exit(1);
            }

            if (paresed_buffer->unvalid_process_csv_check == false) {

                // assing statistics
                pcb->statistics = statistics;
                
                pcb->pid = process_count;

                strncpy(pcb->user_id, paresed_buffer->user_id, sizeof(pcb->user_id) - 1); // copy just the size of process_name 
                pcb->user_id[sizeof(pcb->user_id) - 1] = '\0'; // add null terminator

                // copy the priority
                pcb->prioritie = paresed_buffer->priority;

                // copy the instruction
                pcb->instructions_head = paresed_buffer->instructions_head;

                // copy n instructions
                pcb->programme_compteur = paresed_buffer->instructions_count;

                // copy memoire
                pcb->memoire_necessaire = paresed_buffer->memoire;

                // to prevent random value i think
                pcb->current_instruction = NULL;

                // burst
                pcb->burst_time = paresed_buffer->burst;

                // init remaining time as burst
                pcb->remaining_time = paresed_buffer->burst;

                // creation time same type time_t
                pcb->statistics->temps_creation = paresed_buffer->temps_creation;

                // adding arrival time
                pcb->statistics->temps_arrive = paresed_buffer->temps_arrive;

                // Initialize sibling pointer
                pcb->pid_sibling_next = NULL;

                // Free parsed_buffer struct (instructions are now owned by PCB)
                free(paresed_buffer);

                // Reset buffer for next line
                char_count = 0;
                size_t buffer_size = 128;
                line_pcb = (char*)realloc(line_pcb, buffer_size);
                
                if (line_pcb == NULL) {
                    fprintf(stderr, "ERROR ON: extract_from_buffer failed to reallocate line buffer\n");
                    exit(1);
                }

                // Add pcb to chaine
                if (pcb_flag == 0) { // making the first pcb as head with the flag
                    pcb_chaine_head = pcb;
                    pcb_chaine_end = pcb;
                    pcb_flag = 1;
                } else {
                    // Use pcb_chaine function to add to end
                    if (pcb_chaine_end != NULL) {
                        pcb_chaine_end->pid_sibling_next = pcb;
                        pcb_chaine_end = pcb;
                    } else {
                        fprintf(stderr, "ERROR ON: pcb_chaine_end is NULL but pcb_flag is %d\n", pcb_flag);
                        free(line_pcb);
                        free(pcb);
                        exit(1);
                    }
                }

            } else {
                fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's csv validity check has returned true on line : %s\n", line_pcb);
                free(pcb);
                free(statistics);
                free_parsed_buffer(paresed_buffer);
                free(line_pcb);
                exit(1);
            }

        } else {
            // Add character to buffer
            if (char_count >= size - 1) {
                size *= 2;
                char* new_buffer = (char*)realloc(line_pcb, size);
                if (new_buffer == NULL) {
                    fprintf(stderr, "ERROR ON: extract_from_buffer failed to reallocate line buffer\n");
                    free(line_pcb);
                    exit(1);
                }
                line_pcb = new_buffer;
            }
            line_pcb[char_count++] = (char)char_got;
        }
    }

       // if file doesnt end with new line need handle
    if (char_count > 0) {
        line_pcb[char_count] = '\0';
        
        // Skip empty lines
        if (char_count > 0) {
            parser_return* paresed_buffer = parser_func(line_pcb);
            
            if (paresed_buffer != NULL && paresed_buffer->unvalid_process_csv_check == false) {
                PCB* pcb = (PCB*)malloc(sizeof(PCB));
                PROCESS_STATISTICS* statistics = (PROCESS_STATISTICS*)malloc(sizeof(PROCESS_STATISTICS));
                
                if (pcb && statistics) {
                    memset(pcb, 0, sizeof(PCB));
                    memset(statistics, 0, sizeof(PROCESS_STATISTICS));
                    pcb->statistics = statistics;
                    
                    // Copy data (similar to above)
                    if (strlen(paresed_buffer->process_name) <= 20) {
                        strncpy(pcb->process_name, paresed_buffer->process_name, sizeof(pcb->process_name) - 1);
                        pcb->process_name[sizeof(pcb->process_name) - 1] = '\0';
                    }
                    
                    if (strlen(paresed_buffer->user_id) <= 20) {
                        strncpy(pcb->user_id, paresed_buffer->user_id, sizeof(pcb->user_id) - 1);
                        pcb->user_id[sizeof(pcb->user_id) - 1] = '\0';
                    }
                    
                    pcb->prioritie = paresed_buffer->priority;
                    pcb->instructions_head = paresed_buffer->instructions_head;
                    pcb->programme_compteur = paresed_buffer->instructions_count;
                    pcb->memoire_necessaire = paresed_buffer->memoire;
                    pcb->current_instruction = NULL;
                    pcb->burst_time = paresed_buffer->burst;
                    pcb->statistics->temps_creation = paresed_buffer->temps_creation;
                    pcb->statistics->temps_arrive = paresed_buffer->temps_arrive;
                    pcb->remaining_time = paresed_buffer->burst;
                    pcb->pid_sibling_next = NULL;
                    
                    free(paresed_buffer);
                    
                    // Add to chain
                    if (pcb_flag == 0) {
                        pcb_chaine_head = pcb;
                        pcb_chaine_end = pcb;
                        pcb_flag = 1;

                    } else {
                        if (pcb_chaine_end != NULL) {
                            pcb_chaine_end->pid_sibling_next = pcb;
                            pcb_chaine_end = pcb;
                        }
                    }
                } else {
                    if (pcb) free(pcb);
                    if (statistics) free(statistics);
                    free_parsed_buffer(paresed_buffer);
                }
            } else if (paresed_buffer != NULL) {
                free_parsed_buffer(paresed_buffer);
            }
        } // <-- ADD THIS BRACE to close: if (char_count > 0) inside the outer if

    } // <-- ADD THIS BRACE to close: if (char_count > 0) outer block

    free(line_pcb); // after EOF free the line

    return pcb_chaine_head;
}
