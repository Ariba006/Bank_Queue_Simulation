#ifndef EVENT_H
#define EVENT_H

// Different types of happenings in our bank
typedef enum { 
    CUSTOMER_WALKS_IN, 
    CUSTOMER_LEAVES_HAPPY, 
    TELLER_TAKES_BREAK 
} BankActivity;

// Each event is like an entry in the bank's daily log
typedef struct BankEvent {
    BankActivity whatHappened;
    float when;                  // When it occurred during the day
    void* whoWasInvolved;        // Who was part of this event
    void (whatToDo)(struct BankEvent);  // What we should do when this happens
    struct BankEvent* nextEvent; // What happens next in our day
} BankEvent;

// How we log our bank's activities
typedef void (BankLogger)(const char activity);

// Our bank's announcement system
extern BankLogger bankAnnouncer;

// Function declarations
void setupBankAnnouncer(BankLogger announcer);
BankEvent* planEvent(BankActivity activity, float time, void* involvedPerson, void (action)(BankEvent));
void handleEvent(BankEvent* event);
void announceActivity(const char* whatWeDid);
void tellerBreakAction(BankEvent* event);

#endif