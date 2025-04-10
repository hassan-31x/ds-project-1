#include "ui.h"
#include <algorithm>
#include <iostream>
#include <sstream>

Button::Button(float x, float y, float width, float height,
               const std::string &txt, Color col)
    : rect{x, y, width, height},
      text(txt),
      color(col),
      isToggle(false),
      isToggled(false) {}

// Check if button was clicked by user
bool Button::isClicked()
{
    Vector2 mousePosition = GetMousePosition();
    bool mouseIsOver = CheckCollisionPointRec(mousePosition, rect);

    if (mouseIsOver && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        if (isToggle)
        {
            isToggled = !isToggled; // Flip toggle state
        }
        return true; // Click registered
    }
    return false; // No click
}

// Draw button on screen
void Button::draw()
{
    // Determine button colors based on state
    Color bgColor = color;
    Color textColor = COLOR_LIGHT_TEXT;

    if (isToggle && isToggled)
    {
        bgColor = COLOR_SECONDARY;
    }
    else if (CheckCollisionPointRec(GetMousePosition(), rect))
    {
        bgColor = ColorBrightness(color, 0.2f); // Lighten color on hover
    }

    // Draw button background and border
    DrawRectangleRec(rect, bgColor);
    DrawRectangleLinesEx(rect, 1, ColorAlpha(COLOR_BORDER, 0.5f));

    // Calculate centered text position
    Vector2 textSize =
        MeasureTextEx(GetFontDefault(), text.c_str(), FONT_SIZE, 1);
    float textX = rect.x + (rect.width - textSize.x) / 2;
    float textY = rect.y + (rect.height - textSize.y) / 2;

    // Display button text
    DrawTextEx(GetFontDefault(), text.c_str(), {textX, textY}, FONT_SIZE, 1,
               textColor);
}

InputField::InputField(float x, float y, float width, float height,
                       const std::string &lbl, int maxLen)
    : rect{x, y, width, height},
      label(lbl),
      text(""),
      isActive(false),
      maxLength(maxLen) {}

// Handle text input and focus
void InputField::update()
{
    Vector2 mousePosition = GetMousePosition();
    bool clickedInside = CheckCollisionPointRec(mousePosition, rect) &&
                         IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    // Toggle active state based on clicks
    if (clickedInside)
    {
        isActive = true;
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        isActive = false;
    }

    // Process text input when active
    if (isActive)
    {
        // Add new characters
        int pressedKey = GetCharPressed();
        if (pressedKey > 0 && text.length() < maxLength)
        {
            text += static_cast<char>(pressedKey);
        }

        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
        {
            text.pop_back();
        }
    }
}

// Draw input field and label
void InputField::draw()
{
    // Determine colors based on state
    Color bgColor = isActive ? COLOR_HIGHLIGHT : COLOR_BG;
    Color borderColor = isActive ? COLOR_PRIMARY : COLOR_BORDER;

    // Highlight on hover when not active
    if (!isActive && CheckCollisionPointRec(GetMousePosition(), rect))
    {
        bgColor = ColorBrightness(COLOR_BG, 0.8f);
    }

    // Draw background and border
    DrawRectangleRec(rect, bgColor);
    DrawRectangleLinesEx(rect, 1, borderColor);

    // Display label above field
    DrawTextEx(GetFontDefault(), label.c_str(), {rect.x, rect.y - 20},
               FONT_SIZE, 1, COLOR_TEXT);

    // Show input text
    Vector2 textPosition = {rect.x + 5, rect.y + (rect.height - FONT_SIZE) / 2};
    DrawTextEx(GetFontDefault(), text.c_str(), textPosition, FONT_SIZE, 1,
               COLOR_TEXT);

    // Draw blinking cursor when active
    if (isActive)
    {
        float textWidth =
            MeasureTextEx(GetFontDefault(), text.c_str(), FONT_SIZE, 1).x;
        Vector2 cursorPos = {rect.x + 5 + textWidth, textPosition.y};
        DrawRectangleV(cursorPos, {2, FONT_SIZE}, COLOR_TEXT);
    }
}

