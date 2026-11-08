#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <cstdlib>
using namespace std;

enum ParcelStatus{
    ST_ARRIVED, ST_REGISTERED, ST_SORTED, ST_REQUEST_CREATED, ST_ASSIGNED, 
    ST_COLLECTED, ST_DELIVERED, ST_RETURNED, ST_CANCELLED, ST_ON_HOLD, ST_MISSING
};

enum ActivityType{
    ACT_REGISTER, ACT_SORT, ACT_CREATE_REQUEST, ACT_ASSIGN_RESOURCE, ACT_COLLECT, 
    ACT_DELIVER, ACT_RETURN, ACT_CANCEL, ACT_HOLD, ACT_MARK_MISSING, ACT_UNDO
};

string statusToString(ParcelStatus status); 
string activityToString(ActivityType type);
bool parseStatus(const string& text, ParcelStatus& status); //failed if invalid
bool parseActivityType(const string& text, ActivityType& type); //failed if invalid
// Business rule: final or completed operations and undo entries cannot be reversed.
bool isReversible(ActivityType type);
string currentTimestamp(); //"YYYY-MM-DD HH:MM:SS" format
// Splits a CSV line into fields[] {max maxFields}, returns number of fields found. Move everything to aux (aux top = oldest)
int splitCsvLine(const string& line, string fields[], int maxFields);

//Definitions of the helper functions declared above
static const char* STATUS_NAMES[] = {
    "ARRIVED", "REGISTERED", "SORTED", "REQUEST_CREATED", "ASSIGNED",
    "COLLECTED", "DELIVERED", "RETURNED", "CANCELLED", "ON_HOLD", "MISSING"
};

static const int STATUS_COUNT = 11;

static const char* ACTIVITY_NAMES[] = {
    "REGISTER", "SORT", "CREATE_REQUEST", "ASSIGN_RESOURCE", "COLLECT",
    "DELIVER", "RETURN", "CANCEL", "HOLD", "MARK_MISSING", "UNDO"
};

static const int ACTIVITY_COUNT = 11;

string statusToString(ParcelStatus status) {
    return STATUS_NAMES[status];
}
string activityToString(ActivityType type) {
    return ACTIVITY_NAMES[type];
}

bool parseStatus(const string& text, ParcelStatus& status) {
    for (int i = 0; i < STATUS_COUNT; ++i) {
        if (text == STATUS_NAMES[i]) {
            status = (ParcelStatus)i;
            return true;
        }
    }
    return false;
}

bool parseActivityType(const string& text, ActivityType& type) {
    for (int i = 0; i < ACTIVITY_COUNT; ++i) {
        if (text == ACTIVITY_NAMES[i]) {
            type = (ActivityType)i;
            return true;
        }
    }
    return false;
}

bool isReversible(ActivityType type) {
    return type != ACT_DELIVER && type != ACT_RETURN && type != ACT_COLLECT && type != ACT_REGISTER && type != ACT_UNDO;
}

string currentTimestamp(){
    time_t now = time(nullptr);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buffer);
}

int splitCsvLine(const string& line, string fields[], int maxFields) {
    int count = 0;
    string current = "";

    for (size_t i = 0; i <= line.size(); ++i) {
        if(i == line.size() || line[i] == ',') {
            if(count < maxFields) {
                fields[count] = current;
            }
            ++count;
            current = "";
        } else if(line[i] != '\r') {
                current += line[i++];
            }
    }
    return count;
}

// ActivityRecord structure to hold details of each activity history entry. (stored inside the stack)
struct ActivityRecord {
    int activityId;
    string parcelId;
    ActivityType activityType;
    ParcelStatus previousStatus;
    ParcelStatus newStatus;
    string previousRequestId;
    string newRequestId;
    string previousResourceId;
    string newResourceId;
    string staffId;
    string timestamp;
    bool reversed;

};

//ActivityStack class to manage the history of activities using a linked list implementation.
class ActivityStack{
    private:
        struct Node{
            ActivityRecord data;
            Node* next;
        };
        Node* topNode;
        int count;
        int capacity;

        ActivityStack(const ActivityStack&);
        ActivityStack& operator=(const ActivityStack&);
    
    public:
        explicit ActivityStack(int maxSize = 1000) : topNode(nullptr), count(0), capacity(maxSize) {}
        ~ActivityStack() {clear();}
        bool isEmpty() const {return topNode == nullptr;}
        bool isFull() const {return count >= capacity;}
        int size() const {return count;}

        bool push(const ActivityRecord& rec) {
            if (isFull()) return false;
            Node* n = new Node{rec, topNode};
            topNode = n;
            count++;
            return true;
        }

