#pragma once



#include "structs/ressource.h"

// typedef struct {
//     RESSOURCE_ELEMENT* head; // first element in chaine
//     int size; // how many element in the chaine
// } RESSOURCES_LIST; // chain ressource because we need size

typedef struct {
    RESSOURCE_ELEMENT* ressources; // again using malloc to allocate N ressources
    int ressource_count; // n ressources

    //functions
    // on start
    RESSOURCES_LIST* (*create_ressource_list)();

    // ressources operations
    RESSOURCE_ELEMENT* (*look_for_ressource_in_list)(char* ressource_name);
    RESSOURCE_ELEMENT* (*mark_ressource_available)(RESSOURCE* ressource);
    RESSOURCE_ELEMENT* (*mark_ressource_unavailable)(RESSOURCE* ressource);
    RESSOURCE_ELEMENT* (*check_if_ressource_available)(RESSOURCE* ressource); // if available return  if not return 0
    
} RESSOURCE_MANAGER;