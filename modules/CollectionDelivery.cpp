#include "CollectionDelivery.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>

using namespace std;

namespace {

// Remove whitespace from the start and end
string trim(const string& value) {
    size_t start = 0;
    size_t end = value.length();

    while (start < end &&
           isspace(static_cast<unsigned char>(value[start]))) {
        start++;
    }

    while (end > start &&
           isspace(static_cast<unsigned char>(value[end - 1]))) {
        end--;
    }

    return value.substr(start, end - start);
}

// Read a required field
string readRequired(const string& prompt) {
    string value;

    do {
        cout << prompt;
        getline(cin, value);
        value = trim(value);

        if (value.empty()) {
            cout << "Error: This field cannot be empty.\n";
        }

    } while (value.empty());

    return value;
}

// Read a Y/N answer
bool readYesNo(const string& prompt) {
    while (true) {
        string answer = readRequired(prompt);

        if (answer == "Y" || answer == "y") {
            return true;
        }

        if (answer == "N" || answer == "n") {
            return false;
        }

        cout << "Please enter Y or N.\n";
    }
}

} // namespace

// ==================================================
// Constructor
// ==================================================

CollectionDelivery::CollectionDelivery(ParcelIntake& sharedIntake)
    : intake(sharedIntake), riderCount(0) {

    // Default campus delivery riders
    addRider("R1", "Aiman");
    addRider("R2", "Siti");
    addRider("R3", "Kumar");
    addRider("R4", "Mei Ling");
}

// Add a rider and place them in the rotation
void CollectionDelivery::addRider(
    const string& riderID,
    const string& name
) {
    if (riderCount >= MAX_RIDERS) {
        return;
    }

    riders[riderCount] = {riderID, name, "Available", "-", 0};
    riderRotation.enqueue(riderCount);
    riderCount++;
}

// ==================================================
// Helpers
// ==================================================

// Check whether a parcel is already waiting at the counter
bool CollectionDelivery::isInCounterQueue(
    const string& parcelID
) const {
    string current;

    for (int i = 0; i < counterQueue.size(); i++) {
        counterQueue.getAt(i, current);

        if (current == parcelID) {
            return true;
        }
    }

    return false;
}

// Find which rider is carrying a parcel (-1 if none)
int CollectionDelivery::findRiderByParcel(
    const string& parcelID
) const {
    for (int i = 0; i < riderCount; i++) {
        if (riders[i].currentParcelID == parcelID) {
            return i;
        }
    }

    return -1;
}

// Find a rider by Rider ID (-1 if none)
int CollectionDelivery::findRiderByID(
    const string& riderID
) const {
    for (int i = 0; i < riderCount; i++) {
        if (riders[i].riderID == riderID) {
            return i;
        }
    }

    return -1;
}

// ==================================================
// Collection Counter (Circular Queue 1)
// ==================================================

// A recipient arrives and joins the counter line
void CollectionDelivery::joinCollectionQueue() {
    cout << "\n========== JOIN COLLECTION QUEUE ==========\n";

    if (counterQueue.isFull()) {
        cout << "Error: Counter waiting area is full ("
             << counterQueue.capacity()
             << " customers). Please come back later.\n";
        return;
    }

    string parcelID = readRequired("Enter Parcel ID to collect: ");

    // Read the shared record from Module 1
    Parcel parcel;
    if (!intake.getParcel(parcelID, parcel)) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    if (parcel.status != "Ready for Collection") {
        cout << "Error: Parcel is not ready for collection.\n";
        cout << "Current Status: " << parcel.status << "\n";
        return;
    }

    if (isInCounterQueue(parcelID)) {
        cout << "Error: This parcel is already in the "
             << "collection queue.\n";
        return;
    }

    counterQueue.enqueue(parcelID);

    cout << "\nAdded to collection queue.\n";
    cout << "Parcel ID      : " << parcelID << "\n";
    cout << "Recipient      : " << parcel.recipientName << "\n";
    cout << "Queue Position : " << counterQueue.size() << "\n";
}

