#include "ActivityManager.hpp"
#include "../models/Csvutils.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// ==================================================
// Task 4 - Parcel Activity & Record Management (integrated version)
// --------------------------------------------------
// Stack logic (push / pop / aux stack) is the same as the standalone Task 4.
// What changed for integration:
//   - parcel / request / resource data is no longer a private ParcelRegistry:
//     it is read from the shared Task 1 / 2 / 3 objects
//   - activities arrive through recordActivity() (called by Tasks 1, 2 and 3)
//   - undo restores data through Task 1 (restoreParcelStatus), Task 2
//     (removeActiveRequest) and Task 3 (releaseAssignment)
// ==================================================

namespace {

string currentTimestamp() {
    time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buffer);
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}

string readRequired(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        value = trim(value);
        if (!value.empty()) return value;
        cout << "Error: input cannot be empty.\n";
    }
}

int readNumber(const string& prompt, int minimum, int maximum) {
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) return minimum;   // input closed: leave the menu instead of looping forever
        line = trim(line);
        bool digits = !line.empty();
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] < '0' || line[i] > '9') digits = false;
        }
        if (digits) {
            int value = atoi(line.c_str());
            if (value >= minimum && value <= maximum) return value;
        }
        cout << "Invalid input. Please enter " << minimum << "-" << maximum << ".\n";
    }
}

} // namespace

ActivityManager::ActivityManager(ParcelIntake& parcelIntake, ServiceManager& serviceManager,
                                 CollectionDelivery& collectionDelivery)
    : intake(parcelIntake), service(serviceManager), delivery(collectionDelivery),
      nextActivityId(1), currentStaffId("S01") {
}

void ActivityManager::setStaffId(const string& staffId) {
    currentStaffId = staffId;
}

void ActivityManager::printRecord(const ActivityRecord& r) {
    cout << "Activity ID         : " << r.activityId << "\n";
    cout << "Parcel ID           : " << r.parcelId << "\n";
    cout << "Activity Type       : " << activityToString(r.activityType) << "\n";
    cout << "Status              : " << r.previousStatus << "  ->  " << r.newStatus << "\n";
    cout << "Next Action         : " << r.previousNextAction << "  ->  " << r.newNextAction << "\n";
    cout << "Request ID          : " << r.requestId << "\n";
    cout << "Resource ID         : " << r.resourceId << "\n";
    cout << "Staff ID            : " << r.staffId << "\n";
    cout << "Timestamp           : " << r.timestamp << "\n";
    cout << "Reversed            : " << (r.reversed ? "Yes" : "No") << "\n";
    cout << "----------------------------------------\n";
}

// Moves every record from "from" to "to" (this reverses the order).
void ActivityManager::transferAll(ActivityStack& from, ActivityStack& to) {
    ActivityRecord r;
    while (from.pop(r)) {
        to.push(r);
    }
}

// Req 1: Tasks 1, 2 and 3 call this whenever something changes.
// The other modules have already changed their own data; here the change is only recorded.
void ActivityManager::recordActivity(const string& parcelID, ActivityType type,
                                     const string& previousStatus, const string& newStatus,
                                     const string& previousNextAction, const string& newNextAction,
                                     const string& requestID, const string& resourceID) {
    if (history.isFull()) {
        cout << "[Error] Activity history is full. Activity not recorded.\n";
        return;
    }
    ActivityRecord r;
    r.activityId = nextActivityId++;
    r.parcelId = parcelID;
    r.activityType = type;
    r.previousStatus = previousStatus;
    r.newStatus = newStatus;
    r.previousNextAction = previousNextAction;
    r.newNextAction = newNextAction;
    r.requestId = requestID;
    r.resourceId = resourceID;
    r.staffId = currentStaffId;
    r.timestamp = currentTimestamp();
    r.reversed = false;
    history.push(r);
}

// Req 2: recent activities, newest first
void ActivityManager::displayRecentActivities(int limit) {
    if (history.isEmpty()) {
        cout << "[Info] No activities recorded yet.\n";
        return;
    }
    ActivityStack aux;
    ActivityRecord r;
    int shown = 0;
    cout << "\n--- Recent Activities (newest first) ---\n";
    while (shown < limit && history.pop(r)) {
        printRecord(r);
        aux.push(r);
        ++shown;
    }
    transferAll(aux, history);      // put everything back in the original order
}

