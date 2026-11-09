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

    //Changes: task 5: weight in kg
    double weight = 0.0;

    //Changes: task 5: validate the information carried by a parcel before
    // it may be inserted or updated. 
    //Returns true only when every required filed is present and the weight is not negative.
    bool isValid() const;
};

//Changes: task 5's isValid(), written with the shared field names
inline bool Parcel::isValid() const {
    if (parcelID.empty())        return false;   // key must exist
    if (recipientName.empty())   return false;
    if (parcelType.empty())      return false;   // Task 5's "category"
    if (arrivalDateTime.empty()) return false;
    if (status.empty())          return false;
    if (weight < 0.0)            return false;   // weight cannot be negative
    return true;
}

#endif // PARCEL_H

