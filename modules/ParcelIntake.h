
#ifndef PARCEL_INTAKE_H
#define PARCEL_INTAKE_H

#include "../models/Parcel.h"
#include <string>

class ParcelIntake {
private:
    // Node for the Singly Linked List
    struct ParcelNode {
        Parcel data;
        ParcelNode* next;
    };

    // Node for the incoming FIFO Queue
    struct QueueNode {
        std::string parcelID;
        QueueNode* next;
    };

    // Linked list pointer
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

    // Helper function
    void displayParcel(const Parcel& parcel) const;

public:
    ParcelIntake();
    ~ParcelIntake();

    // Prevent accidental copying of owned nodes
    ParcelIntake(const ParcelIntake&) = delete;
    ParcelIntake& operator=(const ParcelIntake&) = delete;

    // Functional Requirement 1
    void registerParcel();

    // Functional Requirement 2
    void displayAllParcels() const;
    void searchParcel() const;

    // Functional Requirement 3
    void displayPendingQueue() const;
    void processNextParcel();

    // Functional Requirement 4
    void updateParcel();
    void removeParcel();

    // Functional Requirement 5
    void reportMissingParcel();

    // Interfaces for integration with other modules
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
