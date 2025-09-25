#ifndef CUSTOMER_H
#define CUSTOMER_H

// Each customer's story at our bank
typedef struct BankCustomer {
    int customerId;           // Their unique number
    float arrivalTime;        // When they walked in
    float serviceStartTime;   // When they reached a teller
    float departureTime;      // When they left
    int helpedByTeller;       // Which teller assisted them
} BankCustomer;

// Function declarations
BankCustomer* welcomeCustomer(int id, float arrivalTime);
float timeSpentWaiting(BankCustomer* customer);
float timeWithTeller(BankCustomer* customer);
float totalTimeInBank(BankCustomer* customer);

#endif