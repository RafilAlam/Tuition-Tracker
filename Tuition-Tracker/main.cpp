#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <string>
using namespace std;

// prerequisite variables and structures

time_t timestamp = time(&timestamp);
struct tm datetime = *localtime(&timestamp);

string subjectString[4] = {"English", "E-Math", "A-Math", "Science"};
string monthString[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
string dayString[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

struct Lesson_Log {
    int subject; // 0 - English; 1 - E-Math; 2 - A-Math; 3 - Science;
    int day;
    int dayotw;
    int month;
    int fee;
    bool attendance;
};

// Handles logging and reading data

int logData(int subject, int day, int dayotw, int month, int fee = 75, bool attendance = true) {
    ofstream Data("tuition-log.dat", ios::binary | ios::app);

    if (!Data) {
        cout << "Unable to open file!" << endl;
        return 1;
    }

    Lesson_Log lesson;

    lesson.subject = subject;
    lesson.day = day;
    lesson.dayotw = dayotw;
    lesson.month = month;
    lesson.fee = fee;
    lesson.attendance = attendance;

    Data.write((char *) & lesson, sizeof(Lesson_Log));
    Data.close();

    if (!Data.good()) {
        cout << "Error occurred while writing!" << endl;
        return 1;
    }
}

vector<Lesson_Log> loadedlessons;
int readData() {
    ifstream iData("tuition-log.dat", ios::binary | ios::ate);
    if (!iData) {
        cout << "Unable to open file!" << endl;
        return 1;
    }

    streamsize fileSize = iData.tellg();
    iData.seekg(0, ios::beg);

    size_t leCount = fileSize / sizeof(Lesson_Log);

    loadedlessons.resize(leCount);
    for (int i=0; i < leCount; ++i) {
        iData.read((char *) & loadedlessons[i], sizeof(Lesson_Log));
    }
    iData.close();

    if (!iData.good()) {
        cout << "Error occurred while reading!" << endl;
        return 1;
    }
}

// Main Loop

bool running = true;
string input;
vector<string> cmd;
int subject;
int main() {
    cout << "\033[1;32m=======================================\033[0m\n";
    cout << "\033[1;36m    Welcome to the Tuition Tracker!   \033[0m\n";
    cout << "\033[1;36m  Type 'help' for a list of commands.     \033[0m\n";
    cout << "\033[1;32m=======================================\033[0m\n\n";

    while (running) {
        cmd.clear();
        cout << ">";
        getline(cin, input);

        stringstream ss(input);
        string word;
        while (ss >> word) {
            cmd.push_back(word);
        }

        if (cmd.empty()) continue;

        if (cmd[0] == "log") {
            subject = stoi(cmd[cmd.size()-1]);
            if (cmd[1] == "-a") {
                logData(subject, datetime.tm_mday, datetime.tm_wday, datetime.tm_mon, 0, false);
            } else {
                logData(subject, datetime.tm_mday, datetime.tm_wday, datetime.tm_mon);
            }
        } else if (cmd[0] == "dmp") {
            readData();
            int totalFees = 0;
            vector<int> months;
            ofstream logdump("tuition_logdump.txt");
            for (int i=0; i < loadedlessons.size(); ++i) {
                Lesson_Log lesson = loadedlessons[i];
                
                auto it = find(months.begin(), months.end(), lesson.month);
                if (it == months.end()) {
                    if (months.size() > 0) {
                        logdump << endl << endl << "Total Fees: " << totalFees << endl;
                        totalFees = 0;
                    }
                    logdump << "----------------------------------" << endl;
                    logdump << monthString[lesson.month] << endl;
                    months.push_back(lesson.month);
                }
                logdump << "    • " << dayString[lesson.dayotw] << ", " << lesson.day << " - " << subjectString[lesson.subject] << " -> $" << lesson.fee;
                if (!lesson.attendance) {
                    logdump << " (ABSENT)";
                }
                logdump << endl;

                totalFees += lesson.fee;
            }
            logdump << endl << endl << "Total Fees: $" << totalFees << endl;
            logdump << "----------------------------------" << endl;
            logdump.close();
        } else if (cmd[0] == "exit") {
            running = false;
        } else if (cmd[0] == "help") {
            cout << "\n\n\033[1;34m================= HELP MENU =================\033[0m\n";
            cout << "\033[1;33mCommands:\033[0m\n";
            cout << "\033[1;32m  1. log -a   \033[0m- Add a new lesson log (-a to mark as absent).\n";
            cout << "\033[1;32m  2. dmp      \033[0m- Dump all lesson logs into a .txt file.\n";
            cout << "\033[1;32m  3. exit     \033[0m- Exit the session.\n";
            cout << "\033[1;34m=============================================\033[0m\n\n";
        }
    }

    return 0;
}