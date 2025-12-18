#pragma once

#include "structs/process.h"
#include "structs/process_manager.h"

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
    time_t temps_creation;
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
    return (flag == 1) ? true : false;
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
            
            // assign statistics to it's pcb
            pcb->statistics = statistics;

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
                    // pcb->statistics->temps_arrive = paresed_buffer->temps_arrive;
                    pcb->statistics->temps_creation = paresed_buffer->temps_creation;
                    pcb->statistics->temps_arrive = paresed_buffer->temps_arrive;
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
        }
    }

    free(line_pcb); // after EOF free the line

    return pcb_chaine_head;
}



   
// parser_return* parser_func(char* line) {

//     parser_return* parsed_line = (parser_return*)malloc(sizeof(parser_return));
//     // declaring all fields for random valeur
//     if (parsed_line) {
//         parsed_line->process_name[0] = '\0';
//         parsed_line->user_id[0] = '\0';
//         parsed_line->priority = 0;
//         parsed_line->instructions_head = NULL;
//         parsed_line->instructions_count = 0;
//         parsed_line->memoire = 0;
//         parsed_line->burst = 0.0f;
//         parsed_line->temps_creation = time(NULL);
//         parsed_line->temps_arrive = 0.0f;
//         parsed_line->unvalid_process_csv_check = true;
//     }

//     int char_count = 0; 
//     int value_number = 0;
//     char* value = (char*)malloc(2); // 2 for null terminator
//     int line_length = strlen(line); // for time consuming
//     value[0] = '\0';
//     // if line is empty
//     if (line_length == 0) { 
//         fprintf(stderr, "ERROR ON: parser function parser line gived is empty\n");
//         free(parsed_line);
//         free(value);
//         exit(1);
//     }
//     for(int i = 0; i < line_length;i++) { // line_size
//         // wh9 lah ta chi 6h bach nl9a had condition 2 mn baad || hia li kadir lia mochkil flparsing dial instructions
//         // chi 30min bared hd lcomment li lfo9 ead l9it bli f csv l instructions kandirom hkk [AAA,BBB] warah l condition ghatl9a , w ghatbreaki wtf 
//         if (line[i] != ',' ) { // so it's a char of ressource
//             char_count++;
//             // realloc check
//             char* value_temp = (char*)realloc(value, char_count + 1); // extra 1 is for null terminator
//             if (!value_temp) {
//                 fprintf(stderr, "ERROR ON: parser function realloc failed\n");
//                 free(parsed_line);
//                 free(value);
//                 exit(1);
//             }
//             value = value_temp;
//             value[char_count - 1] = line[i];
//             value[char_count] = '\0';

//         } else if (line[i] == ',') {
//             if (value_number > 8) {
//                     fprintf(stderr, "ERROR ON: parser function not all the seven fields are present in line %s", line);
//                     free(parsed_line);
//                     free(value); // Free value here too
//                     exit(1);
//             }
//             switch (value_number) {
//                 case 0:
//                     if (strlen(value) > 20) {
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in process_name\n", line);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     strncpy(parsed_line->process_name, value, sizeof(parsed_line->process_name) - 1); // copy just the size of process_name and leave on char fo \0
//                     parsed_line->process_name[sizeof(parsed_line->process_name) - 1] = '\0'; // add null terminator
//                     // free and reallocate
//                     free(value);
//                     value = (char*)malloc(2); // Allocate 2 for null terminator
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;
//                 case 1:
//                     if (strlen(value) > 20) {
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in user_id\n", line);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     strncpy(parsed_line->user_id, value, sizeof(parsed_line->user_id) - 1); // copy just the size of process_name 
//                     parsed_line->user_id[sizeof(parsed_line->user_id) - 1] = '\0'; // add null terminator
//                     // Free and reallocate
//                     free(value);
//                     value = (char*)malloc(2); // Allocate 2 for null
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;    
//                 case 2:
//                     long value_to_long = strtol(value, NULL, 10);
//                     int val_int = 0;
//                     // casting direct to int can make overflow o we check the limits
//                     if (value_to_long > INT_MAX || value_to_long < INT_MIN) {
//                         // if overflow
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority is ouuuut of range\n", line);
//                         free(parsed_line);
//                         free(value); // if error free value
//                         exit(1);
//                     } else {
//                         val_int = (int)value_to_long;
//                     }

