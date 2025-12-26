#pragma once

#include "../../lib/structs/ressource_manager.h"
#include "../../lib/structs/ressource.h"

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
RESSOURCE_ELEMENT* op_look_for_ressource_in_list(RESSOURCE_MANAGER* self, RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = self->ressources;

    while (head != NULL) {
        if (head->ressource == ressource) {
            return head;
        }
        head = head->next_ressource;
    }

    return NULL;
}

bool op_mark_ressource_available(RESSOURCE_MANAGER* self, RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = self->ressources;

    while (head != NULL) {
        if (head->ressource == ressource) { // if found
            head->disponibilite = true;
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}

bool op_mark_ressource_unavailable(RESSOURCE_MANAGER* self, RESSOURCE ressource) {

    RESSOURCE_ELEMENT* head = self->ressources;

    while (head != NULL) {
        if (head->ressource == ressource) { // when found
            head->disponibilite = false;
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}

bool op_check_if_ressource_available(RESSOURCE_MANAGER* self, RESSOURCE ressource) {
    
    RESSOURCE_ELEMENT* head = self->ressources;

    while (head != NULL) {
        if (head->ressource == ressource || head->disponibilite == true) { // when found
            return true;
        }
        head = head->next_ressource;
    }

    return false;
}

bool op_free_ressource_list(RESSOURCE_MANAGER* self) {

    RESSOURCE_ELEMENT* next = self->ressources;

    while (next != NULL) {

        RESSOURCE_ELEMENT* temp = next;
        next = next->next_ressource;
        free(temp);

    }

    return true;
}

bool op_rm_kill(RESSOURCE_MANAGER* self) {

    if (self->free_ressource_list(self) == false) {
        return false;
    }

    free(self);

    return true;
}


init_rm op_rm_init(RESSOURCE_MANAGER* self) {

    // --------functions assigning

    self->check_if_ressource_available = op_check_if_ressource_available;
    self->create_ressource_list = op_create_ressource_list;
    self->look_for_ressource_in_list = op_look_for_ressource_in_list;
    self->mark_ressource_available = op_mark_ressource_available;
    self->mark_ressource_unavailable = op_mark_ressource_unavailable;
    self->free_ressource_list = op_free_ressource_list;
    self->kill = op_rm_kill;

    self->ressources = self->create_ressource_list(); // create the ressource list then assign it to the ressource head field in ressource manager

    if (self->ressources == NULL) {
        return INIT_ERROR;
    }

    return DONE;
}
