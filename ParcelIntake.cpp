
#include "ParcelIntake.hpp"
#include <iostream>
#include <fstream>
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

// Read a single-digit menu choice
int readChoice(int minimum, int maximum) {
    string input;

    while (true) {
        cout << "Enter choice: ";
        getline(cin, input);
        input = trim(input);

        if (input.length() == 1 &&
            input[0] >= '0' &&
            input[0] <= '9') {

            int choice = input[0] - '0';

            if (choice >= minimum && choice <= maximum) {
                return choice;
            }
        }

        cout << "Invalid choice. Please try again.\n";
    }
}

// Check date/time format: YYYY-MM-DD HH:MM
bool isValidDateTime(const string& value) {
    if (value.length() != 16 ||
        value[4] != '-' ||
        value[7] != '-' ||
        value[10] != ' ' ||
        value[13] != ':') {
        return false;
    }

    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) {
            continue;
        }

        if (!isdigit(static_cast<unsigned char>(value[i]))) {
            return false;
        }
    }

    int year = stoi(value.substr(0, 4));
    int month = stoi(value.substr(5, 2));
    int day = stoi(value.substr(8, 2));
    int hour = stoi(value.substr(11, 2));
    int minute = stoi(value.substr(14, 2));

    if (year < 2000 || month < 1 || month > 12 ||
        hour > 23 || minute > 59) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear =
        (year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0);

    if (leapYear) {
        daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

// Check whether a parcel is in a final state
bool isCompleted(const string& status) {
    return status == "Collected" ||
           status == "Delivered" ||
           status == "Returned" ||
           status == "Cancelled";
}

// Check whether an operation is permitted
bool canChangeStatus(
    const string& current,
    const string& next
) {
    if (current == next || isCompleted(current)) {
        return false;
    }

    if (next == "Cancelled" || next == "Missing") {
        return current != "Missing";
    }

    if (next == "On Hold") {
        return current != "On Hold";
    }

    if (next == "Pending Sorting") {
        return current == "On Hold" ||
               current == "Special Handling";
    }

    if (next == "Collected") {
        return current == "Ready for Collection";
    }

    if (next == "Delivered") {
        return current == "Awaiting Delivery Assignment";
    }

    if (next == "Returned") {
        return current == "Return to Sender";
    }

    return false;
}

} // namespace

// ==================================================
// Constructor and Destructor
// ==================================================

ParcelIntake::ParcelIntake()
    : head(nullptr), front(nullptr), rear(nullptr) {
}

