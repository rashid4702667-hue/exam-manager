# Change Test Date Feature

## Overview
This feature allows users to change the test date of any course in the generated timetable to any other date. The feature is accessible through the Schedule menu and toolbar.

## How to Use

### Prerequisites
1. Connect to a database OR import CSV data
2. Generate a timetable first

### Steps to Change a Test Date

1. **Access the Feature**
   - Go to `Schedule` → `Change Test Date` (Ctrl+T)
   - Or click the "Change Test Date" button in the toolbar

2. **View Available Courses**
   - A dialog will display all courses with their current test dates
   - Format: `ID: COURSE_ID - Date: DD-MM-YYYY`
   - Example: `ID: ML101 - Date: 02-01-2026`

3. **Select Course**
   - Enter the Course ID of the course you want to reschedule
   - The system will validate if the course exists

4. **Enter New Date**
   - Enter the new date in DD-MM-YYYY format
   - The current date will be pre-filled for reference
   - Example: `15-01-2026`

5. **Confirmation**
   - The system will update the schedule and refresh the view
   - A success message will confirm the change

## Features

### Intelligent Date Handling
- Automatically calculates the correct day name (Monday, Tuesday, etc.) from the new date
- Updates day numbers relative to the start date
- Handles all schedule entries for courses with multiple sessions

### Error Handling
- Validates course ID exists
- Prevents changes if no schedule is generated
- Graceful error handling for invalid date formats

### User Experience
- Shows all available courses with current dates
- Pre-fills current date for easy reference
- Keyboard shortcut (Ctrl+T) for quick access
- Immediate visual feedback in the schedule table

## Technical Implementation

### Data Structure
The feature works with the existing schedule format:
```
Day,DayNumber,Date,CourseID,Students,Room,Capacity
```

### Methods Added
- `getCoursesWithDates()`: Returns all courses with their current dates
- `updateCourseDate()`: Updates all schedule entries for a specific course

### Date Calculation
- Uses `std::tm` and `std::mktime` for accurate date calculations
- Supports proper day-of-week calculation
- Handles leap years and month boundaries correctly

## Example Usage Scenario

1. User generates a timetable with ML101 scheduled on 02-01-2026
2. User realizes this conflicts with a holiday
3. User goes to Schedule → Change Test Date
4. System shows: "ID: ML101 - Date: 02-01-2026"
5. User enters "ML101" as course ID
6. User changes date to "05-01-2026"
7. System automatically updates:
   - Date: 02-01-2026 → 05-01-2026
   - Day: Friday → Monday
   - Day Number: Updated relative to start date
8. Schedule table refreshes with new information

## Benefits

- **Flexibility**: Easy to reschedule exams due to conflicts
- **Accuracy**: Automatic day calculation prevents errors
- **User-Friendly**: Simple dialog-based interface
- **Comprehensive**: Updates all related schedule entries
- **Real-time**: Immediate feedback and schedule updates

## Menu Location
- **Menu**: Schedule → Change Test Date
- **Keyboard Shortcut**: Ctrl+T
- **Toolbar**: Available in main toolbar
- **Availability**: Only enabled when a schedule is generated