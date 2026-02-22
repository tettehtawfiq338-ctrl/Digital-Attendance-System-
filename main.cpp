#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <limits>

using namespace std;

// ==============================
// 1. STUDENT CLASS
// ==============================
class Student {
private:
    string indexNumber;
    string name;
    
public:
    Student() : indexNumber(""), name("") {}
    
    Student(string idx, string n) : indexNumber(idx), name(n) {}
    
    // Getters
    string getIndex() const { return indexNumber; }
    string getName() const { return name; }
    
    // Setters
    void setIndex(string idx) { indexNumber = idx; }
    void setName(string n) { name = n; }
    
    // Display student info
    void display() const {
        cout << left << setw(15) << indexNumber 
             << setw(25) << name << endl;
    }
    
    // Convert to string for file storage
    string toString() const {
        return indexNumber + "," + name;
    }
    
    // Load from string (for file reading)
    static Student fromString(const string& data) {
        size_t commaPos = data.find(',');
        if (commaPos != string::npos) {
            string idx = data.substr(0, commaPos);
            string name = data.substr(commaPos + 1);
            return Student(idx, name);
        }
        return Student("", "");
    }
};

// ==============================
// 2. ATTENDANCE RECORD CLASS
// ==============================
class AttendanceRecord {
private:
    string studentIndex;
    char status; // 'P' = Present, 'A' = Absent, 'L' = Late
    
public:
    AttendanceRecord() : studentIndex(""), status('A') {}
    
    AttendanceRecord(string idx, char s) : studentIndex(idx), status(s) {}
    
    // Getters
    string getStudentIndex() const { return studentIndex; }
    char getStatus() const { return status; }
    
    // Setters
    void setStatus(char s) { status = s; }
    
    // Display record
    void display(const vector<Student>& students) const {
        // Find student name
        string studentName = "Unknown";
        for (const auto& student : students) {
            if (student.getIndex() == studentIndex) {
                studentName = student.getName();
                break;
            }
        }
        
        string statusText;
        switch (status) {
            case 'P': statusText = "Present"; break;
            case 'A': statusText = "Absent"; break;
            case 'L': statusText = "Late"; break;
            default: statusText = "Unknown";
        }
        
        cout << left << setw(15) << studentIndex 
             << setw(25) << studentName 
             << setw(10) << statusText << endl;
    }
    
    // Convert to string for file storage
    string toString() const {
        return studentIndex + "," + status;
    }
    
    // Load from string
    static AttendanceRecord fromString(const string& data) {
        size_t commaPos = data.find(',');
        if (commaPos != string::npos) {
            string idx = data.substr(0, commaPos);
            char status = data.substr(commaPos + 1)[0];
            return AttendanceRecord(idx, status);
        }
        return AttendanceRecord("", 'A');
    }
};

// ==============================
// 3. ATTENDANCE SESSION CLASS
// ==============================
class AttendanceSession {
private:
    string courseCode;
    string date;
    string startTime;
    int duration; // in hours
    vector<AttendanceRecord> records;
    
public:
    AttendanceSession() : courseCode(""), date(""), startTime(""), duration(0) {}
    
    AttendanceSession(string course, string d, string time, int dur) 
        : courseCode(course), date(d), startTime(time), duration(dur) {}
    
    // Getters
    string getCourseCode() const { return courseCode; }
    string getDate() const { return date; }
    string getStartTime() const { return startTime; }
    int getDuration() const { return duration; }
    vector<AttendanceRecord> getRecords() const { return records; }
    
    // Get filename for this session
    string getFilename() const {
        // Replace hyphens with underscores for filename
        string formattedDate = date;
        replace(formattedDate.begin(), formattedDate.end(), '-', '_');
        return "session_" + courseCode + "_" + formattedDate + ".txt";
    }
    
    // Add a record
    void addRecord(const AttendanceRecord& record) {
        records.push_back(record);
    }
    
