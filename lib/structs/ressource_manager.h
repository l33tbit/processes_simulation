#pragma once



#include "../../lib/structs/ressource.h"
#include "../../lib/structs/execution_queue.h"

#define ressource_number 6


typedef enum {
    DONE, INIT_ERROR
} init_rm;

typedef struct RESSOURCE_MANAGER {

    RESSOURCE_ELEMENT* ressources;      // 8 bytes: pointer
    int ressource_count;                // 4 bytes: integer
    
    // setters
    SETTER (*set_ressources)(struct RESSOURCE_MANAGER* self, RESSOURCE_ELEMENT* ressources);
    SETTER (*set_ressource_count)(struct RESSOURCE_MANAGER* self, int count);
    
    // getters
    RESSOURCE_ELEMENT* (*get_ressources)(struct RESSOURCE_MANAGER* self);
    int (*get_ressource_count)(struct RESSOURCE_MANAGER* self);
    
    // init & clean
    RESSOURCE_ELEMENT* (*create_ressource_list)(void);
    init_rm (*init)(struct RESSOURCE_MANAGER* self);
    TASK (*free_ressource_list)(struct RESSOURCE_MANAGER* self);
    WORK_RETURN (*kill)(struct RESSOURCE_MANAGER* self);
    
    // resource related
    RESSOURCE_ELEMENT* (*look_for_ressource_in_list)(struct RESSOURCE_MANAGER* self, RESSOURCE ressource);
    TASK (*mark_ressource_available)(struct RESSOURCE_MANAGER* self, RESSOURCE ressource);
    TASK (*mark_ressource_unavailable)(struct RESSOURCE_MANAGER* self, RESSOURCE ressource);
    TASK (*check_if_ressource_available)(struct RESSOURCE_MANAGER* self, RESSOURCE ressource);
    
} RESSOURCE_MANAGER;

