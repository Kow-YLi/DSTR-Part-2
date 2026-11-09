#ifndef ACTIVITY_MANAGER_HPP
#define ACTIVITY_MANAGER_HPP
#include <string>
#include "../models/ActivityLogger.hpp"
<<<<<<< HEAD
#include "ParcelIntake.hpp"
#include "ServiceManager.hpp"
#include "CollectionDelivery.hpp"
=======
#include "ParcelIntake.h"

>>>>>>> task3_collection-delivery

// One entry in the activity history
// ActivityRecord structure to hold details of each activity history entry. (stored inside the stack)
struct ActivityRecord {
    int activityId;
    std::string parcelId;
    ActivityType activityType;
    std::string previousStatus;
    std::string newStatus;
    std::string previousNextAction;
    std::string newNextAction;
    std::string requestId;
    std::string resourceId;
    std::string staffId;
    std::string timestamp;
    bool reversed;

};

//ActivityStack: history of activities using a linked list implementation
//ActivityStack class to manage the history of activities using a linked list implementation.
class ActivityStack{
    private:
        struct Node{
            ActivityRecord data;
            Node* next;
        };
        Node* topNode;
        int count;
        int capacity;

        ActivityStack(const ActivityStack&);
        ActivityStack& operator=(const ActivityStack&);
    
    public:
        explicit ActivityStack(int maxSize = 1000) : topNode(nullptr), count(0), capacity(maxSize) {}
        ~ActivityStack() {clear();}
        bool isEmpty() const {return topNode == nullptr;}
        bool isFull() const {return count >= capacity;}
        int size() const {return count;}

        bool push(const ActivityRecord& rec) {
            if (isFull()) return false;
            Node* n = new Node{rec, topNode};
            topNode = n;
            count++;
            return true;
        }

        bool pop(ActivityRecord& out) {
            if (isEmpty()) return false;
            Node* n = topNode;
            out = n->data;
            topNode = n->next;
            delete n;
            count--;
            return true;
        }

        bool peek(ActivityRecord& out) const {
            if (isEmpty()) return false;
            out = topNode->data;
            return true;
        }

        void clear() {
            ActivityRecord dummy;
            while (pop(dummy)){}
        }


};

//ActivityManager: records, displays and reverses activities (Task 4)
class ActivityManager : public ActivityLogger{
    private:
    ActivityStack history;
    ParcelIntake& intake;       //Task 1 - shared parcel records
    ServiceManager& service;    //Task 2 - undo of service requests
    CollectionDelivery& delivery; //Task 3 - undo of counter / rider assignment
    int nextActivityId;
    std::string currentStaffId;

    static void printRecord(const ActivityRecord& r);

    // Moves every record from "from" to "to" (this reverses the order).
    static void transferAll(ActivityStack& from, ActivityStack& to);
 
public:
    ActivityManager(ParcelIntake& parcelIntake, ServiceManager& serviceManager,
                    CollectionDelivery& collectionDelivery);
 
    // Who is operating the system (stored with every recorded activity)
    void setStaffId(const std::string& staffId);
 
    // Req 1: called by Tasks 1, 2 and 3 whenever something changes
    virtual void logActivity(const std::string& parcelID, ActivityType type,
                             const std::string& previousStatus,
                             const std::string& newStatus,
                             const std::string& previousNextAction,
                             const std::string& newNextAction,
                             const std::string& requestID,
                             const std::string& resourceID);
                  
    // Req 2 & 3: display and review
    void displayRecentActivities(int limit);        //newest first
    void displayParcelHistory(const std::string& parcelId); //oldest -> newest + review

    // Req 4 & 5: undo (with all eligibility / exception checks)
    bool undoActivity(const std::string& parcelId, int activityId);
 
    // CSV: activityId,timestamp,parcelId,activityType,previousStatus,newStatus,
    //      previousNextAction,newNextAction,requestId,resourceId,staffId,reversed
    int  loadFromCSV(const std::string& filePath);   // records loaded, -1 if file missing
    bool saveToCSV(const std::string& filePath);
 
    // Module menu (called from main.cpp)
    void runMenu();
 
    int totalRecords() const { return history.size(); }
};
 
#endif // ACTIVITY_MANAGER_HPP