    // Update a record
    bool updateRecord(string studentIndex, char status) {
        for (auto& record : records) {
            if (record.getStudentIndex() == studentIndex) {
                record.setStatus(status);
                return true;
            }
        }
        return false;
    }
    
    // Initialize records for all students
    void initializeRecords(const vector<Student>& students) {
        records.clear();
        for (const auto& student : students) {
            records.push_back(AttendanceRecord(student.getIndex(), 'A')); // Default: Absent
        }
    }
    
    // Display session info
    void displayHeader() const {
        cout << "\n==========================================" << endl;
        cout << "SESSION: " << courseCode << endl;
        cout << "DATE: " << date << endl;
        cout << "TIME: " << startTime << " (" << duration << " hours)" << endl;
        cout << "==========================================" << endl;
    }
    
    // Display attendance list
    void displayAttendance(const vector<Student>& students) const {
        cout << "\nATTENDANCE LIST:\n";
        cout << left << setw(15) << "Index Number" 
             << setw(25) << "Student Name" 
             << setw(10) << "Status" << endl;
        cout << string(50, '-') << endl;
        
        for (const auto& record : records) {
            record.display(students);
        }
    }
    
    // Generate summary statistics
    void displaySummary(const vector<Student>& students) const {
        int present = 0, absent = 0, late = 0;
        
        for (const auto& record : records) {
            switch (record.getStatus()) {
                case 'P': present++; break;
                case 'A': absent++; break;
                case 'L': late++; break;
            }
        }
        
        int total = present + absent + late;
        
        cout << "\nATTENDANCE SUMMARY:\n";
        cout << "==========================================" << endl;
        cout << "Course: " << courseCode << " (" << date << ")\n";
        cout << "Total Students: " << total << endl;
        cout << "Present: " << present << " (" 
             << (total > 0 ? (present * 100 / total) : 0) << "%)" << endl;
        cout << "Absent: " << absent << " (" 
             << (total > 0 ? (absent * 100 / total) : 0) << "%)" << endl;
        cout << "Late: " << late << " (" 
             << (total > 0 ? (late * 100 / total) : 0) << "%)" << endl;
        cout << "==========================================" << endl;
    }
    
    // Save session to file
    bool saveToFile() const {
        ofstream file(getFilename());
        if (!file.is_open()) {
            cout << "Error: Could not save session to file!" << endl;
            return false;
        }
        
        // Save session header
        file << "COURSE:" << courseCode << endl;
        file << "DATE:" << date << endl;
        file << "TIME:" << startTime << endl;
        file << "DURATION:" << duration << endl;
        file << "ATTENDANCE_RECORDS:" << endl;
        
        // Save each record
        for (const auto& record : records) {
            file << record.toString() << endl;
        }
        
        file.close();
        cout << "Session saved to: " << getFilename() << endl;
        return true;
    }
    
    // Load session from file
    bool loadFromFile(const string& filename, const vector<Student>& students) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not load session file!" << endl;
            return false;
        }
        
        records.clear();
        string line;
        bool readingRecords = false;
        
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            if (line == "ATTENDANCE_RECORDS:") {
                readingRecords = true;
                continue;
            }
            
            if (!readingRecords) {
                // Read session info
                if (line.find("COURSE:") == 0) {
                    courseCode = line.substr(7);
                } else if (line.find("DATE:") == 0) {
                    date = line.substr(5);
                } else if (line.find("TIME:") == 0) {
                    startTime = line.substr(5);
                } else if (line.find("DURATION:") == 0) {
                    duration = stoi(line.substr(9));
                }
            } else {
                // Read attendance records
                AttendanceRecord record = AttendanceRecord::fromString(line);
                records.push_back(record);
            }
        }
        
        file.close();
        
        // If no records were loaded, initialize with current students
        if (records.empty()) {
            initializeRecords(students);
        }
        
        cout << "Session loaded from: " << filename << endl;
        return true;
    }
};

