#include <iostream>
#include <string>
#include <cctype>
 
#include "modules/ParcelIntake.hpp"
#include "modules/ServiceManager.hpp"
#include "modules/CollectionDelivery.hpp"
#include "modules/ActivityManager.hpp"
#include "modules/ParcelCategoryManager.hpp"
 
using namespace std;
 
const string PARCEL_FILE   = "data/parcels.csv";
const string ACTIVITY_FILE = "data/activities.csv";

// Remove whitespace from the start and end
string trimText(const string& value){
    size_t start = 0;
    size_t end = value.length();
    while(start < end && isspace(static_cast<unsigned char>(value[start])))
    start++;
    while (end > start && isspace(static_cast<unsigned char>(value[end - 1])))
    end--;
    return value.substr(start, end - start);
}

//Task 1 menu
void runTask1Menu(ParcelIntake& parcelSystem){
    string input;

    while (true){
        cout << "\n========================================\n";
        cout << " TASK 1: PARCEL INTAKE AND PROCESSING\n";
        cout << "----------------------------------------\n";
        cout << "1. Register New Parcel\n";
        cout << "2. Display All Parcels\n";
        cout << "3. Search Parcel\n";
        cout << "4. Display Pending Queue\n";
        cout << "5. Process Next Parcel\n";
        cout << "6. Update Parcel\n";
        cout << "7. Remove Parcel\n";
        cout << "8. Report Missing Parcel\n";
        cout << "0. Back\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        if(!getline(cin, input)){        //stop if input is closed instead of looping forever
            return;
        };
        input = trimText(input);

        if(input.length() != 1 || input[0] < '0' || input[0] > '8'){
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        int choice = input[0] - '0';

        switch (choice) {
            case 1:
                parcelSystem.registerParcel();
                break;

            case 2:
                parcelSystem.displayAllParcels();
                break;

            case 3:
                parcelSystem.searchParcel();
                break;

            case 4:
                parcelSystem.displayPendingQueue();
                break;

            case 5:
                parcelSystem.processNextParcel();
                break;

            case 6:
                parcelSystem.updateParcel();
                break;

            case 7:
                parcelSystem.removeParcel();
                break;

            case 8:
                parcelSystem.reportMissingParcel();
                break;

            case 0:
                cout << "\nReturning to Main Menu...\n";
                return;

            default:
                cout << "Invalid choice.\n";
                return;
        }

    }
    
};

//Save parcels and activity history
bool saveAllData(ParcelIntake& intake, ActivityManager& activity){
    bool parcelsSaved = intake.saveToCSV(PARCEL_FILE);
    bool activitiesSaved = activity.saveToCSV(ACTIVITY_FILE);
    if(parcelsSaved && activitiesSaved){
        cout << "[Success] Parcels and activity history saved.\n";
        return true;
    }
    return false;
}

int main(){
    cout << "========================================\n";
    cout << "  SMART CAMPUS PARCEL MANAGEMENT SYSTEM\n";
    cout << "========================================\n";

    //Changes: ONE ParcelIntake is shared by every module.
    ParcelIntake intake;
    ServiceManager service(intake);
    CollectionDelivery delivery(intake);
    ActivityManager activity(intake, service, delivery);
    ParcelCategoryManager categories(intake);

    //Changes: task 1 -3 report every changes to task 4; task 3 completes task 2 requests
    intake.setLogger(&activity);
    service.setLogger(&activity);
    delivery.setLogger(&activity);
    delivery.setServiceManager(&service);

    // ---- Load sample data (parcels FIRST, then the activity history) ----
    int parcelsLoaded = intake.loadFromCSV(PARCEL_FILE);
    if (parcelsLoaded >= 0) cout << "Loaded " << parcelsLoaded << " parcels.\n";
    else cout << "[Warning] Cannot open " << PARCEL_FILE << " - starting empty.\n";
 
    int activitiesLoaded = activity.loadFromCSV(ACTIVITY_FILE);
    if (activitiesLoaded >= 0) cout << "Loaded " << activitiesLoaded << " activity records.\n";
    else cout << "[Warning] Cannot open " << ACTIVITY_FILE << " - starting empty.\n";

    //Staff Identification
    string staffId;
    do{
        cout << "Enter your Staff ID: ";
        getline(cin, staffId);
        staffId = trimText(staffId);
    }while(staffId.empty());
    activity.setStaffId(staffId);

    // Main Menu
    string input;
    while(true){
        cout << "\n========================================\n";
        cout << "  MAIN MENU   (Staff: " << staffId << ")\n";
        cout << "========================================\n";
        cout << "1. Parcel Intake, Sorting & Processing   (Task 1)\n";
        cout << "2. Parcel Service & Collection Requests  (Task 2)\n";
        cout << "3. Collection Counter & Delivery         (Task 3)\n";
        cout << "4. Activity History & Undo               (Task 4)\n";
        cout << "5. Parcel Category Organisation          (Task 5)\n";
        cout << "6. Save Data to CSV\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        getline(cin, input);
        input = trimText(input);

        if (input == "1") {
            runTask1Menu(intake);
        } else if (input == "2") {
            service.runMenu();
        } else if (input == "3") {
            delivery.runMenu();
        } else if (input == "4") {
            activity.runMenu();
        } else if (input == "5") {
            categories.runMenu();
        } else if (input == "6") {
            saveAllData(intake, activity);
        } else if (input == "0") {
            cout << "Save data before exiting? (Y/N): ";
            string answer;
            getline(cin, answer);
            answer = trimText(answer);
            if (answer == "Y" || answer == "y") {
                saveAllData(intake, activity);
            }
            cout << "\nExiting system. Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
 
    return 0;
}

    