ParcelIntake::~ParcelIntake() {
    // Delete all queue nodes
    while (front != nullptr) {
        dequeue();
    }

    // Delete all linked-list nodes
    while (head != nullptr) {
        ParcelNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// Singly Linked List Operations

// Search for a parcel node using Parcel ID
ParcelIntake::ParcelNode*
ParcelIntake::findParcelNode(
    const string& parcelID
) const {
    ParcelNode* current = head;

    while (current != nullptr) {
        if (current->data.parcelID == parcelID) {
            return current;
        }

        current = current->next;
    }

    return nullptr;
}

// Insert parcel at the end of the linked list
void ParcelIntake::insertParcel(const Parcel& parcel) {
    ParcelNode* newNode = new ParcelNode{parcel, nullptr};

    if (head == nullptr) {
        head = newNode;
        return;
    }

    ParcelNode* current = head;

    while (current->next != nullptr) {
        current = current->next;
    }

    current->next = newNode;
}

// FIFO Queue Operations

bool ParcelIntake::isQueueEmpty() const {
    return front == nullptr;
}

// Check whether a Parcel ID is in the queue
bool ParcelIntake::isQueued(const string& parcelID) const {
    QueueNode* current = front;

    while (current != nullptr) {
        if (current->parcelID == parcelID) {
            return true;
        }

        current = current->next;
    }

    return false;
}

// Add a parcel to the rear of the queue
void ParcelIntake::enqueue(const string& parcelID) {
    if (isQueued(parcelID)) {
        return;
    }

    QueueNode* newNode = new QueueNode{parcelID, nullptr};

    if (rear == nullptr) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

// Remove the parcel at the front
void ParcelIntake::dequeue() {
    if (isQueueEmpty()) {
        cout << "Error: Incoming queue is empty.\n";
        return;
    }

    QueueNode* temp = front;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
}

// Remove a specific Parcel ID from the queue
void ParcelIntake::removeFromQueue(const string& parcelID) {
    QueueNode* current = front;
    QueueNode* previous = nullptr;

    while (current != nullptr) {
        if (current->parcelID == parcelID) {

            if (previous == nullptr) {
                front = current->next;
            } else {
                previous->next = current->next;
            }

            if (current == rear) {
                rear = previous;
            }

            delete current;
            return;
        }

        previous = current;
        current = current->next;
    }
}

// Display Parcel Details

void ParcelIntake::displayParcel(const Parcel& parcel) const {
    cout << "\n----------------------------------------\n";
    cout << "Parcel ID        : " << parcel.parcelID << "\n";
    cout << "Recipient Name   : " << parcel.recipientName << "\n";
    cout << "Recipient ID     : " << parcel.recipientID << "\n";
    cout << "Delivery Company : " << parcel.deliveryCompany << "\n";
    cout << "Parcel Type      : " << parcel.parcelType << "\n";
    cout << "Arrival DateTime : " << parcel.arrivalDateTime << "\n";
    cout << "Status           : " << parcel.status << "\n";
    cout << "Next Action      : " << parcel.nextAction << "\n";
    cout << "----------------------------------------\n";
}

// Requirement 1: Register New Parcel

void ParcelIntake::registerParcel() {
    Parcel parcel;

    cout << "\n========== REGISTER NEW PARCEL ==========\n";

    parcel.parcelID = readRequired("Enter Parcel ID: ");

    // Duplicate ID validation
    if (findParcelNode(parcel.parcelID) != nullptr) {
        cout << "Error: Parcel ID already exists.\n";
        return;
    }

    parcel.recipientName = readRequired("Enter Recipient Name: ");
    parcel.recipientID = readRequired("Enter Recipient ID: ");
    parcel.deliveryCompany =
        readRequired("Enter Delivery Company: ");
    parcel.parcelType = readRequired("Enter Parcel Type: ");

    // Validate arrival date/time
    while (true) {
        parcel.arrivalDateTime =
            readRequired("Enter Arrival (YYYY-MM-DD HH:MM): ");

        if (isValidDateTime(parcel.arrivalDateTime)) {
            break;
        }

        cout << "Error: Invalid date/time.\n";
    }

    parcel.status = "Pending Sorting";
    parcel.nextAction = "Not Assigned";

    // Insert record into linked list
    insertParcel(parcel);

    // Add ID to the incoming processing queue
    enqueue(parcel.parcelID);

    cout << "\nParcel registered successfully!\n";
    cout << "Parcel ID: " << parcel.parcelID << "\n";
    cout << "Status: " << parcel.status << "\n";
}


// Requirement 2: Display and Search Records

void ParcelIntake::displayAllParcels() const {
    cout << "\n========== ALL PARCEL RECORDS ==========\n";

    if (head == nullptr) {
        cout << "No parcel records available.\n";
        return;
    }

    ParcelNode* current = head;
    int count = 0;

    while (current != nullptr) {
        displayParcel(current->data);
        current = current->next;
        count++;
    }

    cout << "\nTotal Registered Parcels: " << count << "\n";
}

void ParcelIntake::searchParcel() const {
    cout << "\n========== SEARCH PARCEL ==========\n";

    string parcelID = readRequired("Enter Parcel ID: ");

    ParcelNode* found = findParcelNode(parcelID);

    if (found == nullptr) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    cout << "Parcel found!\n";
    displayParcel(found->data);
}

// Integration: retrieve a copy of a parcel
bool ParcelIntake::getParcel(
    const string& parcelID,
    Parcel& result
) const {
    ParcelNode* found = findParcelNode(parcelID);

    if (found == nullptr) {
        return false;
    }

    result = found->data;
    return true;
}


// Requirement 3: Display Incoming Queue

void ParcelIntake::displayPendingQueue() const {
    cout << "\n========== INCOMING PARCEL QUEUE ==========\n";

    if (isQueueEmpty()) {
        cout << "No parcels waiting for processing.\n";
        return;
    }

    QueueNode* current = front;
    int position = 1;

    while (current != nullptr) {
        cout << position << ". "
             << current->parcelID;

        if (current == front) {
            cout << " [FRONT]";
        }

        if (current == rear) {
            cout << " [REAR]";
        }

        cout << "\n";

        current = current->next;
        position++;
    }

    cout << "Total Pending: " << position - 1 << "\n";
}

// Requirement 3: Sort and Process Incoming Parcel

void ParcelIntake::processNextParcel() {
    cout << "\n========== PROCESS NEXT PARCEL ==========\n";

    if (isQueueEmpty()) {
        cout << "Error: No parcels waiting for processing.\n";
        return;
    }

    string parcelID = front->parcelID;
    ParcelNode* found = findParcelNode(parcelID);

    // Prevent processing an invalid record
    if (found == nullptr) {
        cout << "Error: Parcel record not found.\n";
        dequeue();
        return;
    }

    Parcel& parcel = found->data;

    // Prevent processing a parcel in an invalid state
    if (parcel.status != "Pending Sorting") {
        cout << "Error: Parcel is not pending sorting.\n";
        cout << "Current Status: " << parcel.status << "\n";
        dequeue();
        return;
    }

    displayParcel(parcel);

    cout << "\nSelect Next Required Action:\n";
    cout << "1. Prepare for Recipient Collection\n";
    cout << "2. Assign for Campus Delivery\n";
    cout << "3. Send for Special Handling\n";
    cout << "4. Process Return to Sender\n";
    cout << "5. Place on Hold for Investigation\n";

    int choice = readChoice(1, 5);

    switch (choice) {
        case 1:
            parcel.status = "Ready for Collection";
            parcel.nextAction = "Collection";
            break;

        case 2:
            parcel.status = "Awaiting Delivery Assignment";
            parcel.nextAction = "Campus Delivery";
            break;

        case 3:
            parcel.status = "Special Handling";
            parcel.nextAction = "Special Handling";
            break;

        case 4:
            parcel.status = "Return to Sender";
            parcel.nextAction = "Return";
            break;

        case 5:
            parcel.status = "On Hold";
            parcel.nextAction = "Investigation";
            break;
    }

    // Remove processed parcel from the intake queue
    dequeue();

    cout << "\nParcel processed successfully!\n";
    cout << "Parcel ID   : " << parcel.parcelID << "\n";
    cout << "New Status  : " << parcel.status << "\n";
    cout << "Next Action : " << parcel.nextAction << "\n";
}


// Requirement 4: Status Changes
// Controlled status updates for integrated modules

bool ParcelIntake::updateParcelStatus(
    const string& parcelID,
    const string& newStatus
) {
    ParcelNode* found = findParcelNode(parcelID);

    if (found == nullptr) {
        return false;
    }

    Parcel& parcel = found->data;

    if (!canChangeStatus(parcel.status, newStatus)) {
        return false;
    }

    // Restore eligible parcels to the intake queue
    if (newStatus == "Pending Sorting") {
        enqueue(parcelID);
        parcel.nextAction = "Not Assigned";
    } else {
        // Remove parcels that must no longer be sorted
        removeFromQueue(parcelID);
    }

    parcel.status = newStatus;

    if (newStatus == "Cancelled") {
        parcel.nextAction = "None";
    } else if (newStatus == "Missing" ||
               newStatus == "On Hold") {
        parcel.nextAction = "Investigation";
    } else if (newStatus == "Collected" ||
               newStatus == "Delivered" ||
               newStatus == "Returned") {
        parcel.nextAction = "Completed";
    }

    return true;
}

// Requirement 4: Update Parcel Record

void ParcelIntake::updateParcel() {
    cout << "\n========== UPDATE PARCEL ==========\n";

    string parcelID = readRequired("Enter Parcel ID: ");
    ParcelNode* found = findParcelNode(parcelID);

    if (found == nullptr) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    Parcel& parcel = found->data;

    if (isCompleted(parcel.status)) {
        cout << "Error: Completed/cancelled parcel "
             << "cannot be edited.\n";
        return;
    }

    displayParcel(parcel);

    cout << "\nSelect Update Option:\n";
    cout << "1. Recipient Name\n";
    cout << "2. Recipient ID\n";
    cout << "3. Delivery Company\n";
    cout << "4. Parcel Type\n";
    cout << "5. Mark as Collected\n";
    cout << "6. Mark as Delivered\n";
    cout << "7. Mark as Returned\n";
    cout << "8. Cancel Parcel\n";
    cout << "9. Release for Re-Sorting\n";

    int choice = readChoice(1, 9);
    bool updated = false;

    switch (choice) {
        case 1:
            parcel.recipientName =
                readRequired("Enter New Recipient Name: ");
            updated = true;
            break;

        case 2:
            parcel.recipientID =
                readRequired("Enter New Recipient ID: ");
            updated = true;
            break;

        case 3:
            parcel.deliveryCompany =
                readRequired("Enter New Delivery Company: ");
            updated = true;
            break;

        case 4:
            parcel.parcelType =
                readRequired("Enter New Parcel Type: ");
            updated = true;
            break;

        case 5:
            updated = updateParcelStatus(parcelID, "Collected");
            break;

        case 6:
            updated = updateParcelStatus(parcelID, "Delivered");
            break;

        case 7:
            updated = updateParcelStatus(parcelID, "Returned");
            break;

        case 8:
            updated = updateParcelStatus(parcelID, "Cancelled");
            break;

        case 9:
            updated =
                updateParcelStatus(parcelID, "Pending Sorting");
            break;
    }

    if (updated) {
        cout << "\nParcel updated successfully!\n";
        displayParcel(parcel);
    } else {
        cout << "Error: Invalid status transition.\n";
    }
}

// Requirement 4: Remove Parcel Record

void ParcelIntake::removeParcel() {
    cout << "\n========== REMOVE PARCEL ==========\n";

    string parcelID = readRequired("Enter Parcel ID: ");

    ParcelNode* current = head;
    ParcelNode* previous = nullptr;

    while (current != nullptr &&
           current->data.parcelID != parcelID) {
        previous = current;
        current = current->next;
    }

    if (current == nullptr) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    // Only terminal records can be physically removed
    if (!isCompleted(current->data.status)) {
        cout << "Error: Active parcel cannot be removed.\n";
        cout << "Complete or cancel it first.\n";
        return;
    }

    displayParcel(current->data);

    cout << "Confirm permanent removal? (Y/N): ";
    string confirmation;
    getline(cin, confirmation);

    if (confirmation != "Y" && confirmation != "y") {
        cout << "Removal cancelled.\n";
        return;
    }

    removeFromQueue(parcelID);

    if (previous == nullptr) {
        head = current->next;
    } else {
        previous->next = current->next;
    }

    delete current;

    cout << "Parcel record removed successfully.\n";
}

// Requirement 5: Missing Parcel Handling

void ParcelIntake::reportMissingParcel() {
    cout << "\n========== REPORT MISSING PARCEL ==========\n";

    string parcelID = readRequired("Enter Parcel ID: ");
    ParcelNode* found = findParcelNode(parcelID);

    if (found == nullptr) {
        cout << "Error: Parcel not found.\n";
        return;
    }

    if (found->data.status == "Missing") {
        cout << "Error: Parcel already reported missing.\n";
        return;
    }

    if (isCompleted(found->data.status)) {
        cout << "Error: Cannot report a completed "
             << "or cancelled parcel as missing.\n";
        return;
    }

    if (updateParcelStatus(parcelID, "Missing")) {
        cout << "Parcel reported missing successfully.\n";
        cout << "Parcel ID: " << parcelID << "\n";
        cout << "Status: Missing\n";
        cout << "Next Action: Investigation\n";
    } else {
        cout << "Error: Cannot update parcel status.\n";
    }
}