        bool pop(ActivityRecord& out) {
            if (isEmpty()) return false;
            Node* n = topNode;
            out = n->data;
            topNode = n->next;
            delete n;
            count--;
            return true;
        }

        bool peek(ActivityRecord& out) const {
            if (isEmpty()) return false;
            out = topNode->data;
            return true;
        }

        void clear() {
            ActivityRecord dummy;
            while (pop(dummy)){}
        };


};

// Minimal parcel registry, integration point with Tasks 1-3 & 5
// Replace or link with the group real Parcel storage.
struct Parcel {
    string parcelId;
    ParcelStatus status;
    string requestId;
    string resourceId;
    bool used;
};

class ParcelRegistry{
    private:
        static const int MAX_PARCELS = 500;
        Parcel parcels[MAX_PARCELS];
        int total;
    
    public:
        ParcelRegistry() : total(0) {}
        bool addParcel(const string& id, ParcelStatus status = ST_ARRIVED, 
            const string& requestId = "-", const string& resourceId = "-") {
            if (id.empty() || total >= MAX_PARCELS || findParcel(id) != nullptr) return false;
            parcels[total++] = Parcel{id, status, requestId, resourceId, true};
            return true;
        }

        Parcel* findParcel(const string& id) {
            for (int i = 0; i < total; ++i) {
                if (parcels[i].parcelId == id) return &parcels[i];
            }
            return nullptr;
        }

        // Load parcels CSV (parcelId,status,requestId,resourceId), return parcel loaded, -1 if error
        int loadFromCSV(const string& filePath) {
            ifstream file(filePath.c_str());
            if (!file.is_open()) {
                cerr << "[Error] Unable to open file: " << filePath << endl;
                return -1;
            }
            string line, f[4];
            int loaded = 0, lineNo = 0;
            getline(file, line); // skip header
            while (getline(file, line)) {
                ++lineNo;
                if(line.empty() || line == "\r") continue;

                ParcelStatus status;
                if(splitCsvLine(line, f, 4) != 4 || !parseStatus(f[1], status) 
                    || !addParcel(f[0], status, f[2], f[3])) {
                    cout << "[Warning] " << filePath << " data line " 
                    << lineNo << " skipped (invalid).\n";
                    continue;
                }
                ++loaded;
            }
            return loaded;
        }

        //Save all parcels back to CSV, return true if saved successfully.
        bool saveToCSV(const string& filePath) {
            ofstream file(filePath.c_str());
            if (!file.is_open()) {
                cout << "[Error] Unable to open file for writing: " << filePath << endl;
                return false;
            }

            // Write header
            file << "ParcelID,Status,RequestID,ResourceID\n";
            for (int i = 0; i < total; ++i) {
                file << parcels[i].parcelId << ","
                     << statusToString(parcels[i].status) << ","
                     << parcels[i].requestId << ","
                     << parcels[i].resourceId << "\n";
            }
            return true;
        }

};

// ActivityManager class to handle recording and displaying activities.(Task 4)
class ActivityManager{
    private:
        ActivityStack history;
        ParcelRegistry& registry;
        int nextActivityId;

        static void printRecord(const ActivityRecord& r) {
            cout << "Activity ID: " << r.activityId << endl;
            cout << "Parcel ID: " << r.parcelId << endl;
            cout << "Activity Type: " << activityToString(r.activityType) << endl;
            cout << "Previous Status: " << statusToString(r.previousStatus) << endl;
            cout << "New Status: " << statusToString(r.newStatus) << endl;
            cout << "Previous Request ID: " << r.previousRequestId << endl;
            cout << "New Request ID: " << r.newRequestId << endl;
            cout << "Previous Resource ID: " << r.previousResourceId << endl;
            cout << "New Resource ID: " << r.newResourceId << endl;
            cout << "Staff ID: " << r.staffId << endl;
            cout << "Timestamp: " << r.timestamp << endl;
            cout << "Reversed: " << (r.reversed ? "Yes" : "No") << endl;
            cout << "----------------------------------------" << endl;
        }

        //Moves every record from "from" stack to "to" stack, effectively reversing the order of records.
        static void transferAll(ActivityStack& from, ActivityStack& to) {
            ActivityRecord r;
            while (from.pop(r)) {
                to.push(r);
            }
        }

        public:
            int loadFromCSV(const string& filePath); //returns number of records loaded, -1 if error
            bool saveToCSV(const string& filePath); //returns true if saved successfully, false if error
            explicit ActivityManager(ParcelRegistry& reg) : registry(reg), nextActivityId(1) {}

