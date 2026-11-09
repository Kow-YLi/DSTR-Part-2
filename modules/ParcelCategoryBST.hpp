// ============================================================================
// ParcelCategoryBST.h
// Task 5 - Parcel Category Organisation Module
// Self-implemented Binary Search Tree (BST) of Parcel records.
//
// Functional coverage (from the assignment brief):
//   FR1  Create & maintain the tree-based organisation structure
//   FR2  Insert a new parcel record (validate + reject duplicate key)
//   FR3  Search for a parcel (by ID) or a category (list all under it)
//   FR4  Display stored parcel records (in-order, sorted by Parcel ID)
//   FR5  Update or remove a selected parcel record (preserve tree structure)
//   FR6  Handle exceptional conditions (empty, duplicate, not-found, invalid)
//
// The BST is keyed on parcelID (lexicographic order). Insert / search / remove
// therefore run in O(log n) average time. Category listing is a full traversal.
// ============================================================================

#ifndef PARCEL_CATEGORY_BST_HPP
#define PARCEL_CATEGORY_BST_HPP

#include "../models/Parcel.hpp"
#include <iostream>
#include <string>

class ParcelCategoryBST
{
private:
    // ---- Self-implemented tree node (no STL) -----------------------------
    struct Node
    {
        Parcel data;
        Node*  left;
        Node*  right;
        explicit Node(const Parcel& p) : data(p), left(nullptr), right(nullptr) {}
    };

    Node* root;   // root of the BST

    // ---- Recursive helpers ----------------------------------------------
    Node* insertNode(Node* node, const Parcel& p);
    bool  contains(Node* node, const std::string& id) const; // silent duplicate check
    Node* searchNode(Node* node, const std::string& id) const;
    Node* findMinNode(Node* node) const;          // in-order successor source
    Node* removeNode(Node* node, const std::string& id);
    void  destroyTree(Node* node);                // post-order free
    int   countNodes(Node* node) const;

    // In-order traversal: print one row per parcel (sorted by ID).
    void  inorderPrint(Node* node) const;
    // In-order traversal: print every parcel whose category matches (case-insensitive).
    void  inorderByCategory(Node* node, const std::string& cat) const;

    // Fixed-width console table header / separator for tidy output.
    static void printTableHeader();
    static void printSeparator();

public:
    ParcelCategoryBST();
    ~ParcelCategoryBST();

    // FR1
    bool isEmpty() const { return root == nullptr; }

    // FR2  Returns false on duplicate key or invalid data.
    bool insert(const Parcel& p);

    // FR3
    bool searchByID(const std::string& id, Parcel& out) const;
    int  searchByCategory(const std::string& category) const; // returns match count

    // FR4
    void displayAll() const;

    // FR5
    bool update(const std::string& id, const Parcel& newInfo);
    bool remove(const std::string& id);

    // Utility
    int  size() const { return countNodes(root); }

    // Changes: Empty the whole tree (post-order free). Used before the tree is
    // rebuilt from the shared Task 1 records so the category view is never out of date.
    void clear();
 
    // Changes: Build a BALANCED tree from parcels already sorted by Parcel ID (middle element first),
    // so inserting IDs in registration order cannot create a long chain. Tree must be empty.
    void buildBalanced(const Parcel* sortedParcels, int count);
};

#endif // PARCEL_CATEGORY_BST_H
