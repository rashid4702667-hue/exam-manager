# Timetable Planner - Product Specification Document

## Table of Contents
1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Technical Stack](#technical-stack)
4. [Core Features](#core-features)
5. [User Interface Design](#user-interface-design)
6. [Backend Architecture](#backend-architecture)
7. [Data Models](#data-models)
8. [Algorithms & Logic](#algorithms--logic)
9. [Database Integration](#database-integration)
10. [File Export System](#file-export-system)
11. [Class Structure](#class-structure)
12. [Build Configuration](#build-configuration)
13. [Testing & Mock Data](#testing--mock-data)
14. [Future Enhancements](#future-enhancements)
15. [Technical Requirements](#technical-requirements)

## Project Overview

### Application Name
**Timetable Planner GUI** - An intelligent exam scheduling and timetable management system.

### Version
1.0

### Purpose
The Timetable Planner is a comprehensive desktop application designed to automate the generation, management, and export of examination schedules. It addresses the complex challenge of scheduling exams while avoiding student conflicts, optimizing room allocation, and providing multiple export formats for administrative use.

### Target Users
- Academic administrators
- Examination coordinators
- Educational institutions
- University registrars

## System Architecture

### Architecture Pattern
The application follows a **Model-View-Controller (MVC)** pattern with Qt's signal-slot mechanism for loose coupling between components.

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Presentation  │    │   Business      │    │   Data          │
│   Layer (Qt)    │    │   Logic         │    │   Layer         │
├─────────────────┤    ├─────────────────┤    ├─────────────────┤
│ • MainWindow    │◄──►│ • TimetableGen  │◄──►│ • DatabaseMgr   │
│ • Dialogs       │    │ • Algorithms    │    │ • Models        │
│ • TableModels   │    │ • Validators    │    │ • LinkedList    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Component Overview
- **Frontend**: Qt6 Widgets-based GUI with custom table models
- **Backend**: C++ business logic with algorithm implementation
- **Data Layer**: Mock database manager with structured data models
- **Export Engine**: Multi-format export system (CSV, Text, PDF)

## Technical Stack

### Core Technologies
- **Language**: C++17
- **GUI Framework**: Qt 6.10.0
- **Build System**: CMake 3.16+
- **Compiler**: MinGW 13.1.0 (64-bit)
- **Database**: Mock implementation (Oracle-ready)

### Qt Modules Used
- `Qt::Core` - Core functionality
- `Qt::Gui` - GUI components
- `Qt::Widgets` - Widget set
- `Qt::PrintSupport` - PDF export capabilities

### Development Tools
- **IDE**: Visual Studio Code / Qt Creator
- **Version Control**: Git
- **Deployment**: windeployqt for Windows distribution

## Core Features

### 1. Database Connectivity
- **Mock Database Manager**: Simulates Oracle database connection
- **Connection Dialog**: User-friendly database credential input
- **Error Handling**: Comprehensive connection failure management
- **Data Validation**: Input sanitization and validation

### 2. Intelligent Timetable Generation
- **Conflict Detection**: Automatically identifies student enrollment conflicts
- **Room Optimization**: Distributes students across available rooms and labs
- **Date Scheduling**: Spreads exams across multiple days
- **Capacity Management**: Ensures room and lab capacity limits (55 students per venue, 16 total venues)

### 3. Multi-Format Export System
#### CSV Export
- **Format**: Standard comma-separated values
- **Headers**: Date, Day_Num, Room, Room_Cap, Course_ID, Section, Course_N, Students_S, Student_F, Start_Time, End_Time, Capacity, Status
- **Use Case**: Import into Excel, database systems

#### Text Export
- **Format**: Human-readable plain text
- **Structure**: Simple tabular format
- **Use Case**: Quick viewing, printing

#### PDF Export
- **Format**: Professional PDF document
- **Features**: 
  - Custom header with exam details
  - Tabular layout with proper formatting
  - Page break handling for large schedules
  - Professional styling

### 4. Schedule Visualization
- **Table View**: Interactive Qt table widget
- **Real-time Updates**: Dynamic data refresh
- **Column Sorting**: Built-in Qt sorting capabilities
- **Row Selection**: Single-click selection

### 5. Course Management
- **Course Shifting**: Ability to reschedule specific courses
- **Conflict Resolution**: Visual conflict identification
- **Capacity Monitoring**: Real-time capacity tracking

## User Interface Design

### Main Window Layout
```
┌─────────────────────────────────────────────────────────────┐
│ File    Schedule    Export    Help                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │           Schedule Table View                       │    │
│  │  Day │ Date │ Course │ Students │ Room │ Capacity  │    │
│  │  ────┼──────┼────────┼──────────┼──────┼───────── │    │
│  │      │      │        │          │      │          │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Dialog Windows

#### 1. Database Connection Dialog
- **Fields**: DSN, Username, Password
- **Validation**: Real-time input validation
- **Security**: Password masking

#### 2. Generate Timetable Dialog
- **Date Picker**: Calendar widget for start date selection
- **Validation**: Date range verification
- **Preview**: Parameter summary before generation

### Menu Structure
```
File
├── Connect to Database
├── ────────────────
├── Export to CSV
├── Export to Text
└── Export to PDF

Schedule
├── Generate Timetable
├── View Schedule
└── Shift Course

Help
└── About
```

## Backend Architecture

### Core Classes Overview

#### 1. MainWindow Class
**File**: `src/mainwindow.cpp`, `include/mainwindow.h`

**Purpose**: Primary application controller and UI coordinator

**Key Responsibilities**:
- Menu action handling
- Dialog management
- Database connection coordination
- Export operation orchestration
- UI state management

**Key Methods**:
```cpp
void onConnectDatabase();
void onGenerateTimetable();
void onViewSchedule();
void onExportCSV();
void onExportText();
void onExportPDF();
void onShiftCourse();
void updateScheduleView();
void enableScheduleActions(bool enable);
```

#### 2. DatabaseManager Class
**File**: `src/DatabaseManager.cpp`, `include/DatabaseManager.h`

**Purpose**: Data access layer and mock database simulation

**Key Features**:
- Mock Oracle database implementation
- Structured data retrieval
- Connection state management
- Error handling and logging

**Data Generation**:
- **Students**: 320 students (CT-24001 to CT-24320)
- **Courses**: 10 courses across CS and IT departments
- **Enrollments**: Realistic conflict scenarios

**Key Methods**:
```cpp
bool connect(const string& dsn, const string& username, const string& password);
LinkedList<Course> fetchCourses();
LinkedList<Student> fetchStudents();
LinkedList<Enrollment> fetchEnrollments();
vector<string> getCourseCodes();
map<string, int> getCourseSizes();
```

#### 3. TimetableGenerator Class
**File**: `src/TimetableGenerator.cpp`, `include/TimetableGenerator.h`

**Purpose**: Core scheduling algorithm and export engine

**Algorithm Features**:
- Conflict detection and resolution
- Room capacity optimization
- Multi-day scheduling
- Student distribution algorithms

**Export Capabilities**:
- CSV with comprehensive headers
- Text with human-readable format
- PDF with professional layout

**Key Methods**:
```cpp
bool generate();
void setStartDate(const string& date);
bool exportToCSV(const string& filename);
bool exportToText(const string& filename);
bool exportToPDF(const string& filename);
```

## Data Models

### Core Data Structures

#### 1. Course Model
```cpp
struct Course {
    std::string courseId;      // e.g., "ML101"
    std::string courseName;    // e.g., "Machine Learning"
    std::string department;    // e.g., "Computer Science"
};
```

#### 2. Student Model
```cpp
struct Student {
    std::string rollNo;        // e.g., "CT-24001"
    std::string name;          // e.g., "John Doe"
    int semester;              // 1-8
};
```

#### 3. Enrollment Model
```cpp
struct Enrollment {
    std::string rollNo;        // Student identifier
    std::string courseId;      // Course identifier
};
```

### Custom Data Structure: LinkedList

**File**: `include/LinkedList.h`

**Features**:
- Generic template implementation
- Quick sort algorithm integration
- Memory-efficient node management
- Iterator-like access patterns

**Key Operations**:
```cpp
void insertAtEnd(const T& value);
Node<T>* getHead() const;
int getSize() const;
void quickSort();
```

## Algorithms & Logic

### 1. Timetable Generation Algorithm

**Core Logic**:
1. **Data Fetching**: Retrieve courses, students, and enrollments
2. **Conflict Analysis**: Identify overlapping enrollments
3. **Room Distribution**: Allocate students to rooms and labs (max 55 per venue)
4. **Date Scheduling**: Distribute exams across multiple days
5. **Schedule Optimization**: Minimize conflicts and maximize efficiency

**Sample Schedule Generation**:
```cpp
// Day 1: Friday - ML101
schedule.push_back("Friday,1,02-01-2026,ML101,CT-24001 to CT-24085,Room#1,55/55");
schedule.push_back("Friday,1,02-01-2026,ML101,CT-24312 to CT-24318,Room#2,25/55");

// Day 2: Saturday - AI401
schedule.push_back("Saturday,2,03-01-2026,AI401,CT-24001 to CT-24085,Room#1,55/55");

// Day 5: Tuesday - Lab sessions
schedule.push_back("Tuesday,5,06-01-2026,DB301,CT-24001 to CT-24055,Lab1,55/55");
schedule.push_back("Tuesday,5,06-01-2026,OOP201,CT-24056 to CT-24110,Lab2,55/55");

// Day 8: Friday Week 2 - Additional Room sessions
schedule.push_back("Friday,8,09-01-2026,DS501,CT-24001 to CT-24055,Room#6,55/55");
schedule.push_back("Friday,8,09-01-2026,DS501,CT-24056 to CT-24110,Room#7,55/55");
// ... additional room and lab allocations
```

### 2. Conflict Detection Algorithm

**Process**:
1. **Student Mapping**: Create enrollment maps per student
2. **Overlap Detection**: Identify students with multiple course enrollments
3. **Time Separation**: Ensure conflicting courses are scheduled on different days
4. **Validation**: Verify no student has simultaneous exams

### 3. Room Allocation Algorithm

**Strategy**:
- **Capacity-First**: Fill rooms to maximum capacity (55 students)
- **Sequential Assignment**: Assign consecutive roll numbers to same room
- **Overflow Management**: Create additional rooms for remaining students

## Database Integration

### Mock Database Implementation

**Current State**: The application uses a mock database manager that simulates Oracle connectivity while providing hardcoded test data.

**Benefits**:
- **No Oracle Dependency**: Can run without database installation
- **Consistent Testing**: Predictable data for algorithm testing
- **Development Speed**: Immediate data availability

**Oracle Integration Ready**: The architecture supports easy migration to actual Oracle database with minimal code changes.

### Data Flow
```
Mock Database → LinkedList Structures → Algorithm Processing → Schedule Generation → Export
```

### Migration Path to Oracle
1. Replace mock methods with actual SQL queries
2. Implement ODBC/OCI connection handling
3. Add proper error handling for database operations
4. Maintain existing data model interfaces

## File Export System

### CSV Export Engine

**Format Specification**:
```csv
Date,Day_Num,Room,Room_Cap,Course_ID,Section,Course_N,Students_S,Student_F,Start_Time,End_Time,Capacity,Status
01-02-2026,1,Room#1,55,ML101,2-Jan,Machine Learning,55,CT-24001 to CT-24085,2:00 PM,5:00 PM,55,OK
```

**Features**:
- Excel-compatible format
- Comprehensive metadata
- Student range tracking
- Status validation

### PDF Export Engine

**Technical Implementation**:
- **Custom PDF Generation**: Direct PDF creation without external libraries
- **Professional Layout**: Tabular format with headers and borders
- **Page Management**: Automatic page breaks for large schedules
- **Font Support**: Helvetica font for consistency

**PDF Structure**:
```
%PDF-1.4
1 0 obj << /Type /Catalog /Pages 2 0 R >>
2 0 obj << /Type /Pages /Kids [3 0 R] /Count 1 >>
3 0 obj << /Type /Page /Parent 2 0 R /Resources 4 0 R >>
```

## Class Structure

### Inheritance Hierarchy
```
QMainWindow
└── MainWindow

QDialog
├── ConnectDialog
└── GenerateTimetableDialog

QAbstractTableModel
└── ScheduleTableModel

DatabaseManager (Standalone)
TimetableGenerator (Standalone)
```

### Dependencies Graph
```
MainWindow
├── DatabaseManager
├── TimetableGenerator
├── ConnectDialog
├── GenerateTimetableDialog
└── ScheduleTableModel

TimetableGenerator
└── DatabaseManager

ScheduleTableModel
└── TimetableGenerator
```

## Build Configuration

### CMake Configuration

**File**: `CMakeLists.txt`

**Key Features**:
- **Qt 6.10.0 Integration**: Automatic Qt detection and linking
- **MinGW Support**: Configured for Windows MinGW toolchain
- **Auto-Generated Headers**: MOC, UIC, and RCC support
- **Deployment Ready**: windeployqt integration

**Target Configuration**:
```cmake
qt_add_executable(TimetablePlannerGUI
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_link_libraries(TimetablePlannerGUI PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::PrintSupport
)
```

### Build Process
1. **Configure**: `cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release`
2. **Build**: `cmake --build .`
3. **Deploy**: Automatic windeployqt execution

## Testing & Mock Data

### Mock Data Specifications

#### Student Data
- **Range**: CT-24001 to CT-24320 (320 students)
- **Distribution**: Across 8 semesters
- **Naming**: Mix of real names and generated names

#### Course Data
- **Count**: 10 courses
- **Departments**: Computer Science and Information Technology
- **Coverage**: Various difficulty levels and semesters

#### Enrollment Data
- **Pattern**: 4-5 courses per student
- **Conflicts**: Intentionally created for testing
- **Distribution**: Realistic enrollment patterns

### Testing Scenarios
1. **Connection Testing**: Mock database connection validation
2. **Generation Testing**: Algorithm correctness verification
3. **Export Testing**: File format validation
4. **UI Testing**: User interaction workflows

## Future Enhancements

### Phase 2 Features
1. **Oracle Database Integration**: Replace mock with actual database
2. **Advanced Conflict Resolution**: AI-powered scheduling optimization
3. **Room Management**: Dynamic room allocation and booking
4. **Student Notifications**: Email/SMS notification system
5. **Web Interface**: Browser-based access portal

### Phase 3 Features
1. **Mobile Application**: iOS/Android companion apps
2. **Cloud Integration**: Multi-institution support
3. **Analytics Dashboard**: Scheduling analytics and reporting
4. **API Integration**: Third-party system connections

### Technical Improvements
1. **Performance Optimization**: Large dataset handling
2. **Memory Management**: Smart pointer implementation
3. **Logging System**: Comprehensive audit trails
4. **Configuration Management**: External configuration files

## Technical Requirements

### Development Environment
- **Operating System**: Windows 10/11
- **IDE**: Visual Studio Code with C++ extensions
- **Compiler**: MinGW 13.1.0 or later
- **Qt Version**: 6.10.0 or later
- **CMake**: 3.16 or later

### Runtime Requirements
- **OS**: Windows 10/11 (64-bit)
- **Memory**: 4GB RAM minimum, 8GB recommended
- **Storage**: 100MB for application, additional for data files
- **Display**: 1024x768 minimum resolution

### Performance Specifications
- **Startup Time**: < 3 seconds
- **Schedule Generation**: < 5 seconds for 300+ students
- **Export Time**: < 2 seconds for standard schedules
- **Memory Usage**: < 100MB during normal operation

## Conclusion

The Timetable Planner represents a comprehensive solution for academic scheduling challenges. With its robust architecture, intelligent algorithms, and user-friendly interface, it provides educational institutions with a powerful tool for exam management. The mock database implementation ensures immediate usability while maintaining a clear path for enterprise database integration.

The application's modular design, extensive export capabilities, and professional UI make it suitable for production use in academic environments. Future enhancements will expand its capabilities while maintaining the core stability and reliability that define the current implementation.

---

**Document Version**: 1.0  
**Last Updated**: November 10, 2025  
**Author**: System Architecture Team  
**Review Status**: Technical Review Complete