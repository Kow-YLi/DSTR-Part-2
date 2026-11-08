#include <iostream>
#include <string>
#include <limits>
#include "task4.cpp"
using namespace std;

const string PARCEL_FILE = "data/parcels.csv";
const string ACTIVITY_FILE = "data/activities.csv";

int readInt(const string& prompt){
    int value;
    while (true) {
        cout << prompt;
        if(cin >> value){
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer
            return value;
        } else {
            cout << "[Error] Invalid input. Please enter a valid integer." << endl;
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
        }
    }

}

string readWord(const string& prompt){
    string text;
    cout << prompt;
    cin >> text;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer
    return text;
}

void recordActivityMenu(ActivityManager& manager){
    string parcelId = readWord("Parcel ID: ");
    cout << "Type : " << "\n" <<
            "0 REGISTER " << "\n" <<
            "1 SORT" << "\n" <<
            "2 CREATE_REQUEST" << "\n" <<
            "3 ASSIGN_RESOURCE" << "\n" <<
            "4 COLLECT" << "\n" <<
            "5 DELIVER" << "\n" <<
            "6 RETURN" << "\n" <<
            "7 CANCEL" << "\n" <<
            "8 HOLD" << "\n" <<
            "9 MARK_MISSING" << "\n";
    int type = readInt("Activity type: ");
    if (type < 0 || type > 9){
        cout << "[Error] Invalid activity type.\n";
        return;
    }
    cout << "Status: " << "\n" <<
            "0 ARRIVED " << "\n" <<
            "1 REGISTERED" << "\n" <<
            "2 SORTED" << "\n" <<
            "3 REQUEST_CREATED" << "\n" <<
            "4 ASSIGNED" << "\n" <<
            "5 COLLECTED" << "\n" <<
            "6 DELIVERED" << "\n" <<
            "7 RETURNED" << "\n" <<
            "8 CANCELLED" << "\n" <<
            "9 ON_HOLD" << "\n" <<
            "10 MISSING" << "\n";
    int status = readInt("New status: ");
    if(status < 0 || status > 10){
        cout << "[Error] Invalid status.\n";
        return;
    }
    string requestId = readWord("Request ID (- if none): ");
    string resourceId = readWord("Resource ID (- if none): ");
    string staffId = readWord("Staff ID: ");
    if(manager.recordActivity(parcelId, (ActivityType)type, (ParcelStatus)status, requestId, resourceId, staffId))
    cout << "[Success] Activity recorded.\n";
}

int main(){
    ParcelRegistry registry;
    ActivityManager manager(registry);

    int parcelsLoaded = registry.loadFromCSV(PARCEL_FILE);
    if(parcelsLoaded >= 0){
        cout << "Loaded " << parcelsLoaded << " parcels.\n";
    }else{
        cout << "[Warning] Cannot open " << PARCEL_FILE << " - starting empty.\n";
    }

    int activitiesLoaded = manager.loadFromCSV(ACTIVITY_FILE);
    if(activitiesLoaded >= 0){
        cout << "Loaded " << activitiesLoaded << " activity records.\n";
    }else{
        cout << "[Warning] Cannot open " << ACTIVITY_FILE << " - starting empty.\n";
    }

    int choice;
    do{
        cout << "\n===== Task 4 : Parcel Activity & Record Management =====\n"
             << "1. Display recent activities\n"
             << "2. Display / review activity history of a parcel\n"
             << "3. Undo an activity\n"
             << "4. Record a new activity (integration demo)\n"
             << "5. Save data to CSV\n"
             << "0. Exit\n";
        choice = readInt("Task choice: ");

        switch (choice){
            case 1: {
                int limit = readInt("How many recent activities? ");
                if (limit <= 0){
                    cout << "[Error] Number must be positive.\n";
                }else{
                    manager.displayRecentActivities(limit);
                    
                }
                break;
            }
            case 2:{
                manager.displayParcelHistory(readWord("Parcel ID: "));
                break;
            }
            case 3: {
                string parcelId = readWord("Parcel ID: ");
                manager.displayParcelHistory(parcelId);
                int activityId = readInt("Activity ID to reverse: ");
                manager.undoActivity(parcelId, activityId, readWord("Your Staff ID: "));
                break;
            }
            case 4: {
                recordActivityMenu(manager);
                break;

            }
            case 5: {
                if(registry.saveToCSV(PARCEL_FILE) && manager.saveToCSV(ACTIVITY_FILE)){
                    cout << "[Success] Data saved.\n";
                }
                break;
            }
            case 0: {
                cout << "Exiting. \n";
                break;
            }
            default:{
                cout << "[Error] Invalid menu choice.\n";
            }
        }
    } while (choice != 0);
    return 0;
        
}

        
    