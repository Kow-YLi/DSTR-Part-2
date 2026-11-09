#ifndef CSV_UTILS_HPP
#define CSV_UTILS_HPP
#include <string>

// Split a CSV line into fields[] (max maxFields). Returns the number of fields found.
// Does not handle qupted fields.
inline int splitCsvLine(const std::string& line, std::string fields[], int maxFields){
    int count = 0;
    std::string current = "";
    for(size_t i = 0; i <= line.size(); ++i){
        if(i == line.size() || line[i] == ','){
            if(count < maxFields) fields[count] = current;
            ++count;
            current = "";
        }else if(line[i] != '\r'){
            current += line[i];
        }
    }
    return count;
}

#endif // CSV_UTILS_HPP