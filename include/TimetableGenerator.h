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
    
private:
    DatabaseManager& database;
    bool generated;
    std::vector<std::string> schedule;
    std::string startDate;

    void cleanup();
};

#endif // TIMETABLEGENERATOR_H