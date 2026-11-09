// ============================================================================
// main.cpp  (Task 5 - Parcel Category Organisation Module)
//
// Console demonstration / entry point for the BST-based parcel organisation
// module. In the FINAL integrated system this file's menu is replaced by the
// team's single `main()` that also calls Task 1..4; the ParcelCategoryBST class
// is dropped in unchanged because it only depends on the shared Parcel.h.
//
// Build (MinGW / Code::Blocks / VS): compile all .cpp together, e.g.
//     g++ main.cpp ParcelCategoryBST.cpp -o Task5_ParcelCategory
// ============================================================================

#include "ParcelCategoryBST.h"
#include <fstream>
#include <sstream>
#include <limits>

// ---------------------------------------------------------------------------
// Integration point: in the integrated prototype a parcel registered / routed
// by Task 1 (Intake) or serviced by Task 2 can be forwarded here so the whole
// campus shares ONE category-indexed view. Shown for the F2F demonstration.
// ---------------------------------------------------------------------------
static ParcelCategoryBST gCategoryTree;   // shared instance across modules

void onParcelRegistered(const Parcel& p)
{
    gCategoryTree.insert(p);   // Task 1 -> Task 5 bridge
}

// ---------------------------------------------------------------------------
// Tiny helper: read a line and trim surrounding whitespace.
// ---------------------------------------------------------------------------
static std::string trim(const std::string& s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

// ---------------------------------------------------------------------------
// Load sample parcels from a CSV (header + rows). Used so the demo starts with
// realistic data; not required by the brief.
// ---------------------------------------------------------------------------
static int loadCSV(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return 0;
    std::string line;
    int loaded = 0;
    bool header = true;
    while (std::getline(file, line))
    {
        if (header) { header = false; continue; }   // skip header row
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string field;
        Parcel p;
        // ParcelID,Recipient,Category,ArrivalDate,Weight,Status
        if (!std::getline(ss, p.parcelID,    ',')) continue;
        if (!std::getline(ss, p.recipient,   ',')) continue;
        if (!std::getline(ss, p.category,    ',')) continue;
        if (!std::getline(ss, p.arrivalDate, ',')) continue;
        if (!std::getline(ss, field, ',')) continue;
        p.weight = std::stod(field);
        if (!std::getline(ss, p.status))           continue;
        p.parcelID = trim(p.parcelID); p.recipient = trim(p.recipient);
        p.category = trim(p.category);   p.arrivalDate = trim(p.arrivalDate);
        p.status   = trim(p.status);
        if (gCategoryTree.insert(p)) ++loaded;
    }
    return loaded;
}

// ---------------------------------------------------------------------------
// Interactive menu
// ---------------------------------------------------------------------------
static void showMenu()
{
    std::cout << "\n========== Task 5: Parcel Category Organisation (BST) ==========\n";
    std::cout << " 1. Insert a new parcel record\n";
    std::cout << " 2. Search for a parcel by ID\n";
    std::cout << " 3. Search / list parcels by category\n";
    std::cout << " 4. Display all parcels (sorted by ID)\n";
    std::cout << " 5. Update a parcel record\n";
    std::cout << " 6. Remove a parcel record\n";
    std::cout << " 7. Show total count\n";
    std::cout << " 0. Exit\n";
    std::cout << "=================================================================\n";
    std::cout << "Choice: ";
}

static Parcel readParcelFromUser(bool includeID)
{
    Parcel p;
    if (includeID)
    {
        std::cout << "  Parcel ID (unique, e.g. PX0001): ";
        std::getline(std::cin, p.parcelID);
    }
    std::cout << "  Recipient name            : "; std::getline(std::cin, p.recipient);
    std::cout << "  Category (Documents/Electronics/Books/Clothing/Food/Others): ";
    std::getline(std::cin, p.category);
    std::cout << "  Arrival date/time (YYYY-MM-DD HH:MM): "; std::getline(std::cin, p.arrivalDate);
    std::cout << "  Weight (kg)               : ";
    if (!(std::cin >> p.weight)) { std::cin.clear(); p.weight = 0.0; } // guard bad input
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "  Status (Pending/Collected/Delivered/Returned/OnHold): "; std::getline(std::cin, p.status);
    return p;
}

int main()
{
    int n = loadCSV("parcels.csv");
    std::cout << "[System] Loaded " << n << " sample parcels from parcels.csv\n";

    int choice;
    do {
        showMenu();
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(); continue; }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1)
        {
            std::cout << "-- Insert new parcel --\n";
            Parcel p = readParcelFromUser(true);
            if (gCategoryTree.insert(p))
                std::cout << "  [OK] Parcel " << p.parcelID << " inserted.\n";
        }
        else if (choice == 2)
        {
            std::cout << "-- Search by ID --\n  Enter Parcel ID: ";
            std::string id; std::getline(std::cin, id);
            Parcel out;
            if (gCategoryTree.searchByID(id, out))
            {
                std::cout << "  Found: " << out.parcelID << " | " << out.recipient
                          << " | " << out.category << " | " << out.arrivalDate
                          << " | " << out.weight << "kg | " << out.status << "\n";
            }
        }
        else if (choice == 3)
        {
            std::cout << "-- Search by category --\n  Enter category: ";
            std::string cat; std::getline(std::cin, cat);
            int c = gCategoryTree.searchByCategory(cat);
            std::cout << "  Matches: " << c << "\n";
        }
        else if (choice == 4)
        {
            std::cout << "-- Display all --\n";
            gCategoryTree.displayAll();
        }
        else if (choice == 5)
        {
            std::cout << "-- Update parcel --\n  Enter Parcel ID to update: ";
            std::string id; std::getline(std::cin, id);
            std::cout << "  Enter NEW details (Parcel ID stays unchanged):\n";
            Parcel p = readParcelFromUser(false); // do NOT re-read the key
            p.parcelID = id;   // keep the key fixed
            if (gCategoryTree.update(id, p))
                std::cout << "  [OK] Parcel " << id << " updated.\n";
        }
        else if (choice == 6)
        {
            std::cout << "-- Remove parcel --\n  Enter Parcel ID to remove: ";
            std::string id; std::getline(std::cin, id);
            if (gCategoryTree.remove(id))
                std::cout << "  [OK] Parcel " << id << " removed.\n";
        }
        else if (choice == 7)
        {
            std::cout << "  Total parcels organised: " << gCategoryTree.size() << "\n";
        }
        else if (choice == 0)
        {
            std::cout << "[System] Goodbye.\n";
        }
        else
        {
            std::cout << "  [Error] Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