std::string InputField::getValue() const { return text; }

Dropdown::Dropdown(float x, float y, float width, float height,
                   const std::string &lbl, const std::vector<std::string> &opts)
    : rect{x, y, width, height},
      label(lbl),
      options(opts),
      selectedIndex(0),
      isOpen(false) {}

// Handle dropdown selection
void Dropdown::update()
{
    Vector2 mousePosition = GetMousePosition();

    // Toggle dropdown visibility
    if (CheckCollisionPointRec(mousePosition, rect))
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            isOpen = !isOpen;
        }
    }
    else if (isOpen)
    {
        // Check option selection
        bool optionSelected = false;
        for (int i = 0; i < options.size(); i++)
        {
            Rectangle optionRect = {rect.x,
                                    rect.y + rect.height + i * FONT_SIZE * 1.5f,
                                    rect.width, FONT_SIZE * 1.5f};

            if (CheckCollisionPointRec(mousePosition, optionRect))
            {
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    selectedIndex = i;
                    isOpen = false;
                    optionSelected = true;
                    break;
                }
            }
        }

        // Close dropdown if clicked outside
        if (!optionSelected && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            isOpen = false;
        }
    }
}

// Draw dropdown and options
void Dropdown::draw()
{
    // Determine colors based on state
    Color bgColor = COLOR_BG;
    Color borderColor = COLOR_BORDER;

    if (CheckCollisionPointRec(GetMousePosition(), rect))
    {
        bgColor = ColorBrightness(COLOR_BG, 0.8f);
    }

    // Draw main dropdown box
    DrawRectangleRec(rect, bgColor);
    DrawRectangleLinesEx(rect, 1, borderColor);

    // Display label
    DrawTextEx(GetFontDefault(), label.c_str(), {rect.x, rect.y - 20},
               FONT_SIZE, 1, COLOR_TEXT);

    // Show selected option
    std::string displayText =
        (selectedIndex >= 0 && selectedIndex < options.size())
            ? options[selectedIndex]
            : "Select...";
    DrawTextEx(GetFontDefault(), displayText.c_str(),
               {rect.x + 5, rect.y + (rect.height - FONT_SIZE) / 2}, FONT_SIZE,
               1, COLOR_TEXT);

    // Draw dropdown arrow
    DrawTriangle({rect.x + rect.width - 15, rect.y + rect.height / 3},
                 {rect.x + rect.width - 5, rect.y + rect.height / 3},
                 {rect.x + rect.width - 10, rect.y + 2 * rect.height / 3},
                 COLOR_TEXT);

    // Display options when open
    if (isOpen)
    {
        for (int i = 0; i < options.size(); i++)
        {
            Rectangle optionRect = {rect.x,
                                    rect.y + rect.height + i * FONT_SIZE * 1.5f,
                                    rect.width, FONT_SIZE * 1.5f};

            // Determine option colors
            Color optionBg = (i == selectedIndex) ? COLOR_PRIMARY : COLOR_BG;
            Color optionTextColor =
                (i == selectedIndex) ? COLOR_LIGHT_TEXT : COLOR_TEXT;

            if (CheckCollisionPointRec(GetMousePosition(), optionRect))
            {
                optionBg = (i == selectedIndex)
                               ? ColorBrightness(COLOR_PRIMARY, 0.2f)
                               : COLOR_HIGHLIGHT;
            }

            // Draw option background and text
            DrawRectangleRec(optionRect, optionBg);
            DrawRectangleLinesEx(optionRect, 1, COLOR_BORDER);
            DrawTextEx(GetFontDefault(), options[i].c_str(),
                       {optionRect.x + 5,
                        optionRect.y + (optionRect.height - FONT_SIZE) / 2},
                       FONT_SIZE, 1, optionTextColor);
        }
    }
}

int Dropdown::getSelectedIndex() const { return selectedIndex; }