// Req 2 & 3: history of one parcel (oldest -> newest) + review summary
void ActivityManager::displayParcelHistory(const string& parcelId) {
    Parcel parcel;
    if (!intake.getParcel(parcelId, parcel)) {
        cout << "[Error] Invalid Parcel ID: " << parcelId << "\n";
        return;
    }
    if (history.isEmpty()) {
        cout << "[Info] No activities recorded yet.\n";
        return;
    }

    ActivityStack aux;
    transferAll(history, aux);      // aux top = oldest record

    ActivityRecord r, lastMatch;
    int matches = 0;
    cout << "\n--- Activity History for Parcel ID: " << parcelId << " (oldest -> newest) ---\n";
    while (aux.pop(r)) {
        if (r.parcelId == parcelId) {
            printRecord(r);
            lastMatch = r;
            ++matches;
        }
        history.push(r);            // history top = newest record again
    }
    if (matches == 0) {
        cout << "[Info] No activities found for Parcel ID: " << parcelId << "\n";
        return;
    }

    cout << "\nReview Summary\n"
         << " Total Activities        : " << matches << "\n"
         << " Most Recent Activity    : #" << lastMatch.activityId << " "
         << activityToString(lastMatch.activityType) << " at " << lastMatch.timestamp << "\n"
         << " Previous Status         : " << lastMatch.previousStatus << "\n"
         << " Current Status          : " << parcel.status << "\n"
         << " Current Next Action     : " << parcel.nextAction << "\n";
}

// Req 4 & 5: undo.
// Rules: parcel exists; activity exists and belongs to the parcel; type is reversible (ActivityLogger.hpp);
// not reversed already; no newer non-reversed activity for the same parcel; parcel still in the
// status the activity produced; the owning module (Task 1/2/3) accepts the restore.
bool ActivityManager::undoActivity(const string& parcelId, int activityId) {
    Parcel parcel;
    if (!intake.getParcel(parcelId, parcel)) {
        cout << "[Error] Invalid Parcel ID: " << parcelId << "\n";
        return false;
    }
    if (history.isEmpty()) {
        cout << "[Error] No activities recorded to undo.\n";
        return false;
    }
    // Check whether there is space to record the undo activity
    if (history.isFull()) {
        cout << "[Error] Activity history is full. Cannot record undo.\n";
        return false;
    }

    ActivityStack aux;
    ActivityRecord r, target;
    bool found = false, newerActiveExists = false;

    while (history.pop(r)) {
        if (r.activityId == activityId) {
            target = r;
            found = true;
            break;
        }
        if (r.parcelId == parcelId && !r.reversed && r.activityType != ACT_UNDO) {
            newerActiveExists = true;
        }
        aux.push(r);
    }

    string error = "";
    if (!found) {
        error = "[Error] Activity ID not found in history.";
    } else if (target.parcelId != parcelId) {
        error = "[Error] Activity ID does not belong to the specified Parcel ID.";
    } else if (target.activityType == ACT_UNDO) {
        error = "[Error] An undo entry cannot be reversed.";
    } else if (!isReversible(target.activityType)) {
        error = "[Error] Activity type " + activityToString(target.activityType) + " cannot be reversed.";
    } else if (target.reversed) {
        error = "[Error] Activity has already been reversed.";
    } else if (newerActiveExists) {
        error = "[Error] There are newer non-reversed activities for this parcel. Undo the most recent one first.";
    } else if (parcel.status != target.newStatus) {
        error = "[Error] Parcel current status does not match the activity's new status. Cannot undo.";
    }

    // Ask the owning module to restore its data
    if (error == "") {
        bool restored = false;
        if (target.activityType == ACT_CREATE_REQUEST) {
            restored = service.removeActiveRequest(target.requestId);                  // Task 2
        } else if (target.activityType == ACT_ASSIGN_RESOURCE) {
            restored = delivery.releaseAssignment(parcelId, target.resourceId);        // Task 3
        } else {
            restored = intake.restoreParcelStatus(parcelId, target.newStatus,          // Task 1
                                                  target.previousStatus, target.previousNextAction);
        }
        if (!restored) {
            error = "[Error] The module that owns this data refused the undo (data already changed).";
        }
    }

    // On any error put the stack back exactly as it was
    if (error != "") {
        if (found) {
            history.push(target);
        }
        transferAll(aux, history);
        cout << error << "\n";
        return false;
    }

    target.reversed = true;
    history.push(target);
    transferAll(aux, history);

    // The undo itself is logged as a new activity
    ActivityRecord undoRecord;
    undoRecord.activityId = nextActivityId++;
    undoRecord.parcelId = parcelId;
    undoRecord.activityType = ACT_UNDO;
    undoRecord.previousStatus = target.newStatus;
    undoRecord.newStatus = target.previousStatus;
    undoRecord.previousNextAction = target.newNextAction;
    undoRecord.newNextAction = target.previousNextAction;
    undoRecord.requestId = target.requestId;
    undoRecord.resourceId = target.resourceId;
    undoRecord.staffId = currentStaffId;
    undoRecord.timestamp = currentTimestamp();
    undoRecord.reversed = false;
    history.push(undoRecord);

    cout << "[Success] Activity #" << activityId << " for Parcel " << parcelId
         << " reversed. Status restored to: " << target.previousStatus << "\n";
    return true;
}

