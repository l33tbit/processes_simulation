
#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    int data;
    struct node* next;
} node;

node* create_empty_chaine() {
    node* head = NULL;
    return head;
}

node* create_node(int value) {
    node* newE = (node*)malloc(sizeof(node));

    if (newE == NULL) {
        printf("allocation error");
        exit(1);
    }

    newE->data = value;
    newE->next = NULL;

    return newE;
}

node* insert_debut(node* head, int v) {
    node* newE = create_node(v);
    newE->next = head;
    return newE;
}

node* insert_fin(node* head, int v) {
    node* newE = create_node(v);
    if (head == NULL) {
        head = newE;
    } else {
        node* temp = head;
        
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newE;
    }
    return head;
}

node* insert_av(node* head, int v, int v_search) {
    if (head == NULL) {
        printf("chaine is empty\n");
    } else if (head->data == v_search) {
        node* newE = create_node(v);
        newE->next = head;
        head = newE;
    } else {
        node* temp = head;
        node* previous = head;

        while (temp != NULL && temp->data != v_search) {
            previous = temp;
            temp = temp->next;
        }

        if (temp != NULL) {
            node* newE = create_node(v);
            previous->next = newE;
            newE->next = temp;
        } else {
            printf("value not found\n");
        }
    } 
    return head;
}

node* insert_ap(node* head, int v, int v_search) {
    if (head == NULL) {
        printf("chaine is empty\n");
    } else {
        node* current = head;

        while (current != NULL) {
            if (current->data == v_search) {
                node* newE = create_node(v);
                newE->next = current->next;
                current->next = newE;
                break;
            }
            current = current->next;
        }

        if (current == NULL) {
            printf("value not found\n");
        }
    }
    return head;
}

node* search_node(node* head, int v_search) { // will return NULL if value not found if found will return the node adress
    node* temp = head;
    node* node_searched = NULL;
    
    if (head == NULL) {
        printf("empty chaine\n");
        return NULL;
    }

    while (temp != NULL) {
        if (temp->data == v_search) {
            node_searched = temp;
            break;
        }
        temp = temp->next;
    }

    return (node_searched == NULL) ? NULL : node_searched;
}

node* alter_nodes(node* head, int v1, int v2) {
    node* value1 = search_node(head, v1);
    node* value2 = search_node(head, v2);

    node* temp = (node*)malloc(sizeof(node));
    temp->data = value1->data;
    value1->data = value2->data;
    value2->data = temp->data;
    free(temp);
    return head;
}

node* sort_ascending(node* head) {
    node* current = head;
    node* index = head;
    int temp;
    while (current != NULL) {
        index = current->next;
        while(index != NULL) {
            if (current->data > index->data) {
                temp = current->data;
                current->data = index->data;
                index->data = temp;
            }
            index = index->next;
        }
        current = current->next;
    }
    return head;
}

void free_list(node* head) {
    node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}


int main() {
    node* chaine = create_empty_chaine();
    node* head = create_node(5);
    printf("1 - first element's data %d\n\n\n", head->data);

    head = insert_debut(head, 6);
    printf("2 - first element's data %d\n", head->data);
    printf("2 - second element's data %d\n\n\n", head->next->data);

    head = insert_fin(head, 7);
    printf("3 - first element's data %d\n", head->data);
    printf("3 - second element's data %d\n", head->next->data);
    printf("3 - third element's data %d\n\n\n", head->next->next->data);

    head = insert_av(head, 8, 7);
    printf("4 - first element's data %d\n", head->data);
    printf("4 - second element's data %d\n", head->next->data);
    printf("4 - third element's data %d\n", head->next->next->data);
    printf("4 - fourth element's data %d\n\n\n", head->next->next->next->data);

    head = insert_ap(head, 9, 8);
    printf("5 - first element's data %d\n", head->data);
    printf("5 - second element's data %d\n", head->next->data);
    printf("5 - third element's data %d\n", head->next->next->data);
    printf("5 - fourth element's data %d\n", head->next->next->next->data);
    printf("5 - fifth element's data %d\n\n\n", head->next->next->next->next->data);

    head = alter_nodes(head, 7, 9);
    head = alter_nodes(head, 5, 6);
    printf("5 - first element's data %d\n", head->data);
    printf("5 - second element's data %d\n", head->next->data);
    printf("5 - third element's data %d\n", head->next->next->data);
    printf("5 - fourth element's data %d\n", head->next->next->next->data);
    printf("5 - fifth element's data %d\n\n\n", head->next->next->next->next->data);

    head = sort_ascending(head);
    printf("5 - first element's data %d\n", head->data);
    printf("5 - second element's data %d\n", head->next->data);
    printf("5 - third element's data %d\n", head->next->next->data);
    printf("5 - fourth element's data %d\n", head->next->next->next->data);
    printf("5 - fifth element's data %d\n\n\n", head->next->next->next->next->data);


    free_list(head);
    return 0;
}