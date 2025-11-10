#include "../include/DatabaseManager.h"
#include <iostream>

using namespace std;

/**
 * MOCK DATABASE MANAGER - FOR TESTING WITHOUT ORACLE
 * This version uses hardcoded data instead of connecting to Oracle
 * Perfect for testing the algorithm without database setup
 */

DatabaseManager::DatabaseManager()
    : hEnv(nullptr), hDbc(nullptr), hStmt(nullptr), connected(false) {
    cout << "Using MOCK Database Manager (No Oracle needed)" << endl;
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::checkSQLReturn(int ret, const string& operation) {
    (void)ret;         // Suppress unused parameter warning
    (void)operation;   // Suppress unused parameter warning
    return true;
}

string DatabaseManager::getSQLError(short handleType, void* handle) {
    (void)handleType;  // Suppress unused parameter warning
    (void)handle;      // Suppress unused parameter warning
    return "";
}

bool DatabaseManager::connect(const string& dsn, const string& username, 
                           const string& password) {
    (void)dsn;         // Suppress unused parameter warning
    (void)username;    // Suppress unused parameter warning
    (void)password;    // Suppress unused parameter warning
    cout << "\n=== MOCK CONNECTION (Testing Mode) ===" << endl;
    cout << "Simulating connection to Oracle database..." << endl;
    cout << " Mock connection successful!" << endl;
    cout << "Using hardcoded test data\n" << endl;
    
    connected = true;
    return true;
}

void DatabaseManager::disconnect() {
    if (connected) {
        cout << "Disconnected from mock database." << endl;
        connected = false;
    }
}

LinkedList<Course> DatabaseManager::fetchCourses() {
    LinkedList<Course> courses;
    
    if (!connected) {
        cerr << "Not connected to database." << endl;
        return courses;
    }
    
    // Hardcoded mock data
    courses.insertAtEnd(Course("ML101", "Machine Learning", "Computer Science"));
    courses.insertAtEnd(Course("OOP201", "Object Oriented Programming", "Computer Science"));
    courses.insertAtEnd(Course("DB301", "Database Systems", "Information Technology"));
    courses.insertAtEnd(Course("AI401", "Artificial Intelligence", "Computer Science"));
    courses.insertAtEnd(Course("DS501", "Data Structures", "Computer Science"));
    courses.insertAtEnd(Course("CN601", "Computer Networks", "Information Technology"));
    courses.insertAtEnd(Course("SE701", "Software Engineering", "Computer Science"));
    courses.insertAtEnd(Course("WD801", "Web Development", "Information Technology"));
    courses.insertAtEnd(Course("CC901", "Cloud Computing", "Computer Science"));
    courses.insertAtEnd(Course("CY102", "Cybersecurity", "Information Technology"));
    
    cout << "Fetched " << courses.getSize() << " courses from mock database." << endl;
    return courses;
}

LinkedList<Student> DatabaseManager::fetchStudents() {
    LinkedList<Student> students;
    
    if (!connected) {
        cerr << "Not connected to database." << endl;
        return students;
    }
    
    // Hardcoded mock data
    students.insertAtEnd(Student("CT-24001", "Ayan Anwer", 1));
    students.insertAtEnd(Student("CT-24002", "Maaz Ali", 1));
    students.insertAtEnd(Student("CT-24003", "Saad Rashid", 2));
    students.insertAtEnd(Student("CT-24004", "David Willey", 2));
    students.insertAtEnd(Student("CT-24005", "John Cena", 1));
    students.insertAtEnd(Student("CT-24006", "Undertaker", 2));
    students.insertAtEnd(Student("CT-24007", "Final Boss", 1));
    students.insertAtEnd(Student("CT-24008", "Roman Reigns", 2));
    students.insertAtEnd(Student("CT-24009", "Imran Khan", 1));
    students.insertAtEnd(Student("CT-24010", "Shareef Bhai", 2));
    
    // Generate students CT-24011 to CT-24320 (310 additional students)
    for (int i = 11; i <= 320; i++) {
        string rollNo = "CT-24" + string(3 - to_string(i).length(), '0') + to_string(i);
        string name = "Student" + to_string(i);
        int semester = ((i % 8) + 1); // Distribute across semesters 1-8
        students.insertAtEnd(Student(rollNo, name, semester));
    }
    
    cout << "Fetched " << students.getSize() << " students from mock database." << endl;
    return students;
}

LinkedList<Enrollment> DatabaseManager::fetchEnrollments() {
    LinkedList<Enrollment> enrollments;
    
    if (!connected) {
        cerr << "Not connected to database." << endl;
        return enrollments;
    }
    
    // Hardcoded mock data - creating realistic conflicts
    // Student CT-24001
    enrollments.insertAtEnd(Enrollment("CT-24001", "ML101"));
    enrollments.insertAtEnd(Enrollment("CT-24001", "OOP201"));
    enrollments.insertAtEnd(Enrollment("CT-24001", "AI401"));
    enrollments.insertAtEnd(Enrollment("CT-24001", "DS501"));
    enrollments.insertAtEnd(Enrollment("CT-24001", "SE701"));
    
    // Student CT-24002 to CT-24010 enrollments...
    // [Previous enrollments remain the same]
    
    // Generate enrollments for students CT-24011 to CT-24320
    vector<string> allCourses = {"ML101", "OOP201", "DB301", "AI401", "DS501", 
                               "CN601", "SE701", "WD801", "CC901", "CY102"};
    
    for (int i = 11; i <= 320; i++) {
        string rollNo = "CT-24" + string(3 - to_string(i).length(), '0') + to_string(i);
        
        // Each student enrolls in 4-5 courses (creates realistic conflicts)
        int numCourses = 4 + (i % 2); // Alternates between 4 and 5 courses
        int startCourse = (i % 6); // Rotate through different starting points
        
        for (int j = 0; j < numCourses; j++) {
            int courseIndex = (startCourse + j) % allCourses.size();
            enrollments.insertAtEnd(Enrollment(rollNo, allCourses[courseIndex]));
        }
    }
    
    cout << "Fetched " << enrollments.getSize() << " enrollments from mock database." << endl;
    return enrollments;
}

vector<string> DatabaseManager::getCourseCodes() {
    vector<string> codes;
    LinkedList<Course> courses = fetchCourses();
    Node<Course>* current = courses.getHead();
    
    while (current != nullptr) {
        codes.push_back(current->data.courseId);
        current = current->next;
    }
    return codes;
}

vector<string> DatabaseManager::getCourseNames() {
    vector<string> names;
    LinkedList<Course> courses = fetchCourses();
    Node<Course>* current = courses.getHead();
    
    while (current != nullptr) {
        names.push_back(current->data.courseName);
        current = current->next;
    }
    return names;
}

vector<pair<string, string>> DatabaseManager::getStudentList() {
    vector<pair<string, string>> studentInfo;
    LinkedList<Student> students = fetchStudents();
    Node<Student>* current = students.getHead();
    
    while (current != nullptr) {
        studentInfo.push_back(make_pair(current->data.rollNo, current->data.name));
        current = current->next;
    }
    return studentInfo;
}

vector<pair<string, vector<string>>> DatabaseManager::getStudentEnrollments() {
    vector<pair<string, vector<string>>> enrollmentMap;
    LinkedList<Student> students = fetchStudents();
    LinkedList<Enrollment> allEnrollments = fetchEnrollments();
    
    Node<Student>* currentStudent = students.getHead();
    while (currentStudent != nullptr) {
        vector<string> studentCourses;
        Node<Enrollment>* currentEnroll = allEnrollments.getHead();
        
        while (currentEnroll != nullptr) {
            if (currentEnroll->data.rollNo == currentStudent->data.rollNo) {
                studentCourses.push_back(currentEnroll->data.courseId);
            }
            currentEnroll = currentEnroll->next;
        }
        
        enrollmentMap.push_back(make_pair(currentStudent->data.rollNo, studentCourses));
        currentStudent = currentStudent->next;
    }
    return enrollmentMap;
}

map<string, int> DatabaseManager::getCourseSizes() {
    map<string, int> courseSizes;
    LinkedList<Enrollment> enrollments = fetchEnrollments();
    Node<Enrollment>* current = enrollments.getHead();
    
    while (current != nullptr) {
        courseSizes[current->data.courseId]++;
        current = current->next;
    }
    return courseSizes;
}