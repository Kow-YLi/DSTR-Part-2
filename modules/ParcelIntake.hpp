
#ifndef PARCEL_INTAKE_H
#define PARCEL_INTAKE_H

#include "../models/Parcel.h"
#include <string>

class ParcelIntake {
private:
    // Singly Linked List node
    struct ParcelNode {
        Parcel data;
        ParcelNode* next;
    };

    // FIFO Queue node
    struct QueueNode {
        std::string parcelID;
        QueueNode* next;
    };

    // Linked List pointer
    ParcelNode* head;

    // Queue pointers
    QueueNode* front;
    QueueNode* rear;

    // Linked List operations
    ParcelNode* findParcelNode(
        const std::string& parcelID
    ) const;

    void insertParcel(const Parcel& parcel);

    // Queue operations
    void enqueue(const std::string& parcelID);
    void dequeue();
    void removeFromQueue(const std::string& parcelID);

    bool isQueueEmpty() const;
    bool isQueued(const std::string& parcelID) const;

    // Display helper
    void displayParcel(const Parcel& parcel) const;

public:
    ParcelIntake();
    ~ParcelIntake();

    // Prevent accidental copying
    ParcelIntake(const ParcelIntake&) = delete;
    ParcelIntake& operator=(const ParcelIntake&) = delete;

    // Register a new parcel
    void registerParcel();

    // Display and search parcels
    void displayAllParcels() const;
    void searchParcel() const;

    // Process incoming parcels
    void displayPendingQueue() const;
    void processNextParcel();

    // Update and remove records
    void updateParcel();
    void removeParcel();

    // Handle missing parcels
    void reportMissingParcel();

    // Integration with other modules
    bool getParcel(
        const std::string& parcelID,
        Parcel& result
    ) const;

    bool updateParcelStatus(
        const std::string& parcelID,
        const std::string& newStatus
    );
};

#endif // PARCEL_INTAKE_H
