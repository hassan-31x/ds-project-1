#include "scheduler.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>

std::string TimeSlot::toString() const
{
    std::stringstream timeStream;
    timeStream << dayToString(day) << " "
               << hour << ":00-" << (hour + duration) << ":00";
    return timeStream.str();
}

bool TimeSlot::overlaps(const TimeSlot &otherSlot) const
{
    if (day != otherSlot.day)
        return false;
    const int currentEnd = hour + duration;
    const int comparisonEnd = otherSlot.hour + otherSlot.duration;
    return (hour < comparisonEnd && otherSlot.hour < currentEnd);
}

std::string TimeSlot::dayToString(int weekday)
{
    switch (weekday)
    {
    case 0:
        return "Monday";
    case 1:
        return "Tuesday";
    case 2:
        return "Wednesday";
    case 3:
        return "Thursday";
    case 4:
        return "Friday";
    default:
        return "Unknown";
    }
}

Teacher::Teacher(const std::string &teacherID, const std::string &teacherName)
    : id(teacherID), name(teacherName) {}

void Teacher::addAvailableTimeSlot(const TimeSlot &newSlot)
{
    availableTimeSlots.push_back(newSlot);
}

Course::Course(const std::string &courseCode,
               const std::string &courseTitle,
               int creditCount)
    : code(courseCode), title(courseTitle), creditHours(creditCount) {}

void Course::assignTeacher(std::shared_ptr<Teacher> instructor)
{
    assignedTeachers.push_back(instructor);
}

Section::Section(const std::string &sectionID,
                 std::shared_ptr<Course> courseData)
    : id(sectionID), course(courseData), teacher(nullptr) {}

void Section::assignTeacher(std::shared_ptr<Teacher> instructor)
{
    teacher = instructor;
}

void Section::addTimeSlot(const TimeSlot &newSlot)
{
    timeSlots.push_back(newSlot);
}

ClassScheduler::ClassScheduler() {}

ClassScheduler::~ClassScheduler()
{
    courses.clear();
    teachers.clear();
    sections.clear();
}

void ClassScheduler::addCourse(std::shared_ptr<Course> newCourse)
{
    courses.push_back(newCourse);
}

void ClassScheduler::addTeacher(std::shared_ptr<Teacher> newInstructor)
{
    teachers.push_back(newInstructor);
}

void ClassScheduler::addSection(std::shared_ptr<Section> newSection)
{
    sections.push_back(newSection);
}

void ClassScheduler::addPreference(const StudentPreference &newPreference)
{
    preferences.push_back(newPreference);
}

const std::vector<std::shared_ptr<Course>> &ClassScheduler::getCourses() const
{
    return courses;
}

const std::vector<std::shared_ptr<Teacher>> &ClassScheduler::getTeachers() const
{
    return teachers;
}

const std::vector<std::shared_ptr<Section>> &ClassScheduler::getSections() const
{
    return sections;
}

bool ClassScheduler::generateSchedule()
{
    for (auto &section : sections)
    {
        section->teacher = nullptr;
        section->timeSlots.clear();
    }

    std::vector<TimeSlot> allTimeSlots;
    for (int day = 0; day < 5; day++)
    {
        for (int hour = 8; hour <= 16; hour++)
        {
            allTimeSlots.push_back({day, hour, 1});
        }
    }

    std::vector<std::string> elements;
    for (const auto &slot : allTimeSlots)
    {
        std::stringstream ss;
        ss << "ts_" << slot.day << "_" << slot.hour;
        elements.push_back(ss.str());
    }

    scheduleTree.createFromUniversalSet(elements);
    applyConstraints();

    auto arrangements = scheduleTree.getPossibleArrangements();
    if (arrangements.empty())
        return false;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::vector<std::shared_ptr<Section>> shuffledSections = sections;
    std::shuffle(shuffledSections.begin(), shuffledSections.end(), generator);

    for (auto &section : shuffledSections)
    {
        std::vector<std::shared_ptr<Teacher>> suitableTeachers;
        for (auto &teacher : section->course->assignedTeachers)
        {
            bool preferred = false;
            bool avoided = false;

            for (const auto &pref : preferences)
            {
                if (pref.courseCode == section->course->code &&
                    pref.teacherId == teacher->id)
                {
                    if (pref.type == StudentPreference::PREFER_TEACHER)
                        preferred = true;
                    else if (pref.type == StudentPreference::AVOID_TEACHER)
                        avoided = true;
                }
            }

            if (!avoided || preferred)
                suitableTeachers.push_back(teacher);
        }

        if (suitableTeachers.empty() && !section->course->assignedTeachers.empty())
        {
            suitableTeachers = section->course->assignedTeachers;
        }

        if (suitableTeachers.empty())
            continue;

        std::shuffle(suitableTeachers.begin(), suitableTeachers.end(), generator);
        bool assigned = false;

        for (auto &teacher : suitableTeachers)
        {
            std::vector<TimeSlot> validSlots;
            for (const auto &slot : teacher->availableTimeSlots)
            {
                bool preferred = false;
                bool avoided = false;

                for (const auto &pref : preferences)
                {
                    if (pref.courseCode == section->course->code)
                    {
                        if (pref.type == StudentPreference::PREFER_TIME_SLOT &&
                            pref.timeSlot.day == slot.day &&
                            pref.timeSlot.hour == slot.hour)
                        {
                            preferred = true;
                        }
                        else if (pref.type == StudentPreference::AVOID_TIME_SLOT &&
                                 pref.timeSlot.day == slot.day &&
                                 pref.timeSlot.hour == slot.hour)
                        {
                            avoided = true;
                        }
                    }
                }

                if (avoided && !preferred)
                    continue;

                bool conflict = false;
                for (auto &otherSection : sections)
                {
                    if (otherSection->teacher == teacher)
                    {
                        for (const auto &otherSlot : otherSection->timeSlots)
                        {
                            if (slot.overlaps(otherSlot))
                                conflict = true;
                        }
                    }
                    if (conflict)
                        break;
                }

                if (!conflict)
                    validSlots.push_back(slot);
            }

            if (validSlots.empty())
                continue;
            std::shuffle(validSlots.begin(), validSlots.end(), generator);

            for (const auto &slot : validSlots)
            {
                TimeSlot extendedSlot = slot;
                bool valid = true;

                for (int i = 1; i < section->course->creditHours; i++)
                {
                    extendedSlot.duration++;
                    if (extendedSlot.hour + extendedSlot.duration > 18)
                    {
                        valid = false;
                        break;
                    }

                    for (auto &otherSection : sections)
                    {
                        if (otherSection->teacher == teacher)
                        {
                            for (const auto &otherSlot : otherSection->timeSlots)
                            {
                                if (extendedSlot.overlaps(otherSlot))
                                {
                                    valid = false;
                                    break;
                                }
                            }
                        }
                        if (!valid)
                            break;
                    }
                    if (!valid)
                        break;
                }

                if (valid)
                {
                    section->assignTeacher(teacher);
                    section->addTimeSlot(extendedSlot);
                    assigned = true;
                    break;
                }
            }
            if (assigned)
                break;
        }
    }
    return validateSchedule();
}

