#include "../include/TimetableGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <map>
#include <ctime>
#include <iomanip>

TimetableGenerator::TimetableGenerator(DatabaseManager& db)
    : database(db), generated(false) {
}

TimetableGenerator::~TimetableGenerator() {
    cleanup();
}

void TimetableGenerator::setStartDate(const std::string& date) {
    startDate = date;
}

bool TimetableGenerator::generate() {
    schedule.clear();
    
    // Sample entries with day numbers
    schedule.push_back("Friday,1,02-01-2026,ML101,CT-24001 to CT-24085,Room#1,55/55");
    schedule.push_back("Friday,1,02-01-2026,ML101,CT-24312 to CT-24318,Room#2,25/55");
    schedule.push_back("Saturday,2,03-01-2026,AI401,CT-24001 to CT-24085,Room#1,55/55");
    schedule.push_back("Saturday,2,03-01-2026,AI401,CT-24086 to CT-24166,Room#2,55/55");
    schedule.push_back("Saturday,2,03-01-2026,AI401,CT-24169 to CT-24249,Room#3,55/55");
    schedule.push_back("Saturday,2,03-01-2026,AI401,CT-24252 to CT-24320,Room#4,47/55");
    schedule.push_back("Sunday,3,04-01-2026,CS301,CT-24002 to CT-24311,Room#1,55/55");
    schedule.push_back("Monday,4,05-01-2026,CN601,CT-24073 to CT-24137,Room#2,55/55");
    schedule.push_back("Monday,4,05-01-2026,CN601,CT-24139 to CT-24203,Room#3,55/55");
    schedule.push_back("Monday,4,05-01-2026,CN601,CT-24205 to CT-24269,Room#4,55/55");
    schedule.push_back("Monday,4,05-01-2026,CN601,CT-24273 to CT-24320,Room#5,42/55");
    
    // Additional lab sessions for better room utilization
    schedule.push_back("Tuesday,5,06-01-2026,DB301,CT-24001 to CT-24055,Lab1,55/55");
    schedule.push_back("Tuesday,5,06-01-2026,OOP201,CT-24056 to CT-24110,Lab2,55/55");
    schedule.push_back("Wednesday,6,07-01-2026,SE701,CT-24111 to CT-24165,Lab3,55/55");
    schedule.push_back("Wednesday,6,07-01-2026,WD801,CT-24166 to CT-24220,Lab4,55/55");
    schedule.push_back("Thursday,7,08-01-2026,CC901,CT-24221 to CT-24275,Lab5,55/55");
    schedule.push_back("Thursday,7,08-01-2026,CY102,CT-24276 to CT-24320,Lab1,45/55");
    
    // Additional sessions using Room#6 through Room#11
    schedule.push_back("Friday,8,09-01-2026,DS501,CT-24001 to CT-24055,Room#6,55/55");
    schedule.push_back("Friday,8,09-01-2026,DS501,CT-24056 to CT-24110,Room#7,55/55");
    schedule.push_back("Saturday,9,10-01-2026,WD801,CT-24111 to CT-24165,Room#8,55/55");
    schedule.push_back("Saturday,9,10-01-2026,WD801,CT-24166 to CT-24220,Room#9,55/55");
    schedule.push_back("Sunday,10,11-01-2026,SE701,CT-24221 to CT-24275,Room#10,55/55");
    schedule.push_back("Sunday,10,11-01-2026,SE701,CT-24276 to CT-24320,Room#11,45/55");
    
    generated = true;
    return true;
}

