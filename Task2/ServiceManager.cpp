#include "ServiceManager.h"

// Constructor
ServiceManager::ServiceManager() {
    size = 0;
    historySize = 0;
    totalRequests = 0;
    nextRequestTime = 1;
}

// Check which request has higher priority
bool ServiceManager::higherPriority(int a, int b) {
    if (requests[a].priority > requests[b].priority) {
        return true;
    }

    if (requests[a].priority == requests[b].priority &&
        requests[a].requestTime < requests[b].requestTime) {
        return true;
    }

    return false;
}

// Move request upwards
void ServiceManager::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (higherPriority(index, parent)) {
            ServiceRequest temp = requests[index];
            requests[index] = requests[parent];
            requests[parent] = temp;
            index = parent;
        }
        else {
            break;
        }
    }
}

// Move request downwards
void ServiceManager::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int highest = index;

        if (left < size && higherPriority(left, highest)) {
            highest = left;
        }

        if (right < size && higherPriority(right, highest)) {
            highest = right;
        }

        if (highest != index) {
            ServiceRequest temp = requests[index];
            requests[index] = requests[highest];
            requests[highest] = temp;
            index = highest;
        }
        else {
            break;
        }
    }
}

// Create a new service request
void ServiceManager::createRequest() {
    if (totalRequests >= MAX) {
        cout << "\nMaximum request limit reached!\n";
        return;
    }

    ServiceRequest newRequest;

    cout << "\nEnter Request ID: ";
    cin >> newRequest.requestID;

    // Check duplicate ID in active requests
    for (int i = 0; i < size; i++) {
        if (requests[i].requestID == newRequest.requestID) {
            cout << "Request ID already exists!\n";
            return;
        }
    }

    // Check duplicate ID in history
    for (int i = 0; i < historySize; i++) {
        if (history[i].requestID == newRequest.requestID) {
            cout << "Request ID already exists in history!\n";
            return;
        }
    }

    cout << "Enter Parcel ID: ";
    cin >> newRequest.parcelID;

    cout << "Enter Recipient Name: ";
    cin >> ws;
    getline(cin, newRequest.recipientName);

    if (newRequest.recipientName.empty()) {
        cout << "Recipient name cannot be empty!\n";
        return;
    }

    cout << "\nSelect Service Type:\n";
    cout << "1. Special Handling\n";
    cout << "2. Re-delivery\n";
    cout << "3. Campus Delivery\n";
    cout << "4. Recipient Collection\n";
    cout << "5. Return to Sender\n";

    int choice;
    cout << "Enter choice: ";

    if (!(cin >> choice)) {
        cout << "Invalid input! Please enter a number.\n";
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    switch (choice) {
        case 1:
            newRequest.serviceType = "Special Handling";
            newRequest.priority = 5;
            break;

        case 2:
            newRequest.serviceType = "Re-delivery";
            newRequest.priority = 4;
            break;

        case 3:
            newRequest.serviceType = "Campus Delivery";
            newRequest.priority = 3;
            break;

        case 4:
            newRequest.serviceType = "Recipient Collection";
            newRequest.priority = 2;
            break;

        case 5:
            newRequest.serviceType = "Return to Sender";
            newRequest.priority = 1;
            break;

        default:
            cout << "Invalid service type! Choose from 1 to 5.\n";
            return;
    }

    newRequest.requestTime = nextRequestTime++;
    newRequest.status = "Pending";

    requests[size] = newRequest;
    heapifyUp(size);
    size++;

    totalRequests++;

    cout << "\nService request created successfully!\n";
}

// Display active requests
void ServiceManager::displayRequests() {
    if (size == 0) {
        cout << "\nNo active service requests found.\n";
        return;
    }

    cout << "\n========== ACTIVE SERVICE REQUESTS ==========\n";

    for (int i = 0; i < size; i++) {
        cout << "\nRequest ID: " << requests[i].requestID;
        cout << "\nParcel ID: " << requests[i].parcelID;
        cout << "\nRecipient: " << requests[i].recipientName;
        cout << "\nService Type: " << requests[i].serviceType;
        cout << "\nPriority: " << requests[i].priority;
        cout << "\nStatus: " << requests[i].status;
        cout << "\n---------------------------------------------\n";
    }
}

// Search for a request
void ServiceManager::searchRequest() {
    string searchID;

    cout << "\nEnter Request ID to search: ";
    cin >> searchID;

    for (int i = 0; i < size; i++) {
        if (requests[i].requestID == searchID) {
            cout << "\nRequest found!\n";
            cout << "Request ID: " << requests[i].requestID << endl;
            cout << "Parcel ID: " << requests[i].parcelID << endl;
            cout << "Recipient: " << requests[i].recipientName << endl;
            cout << "Service Type: " << requests[i].serviceType << endl;
            cout << "Priority: " << requests[i].priority << endl;
            cout << "Status: " << requests[i].status << endl;
            return;
        }
    }

    for (int i = 0; i < historySize; i++) {
        if (history[i].requestID == searchID) {
            cout << "\nRequest found in history!\n";
            cout << "Request ID: " << history[i].requestID << endl;
            cout << "Parcel ID: " << history[i].parcelID << endl;
            cout << "Recipient: " << history[i].recipientName << endl;
            cout << "Service Type: " << history[i].serviceType << endl;
            cout << "Priority: " << history[i].priority << endl;
            cout << "Status: " << history[i].status << endl;
            return;
        }
    }

    cout << "\nRequest ID not found!\n";
}


// Process the highest-priority request
void ServiceManager::processRequest() {
    if (size == 0) {
        cout << "\nNo active requests available to process!\n";
        return;
    }

    ServiceRequest processedRequest = requests[0];

    cout << "\nProcessing request...\n";
    cout << "Request ID: " << processedRequest.requestID << endl;
    cout << "Parcel ID: " << processedRequest.parcelID << endl;
    cout << "Service Type: " << processedRequest.serviceType << endl;

    // Set the final status
    if (processedRequest.serviceType == "Return to Sender") {
        processedRequest.status = "Returned";
    }
    else {
        processedRequest.status = "Completed";
    }

    // Store the request in history
    history[historySize] = processedRequest;
    historySize++;

    // Remove the highest-priority request
    requests[0] = requests[size - 1];
    size--;

    // Restore the heap structure
    if (size > 0) {
        heapifyDown(0);
    }

    cout << "\nRequest status updated to: "
         << processedRequest.status << endl;
}


// Cancel a service request
void ServiceManager::cancelRequest() {
    if (size == 0) {
        cout << "\nNo active requests available to cancel!\n";
        return;
    }

    string cancelID;

    cout << "\nEnter Request ID to cancel: ";
    cin >> cancelID;

    int index = -1;

    for (int i = 0; i < size; i++) {
        if (requests[i].requestID == cancelID) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        for (int i = 0; i < historySize; i++) {
            if (history[i].requestID == cancelID) {
                cout << "\nThis request is already "
                     << history[i].status << "!\n";
                return;
            }
        }

        cout << "\nRequest ID not found!\n";
        return;
    }

    requests[index].status = "Cancelled";

    history[historySize] = requests[index];
    historySize++;

    requests[index] = requests[size - 1];
    size--;

    if (index < size) {
        if (index > 0 &&
            higherPriority(index, (index - 1) / 2)) {
            heapifyUp(index);
        }
        else {
            heapifyDown(index);
        }
    }

    cout << "\nRequest cancelled successfully!\n";
}