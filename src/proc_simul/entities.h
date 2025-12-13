
#include <time.h>

enum {
    READY, BLOCKED, EXECUTION, TERMINATED
} E_etat;

typedef struct {
    char ressource_name[10];
    bool disponibilite;
} RESSOURCE;

typedef struct {
    tm temps_arrive
} PROCESS_STATISTICS;

typedef struct {
    int pcb_id;
    char process_name[20];
    char user_id[20];
    int ppid;
    E_etat etat; // one of the values in the enum e_etat
    int prioritie; // priorities from 1 to 5 ; 1-critical

    char* instructions;
    long programme_compteur; // max 9,223,372,036,854,775,807 instruction
    int memoire_necessaire; // en MB

    int burst_time; // total temps necessaire en ms pour l'exec . burst = compte_temps + temps_restant
    int cpu_time_used; // temps cpu consomme en ms init 0
    int remaining_time; // temps restant : = burst - cpu_time_used
    long cpu_usage; // 

    
} PCB;