// ==============================
// 4. SYSTEM MANAGER CLASS
// ==============================
class AttendanceSystem {
private:
    vector<Student> students;
    vector<AttendanceSession> sessions;
    string studentsFile = "students.txt";
    
public:
    // Constructor
    AttendanceSystem() {
        loadStudents();
    }
    
    // ========== STUDENT MANAGEMENT ==========
    
    // Register a new student
    void registerStudent() {
        cout << "\n--- REGISTER NEW STUDENT ---\n";
        string index, name;
        
        cout << "Enter index number: ";
        getline(cin, index);
        
        // Check if student already exists
        for (const auto& student : students) {
            if (student.getIndex() == index) {
                cout << "Error: Student with index " << index << " already exists!" << endl;
                return;
            }
        }
        
        cout << "Enter student name: ";
        getline(cin, name);
        
        Student newStudent(index, name);
        students.push_back(newStudent);
        
        cout << "Student registered successfully!" << endl;
        saveStudents();
    }
    
    // View all students
    void viewAllStudents() const {
        cout << "\n--- ALL REGISTERED STUDENTS ---\n";
        if (students.empty()) {
            cout << "No students registered yet." << endl;
            return;
        }
        
        cout << left << setw(15) << "Index Number" 
             << setw(25) << "Student Name" << endl;
        cout << string(40, '-') << endl;
        
        for (const auto& student : students) {
            student.display();
        }
        
        cout << "\nTotal: " << students.size() << " students" << endl;
    }
    
    // Search student by index
    void searchStudent() const {
        cout << "\n--- SEARCH STUDENT ---\n";
        string index;
        
        cout << "Enter index number to search: ";
        getline(cin, index);
        
        bool found = false;
        for (const auto& student : students) {
            if (student.getIndex() == index) {
                cout << "\nSTUDENT FOUND:\n";
                cout << "Index: " << student.getIndex() << endl;
                cout << "Name: " << student.getName() << endl;
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Student with index " << index << " not found." << endl;
        }
    }
    
    // ========== SESSION MANAGEMENT ==========
    
    // Create a new attendance session
    void createSession() {
        cout << "\n--- CREATE NEW ATTENDANCE SESSION ---\n";
        
        if (students.empty()) {
            cout << "Error: No students registered. Please register students first." << endl;
            return;
        }
        
        string course, date, time;
        int duration;
        
        cout << "Enter course code: ";
        getline(cin, course);
        
        cout << "Enter date (YYYY-MM-DD): ";
        getline(cin, date);
        
        cout << "Enter start time (HH:MM): ";
        getline(cin, time);
        
        cout << "Enter duration (hours): ";
        cin >> duration;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Create new session
        AttendanceSession newSession(course, date, time, duration);
        newSession.initializeRecords(students);
        
        sessions.push_back(newSession);
        
        cout << "\nSession created successfully!" << endl;
        newSession.displayHeader();
    }
    
    // View all sessions
    void viewAllSessions() const {
        cout << "\n--- ALL ATTENDANCE SESSIONS ---\n";
        if (sessions.empty()) {
            cout << "No sessions created yet." << endl;
            return;
        }
        
        for (size_t i = 0; i < sessions.size(); i++) {
            cout << i + 1 << ". " << sessions[i].getCourseCode() 
                 << " - " << sessions[i].getDate() 
                 << " (" << sessions[i].getStartTime() << ")" << endl;
        }
    }
    
    // ========== ATTENDANCE MARKING ==========
    
    // Mark attendance for a session
    void markAttendance() {
        if (sessions.empty()) {
            cout << "No sessions available. Please create a session first." << endl;
            return;
        }
        
        cout << "\n--- MARK ATTENDANCE ---\n";
        viewAllSessions();
        
        int choice;
        cout << "\nSelect session number: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice < 1 || choice > (int)sessions.size()) {
            cout << "Invalid session selection!" << endl;
            return;
        }
        
        AttendanceSession& session = sessions[choice - 1];
        session.displayHeader();
        
        cout << "\nMark attendance for each student:\n";
        cout << "(P = Present, A = Absent, L = Late)\n" << endl;
        
        vector<AttendanceRecord> records = session.getRecords();
        
        for (size_t i = 0; i < students.size(); i++) {
            cout << i + 1 << ". " << students[i].getIndex() 
                 << " - " << students[i].getName() << ": ";
            
            string input;
            getline(cin, input);
            
            if (!input.empty()) {
                char status = toupper(input[0]);
                if (status == 'P' || status == 'A' || status == 'L') {
                    session.updateRecord(students[i].getIndex(), status);
                } else {
                    cout << "  Invalid input. Keeping as Absent." << endl;
                }
            }
        }
        
        cout << "\nAttendance marked successfully!" << endl;
    }
    