            // Req 1: Record an activity in the history stack, updating the parcel's status (called by other modules)
            // Applies the change to parcel and records it so it can be undone
            bool recordActivity(const string& parcelId, ActivityType type, ParcelStatus newStatus, 
                                const string& requestId, const string& resourceId, const string& staffId) {
                Parcel* p = registry.findParcel(parcelId);
                if (p == nullptr) {
                    cout << "[Error] Invalid Parcel ID: " << parcelId << endl;
                    return false;
                }
                if (history.isFull()) {
                    cout << "[Error] Activity history is full. Cannot record new activity." << endl;
                    return false;
                }

                ActivityRecord r;
                r.activityId = nextActivityId++;
                r.parcelId = parcelId;
                r.activityType = type;
                r.previousStatus = p->status;
                r.newStatus = newStatus;
                r.previousRequestId = p->requestId;
                r.newRequestId = requestId;
                r.previousResourceId = p->resourceId;
                r.newResourceId = resourceId;
                r.staffId = staffId;
                r.timestamp = currentTimestamp();
                r.reversed = false;


                // Update parcel status and related fields
                p->status = newStatus;
                p->requestId = requestId;
                p->resourceId = resourceId;

                return history.push(r);
            }

            //Req 2: recent activities display function, shows the most recent activities first.
            void displayRecentActivities(int limit){
                if(history.isEmpty()){
                    cout << "[Info] No activities recorded yet." << endl;
                    return;
                }
                ActivityStack aux;
                ActivityRecord r;
                int shown = 0;
                cout << "\n --- Recent Activities (newest first) ---" << endl;
                while(shown < limit && history.pop(r)){
                    printRecord(r);
                    aux.push(r);
                    ++shown;
                }
                // Transfer activities back to the original stack
                transferAll(aux, history);
            }

            //Req 2&3: history of one parcel (oldest -> newest)
            void displayParcelHistory(const string& parcelId){
                Parcel* p = registry.findParcel(parcelId);
                if(p == nullptr){
                    cout << "[Error] Invalid Parcel ID: " << parcelId << endl;
                    return;
                }
                if(history.isEmpty()){
                    cout << "[Info] No activities recorded yet." << endl;
                    return;
                }

                //Step 1: pop everything from history, check for matching parcelId, print if found, and push to aux stack. (aux top = Oldest Record)
                ActivityStack aux;
                transferAll(history, aux);
                
                //Step 2: pop from aux (oldest first), print if parcelId matches, and push back to history (history top = Newest Record)
                ActivityRecord r, lastMatch;
                int matches = 0;
                cout << "\n --- Activity History for Parcel ID: " << parcelId << " (oldest -> newest) ---" << endl;
                while(aux.pop(r)){
                    if(r.parcelId == parcelId){
                        printRecord(r);
                        lastMatch = r;
                        ++matches;
                    }
                    history.push(r); //restores the original order of history stack
                }
                if(matches == 0){
                    cout << "[Info] No activities found for Parcel ID: " << parcelId << endl;
                    return;
                }

                //Review summary of the last matching record (Req 3)
                cout << "\n Review Summary\n" 
                     << " Total Activities          : " << matches << "\n"
                     << " Most Recent Activity ID   : #" << lastMatch.activityId << " " << activityToString(lastMatch.activityType) << " at " << lastMatch.timestamp << "\n"
                     << " Previous Status           : " << statusToString(lastMatch.previousStatus) << "\n"
                     << " Current Status            : " << statusToString(p->status) << "\n"
                     << " Request ID                : " << p->requestId << "\n"
                     << " Resource ID               : " << p->resourceId << "\n";
            }

            // Req 4&5: undo a recent activity
            // Rules: 1.Parcel must exist, 2. Activity type must be reversible, 3. Activity must not have been reversed already. 
            // 4. Activity must be the most recent non-reversed activity for that parcel. 5. Activity must be exist for that parcel. 
            // 6. Parcel current status must match the activity's new status, else restoring would create an invalid status.

