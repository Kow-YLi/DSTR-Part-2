#include <iostream>
#include <string>

#include "modules/ParcelIntake.h"
#include "modules/CollectionDelivery.h"
// TODO: include the other modules when they are ready, e.g.
// #include "modules/Module2File.h"
// #include "modules/Module4File.h"
// #include "modules/Module5File.h"

using namespace std;

// ==================================================
// Module 1 menu
// (ParcelIntake has no runMenu() yet, so its menu
//  lives here. If Module 1 adds runMenu() later,
//  replace this with intake.runMenu().)
// ==================================================

void runModule1Menu(ParcelIntake& intake) {
    string input;
    string message;   // shown above the prompt after the menu

    while (true) {
        cout << "\n========================================\n";
        cout << " MODULE 1: PARCEL INTAKE, SORTING\n";
        cout << "           & PROCESSING\n";
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

// Message for modules that are not finished yet
string notReadyYet(const string& moduleName) {
    return "ERROR: " + moduleName +
           " is not available yet (still under development).";
}

// Remove spaces before/after the typed choice
string trimInput(const string& value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

// ==================================================
// Main program
// ==================================================

int main() {
    // ONE shared parcel system.
    // Every module receives this same object, so they
    // all read and update the same parcel records.
    ParcelIntake intake;                 // Module 1
    CollectionDelivery counter(intake);  // Module 3

    // TODO: create the other modules with the shared data
    // Module2Class service(intake);
    // Module4Class records(intake);
    // Module5Class categories(intake);

    string input;
    string message;   // shown above the prompt after the menu

    while (true) {
        cout << "\n========================================\n";
        cout << "  SMART CAMPUS PARCEL MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << "1. Parcel Intake, Sorting & Processing\n";
        cout << "2. Parcel Service & Collection Management\n";
        cout << "3. Collection Counter & Delivery Resources\n";
        cout << "4. Parcel Activity & Record Management\n";
        cout << "5. Parcel Category Organisation\n";
        cout << "0. Exit\n";
        cout << "========================================\n";

        if (!message.empty()) {
            cout << ">> " << message << "\n";
            message = "";
        }

        cout << "Enter your choice: ";

        getline(cin, input);
        input = trimInput(input);

        if (input == "1") {
            runModule1Menu(intake);
        } else if (input == "2") {
            message = notReadyYet("Module 2");   // TODO: service.runMenu();
        } else if (input == "3") {
            counter.runMenu();
        } else if (input == "4") {
            message = notReadyYet("Module 4");   // TODO: records.runMenu();
        } else if (input == "5") {
            message = notReadyYet("Module 5");   // TODO: categories.runMenu();
        } else if (input == "0") {
            cout << "\nThank you for using the system. Goodbye!\n";
            break;
        } else {
            message = "Invalid choice. Please enter 0-5.";
        }
    }

    return 0;
}