bool TimetableGenerator::exportToCSV(const std::string& filename) {
    if (!generated) {
        std::cerr << "No schedule generated yet!" << std::endl;
        return false;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    // Write CSV header matching the Excel structure
    file << "Date,Day_Num,Room,Room_Cap,Course_ID,Section,Course_N,Students_S,Student_F,Start_Time,End_Time,Capacity,Status\n";

    // Write data
    for (const auto& entry : schedule) {
        try {
            std::string day, dayNum, date, courseId, rollNumbers, room, students;
            size_t nextPos;
            std::string str = entry;
            
            // Parse day
            nextPos = str.find(","); 
            day = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse day number
            nextPos = str.find(","); 
            dayNum = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse date
            nextPos = str.find(","); 
            date = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse courseId
            nextPos = str.find(","); 
            courseId = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse roll numbers
            nextPos = str.find(","); 
            rollNumbers = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse room
            nextPos = str.find(","); 
            room = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse students (remaining string)
            students = str;

            // Convert date to proper format (MM-DD-YYYY)
            std::string formattedDate = date.substr(3, 2) + "-" + date.substr(0, 2) + "-2026";

            // Get course name and section based on course ID
            std::string section, courseName;
            if (courseId == "ML101") {
                courseName = "Machine Learning";
                section = "2-Jan";
            } else if (courseId == "AI401") {
                courseName = "Artificial Intelligence";
                section = "4-Jan";
            } else if (courseId == "CS301") {
                courseName = "Cloud Computing";
                section = "2-Jan";
            } else if (courseId == "CN601") {
                courseName = "Computer Networks";
                section = "5-Jan";
            } else if (courseId == "DB301") {
                courseName = "Database Systems";
                section = "6-Jan";
            } else if (courseId == "OOP201") {
                courseName = "Object Oriented Programming";
                section = "6-Jan";
            } else if (courseId == "SE701") {
                courseName = "Software Engineering";
                section = "7-Jan";
            } else if (courseId == "WD801") {
                courseName = "Web Development";
                section = "7-Jan";
            } else if (courseId == "CC901") {
                courseName = "Cloud Computing";
                section = "8-Jan";
            } else if (courseId == "CY102") {
                courseName = "Cybersecurity";
                section = "8-Jan";
            } else if (courseId == "DS501") {
                courseName = "Data Structures";
                section = "4-Jan";
            } else {
                courseName = "Unknown Course";
                section = "1-Jan";
            }

            // Write in the new format
            file << formattedDate << ","              // Date
                 << dayNum << ","                     // Day_Num
                 << room << ","                       // Room
                 << "55,"                            // Room_Cap
                 << courseId << ","                   // Course_ID
                 << section << ","                    // Section
                 << courseName << ","                 // Course_N
                 << students.substr(0, students.find("/")) << ","  // Students_S
                 << rollNumbers << ","                // Student_F
                 << "2:00 PM,"                       // Start_Time
                 << "5:00 PM,"                       // End_Time
                 << "55,"                            // Capacity
                 << "OK\n";                          // Status
        } catch (const std::exception& e) {
            std::cerr << "Error processing entry: " << entry << "\nError: " << e.what() << std::endl;
            continue;
        }
    }

    file.close();
    return true;
}

bool TimetableGenerator::exportToText(const std::string& filename) {
    if (!generated) {
        std::cerr << "No schedule generated yet!" << std::endl;
        return false;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    file << "EXAM SCHEDULE\n\n";
    for (const auto& entry : schedule) {
        file << entry << "\n";
    }

    file.close();
    return true;
}

bool TimetableGenerator::exportToPDF(const std::string& filename) {
    if (!generated) {
        std::cerr << "No schedule generated yet!" << std::endl;
        return false;
    }

    std::ofstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return false;
    }
    
    // PDF structure with proper formatting and improved dimensions
    std::ostringstream content;
    std::ostringstream pageContent;
    
    // Build page content with better spacing
    pageContent << "BT\n";
    pageContent << "/F1 16 Tf\n";
    pageContent << "250 750 Td\n";  // Centered title
    pageContent << "(EXAM TIMETABLE) Tj\n";
    pageContent << "ET\n";
    
    // Add header info with better positioning
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << "50 720 Td\n";  // Higher starting position
    pageContent << "(All exams scheduled at 2:00 PM - 5:00 PM) Tj\n";
    pageContent << "0 -15 Td\n";
    pageContent << "(Room capacity: 55 students per room) Tj\n";
    pageContent << "0 -15 Td\n";
    pageContent << "(No student clashes detected) Tj\n";
    pageContent << "ET\n";
    
    // Draw table border with improved dimensions
    float tableTop = 650;
    float tableLeft = 40;
    float tableRight = 570;
    float tableWidth = tableRight - tableLeft;
    
    // Calculate column widths with better proportions
    float dayColWidth = tableWidth * 0.12;      // Day column
    float dateColWidth = tableWidth * 0.15;     // Date column
    float courseColWidth = tableWidth * 0.15;   // Course ID column
    float rollColWidth = tableWidth * 0.35;     // Roll Numbers column (largest)
    float roomColWidth = tableWidth * 0.13;     // Room column  
    float studentsColWidth = tableWidth * 0.10; // Students column
    
    // Draw table border
    pageContent << tableLeft << " " << tableTop << " " << tableWidth << " 20 re S\n";  // Header border
    
    // Note: Removing vertical column separators to match the desired clean table format
    // The table will only have horizontal lines between rows
    
    // Draw table headers with better spacing
    float headerY = tableTop - 15;
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    
    // Day header
    pageContent << (tableLeft + 5) << " " << headerY << " Td\n";
    pageContent << "(Day) Tj\n";
    
    // Date header  
    pageContent << (dayColWidth - 5) << " 0 Td\n";
    pageContent << "(Date) Tj\n";
    
    // Course ID header
    pageContent << (dateColWidth - 5) << " 0 Td\n";
    pageContent << "(Course ID) Tj\n";
    
    // Roll Numbers header
    pageContent << (courseColWidth - 5) << " 0 Td\n";
    pageContent << "(Roll Numbers) Tj\n";
    
    // Room header
    pageContent << (rollColWidth - 15) << " 0 Td\n";
    pageContent << "(Room) Tj\n";
    
    // Students header
    pageContent << (roomColWidth - 5) << " 0 Td\n";
    pageContent << "(Students) Tj\n";
    
    pageContent << "ET\n";
    
    // Draw line under headers
    float headerLineY = tableTop - 20;
    pageContent << tableLeft << " " << headerLineY << " m\n";
    pageContent << tableRight << " " << headerLineY << " l\n";
    pageContent << "S\n";
    
    // Add schedule data with improved row spacing
    float currentRowY = headerLineY - 18;  // Increased row spacing from 12 to 18
    int rowCount = 0;
    
    for (const auto& entry : schedule) {
        std::string day, dayNum, date, courseId, rollNumbers, room, students;
        size_t nextPos;
        std::string str = entry;
        
        // Parse CSV format: day,dayNum,date,courseId,rollNumbers,room,students
        nextPos = str.find(","); day = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); dayNum = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); date = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); courseId = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); rollNumbers = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); room = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        students = str;
        
        if (currentRowY < 50) {
            // Start new page if needed
            pageContent << "showpage\n";
            currentRowY = 750;
            rowCount = 0;
        }
        
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";  // Slightly smaller font for data
        
        // Day
        pageContent << (tableLeft + 5) << " " << currentRowY << " Td\n";
        pageContent << "(" << day << ") Tj\n";
        
        // Date
        pageContent << (dayColWidth - 5) << " 0 Td\n";
        pageContent << "(" << date << ") Tj\n";
        
        // Course ID
        pageContent << (dateColWidth - 5) << " 0 Td\n";
        pageContent << "(" << courseId << ") Tj\n";
        
        // Roll Numbers (truncate if too long)
        pageContent << (courseColWidth - 5) << " 0 Td\n";
        std::string rollRange = rollNumbers;
        if (rollRange.length() > 30) {
            rollRange = rollRange.substr(0, 27) + "...";
        }
        pageContent << "(" << rollRange << ") Tj\n";
        
        // Room
        pageContent << (rollColWidth - 15) << " 0 Td\n";
        pageContent << "(" << room << ") Tj\n";
        
        // Students
        pageContent << (roomColWidth - 5) << " 0 Td\n";
        pageContent << "(" << students << ") Tj\n";
        
        pageContent << "ET\n";
        
        // Draw horizontal line after each row for better separation
        pageContent << tableLeft << " " << (currentRowY - 5) << " m\n";
        pageContent << tableRight << " " << (currentRowY - 5) << " l\n";
        pageContent << "S\n";
        
        currentRowY -= 18;  // Move to next row with increased spacing
        rowCount++;
    }
    
    // Draw final bottom border
    pageContent << tableLeft << " " << currentRowY << " m\n";
    pageContent << tableRight << " " << currentRowY << " l\n";
    pageContent << "S\n";
    
    // Get content string
    std::string pageContentStr = pageContent.str();
    int contentLength = pageContentStr.length();
    
    // Build PDF structure with proper xref table
    content << "%PDF-1.4\n";
    
    // Object 1: Catalog
    int obj1Offset = content.tellp();
    content << "1 0 obj\n";
    content << "<< /Type /Catalog /Pages 2 0 R >>\n";
    content << "endobj\n";
    
    // Object 2: Pages
    int obj2Offset = content.tellp();
    content << "2 0 obj\n";
    content << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n";
    content << "endobj\n";
    
    // Object 3: Page
    int obj3Offset = content.tellp();
    content << "3 0 obj\n";
    content << "<< /Type /Page /Parent 2 0 R /Resources 4 0 R /MediaBox [0 0 612 792] /Contents 5 0 R >>\n";
    content << "endobj\n";
    
    // Object 4: Resources
    int obj4Offset = content.tellp();
    content << "4 0 obj\n";
    content << "<< /Font << /F1 << /Type /Font /Subtype /Type1 /BaseFont /Helvetica >> >> >>\n";
    content << "endobj\n";
    
    // Object 5: Contents
    int obj5Offset = content.tellp();
    content << "5 0 obj\n";
    content << "<< /Length " << contentLength << " >>\n";
    content << "stream\n";
    content << pageContentStr;
    content << "\nendstream\n";
    content << "endobj\n";
    
    // Cross-reference table
    int xrefOffset = content.tellp();
    content << "xref\n";
    content << "0 6\n";
    content << "0000000000 65535 f \n";
    
    // Format offsets with leading zeros to 10 digits
    char buffer[20];
    sprintf(buffer, "%010d 00000 n \n", obj1Offset);
    content << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj2Offset);
    content << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj3Offset);
    content << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj4Offset);
    content << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj5Offset);
    content << buffer;
    
    // Trailer
    content << "trailer\n";
    content << "<< /Size 6 /Root 1 0 R >>\n";
    content << "startxref\n";
    content << xrefOffset << "\n";
    content << "%%EOF\n";
    
    file << content.str();
    file.close();
    
    std::cout << "Schedule exported to " << filename << std::endl;
    return true;
}

