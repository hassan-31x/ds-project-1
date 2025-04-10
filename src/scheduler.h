#pragma once

#include "pq_tree.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>

// Time slot structure
struct TimeSlot {
    int day;           // 0-4 (Monday-Friday)
    int hour;          // 8-17 (8:00 - 17:00)
    int duration;      // Duration in hours
    
    std::string toString() const;
    bool overlaps(const TimeSlot& other) const;
    
    static std::string dayToString(int day);
};

// Teacher class
class Teacher {
public:
    std::string id;
    std::string name;
    std::vector<TimeSlot> availableTimeSlots;
    
    Teacher(const std::string& id, const std::string& name);
    void addAvailableTimeSlot(const TimeSlot& slot);
};

// Course class
class Course {
public:
    std::string code;
    std::string title;
    int creditHours;
    
    std::vector<std::shared_ptr<Teacher>> assignedTeachers;
    
    Course(const std::string& code, const std::string& title, int credits);
    void assignTeacher(std::shared_ptr<Teacher> teacher);
};

// Section class
class Section {
public:
    std::string id;
    std::shared_ptr<Course> course;
    std::shared_ptr<Teacher> teacher;
    std::vector<TimeSlot> timeSlots;
    
    Section(const std::string& id, std::shared_ptr<Course> course);
    void assignTeacher(std::shared_ptr<Teacher> teacher);
    void addTimeSlot(const TimeSlot& slot);
};

// Student preferences
struct StudentPreference {
    enum PreferenceType {
        PREFER_TEACHER,
        PREFER_TIME_SLOT,
        AVOID_TEACHER,
        AVOID_TIME_SLOT
    };
    
    PreferenceType type;
    std::string courseCode;
    
    // For teacher preferences
    std::string teacherId;
    
    // For time slot preferences
    TimeSlot timeSlot;
    
    float weight; // Importance of this preference (0.0-1.0)
};

// Class Scheduler
class ClassScheduler {
private:
    std::vector<std::shared_ptr<Course>> courses;
    std::vector<std::shared_ptr<Teacher>> teachers;
    std::vector<std::shared_ptr<Section>> sections;
    std::vector<StudentPreference> preferences;
    
    PQTree scheduleTree;
    
    // Internal scheduling data
    std::map<std::string, std::vector<std::string>> possibleAssignments;
    
public:
    ClassScheduler();
    ~ClassScheduler();
    
    // Data management
    void addCourse(std::shared_ptr<Course> course);
    void addTeacher(std::shared_ptr<Teacher> teacher);
    void addSection(std::shared_ptr<Section> section);
    void addPreference(const StudentPreference& preference);
    
    // Get data for UI
    const std::vector<std::shared_ptr<Course>>& getCourses() const;
    const std::vector<std::shared_ptr<Teacher>>& getTeachers() const;
    const std::vector<std::shared_ptr<Section>>& getSections() const;
    
    // Scheduling operations
    bool generateSchedule();
    bool validateSchedule() const;
    float evaluateSchedule() const;
    
    // Apply PQ tree constraints for scheduling
    void applyConstraints();
    
    // Get schedule results
    std::vector<std::shared_ptr<Section>> getSchedule() const;
}; 