#include "ServiceManager.hpp"


// Constructor
//Changes: takes the shared ParcelIntake
ServiceManager::ServiceManager(ParcelIntake& sharedIntake) : intake(sharedIntake), logger(nullptr){
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

    //Changes: the parcel must exist in the shared Task 1 records
    Parcel parcel;
    if(!intake.getParcel(newRequest.parcelID, parcel)){
        cout << "Parcel not found! Please register it first.\n";
        return;
    }

    //Changes: Only one active request per parcel
    for(int i = 0; i < size; i++){
        if(requests[i].parcelID == newRequest.parcelID){
            cout << "This parcel already has an active request (" << requests[i].requestID << ")!\n";
            return;
        }
    }

    cout << "Enter Recipient Name: ";
    cin >> ws;
    getline(cin, newRequest.recipientName);

    if (newRequest.recipientName.empty()) {
        cout << "Recipient name cannot be empty!\n";
        return;
    }

    //Changes: the name must match the shared parcel record 
    if(newRequest.recipientName != parcel.recipientName){
        cout << "Recipient name does not match the parcel record (" << parcel.recipientName << ")!\n";
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

    //Changes: the service type must match how task 1 sorted the parcel
    string requiredStatus;
    if(newRequest.serviceType == "Special Handing"){
        requiredStatus = "Special Handling";
    }else if(newRequest.serviceType == "Re-delivery" || newRequest.serviceType == "Campus Delivery"){
        requiredStatus = "Awaiting Delivery Assignment";
    }else if(newRequest.serviceType == "Recipient Collection"){
        requiredStatus = "Ready for Collection";
    }else{
        requiredStatus = "Return to Sender";
    }

    if(parcel.status != requiredStatus){
        cout << "This service needs the parcel status to be '" << requiredStatus << "'.\nCurrent parcel status: " << parcel.status << "\n";
        return;
    }

    newRequest.requestTime = nextRequestTime++;
    newRequest.status = "Pending";

    requests[size] = newRequest;
    heapifyUp(size);
    size++;

    totalRequests++;

    //Changes: Task 4: record the CREATE_REQUEST acitivity
    //Parcel status does not change when a request is created
    if(logger != nullptr){
        logger->recordActivity(newRequest.parcelID, ACT_CREATE_REQUEST, parcel.status, parcel.status, parcel.nextAction, parcel.nextAction, newRequest.requestID, "-");
    }   

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

    //Changes: update the shared parcel record (Task1) when the service finiahes the parcel's journey
    bool parcelUpdated = true;
    if(processedRequest.serviceType == "Return to Sender"){
        parcelUpdated = intake.updateParcelStatus(processedRequest.parcelID, "Returned", processedRequest.requestID, "-");
    }else if(processedRequest.serviceType == "Special Handling"){
        //speacial handling finished: release the parcel for re-sorting
        parcelUpdated = intake.updateParcelStatus(
            processedRequest.parcelID, "Pending Sorting",
            processedRequest.requestID, "-"
        );
    }
    if(!parcelUpdated){
        cout << "Warning: the parcel status was not updated " << "(it was changed in another module).\n";
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

    //Changes: Task 4: record the PROCESS_REQUEST activity
    Parcel processedParcel;
    if(logger != nullptr && intake.getParcel(processedRequest.parcelID, processedParcel)){
        logger->recordActivity(processedRequest.parcelID, ACT_PROCESS_REQUEST, 
                                processedParcel.status, processedParcel.status, 
                                processedParcel.nextAction, processedParcel.nextAction, 
                                processedRequest.requestID,  "-");
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

    //Changes: keep the parcel ID: the request leaves the heap below
    string cancelledParcelID = requests[index].parcelID;

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

    //Changes: Task 4: record the CANCEL_REQUEST activity
    Parcel cancelledParcel;
    if(logger != nullptr && intake.getParcel(cancelledParcelID, cancelledParcel)){
        logger->recordActivity(cancelledParcelID, ACT_CANCEL_REQUEST, 
                               cancelledParcel.status, cancelledParcel.status, 
                               cancelledParcel.nextAction, cancelledParcel.nextAction, 
                               cancelID, "-");
                                
    }

    cout << "\nRequest cancelled successfully!\n";

}

//New changes below: Attach the Task 4 activity logger
void ServiceManager::setLogger(ActivityLogger* activityLogger){
    logger = activityLogger;
}

// Used by task 4 undo of a CREATE_REQUEST activity
//Same removal steps as cancelRequest() above, but by request ID and silent
bool ServiceManager::removeActiveRequest(const string& requestID){
    int index = -1;
    for(int i =0; i < size; i++){
        if(requests[i].requestID == requestID){
            index = i;
            break;
        }
    }
    if(index == -1){
        return false;       //already processed or cancelled
    }

    requests[index].status = "Cancelled";
    
    history[historySize] = requests[index];
    historySize++;

    requests[index] = requests[size - 1];
    size--;

    if(index < size){
        if(index > 0 && higherPriority(index, (index - 1) / 2)){
            heapifyUp(index);
        }else{
            heapifyDown(index);
        }
    }
    return true;
}

//Same removal steps as cancelRequest(), with a chosen final status
void ServiceManager::removeRequestAt(int index, const string& finalStatus){
    requests[index].status = finalStatus;

    history[historySize] = requests[index];
    historySize++;

    requests[index] = requests[size - 1];
    size--;

    if(index < size){
        if(index > 0 && higherPriority(index, (index - 1) / 2)){
            heapifyUp(index);
        }else{
            heapifyDown(index);
        }
    }
}

// Used by task 3: which active request belongs to this parcel?
bool ServiceManager::findActiveRequestForParcel(
    const string& parcelID, string& requestID
)const{
    for(int i = 0; i < size; i++){
        if (requests[i].parcelID == parcelID) {
            requestID = requests[i].requestID;
            return true;
        }
    }
    return false;
}

// Used by Task 3 after a collection / delivery is completed
bool ServiceManager::completeRequestForParcel(const string& parcelID){
    for(int i = 0; i < size; i++){
        if(requests[i].parcelID == parcelID){
            string requestID = requests[i].requestID;
            removeRequestAt(i, "Completed");

            //Task 4: record the PROCESS_REQUEST activity
            Parcel parcel;
            if(logger != nullptr && intake.getParcel(parcelID, parcel)){
                logger->recordActivity(parcelID, ACT_PROCESS_REQUEST, parcel.status, parcel.status, 
                                       parcel.nextAction, parcel.nextAction, requestID, "-");
            }
            return true;
        }
    }
    return false;
}

//The menu from task 2 main.cpp
void ServiceManager::runMenu(){
    int choice = -1;

    do{
        cout << "\n========== SERVICE MANAGEMENT MENU ==========\n";
        cout << "1. Create Service Request\n";
        cout << "2. Display Active Requests\n";
        cout << "3. Search Service Request\n";
        cout << "4. Process Next Request\n";
        cout << "5. Cancel Service Request\n";
        cout << "0. Back\n";   // [ORIGINAL] "0. Exit"
        cout << "Enter choice: ";

        if(!(cin >> choice)){
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch(choice){
            case 1:
            createRequest();
            break;

            case 2:
            displayRequests();
            break;

            case 3:
            searchRequest();
            break;

            case 4:
            processRequest();
            break;

            case 5:
            cancelRequest();
            break;

            case 0:
            cout << "\nReturning to the main menu.\n";
            break;

            default:
            cout << "\nInvalid choice! Please choose 0 to 5.\n";
        }
    } while(choice != 0);

    // Task 2 reads with cin >> (leaves the Enter key in the input).
    // The integrated main menu reads whole lines with getline, so that leftover enter here,
    // otherwise the main menu would show "Invalid choice".
    cin.ignore(1000, '\n');
}