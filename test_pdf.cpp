#include "include/TimetableGenerator.h"
#include "include/DatabaseManager.h"
#include <iostream>

int main() {
    try {
        // Create database manager and timetable generator
        DatabaseManager dbManager;
        TimetableGenerator generator(dbManager);
        
        // Connect to mock database
        if (!dbManager.connect("mock", "test", "test")) {
            std::cerr << "Failed to connect to database" << std::endl;
            return 1;
        }
        
        // Generate timetable
        generator.setStartDate("02-01-2026");
        if (!generator.generate()) {
            std::cerr << "Failed to generate timetable" << std::endl;
            return 1;
        }
        
        std::cout << "Generated schedule with " << generator.getSchedule().size() << " entries" << std::endl;
        
        // Export to PDF using our new manual PDF generator
        std::string pdfFile = "test_exam_schedule.pdf";
        if (generator.exportToPDF(pdfFile)) {
            std::cout << "PDF exported successfully to: " << pdfFile << std::endl;
        } else {
            std::cerr << "Failed to export PDF" << std::endl;
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}