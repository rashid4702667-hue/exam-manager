#ifndef MODELS_H
#define MODELS_H

#include <string>

struct Course {
    std::string courseId;
    std::string courseName;
    std::string department;
    
    Course(std::string id, std::string name, std::string dept) 
        : courseId(id), courseName(name), department(dept) {}
};

struct Student {
    std::string rollNo;
    std::string name;
    int semester;
    
    Student(std::string roll, std::string n, int sem) 
        : rollNo(roll), name(n), semester(sem) {}
};

struct Enrollment {
    std::string rollNo;
    std::string courseId;
    
    Enrollment(std::string roll, std::string course) 
        : rollNo(roll), courseId(course) {}
};

#endif // MODELS_H