// The counter serves the customer at the front
void CollectionDelivery::serveNextCustomer() {
    cout << "\n========== SERVE NEXT CUSTOMER ==========\n";

    string parcelID;
    if (!counterQueue.peek(parcelID)) {
        cout << "No customers waiting at the counter.\n";
        return;
    }

    Parcel parcel;

    // The parcel may have changed in another module
    // (e.g. reported missing) while the customer waited
    if (!intake.getParcel(parcelID, parcel) ||
        parcel.status != "Ready for Collection") {
        cout << "Error: Parcel " << parcelID
             << " is no longer available for collection.\n";
        counterQueue.dequeue();
        cout << "Customer removed from the queue.\n";
        return;
    }

    cout << "Now serving Parcel ID: " << parcelID << "\n";
    cout << "Recipient Name       : " << parcel.recipientName << "\n";
    cout << "Parcel Type          : " << parcel.parcelType << "\n";

    // Identity check before handing over the parcel
    string recipientID =
        readRequired("Enter customer's Recipient ID to verify: ");

    if (recipientID != parcel.recipientID) {
        cout << "Error: Recipient ID does not match. "
             << "Parcel not released.\n";
        counterQueue.dequeue();
        cout << "Customer removed from the queue.\n";
        return;
    }

    // Update the shared record through Module 1
    if (intake.updateParcelStatus(parcelID, "Collected")) {
        counterQueue.dequeue();
        cout << "\nParcel collected successfully!\n";
        cout << "Parcel ID : " << parcelID << "\n";
        cout << "Status    : Collected\n";
    } else {
        cout << "Error: Could not update parcel status.\n";
    }
}

// Show everyone waiting at the counter
void CollectionDelivery::displayCounterQueue() const {
    cout << "\n========== COLLECTION COUNTER QUEUE ==========\n";

    if (counterQueue.isEmpty()) {
        cout << "No customers waiting.\n";
        return;
    }

    string parcelID;
    Parcel parcel;

    for (int i = 0; i < counterQueue.size(); i++) {
        counterQueue.getAt(i, parcelID);

        cout << i + 1 << ". " << parcelID;

        if (intake.getParcel(parcelID, parcel)) {
            cout << " (" << parcel.recipientName << ")";
        }

        cout << "  [slot " << counterQueue.indexOf(i) << "]";

        if (i == 0) {
            cout << " [FRONT]";
        }

        if (i == counterQueue.size() - 1) {
            cout << " [REAR]";
        }

        cout << "\n";
    }

    cout << "Waiting: " << counterQueue.size()
         << " / " << counterQueue.capacity() << "\n";
}

// ==================================================
// Delivery Resources (Circular Queue 2)
// ==================================================

// Give a parcel to the next free rider (round-robin)
void CollectionDelivery::assignDelivery() {
    cout << "\n========== ASSIGN DELIVERY ==========\n";

    if (riderRotation.isEmpty()) {
        cout << "Error: All riders are currently on delivery.\n";
        return;
    }

    string parcelID = readRequired("Enter Parcel ID to deliver: ");

    Parcel parcel;
    if (!intake.getParcel(parcelID, parcel)) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    if (parcel.status != "Awaiting Delivery Assignment") {
        cout << "Error: Parcel is not awaiting delivery.\n";
        cout << "Current Status: " << parcel.status << "\n";
        return;
    }

    int existing = findRiderByParcel(parcelID);
    if (existing != -1) {
        cout << "Error: Parcel is already assigned to "
             << riders[existing].riderID << " ("
             << riders[existing].name << ").\n";
        return;
    }

    // Take the rider at the front of the rotation
    int index;
    riderRotation.peek(index);
    riderRotation.dequeue();

    riders[index].status = "On Delivery";
    riders[index].currentParcelID = parcelID;

    cout << "\nDelivery assigned successfully!\n";
    cout << "Parcel ID : " << parcelID << "\n";
    cout << "Rider     : " << riders[index].riderID
         << " (" << riders[index].name << ")\n";
    cout << "Deliver to: " << parcel.recipientName << "\n";
}

