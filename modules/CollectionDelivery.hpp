#ifndef COLLECTION_DELIVERY_HPP
#define COLLECTION_DELIVERY_HPP

#include "ParcelIntake.hpp"
#include "CircularQueue.hpp"
#include "ServiceManager.hpp"
#include "../models/ActivityLogger.hpp"
#include <string>


class CollectionDelivery {
private:
    // Size limits
    static const int COUNTER_CAPACITY = 10; // waiting area
    static const int MAX_RIDERS = 5;        // delivery staff

    // Delivery rider record
    struct Rider {
        std::string riderID;
        std::string name;
        std::string status;          // "Available" / "On Delivery"
        std::string currentParcelID; // "-" when free
        int completedDeliveries;
    };

    // Shared parcel data from Module 1
    ParcelIntake& intake;

    // Circular Queue 1: customers waiting at the counter
    // (stores the Parcel ID each customer is collecting)
    CircularQueue<std::string, COUNTER_CAPACITY> counterQueue;

    // Circular Queue 2: round-robin rotation of free riders
    // (stores an index into the riders array)
    CircularQueue<int, MAX_RIDERS> riderRotation;

    // All riders (fixed delivery resources)
    Rider riders[MAX_RIDERS];
    int riderCount;

    //Changes: Task 4 hook: assignments are reported to this logger
    //May be nullptr when Task 3 runs on its own
    ActivityLogger* logger;
    ServiceManager* service;        //Task 2 link, may be nullptr when task 3 runs on its own

    // Helpers
    bool isInCounterQueue(const std::string& parcelID) const;
    int findRiderByParcel(const std::string& parcelID) const;
    int findRiderByID(const std::string& riderID) const;
    void addRider(const std::string& riderID,
                  const std::string& name);

    //Changes: New helper for undo: remove one customer from the counter queue while keeping everybody else in order
    bool removeFromCounterQueue(const std::string& parcelID);

    //Changes: New helpers for task 2 link
    std::string findLinkedRequest(const std::string& parcelID) const;   //"-" if none
    void completeLinkedRequest(const std::string& parcelID);

public:
    explicit CollectionDelivery(ParcelIntake& sharedIntake);

    // Prevent accidental copying
    CollectionDelivery(const CollectionDelivery&) = delete;
    CollectionDelivery& operator=(const CollectionDelivery&) = delete;

    // Collection counter
    void joinCollectionQueue();
    void serveNextCustomer();
    void displayCounterQueue() const;

    // Delivery resources
    void assignDelivery();
    void completeDelivery();
    void displayRiders() const;

    // Module menu (called from main.cpp)
    void runMenu();

    //Changes:Attach task 4 activity logger
    void setLogger(ActivityLogger* activityLogger);

    void setServiceManager(ServiceManager* serviceManager);

  
    bool releaseAssignment(const std::string& parcelID, const std::string& resourceID);
};

#endif // COLLECTION_DELIVERY_HPP