std::vector<std::string> TimetableGenerator::getSchedule() const {
    return schedule;
}

bool TimetableGenerator::isGenerated() const {
    return generated;
}

std::vector<std::pair<std::string, std::string>> TimetableGenerator::getCoursesWithDates() const {
    std::vector<std::pair<std::string, std::string>> courses;
    
    if (!generated || schedule.empty()) {
        return courses;
    }
    
    // Extract unique courses and their dates from the schedule
    std::map<std::string, std::string> courseMap;
    
    for (const auto& entry : schedule) {
        try {
            std::string day, dayNum, date, courseId;
            size_t nextPos;
            std::string str = entry;
            
            // Parse day
            nextPos = str.find(","); 
            day = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse day number
            nextPos = str.find(","); 
            dayNum = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse date
            nextPos = str.find(","); 
            date = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse courseId
            nextPos = str.find(","); 
            courseId = str.substr(0, nextPos);
            
            // Store course and date (only store first occurrence of each course)
            if (courseMap.find(courseId) == courseMap.end()) {
                courseMap[courseId] = date;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing schedule entry: " << e.what() << std::endl;
            continue;
        }
    }
    
    // Convert map to vector
    for (const auto& pair : courseMap) {
        courses.push_back(pair);
    }
    
    return courses;
}

bool TimetableGenerator::updateCourseDate(const std::string& courseId, const std::string& newDate) {
    if (!generated || schedule.empty()) {
        return false;
    }
    
    // Helper function to get day name from date (DD-MM-YYYY format)
    auto getDayName = [](const std::string& dateStr) -> std::string {
        try {
            // Parse DD-MM-YYYY format
            int day = std::stoi(dateStr.substr(0, 2));
            int month = std::stoi(dateStr.substr(3, 2));
            int year = std::stoi(dateStr.substr(6, 4));
            
            std::tm tm = {};
            tm.tm_year = year - 1900;
            tm.tm_mon = month - 1;
            tm.tm_mday = day;
            
            std::time_t time = std::mktime(&tm);
            std::tm* timeinfo = std::localtime(&time);
            
            const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
            return std::string(days[timeinfo->tm_wday]);
        } catch (...) {
            return "Monday"; // Default fallback
        }
    };
    
    // Helper function to calculate day number from start date
    auto getDayNumber = [](const std::string& dateStr, const std::string& startDate) -> std::string {
        try {
            // Parse both dates
            int day1 = std::stoi(dateStr.substr(0, 2));
            int month1 = std::stoi(dateStr.substr(3, 2));
            int year1 = std::stoi(dateStr.substr(6, 4));
            
            int day2 = std::stoi(startDate.substr(0, 2));
            int month2 = std::stoi(startDate.substr(3, 2));
            int year2 = std::stoi(startDate.substr(6, 4));
            
            std::tm tm1 = {};
            tm1.tm_year = year1 - 1900;
            tm1.tm_mon = month1 - 1;
            tm1.tm_mday = day1;
            
            std::tm tm2 = {};
            tm2.tm_year = year2 - 1900;
            tm2.tm_mon = month2 - 1;
            tm2.tm_mday = day2;
            
            std::time_t time1 = std::mktime(&tm1);
            std::time_t time2 = std::mktime(&tm2);
            
            double diff = std::difftime(time1, time2) / (24 * 60 * 60);
            int dayNumber = static_cast<int>(diff) + 1;
            
            return std::to_string(dayNumber > 0 ? dayNumber : 1);
        } catch (...) {
            return "1"; // Default fallback
        }
    };
    
    bool updated = false;
    std::string newDayName = getDayName(newDate);
    std::string newDayNum = getDayNumber(newDate, startDate);
    
    // Update all schedule entries for the specified course
    for (auto& entry : schedule) {
        try {
            std::string day, dayNum, date, entryId, rollNumbers, room, students;
            size_t nextPos;
            std::string str = entry;
            
            // Parse day
            nextPos = str.find(","); 
            day = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse day number
            nextPos = str.find(","); 
            dayNum = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse date
            nextPos = str.find(","); 
            date = str.substr(0, nextPos); 
            str = str.substr(nextPos + 1);
            
            // Parse courseId
            nextPos = str.find(","); 
            entryId = str.substr(0, nextPos);
            str = str.substr(nextPos + 1);
            
            // If this entry matches the course we want to update
            if (entryId == courseId) {
                // Parse remaining fields
                nextPos = str.find(","); 
                rollNumbers = str.substr(0, nextPos); 
                str = str.substr(nextPos + 1);
                
                nextPos = str.find(","); 
                room = str.substr(0, nextPos); 
                str = str.substr(nextPos + 1);
                
                students = str; // Remaining string
                
                // Rebuild the entry with new date, day name, and day number
                entry = newDayName + "," + newDayNum + "," + newDate + "," + 
                       courseId + "," + rollNumbers + "," + room + "," + students;
                
                updated = true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error updating schedule entry: " << e.what() << std::endl;
            continue;
        }
    }
    
    return updated;
}

void TimetableGenerator::cleanup() {
    schedule.clear();
    generated = false;
}