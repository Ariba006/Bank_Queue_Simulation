#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/event.h"

// Our bank's public address system
bankAnnouncer = NULL;

void setupBankAnnouncer(announcer) {
    bankAnnouncer = announcer;
}

BankEvent* planEvent(BankActivity activity, float time, void* involvedPerson, void (action)(BankEvent)) {
    BankEvent* newHappening = (BankEvent*)malloc(sizeof(BankEvent));
    newHappening->whatHappened = activity;
    newHappening->when = time;
    newHappening->whoWasInvolved = involvedPerson;
    newHappening->whatToDo = action;
    newHappening->nextEvent = NULL;
    return newHappening;
}

void handleEvent(BankEvent* event) {
    if (event && event->whatToDo) {
        announceActivity("Handling Daily Activity");
        event->whatToDo(event);  // Do what we planned for this moment
    }
}

void announceActivity(const char* whatWeDid) {
    if (bankAnnouncer) {
        char announcement[100];
        snprintf(announcement, sizeof(announcement), "Bank Activity: %s", whatWeDid);
        bankAnnouncer(announcement);
    }
}

void tellerBreakAction(BankEvent* event) {
    announceActivity("Teller Taking a Break");
    // In a full implementation, this would handle teller break logic
}
