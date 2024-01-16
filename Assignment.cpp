#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Employee {
public:
    vector<string> data; 

    string extractDate(const string& dateTime) const {
        size_t pos = dateTime.find(' ');
        return (pos != string::npos) ? dateTime.substr(0, pos) : dateTime;
    }

    bool areConsecutiveWorkingDates(const string& date1, const string& date2) const {
        return convertToComparableDate(date2) == convertToComparableDate(date1) + 1;
    }

    int convertToComparableDate(const string& date) const {
        size_t pos1 = date.find('/');
        size_t pos2 = date.rfind('/');

        int month = stoi(date.substr(0, pos1));
        int day = stoi(date.substr(pos1 + 1, pos2 - pos1 - 1));
        int year = stoi(date.substr(pos2 + 1));

        return year * 10000 + month * 100 + day;
    }

    string extractTime(const string& dateTime) const {
        size_t pos = dateTime.find(' ');
        return (pos != string::npos) ? dateTime.substr(pos + 1) : "";
    }

    int convertToComparableTime(const string& time) const {
        size_t pos = time.find(':');
        if (pos == string::npos) {
            throw invalid_argument("Invalid time format: " + time);
        }

        int hour = stoi(time.substr(0, pos));
        int minute = stoi(time.substr(pos + 1));

        size_t ampmPos = time.find("AM");
        if (ampmPos != string::npos) {
            if (hour == 12) {
                hour = 0; 
            }
        } else {
            size_t pmPos = time.find("PM");
            if (pmPos != string::npos && hour != 12) {
                hour += 12;  
            }
        }

        return hour * 60 + minute;
    }
};

int main() {
    string filePath = "C:\\Users\\Lenovo\\OneDrive\\Desktop\\Bluejay Delivery Assigment\\Assignment_Timecard.csv - Sheet1.csv";

    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }
    
    //unordered_map to store emplyee id and corresponding data since we need to search for data for each employee and key value structure can be helpful here.
    //Additionaly Unordered_map will take TC = Log(n) and will be useful for larger values of n 
    
    unordered_map<string, set<string>> employeeTimeGaps; // set will contain the shift gaps  
    unordered_map<string, vector<string>> employeesMoreThan14Hours; //shit stored in vector 
    unordered_map<string, set<string>> employeeDatesMap; // set will contain unique dates on which emplyee worked 

    string line;
    Employee emp;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        emp.data = tokens;

        string employeeID = emp.data[0];
        string workDate = emp.extractDate(emp.data[2]);

        employeeDatesMap[employeeID].insert(workDate);
    }

    inputFile.close();

    // Iterate over the map and find employees who worked for 7 consecutive working days
    cout << "Employee who have worked for 7 consecutive days" << endl;
    for (const auto& entry : employeeDatesMap) {
        const set<string>& dates = entry.second;
        if (dates.size() >= 7) {
            auto it = dates.begin();
            auto endIt = prev(dates.end(), 6); 
            while (it != endIt) {
                auto nextIt = next(it, 1);
                if (nextIt != dates.end() && emp.areConsecutiveWorkingDates(*it, *nextIt)) {
                    break;
                }
                ++it;
            }
            if (it == endIt) {
                cout << "Employee ID: " << entry.first << endl;
                cout << endl;
            }
        }
    }

    inputFile.open(filePath);
    if (!inputFile.is_open()) {
        cout << "Error opening file" << endl;
        return 1;
    }

    while (getline(inputFile, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        emp.data = tokens;

        string employeeID = emp.data[0];
        string checkInDate = emp.extractDate(emp.data[2]);
        string checkOutTime = emp.extractTime(emp.data[3]);
        string nextCheckInTime = emp.extractTime(emp.data[2]);

        try {
            int checkOutComparableTime = emp.convertToComparableTime(checkOutTime);
            int nextCheckInComparableTime = emp.convertToComparableTime(nextCheckInTime);

            int timeGap = abs(nextCheckInComparableTime - checkOutComparableTime);

            if (timeGap > 60 && timeGap < 600) {
                employeeTimeGaps[employeeID].insert(checkInDate);
            }

            int shiftDuration = emp.convertToComparableTime(emp.data[4]);
            if (shiftDuration > 840) {  
                employeesMoreThan14Hours[employeeID].push_back(checkInDate);
            }

        } catch (const invalid_argument& e) {
            // cerr << "Error: " << e.what() << endl;
        }
    }

    inputFile.close();

    cout << "Employees who have less than 10 hours of time between shifts but greater than 1 hour" << endl;
    for (const auto& entry : employeeTimeGaps) {
        cout << "Employee ID: " << entry.first << endl;
    }
    cout << endl;

    for (const auto& entry : employeesMoreThan14Hours) {
        cout << "Employee ID: " << entry.first << " has worked more than 14 hours";
    }

    return 0;
}