// CSV load: activityId,timestamp,parcelId,activityType,previousStatus,newStatus,
//           previousNextAction,newNextAction,requestId,resourceId,staffId,reversed
int ActivityManager::loadFromCSV(const string& filePath) {
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return -1;
    }
    string line, f[12];
    int loaded = 0, lineNo = 0;
    getline(file, line);            // skip header
    while (getline(file, line)) {
        ++lineNo;
        if (line.empty() || line == "\r") continue;

        ActivityRecord r;
        Parcel parcel;
        bool valid = (splitCsvLine(line, f, 12) == 12) &&
                     parseActivityType(f[3], r.activityType) &&
                     (f[11] == "0" || f[11] == "1") &&
                     atoi(f[0].c_str()) > 0 &&
                     intake.getParcel(f[2], parcel);
        if (!valid || history.isFull()) {
            cout << "[Warning] " << filePath << " data line " << lineNo << " skipped (invalid).\n";
            continue;
        }
        r.activityId = atoi(f[0].c_str());
        r.timestamp = f[1];
        r.parcelId = f[2];
        r.previousStatus = f[4];
        r.newStatus = f[5];
        r.previousNextAction = f[6];
        r.newNextAction = f[7];
        r.requestId = f[8];
        r.resourceId = f[9];
        r.staffId = f[10];
        r.reversed = (f[11] == "1");
        history.push(r);
        if (r.activityId >= nextActivityId) nextActivityId = r.activityId + 1;
        ++loaded;
    }
    return loaded;
}

// Save oldest first, using an auxiliary stack
bool ActivityManager::saveToCSV(const string& filePath) {
    ofstream file(filePath.c_str());
    if (!file.is_open()) {
        cout << "[Error] Unable to open file for writing: " << filePath << "\n";
        return false;
    }
    file << "activityId,timestamp,parcelId,activityType,previousStatus,newStatus,"
            "previousNextAction,newNextAction,requestId,resourceId,staffId,reversed\n";

    ActivityStack aux;
    transferAll(history, aux);
    ActivityRecord r;
    while (aux.pop(r)) {
        file << r.activityId << "," << r.timestamp << "," << r.parcelId << ","
             << activityToString(r.activityType) << "," << r.previousStatus << ","
             << r.newStatus << "," << r.previousNextAction << "," << r.newNextAction << ","
             << r.requestId << "," << r.resourceId << "," << r.staffId << ","
             << (r.reversed ? "1" : "0") << "\n";
        history.push(r);
    }
    return true;
}

// Module menu
void ActivityManager::runMenu() {
    while (true) {
        cout << "\n========================================\n";
        cout << " TASK 4: PARCEL ACTIVITY & RECORD MANAGEMENT\n";
        cout << " (Staff: " << currentStaffId << ", records: " << history.size() << ")\n";
        cout << "========================================\n";
        cout << "1. Display Recent Activities\n";
        cout << "2. Display / Review Parcel Activity History\n";
        cout << "3. Undo an Activity\n";
        cout << "0. Back to Main Menu\n";

        int choice = readNumber("Enter choice: ", 0, 3);
        if (choice == 0) return;

        if (choice == 1) {
            displayRecentActivities(readNumber("How many recent activities? (1-50): ", 1, 50));
        } else if (choice == 2) {
            displayParcelHistory(readRequired("Enter Parcel ID: "));
        } else {
            string parcelId = readRequired("Enter Parcel ID: ");
            int activityId = readNumber("Enter Activity ID to undo: ", 1, 1000000);
            undoActivity(parcelId, activityId);
        }
    }
}