//                     if (val_int > 5 || val_int < 1) {
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority out of range(1-5)\n", line);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     parsed_line->priority = val_int; // atoi stand for ascii to integer and located in stdlib; maybe make ours if we still have time
//                     free(value);
//                     value = (char*)malloc(2);
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;    
//                 case 3:
//                     if (strlen(value) < 1 || strlen(value) > 60000) {
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions error\n", line);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     insruction_parser_return* parsed_instructions = instruction_parser(value);
//                     if (parsed_instructions == NULL || parsed_instructions->instructions_head == NULL) { // NULLTY CHECK
//                         fprintf(stderr, "ERROR ON: parser line function, instruction_parser has returned a NULL value\n"); 
//                         if (parsed_instructions) {
//                             free_instructions_chaine(parsed_instructions->instructions_head);
//                             free(parsed_instructions);
//                         }
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     parsed_line->instructions_head = parsed_instructions->instructions_head; // we assign the adress to the variable we'll return
//                     parsed_line->instructions_count = parsed_instructions->count; // here we define the count to check later from csv 
//                     // free parsed after extracting 
//                     free(parsed_instructions);
//                     // Free and reallocate 
//                     free(value);
//                     value = (char*)malloc(2); 
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;
//                 case 4:
//                     long ins_cnt_lng = strtol(value, NULL, 10);
//                     int value__int = 0;
//                     if (ins_cnt_lng > INT_MAX || ins_cnt_lng < INT_MIN) {
//                         // if overflow case
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
//                         free_instructions_chaine(parsed_line->instructions_head);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     } else {
//                         value__int = (int)ins_cnt_lng;
//                     }

//                     if (value__int == 0) {
//                         fprintf(stderr, "ERROR ON: the parser function the instructions_count specified is invalid\n");
//                         free_instructions_chaine(parsed_line->instructions_head);
//                         free(parsed_line);
//                         free(value);
//                         exit(1);
//                     }
//                     if (parsed_line->instructions_count != value__int) { // we check if the parsed ressource count are correct actualy this add a layer of validating
//                         fprintf(stderr, "ERROR ON: the parser function the instructions_count specified in csv :%d doesn't equal to the counted by parser: %d\n", value__int, parsed_line->instructions_count);
//                         free_instructions_chaine(parsed_line->instructions_head);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     free(value);
//                     value = (char*)malloc(2);
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;
//                 case 5:
//                     long memoire_lng = strtol(value, NULL, 10); // 10 for base deciaml
//                     int value_int = 0;
//                     if (memoire_lng > INT_MAX || memoire_lng < INT_MIN) {
//                         // if overflow case
//                         fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
//                         free_instructions_chaine(parsed_line->instructions_head);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     } else {
//                         value_int = (int)memoire_lng;
//                     }
//                     if (value_int == 0) {
//                         fprintf(stderr, "ERROR ON: the parser function the memoire specified is invalid\n");
//                         free_instructions_chaine(parsed_line->instructions_head);                        
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     parsed_line->memoire = value_int;
//                     free(value);
//                     value = (char*)malloc(2);
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;
//                 case 6:
//                     float burst = strtof(value, NULL);
//                     if (burst == 0) {
//                         fprintf(stderr, "ERROR ON: the parser function the BURST specified is invalid\n");
//                         free_instructions_chaine(parsed_line->instructions_head);
//                         free(parsed_line);
//                         free(value); // free the value
//                         exit(1);
//                     }
//                     parsed_line->burst = burst;
//                     parsed_line->temps_creation = time(NULL); // spawn creation time
//                     free(value);
//                     value = (char*)malloc(2);
//                     char_count = 0;
//                     value[0] = '\0';
//                     break;
//                 case 7:
//                     printf("DEBUG: Parsing arrival time from value: '%s'\n", value);
//                     float temp_arrive = strtof(value, NULL);
//                     printf("DEBUG: Parsed arrival time: %f\n", temp_arrive);
//                     // if (temp_arrive == 0) {
//                     //     fprintf(stderr, "ERROR ON: the parser function temps arrive is invalid\n");
//                     //     free_instructions_chaine(parsed_line->instructions_head);
//                     //     free(parsed_line);
//                     //     free(value); // free the value
//                     //     exit(1);
//                     // }
//                     parsed_line->unvalid_process_csv_check = false;
//                     parsed_line->temps_arrive = temp_arrive;

