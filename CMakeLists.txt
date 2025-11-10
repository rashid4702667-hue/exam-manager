cmake_minimum_required(VERSION 3.16)

# Add MinGW to the path
set(MINGW_PATH "C:/Qt/Tools/mingw1310_64/bin")
set(ENV{PATH} "${MINGW_PATH};$ENV{PATH}")

# Set Qt paths
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.0/mingw_64")
set(Qt6_DIR "C:/Qt/6.10.0/mingw_64/lib/cmake/Qt6")

project(TimetablePlannerGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.0/mingw_64")
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets
    Charts
)

# Try to find DataVisualization, but make it optional
find_package(Qt6 QUIET COMPONENTS DataVisualization)

# Add PrintSupport for PDF export
find_package(Qt6 REQUIRED COMPONENTS PrintSupport)

# Enable Qt features
qt_standard_project_setup()

# Set Windows specific configuration
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

# Set autouic search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6Gui_INCLUDE_DIRS})
include_directories(${Qt6Widgets_INCLUDE_DIRS})

# Find Qt package components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)
find_package(Qt6 QUIET COMPONENTS DataVisualization)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/connectdialog.cpp
    src/generatetimetabledialog.cpp
    src/scheduletablemodel.cpp
    src/DatabaseManager.cpp
    src/TimetableGenerator.cpp
    src/analyticswindow.cpp
)

# Header files
set(HEADERS
    include/mainwindow.h
    include/connectdialog.h
    include/generatetimetabledialog.h
    include/scheduletablemodel.h
    include/analyticswindow.h
)

# UI files
set(UI_FILES
    ui/mainwindow.ui
    ui/connectdialog.ui
    ui/generatetimetabledialog.ui
    ui/analyticswindow.ui
)

# Create executable
qt_add_executable(TimetablePlannerGUI
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Link Qt libraries
target_link_libraries(TimetablePlannerGUI PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::PrintSupport
    Qt::Charts
)

# Link DataVisualization if available
if(TARGET Qt::DataVisualization)
    target_link_libraries(TimetablePlannerGUI PRIVATE Qt::DataVisualization)
    target_compile_definitions(TimetablePlannerGUI PRIVATE HAVE_DATAVISUALIZATION)
endif()

# Set the UI directory for the target
set_target_properties(TimetablePlannerGUI PROPERTIES
    AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# Include build directory for generated UI headers
target_include_directories(TimetablePlannerGUI PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# Handle deployment
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
    install(TARGETS TimetablePlannerGUI RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}")
    
    # Use windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${CMAKE_PREFIX_PATH}/bin")
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET TimetablePlannerGUI POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --verbose 0
                    --no-compiler-runtime
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:TimetablePlannerGUI>"
            COMMENT "Deploying Qt dependencies"
        )
    endif()
endif()