    // ========== REPORTS ==========
    
    // View attendance for a session
    void viewAttendanceReport() {
        if (sessions.empty()) {
            cout << "No sessions available." << endl;
            return;
        }
        
        cout << "\n--- VIEW ATTENDANCE REPORT ---\n";
        viewAllSessions();
        
        int choice;
        cout << "\nSelect session number: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice < 1 || choice > (int)sessions.size()) {
            cout << "Invalid session selection!" << endl;
            return;
        }
        
        AttendanceSession& session = sessions[choice - 1];
        session.displayHeader();
        session.displayAttendance(students);
        
        cout << "\nGenerate summary? (Y/N): ";
        char response;
        cin >> response;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (toupper(response) == 'Y') {
            session.displaySummary(students);
        }
    }
    
    // View attendance summary
    void viewAttendanceSummary() {
        if (sessions.empty()) {
            cout << "No sessions available." << endl;
            return;
        }
        
        cout << "\n--- ATTENDANCE SUMMARY ---\n";
        
        for (auto& session : sessions) {
            session.displaySummary(students);
        }
    }
    
    // ========== FILE OPERATIONS ==========
    
    // Save all students to file
    void saveStudents() {
        ofstream file(studentsFile);
        if (!file.is_open()) {
            cout << "Error: Could not save students to file!" << endl;
            return;
        }
        
        for (const auto& student : students) {
            file << student.toString() << endl;
        }
        
        file.close();
        cout << "Students saved to " << studentsFile << endl;
    }
    
    // Load students from file
    void loadStudents() {
        ifstream file(studentsFile);
        if (!file.is_open()) {
            cout << "No existing student data found. Starting fresh." << endl;
            return;
        }
        
        students.clear();
        string line;
        
        while (getline(file, line)) {
            if (!line.empty()) {
                Student student = Student::fromString(line);
                students.push_back(student);
            }
        }
        
        file.close();
        cout << "Loaded " << students.size() << " students from " << studentsFile << endl;
    }
    
    // Save all sessions
    void saveAllSessions() {
        for (auto& session : sessions) {
            session.saveToFile();
        }
        cout << "All sessions saved." << endl;
    }
    
    // Load a specific session from file
    void loadSessionFromFile() {
        cout << "\n--- LOAD SESSION FROM FILE ---\n";
        cout << "Enter session filename (e.g., session_EEE227_2026_02_10.txt): ";
        
        string filename;
        getline(cin, filename);
        
        AttendanceSession newSession;
        if (newSession.loadFromFile(filename, students)) {
            sessions.push_back(newSession);
            cout << "Session loaded successfully!" << endl;
        }
    }
    
    // ========== DEMO MODE ==========
    
    // Add demo data for testing
    void addDemoData() {
        cout << "\nAdding demo data...\n";
        
        // Add demo students
        students.push_back(Student("EE2001", "Kwame Mensah"));
        students.push_back(Student("EE2002", "Ama Boateng"));
        students.push_back(Student("EE2003", "Kojo Asare"));
        students.push_back(Student("EE2004", "Esi Ampofo"));
        students.push_back(Student("EE2005", "Yaw Ofori"));
        
        // Add demo session
        AttendanceSession demoSession("EEE227", "2026-02-10", "09:00", 2);
        demoSession.initializeRecords(students);
        demoSession.updateRecord("EE2001", 'P');
        demoSession.updateRecord("EE2002", 'L');
        demoSession.updateRecord("EE2003", 'P');
        demoSession.updateRecord("EE2004", 'A');
        demoSession.updateRecord("EE2005", 'P');
        
        sessions.push_back(demoSession);
        
        // Save data
        saveStudents();
        demoSession.saveToFile();
        
        cout << "Demo data added successfully!" << endl;
    }
    
    // ========== MAIN MENU ==========
    
    void displayMenu() {
        cout << "\n==========================================" << endl;
        cout << "    DIGITAL ATTENDANCE SYSTEM" << endl;
        cout << "==========================================" << endl;
        cout << "1. Student Management" << endl;
        cout << "2. Attendance Session Management" << endl;
        cout << "3. Mark Attendance" << endl;
        cout << "4. Reports and Summary" << endl;
        cout << "5. File Operations" << endl;
        cout << "6. Add Demo Data (for testing)" << endl;
        cout << "0. Exit" << endl;
        cout << "==========================================" << endl;
        cout << "Registered Students: " << students.size() << endl;
        cout << "Active Sessions: " << sessions.size() << endl;
        cout << "==========================================" << endl;
    }
    
    void studentManagementMenu() {
        int choice;
        do {
            cout << "\n--- STUDENT MANAGEMENT ---\n";
            cout << "1. Register New Student\n";
            cout << "2. View All Students\n";
            cout << "3. Search Student by Index\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: registerStudent(); break;
                case 2: viewAllStudents(); break;
                case 3: searchStudent(); break;
                case 0: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
    
    void sessionManagementMenu() {
        int choice;
        do {
            cout << "\n--- SESSION MANAGEMENT ---\n";
            cout << "1. Create New Lecture Session\n";
            cout << "2. View All Sessions\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: createSession(); break;
                case 2: viewAllSessions(); break;
                case 0: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
    
    void reportsMenu() {
        int choice;
        do {
            cout << "\n--- REPORTS AND SUMMARY ---\n";
            cout << "1. View Attendance Report for Session\n";
            cout << "2. View Attendance Summary\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: viewAttendanceReport(); break;
                case 2: viewAttendanceSummary(); break;
                case 0: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
    
    void fileOperationsMenu() {
        int choice;
        do {
            cout << "\n--- FILE OPERATIONS ---\n";
            cout << "1. Save All Students to File\n";
            cout << "2. Save All Sessions to Files\n";
            cout << "3. Load Session from File\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: saveStudents(); break;
                case 2: saveAllSessions(); break;
                case 3: loadSessionFromFile(); break;
                case 0: cout << "Returning to main menu...\n"; break;
                default: cout << "Invalid choice!\n";
            }
        } while (choice != 0);
    }
    
    // Main program loop
    void run() {
        int choice;
        
        do {
            displayMenu();
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: studentManagementMenu(); break;
                case 2: sessionManagementMenu(); break;
                case 3: markAttendance(); break;
                case 4: reportsMenu(); break;
                case 5: fileOperationsMenu(); break;
                case 6: addDemoData(); break;
                case 0: 
                    cout << "\nSaving data before exit...\n";
                    saveStudents();
                    saveAllSessions();
                    cout << "Goodbye!\n";
                    break;
                default: 
                    cout << "Invalid choice! Please try again.\n";
            }
            
        } while (choice != 0);
    }
};

// ==============================
// 5. MAIN FUNCTION
// ==============================
int main() {
    cout << "==========================================" << endl;
    cout << "  DIGITAL ATTENDANCE SYSTEM - EEE227" << endl;
    cout << "      Midterm Capstone Project" << endl;
    cout << "==========================================" << endl;
    cout << "Programme: HND Electrical Engineering (L200)\n" << endl;
    
    AttendanceSystem system;
    system.run();
    
    return 0;
}