// ============================================================================
// ParcelCategoryBST.cpp
// Implementation of the self-made BST for the Parcel Category Organisation
// Module (Task 5). See ParcelCategoryBST.h for the functional mapping.
// ============================================================================

#include "ParcelCategoryBST.h"
#include <algorithm>   // std::tolower (char helper only)
#include <cctype>
#include <iomanip>     // std::setw / std::left for tidy console tables

// ---------------------------------------------------------------------------
// Small helper: case-insensitive string comparison (used for category filter).
// ---------------------------------------------------------------------------
static bool iequals(const std::string& a, const std::string& b)
{
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i]))
            return false;
    return true;
}

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------
ParcelCategoryBST::ParcelCategoryBST() : root(nullptr) {}

ParcelCategoryBST::~ParcelCategoryBST()
{
    destroyTree(root);
}

void ParcelCategoryBST::destroyTree(Node* node)
{
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

// ---------------------------------------------------------------------------
// FR2 - Insert (recursive). Rejects duplicate key (parcelID already present)
// and invalid data. Returns true on success.
// ---------------------------------------------------------------------------
ParcelCategoryBST::Node* ParcelCategoryBST::insertNode(Node* node, const Parcel& p)
{
    if (node == nullptr)
        return new Node(p);                 // empty spot found -> attach

    if (p.parcelID == node->data.parcelID)
        return node;                        // FR6: duplicate key -> keep existing

    if (p.parcelID < node->data.parcelID)
        node->left  = insertNode(node->left,  p);
    else
        node->right = insertNode(node->right, p);

    return node;                            // unchanged parent pointer
}

bool ParcelCategoryBST::insert(const Parcel& p)
{
    if (!p.isValid())                       // FR6: invalid parcel information
    {
        std::cout << "  [Error] Cannot insert: parcel information is invalid.\n";
        return false;
    }
    // Reject duplicate key up-front so we can report it clearly.
    // Uses the SILENT contains() helper so a normal (new) insert does not
    // produce a misleading "not found" message.
    if (contains(root, p.parcelID))         // FR6: duplicate parcel / organisation key
    {
        std::cout << "  [Error] Cannot insert: Parcel ID \"" << p.parcelID
                  << "\" already exists (duplicate key).\n";
        return false;
    }
    root = insertNode(root, p);
    return true;
}

// Silent membership test used only for the duplicate-key check above.
bool ParcelCategoryBST::contains(Node* node, const std::string& id) const
{
    if (node == nullptr) return false;
    if (id == node->data.parcelID) return true;
    if (id < node->data.parcelID) return contains(node->left, id);
    return contains(node->right, id);
}

// ---------------------------------------------------------------------------
// FR3 - Search by unique Parcel ID (recursive). Copies the found record into
// `out` and returns true, otherwise false.
// ---------------------------------------------------------------------------
ParcelCategoryBST::Node* ParcelCategoryBST::searchNode(Node* node, const std::string& id) const
{
    if (node == nullptr) return nullptr;    // FR6: searched item does not exist
    if (id == node->data.parcelID) return node;
    if (id < node->data.parcelID) return searchNode(node->left, id);
    return searchNode(node->right, id);
}

bool ParcelCategoryBST::searchByID(const std::string& id, Parcel& out) const
{
    if (isEmpty())                          // FR6: empty data structure
    {
        std::cout << "  [Error] Search failed: the organisation structure is empty.\n";
        return false;
    }
    Node* found = searchNode(root, id);
    if (found == nullptr)                   // FR6: non-existent parcel
    {
        std::cout << "  [Error] Parcel ID \"" << id << "\" was not found.\n";
        return false;
    }
    out = found->data;
    return true;
}

// ---------------------------------------------------------------------------
// FR3 - List every parcel belonging to a category (in-order traversal, so the
// list is still sorted by Parcel ID). Returns the number of matches.
// ---------------------------------------------------------------------------
void ParcelCategoryBST::inorderByCategory(Node* node, const std::string& cat) const
{
    if (node == nullptr) return;
    inorderByCategory(node->left, cat);
    if (iequals(node->data.category, cat))
    {
        std::cout << "  " << node->data.parcelID << " | "
                  << node->data.recipient   << " | "
                  << node->data.category     << " | "
                  << node->data.arrivalDate  << " | "
                  << node->data.weight       << " kg | "
                  << node->data.status       << "\n";
    }
    inorderByCategory(node->right, cat);
}

int ParcelCategoryBST::searchByCategory(const std::string& category) const
{
    if (isEmpty())
    {
        std::cout << "  [Error] Search failed: the organisation structure is empty.\n";
        return 0;
    }
    std::cout << "  Parcels under category \"" << category << "\":\n";
    inorderByCategory(root, category);   // prints matches, sorted by Parcel ID

    // Re-count (small data set) to return an accurate integer to the caller.
    struct Counter { static void go(Node* n, const std::string& c, int& k)
    {
        if (!n) return;
        go(n->left, c, k);
        if (iequals(n->data.category, c)) ++k;
        go(n->right, c, k);
    } };
    int count = 0;
    Counter::go(root, category, count);
    if (count == 0)
        std::cout << "  [Info] No parcels found for category \"" << category << "\".\n";
    return count;
}

// ---------------------------------------------------------------------------
// FR4 - Display all records (in-order => sorted by Parcel ID).
// ---------------------------------------------------------------------------
void ParcelCategoryBST::printTableHeader()
{
    std::cout << "  " << "ParcelID | Recipient        | Category    | Arrival           | Weight   | Status\n";
}

void ParcelCategoryBST::printSeparator()
{
    std::cout << "  " << "-------------------------------------------------------------------------------\n";
}

void ParcelCategoryBST::inorderPrint(Node* node) const
{
    if (node == nullptr) return;
    inorderPrint(node->left);
    std::cout << "  "
              << std::left
              << std::setw(9)  << node->data.parcelID
              << std::setw(18) << node->data.recipient.substr(0, 16)
              << std::setw(13) << node->data.category
              << std::setw(19) << node->data.arrivalDate
              << std::setw(10) << (std::to_string((int)node->data.weight) + "kg")
              << node->data.status << "\n";
    inorderPrint(node->right);
}

void ParcelCategoryBST::displayAll() const
{
    if (isEmpty())                          // FR6: empty data structure
    {
        std::cout << "  [Info] The organisation structure is currently empty.\n";
        return;
    }
    printSeparator();
    printTableHeader();
    printSeparator();
    inorderPrint(root);
    printSeparator();
    std::cout << "  Total parcels organised: " << size() << "\n";
}

// ---------------------------------------------------------------------------
// FR5 - Update. Replaces every field EXCEPT parcelID (the key) of an existing
// record. Returns false if the record does not exist or new data is invalid.
// ---------------------------------------------------------------------------
bool ParcelCategoryBST::update(const std::string& id, const Parcel& newInfo)
{
    if (isEmpty())
    {
        std::cout << "  [Error] Update failed: the organisation structure is empty.\n";
        return false;
    }
    if (!newInfo.isValid())                 // FR6: invalid parcel information
    {
        std::cout << "  [Error] Update failed: new parcel information is invalid.\n";
        return false;
    }
    Node* target = searchNode(root, id);
    if (target == nullptr)                  // FR6: update a non-existent record
    {
        std::cout << "  [Error] Update failed: Parcel ID \"" << id << "\" does not exist.\n";
        return false;
    }
    // Key stays fixed; other attributes are refreshed. This keeps the BST
    // ordering valid (the key never changes).
    target->data.recipient   = newInfo.recipient;
    target->data.category    = newInfo.category;
    target->data.arrivalDate = newInfo.arrivalDate;
    target->data.weight      = newInfo.weight;
    target->data.status      = newInfo.status;
    return true;
}

// ---------------------------------------------------------------------------
// FR5 - Remove. Standard BST deletion that PRESERVES tree structure:
//   - 0 children : delete the node
//   - 1 child    : replace node with its single child
//   - 2 children : copy in-order successor (min of right subtree) into the node,
//                  then delete that successor. (FR6: removing a node that
//                  affects the structure is handled correctly.)
// ---------------------------------------------------------------------------
ParcelCategoryBST::Node* ParcelCategoryBST::findMinNode(Node* node) const
{
    while (node && node->left) node = node->left;
    return node;
}

ParcelCategoryBST::Node* ParcelCategoryBST::removeNode(Node* node, const std::string& id)
{
    if (node == nullptr) return nullptr;    // FR6: delete a non-existent record

    if (id < node->data.parcelID)
        node->left = removeNode(node->left, id);
    else if (id > node->data.parcelID)
        node->right = removeNode(node->right, id);
    else
    {
        // Found the node to delete.
        if (node->left == nullptr && node->right == nullptr)   // 0 children
        {
            delete node;
            return nullptr;
        }
        if (node->left == nullptr)                             // 1 child (right)
        {
            Node* temp = node->right;
            delete node;
            return temp;
        }
        if (node->right == nullptr)                            // 1 child (left)
        {
            Node* temp = node->left;
            delete node;
            return temp;
        }
        // 2 children: replace with in-order successor (smallest in right subtree)
        Node* succ = findMinNode(node->right);
        node->data = succ->data;                  // copy successor's parcel data
        node->right = removeNode(node->right, succ->data.parcelID); // delete successor
    }
    return node;
}

bool ParcelCategoryBST::remove(const std::string& id)
{
    if (isEmpty())                          // FR6: empty data structure
    {
        std::cout << "  [Error] Remove failed: the organisation structure is empty.\n";
        return false;
    }
    // Verify existence first to give a clear "not found" message.
    Parcel dummy;
    if (!searchByID(id, dummy)) return false; // message already printed
    root = removeNode(root, id);
    return true;
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------
int ParcelCategoryBST::countNodes(Node* node) const
{
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}
