#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <iostream>
#include <string>

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

    bool higherPriority(int a, int b);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    ServiceManager();

    void createRequest();
    void displayRequests();
    void searchRequest();
    void processRequest();
    void cancelRequest();
};

#endif
