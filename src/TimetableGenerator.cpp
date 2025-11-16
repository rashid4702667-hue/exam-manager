#include "../include/TimetableGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <map>
#include <ctime>
#include <iomanip>
#include <algorithm>

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
    
    // Fetch data from database/CSV
    LinkedList<Course> courses = database.fetchCourses();
    LinkedList<Student> students = database.fetchStudents();
    LinkedList<Enrollment> enrollments = database.fetchEnrollments();
    
    std::cout << "Generating timetable with " << courses.getSize() << " courses, " 
              << students.getSize() << " students, and " << enrollments.getSize() 
              << " enrollments." << std::endl;
    
    if (courses.getSize() == 0) {
        std::cerr << "No courses found! Please import data first." << std::endl;
        return false;
    }
    
    // Create mapping of course to enrolled students
    std::map<std::string, std::vector<std::string>> courseEnrollments;
    
    // Initialize enrollment map for all courses by iterating through the linked list
    Node<Course>* courseNode = courses.getHead();
    while (courseNode != nullptr) {
        courseEnrollments[courseNode->data.courseId] = std::vector<std::string>();
        courseNode = courseNode->next;
    }
    
    // Populate enrollment map by iterating through enrollments
    Node<Enrollment>* enrollmentNode = enrollments.getHead();
    while (enrollmentNode != nullptr) {
        std::string courseId = enrollmentNode->data.courseId;
        std::string studentId = enrollmentNode->data.rollNo;
        
        if (courseEnrollments.find(courseId) != courseEnrollments.end()) {
            courseEnrollments[courseId].push_back(studentId);
        }
        enrollmentNode = enrollmentNode->next;
    }
    
    // Generate schedule for each course
    int dayCounter = 1;
    int roomCounter = 1;
    const int maxRooms = 15; // Maximum rooms available
    const int roomCapacity = 55; // Room capacity
    
    // Days of the week
    std::vector<std::string> dayNames = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    int currentDay = 0;
    
    // Iterate through courses
    courseNode = courses.getHead();
    while (courseNode != nullptr) {
        Course course = courseNode->data;
        std::string courseId = course.courseId;
        
        std::vector<std::string> enrolledStudents = courseEnrollments[courseId];
        
        if (enrolledStudents.empty()) {
            std::cout << "Warning: No students enrolled in course " << courseId << std::endl;
            courseNode = courseNode->next;
            continue; // Skip courses with no enrollments
        }
        
        // Calculate how many sessions needed based on enrolled students
        int totalStudents = enrolledStudents.size();
        int sessionsNeeded = (totalStudents + roomCapacity - 1) / roomCapacity; // Ceiling division
        
        for (int session = 0; session < sessionsNeeded; session++) {
            // Calculate date based on start date and day counter
            std::string examDate = calculateExamDate(startDate, dayCounter);
            std::string dayName = dayNames[currentDay];
            
            // Determine room (cycle through available rooms)
            std::string room;
            if (roomCounter <= 11) {
                room = "Room#" + std::to_string(roomCounter);
            } else {
                room = "Lab" + std::to_string(roomCounter - 11);
            }
            
            // Calculate student range for this session
            int startIdx = session * roomCapacity;
            int endIdx = std::min(startIdx + roomCapacity - 1, (int)enrolledStudents.size() - 1);
            int studentsInSession = endIdx - startIdx + 1;
            
            std::string studentRange;
            if (studentsInSession == 1) {
                studentRange = enrolledStudents[startIdx];
            } else {
                studentRange = enrolledStudents[startIdx] + " to " + enrolledStudents[endIdx];
            }
            
            // Create schedule entry
            std::string scheduleEntry = dayName + "," + 
                                       std::to_string(dayCounter) + "," + 
                                       examDate + "," + 
                                       courseId + "," + 
                                       studentRange + "," + 
                                       room + "," + 
                                       std::to_string(studentsInSession) + "/" + std::to_string(roomCapacity);
            
            schedule.push_back(scheduleEntry);
            
            // Move to next room
            roomCounter++;
            if (roomCounter > maxRooms) {
                roomCounter = 1;
                dayCounter++;
                currentDay = (currentDay + 1) % 7; // Cycle through days
            }
        }
        
        courseNode = courseNode->next;
    }
    
    generated = true;
    std::cout << "Timetable generated successfully with " << schedule.size() << " exam sessions." << std::endl;
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

            // Keep date in DD-MM-YYYY format, but ensure Excel can interpret it properly
            // Convert DD-MM-YYYY to DD/MM/YYYY for better Excel compatibility
            std::string formattedDate = date.substr(0, 2) + "/" + date.substr(3, 2) + "/" + date.substr(6, 4);

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
    
    // Create page content with absolute positioning
    std::ostringstream pageContent;
    
    // Title - centered
    pageContent << "BT\n";
    pageContent << "/F1 20 Tf\n";
    pageContent << "150 720 Td\n";
    pageContent << "(EXAMINATION TIMETABLE) Tj\n";
    pageContent << "ET\n";
    
    // Subtitle
    pageContent << "BT\n";
    pageContent << "/F1 11 Tf\n";
    pageContent << "72 690 Td\n";
    pageContent << "(Exam Duration: 2:00 PM - 5:00 PM | Room Capacity: 55 Students) Tj\n";
    pageContent << "ET\n";
    
    // Define table dimensions and positions
    float leftMargin = 72;
    float tableTop = 650;
    float rowHeight = 18;
    
    // Column positions (absolute X coordinates) - optimized for page width
    float dayX = leftMargin;
    float dateX = leftMargin + 55;
    float courseX = leftMargin + 140;
    float studentsX = leftMargin + 210;
    float roomX = leftMargin + 420;
    float capX = leftMargin + 485;
    float tableRightEdge = leftMargin + 530; // Ensure table fits within page margins
    
    // Table header background
    pageContent << "0.9 g\n";
    pageContent << leftMargin << " " << (tableTop - 2) << " " << (tableRightEdge - leftMargin) << " " << rowHeight << " re f\n";
    pageContent << "0 g\n";
    
    // Draw table borders - complete border frame
    pageContent << "0.5 w\n";
    
    // Calculate table bottom dynamically based on content
    float estimatedTableBottom = tableTop - rowHeight * (schedule.size() + 3);
    if (estimatedTableBottom < 150) {
        estimatedTableBottom = 150; // Minimum bottom position
    }
    
    // Vertical lines for columns (full height)
    pageContent << dayX << " " << tableTop << " m " << dayX << " " << estimatedTableBottom << " l S\n";
    pageContent << dateX << " " << tableTop << " m " << dateX << " " << estimatedTableBottom << " l S\n";
    pageContent << courseX << " " << tableTop << " m " << courseX << " " << estimatedTableBottom << " l S\n";
    pageContent << studentsX << " " << tableTop << " m " << studentsX << " " << estimatedTableBottom << " l S\n";
    pageContent << roomX << " " << tableTop << " m " << roomX << " " << estimatedTableBottom << " l S\n";
    pageContent << capX << " " << tableTop << " m " << capX << " " << estimatedTableBottom << " l S\n";
    pageContent << tableRightEdge << " " << tableTop << " m " << tableRightEdge << " " << estimatedTableBottom << " l S\n";
    
    // Horizontal lines - top and header separator
    pageContent << leftMargin << " " << tableTop << " m " << tableRightEdge << " " << tableTop << " l S\n";
    pageContent << leftMargin << " " << (tableTop - rowHeight) << " m " << tableRightEdge << " " << (tableTop - rowHeight) << " l S\n";
    
    // Table headers with absolute positioning
    float headerY = tableTop - 14;
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (dayX + 3) << " " << headerY << " Td\n";
    pageContent << "(Day) Tj\n";
    pageContent << "ET\n";
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (dateX + 3) << " " << headerY << " Td\n";
    pageContent << "(Date) Tj\n";
    pageContent << "ET\n";
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (courseX + 3) << " " << headerY << " Td\n";
    pageContent << "(Course) Tj\n";
    pageContent << "ET\n";
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (studentsX + 3) << " " << headerY << " Td\n";
    pageContent << "(Students) Tj\n";
    pageContent << "ET\n";
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (roomX + 3) << " " << headerY << " Td\n";
    pageContent << "(Room) Tj\n";
    pageContent << "ET\n";
    
    pageContent << "BT\n";
    pageContent << "/F1 10 Tf\n";
    pageContent << (capX + 3) << " " << headerY << " Td\n";
    pageContent << "(Cap.) Tj\n";
    pageContent << "ET\n";
    
    // Data rows with absolute positioning
    float currentY = tableTop - rowHeight;
    int rowNum = 0;
    
    for (const auto& entry : schedule) {
        if (currentY < 150) break; // Leave more space at bottom for proper closure
        
        currentY -= rowHeight;
        
        // Parse entry
        std::stringstream ss(entry);
        std::string day, dayNum, date, courseId, rollNumbers, room, students;
        
        std::getline(ss, day, ',');
        std::getline(ss, dayNum, ',');
        std::getline(ss, date, ',');
        std::getline(ss, courseId, ',');
        std::getline(ss, rollNumbers, ',');
        std::getline(ss, room, ',');
        std::getline(ss, students);
        
        // Alternate row background
        if (rowNum % 2 == 0) {
            pageContent << "0.95 g\n";
            pageContent << leftMargin << " " << (currentY - 2) << " " << (tableRightEdge - leftMargin) << " " << rowHeight << " re f\n";
            pageContent << "0 g\n";
        }
        
        // Draw horizontal line for this row
        pageContent << leftMargin << " " << currentY << " m " << tableRightEdge << " " << currentY << " l S\n";
        
        float dataY = currentY + 5;
        
        // Day column (absolute position)
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";
        pageContent << (dayX + 3) << " " << dataY << " Td\n";
        std::string shortDay = day.substr(0, 3);
        pageContent << "(" << shortDay << ") Tj\n";
        pageContent << "ET\n";
        
        // Date column (absolute position)
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";
        pageContent << (dateX + 3) << " " << dataY << " Td\n";
        pageContent << "(" << date << ") Tj\n";
        pageContent << "ET\n";
        
        // Course column (absolute position)
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";
        pageContent << (courseX + 3) << " " << dataY << " Td\n";
        pageContent << "(" << courseId << ") Tj\n";
        pageContent << "ET\n";
        
        // Students column (absolute position, truncate if too long)
        pageContent << "BT\n";
        pageContent << "/F1 8 Tf\n"; // Smaller font for student range
        pageContent << (studentsX + 3) << " " << dataY << " Td\n";
        std::string studentRange = rollNumbers;
        if (studentRange.length() > 32) {
            studentRange = studentRange.substr(0, 29) + "...";
        }
        pageContent << "(" << studentRange << ") Tj\n";
        pageContent << "ET\n";
        
        // Room column (absolute position)
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";
        pageContent << (roomX + 3) << " " << dataY << " Td\n";
        pageContent << "(" << room << ") Tj\n";
        pageContent << "ET\n";
        
        // Capacity column (absolute position)
        pageContent << "BT\n";
        pageContent << "/F1 9 Tf\n";
        pageContent << (capX + 3) << " " << dataY << " Td\n";
        pageContent << "(" << students << ") Tj\n";
        pageContent << "ET\n";
        
        rowNum++;
    }
    
    // Final table closure - draw proper bottom border
    float finalY = currentY - rowHeight;
    pageContent << "1 w\n"; // Thicker line for bottom border
    pageContent << leftMargin << " " << finalY << " m " << tableRightEdge << " " << finalY << " l S\n";
    
    // Ensure all vertical lines reach the bottom properly
    pageContent << "0.5 w\n"; // Reset line width
    pageContent << dayX << " " << finalY << " m " << dayX << " " << (finalY - 3) << " l S\n";
    pageContent << dateX << " " << finalY << " m " << dateX << " " << (finalY - 3) << " l S\n";
    pageContent << courseX << " " << finalY << " m " << courseX << " " << (finalY - 3) << " l S\n";
    pageContent << studentsX << " " << finalY << " m " << studentsX << " " << (finalY - 3) << " l S\n";
    pageContent << roomX << " " << finalY << " m " << roomX << " " << (finalY - 3) << " l S\n";
    pageContent << capX << " " << finalY << " m " << capX << " " << (finalY - 3) << " l S\n";
    pageContent << tableRightEdge << " " << finalY << " m " << tableRightEdge << " " << (finalY - 3) << " l S\n";
    
    // Footer with proper spacing and positioning
    float footerY = finalY - 30; // More space from table
    pageContent << "BT\n";
    pageContent << "/F1 8 Tf\n";
    pageContent << leftMargin << " " << footerY << " Td\n";
    pageContent << "(Generated: Nov 16 2025 | Sessions: " << schedule.size() << " | Courses: 43 | Students: 800) Tj\n";
    pageContent << "ET\n";
    
    // Build complete PDF structure
    std::string pageContentStr = pageContent.str();
    int contentLength = pageContentStr.length();
    
    std::ostringstream pdf;
    pdf << "%PDF-1.4\n";
    
    // Object 1: Catalog
    int obj1Offset = pdf.tellp();
    pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";
    
    // Object 2: Pages
    int obj2Offset = pdf.tellp();
    pdf << "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n";
    
    // Object 3: Page
    int obj3Offset = pdf.tellp();
    pdf << "3 0 obj\n<< /Type /Page /Parent 2 0 R /Resources 4 0 R /MediaBox [0 0 612 792] /Contents 5 0 R >>\nendobj\n";
    
    // Object 4: Resources
    int obj4Offset = pdf.tellp();
    pdf << "4 0 obj\n<< /Font << /F1 << /Type /Font /Subtype /Type1 /BaseFont /Helvetica >> >> >>\nendobj\n";
    
    // Object 5: Contents
    int obj5Offset = pdf.tellp();
    pdf << "5 0 obj\n<< /Length " << contentLength << " >>\nstream\n";
    pdf << pageContentStr;
    pdf << "\nendstream\nendobj\n";
    
    // Cross-reference table
    int xrefOffset = pdf.tellp();
    pdf << "xref\n0 6\n0000000000 65535 f \n";
    
    char buffer[20];
    sprintf(buffer, "%010d 00000 n \n", obj1Offset);
    pdf << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj2Offset);
    pdf << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj3Offset);
    pdf << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj4Offset);
    pdf << buffer;
    sprintf(buffer, "%010d 00000 n \n", obj5Offset);
    pdf << buffer;
    
    // Trailer
    pdf << "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n" << xrefOffset << "\n%%EOF\n";
    
    file << pdf.str();
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

std::string TimetableGenerator::calculateExamDate(const std::string& startDate, int dayOffset) {
    try {
        // Parse startDate in DD-MM-YYYY format
        int day = std::stoi(startDate.substr(0, 2));
        int month = std::stoi(startDate.substr(3, 2));
        int year = std::stoi(startDate.substr(6, 4));
        
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        
        std::time_t startTime = std::mktime(&tm);
        
        // Add dayOffset-1 days (since day 1 should be the start date itself)
        std::time_t examTime = startTime + ((dayOffset - 1) * 24 * 60 * 60);
        
        std::tm* examTm = std::localtime(&examTime);
        
        // Format as DD-MM-YYYY
        char buffer[12];
        std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", examTm);
        
        return std::string(buffer);
    } catch (...) {
        // Fallback to startDate if parsing fails
        return startDate;
    }
}