bool ClassScheduler::validateSchedule() const
{
    for (const auto &section : sections)
    {
        if (!section->teacher || section->timeSlots.empty())
            return false;

        for (const auto &otherSection : sections)
        {
            if (section == otherSection)
                continue;
            if (section->teacher != otherSection->teacher)
                continue;

            for (const auto &slot : section->timeSlots)
            {
                for (const auto &otherSlot : otherSection->timeSlots)
                {
                    if (slot.overlaps(otherSlot))
                        return false;
                }
            }
        }
    }
    return true;
}

float ClassScheduler::evaluateSchedule() const
{
    if (!validateSchedule())
        return 0.0f;

    float totalScore = 0.0f;
    float maxScore = 0.0f;

    for (const auto &section : sections)
    {
        for (const auto &preference : preferences)
        {
            if (preference.courseCode != section->course->code)
                continue;

            maxScore += preference.weight;

            switch (preference.type)
            {
            case StudentPreference::PREFER_TEACHER:
                if (section->teacher && section->teacher->id == preference.teacherId)
                {
                    totalScore += preference.weight;
                }
                break;

            case StudentPreference::AVOID_TEACHER:
                if (!section->teacher || section->teacher->id != preference.teacherId)
                {
                    totalScore += preference.weight;
                }
                break;

            case StudentPreference::PREFER_TIME_SLOT:
                for (const auto &slot : section->timeSlots)
                {
                    if (slot.day == preference.timeSlot.day && slot.hour == preference.timeSlot.hour)
                    {
                        totalScore += preference.weight;
                        break;
                    }
                }
                break;

            case StudentPreference::AVOID_TIME_SLOT:
                bool conflict = false;
                for (const auto &slot : section->timeSlots)
                {
                    if (slot.day == preference.timeSlot.day && slot.hour == preference.timeSlot.hour)
                    {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict)
                    totalScore += preference.weight;
                break;
            }
        }
    }

    return maxScore > 0.0f ? (totalScore / maxScore) : 1.0f;
}

void ClassScheduler::applyConstraints()
{
    for (const auto &course : courses)
    {
        if (course->creditHours <= 1)
            continue;

        for (int day = 0; day < 5; day++)
        {
            for (int hour = 8; hour <= 18 - course->creditHours; hour++)
            {
                std::set<std::string> slots;
                for (int offset = 0; offset < course->creditHours; offset++)
                {
                    std::stringstream ss;
                    ss << "ts_" << day << "_" << (hour + offset);
                    slots.insert(ss.str());
                }
                if (scheduleTree.reduce(slots))
                    return;
            }
        }
    }
}

std::vector<std::shared_ptr<Section>> ClassScheduler::getSchedule() const
{
    std::vector<std::shared_ptr<Section>> validSections;
    for (const auto &section : sections)
    {
        if (section->teacher && !section->timeSlots.empty())
        {
            validSections.push_back(section);
        }
    }
    return validSections;
}