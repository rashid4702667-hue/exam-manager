#include "scheduletablemodel.h"
#include <sstream>
#include <map>

ScheduleTableModel::ScheduleTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , maxDays(15)
    , maxRooms(8)
{
    // Initialize empty schedule
    schedule.resize(maxDays);
    for (auto &day : schedule) {
        day.resize(maxRooms);
    }
}

int ScheduleTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return maxDays;
}

int ScheduleTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return maxRooms;
}

QVariant ScheduleTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const ExamSlot &slot = schedule[index.row()][index.column()];
        if (!slot.courseId.isEmpty()) {
            return QString("%1\n%2\nStudents: %3")
                .arg(slot.courseId)
                .arg(slot.courseName)
                .arg(slot.students);
        }
        return "Empty";
    }

    return QVariant();
}

QVariant ScheduleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return QString("Room #%1").arg(section + 1);
    } else {
        return QString("Day %1").arg(section + 1);
    }
}

void ScheduleTableModel::setSchedule(const TimetableGenerator &generator)
{
    beginResetModel();
    
    // Clear current schedule
    for (auto &day : schedule) {
        day.fill(ExamSlot());
    }

    if (generator.isGenerated()) {
        std::vector<std::string> generatedSchedule = generator.getSchedule();
        
        for (const auto& line : generatedSchedule) {
            if (line.empty()) continue;
            
            try {
                // Parse CSV format: day,dayNum,date,courseId,rollNumbers,room,students
                std::string str = line;
                std::string token;
                std::vector<std::string> tokens;
                std::stringstream ss(str);
                
                // Split the line into tokens
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                
                // Make sure we have all the required fields
                if (tokens.size() < 7) {
                    qDebug() << "Invalid line format:" << QString::fromStdString(line);
                    continue;
                }
                
                // Get day number (tokens[1])
                int dayNum = std::stoi(tokens[1]) - 1;
                
                // Get course ID (tokens[3])
                std::string courseId = tokens[3];
                
                // Get roll numbers (tokens[4])
                std::string studentRange = tokens[4];
                
                // Get room number from "Room#X" format (tokens[5])
                std::string roomStr = tokens[5];
                int roomNum = std::stoi(roomStr.substr(roomStr.find("#") + 1)) - 1;
                
                // Get number of students from "X/Y" format (tokens[6])
                std::string capacity = tokens[6];
                int numStudents = std::stoi(capacity.substr(0, capacity.find("/")));
                
                if (dayNum < maxDays && roomNum < maxRooms) {
                    ExamSlot slot;
                    slot.courseId = QString::fromStdString(courseId);
                    slot.courseName = QString::fromStdString(studentRange);
                    slot.students = numStudents;
                    schedule[dayNum][roomNum] = slot;
                }
            } catch (const std::exception& e) {
                qDebug() << "Error parsing schedule line:" << QString::fromStdString(line) << "\nError:" << e.what();
                continue;
            }
        }
    }

    endResetModel();
}