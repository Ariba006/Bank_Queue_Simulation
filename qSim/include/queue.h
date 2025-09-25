#ifndef QUEUE_H
#define QUEUE_H

#include "customer.h"

// A node in our waiting line
typedef struct CustomerNode {
    BankCustomer* customer;
    struct CustomerNode* next;
} CustomerNode;

// The waiting line itself
typedef struct Queue {
    CustomerNode* front;
    CustomerNode* rear;
    int length;
} Queue;

// Function declarations
Queue* createLine();
Queue** setupWaitingAreas(int numTellers, int singleLine);
void cleanupWaitingAreas(Queue** areas, int numTellers);
void addToLine(Queue* line, BankCustomer* customer);
BankCustomer* nextInLine(Queue* line);
int isLineEmpty(Queue* line);
int getLineLength(Queue* line);
Queue* findShortestLine(Queue** lines, int numLines);

#endif