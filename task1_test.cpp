
#include <iostream>
#include <string>
#include "modules/ParcelIntake.h"

using namespace std;

int main() {
    ParcelIntake parcelSystem;
    string input;
    int choice;

    do {
        cout << "\n========================================\n";
        cout << "  SMART CAMPUS PARCEL MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << " TASK 1: PARCEL INTAKE AND PROCESSING\n";
        cout << "----------------------------------------\n";
        cout << "1. Register New Parcel\n";
        cout << "2. Display All Parcels\n";
        cout << "3. Search Parcel\n";
        cout << "4. Display Pending Queue\n";
        cout << "5. Process Next Parcel\n";
        cout << "6. Update Parcel\n";
        cout << "7. Remove Parcel\n";
        cout << "8. Report Missing Parcel\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        getline(cin, input);

        if (input.length() != 1 ||
            input[0] < '0' ||
            input[0] > '8') {
            cout << "Invalid choice. Try again.\n";
            choice = -1;
            continue;
        }

        choice = input[0] - '0';

        switch (choice) {
            case 1:
                parcelSystem.registerParcel();
                break;

            case 2:
                parcelSystem.displayAllParcels();
                break;

            case 3:
                parcelSystem.searchParcel();
                break;

            case 4:
                parcelSystem.displayPendingQueue();
                break;

            case 5:
                parcelSystem.processNextParcel();
                break;

            case 6:
                parcelSystem.updateParcel();
                break;

            case 7:
                parcelSystem.removeParcel();
                break;

            case 8:
                parcelSystem.reportMissingParcel();
                break;

            case 0:
                cout << "\nExiting Task 1 module...\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}