std::string Dropdown::getSelectedOption() const
{
    return (selectedIndex >= 0 && selectedIndex < options.size())
               ? options[selectedIndex]
               : "";
}

ScheduleUI::ScheduleUI()
    : scheduler(std::make_shared<ClassScheduler>()),
      currentTab(Tab::COURSES),
      isDragging(false),
      draggedSection(nullptr),
      dragOffset({0, 0}),
      addCourseButton(0, 0, 0, 0, "", BLACK),
      addTeacherButton(0, 0, 0, 0, "", BLACK),
      addSectionButton(0, 0, 0, 0, "", BLACK),
      courseDropdown(0, 0, 0, 0, "", {}),
      teacherDropdown(0, 0, 0, 0, "", {}),
      preferenceTypeDropdown(0, 0, 0, 0, "", {}),
      preferenceCourseDropdown(0, 0, 0, 0, "", {}),
      preferenceTeacherDropdown(0, 0, 0, 0, "", {}),
      addPreferenceButton(0, 0, 0, 0, "", BLACK) {}

ScheduleUI::~ScheduleUI() { scheduler.reset(); }

void ScheduleUI::initialize()
{
    // Set up tab buttons
    float tabWidth = SCREEN_WIDTH / 4.0f;
    tabButtons.push_back(Button(0, 0, tabWidth, 40, "Courses", COLOR_PRIMARY));
    tabButtons.push_back(
        Button(tabWidth, 0, tabWidth, 40, "Teachers", COLOR_PRIMARY));
    tabButtons.push_back(
        Button(tabWidth * 2, 0, tabWidth, 40, "Schedule", COLOR_PRIMARY));
    tabButtons.push_back(
        Button(tabWidth * 3, 0, tabWidth, 40, "Preferences", COLOR_PRIMARY));

    // Activate first tab
    tabButtons[0].isToggle = true;
    tabButtons[0].isToggled = true;

    // Create Generate Schedule button
    actionButtons.push_back(Button(SCREEN_WIDTH - 200, SCREEN_HEIGHT - 50, 180,
                                   40, "Generate Schedule", COLOR_ACCENT));

    // Initialize course input fields
    courseInputs.push_back(
        InputField(PADDING, 80, 300, 40, "Course Code (e.g., CS101)"));
    courseInputs.push_back(InputField(PADDING, 150, 300, 40, "Course Title"));
    courseInputs.push_back(InputField(PADDING, 220, 300, 40, "Credit Hours"));
    addCourseButton =
        Button(PADDING, 290, 300, 40, "Add Course", COLOR_PRIMARY);

    // Load sample data for demonstration
    loadDemoData();
}

void ScheduleUI::run()
{
    // Initialize application window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Class Scheduler");
    SetTargetFPS(60);

    initialize();

    // Main application loop
    while (!WindowShouldClose())
    {
        update();

        BeginDrawing();
        ClearBackground(COLOR_BG);

        draw();

        EndDrawing();
    }

    shutdown();
}

void ScheduleUI::shutdown() { CloseWindow(); }

// Update UI state each frame
void ScheduleUI::update()
{
    // Handle tab switching
    for (size_t i = 0; i < tabButtons.size(); i++)
    {
        if (tabButtons[i].isClicked())
        {
            currentTab = static_cast<Tab>(i);
            // Update button states
            for (auto &button : tabButtons)
                button.isToggled = false;
            tabButtons[i].isToggled = true;
        }
    }

    // Check Generate Schedule button
    if (actionButtons[0].isClicked())
    {
        generateSchedule();
    }

    // Update current tab components
    switch (currentTab)
    {
    case Tab::COURSES:
        for (auto &input : courseInputs)
            input.update();
        if (addCourseButton.isClicked())
            addCourse();
        break;

    case Tab::TEACHERS:
        for (auto &input : teacherInputs)
            input.update();
        if (addTeacherButton.isClicked())
            addTeacher();
        break;

    case Tab::SCHEDULE:
        handleScheduleDrag();
        break;

    case Tab::PREFERENCES:
        preferenceTypeDropdown.update();
        preferenceCourseDropdown.update();
        preferenceTeacherDropdown.update();
        if (addPreferenceButton.isClicked())
            addPreference();
        break;
    }

    // Update dropdown options
    std::vector<std::string> courseOptions;
    for (const auto &course : scheduler->getCourses())
    {
        courseOptions.push_back(course->code + ": " + course->title);
    }

    std::vector<std::string> teacherOptions;
    for (const auto &teacher : scheduler->getTeachers())
    {
        teacherOptions.push_back(teacher->id + ": " + teacher->name);
    }

    // Refresh dropdown contents
    courseDropdown.options = courseOptions;
    teacherDropdown.options = teacherOptions;
    preferenceCourseDropdown.options = courseOptions;
    preferenceTeacherDropdown.options = teacherOptions;
}

