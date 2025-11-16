#include <iostream>
#include "include/DatabaseManager.h"
#include "include/TimetableGenerator.h"

int main() {
    DatabaseManager db;
    
    // Import CSV data
    std::string csvFile = "meow.csv";
    if (db.importFromCSV(csvFile)) {
        std::cout << "CSV import successful!\n" << std::endl;
        
        // Create timetable generator and generate schedule
        TimetableGenerator generator(db);
        generator.setStartDate("16-11-2025");
        
        if (generator.generate()) {
            std::cout << "Timetable generation successful!\n" << std::endl;
            
            // Export to CSV to see results
            generator.exportToCSV("build/test_generated_schedule.csv");
            
            // Show some statistics
            auto schedule = generator.getSchedule();
            std::cout << "Generated " << schedule.size() << " exam sessions" << std::endl;
            
            // Show first few entries
            std::cout << "\nFirst 5 schedule entries:" << std::endl;
            for (size_t i = 0; i < std::min((size_t)5, schedule.size()); i++) {
                std::cout << schedule[i] << std::endl;
            }
        } else {
            std::cout << "Timetable generation failed!" << std::endl;
        }
    } else {
        std::cout << "CSV import failed!" << std::endl;
    }
    
    return 0;
}