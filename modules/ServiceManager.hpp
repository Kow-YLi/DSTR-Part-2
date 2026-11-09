#ifndef SERVICEMANAGER_HPP
#define SERVICEMANAGER_HPP

#include <iostream>
#include <string>
#include "ParcelIntake.hpp"
#include "../models/ActivityLogger.hpp"

using namespace std;

const int MAX = 100;

struct ServiceRequest {
    string requestID;
    string parcelID;
    string recipientName;
    string serviceType;
    int priority;
    int requestTime;
    string status;
};

class ServiceManager {
private:
    ServiceRequest requests[MAX];
    ServiceRequest history[MAX];

    int size;
    int historySize;
    int totalRequests;
    int nextRequestTime;

    //Chenges: Shared parcel data(Task 1) and Task 4 hook
    ParcelIntake& intake;
    ActivityLogger* logger;     //may be nullptr when Task 2 runs on its own

    bool higherPriority(int a, int b);
    void heapifyUp(int index);
    void heapifyDown(int index);

    //Chenges: new helper: move one active request into the history with a final status and repair the heap
    // same steps as cancelRequest()
    void removeRequestAt(int index, const string& finalStatus);

public:
    ServiceManager(ParcelIntake& sharedIntake);     //Changes: needs the shared ParcelIntake object

    void createRequest();
    void displayRequests();
    void searchRequest();
    void processRequest();
    void cancelRequest();
    void runMenu();     //Changes:the menu that used to be in Task 2 own main.cpp
    void setLogger(ActivityLogger* activityLogger);     //Changes: Attach the Task 4 activity logger

    //Changes: Task 4 undo support: cancel an Active request by ID (return false if the request in not active)
    bool removeActiveRequest(const string & requestID);

    //Changes: used by Task 3 (collection counter or riders): find the Active request of a parcel (false if the parcel has none)
    bool findActiveRequestForParcel(const string& parcelID, string& requestID) const;

    //Changes: used by Task 3 once a collection or delivery is done: marks the parcel active request "Completed"
    //Then update status action record in Task 4
    bool completeRequestForParcel(const string& parcelID);


};
#endif
