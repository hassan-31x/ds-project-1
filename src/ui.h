#pragma once

#include "scheduler.h"
// #include "raylib.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>


#include "..\raylib\include\raylib.h" // windows
// #include "/usr/local/opt/raylib/include/raylib.h" // macos
// #include "/opt/homebrew/include/raylib.h" // macos

// UI Colors
#define COLOR_BG         (Color){245, 245, 245, 255}
#define COLOR_PRIMARY    (Color){0, 120, 212, 255}
#define COLOR_SECONDARY  (Color){0, 90, 158, 255}
#define COLOR_ACCENT     (Color){255, 185, 0, 255}
#define COLOR_TEXT       (Color){51, 51, 51, 255}
#define COLOR_LIGHT_TEXT (Color){255, 255, 255, 255}
#define COLOR_BORDER     (Color){200, 200, 200, 255}
#define COLOR_HIGHLIGHT  (Color){230, 240, 250, 255}
#define COLOR_ERROR      (Color){209, 52, 56, 255}
#define COLOR_SUCCESS    (Color){16, 124, 16, 255}

// UI Constants
const int SCREEN_WIDTH       = 1280;
const int SCREEN_HEIGHT      = 720;
const int CELL_HEIGHT        = 70;
const int TIME_HEADER_HEIGHT = 40;
const int DAY_HEADER_WIDTH   = 120;
const int PADDING            = 10;
const int COURSE_LIST_WIDTH  = 300;
const int FONT_SIZE          = 20;
const int HEADER_FONT_SIZE   = 24;

// UI Components
enum class Tab {
    COURSES,
    TEACHERS,
    SECTIONS,
    SCHEDULE,
    PREFERENCES
};

// Button structure
struct Button {
    Rectangle   rect;
    std::string text;
    Color       color;
    bool        isToggle;
    bool        isToggled;

    Button(float x, float y, float width, float height, const std::string& txt,
           Color col);
    bool isClicked();
    void draw();
};

// Input Field structure
struct InputField {
    Rectangle   rect;
    std::string label;
    std::string text;
    bool        isActive;
    int         maxLength;

    InputField(float x, float y, float width, float height,
               const std::string& lbl, int maxLen = 50);
    void        update();
    void        draw();
    std::string getValue() const;
};

// Dropdown structure
struct Dropdown {
    Rectangle                rect;
    std::string              label;
    std::vector<std::string> options;
    int                      selectedIndex;
    bool                     isOpen;

    Dropdown(float x, float y, float width, float height,
             const std::string& lbl, const std::vector<std::string>& opts);
    void        update();
    void        draw();
    void        drawOptions();
    int         getSelectedIndex() const;
    std::string getSelectedOption() const;
};

// ScheduleUI class
class ScheduleUI {
private:

    // Data
    std::shared_ptr<ClassScheduler> scheduler;

    // UI State
    Tab                      currentTab;
    bool                     isDragging;
    std::shared_ptr<Section> draggedSection;
    Vector2                  dragOffset;

    // UI Components
    std::vector<Button> tabButtons;
    std::vector<Button> actionButtons;

    // Course Tab
    std::vector<InputField> courseInputs;
    Button                  addCourseButton;

    // Teacher Tab
    std::vector<InputField> teacherInputs;
    Button                  addTeacherButton;

    // Section Tab
    std::vector<InputField> sectionInputs;
    Dropdown                courseDropdown;
    Dropdown                teacherDropdown;
    Button                  addSectionButton;

    // Preference Tab
    std::vector<InputField> preferenceInputs;
    Dropdown                preferenceTypeDropdown;
    Dropdown                preferenceCourseDropdown;
    Dropdown                preferenceTeacherDropdown;
    std::vector<Button>     preferenceTimeButtons;
    Button                  addPreferenceButton;

    // Schedule View
    Rectangle scheduleViewport;

    // Helper Methods
    void drawCoursesTab();
    void drawTeachersTab();
    void drawSectionsTab();
    void drawScheduleTab();
    void drawPreferencesTab();

    void drawScheduleGrid();
    void drawScheduleItems();
    void handleScheduleDrag();

    void addCourse();
    void addTeacher();
    void addSection();
    void addPreference();

    void generateSchedule();

public:

    ScheduleUI();
    ~ScheduleUI();

    void initialize();
    void run();
    void shutdown();

    void update();
    void draw();

    void loadDemoData();
};