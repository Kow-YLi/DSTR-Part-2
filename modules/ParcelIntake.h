
#ifndef PARCEL_INTAKE_H
#define PARCEL_INTAKE_H

class ParcelIntake {
public:
    ParcelIntake();
    ~ParcelIntake();

    void registerParcel();
    void displayAllParcels() const;
    void searchParcel() const;
    void displayPendingQueue() const;
    void processNextParcel();
    void updateParcel();
    void removeParcel();
    void reportMissingParcel();
};

#endif

