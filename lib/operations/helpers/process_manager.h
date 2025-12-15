#pragma once

#include "structs/process.h"

#include <stdbool.h>

// need_to_be_changed
char instructions_list[6] = ["aaa", "bbb", "ccc", "ddd", "eee", "fff"];

typedef struct {
    PCB* head;
    int size;
} buffer_return;


typedef struct {
    int pid;
    char process_name[20];
    char user_id[20];
    int ppid;
    E_etat etat; // one of the values in the enum e_etat
    int prioritie; // priorities from 1 to 5 ; 1-critical

    INSTRUCTION* instructions;
    long programme_compteur; // max 9,223,372,036,854,775,807 instruction
    int memoire_necessaire; // en MB
    INSTRUCTION* current_instruction; // l'instruction en train de s executer

    int burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    float cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    int cpu_usage; // initialized as 0

    PROCESS_STATISTICS* statistics; 

    int* pid_childrens;
    PCB* pid_sibling_next; // pointeur vers next sib
    PCB* pid_sibling_previous; // pointeur vers previous sib
} PCB;

typedef struct {
    uint32_t* instruct_id; // the id of instruction because process can stop at it if need ressources, pointer because it can be too long
    PCB* process; // the id of the process owner

    float time_remaining; // in nano seconds
    RESSOURCE_ELEMENT* type; // type of instruction which is ressource needed
    INSTRUCTION_STATE* state; // state of instruction 
    INSTRUCTION* next;
} INSTRUCTION;

buffer_return* extract_from_buffer(FILE* csv_buffer) {
    
    int count = 0;

    char line_pcb[70000];
    while(fgets(line_pcb, sizeof(buffer), csv_buffer)) { // dont forget temps creation
        

        PCB pcb = {
            count,

        }
    }
}





// process_name,user_id,ppid,priority,[instruction],n_instruction,memoire,burst,
typedef struct {
    char process_name[20];
    char user_id[20];
    int priority;
    char** instructions;
    int instructions_count;
    int memoire;
    float burst;
    bool unvalid_process;
} parser_return;

typedef struct {
    INSTRUCTION* head;
    int size;
} insruction_parser_return;

parser_return* parser(char* line) {
    parser_return parsed_line = {};

    int loop = 0; 
    int char_count = 0;
    int value_number = 0;
    char value[100];
    for(int i = 0; i < 10000;i++) { // line_size
        if (line[i] != ',') {
            value[char_count] = line[i];
            char_count++;
        } else if (line[i] == ',') {
            switch (value) {
                case 0:
                    if (sizeof(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in process_name\n", line);
                        exit(1);
                    }
                    (value > 20) ? 
                    parsed_line->process_name = value;
                    break;    
                case 1:
                    if (sizeof(value) > 20) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \nhas exceded 20 caracter in user_id\n", line);
                        exit(1);
                    }
                    parsed_line->user_id = value;
                    break;    
                case 2:
                    if ((int)strtol(value, NULL, 10) > 5 || (int)strtol(value, NULL, 10) < 1) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \npriority out of range(1-5)\n", line);
                        exit(1);
                    }
                    parsed_line->priority = (int)strtol(value, NULL, 10); // atoi stand for ascii to integer and located in stdlib; maybe make ours if we still have time
                    break;    
                case 3:
                    if (sizeof(value) < 1) {
                        fprintf(stderr, "ERROR ON: parser function process line in csv '%s' \ninstructions error\n", line);
                        parsed_line->unvalid_process = true;
                        exit(1);
                    }
                    insruction_parser_return* parsed_instructions = instruction_parser(value);
                    
            }

            char_count = 0;
            value_number++;
            memset(value, 0, sizeof(value)); // setting all the bytes in memory will make it equivalent to empty string

            if (value_number > 6) {
                fprintf(stderr, "ERROR ON: parser function (csv file data unvalid)", line);
                exit(1);
            }
        } else if (line[i] == '\n') {
            break;
        } else {
            fprintf(stderr, "ERROR ON: parser function", line);
            exit(1);
        }
    }
    return parsed_line;
}


typedef struct {
    char** instructions;
    int count;
} insruction_parser_return;

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
    for (int i = 1; i < 20001; i++) {// instructions_count // initializing i to 1 bach na9zo hadak '['
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
            returned->instructions[returned->count] = malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
            if (returned->instructions[returned->count] == NULL) {
                fprintf(stderr, "ERROR ON: instruction_parser failed allocating the instruction\n");
                free(returned->instructions);
                free(returned);
                exit(1);
            }
            instruction[3] = '\0';
            strcpy(returned->instructions[returned->count], instruction); // copy the string to the allocated instruction but if the instruction is not ended with \0 strcpy will copy more exceeding the buffer
            returned->instructions[returned->count][3] = '\0';
            returned->count++;
            instruction_char_count = 0;
            instruction[0] = "\0"; // clearing the array
        } else if (value[i] == ']') { // didnt merge it with previous if for time, like ida zedt wahed l if (value[i] == '[')  ghayexecuteha bzf which is bad
            if (instruction_char_count != 3) { // ressource is more than 3 characters
                // "concurrence bagha la vendetta"
                fprintf(stderr, "ERROR ON: instruction_parser failed at line %s\nan instruction %s with length %d is more than allowed", value, instruction, instruction_char_count);
                free(returned->instructions); // liberer memoire
                free(returned); // liberer memoire
                exit(1);
            }
            returned->instructions[returned->count] = malloc(4 * sizeof(char));// chwiiiiiiiya 3la lbufferoverflow, 3 + \0 null terminator
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
        } else if (i == 20000) { // that why we make 20001 in the condition
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