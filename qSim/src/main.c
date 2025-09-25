#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../include/event.h"
#include "../include/customer.h"
#include "../include/teller.h"
#include "../include/queue.h"
#include "../include/constants.h"

// Our "BankDay" is basically a diary of everything that happens today
typedef struct BankDay {
    int expectedCustomers;      
    int numberOfTellers;        
    float bankHours;            
    float averageServiceTime;   
    int useSingleLine;          

    BankEvent* dailySchedule;      
    BankCustomer** todayCustomers; 
    BankTeller** workingTellers;   
    Queue** waitingAreas;          

    // How the day went
    float totalCustomerTimeInBank;
    float longestWaitTime;
    float totalServiceTimeProvided;
    float totalTellerDowntime;
    int customersServedToday;
} BankDay;

// Global pointer — think of it as the branch manager's clipboard
BankDay* todaysOperations = NULL;

// --- Function declarations ---
void announce(const char* message);
void addEvent(BankEvent** schedule, BankEvent* newEvent);
BankEvent* getNextEvent(BankEvent** schedule);
void whenCustomerArrives(BankEvent* event);
void whenCustomerLeaves(BankEvent* event);
void whenTellerTakesBreak(BankEvent* event);
float randomServiceTime(float avgTime);
float randomBreakTime();
void runSimulation(int customers, int tellers, float hours, float avgService, int singleLine);
void endOfDayReport(BankDay* day);
void cleanUp(BankDay* day);

//Prints messages nicely
void announce(const char* message) {
    printf("%s\n", message);
}

// Adds a new event to the day's schedule (keeps events sorted by time)
void addEvent(BankEvent** schedule, BankEvent* newEvent) {
    announce("Scheduling a new event...");

    if (*schedule == NULL || newEvent->when < (*schedule)->when) {
        newEvent->nextEvent = *schedule;
        *schedule = newEvent;
    } else {
        BankEvent* current = *schedule;
        while (current->nextEvent != NULL && current->nextEvent->when <= newEvent->when) {
            current = current->nextEvent;
        }
        newEvent->nextEvent = current->nextEvent;
        current->nextEvent = newEvent;
    }
}

//  Gets the very next event we need to deal with
BankEvent* getNextEvent(BankEvent** schedule) {
    announce("Moving to the next event...");
    if (*schedule == NULL) return NULL;

    BankEvent* next = *schedule;
    *schedule = (*schedule)->nextEvent;
    next->nextEvent = NULL;
    return next;
}

// What happens when a customer walks in
void whenCustomerArrives(BankEvent* event) {
    announce("A customer just walked in!");

    BankCustomer* customer = (BankCustomer*)event->whoWasInvolved;
    BankDay* bank = todaysOperations;

    // Find which line to join
    Queue* chosenLine;
    if (bank->useSingleLine) {
        chosenLine = bank->waitingAreas[0];
    } else {
        chosenLine = findShortestLine(bank->waitingAreas, bank->numberOfTellers);
    }

    addToLine(chosenLine, customer);

    // If they're first, serve them right away
    for (int i = 0; i < bank->numberOfTellers; i++) {
        Queue* checkLine = bank->useSingleLine ? bank->waitingAreas[0] : bank->waitingAreas[i];
        if (getLineLength(checkLine) == 1) {
            float serviceTime = randomServiceTime(bank->averageServiceTime);
            BankEvent* done = planEvent(CUSTOMER_LEAVES_HAPPY,
                                        event->when + serviceTime,
                                        customer,
                                        whenCustomerLeaves);
            addEvent(&bank->dailySchedule, done);
            break;
        }
    }
}

// What happens when a customer finishes and leaves
void whenCustomerLeaves(BankEvent* event) {
    announce("Customer is done and leaves happily.");

    BankCustomer* customer = (BankCustomer*)event->whoWasInvolved;
    BankDay* bank = todaysOperations;

    float totalBankTime = totalTimeInBank(customer);
    float waitTime = timeSpentWaiting(customer);

    bank->totalCustomerTimeInBank += totalBankTime;
    bank->totalServiceTimeProvided += timeWithTeller(customer);
    bank->customersServedToday++;

    if (waitTime > bank->longestWaitTime) {
        bank->longestWaitTime = waitTime;
    }

    int tellerId = customer->helpedByTeller;
    Queue* line = bank->useSingleLine ? bank->waitingAreas[0] : bank->waitingAreas[tellerId];

    if (!isLineEmpty(line)) {
        BankCustomer* nextCustomer = nextInLine(line);
        float serviceTime = randomServiceTime(bank->averageServiceTime);
        BankEvent* nextService = planEvent(CUSTOMER_LEAVES_HAPPY,
                                           event->when + serviceTime,
                                           nextCustomer,
                                           whenCustomerLeaves);
        addEvent(&bank->dailySchedule, nextService);
    } else {
        float breakTime = randomBreakTime();
        BankEvent* breakEvent = planEvent(TELLER_TAKES_BREAK,
                                          event->when + breakTime,
                                          bank->workingTellers[tellerId],
                                          whenTellerTakesBreak);
        addEvent(&bank->dailySchedule, breakEvent);
    }

    free(customer);
}

