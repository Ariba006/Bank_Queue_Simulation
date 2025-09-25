#ifndef TELLER_H
#define TELLER_H

#include "queue.h"

// Each teller's work day
typedef struct BankTeller {
    int tellerId;
    float timeHelpingCustomers;
    float timeOnBreak;
    int customersHelped;
    Queue* waitingLine;        // The line of people waiting for this teller
    int sharesCommonLine;      // Do they share one big line or have their own?
} BankTeller;

// Function declarations
BankTeller* hireTeller(int id, int sharesCommonLine);
void helpCustomer(BankTeller* teller, BankCustomer* customer, float serviceTime);
void takeBreak(BankTeller* teller, float breakTime);
