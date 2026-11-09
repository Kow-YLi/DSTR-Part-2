#ifndef ACTIVITY_LOGGER_HPP
#define ACTIVITY_LOGGER_HPP
#include <string>

enum ActivityType {
    ACT_REGISTER, ACT_SORT, ACT_CREATE_REQUEST, ACT_ASSIGN_RESOURCE,
    ACT_COLLECT, ACT_DELIVER, ACT_RETURN, ACT_CANCEL, ACT_HOLD,
    ACT_MARK_MISSING, ACT_UNDO,
    ACT_RESORT, ACT_PROCESS_REQUEST, ACT_CANCEL_REQUEST
};

const int ACTIVITY_TYPE_COUNT = 14;
 
inline std::string activityToString(ActivityType type) {
    static const char* names[ACTIVITY_TYPE_COUNT] = {
        "REGISTER", "SORT", "CREATE_REQUEST", "ASSIGN_RESOURCE",
        "COLLECT", "DELIVER", "RETURN", "CANCEL", "HOLD",
        "MARK_MISSING", "UNDO",
        "RESORT", "PROCESS_REQUEST", "CANCEL_REQUEST"
    };
    return names[type];
}

// false if the text is not a valid activity type
inline bool parseActivityType(const std::string& text, ActivityType& type) {
    for (int i = 0; i < ACTIVITY_TYPE_COUNT; ++i) {
        if (text == activityToString((ActivityType)i)) {
            type = (ActivityType)i;
            return true;
        }
    }
    return false;
}

// Business rule: final or completed operations, registration, request
// processing/cancellation and undo entries cannot be reversed.
inline bool isReversible(ActivityType type) {
    return type == ACT_SORT || type == ACT_CREATE_REQUEST ||
           type == ACT_ASSIGN_RESOURCE || type == ACT_CANCEL ||
           type == ACT_HOLD || type == ACT_MARK_MISSING ||
           type == ACT_RESORT;
}

class ActivityLogger {
public:
    virtual ~ActivityLogger() {}
 
    // Req 1 (record parcel-related activities). "-" = not applicable.
    virtual void recordActivity(const std::string& parcelID,
                                ActivityType type,
                                const std::string& previousStatus,
                                const std::string& newStatus,
                                const std::string& previousNextAction,
                                const std::string& newNextAction,
                                const std::string& requestID,
                                const std::string& resourceID) = 0;
};
 
#endif // ACTIVITY_LOGGER_HPP