// A rider returns and confirms the delivery
void CollectionDelivery::completeDelivery() {
    cout << "\n========== COMPLETE DELIVERY ==========\n";

    string riderID = readRequired("Enter Rider ID: ");
    int index = findRiderByID(riderID);

    if (index == -1) {
        cout << "Error: Rider not found.\n";
        return;
    }

    Rider& rider = riders[index];

    if (rider.status != "On Delivery") {
        cout << "Error: " << rider.riderID
             << " has no active delivery.\n";
        return;
    }

    cout << "Rider " << rider.riderID << " (" << rider.name
         << ") is delivering " << rider.currentParcelID << ".\n";

    bool success = readYesNo("Was the parcel delivered? (Y/N): ");

    if (success) {
        // Update the shared record through Module 1
        if (intake.updateParcelStatus(rider.currentParcelID,
                                      "Delivered")) {
            rider.completedDeliveries++;
            cout << "Parcel " << rider.currentParcelID
                 << " marked as Delivered.\n";
        } else {
            cout << "Error: Parcel status could not be updated "
                 << "(it may have been changed in another module).\n";
        }
    } else {
        cout << "Delivery not completed. Parcel "
             << rider.currentParcelID
             << " stays Awaiting Delivery Assignment\n"
             << "and can be assigned again.\n";
    }

    // Free the rider and send them to the BACK of the rotation
    rider.status = "Available";
    rider.currentParcelID = "-";
    riderRotation.enqueue(index);

    cout << rider.riderID << " is available again "
         << "(rejoined the rotation at the rear).\n";
}

// Show all riders and the rotation order
void CollectionDelivery::displayRiders() const {
    cout << "\n========== DELIVERY RIDERS ==========\n";

    cout << left
         << setw(6) << "ID"
         << setw(12) << "Name"
         << setw(14) << "Status"
         << setw(10) << "Parcel"
         << "Completed\n";
    cout << "----------------------------------------------\n";

    for (int i = 0; i < riderCount; i++) {
        cout << left
             << setw(6) << riders[i].riderID
             << setw(12) << riders[i].name
             << setw(14) << riders[i].status
             << setw(10) << riders[i].currentParcelID
             << riders[i].completedDeliveries << "\n";
    }

    cout << "\nRotation order (next rider first):\n";

    if (riderRotation.isEmpty()) {
        cout << "No riders available.\n";
        return;
    }

    int index;
    for (int i = 0; i < riderRotation.size(); i++) {
        riderRotation.getAt(i, index);
        cout << i + 1 << ". " << riders[index].riderID
             << " (" << riders[index].name << ")";

        if (i == 0) {
            cout << " [NEXT]";
        }

        cout << "\n";
    }
}

// ==================================================
// Module Menu
// ==================================================

void CollectionDelivery::runMenu() {
    string input;

    while (true) {
        cout << "\n========================================\n";
        cout << " MODULE 3: COLLECTION COUNTER &\n";
        cout << "           DELIVERY RESOURCES\n";
        cout << "----------------------------------------\n";
        cout << "1. Join Collection Queue\n";
        cout << "2. Serve Next Customer\n";
        cout << "3. Display Counter Queue\n";
        cout << "4. Assign Parcel to Next Rider\n";
        cout << "5. Complete Delivery\n";
        cout << "6. Display Riders & Rotation\n";
        cout << "0. Back\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        getline(cin, input);
        input = trim(input);

        if (input.length() != 1 ||
            input[0] < '0' || input[0] > '6') {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        switch (input[0]) {
            case '1': joinCollectionQueue(); break;
            case '2': serveNextCustomer(); break;
            case '3': displayCounterQueue(); break;
            case '4': assignDelivery(); break;
            case '5': completeDelivery(); break;
            case '6': displayRiders(); break;
            case '0': return;
        }
    }
}