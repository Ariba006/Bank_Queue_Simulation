#include <stdlib.h>
#include <stdio.h>
#include "../include/queue.h"

Queue* createLine() {
    Queue* newLine = (Queue*)malloc(sizeof(Queue));
    newLine->front = NULL;
    newLine->rear = NULL;
    newLine->length = 0;
    return newLine;
}

Queue** setupWaitingAreas(int numTellers, int singleLine) {
    Queue** areas = (Queue*)malloc((singleLine ? 1 : numTellers) * sizeof(Queue));
    int linesToCreate = singleLine ? 1 : numTellers;
    
    for (int i = 0; i < linesToCreate; i++) {
        areas[i] = createLine();
    }
    return areas;
}

void cleanupWaitingAreas(Queue** areas, int numTellers) {
    for (int i = 0; i < numTellers; i++) {
        if (areas[i]) {
            // Free any remaining customers in line
            while (!isLineEmpty(areas[i])) {
                BankCustomer* customer = nextInLine(areas[i]);
                free(customer);
            }
            free(areas[i]);
        }
    }
    free(areas);
}

void addToLine(Queue* line, BankCustomer* customer) {
    CustomerNode* newNode = (CustomerNode*)malloc(sizeof(CustomerNode));
    newNode->customer = customer;
    newNode->next = NULL;
    
    if (line->rear == NULL) {
        line->front = line->rear = newNode;
    } else {
        line->rear->next = newNode;
        line->rear = newNode;
    }
    line->length++;
}

BankCustomer* nextInLine(Queue* line) {
    if (isLineEmpty(line)) return NULL;
    
    CustomerNode* temp = line->front;
    BankCustomer* customer = temp->customer;
    line->front = line->front->next;
    
    if (line->front == NULL) {
        line->rear = NULL;
    }
    
    free(temp);
    line->length--;
    return customer;
}

int isLineEmpty(Queue* line) {
    return line->front == NULL;
}

int getLineLength(Queue* line) {
    return line->length;
}

Queue* findShortestLine(Queue** lines, int numLines) {
    Queue* shortest = lines[0];
    for (int i = 1; i < numLines; i++) {
        if (getLineLength(lines[i]) < getLineLength(shortest)) {
            shortest = lines[i];
        }
    }
    return shortest;
}