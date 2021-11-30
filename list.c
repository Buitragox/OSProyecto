#include "list.h"

// Implementación tomada de https://www.geeksforgeeks.org/priority-queue-using-linked-list/

Node* newNode(int d, int p, int i)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = d;
    temp->priority = p;
    temp->index = i;
    temp->next = NULL;
 
    return temp;
}


int peekData(Node ** head){
    return(*head)->data;
}

int peekIndex(Node ** head){
    return(*head)->index;
}


void pop(Node ** head){
    Node * temp = *head;
    (*head) = (*head)-> next;
    free(temp);
}

int empty(Node ** head){
    return (*head) == NULL;
}


void push(Node ** head, int d, int p, int i){
    Node* start = (*head);
 
    // Create new Node
    Node* temp = newNode(d, p, i);
 
    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if (empty(head)) {
        (*head) = temp;
    }
    else if ((*head)->priority > p) {
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL && start->next->priority < p) {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}


