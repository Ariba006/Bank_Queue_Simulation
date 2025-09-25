#include <stdlib.h>
#include "../include/teller.h"

BankTeller* hireTeller(int id, int sharesCommonLine) {
    BankTeller* newEmployee = (BankTeller*)malloc(sizeof(BankTeller));
    newEmployee->tellerId = id;
    newEmployee->timeHelpingCustomers = 0;
    newEmployee->timeOnBreak = 0;
    newEmployee->customersHelped = 0;
    newEmployee->waitingLine = NULL;  // Will be set up later
    newEmployee->sharesCommonLine = sharesCommonLine;
    return newEmployee;
}

void helpCustomer(BankTeller* teller, BankCustomer* customer, float serviceTime) {
    teller->timeHelpingCustomers += serviceTime;
    teller->customersHelped++;
    customer->helpedByTeller = teller->tellerId;
}

void takeBreak(BankTeller* teller, float breakTime) {
    teller->timeOnBreak += breakTime;
}