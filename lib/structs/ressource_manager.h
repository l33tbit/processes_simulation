#pragma once



typedef enum {
    MEMORY, SOCKET, FILE, CPU, DEVICE_A, DEVICE_B, DEVICE_C
} RESSOURCE;

typedef struct {
    char* ressource_name[10]; // nom de ressource
    RESSOURCE* ressource; // type 
    bool disponibilite; // dispo ou non 1 / 0
} RESSOURCE_ELEMENT;

typedef struct {
    RESSOURCE_ELEMENT* head; // first element in chaine
    RESSOURCE_ELEMENT* tail; // last one
    int size; // how many element in the chaine<
} RESSOURCES_LIST; // chain ressource because we need size

typedef struct {
    RESSOURCES_LIST* ressources; // again using malloc to allocate N ressources
    int ressource_count; // n ressources

    //functions
    // on start
    RESSOURCE_LIST* (*create_ressource_list)();

    // ressources operations
    RESSOURCE_ELEMENT* (*look_for_ressource_in_list)(char* ressource_name);
    bool (*mark_ressource_available)(RESSOURCE* ressource);
    bool (*mark_ressource_unavailable)(RESSOURCE* ressource);
    bool (*check_if_ressource_available)(RESSOURCE* ressource); // if available return  if not return 0
    
} RESSOURCE_MANAGER;