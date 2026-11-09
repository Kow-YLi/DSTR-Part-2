#include "ParcelCategoryManager.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
 
using namespace std;
 
namespace {
 
// Read one line and trim surrounding spaces (Task 5's original trim() helper)
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}
 
// Keep asking until the user types something
string readRequired(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        value = trim(value);
        if (!value.empty()) return value;
        cout << "Error: input cannot be empty.\n";
    }
}
 
// Read a menu choice between minimum and maximum
int readChoice(int minimum, int maximum) {
    string line;
    while (true) {
        cout << "Enter choice: ";
        if (!getline(cin, line)) return minimum;   // input closed: leave the menu instead of looping forever
        line = trim(line);
        bool digits = !line.empty();
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] < '0' || line[i] > '9') digits = false;
        }
        if (digits) {
            int value = atoi(line.c_str());
            if (value >= minimum && value <= maximum) return value;
        }
        cout << "Invalid choice. Please enter " << minimum << "-" << maximum << ".\n";
    }
}
 
} // namespace
 
ParcelCategoryManager::ParcelCategoryManager(ParcelIntake& sharedIntake)
    : intake(sharedIntake) {
}
 
// Rebuild the BST from the shared Task 1 records.
// [FROM DIFF] Balanced: copy the valid parcels, sort them by ID (insertion sort, no STL), then insert middle-first.
void ParcelCategoryManager::syncFromIntake() {
    tree.clear();
    int total = intake.getParcelCount();
    if (total == 0) return;
 
    Parcel* all = new Parcel[total];
    int count = 0;
    for (int i = 0; i < total; i++) {
        Parcel p;
        if (intake.getParcelAt(i, p) && !p.parcelID.empty()) {
            all[count++] = p;
        }
    }
    for (int i = 1; i < count; i++) {
        Parcel key = all[i];
        int j = i - 1;
        while (j >= 0 && all[j].parcelID > key.parcelID) {
            all[j + 1] = all[j];
            j--;
        }
        all[j + 1] = key;
    }
    tree.buildBalanced(all, count);
    delete[] all;
}
 
// Full detail of one parcel (all shared fields)
void ParcelCategoryManager::printParcel(const Parcel& p) {
    cout << "  Parcel ID        : " << p.parcelID        << "\n";
    cout << "  Recipient Name   : " << p.recipientName   << "\n";
    cout << "  Recipient ID     : " << p.recipientID     << "\n";
    cout << "  Delivery Company : " << p.deliveryCompany << "\n";
    cout << "  Category (Type)  : " << p.parcelType      << "\n";
    cout << "  Arrival          : " << p.arrivalDateTime << "\n";
    cout << "  Weight (kg)      : " << p.weight          << "\n";   // [WEIGHT]
    cout << "  Status           : " << p.status          << "\n";
    cout << "  Next Action      : " << p.nextAction      << "\n";
}
 
// Weight in kg: same rule as Task 1 (> 0 and <= 100)
static double readWeight(const string& prompt) {
    while (true) {
        string value = readRequired(prompt);
        char* endPtr = nullptr;
        double w = strtod(value.c_str(), &endPtr);
        if (*endPtr == '\0' && w > 0.0 && w <= 100.0) return w;
        cout << "Error: weight must be a number greater than 0 and at most 100 (kg).\n";
    }
}
 
// FR2: insert = validate and insert into the BST, then register in Task 1 (so Tasks 1-4 see it too).
// [FROM DIFF] If Task 1 refuses, the tree insert is rolled back.
void ParcelCategoryManager::insertParcel() {
    cout << "\n========== INSERT NEW PARCEL ==========\n";
    Parcel p;
    p.parcelID        = readRequired("Enter Parcel ID: ");
    p.recipientName   = readRequired("Enter Recipient Name: ");
    p.recipientID     = readRequired("Enter Recipient ID: ");
    p.deliveryCompany = readRequired("Enter Delivery Company: ");
    p.parcelType      = readRequired("Enter Parcel Type / Category: ");
    p.weight          = readWeight("Enter Weight in kg: ");
    while (true) {
        p.arrivalDateTime = readRequired("Enter Arrival (YYYY-MM-DD HH:MM): ");
        if (ParcelIntake::isValidArrivalDateTime(p.arrivalDateTime)) break;
        cout << "Error: Invalid date/time.\n";
    }
    p.status     = "Pending Sorting";     // Task 1 rule: new parcels start here
    p.nextAction = "Not Assigned";
 
    if (!tree.insert(p)) return;          // BST: invalid data / duplicate key (message printed inside)
    if (!intake.addParcel(p)) {           // Task 1 refused: undo the tree insert
        tree.remove(p.parcelID);
        return;
    }
    cout << "\nParcel registered in Task 1 and added to the category tree.\n";
}
 
