#include "ServiceManager.h"

int main() {
    ServiceManager manager;
    int choice;

    cout << "====================================\n";
    cout << " SMART CAMPUS PARCEL MANAGEMENT\n";
    cout << " SERVICE MANAGEMENT MODULE\n";
    cout << "====================================\n";

    do {
        cout << "\n========== SERVICE MANAGEMENT MENU ==========\n";
        cout << "1. Create Service Request\n";
        cout << "2. Display Active Requests\n";
        cout << "3. Search Service Request\n";
        cout << "4. Process Next Request\n";
        cout << "5. Cancel Service Request\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                manager.createRequest();
                break;

            case 2:
                manager.displayRequests();
                break;

            case 3:
                manager.searchRequest();
                break;

            case 4:
                manager.processRequest();
                break;

            case 5:
                manager.cancelRequest();
                break;

            case 0:
                cout << "\nExiting Service Management Module.\n";
                break;

            default:
                cout << "\nInvalid choice! Please choose 0 to 5.\n";
        }

    } while (choice != 0);

    return 0;
}
