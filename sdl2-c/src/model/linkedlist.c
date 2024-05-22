#include "linkedlist.h"
#include <stdlib.h>
#include <stdbool.h>

/**
* Initiates a new Node.
* @param data Node value to be stored.
*/
Node* node_create(void* data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

/**
* Initiates a new Linked List.
*/
Linkedlist* linkedlist_create() {
    Linkedlist* list = malloc(sizeof(Linkedlist));
    list->length = 0;
    return list;
}

/**
* Boolean to check wether list is empty or not.
*/
bool linkedlist_is_empty(Linkedlist* list) {
    return (list->length == 0);
}

/**
* Adds a Node at the end of the current List.
* @param list to place new node in.
* @param new_node Node to add.
*/
void linkedlist_push(Linkedlist* list, Node* new_node) {
    if(linkedlist_is_empty(list)) {
        new_node->index = 0;
        list->first = new_node;
        list->last = new_node;
    } else {
        new_node->index = list->last->index + 1;
        new_node->prev = list->last;
        list->last->next = new_node;
        list->last = new_node;
    }
    list->length++;
}

/**
* Removes or 'pops' last Node of Linked list.
* @return Last Node in list.
*/
Node* linkedlist_pop(Linkedlist* list) {
    if(linkedlist_is_empty(list)) { 
        return NULL; 
    } else if(list->length == 1) {
        Node* node = list->first;
        list->first = NULL;
        list->last = NULL;
        list->length--;
        return node;
    } else {
        Node* node = list->last;
        list->last->prev->next = NULL;
        list->last = list->last->prev;
        list->length--;
        return node;
    }
}