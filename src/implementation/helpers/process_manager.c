#pragma once

#include "../../../lib/structs/process.h"
#include "../../../lib/structs/process_manager.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h> // for INT_MAX AND INT_MIN


// moke ressources need_to_be_changed if ressources changed
char instructions_list[6][4] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF"};
int instruction_list_len = 6;


typedef struct PARSED_RETURN {
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
} PARSED_RETURN;

typedef struct INSTRUCTION_PARSED_RETURN {
    INSTRUCTION* instructions_head;
    INSTRUCTION* instructions_fin; //adding it for time consuming
    int count;
} INSTRUCTION_PARSED_RETURN;


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

TASK check_known_ressource(char ressource[]) {
    int flag = 0;
    for (int i = 0; i < 6; i++) {
        if (strcmp(instructions_list[i], ressource) == 0) {
            flag = 1;
            break;
        }
    }

    if (flag == 1) 
        return TASK_SUCC;
    else
        return TASK_ERR;

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

// btw this list is for simulation purpose char instructions_list[6][4] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF"};
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

void free_parsed_buffer(PARSED_RETURN* paresed_buffer) {
    if (paresed_buffer) {
        free_instructions_chaine(paresed_buffer->instructions_head);
        free(paresed_buffer);
    }
}

// ---------------pricipale functions

// prototypes latb9a yaati error d return type unmatched like (char*)* chi haja hkk
PARSED_RETURN* parser_func(char* line);
INSTRUCTION_PARSED_RETURN* instruction_parser(char* value);




PCB* extract_from_buffer(PROCESS_MANAGER* self) {
    PCB* pcb_chaine_head = NULL;
    PCB* pcb_chaine_end = NULL;
    int pcb_flag = 0;
    int process_count = 0;
    
    size_t size = 128;
    char* line_pcb = (char*)malloc(size);
    if (line_pcb == NULL) {
        fprintf(stderr, "ERROR ON : extract_from_buffer Failed to allocate line buffer\n");
        exit(1);
    }
    
    printf("=== creating process table ===\n");
    
    while (fgets(line_pcb, size, self->get_processus_buffer(self)) != NULL) {
        // Remove newline
        line_pcb[strcspn(line_pcb, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(line_pcb) == 0) {
            continue;
        }
        
        process_count++;
        printf("processing line %d: %s\n", process_count, line_pcb);
        
        PARSED_RETURN* parsed_buffer = parser_func(line_pcb);
        
        if (parsed_buffer == NULL) {
            fprintf(stderr, "ERROR ON: parser_func returned NULL for line: %s\n", line_pcb);
            continue;
        }
        
        if (parsed_buffer->unvalid_process_csv_check) {
            fprintf(stderr, "WARN: Invalid CSV format on line: %s\n", line_pcb);
            free_parsed_buffer(parsed_buffer);
            continue;
        }
        
        // creating the pcb
        PCB* pcb = (PCB*)calloc(1, sizeof(PCB)); // usiing calloc to prevent valeur aleatoire
        PROCESS_STATISTICS* statistics = (PROCESS_STATISTICS*)calloc(1, sizeof(PROCESS_STATISTICS)); // same
        
        if (!pcb || !statistics) {
            fprintf(stderr, "ERROR ON: extract_from_buffer pcb or statistics allocation failed\n");
            if (pcb) free(pcb);
            if (statistics) free(statistics);
            free_parsed_buffer(parsed_buffer);
            continue;
        }

        // init
        pcb->pid = process_count; 
        // pcb->init(pcb);
        pcb->statistics = statistics;
        // copy the process name
        strncpy(pcb->process_name, parsed_buffer->process_name, 
                sizeof(pcb->process_name) - 1);
        pcb->process_name[sizeof(pcb->process_name) - 1] = '\0';
        
        // then user id
        strncpy(pcb->user_id, parsed_buffer->user_id, 
                sizeof(pcb->user_id) - 1);
        pcb->user_id[sizeof(pcb->user_id) - 1] = '\0';
        
        // rest fields
        pcb->prioritie = parsed_buffer->priority;
        pcb->instructions_head = parsed_buffer->instructions_head;
        parsed_buffer->instructions_head = NULL;
        pcb->programme_compteur = parsed_buffer->instructions_count;
        pcb->memoire_necessaire = parsed_buffer->memoire;
        pcb->burst_time = parsed_buffer->burst;
        pcb->remaining_time = parsed_buffer->burst;  // init remaining time
        
        // statistics
        pcb->statistics->temps_creation = parsed_buffer->temps_creation;
        pcb->statistics->temps_arrive = parsed_buffer->temps_arrive;
        pcb->statistics->temps_attente = 0.0f;  // init temps attente
        
        pcb->etat = PROCESS_NEW; // setting state needed in pushing from process table to ready queue
        pcb->pid_sibling_next = NULL; // needed af to prevent valeur aleatoire
        
        // free the parsed buffer
        free(parsed_buffer);
        
        // chaine the pcb
        if (pcb_chaine_head == NULL) {
            pcb_chaine_head = pcb;
            pcb_chaine_end = pcb;
        } else {
            pcb_chaine_end->pid_sibling_next = pcb;
            pcb_chaine_end = pcb;
        }
        
        printf("added PCB %d: %s (temps arrive: %f, burst: %f)\n", 
               pcb->pid, pcb->process_name, 
               pcb->statistics->temps_arrive, pcb->burst_time);
    }
    
    free(line_pcb);
    
    if (pcb_chaine_end != NULL) {
        pcb_chaine_end->pid_sibling_next = NULL;
    }
    
    printf("=== process table complete: %d processes ===\n\n", process_count);
    return pcb_chaine_head;
}

PARSED_RETURN* parser_func(char* line) {
    PARSED_RETURN* parsed_line = (PARSED_RETURN*)malloc(sizeof(PARSED_RETURN));
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

    for(int i = 0; i <= line_length; i++) {  // change it to <= because of end of file
        // if the char is not ,
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
        else // , or end of string
        {
            // check fields need_to_be_changed if fields changes
            if (value_number > 8) {
                fprintf(stderr, "ERROR ON: parser function too many fields in line %s\n", line);
                free(parsed_line);
                free(value);
                exit(1);
            }
            
            // // need_to_be_deleted Handle empty value (can happen with trailing comma)
            // if (char_count == 0) {
            //     // Empty field - might be trailing comma, skip it
            //     if (value_number >= 8) {
            //         // We have all required fields, ignore trailing empty field
            //         free(value);
            //         value = (char*)malloc(2);
            //         char_count = 0;
            //         value[0] = '\0';
            //         value_number++;
            //         continue;
            //     } else if (value_number == 7) {
            //         // Empty arrival time field - set to 0
            //         parsed_line->unvalid_process_csv_check = false;
            //         parsed_line->temps_arrive = 0.0f;
            //         free(value);
            //         value = (char*)malloc(2);
            //         char_count = 0;
            //         value[0] = '\0';
            //         value_number++;
            //         continue;
            //     }
            // }
            
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
                    INSTRUCTION_PARSED_RETURN* parsed_instructions = instruction_parser(value);
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
                    float temp_arrive = strtof(value, NULL);
                    parsed_line->temps_arrive = temp_arrive;
                    parsed_line->unvalid_process_csv_check = false; // last field is present and valid so valid process
                    break;
                    
                default:
                    free_instructions_chaine(parsed_line->instructions_head);
                    free(parsed_line);
                    free(value);
                    fprintf(stderr, "ERROR ON: the parser function the value_number has exceded the number of columns in csv file (protocol: 8) current:%d\n", value_number);
                    exit(1);
            }

            // reset all for next field
            free(value);
            value = (char*)malloc(2);
            char_count = 0;
            value[0] = '\0';
            value_number++;
            
            // break if end of string
            if (i == line_length) {
                break;
            } 
        }
    } 

    return parsed_line;
}



INSTRUCTION_PARSED_RETURN* instruction_parser(char* value) { // retrieve instruction name .. value is the instructions line
    if (value[0] == '\0' || value[0] != '[') { // we already checked NULLTY, check string hadi jsp ida kan khawi to make sure and check instruction line satts with '['
        fprintf(stderr, "ERROR ON: instruction parser check the validity of instruction line\n");
        exit(1);
    }
    INSTRUCTION_PARSED_RETURN* returned = (INSTRUCTION_PARSED_RETURN*)malloc(sizeof(INSTRUCTION_PARSED_RETURN));
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
            if (check_known_ressource(instruction) == TASK_ERR) {
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
                free(new_node);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }

            // increase the instructions_count by 1
            returned->count++;

            // init node
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
            if (check_known_ressource(instruction) == TASK_ERR) {
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
                free(new_node);
                free_instructions_chaine(returned->instructions_head); // liberer memoire
                free(returned);
                exit(1);
            }

            // increase the instructions_count by 1
            returned->count++;

            // init node
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