#pragma once

#include "structs/process.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h> // for INT_MAX AND INT_MIN

// need_to_be_changed
char instructions_list[6][4] = {"aaa", "bbb", "ccc", "ddd", "eee", "fff"};
int instruction_list_len = 6;

typedef struct {
    PCB* head;
    int size;
} buffer_return;
// process_name,user_id,ppid,priority,[instruction],n_instruction,memoire,burst,
typedef struct parser_return{
    char process_name[20];
    char user_id[20];
    int priority;
    char** instructions;
    int instructions_count;
    int memoire;
    float burst;
    time_t temps_creation;
    bool unvalid_process_csv_check;
} parser_return;

void free_parsed_buffer(parser_return* parsed_buffer) {
    if (!parsed_buffer) return;
    free(parsed_buffer->process_name);
    free(parsed_buffer->user_id);
    for (int i = 0; i < parsed_buffer->instructions_count; i++) {
        free((parsed_buffer->instructions[i]));
    
    }
    free(parsed_buffer);
}

// buffer_return* extract_from_buffer(FILE* csv_buffer) {
    
//     int count = 0;

//     char line_pcb[70000];
//     while(fgets(line_pcb, sizeof(line_pcb), csv_buffer)) { // dont forget temps creation
//         parser_return* paresed_buffer = parser(line_pcb); // intializing the returned struct after parsing a line
//         PCB* pcb = (PCB*)malloc(sizeof(PCB)); // initializing the pcb
        
//         if (paresed_buffer == NULL) {
//             fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's has returned NULL %s\n", paresed_buffer->process_name);
//             exit(1);
//         }

//         // copy the process name
//         if (strlen(paresed_buffer->process_name) > 20) { // if the process name from parsed buffer > 20 (the allowed on pcb struct)
//             fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's process name has exceed 20 char %s\n", paresed_buffer->process_name);
//             free_parsed_buffer(paresed_buffer);
//             exit(1);
//         }
//         strncpy(pcb->process_name, paresed_buffer->process_name, sizeof(paresed_buffer->process_name) - 1); // copy just the size of process_name 
//         pcb->process_name[sizeof(pcb->process_name) - 1] = '\0'; // add null terminator


//         // copy the user_id
//         if (strlen(paresed_buffer->user_id) > 20) { // if the process name from parsed buffer > 20 (the allowed on pcb struct)
//             fprintf(stderr, "ERROR ON: parser function extract_from_buffer parsed buffer's user_id has exceed 20 char %s\n", paresed_buffer->user_id);
//             free_parsed_buffer(paresed_buffer);
//             exit(1);
//         }
//         strncpy(pcb->user_id, paresed_buffer->user_id, sizeof(paresed_buffer->user_id) - 1); // copy just the size of process_name 
//         pcb->user_id[sizeof(pcb->user_id) - 1] = '\0'; // add null terminator

//         // copy the priority
//         pcb->prioritie = paresed_buffer->priority;

//         // copy the instruction
//         pcb->instr

        
//     }
// }



typedef struct {
    char** instructions;
    int count;
} insruction_parser_return;