// Draw entire UI
void ScheduleUI::draw()
{
    // Draw tab buttons
    for (auto &button : tabButtons)
    {
        button.draw();
    }

    // Draw current tab content
    switch (currentTab)
    {
    case Tab::COURSES:
        drawCoursesTab();
        break;
    case Tab::TEACHERS:
        drawTeachersTab();
        break;
    case Tab::SCHEDULE:
        drawScheduleTab();
        break;
    case Tab::PREFERENCES:
        drawPreferencesTab();
        break;
    }

    // Draw action buttons
    for (auto &button : actionButtons)
    {
        button.draw();
    }
}

void ScheduleUI::drawCoursesTab()
{
    // Draw title
    DrawTextEx(GetFontDefault(), "Courses", {PADDING, 50}, HEADER_FONT_SIZE, 1,
               COLOR_TEXT);

    // Draw input fields
    for (auto &input : courseInputs)
    {
        input.draw();
    }

    // Draw Add Course button
    addCourseButton.draw();

    // List existing courses
    DrawTextEx(GetFontDefault(), "Existing Courses:", {PADDING, 350}, FONT_SIZE,
               1, COLOR_TEXT);
    int yPos = 380;
    for (const auto &course : scheduler->getCourses())
    {
        std::string courseInfo = course->code + ": " + course->title + " (" +
                                 std::to_string(course->creditHours) +
                                 " credits)";
        DrawTextEx(GetFontDefault(), courseInfo.c_str(), {PADDING, (float)yPos},
                   FONT_SIZE, 1, COLOR_TEXT);
        yPos += FONT_SIZE + 5;
    }
}

void ScheduleUI::drawTeachersTab()
{
    // Draw title
    DrawTextEx(GetFontDefault(), "Teachers", {PADDING, 50}, HEADER_FONT_SIZE, 1,
               COLOR_TEXT);

    // Draw input fields
    for (auto &input : teacherInputs)
    {
        input.draw();
    }

    // Draw Add Teacher button
    addTeacherButton.draw();

    // List existing teachers
    DrawTextEx(GetFontDefault(), "Existing Teachers:", {PADDING, 350},
               FONT_SIZE, 1, COLOR_TEXT);
    int yPos = 380;
    for (const auto &teacher : scheduler->getTeachers())
    {
        std::string teacherInfo = teacher->id + ": " + teacher->name;
        DrawTextEx(GetFontDefault(), teacherInfo.c_str(),
                   {PADDING, (float)yPos}, FONT_SIZE, 1, COLOR_TEXT);
        yPos += FONT_SIZE + 5;
    }
}

void ScheduleUI::drawScheduleTab()
{
    // Draw title
    DrawTextEx(GetFontDefault(), "Class Schedule", {PADDING, 50},
               HEADER_FONT_SIZE, 1, COLOR_TEXT);

    // Draw schedule components
    drawScheduleGrid();
    drawScheduleItems();
}

