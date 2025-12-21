#pragma once

#include "structs/ressource_manager.h"
#include "structs/ressource.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define ressource_number 6

// on start
RESSOURCE_ELEMENT* op_create_ressource_list(void) {
    RESSOURCE_ELEMENT* ressources_head = NULL;
    RESSOURCE_ELEMENT* last = NULL; // for the loop

    char* ressource_names[] = {"AAA", "BBB", "CCC", "DDD", "EEE", "FFF"};

    for (int i = 0; i < ressource_number; i++) {

        RESSOURCE_ELEMENT* node = (RESSOURCE_ELEMENT*)malloc(sizeof(RESSOURCE_ELEMENT));

        if (node == NULL) {  // CHECK FAIL
            printf("ERROR ON: op_create_ressource_list failed to allocate new node");
            // if a node has failed the ressource head should be freed
            while(ressources_head != NULL) {
                RESSOURCE_ELEMENT* temp = ressources_head;
                ressources_head = ressources_head->next_ressource;
                free(temp);
            }
            exit(1);
        }

        node->ressource = (RESSOURCE)i; // Direct cast is cleaner
        strcpy(node->ressource_name, ressource_names[i]);
        node->disponibilite = true; // true when ylh created
        node->next_ressource = NULL;

        if (ressources_head == NULL) {
            // If the list is empty, this node is the head
            ressources_head = node;
            last = node;
        } else {
            // Otherwise, append to the end of the list
            last->next_ressource = node;
            last = node;
        }
    }

    return ressources_head;
}


// ressources operations
RESSOURCE_ELEMENT* op_look_for_ressource_in_list(RESSOURCE_ELEMENT* ressources_head, RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = ressources_head;

    while (head != NULL) {
        if (head->ressource == ressource) {
            return head;
        }
        head = head->next_ressource;
    }

    return NULL;
}

bool op_mark_ressource_available(RESSOURCE_ELEMENT* ressources_head,RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = ressources_head;

    while (head != NULL) {
        if (head->ressource == ressource) { // if found
            head->disponibilite = true;
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}

bool op_mark_ressource_unavailable(RESSOURCE_ELEMENT* ressources_head, RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = ressources_head;

    while (head != NULL) {
        if (head->ressource == ressource) { // when found
            head->disponibilite = false;
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}

bool op_check_if_ressource_available(RESSOURCE_ELEMENT* ressources_head, RESSOURCE ressource) {
    
    RESSOURCE_ELEMENT* head = ressources_head;

    while (head != NULL) {
        if (head->ressource == ressource || head->disponibilite == true) { // when found
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}