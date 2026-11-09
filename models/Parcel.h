
#ifndef PARCEL_HPP
#define PARCEL_HPP

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

#endif // PARCEL_HPP

