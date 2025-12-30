#pragma once

#include <stdbool.h>
#include "../../lib/structs/enums.h"

typedef enum {
    AAA, BBB, CCC, DDD, EEE, FFF
} RESSOURCE;


typedef struct RESSOURCE_ELEMENT {
    char ressource_name[10];            // 10 bytes: chaine de caractere 10 bytes
    RESSOURCE ressource;                // 4 bytes: enum so integer
    bool disponibilite;                 // 1 byte: boolean
    // need 1 bytes : <
    struct RESSOURCE_ELEMENT* next_ressource;  // 8 bytes: pointer
    
    // setters
    SETTER (*set_ressource_name)(struct RESSOURCE_ELEMENT* self, const char* name);
    SETTER (*set_ressource)(struct RESSOURCE_ELEMENT* self, RESSOURCE ressource);
    SETTER (*set_disponibilite)(struct RESSOURCE_ELEMENT* self, bool disponibilite);
    SETTER (*set_next_ressource)(struct RESSOURCE_ELEMENT* self, struct RESSOURCE_ELEMENT* next);
    
    // getters
    char* (*get_ressource_name)(struct RESSOURCE_ELEMENT* self);
    RESSOURCE (*get_ressource)(struct RESSOURCE_ELEMENT* self);
    bool (*get_disponibilite)(struct RESSOURCE_ELEMENT* self);
    struct RESSOURCE_ELEMENT* (*get_next_ressource)(struct RESSOURCE_ELEMENT* self);
    
} RESSOURCE_ELEMENT;