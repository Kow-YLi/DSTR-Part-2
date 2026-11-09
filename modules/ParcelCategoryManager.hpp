#ifndef PARCEL_CATEGORY_MANAGER_HPP
#define PARCEL_CATEGORY_MANAGER_HPP
 
#include "ParcelIntake.hpp"
#include "ParcelCategoryBST.hpp"
 
// ==================================================
// Module 5: Parcel Category Organisation  (NEW FILE - integration layer)
// --------------------------------------------------
// Same pattern as Modules 2-4: it receives a reference to the shared
// ParcelIntake object (Task 1) and exposes runMenu() for main.cpp.
//
// The BST (ParcelCategoryBST, the Task 5 data structure) is an INDEX of
// the shared parcel records, keyed on Parcel ID:
//   - Task 1 stays the single source of truth for parcel data.
//   - Before every menu action the tree is rebuilt from Task 1
//     (getParcelCount / getParcelAt), so parcels registered in Task 1 and
//     status changes made by Tasks 2, 3 and 4 (including undo) always show up here.
//   - Update / Remove are applied to Task 1 FIRST (so Task 1's rules are
//     respected), then mirrored in the BST (FR5 insert/update/delete logic).
// ==================================================
class ParcelCategoryManager {
private:
    ParcelIntake& intake;          // shared Task 1 records
    ParcelCategoryBST tree;        // Task 5 BST
 
    void syncFromIntake();         // rebuild BST from Task 1 records
    void insertParcel();           // FR2: BST insert + register in Task 1
    void searchByID();             // FR3
    void searchByCategory();       // FR3
    void displayAll();             // FR4
    void updateParcel();           // FR5
    void removeParcel();           // FR5
    static void printParcel(const Parcel& p);
 
public:
    explicit ParcelCategoryManager(ParcelIntake& sharedIntake);
 
    // Prevent accidental copying
    ParcelCategoryManager(const ParcelCategoryManager&) = delete;
    ParcelCategoryManager& operator=(const ParcelCategoryManager&) = delete;
 
    // Module menu (called from main.cpp)
    void runMenu();
};
 
#endif // PARCEL_CATEGORY_MANAGER_HPP