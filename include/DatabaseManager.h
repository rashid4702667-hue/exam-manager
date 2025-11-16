#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "LinkedList.h"
#include "Models.h"

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect(const std::string& dsn, const std::string& username, const std::string& password);
    bool importFromCSV(const std::string& filename);
    void disconnect();

    // Core data fetch methods
    LinkedList<Course> fetchCourses();
    LinkedList<Student> fetchStudents();
    LinkedList<Enrollment> fetchEnrollments();

    // GUI helper methods
    std::vector<std::string> getCourseCodes();
    std::vector<std::string> getCourseNames();
    std::vector<std::pair<std::string, std::string>> getStudentList();
    std::vector<std::pair<std::string, std::vector<std::string>>> getStudentEnrollments();
    std::map<std::string, int> getCourseSizes();
    
    // Check if data is loaded from CSV
    bool isDataFromCSV() const { return csvDataLoaded; }

private:
    bool connected;
    bool csvDataLoaded;
    
    // CSV data storage
    LinkedList<Course> csvCourses;
    LinkedList<Student> csvStudents;
    LinkedList<Enrollment> csvEnrollments;
    void* hEnv;   // Using void* to avoid including sql.h
    void* hDbc;   // Using void* to avoid including sql.h
    void* hStmt;  // Using void* to avoid including sql.h
    
    bool checkSQLReturn(int ret, const std::string& operation);
    std::string getSQLError(short handleType, void* handle);
};

#endif // DATABASEMANAGER_H