void ScheduleUI::drawPreferencesTab()
{
    // Draw title
    DrawTextEx(GetFontDefault(), "Preferences", {PADDING, 50}, HEADER_FONT_SIZE,
               1, COLOR_TEXT);

    // Draw dropdowns
    preferenceTypeDropdown.draw();
    preferenceCourseDropdown.draw();

    // Show relevant components based on preference type
    if (preferenceTypeDropdown.getSelectedIndex() == 0 ||
        preferenceTypeDropdown.getSelectedIndex() == 2)
    {
        preferenceTeacherDropdown.draw();
    }
    else
    {
        // Draw time slot buttons
        DrawTextEx(GetFontDefault(), "Select Time Slot:", {PADDING, 270},
                   FONT_SIZE, 1, COLOR_TEXT);
        for (auto &button : preferenceTimeButtons)
        {
            button.draw();
        }
    }

    // Draw Add Preference button
    addPreferenceButton.draw();
}

void ScheduleUI::drawScheduleGrid()
{
    // Draw time column headers
    for (int hour = 8; hour <= 17; hour++)
    {
        std::string timeLabel = std::to_string(hour) + ":00";
        float yPos = TIME_HEADER_HEIGHT + (hour - 8) * CELL_HEIGHT;

        DrawRectangle(0, yPos, DAY_HEADER_WIDTH, CELL_HEIGHT, COLOR_HIGHLIGHT);
        DrawRectangleLines(0, yPos, DAY_HEADER_WIDTH, CELL_HEIGHT,
                           COLOR_BORDER);

        // Center text in time cell
        Vector2 textSize =
            MeasureTextEx(GetFontDefault(), timeLabel.c_str(), FONT_SIZE, 1);
        DrawTextEx(GetFontDefault(), timeLabel.c_str(),
                   {(DAY_HEADER_WIDTH - textSize.x) / 2,
                    yPos + (CELL_HEIGHT - textSize.y) / 2},
                   FONT_SIZE, 1, COLOR_TEXT);
    }

    // Draw day row headers
    const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
    float dayWidth = (SCREEN_WIDTH - DAY_HEADER_WIDTH) / 5;
    for (int day = 0; day < 5; day++)
    {
        float xPos = DAY_HEADER_WIDTH + day * dayWidth;

        DrawRectangle(xPos, 0, dayWidth, TIME_HEADER_HEIGHT, COLOR_HIGHLIGHT);
        DrawRectangleLines(xPos, 0, dayWidth, TIME_HEADER_HEIGHT, COLOR_BORDER);

        // Center text in day header
        Vector2 textSize =
            MeasureTextEx(GetFontDefault(), days[day], FONT_SIZE, 1);
        DrawTextEx(GetFontDefault(), days[day],
                   {xPos + (dayWidth - textSize.x) / 2,
                    (TIME_HEADER_HEIGHT - textSize.y) / 2},
                   FONT_SIZE, 1, COLOR_TEXT);
    }

    // Draw grid lines
    for (int day = 0; day < 5; day++)
    {
        for (int hour = 8; hour <= 17; hour++)
        {
            float xPos = DAY_HEADER_WIDTH + day * dayWidth;
            float yPos = TIME_HEADER_HEIGHT + (hour - 8) * CELL_HEIGHT;
            DrawRectangleLines(xPos, yPos, dayWidth, CELL_HEIGHT, COLOR_BORDER);
        }
    }
}

void ScheduleUI::drawScheduleItems()
{
    float dayWidth = (SCREEN_WIDTH - DAY_HEADER_WIDTH) / 5;

    // Draw all scheduled sections
    for (const auto &section : scheduler->getSections())
    {
        if (!section->teacher || section->timeSlots.empty())
            continue;

        // Draw each time slot for the section
        for (const auto &slot : section->timeSlots)
        {
            float xPos = DAY_HEADER_WIDTH + slot.day * dayWidth;
            float yPos = TIME_HEADER_HEIGHT + (slot.hour - 8) * CELL_HEIGHT;
            float height = slot.duration * CELL_HEIGHT;

            // Draw section block
            DrawRectangle(xPos + 1, yPos + 1, dayWidth - 2, height - 2,
                          COLOR_PRIMARY);

            // Create section info text
            std::string sectionInfo =
                section->course->code + " (" + section->id + ")\n" +
                section->teacher->name + "\n" + slot.toString();

            // Draw section text
            DrawTextEx(GetFontDefault(), sectionInfo.c_str(),
                       {xPos + 5, yPos + 5}, FONT_SIZE, 1, COLOR_LIGHT_TEXT);
        }
    }
}

