
#ifndef PARCEL_H
#define PARCEL_H

#include <string>

struct Parcel {
    std::string parcelID;
    std::string recipientName;
    std::string recipientID;
    std::string deliveryCompany;
    std::string parcelType;
    std::string arrivalDateTime;
    std::string status;
    std::string nextAction;
};

#endif // PARCEL_H

