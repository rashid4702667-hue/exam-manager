#ifndef TIMETABLEGENERATOR_H
#define TIMETABLEGENERATOR_H

#include <vector>
#include <string>
#include "Models.h"
#include "LinkedList.h"
#include "DatabaseManager.h"

class TimetableGenerator {
public:
    TimetableGenerator(DatabaseManager& db);
    ~TimetableGenerator();

    void setStartDate(const std::string& date);
    bool generate();
    bool exportToCSV(const std::string& filename);
    bool exportToText(const std::string& filename);
    bool exportToPDF(const std::string& filename);
    
    // Getters for the generated timetable
    std::vector<std::string> getSchedule() const;
    bool isGenerated() const;
    
    // Methods for changing test dates
    std::vector<std::pair<std::string, std::string>> getCoursesWithDates() const;
    bool updateCourseDate(const std::string& courseId, const std::string& newDate);
    
private:
    DatabaseManager& database;
    bool generated;
    std::vector<std::string> schedule;
    std::string startDate;

    void cleanup();
    std::string calculateExamDate(const std::string& startDate, int dayOffset);
};

#endif // TIMETABLEGENERATOR_H