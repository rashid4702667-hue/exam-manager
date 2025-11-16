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
    
    // We'll paginate the table so all sessions are included across pages.
    // Define page and table metrics
    float leftMargin = 72;
    float tableTop = 650;
    float rowHeight = 18;

    float dayX = leftMargin;
    float dateX = leftMargin + 55;
    float courseX = leftMargin + 140;
    float studentsX = leftMargin + 210;
    float roomX = leftMargin + 420;
    float capX = leftMargin + 485;
    float tableRightEdge = leftMargin + 530;
    float minBottom = 150; // minimum bottom Y for table on each page

    // Helper to create a new page header (returns an ostringstream with header content)
    auto createPageHeader = [&](int pageNumber) {
        std::ostringstream p;

        // Title - centered
        p << "BT\n";
        p << "/F1 20 Tf\n";
        p << "150 720 Td\n";
        p << "(EXAMINATION TIMETABLE) Tj\n";
        p << "ET\n";

        // Subtitle
        p << "BT\n";
        p << "/F1 11 Tf\n";
        p << "72 690 Td\n";
        p << "(Exam Duration: 2:00 PM - 5:00 PM | Room Capacity: 55 Students) Tj\n";
        p << "ET\n";

        // Table header background and borders
        p << "0.9 g\n";
        p << leftMargin << " " << (tableTop - 2) << " " << (tableRightEdge - leftMargin) << " " << rowHeight << " re f\n";
        p << "0 g\n";
        p << "0.5 w\n";

        // Note: Vertical lines will be drawn dynamically as content is added
        // We'll draw them only to the actual table bottom, not to minBottom

        // Horizontal lines - top and header separator
        p << leftMargin << " " << tableTop << " m " << tableRightEdge << " " << tableTop << " l S\n";
        p << leftMargin << " " << (tableTop - rowHeight) << " m " << tableRightEdge << " " << (tableTop - rowHeight) << " l S\n";

        float headerY = tableTop - 14;

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (dayX + 3) << " " << headerY << " Td\n";
        p << "(Day) Tj\n";
        p << "ET\n";

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (dateX + 3) << " " << headerY << " Td\n";
        p << "(Date) Tj\n";
        p << "ET\n";

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (courseX + 3) << " " << headerY << " Td\n";
        p << "(Course) Tj\n";
        p << "ET\n";

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (studentsX + 3) << " " << headerY << " Td\n";
        p << "(Students) Tj\n";
        p << "ET\n";

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (roomX + 3) << " " << headerY << " Td\n";
        p << "(Room) Tj\n";
        p << "ET\n";

        p << "BT\n";
        p << "/F1 10 Tf\n";
        p << (capX + 3) << " " << headerY << " Td\n";
        p << "(Cap.) Tj\n";
        p << "ET\n";

        return p;
    };

    // Build pages by streaming rows into page headers and creating new pages when needed
    std::vector<std::string> pageContents;
    std::ostringstream currentPage = createPageHeader(1);
    float currentY = tableTop - rowHeight;
    int rowNum = 0;

    for (const auto& entry : schedule) {
        // If not enough space for another row, finish current page and start a new one
        if (currentY < (minBottom + rowHeight)) {
            // Draw vertical lines from tableTop to current table bottom
            currentPage << "0.5 w\n";
            float tableBottom = currentY;
            currentPage << dayX << " " << tableTop << " m " << dayX << " " << tableBottom << " l S\n";
            currentPage << dateX << " " << tableTop << " m " << dateX << " " << tableBottom << " l S\n";
            currentPage << courseX << " " << tableTop << " m " << courseX << " " << tableBottom << " l S\n";
            currentPage << studentsX << " " << tableTop << " m " << studentsX << " " << tableBottom << " l S\n";
            currentPage << roomX << " " << tableTop << " m " << roomX << " " << tableBottom << " l S\n";
            currentPage << capX << " " << tableTop << " m " << capX << " " << tableBottom << " l S\n";
            currentPage << tableRightEdge << " " << tableTop << " m " << tableRightEdge << " " << tableBottom << " l S\n";
            
            // Draw bottom border
            float finalY = currentY - rowHeight;
            currentPage << "1 w\n";
            currentPage << leftMargin << " " << finalY << " m " << tableRightEdge << " " << finalY << " l S\n";

            float footerY = finalY - 30;
            currentPage << "BT\n";
            currentPage << "/F1 8 Tf\n";
            currentPage << leftMargin << " " << footerY << " Td\n";
            currentPage << "(Generated: Nov 16 2025 | Sessions: " << schedule.size() << ") Tj\n";
            currentPage << "ET\n";

            pageContents.push_back(currentPage.str());

            // Start a new page
            currentPage = createPageHeader(static_cast<int>(pageContents.size()) + 1);
            currentY = tableTop - rowHeight;
            rowNum = 0;
        }

        // Add row
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
            currentPage << "0.95 g\n";
            currentPage << leftMargin << " " << (currentY - 2) << " " << (tableRightEdge - leftMargin) << " " << rowHeight << " re f\n";
            currentPage << "0 g\n";
        }

        // Draw horizontal line for this row
        currentPage << leftMargin << " " << currentY << " m " << tableRightEdge << " " << currentY << " l S\n";

        float dataY = currentY + 5;

        // Day
        currentPage << "BT\n";
        currentPage << "/F1 9 Tf\n";
        currentPage << (dayX + 3) << " " << dataY << " Td\n";
        std::string shortDay = day.substr(0, 3);
        currentPage << "(" << shortDay << ") Tj\n";
        currentPage << "ET\n";

        // Date
        currentPage << "BT\n";
        currentPage << "/F1 9 Tf\n";
        currentPage << (dateX + 3) << " " << dataY << " Td\n";
        currentPage << "(" << date << ") Tj\n";
        currentPage << "ET\n";

        // Course
        currentPage << "BT\n";
        currentPage << "/F1 9 Tf\n";
        currentPage << (courseX + 3) << " " << dataY << " Td\n";
        currentPage << "(" << courseId << ") Tj\n";
        currentPage << "ET\n";

        // Students (truncate if too long)
        currentPage << "BT\n";
        currentPage << "/F1 8 Tf\n";
        currentPage << (studentsX + 3) << " " << dataY << " Td\n";
        std::string studentRange = rollNumbers;
        if (studentRange.length() > 32) {
            studentRange = studentRange.substr(0, 29) + "...";
        }
        currentPage << "(" << studentRange << ") Tj\n";
        currentPage << "ET\n";

        // Room
        currentPage << "BT\n";
        currentPage << "/F1 9 Tf\n";
        currentPage << (roomX + 3) << " " << dataY << " Td\n";
        currentPage << "(" << room << ") Tj\n";
        currentPage << "ET\n";

        // Capacity
        currentPage << "BT\n";
        currentPage << "/F1 9 Tf\n";
        currentPage << (capX + 3) << " " << dataY << " Td\n";
        currentPage << "(" << students << ") Tj\n";
        currentPage << "ET\n";

        rowNum++;
    }

    // Push last page
    if (!currentPage.str().empty()) {
        // Draw vertical lines from tableTop to actual table bottom
        currentPage << "0.5 w\n";
        float tableBottom = currentY;
        currentPage << dayX << " " << tableTop << " m " << dayX << " " << tableBottom << " l S\n";
        currentPage << dateX << " " << tableTop << " m " << dateX << " " << tableBottom << " l S\n";
        currentPage << courseX << " " << tableTop << " m " << courseX << " " << tableBottom << " l S\n";
        currentPage << studentsX << " " << tableTop << " m " << studentsX << " " << tableBottom << " l S\n";
        currentPage << roomX << " " << tableTop << " m " << roomX << " " << tableBottom << " l S\n";
        currentPage << capX << " " << tableTop << " m " << capX << " " << tableBottom << " l S\n";
        currentPage << tableRightEdge << " " << tableTop << " m " << tableRightEdge << " " << tableBottom << " l S\n";
        
        // Draw bottom border for the last page
        float finalY = currentY - rowHeight;
        currentPage << "1 w\n";
        currentPage << leftMargin << " " << finalY << " m " << tableRightEdge << " " << finalY << " l S\n";

        float footerY = finalY - 30;
        currentPage << "BT\n";
        currentPage << "/F1 8 Tf\n";
        currentPage << leftMargin << " " << footerY << " Td\n";
        currentPage << "(Generated: Nov 16 2025 | Sessions: " << schedule.size() << ") Tj\n";
        currentPage << "ET\n";

        pageContents.push_back(currentPage.str());
    }

    // Build complete PDF structure with one page object per page
    std::ostringstream pdf;
    pdf << "%PDF-1.4\n";

    int numPages = static_cast<int>(pageContents.size());
    int totalObjects = 3 + numPages + numPages; // 1:Catalog,2:Pages, 3..(2+numPages): Page objs, resources, then numPages content objs
    int resourcesObjNum = 3 + numPages;

    std::vector<int> offsets(totalObjects + 1, 0);

    // Object 1: Catalog
    offsets[1] = static_cast<int>(pdf.tellp());
    pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";

    // Object 2: Pages (kids list)
    offsets[2] = static_cast<int>(pdf.tellp());
    pdf << "2 0 obj\n<< /Type /Pages /Kids [";
    for (int i = 0; i < numPages; ++i) {
        int pageObjNum = 3 + i;
        pdf << pageObjNum << " 0 R ";
    }
    pdf << "] /Count " << numPages << " >>\nendobj\n";

    // Page objects (3 .. 2+numPages)
    for (int i = 0; i < numPages; ++i) {
        int pageObjNum = 3 + i;
        offsets[pageObjNum] = static_cast<int>(pdf.tellp());
        int contentObjNum = resourcesObjNum + 1 + i;
        pdf << pageObjNum << " 0 obj\n<< /Type /Page /Parent 2 0 R /Resources " << resourcesObjNum << " 0 R /MediaBox [0 0 612 792] /Contents " << contentObjNum << " 0 R >>\nendobj\n";
    }

    // Resources object
    offsets[resourcesObjNum] = static_cast<int>(pdf.tellp());
    pdf << resourcesObjNum << " 0 obj\n<< /Font << /F1 << /Type /Font /Subtype /Type1 /BaseFont /Helvetica >> >> >>\nendobj\n";

    // Content objects (streams)
    for (int i = 0; i < numPages; ++i) {
        int contentObjNum = resourcesObjNum + 1 + i;
        offsets[contentObjNum] = static_cast<int>(pdf.tellp());
        std::string& content = pageContents[i];
        int length = static_cast<int>(content.length());
        pdf << contentObjNum << " 0 obj\n<< /Length " << length << " >>\nstream\n";
        pdf << content;
        pdf << "\nendstream\nendobj\n";
    }

    // Cross-reference table
    int xrefOffset = static_cast<int>(pdf.tellp());
    pdf << "xref\n0 " << (totalObjects + 1) << "\n";
    pdf << "0000000000 65535 f \n";
    char buffer[64];
    for (int i = 1; i <= totalObjects; ++i) {
        sprintf(buffer, "%010d 00000 n \n", offsets[i]);
        pdf << buffer;
    }

    // Trailer
    pdf << "trailer\n<< /Size " << (totalObjects + 1) << " /Root 1 0 R >>\nstartxref\n" << xrefOffset << "\n%%EOF\n";

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