parser_return* parser(char* line) {
    parser_return* parsed_line = (parser_return*)malloc(sizeof(parser_return));

    int char_count = 0;
    int value_number = 0;
    char* value = (char*)malloc(sizeof(char)); // change it to dynamic allocation
    line[strlen(line)] = '\0';
    int line_length = strlen(line); // for time consuming
    if (line[line_length] != '\n' && line[line_length - 1] != '\0' && line[line_length - 1] != '\n') { // trust no one
        fprintf(stderr, "ERROR ON: parser function parser line gived is unvalid (probably will lead to loop infinite)\n", line);
        exit(1);
    }
    for(int i = 0; i < line_length;i++) { // line_size
        if (line[i] != ',') { // so it's a char of ressource
            value = (char*)realloc(value, (char_count * sizeof(char)) + 1); // extra 1 is for null terminator
            char_count++;
        } else if (line[i] == ',') {
            switch (value_number) {
                case 0:
                    if (strlen(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in process_name\n", line);
                        free(parsed_line); 
                        free(value); // free the value
                        exit(1);
                    }
                    strncpy(parsed_line->process_name, value, sizeof(parsed_line->process_name) - 1); // copy just the size of process_name and leave on char fo \0
                    parsed_line->process_name[sizeof(parsed_line->process_name) - 1] = '\0'; // add null terminator
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;
                case 1:
                    if (strlen(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in user_id\n", line);
                        free(parsed_line->process_name);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    strncpy(parsed_line->user_id, value, sizeof(parsed_line->user_id) - 1); // copy just the size of process_name 
                    parsed_line->user_id[sizeof(parsed_line->user_id) - 1] = '\0'; // add null terminator
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;    
                case 2:
                    long value_to_long = strtol(value, NULL, 10);
                    int val_int = 0;
                    // casting direct to int can make overflow o we check the limits
                    if (value_to_long > INT_MAX || value_to_long < INT_MIN) {
                        // if overflow
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority is ouuuut of range\n", line);
                        exit(1);
                    } else {
                        val_int = (int)value_to_long;
                    }

                    if (val_int > 5 || val_int < 1) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority out of range(1-5)\n", line);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    parsed_line->priority = val_int; // atoi stand for ascii to integer and located in stdlib; maybe make ours if we still have time
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;    
                case 3:
                    if (strlen(value) < 1 || strlen(value) > 60000) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions error\n", line);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    insruction_parser_return* parsed_instructions = instruction_parser(value);
                    if (parsed_instructions == NULL || parsed_instructions->instructions == NULL) { // NULLTY CHECKS
                        fprintf(stderr, "ERROR ON: parser line function, instruction_parser has returned a NULL value\n"); 
                        free(parsed_instructions->instructions);
                        free(parsed_instructions);  // setting free a null value is safe :-)
                        free(value); // free the value
                        exit(1);
                    }
                    parsed_line->instructions = parsed_instructions->instructions; // we assign the adress to the variable we'll return
                    parsed_line->instructions_count = parsed_instructions->count; // here we define the count to check later from csv 
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;
                case 4:
                    long ins_cnt_lng = strtol(value, NULL, 10);
                    int value__int = 0;
                    if (ins_cnt_lng > INT_MAX || ins_cnt_lng < INT_MIN) {
                        // if overflow case
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
                        free(value); // free the value
                        exit(1);
                    } else {
                        value__int = (int)ins_cnt_lng;
                    }

                    if (value__int == 0) {
                        fprintf(stderr, "ERROR ON: the parser function the instructions_count specified is invalid\n");
                        exit(1);
                    }
                    if (parsed_line->instructions_count != value__int) { // we check if the parsed ressource count are correct actualy this add a layer of validating
                        fprintf(stderr, "ERROR ON: the parser function the instructions_count specified in csv :%d doesn't equal to the counted by parser: %d\n", value__int, parsed_line->instructions_count);
                        for (int i = 0; i < parsed_line->instructions_count; i++) {
                            free(parsed_line->instructions[i]);
                        } 
                        free(parsed_line->instructions);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;
                case 5:
                    long memoire_lng = strtol(value, NULL, 10); // 10 for base deciaml
                    int value_int = 0;
                    if (memoire_lng > INT_MAX || memoire_lng < INT_MIN) {
                        // if overflow case
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions are out of range\n", line);
                        free(parsed_line->instructions);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    } else {
                        value_int = (int)memoire_lng;
                    }
                    if (value_int == 0) {
                        fprintf(stderr, "ERROR ON: the parser function the memoire specified is invalid\n");
                        for (int i = 0; i < parsed_line->instructions_count; i++) {
                            free(parsed_line->instructions[i]);
                        }
                        free(parsed_line->instructions);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    parsed_line->memoire = value_int;
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;
                case 6:
                    float burst = strtof(value, NULL);
                    if (burst == NULL) {
                        fprintf(stderr, "ERROR ON: the parser function the BURST specified is invalid\n");
                        for (int i = 0; i < parsed_line->instructions_count; i++) {
                            free(parsed_line->instructions[i]);
                        }
                        free(parsed_line->instructions);
                        free(parsed_line);
                        free(value); // free the value
                        exit(1);
                    }
                    parsed_line->unvalid_process_csv_check = false;
                    parsed_line->burst = burst;
                    value = (char*)realloc(value, sizeof(char)); // realloc the value to its original
                    break;
                default:
                    for (int i = 0; i < parsed_line->instructions_count; i++) {
                        free(parsed_line->instructions[i]);
                    }
                    free(parsed_line->instructions);
                    free(parsed_line);
                    free(value); // free the value
                    fprintf(stderr, "ERROR ON: the parser function the value_number has exceded the number of columns in csv file (protocol: 7) current:%d\n", value_number);
                    exit(1);
            }
            char_count = 0;
            value_number++;
            if (value_number < 7) {
                fprintf(stderr, "ERROR ON: parser function fields excedded in line %s", line);
                exit(1);
            }
            memset(value, 0, sizeof(value)); // setting all the bytes in memory will make it equivalent to empty string

            if (value_number > 6) {
                fprintf(stderr, "ERROR ON: parser function (csv file data unvalid)", line);
                exit(1);
            }
        } else if (line[i] == '\n') {
            if (value_number < 7) {
                fprintf(stderr, "ERROR ON: parser function not all the seven fields are present in line %s", line);
                exit(1);
            }
            break;
        } else {
            fprintf(stderr, "ERROR ON: parser function on line %s\nunknwon error", line);
            exit(1);
        }
    }
    return parsed_line;
}

bool check_known_ressource(char ressource[]) {
    for (int i = 0; i < instruction_list_len; i++) {
        int flag = 0;
        if (strcmp(instructions_list[i], ressource)) {
            flag = 1
        }
    }
    return (flag == 1) ? true : false;
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

    returned->instructions = (char**)malloc(20000 * sizeof(char*)); // 20000 instruction each instruction is a pointer to a string and has exactly 3caracters
    if (returned->instructions == NULL) {
        free(returned); // leakmemory eskive
        fprintf(stderr, "ERROR ON: instruction_parser function, dynamic allocation returned->instructions failed\n");
        exit(1);
    }

    returned->count = 0;
    char instruction[4] = {0}; // initializing it to prevent random value

    int instruction_char_count = 0;
    for (int i = 1; i < 60001; i++) {// instructions_count // initializing i to 1 bach na9zo hadak '['
        if (value[i] != ',' && instruction_char_count < 3) { // if value is a ressource character and we didnt arrive to the end which is 3characters
            instruction[instruction_char_count] = value[i]; // character at instruction retriving variable = fgets or instructions line char
            instruction_char_count++;
        } else if (value[i] == ',' && instruction_char_count == 3) { // if tge char in instructions line is comma and instruction_char_count is 3 mean that valid instruction variable so we have a ressource
            if (instruction_char_count != 3) { // ressource is more than 3 characters
                // "concurrence bagha la vendetta"
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
                free(returned->instructions); // liberer memoire
                free(returned); // liberer memoire
                exit(1);
            }
            returned->instructions[returned->count] = (char*)malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
            if (returned->instructions[returned->count] == NULL) {
                fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
                free(returned->instructions);
                free(returned);
                exit(1);
            }
            instruction[3] = '\0';

            // check if the ressource is a known ressource
            if (!check_known_ressource(instruction)) {
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\ninstruction %s is not allowed", value, instruction);
                free(returned->instructions);
                free(returned);
                exit(1);
            }

            strcpy(returned->instructions[returned->count], instruction); // copy the string to the allocated instruction but if the instruction is not ended with \0 strcpy will copy more exceeding the buffer
            returned->instructions[returned->count][3] = '\0';
            returned->count++;
            instruction_char_count = 0;
            instruction[0] = '\0'; // clearing the array
        } else if (value[i] == ']' || value[i] == '\0') { // didnt merge it with previous if for time, like ida zedt wahed l if (value[i] == ']')  ghayexecuteha bzf which is bad
            if (instruction_char_count != 3) { // ressource is more than 3 characters
                // "concurrence bagha la vendetta"
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
                free(returned->instructions); // liberer memoire
                free(returned); // liberer memoire
                exit(1);
            }
            returned->instructions[returned->count] = (char*)malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
            if (returned->instructions[returned->count] == NULL) {
                fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
                free(returned->instructions);
                free(returned);
                exit(1);
            }
            instruction[3] = '\0';
            strcpy(returned->instructions[returned->count], instruction); // copy the string to the allocated instruction
            returned->count++;
            break; // instead of setting char count to 0 break the loop and return the parsed instructions
        } else if (i == 60000) { // that why we make 60001 in the condition
            fprintf(stderr, "ERROR ON: instruction_parser the ] ending instruction never found\n");
            free(returned->instructions);
            free(returned);
            exit(1);
        } else {
            fprintf(stderr, "ERROR ON: instruction_parser function process line in csv \n '%s' unvalid instruction with unknwon error\n", value);
            // free the instructions then the list then returned
            for (int i = 0; i < returned->count; i++) {
                free(returned->instructions[i]);
            }
            free(returned->instructions);
            free(returned);
            exit(1);
        }
    }
    return returned;
}