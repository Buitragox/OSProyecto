#ifndef __LIST__
#define __LIST__
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int data;
 
    // Lower values indicate higher priority
    int priority;
 
    struct node* next;
 
} Node;

Node* newNode(int d, int p);

int peek(Node** head);

void pop(Node** head);

void push(Node** head, int d, int p);

int empty(Node **head);

#endif // __LIST__