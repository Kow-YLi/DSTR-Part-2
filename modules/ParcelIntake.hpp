
#ifndef PARCEL_INTAKE_HPP
#define PARCEL_INTAKE_HPP

#include "../models/Parcel.hpp"
#include "../models/ActivityLogger.hpp"
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

    //Every important change is reported to logger
    ActivityLogger* logger;

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

    // Sends one change to the activity logger
    void logChange(const std::string& parcelID, ActivityType type,
                   const std::string& previousStatus, const std::string& newStatus,
                   const std::string& previousNextAction, const std::string& newNextAction,
                   const std::string& requestID, const std::string& resourceID) const;


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
        const std::string& newStatus,
        const std::string& requestID = "-",     //Two optional parameter for 
        const std::string& resourceID = "-"     //task 2 and 3 to add info to activity history
    );

    // Task 4 undo support: puts an earlier status back. Only succeeds if the parcel is currently in expectedCurrentStatus.
    bool restoreParcelStatus(
        const std::string& parcelID, 
        const std::string& expectedCurrentStatus,
        const std::string& previousStatus, 
        const std::string& previousNextAction
    );

    // Attach the Task 4 Activity Logger
    void setLogger(ActivityLogger* activityLogger);

    //Read-only access to All parcel records, so another module can build own structure from the shared records without toughing Task 1 linked list
    int getParcelCount() const;
    bool getParcelAt(int index, Parcel& result) const; 

    //Sample data in CSV
    int loadFromCSV(const std::string& filePath);       //Parcels loaded, -1 if file is missing
    bool saveToCSV(const std::string& filePath) const;
    

};

#endif // PARCEL_INTAKE_HPP
