#ifndef COLLECTION_DELIVERY_HPP
#define COLLECTION_DELIVERY_HPP

#include "ParcelIntake.hpp"
#include "CircularQueue.hpp"
#include <string>

// ==================================================
// Module 3: Collection Counter &
//           Delivery Resource Management
// --------------------------------------------------
// Works on the SAME parcel records as Module 1
// (through a reference to the shared ParcelIntake
// object), so every status change made here is
// visible in every other module.
// ==================================================

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

    // Helpers
    bool isInCounterQueue(const std::string& parcelID) const;
    int findRiderByParcel(const std::string& parcelID) const;
    int findRiderByID(const std::string& riderID) const;
    void addRider(const std::string& riderID,
                  const std::string& name);

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
};

#endif // COLLECTION_DELIVERY_HPP