            bool undoActivity(const string& parcelId, int activityId, const string& staffId){
                Parcel* p = registry.findParcel(parcelId);
                if(p == nullptr){
                    cout << "[Error] Invalid Parcel ID: " << parcelId << endl;
                    return false;
                }
                if(history.isEmpty()){
                    cout << "[Error]No activities recorded to undo." << endl;
                    return false;
                }

                ActivityStack aux;
                ActivityRecord r, target;
                bool found = false, newerActiveExists = false;

                //Pop until the selected activity is found.
                while(history.pop(r)){
                    if( r.activityId == activityId){
                        target = r;
                        found = true;
                        break;
                    }
                    if(r.parcelId == parcelId && !r.reversed && r.activityType != ACT_UNDO){
                        newerActiveExists = true;
                    }
                    aux.push(r);
                }

                string error = "";
                if(!found){
                    error = "[Error] Activity ID not found in history.";
                } else if(target.parcelId != parcelId){
                    error = "[Error] Activity ID does not belong to the specified Parcel ID.";
                } else if (target.activityType == ACT_UNDO){
                    error = "[Error] An undo entry cannot be reversed.";
                } else if(!isReversible(target.activityType)){
                    error = "[Error] Activity type " + activityToString(target.activityType) + " cannot be reversed.";
                } else if(target.reversed){
                    error = "[Error] Activity has already been reversed.";
                } else if(newerActiveExists){
                    error = "[Error] There are newer non-reversed activities for this parcel. Undo must be the most recent.";
                } else if(p->status != target.newStatus){
                    error = "[Error] Parcel current status does not match the activity's new status. Cannot undo.";
                }

                // put everything back untouched.
                if(error != ""){

                    if(found){
                        // push back target and everything from aux to history
                        history.push(target);
                    }
                    transferAll(aux, history);
                    cout << error << endl;
                    return false;
                }

                //Restore previous parcel or request or resource information
                p->status = target.previousStatus;
                p->requestId = target.previousRequestId;
                p->resourceId = target.previousResourceId;
                target.reversed = true;
                history.push(target); // push the reversed activity back to history
                transferAll(aux, history); // restore the newest history

                // Log the undo itself as a new activity
                ActivityRecord undoRecord;
                undoRecord.activityId = nextActivityId++;
                undoRecord.parcelId = parcelId;
                undoRecord.activityType = ACT_UNDO;
                undoRecord.newStatus = target.previousStatus;
                undoRecord.previousStatus = target.newStatus;
                undoRecord.previousRequestId = target.newRequestId;
                undoRecord.previousResourceId = target.newResourceId;
                undoRecord.newRequestId = target.previousRequestId;
                undoRecord.newResourceId = target.previousResourceId;
                undoRecord.staffId = staffId;
                undoRecord.timestamp = currentTimestamp();
                undoRecord.reversed = false;
                history.push(undoRecord);

                cout << "[Success] Activity ID #" << activityId << " for Parcel ID " << parcelId << " has been restored to " << statusToString(p->status) << ".\n";
                return true;

            }

            int totalRecords() const {
                return history.size();
            }

};

//CSV load / save
int ActivityManager::loadFromCSV(const string& filePath) {
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        cerr << "[Error] Unable to open file: " << filePath << endl;
        return -1;
    }
     string line, f[12];
     int loaded = 0, lineNo = 0;
     getline(file, line); // skip header
     while (getline(file, line)) {
        ++lineNo;
        if(line.empty() || line == "\r") continue;

        ActivityRecord r;
        bool valid = (splitCsvLine(line, f, 12) == 12) && 
                      parseActivityType(f[3], r.activityType) &&
                      parseStatus(f[4], r.previousStatus) &&
                      parseStatus(f[5], r.newStatus) &&
                      (f[11] == "0" || f[11] == "1") &&
                      atoi(f[0].c_str()) > 0 &&
                      registry.findParcel(f[2]) != nullptr;
        if (!valid || history.isFull()){
            cout << "[Warning] " << filePath << " data line " << lineNo << " skipped (invalid).\n";
            continue;
        }

        r.activityId = atoi(f[0].c_str());
        r.timestamp = f[1];
        r.parcelId = f[2];
        r.previousRequestId = f[6];
        r.newRequestId = f[7];
        r.previousResourceId = f[8];
        r.newResourceId = f[9];
        r.staffId = f[10];
        r.reversed = (f[11] == "1");
        history.push(r);
        if(r.activityId >= nextActivityId) nextActivityId = r.activityId + 1;
        ++loaded;
    }
    return loaded;
    
}

// Save the history to csv (oldest first) using stack operations
bool ActivityManager::saveToCSV(const string& filePath) {
    ofstream file(filePath.c_str());
    if (!file.is_open()) {
        cout << "[Error] Unable to open file for writing: " << filePath << endl;
        return false;
    }

    // Write header
    file << "ActivityID,Timestamp,ParcelID,ActivityType,PreviousStatus,NewStatus,PreviousRequestID,NewRequestID,PreviousResourceID,NewResourceID,StaffID,Reversed\n";

    // Use an auxiliary stack to reverse the order for saving (oldest first)
    ActivityStack aux;
    transferAll(history, aux);
    ActivityRecord r;
    while (aux.pop(r)) {
        file << r.activityId << ","
             << r.timestamp << ","
             << r.parcelId << ","
             << activityToString(r.activityType) << ","
             << statusToString(r.previousStatus) << ","
             << statusToString(r.newStatus) << ","
             << r.previousRequestId << ","
             << r.newRequestId << ","
             << r.previousResourceId << ","
             << r.newResourceId << ","
             << r.staffId << ","
             << (r.reversed ? "1" : "0") << "\n";
        history.push(r);  //restore stack order
    }

    return true;
}
