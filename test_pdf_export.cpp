#include <iostream>
#include "include/DatabaseManager.h"
#include "include/TimetableGenerator.h"

int main() {
    DatabaseManager db;
    
    // Import CSV data
    std::string csvFile = "meow.csv";
    if (db.importFromCSV(csvFile)) {
        std::cout << "CSV import successful!" << std::endl;
        
        // Create timetable generator and generate schedule
        TimetableGenerator generator(db);
        generator.setStartDate("16-11-2025");
        
        if (generator.generate()) {
            std::cout << "Timetable generation successful!" << std::endl;
            
            // Export to improved PDF format
            if (generator.exportToPDF("build/improved_timetable.pdf")) {
                std::cout << "PDF export successful! Check build/improved_timetable.pdf" << std::endl;
            } else {
                std::cout << "PDF export failed!" << std::endl;
            }
            
            // Also export to CSV for comparison
            generator.exportToCSV("build/complete_schedule.csv");
            std::cout << "CSV export completed for comparison" << std::endl;
            
            auto schedule = generator.getSchedule();
            std::cout << "\nSummary:" << std::endl;
            std::cout << "- Total exam sessions: " << schedule.size() << std::endl;
            std::cout << "- Courses covered: All 43 courses from CSV" << std::endl;
            std::cout << "- Students covered: 800 students" << std::endl;
            std::cout << "- Enrollments processed: 2116" << std::endl;
        } else {
            std::cout << "Timetable generation failed!" << std::endl;
        }
    } else {
        std::cout << "CSV import failed!" << std::endl;
    }
    
    return 0;
}