void ScheduleUI::addTeacher()
{
    if (teacherInputs.size() < 2)
        return;

    std::string teacherID = teacherInputs[0].getValue();
    std::string teacherName = teacherInputs[1].getValue();

    if (!teacherID.empty() && !teacherName.empty())
    {
        auto newTeacher = std::make_shared<Teacher>(teacherID, teacherName);

        // Set default availability (Mon-Fri 8am-5pm)
        for (int day = 0; day < 5; day++)
        {
            for (int hour = 8; hour <= 16; hour++)
            {
                TimeSlot slot;
                slot.day = day;
                slot.hour = hour;
                slot.duration = 1;
                newTeacher->addAvailableTimeSlot(slot);
            }
        }

        scheduler->addTeacher(newTeacher);
        // Clear input fields
        for (auto &input : teacherInputs)
            input.text.clear();
    }
}

void ScheduleUI::addSection()
{
    std::string sectionID = sectionInputs[0].getValue();
    int courseIndex = courseDropdown.getSelectedIndex();
    int teacherIndex = teacherDropdown.getSelectedIndex();

    if (sectionID.empty() || courseIndex < 0 || teacherIndex < 0)
        return;

    auto selectedCourse = scheduler->getCourses()[courseIndex];
    auto selectedTeacher = scheduler->getTeachers()[teacherIndex];

    // Ensure teacher is assigned to course
    if (std::find(selectedCourse->assignedTeachers.begin(),
                  selectedCourse->assignedTeachers.end(),
                  selectedTeacher) == selectedCourse->assignedTeachers.end())
    {
        selectedCourse->assignTeacher(selectedTeacher);
    }

    // Create and add new section
    auto newSection = std::make_shared<Section>(sectionID, selectedCourse);
    newSection->assignTeacher(selectedTeacher);
    scheduler->addSection(newSection);

    // Clear section ID field
    sectionInputs[0].text.clear();
}

void ScheduleUI::addPreference()
{
    int prefType = preferenceTypeDropdown.getSelectedIndex();
    int courseIndex = preferenceCourseDropdown.getSelectedIndex();

    if (courseIndex < 0)
        return;

    StudentPreference newPref;
    newPref.courseCode = scheduler->getCourses()[courseIndex]->code;

    switch (prefType)
    {
    case 0: // Prefer Teacher
    case 2: // Avoid Teacher
    {
        int teacherIndex = preferenceTeacherDropdown.getSelectedIndex();
        if (teacherIndex >= 0)
        {
            newPref.type =
                static_cast<StudentPreference::PreferenceType>(prefType);
            newPref.teacherId = scheduler->getTeachers()[teacherIndex]->id;
            scheduler->addPreference(newPref);
        }
    }
    break;

    case 1: // Prefer Time
    case 3: // Avoid Time
    {
        for (size_t i = 0; i < preferenceTimeButtons.size(); i++)
        {
            if (preferenceTimeButtons[i].isToggled)
            {
                newPref.type =
                    static_cast<StudentPreference::PreferenceType>(
                        prefType);
                newPref.timeSlot.day = i / 10;
                newPref.timeSlot.hour = (i % 10) + 8;
                newPref.timeSlot.duration = 1;
                scheduler->addPreference(newPref);
                preferenceTimeButtons[i].isToggled = false;
            }
        }
    }
    break;
    }
}

void ScheduleUI::generateSchedule()
{
    bool success = scheduler->generateSchedule();

    if (success)
    {
        // Switch to schedule tab
        currentTab = Tab::SCHEDULE;
        for (auto &button : tabButtons)
            button.isToggled = false;
        tabButtons[static_cast<int>(Tab::SCHEDULE)].isToggled = true;
    }
}