//                     free(value);
//                     value = (char*)malloc(2);
//                     char_count = 0;
//                     value[0] = '\0';    
//                     printf("DEBUG: Parsed arrival time: %f for process %s\n", parsed_line->temps_arrive, parsed_line->process_name);
//                     break;
//                 default:
//                     free_instructions_chaine(parsed_line->instructions_head);
//                     free(parsed_line);
//                     free(value); // free the value
//                     fprintf(stderr, "ERROR ON: the parser function the value_number has exceded the number of columns in csv file (protocol: 7) current:%d\n", value_number);
//                     exit(1);
//             }
//             // si + 8 rah field exceeded
//             if (value_number > 8) {
//                 fprintf(stderr, "ERROR ON: parser function fields excedded in line %s", line);
//                 if (value) free(value);
//                 free_instructions_chaine(parsed_line->instructions_head);
//                 free(parsed_line);
//                 free(value);
//                 exit(1);
//             }
//             value_number++;
//         } else {
//             fprintf(stderr, "ERROR ON: parser function on line %s\nunknwon error", line);
//             free(parsed_line);
//             free(value);
//             exit(1);
//         }
//     }

//     // if there is missing field
//     if (char_count > 0 && value_number < 8) {
//         // missing temps arrive
//         if (value_number == 7) {
//             float temps_arrive = strtof(value, NULL);
//             // if (temps_arrive == 0) {
//             //     fprintf(stderr, "ERROR ON: the parser function the BURST specified is invalid\n");
//             //     free_instructions_chaine(parsed_line->instructions_head);
//             //     free(parsed_line);
//             //     free(value);
//             //     exit(1);
//             // }
//             parsed_line->unvalid_process_csv_check = false;
//             parsed_line->temps_arrive = temps_arrive;
//             // parsed_line->temps_creation = time(NULL);
//             value_number++;
//         } else {
//             fprintf(stderr, "ERROR ON: parser function incomplete line, got %d fields need 8\n", value_number);
//             free_instructions_chaine(parsed_line->instructions_head);
//             free(parsed_line);
//             free(value);
//             exit(1);
//         }
//     }
    
//     // make sure we have all 8 fields
//     if (value_number > 8) {
//         fprintf(stderr, "ERROR ON: parser function incomplete line, got %d fields need 7 in %s\n", value_number, value);
//         free_instructions_chaine(parsed_line->instructions_head);
//         free(parsed_line);
//         if (value) free(value);
//         exit(1);
//     }
    
//     if (value) free(value);
    
//     return parsed_line;
// }

