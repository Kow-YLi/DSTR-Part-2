// ============================================================================
// Parcel.h
// Shared parcel record used across ALL modules of the integrated
// "Smart Campus Parcel Collection and Delivery Management System".
//
// Task 5 (Parcel Category Organisation Module) organises these records in a
// self-implemented Binary Search Tree (BST). The SAME struct is shared with the
// other four modules so that, for example, a parcel registered in Task 1 can be
// inserted into the Task 5 BST without any conversion.
//
// NOTE: No STL containers (vector / list / stack / queue / priority_queue /
// deque) are used anywhere in this module - the BST is implemented from scratch.
// ============================================================================

#ifndef PARCEL_H
#define PARCEL_H

#include <string>

// The organisation key chosen for this module is the unique Parcel ID.
// Each parcel also carries a "category" (Documents / Electronics / Books /
// Clothing / Food / Others) which acts as a secondary organisation key that
// staff can list / filter by.
struct Parcel
{
    std::string parcelID;    // UNIQUE key for the BST (e.g. "PX0001")
    std::string recipient;   // recipient name
    std::string category;    // organisation key (parcel type)
    std::string arrivalDate; // arrival date / time "YYYY-MM-DD HH:MM"
    double      weight = 0.0;// weight in kilograms (>= 0)
    std::string status;      // Pending / Collected / Delivered / Returned / OnHold

    // ------------------------------------------------------------------
    // Validate the information carried by a parcel BEFORE it may be inserted
    // or updated. Returns true only when every required field is present and
    // within sensible bounds. Used to satisfy FR2 / FR6 (exception handling).
    // ------------------------------------------------------------------
    bool isValid() const;
};

inline bool Parcel::isValid() const
{
    if (parcelID.empty())        return false; // key must exist
    if (recipient.empty())       return false;
    if (category.empty())        return false;
    if (arrivalDate.empty())     return false;
    if (status.empty())          return false;
    if (weight < 0.0)            return false; // weight cannot be negative
    return true;
}

#endif // PARCEL_H