void ScheduleUI::handleScheduleDrag()
{
    float dayWidth = (SCREEN_WIDTH - DAY_HEADER_WIDTH) / 5;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();

        // Check if clicking a schedule block
        for (const auto &section : scheduler->getSections())
        {
            for (const auto &slot : section->timeSlots)
            {
                float xPos = DAY_HEADER_WIDTH + slot.day * dayWidth;
                float yPos = TIME_HEADER_HEIGHT + (slot.hour - 8) * CELL_HEIGHT;
                Rectangle slotRect = {
                    xPos, yPos, dayWidth,
                    CELL_HEIGHT * static_cast<float>(slot.duration)};

                if (CheckCollisionPointRec(mousePos, slotRect))
                {
                    isDragging = true;
                    draggedSection = section;
                    dragOffset = {mousePos.x - xPos, mousePos.y - yPos};
                    return;
                }
            }
        }
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && isDragging)
    {
        // Update section position
        Vector2 mousePos = GetMousePosition();
        int newDay = (mousePos.x - DAY_HEADER_WIDTH) / dayWidth;
        int newHour = (mousePos.y - TIME_HEADER_HEIGHT) / CELL_HEIGHT + 8;

        // Validate boundaries
        newDay = std::clamp(newDay, 0, 4);
        newHour = std::clamp(newHour, 8, 17);

        if (draggedSection && !draggedSection->timeSlots.empty())
        {
            // Update first time slot (simplified example)
            draggedSection->timeSlots[0].day = newDay;
            draggedSection->timeSlots[0].hour = newHour;
        }

        isDragging = false;
        draggedSection = nullptr;
    }
}

void ScheduleUI::addCourse()
{
    if (courseInputs.size() >= 3)
    {
        std::string code = courseInputs[0].getValue();
        std::string title = courseInputs[1].getValue();
        int credits = 3; // Default

        try
        {
            credits = std::stoi(courseInputs[2].getValue());
        }
        catch (...)
        {
            // Use default value if parsing fails
        }

        if (!code.empty() && !title.empty())
        {
            auto course = std::make_shared<Course>(code, title, credits);
            scheduler->addCourse(course);

            // Clear inputs
            for (auto &input : courseInputs)
            {
                input.text.clear();
            }
        }
    }
}

void ScheduleUI::loadDemoData()
{
    // Sample courses
    auto cs101 = std::make_shared<Course>("CS101", "Intro to Programming", 3);
    auto math201 = std::make_shared<Course>("MATH201", "Calculus II", 4);
    scheduler->addCourse(cs101);
    scheduler->addCourse(math201);

    // Sample teachers
    auto teacher1 = std::make_shared<Teacher>("T1", "Dr. Smith");
    auto teacher2 = std::make_shared<Teacher>("T2", "Prof. Johnson");

    // Set available times
    for (int day = 0; day < 5; day++)
    {
        for (int hour = 8; hour <= 16; hour++)
        {
            TimeSlot slot;
            slot.day = day;
            slot.hour = hour;
            slot.duration = 1;
            teacher1->addAvailableTimeSlot(slot);
            teacher2->addAvailableTimeSlot(slot);
        }
    }
    scheduler->addTeacher(teacher1);
    scheduler->addTeacher(teacher2);

    // Assign teachers to courses
    cs101->assignTeacher(teacher1);
    cs101->assignTeacher(teacher2);
    math201->assignTeacher(teacher1);

    // Create demo sections
    auto sectionA = std::make_shared<Section>("A", cs101);
    sectionA->assignTeacher(teacher1);
    scheduler->addSection(sectionA);

    auto sectionB = std::make_shared<Section>("B", math201);
    sectionB->assignTeacher(teacher1);
    scheduler->addSection(sectionB);

    // Generate initial schedule
    scheduler->generateSchedule();
}