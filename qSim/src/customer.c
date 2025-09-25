#include <stdlib.h>
#include "../include/customer.h"

BankCustomer* welcomeCustomer(int id, float arrivalTime) {
    BankCustomer* newVisitor = (BankCustomer*)malloc(sizeof(BankCustomer));
    newVisitor->customerId = id;
    newVisitor->arrivalTime = arrivalTime;
    newVisitor->serviceStartTime = -1;  // Not helped yet
    newVisitor->departureTime = -1;     // Still in the bank
    newVisitor->helpedByTeller = -1;    // Haven't met a teller yet
    return newVisitor;
}

float timeSpentWaiting(BankCustomer* customer) {
    if (customer->serviceStartTime < 0) return 0;  // Didn't wait at all!
    return customer->serviceStartTime - customer->arrivalTime;
}

float timeWithTeller(BankCustomer* customer) {
    if (customer->departureTime < 0 || customer->serviceStartTime < 0) return 0;
    return customer->departureTime - customer->serviceStartTime;
}

float totalTimeInBank(BankCustomer* customer) {
    if (customer->departureTime < 0) return 0;  // Still in the bank!
    return customer->departureTime - customer->arrivalTime;
}