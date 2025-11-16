#ifndef MODELS_H
#define MODELS_H

#include <string>

struct Course {
    std::string courseId;
    std::string courseName;
    std::string department;
    
    Course() : courseId(""), courseName(""), department("") {}
    Course(std::string id, std::string name, std::string dept) 
        : courseId(id), courseName(name), department(dept) {}
};

struct Student {
    std::string rollNo;
    std::string name;
    int semester;
    std::string batch;
    std::string program;
    
    Student() : rollNo(""), name(""), semester(0), batch(""), program("") {}
    Student(std::string roll, std::string n, int sem) 
        : rollNo(roll), name(n), semester(sem), batch(""), program("") {}
        
    Student(std::string roll, std::string n, std::string b, std::string p) 
        : rollNo(roll), name(n), semester(0), batch(b), program(p) {}
};

struct Enrollment {
    std::string rollNo;
    std::string courseId;
    
    Enrollment(std::string roll, std::string course) 
        : rollNo(roll), courseId(course) {}
};

#endif // MODELS_H