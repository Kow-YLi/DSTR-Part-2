#include <iostream>
#include <string>
#include "modules/ParcelIntake.h"
#include "modules/CollectionDelivery.h"

using namespace std;

// Test program for Module 3.
// Module 1 is included because Module 3 can only work on
// parcels that Module 1 has registered and processed.

void runModule1Menu(ParcelIntake& intake) {
    string input;

    while (true) {
        cout << "\n========================================\n";
        cout << " MODULE 1: PARCEL INTAKE & PROCESSING\n";
        cout << "----------------------------------------\n";
        cout << "1. Register New Parcel\n";
        cout << "2. Display All Parcels\n";
        cout << "3. Search Parcel\n";
        cout << "4. Display Pending Queue\n";
        cout << "5. Process Next Parcel\n";
        cout << "6. Update Parcel\n";
        cout << "7. Remove Parcel\n";
        cout << "8. Report Missing Parcel\n";
        cout << "0. Back\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        getline(cin, input);

        if (input.length() != 1 ||
            input[0] < '0' || input[0] > '8') {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        switch (input[0]) {
            case '1': intake.registerParcel(); break;
            case '2': intake.displayAllParcels(); break;
            case '3': intake.searchParcel(); break;
            case '4': intake.displayPendingQueue(); break;
            case '5': intake.processNextParcel(); break;
            case '6': intake.updateParcel(); break;
            case '7': intake.removeParcel(); break;
            case '8': intake.reportMissingParcel(); break;
            case '0': return;
        }
    }
}

int main() {
    // ONE shared parcel system used by both modules
    ParcelIntake intake;
    CollectionDelivery counter(intake);

    string input;

    while (true) {
        cout << "\n========================================\n";
        cout << "  SMART CAMPUS PARCEL MANAGEMENT SYSTEM\n";
        cout << "        (Task 3 Test Program)\n";
        cout << "========================================\n";
        cout << "1. Module 1: Parcel Intake & Processing\n";
        cout << "2. Module 3: Collection Counter & Delivery\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        getline(cin, input);

        if (input == "1") {
            runModule1Menu(intake);
        } else if (input == "2") {
            counter.runMenu();
        } else if (input == "0") {
            cout << "\nExiting...\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}