// FR3: search by unique Parcel ID
void ParcelCategoryManager::searchByID() {
    cout << "\n========== SEARCH PARCEL BY ID ==========\n";
    string id = readRequired("Enter Parcel ID: ");
    Parcel found;
    if (tree.searchByID(id, found)) {        // prints its own error if empty / not found
        cout << "\nParcel found:\n";
        printParcel(found);
    }
}
 
// FR3: list every parcel of one category
void ParcelCategoryManager::searchByCategory() {
    cout << "\n========== SEARCH BY CATEGORY ==========\n";
    string category = readRequired("Enter Category (parcel type): ");
    int matches = tree.searchByCategory(category);
    if (matches > 0) {
        cout << "  Total matches: " << matches << "\n";
    }
}
 
// FR4: display all parcels (in-order = sorted by Parcel ID)
void ParcelCategoryManager::displayAll() {
    cout << "\n========== ALL PARCELS (BST IN-ORDER) ==========\n";
    tree.displayAll();
}
 
// FR5: update. Task 1 is updated first (it enforces the completed/cancelled rule),
// then the same change is applied to the BST node.
void ParcelCategoryManager::updateParcel() {
    cout << "\n========== UPDATE PARCEL ==========\n";
    string id = readRequired("Enter Parcel ID: ");
 
    Parcel current;
    if (!tree.searchByID(id, current)) {
        return;                              // FR6: empty tree / non-existent record already reported
    }
 
    cout << "\nCurrent record:\n";
    printParcel(current);
    cout << "\n(Press Enter to keep the current value. Status is changed in Task 1/2/3.)\n";
 
    Parcel updated = current;
    string input;
 
    cout << "New Recipient Name [" << current.recipientName << "]: ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) updated.recipientName = input;
 
    cout << "New Category [" << current.parcelType << "]: ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) updated.parcelType = input;
 
    cout << "New Weight in kg [" << current.weight << "]: ";       // [WEIGHT]
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        double w = atof(input.c_str());
        if (w > 0.0 && w <= 100.0) updated.weight = w;
        else cout << "  [Info] Invalid weight ignored, keeping " << current.weight << " kg.\n";
    }
 
    if (!intake.updateParcelDetails(id, updated.recipientName, updated.parcelType, updated.weight)) {
        cout << "  [Error] Update failed: completed or cancelled parcels cannot be edited.\n";
        return;
    }
    tree.update(id, updated);                // mirror in the BST (key unchanged, structure kept)
    cout << "\nParcel updated successfully.\n";
}
 
// FR5: remove. Task 1 decides whether the parcel may be removed (only finished parcels),
// then the BST node is deleted (0 / 1 / 2 children cases).
void ParcelCategoryManager::removeParcel() {
    cout << "\n========== REMOVE PARCEL ==========\n";
    string id = readRequired("Enter Parcel ID: ");
 
    Parcel current;
    if (!tree.searchByID(id, current)) {
        return;                              // FR6
    }
 
    printParcel(current);
    if (!intake.isParcelRemovable(id)) {   // [FROM DIFF] check Task 1's rule BEFORE asking for confirmation
        cout << "  [Error] Active parcel cannot be removed. Complete or cancel it first.\n";
        return;
    }
    cout << "Confirm permanent removal? (Y/N): ";
    string confirmation;
    getline(cin, confirmation);
    confirmation = trim(confirmation);
    if (confirmation != "Y" && confirmation != "y") {
        cout << "Removal cancelled.\n";
        return;
    }
 
    if (!intake.removeParcelRecord(id)) {
        cout << "  [Error] Active parcel cannot be removed. Complete or cancel it first.\n";
        return;
    }
    tree.remove(id);                         // BST deletion keeps the tree valid
    cout << "Parcel record removed successfully.\n";
}
 
void ParcelCategoryManager::runMenu() {
    while (true) {
        syncFromIntake();    // always show the latest data from Tasks 1-4
 
        cout << "\n========================================\n";
        cout << " TASK 5: PARCEL CATEGORY ORGANISATION\n";
        cout << " (BST index, " << tree.size() << " parcels)\n";
        cout << "========================================\n";
        cout << "1. Insert New Parcel\n";
        cout << "2. Search Parcel by ID\n";
        cout << "3. Search Parcels by Category\n";
        cout << "4. Display All Parcels (sorted by Parcel ID)\n";
        cout << "5. Update Parcel Record\n";
        cout << "6. Remove Parcel Record\n";
        cout << "0. Back to Main Menu\n";
 
        int choice = readChoice(0, 6);
        switch (choice) {
            case 1: insertParcel();     break;
            case 2: searchByID();       break;
            case 3: searchByCategory(); break;
            case 4: displayAll();       break;
            case 5: updateParcel();     break;
            case 6: removeParcel();     break;
            case 0: return;
        }
    }
}