parser_return* parser_func(char* line) {
    parser_return* parsed_line = (parser_return*)malloc(sizeof(parser_return));
    // Initialize all fields
    if (parsed_line) {
        parsed_line->process_name[0] = '\0';
        parsed_line->user_id[0] = '\0';
        parsed_line->priority = 0;
        parsed_line->instructions_head = NULL;
        parsed_line->instructions_count = 0;
        parsed_line->memoire = 0;
        parsed_line->burst = 0.0f;
        parsed_line->temps_creation = time(NULL);
        parsed_line->temps_arrive = 0.0f;
        parsed_line->unvalid_process_csv_check = true;
    }

    int char_count = 0; 
    int value_number = 0;
    char* value = (char*)malloc(2);
    int line_length = strlen(line);
    value[0] = '\0';
    
    if (line_length == 0) { 
        fprintf(stderr, "ERROR ON: parser function parser line gived is empty\n");
        free(parsed_line);
        free(value);
        exit(1);
    }

    for(int i = 0; i <= line_length; i++) {  // Changed to <= to handle end of string
        // Check if we reached a comma OR the end of string
        if (i < line_length && line[i] != ',') {
            char_count++;
            char* value_temp = (char*)realloc(value, char_count + 1);
            if (!value_temp) {
                fprintf(stderr, "ERROR ON: parser function realloc failed\n");
                free(parsed_line);
                free(value);
                exit(1);
            }
            value = value_temp;
            value[char_count - 1] = line[i];
            value[char_count] = '\0';
        } 
        else // We hit a comma OR end of string
        {
            // Process the accumulated value
            if (value_number > 8) {
                fprintf(stderr, "ERROR ON: parser function too many fields in line %s\n", line);
                free(parsed_line);
                free(value);
                exit(1);
            }
            
            // Handle empty value (can happen with trailing comma)
            if (char_count == 0) {
                // Empty field - might be trailing comma, skip it
                if (value_number >= 8) {
                    // We have all required fields, ignore trailing empty field
                    free(value);
                    value = (char*)malloc(2);
                    char_count = 0;
                    value[0] = '\0';
                    value_number++;
                    continue;
                } else if (value_number == 7) {
                    // Empty arrival time field - set to 0
                    parsed_line->unvalid_process_csv_check = false;
                    parsed_line->temps_arrive = 0.0f;
                    free(value);
                    value = (char*)malloc(2);
                    char_count = 0;
                    value[0] = '\0';
                    value_number++;
                    continue;
                }
            }
            
            switch (value_number) {
                case 0:
                    if (strlen(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in process_name\n", line);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    strncpy(parsed_line->process_name, value, sizeof(parsed_line->process_name) - 1);
                    parsed_line->process_name[sizeof(parsed_line->process_name) - 1] = '\0';
                    break;
                    
                case 1:
                    if (strlen(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in user_id\n", line);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    strncpy(parsed_line->user_id, value, sizeof(parsed_line->user_id) - 1);
                    parsed_line->user_id[sizeof(parsed_line->user_id) - 1] = '\0';
                    break;
                    
                case 2:
                    long value_to_long = strtol(value, NULL, 10);
                    int val_int = 0;
                    if (value_to_long > INT_MAX || value_to_long < INT_MIN) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority is ouuuut of range\n", line);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    } else {
                        val_int = (int)value_to_long;
                    }
                    if (val_int > 5 || val_int < 1) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority out of range(1-5)\n", line);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    parsed_line->priority = val_int;
                    break;
                    
                case 3:
                    if (strlen(value) < 1 || strlen(value) > 60000) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions error\n", line);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    insruction_parser_return* parsed_instructions = instruction_parser(value);
                    if (parsed_instructions == NULL || parsed_instructions->instructions_head == NULL) {
                        fprintf(stderr, "ERROR ON: parser line function, instruction_parser has returned a NULL value\n"); 
                        if (parsed_instructions) {
                            free_instructions_chaine(parsed_instructions->instructions_head);
                            free(parsed_instructions);
                        }
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    parsed_line->instructions_head = parsed_instructions->instructions_head;
                    parsed_line->instructions_count = parsed_instructions->count;
                    free(parsed_instructions);
                    break;
                    
                case 4:
                    long ins_cnt_lng = strtol(value, NULL, 10);
                    int value__int = 0;
                    if (ins_cnt_lng > INT_MAX || ins_cnt_lng < INT_MIN) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
                        free_instructions_chaine(parsed_line->instructions_head);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    } else {
                        value__int = (int)ins_cnt_lng;
                    }
                    if (value__int == 0) {
                        fprintf(stderr, "ERROR ON: the parser function the instructions_count specified is invalid\n");
                        free_instructions_chaine(parsed_line->instructions_head);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    if (parsed_line->instructions_count != value__int) {
                        fprintf(stderr, "ERROR ON: the parser function the instructions_count specified in csv :%d doesn't equal to the counted by parser: %d\n", 
                                value__int, parsed_line->instructions_count);
                        free_instructions_chaine(parsed_line->instructions_head);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    break;
                    
                case 5:
                    long memoire_lng = strtol(value, NULL, 10);
                    int value_int = 0;
                    if (memoire_lng > INT_MAX || memoire_lng < INT_MIN) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
                        free_instructions_chaine(parsed_line->instructions_head);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    } else {
                        value_int = (int)memoire_lng;
                    }
                    if (value_int == 0) {
                        fprintf(stderr, "ERROR ON: the parser function the memoire specified is invalid\n");
                        free_instructions_chaine(parsed_line->instructions_head);                        
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    parsed_line->memoire = value_int;
                    break;
                    
                case 6:
                    float burst = strtof(value, NULL);
                    if (burst == 0) {
                        fprintf(stderr, "ERROR ON: the parser function the BURST specified is invalid\n");
                        free_instructions_chaine(parsed_line->instructions_head);
                        free(parsed_line);
                        free(value);
                        exit(1);
                    }
                    parsed_line->burst = burst;
                    break;
                    
                case 7:
                    // This is the arrival time field
                    float temp_arrive = strtof(value, NULL);
                    parsed_line->unvalid_process_csv_check = false;
                    parsed_line->temps_arrive = temp_arrive;
                    break;
                    
                default:
                    free_instructions_chaine(parsed_line->instructions_head);
                    free(parsed_line);
                    free(value);
                    fprintf(stderr, "ERROR ON: the parser function the value_number has exceded the number of columns in csv file (protocol: 8) current:%d\n", value_number);
                    exit(1);
            }
            
            // Reset for next field
            free(value);
            value = (char*)malloc(2);
            char_count = 0;
            value[0] = '\0';
            value_number++;
            
            // If we're at the end of string, break
            if (i == line_length) break;
        }
    }

    // Check if we got all required fields (8 fields)
    if (value_number < 8) {
        fprintf(stderr, "ERROR ON: parser function incomplete line, got %d fields need 8\n", value_number);
        free_instructions_chaine(parsed_line->instructions_head);
        free(parsed_line);
        free(value);
        exit(1);
    }
    
    if (value) free(value);
    
    return parsed_line;
}

insruction_parser_return* instruction_parser(char* value) { // retrieve instruction name .. value is the instructions line
    if (value[0] == '\0' || value[0] != '[') { // we already checked NULLTY, check string hadi jsp ida kan khawi to make sure and check instruction line satts with '['
        fprintf(stderr, "ERROR ON: instruction parser check the validity of instruction line\n");
        exit(1);
    }
    insruction_parser_return* returned = (insruction_parser_return*)malloc(sizeof(insruction_parser_return));
    if (returned == NULL) {
        fprintf(stderr, "ERROR ON: instruction_parser function, dynamic allocation returned failed\n");
        exit(1);
    }

    // declare all fields
    returned->instructions_head = NULL;
    returned->instructions_fin = NULL;
    returned->count = 0;

  

    returned->count = 0;
    char instruction[4] = {0}; // initializing it to prevent random value

    int instruction_char_count = 0;
    for (int i = 1; i < 60001; i++) {// instructions_count // initializing i to 1 bach na9zo hadak '['
      

        if (value[i] == '[') {
            continue;
        } else if (value[i] != '.' && value[i] != ']' && instruction_char_count < 3) { // if value is a ressource character and we didnt arrive to the end which is 3characters
            instruction[instruction_char_count] = value[i]; // character at instruction retriving variable = fgets or instructions line char
            instruction_char_count++;
        } else if (value[i] == '.') { // if tge char in instructions line is comma and instruction_char_count is 3 mean that valid instruction variable so we have a ressource
           
            if (instruction_char_count != 3) { // ressource is more than 3 characters
                // "concurrence bagha la vendetta"
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned); // liberer memoire
                exit(1);
            }
           
            INSTRUCTION* new_node = (INSTRUCTION*)malloc(sizeof(INSTRUCTION)); // allocate an instruction in next to fill it
            if (new_node == NULL) {
                fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }
            
            instruction[3] = '\0';

            // check if the ressource is a known ressource
            if (check_known_ressource(instruction) == false) {
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
                free(new_node);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }

            // increase the instructions_count by 1
            returned->count++;

            // Initialize new node
            new_node->state = NOT_STARTED;
            new_node->next = NULL;
            
            // add it to chaine
            INSTRUCTION* end = add_instruction_type(returned->count, new_node, instruction);

            // adding if head
            if (returned->instructions_head == NULL) {
                returned->instructions_head = end;
                returned->instructions_fin = end;
            } else {
                returned->instructions_fin->next = end;
                returned->instructions_fin = end;
            }

            // make instructions char index 0
            instruction_char_count = 0;
            memset(instruction, 0, sizeof(instruction)); // clearing the array

        } else if (value[i] == ']') { // didnt merge it with previous if for time, like ida zedt wahed l if (value[i] == ']')  ghayexecuteha bzf which is bad
            
            if (instruction_char_count != 3) { // ressource is more than 3 characters
                // "concurrence bagha la vendetta"
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned); // liberer memoire
                exit(1);
            }
           
            INSTRUCTION* new_node = (INSTRUCTION*)malloc(sizeof(INSTRUCTION)); // allocate an instruction in next to fill it
            if (new_node == NULL) {
                fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }
            
            instruction[3] = '\0';

            // check if the ressource is a known ressource
            if (check_known_ressource(instruction) == false) {
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
                free(new_node);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }
            
            // increase the instructions_count by 1
            returned->count++;

            // Initialize new node
            new_node->state = NOT_STARTED;
            new_node->next = NULL;
            
            // add it to chaine
            INSTRUCTION* end = add_instruction_type(returned->count, new_node, instruction);

            if (returned->instructions_head == NULL) {
                returned->instructions_head = end;
                returned->instructions_fin = end;
            } else {
                returned->instructions_fin->next = end;
                returned->instructions_fin = end;
            }

            // make instructions char index 0
            instruction_char_count = 0;
          
            break; // instead of setting char count to 0 break the loop and return the parsed instructions
       
        } else if (i == 60000) { // that why we make 60001 in the condition
            fprintf(stderr, "ERROR ON: instruction_parser the ] ending instruction never found\n");
            free_instructions_chaine(returned->instructions_head);
            free(returned);
            exit(1);
        
        } else {
            fprintf(stderr, "ERROR ON: instruction_parser function process line in csv \n '%s' unvalid instruction with unknwon error %s\n", value, instruction);
            // free the instructions then the list then returned
            free_instructions_chaine(returned->instructions_head);
            free(returned);
            exit(1);
        }
    }
    
    // at least one instruction if list not empty
    if (value[1] != ']' && returned->count == 0) {
        fprintf(stderr, "ERROR ON: instruction_parser no instructions found in non-empty list\n");
        free_instructions_chaine(returned->instructions_head);
        free(returned);
        exit(1);
    }
    
    return returned;
}



// old without chained list 
// insruction_parser_return* instruction_parser(char* value) { // retrieve instruction name .. value is the instructions line
//     if (value[0] == '\0' || value[0] != '[') { // we already checked NULLTY, check string hadi jsp ida kan khawi to make sure and check instruction line satts with '['
//         fprintf(stderr, "ERROR ON: instruction parser check the validity of instruction line\n");
//         exit(1);
//     }
//     insruction_parser_return* returned = (insruction_parser_return*)malloc(sizeof(insruction_parser_return));
//     if (returned == NULL) {
//         fprintf(stderr, "ERROR ON: instruction_parser function, dynamic allocation returned failed\n");
//         exit(1);
//     }

//     returned->instructions = (char**)malloc(20000 * sizeof(char*)); // 20000 instruction each instruction is a pointer to a string and has exactly 3caracters
//     if (returned->instructions == NULL) {
//         free(returned); // leakmemory eskive
//         fprintf(stderr, "ERROR ON: instruction_parser function, dynamic allocation returned->instructions failed\n");
//         exit(1);
//     }

//     returned->count = 0;
//     char instruction[4] = {0}; // initializing it to prevent random value

//     int instruction_char_count = 0;
//     for (int i = 1; i < 60001; i++) {// instructions_count // initializing i to 1 bach na9zo hadak '['
//         if (value[i] != ',' && instruction_char_count < 3) { // if value is a ressource character and we didnt arrive to the end which is 3characters
//             instruction[instruction_char_count] = value[i]; // character at instruction retriving variable = fgets or instructions line char
//             instruction_char_count++;
//         } else if (value[i] == ',' && instruction_char_count == 3) { // if tge char in instructions line is comma and instruction_char_count is 3 mean that valid instruction variable so we have a ressource
//             if (instruction_char_count != 3) { // ressource is more than 3 characters
//                 // "concurrence bagha la vendetta"
//                 fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
//                 free(returned->instructions); // liberer memoire
//                 free(returned); // liberer memoire
//                 exit(1);
//             }
//             returned->instructions[returned->count] = (char*)malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
//             if (returned->instructions[returned->count] == NULL) {
//                 fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
//                 free(returned->instructions);
//                 free(returned);
//                 exit(1);
//             }
//             instruction[3] = '\0';

//             // check if the ressource is a known ressource
//             if (check_known_ressource(instruction) == false) {
//                 fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
//                 free(returned->instructions);
//                 free(returned);
//                 exit(1);
//             }

//             strcpy(returned->instructions[returned->count], instruction); // copy the string to the allocated instruction but if the instruction is not ended with \0 strcpy will copy more exceeding the buffer
//             returned->instructions[returned->count][3] = '\0';
//             returned->count++;
//             instruction_char_count = 0;
//             instruction[0] = '\0'; // clearing the array
//         } else if (value[i] == ']' || value[i] == '\0') { // didnt merge it with previous if for time, like ida zedt wahed l if (value[i] == ']')  ghayexecuteha bzf which is bad
//             if (instruction_char_count != 3) { // ressource is more than 3 characters
//                 // "concurrence bagha la vendetta"
//                 fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
//                 free(returned->instructions); // liberer memoire
//                 free(returned); // liberer memoire
//                 exit(1);
//             }
//             returned->instructions[returned->count] = (char*)malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
//             if (returned->instructions[returned->count] == NULL) {
//                 fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
//                 free(returned->instructions);
//                 free(returned);
//                 exit(1);
//             }
//             instruction[3] = '\0';
//             strcpy(returned->instructions[returned->count], instruction); // copy the string to the allocated instruction
//             returned->count++;
//             break; // instead of setting char count to 0 break the loop and return the parsed instructions
//         } else if (i == 60000) { // that why we make 60001 in the condition
//             fprintf(stderr, "ERROR ON: instruction_parser the ] ending instruction never found\n");
//             free(returned->instructions);
//             free(returned);
//             exit(1);
//         } else {
//             fprintf(stderr, "ERROR ON: instruction_parser function process line in csv \n '%s' unvalid instruction with unknwon error\n", value);
//             // free the instructions then the list then returned
//             for (int i = 0; i < returned->count; i++) {
//                 free(returned->instructions[i]);
//             }
//             free(returned->instructions);
//             free(returned);
//             exit(1);
//         }
//     }
//     return returned;
// }