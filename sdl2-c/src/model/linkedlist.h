
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdbool.h>

/**
* Node structure for a double linked list. 
* Contains data, index of node, next pointer and previous pointer.
*/
typedef struct Node{
    void* data;
    int index;
    struct Node* next;
    struct Node* prev;
}Node;

/**
* Initiates a new Node.
* @param data Node value to be stored.
*/
Node* node_create(void* data);

/**
* Linkedlist structure for a double linked list.
* Contains 'first' node pointer, 'last' node pointer
* and current length of list.
*/
typedef struct Linkedlist{
    Node* first;
    Node* last;
    int length;
}Linkedlist;

/**
* Initiates a new Linked List.
*/
Linkedlist* linkedlist_create();

/**
* Boolean to check wether list is empty or not.
*/
bool linkedlist_is_empty();

/**
* Adds a Node at the end of the current List.
* @param list to place new node in.
* @param new_node Node to add.
*/
void linkedlist_push(Linkedlist* list, Node* new_node);

/**
* Removes or 'pops' last Node of Linked list.
* @return Last Node in list.
*/
Node* linkedlist_pop(Linkedlist* list);

#endif