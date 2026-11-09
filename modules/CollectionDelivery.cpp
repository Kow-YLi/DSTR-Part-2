#include "CollectionDelivery.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>

using namespace std;

namespace {


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

} 

// ==================================================
// Constructor
// ==================================================


CollectionDelivery::CollectionDelivery(ParcelIntake& sharedIntake)
    : intake(sharedIntake), riderCount(0), logger(nullptr), service(nullptr) {      

    
    addRider("R1", "Aiman");
    addRider("R2", "Siti");
    addRider("R3", "Kumar");
    addRider("R4", "Mei Ling");
}


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


void CollectionDelivery::joinCollectionQueue() {
    cout << "\n========== JOIN COLLECTION QUEUE ==========\n";

    if (counterQueue.isFull()) {
        cout << "Error: Counter waiting area is full ("
             << counterQueue.capacity()
             << " customers). Please come back later.\n";
        return;
    }

    string parcelID = readRequired("Enter Parcel ID to collect: ");

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

    if(logger != nullptr){
        logger->recordActivity(parcelID, ACT_ASSIGN_RESOURCE,
                               parcel.status, parcel.status,
                               parcel.nextAction, parcel.nextAction,
                               findLinkedRequest(parcelID), "COUNTER");
    }

    cout << "\nAdded to collection queue.\n";
    cout << "Parcel ID      : " << parcelID << "\n";
    cout << "Recipient      : " << parcel.recipientName << "\n";
    cout << "Queue Position : " << counterQueue.size() << "\n";
}

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

    
    string recipientID =
        readRequired("Enter customer's Recipient ID to verify: ");

    if (recipientID != parcel.recipientID) {
        cout << "Error: Recipient ID does not match. "
             << "Parcel not released.\n";
        counterQueue.dequeue();
        cout << "Customer removed from the queue.\n";
        return;
    }

    
    if (intake.updateParcelStatus(parcelID, "Collected", findLinkedRequest(parcelID), "COUNTER")) {
        counterQueue.dequeue();
        cout << "\nParcel collected successfully!\n";
        cout << "Parcel ID : " << parcelID << "\n";
        cout << "Status    : Collected\n";
        completeLinkedRequest(parcelID);        //Changes: the parcel's task 2 request is completed too
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


    if(logger != nullptr){
        logger->recordActivity(parcelID, ACT_ASSIGN_RESOURCE,
                               parcel.status, parcel.status,
                               parcel.nextAction, parcel.nextAction,
                               findLinkedRequest(parcelID), 
                               riders[index].riderID);
    }

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
        if (intake.updateParcelStatus(rider.currentParcelID,        //Below tells task 4 which rider delivered it
                                      "Delivered", findLinkedRequest(rider.currentParcelID), rider.riderID)) {
            
            rider.completedDeliveries++;
            cout << "Parcel " << rider.currentParcelID
                 << " marked as Delivered.\n";
            completeLinkedRequest(rider.currentParcelID);   //Changes: the parcel's task 2 request is completed too
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

//New changes below: Attach task 4 activity logger
void CollectionDelivery::setLogger(ActivityLogger* activityLogger){
    logger = activityLogger;
}

//Remove one parcel from the counter queue, keeping the order of the others.
//Use only circular-queue operations (peek/dequeue/enqueue)
bool CollectionDelivery::removeFromCounterQueue(const string& parcelID){
    bool removed = false;
    int originalSize = counterQueue.size();
    string current;

    for(int i = 0; i < originalSize; i++) {
        counterQueue.peek(current);
        counterQueue.dequeue();

        if(!removed && current == parcelID){
            removed = true;     //drop this counter
        }else{
            counterQueue.enqueue(current);      //everyone else rejoins in order
        }
    }
    return removed;
}

//Attach task 2 (service requests)
void CollectionDelivery::setServiceManager(ServiceManager* serviceManager){
    service = serviceManager;
}

//task 2 request ID of this parcel's Active request ("-" if none / task 2 not attached)
string CollectionDelivery::findLinkedRequest(const string& parcelID) const{
    string requestID = "-";
    if(service != nullptr){
        service->findActiveRequestForParcel(parcelID, requestID);
    }
    return requestID;
}

//Mark the parcel's active task 2 request as Completed and tell the user
void CollectionDelivery::completeLinkedRequest(const string& parcelID){
    if(service == nullptr){
        return;
    }
    string requestID = findLinkedRequest(parcelID);
    if(service->completeRequestForParcel(parcelID)){
        cout << "Linked service request " << requestID << " marked as Completed.\n";
    }
}

//used by task 4 undo of an ASSIGN_RESOURCE activity
bool CollectionDelivery::releaseAssignment(const string& parcelID, const string& resourceID){
    //Counter assignment: take the customer out of the waiting line
    if(resourceID == "COUNTER"){
        return removeFromCounterQueue(parcelID);
    }

    //Rider assignment: the same rider must still be carrying this parcel
    int index = findRiderByParcel(parcelID);

    if(index == -1 || riders[index].riderID != resourceID || riderRotation.isFull()){
        return false;
    }

    //Free the rider and put them back in the rotation (at the rear)
    riders[index].status = "Available";
    riders[index].currentParcelID = "-";
    riderRotation.enqueue(index);
    return true;
}