// Random service time (so every transaction isn't identical)
float randomServiceTime(float avgTime) {
    float variation = (rand() / (float)RAND_MAX) * SERVICE_VARIATION;
    return avgTime * (0.5 + variation);
}

// Random break time for tellers
float randomBreakTime() {
    return MIN_QUICK_CHECK + (rand() / (float)RAND_MAX) * MAX_COFFEE_BREAK;
}

// Runs the full-day simulation
void runSimulation(int customers, int tellers, float hours, float avgService, int singleLine) {
    srand(time(NULL));

    setupBankAnnouncer(announce);

    BankDay today;
    todaysOperations = &today;

    today.expectedCustomers = customers;
    today.numberOfTellers = tellers;
    today.bankHours = hours;
    today.averageServiceTime = avgService;
    today.useSingleLine = singleLine;

    today.dailySchedule = NULL;
    today.todayCustomers = (BankCustomer*)malloc(customers * sizeof(BankCustomer));
    today.workingTellers = (BankTeller*)malloc(tellers * sizeof(BankTeller));
    today.waitingAreas = setupWaitingAreas(tellers, singleLine);
    today.totalCustomerTimeInBank = 0;
    today.longestWaitTime = 0;
    today.totalServiceTimeProvided = 0;
    today.totalTellerDowntime = 0;
    today.customersServedToday = 0;

    printf("\n Bank Opens!\n");
    printf("Expected customers: %d | Tellers: %d | Line system: %s\n",
           customers, tellers, singleLine ? "One Big Line" : "Separate Lines");

    // Schedule all arrivals
    for (int i = 0; i < customers; i++) {
        float arrivalTime = hours * rand() / (float)RAND_MAX;
        today.todayCustomers[i] = welcomeCustomer(i, arrivalTime);

        BankEvent* arrival = planEvent(CUSTOMER_WALKS_IN,
                                       arrivalTime,
                                       today.todayCustomers[i],
                                       whenCustomerArrives);
        addEvent(&today.dailySchedule, arrival);
    }

    // Hire tellers and let them settle in
    for (int i = 0; i < tellers; i++) {
        today.workingTellers[i] = hireTeller(i, singleLine);
        float initialBreak = randomBreakTime();
        BankEvent* firstActivity = planEvent(TELLER_TAKES_BREAK,
                                             initialBreak,
                                             today.workingTellers[i],
                                             whenTellerTakesBreak);
        addEvent(&today.dailySchedule, firstActivity);
    }

    float currentTime = 0;
    while (today.dailySchedule != NULL && currentTime <= hours) {
        BankEvent* current = getNextEvent(&today.dailySchedule);
        currentTime = current->when;

        if (currentTime <= hours) {
            handleEvent(current);
        }
        free(current);
    }

    endOfDayReport(&today);
    cleanUp(&today);
    todaysOperations = NULL;
}

// Prints a friendly report
void endOfDayReport(BankDay* day) {
    printf("\n End of Day Summary\n");
    printf("System: %s | Customers Served: %d\n",
           day->useSingleLine ? "Shared Line" : "Multiple Lines",
           day->customersServedToday);

    if (day->customersServedToday > 0) {
        float avgTime = day->totalCustomerTimeInBank / day->customersServedToday;
        printf(" Average Time in Bank: %.2f mins |  Longest Wait: %.2f mins\n",
               avgTime, day->longestWaitTime);
        printf("Total Service Time: %.2f mins\n", day->totalServiceTimeProvided);
    }

    printf("Bank is now closed. See you tomorrow!\n");
}

//  Frees up all memory (end of day cleanup)
void cleanUp(BankDay* day) {
    if (day->todayCustomers) {
        for (int i = 0; i < day->expectedCustomers; i++) {
            if (day->todayCustomers[i]) free(day->todayCustomers[i]);
        }
        free(day->todayCustomers);
    }
    if (day->workingTellers) {
        for (int i = 0; i < day->numberOfTellers; i++) {
            if (day->workingTellers[i]) free(day->workingTellers[i]);
        }
        free(day->workingTellers);
    }
    if (day->waitingAreas) {
        cleanupWaitingAreas(day->waitingAreas, day->numberOfTellers);
    }
}

//  Program starts here
int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s #customers #tellers hours averageServiceTime\n", argv[0]);
        printf("Example: %s 100 4 60 2.3\n", argv[0]);
        return 1;
    }

    int customers = atoi(argv[1]);
    int tellers = atoi(argv[2]);
    float hours = atof(argv[3]);
    float avgService = atof(argv[4]);

    if (customers <= 0 || tellers <= 0 || hours <= 0 || avgService <= 0) {
        printf("Please use positive numbers!\n");
        return 1;
    }

    printf("\n BANK SIMULATION: Let's see which system works better!\n");

    printf("\n=== Scenario 1: Separate Lines ===\n");
    runSimulation(customers, tellers, hours, avgService, 0);

    printf("\n=== Scenario 2: One Shared Line ===\n");
    runSimulation(customers, tellers, hours, avgService, 1);

    printf("\n Simulation Complete! Usually, the shared line keeps things fairer.\n");
    return 0;
}