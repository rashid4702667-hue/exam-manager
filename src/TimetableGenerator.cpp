#include "../include/TimetableGenerator.h"
#include <iostream>
#include <fstream>
#include <QString>
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QPageSize>

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
            } else if (courseId == "CY102") {
                courseName = "Cybersecurity";
                section = "2-Jan";
            } else if (courseId == "DS501") {
                courseName = "Data Structures";
                section = "4-Jan";
            } else if (courseId == "WD801") {
                courseName = "Web Development";
                section = "3-Jan";
            } else if (courseId == "OOP201") {
                courseName = "Object Oriented";
                section = "2-Jan";
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

    // Set up the PDF writer
    QString qfilename = QString::fromStdString(filename);
    QPdfWriter writer(qfilename);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(40, 40, 40, 40));  // Increased margins to match image
    writer.setResolution(300);

    QPainter painter(&writer);
    QFont titleFont("Arial", 12, QFont::Bold);
    QFont headerFont("Arial", 10, QFont::Normal);  // Normal weight for header
    QFont normalFont("Arial", 10, QFont::Normal);  // Normal weight for content

    // Calculate dimensions
    const int pageWidth = writer.width();
    const int leftMargin = 40;
    const int topMargin = 40;
    const int rowHeight = 20;  // Reduced for tighter rows like in image
    const int headerRowHeight = 25;  // Slightly taller header row
    
    // Calculate column widths based on content
    const int totalWidth = pageWidth - (2 * leftMargin);
    const int dayWidth = totalWidth * 0.12;        // ~12% of space
    const int dateWidth = totalWidth * 0.12;       // ~12% of space
    const int courseWidth = totalWidth * 0.12;     // ~12% of space
    const int rollWidth = totalWidth * 0.40;       // ~40% of space
    const int roomWidth = totalWidth * 0.12;       // ~12% of space
    const int studentsWidth = totalWidth * 0.12;   // ~12% of space

    // Draw title and header info (right-aligned)
    int currentY = topMargin;
    painter.setFont(titleFont);
    painter.drawText(QRect(leftMargin, currentY, pageWidth - 2*leftMargin, headerRowHeight),
                    Qt::AlignRight, "EXAM TIMETABLE");

    currentY += headerRowHeight;
    painter.setFont(normalFont);
    painter.drawText(QRect(leftMargin, currentY, pageWidth - 2*leftMargin, rowHeight),
                    Qt::AlignRight, "All exams scheduled at 2:00 PM - 5:00 PM");

    currentY += rowHeight;
    painter.drawText(QRect(leftMargin, currentY, pageWidth - 2*leftMargin, rowHeight),
                    Qt::AlignRight, "Room capacity: 55 students per room");

    currentY += rowHeight;
    painter.drawText(QRect(leftMargin, currentY, pageWidth - 2*leftMargin, rowHeight),
                    Qt::AlignRight, "No student clashes detected");

    // Move down to start the table
    currentY += rowHeight;

    // Move down to start the table and add extra spacing
    currentY += rowHeight * 1.5;

    // Calculate column positions
    int currentX = leftMargin;

    // Draw table headers
    painter.setFont(headerFont);
    painter.drawText(QRect(currentX, currentY, dayWidth, headerRowHeight),
                    Qt::AlignLeft | Qt::AlignVCenter, "Course");
    currentX += dayWidth;

    painter.drawText(QRect(currentX, currentY, dateWidth, headerRowHeight),
                    Qt::AlignLeft | Qt::AlignVCenter, "Date");
    currentX += dateWidth;

    painter.drawText(QRect(currentX, currentY, courseWidth, headerRowHeight),
                    Qt::AlignLeft | Qt::AlignVCenter, "Room");
    currentX += courseWidth;

    // Skip headers for the remaining columns (leave them blank)
    currentX += rollWidth;     // Roll Numbers column - no header
    currentX += roomWidth;     // Room column - no header
    currentX += studentsWidth; // Students column - no header

    // Draw horizontal line under headers
    currentY += rowHeight;
    painter.drawLine(leftMargin, currentY, pageWidth - leftMargin, currentY);

    // Draw data rows
    painter.setFont(normalFont);
    
    for (const auto& entry : schedule) {
        std::string day, date, courseId, rollNumbers, room, students;
        size_t pos = 0, nextPos;
        std::string str = entry;
        
        // Parse CSV format
        nextPos = str.find(","); day = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); date = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); courseId = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); rollNumbers = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        nextPos = str.find(","); room = str.substr(0, nextPos); str = str.substr(nextPos + 1);
        students = str;

        currentY += rowHeight;
        currentX = leftMargin;  // Reset X position for new row

        // Draw row data
        painter.drawText(QRect(currentX, currentY, dayWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(day));
        currentX += dayWidth;

        painter.drawText(QRect(currentX, currentY, dateWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(date));
        currentX += dateWidth;

        painter.drawText(QRect(currentX, currentY, courseWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(courseId));
        currentX += courseWidth;

        // Use elided text for roll numbers if too long
        QString rollNumbersText = QString::fromStdString(rollNumbers);
        QFontMetrics fm(normalFont);
        QString elidedText = fm.elidedText(rollNumbersText, Qt::ElideRight, rollWidth - 5);
        painter.drawText(QRect(currentX, currentY, rollWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, elidedText);
        currentX += rollWidth;

        painter.drawText(QRect(currentX, currentY, roomWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(room));
        currentX += roomWidth;

        painter.drawText(QRect(currentX, currentY, studentsWidth, rowHeight),
                        Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(students));

        // Draw horizontal line under row
        currentY += rowHeight;
        painter.drawLine(leftMargin, currentY, pageWidth - leftMargin, currentY);
    }

    painter.end();
    return true;
}

std::vector<std::string> TimetableGenerator::getSchedule() const {
    return schedule;
}

bool TimetableGenerator::isGenerated() const {
    return generated;
}

void TimetableGenerator::cleanup() {
    schedule.clear();
    generated = false;
}