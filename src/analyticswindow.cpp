#include "analyticswindow.h"
#include "ui_analyticswindow.h"
#include "DatabaseManager.h"
#include "TimetableGenerator.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <algorithm>
#include <numeric>

AnalyticsWindow::AnalyticsWindow(DatabaseManager* dbManager, TimetableGenerator* timetableGen, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnalyticsWindow)
    , m_databaseManager(dbManager)
    , m_timetableGenerator(timetableGen)
    , m_examLoadChartView(nullptr)
    , m_departmentChartView(nullptr)
    , m_roomUtilizationChartView(nullptr)
    , m_conflictChartView(nullptr)
    , m_examTimingChartView(nullptr)
#ifdef HAVE_DATAVISUALIZATION
    , m_roomLoad3DSurface(nullptr)
#endif
    , m_roomLoad3DWidget(nullptr)
    , m_studentDistributionChartView(nullptr)
{
    ui->setupUi(this);
    
    // Set window properties for better navigation
    setWindowTitle("📊 Exam Schedule Analytics Dashboard - Timetable Planner");
    setMinimumSize(1200, 800);
    
    // Set window flags to make it a proper dialog
    if (parent) {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        setAttribute(Qt::WA_DeleteOnClose, false);
    }
    
    // Connect signals
    connect(ui->refreshButton, &QPushButton::clicked, this, &AnalyticsWindow::onRefreshClicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &AnalyticsWindow::onExportClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &AnalyticsWindow::onCloseClicked);
    
    // Setup chart views
    setupChartViews();
    
    // Load initial data
    refreshAnalytics();
}

AnalyticsWindow::~AnalyticsWindow()
{
    delete ui;
    clearCharts();
}

void AnalyticsWindow::setDatabaseManager(DatabaseManager* dbManager)
{
    m_databaseManager = dbManager;
}

void AnalyticsWindow::setTimetableGenerator(TimetableGenerator* timetableGen)
{
    m_timetableGenerator = timetableGen;
}

void AnalyticsWindow::refreshAnalytics()
{
    // Clear existing charts
    clearCharts();
    
    // Load fresh data
    loadAnalyticsData();
    
    // Create all charts
    createExamLoadChart();
    createDepartmentDistributionChart();
    createRoomUtilizationChart();
    createConflictDetectionChart();
    createExamTimingChart();
    createRoomLoadOverDaysChart();
    createStudentExamDistributionChart();
}

void AnalyticsWindow::setupChartViews()
{
    // Initialize chart views with proper styling
    auto createStyledChartView = [](QChart* chart) -> QChartView* {
        auto chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setStyleSheet("QChartView { background-color: white; border: 1px solid #ddd; border-radius: 4px; }");
        return chartView;
    };
    
    // Will be created when charts are generated
}

void AnalyticsWindow::clearCharts()
{
    // Clean up existing chart views
    if (m_examLoadChartView) {
        m_examLoadChartView->deleteLater();
        m_examLoadChartView = nullptr;
    }
    if (m_departmentChartView) {
        m_departmentChartView->deleteLater();
        m_departmentChartView = nullptr;
    }
    if (m_roomUtilizationChartView) {
        m_roomUtilizationChartView->deleteLater();
        m_roomUtilizationChartView = nullptr;
    }
    if (m_conflictChartView) {
        m_conflictChartView->deleteLater();
        m_conflictChartView = nullptr;
    }
    if (m_examTimingChartView) {
        m_examTimingChartView->deleteLater();
        m_examTimingChartView = nullptr;
    }
    if (m_roomLoad3DWidget) {
        m_roomLoad3DWidget->deleteLater();
        m_roomLoad3DWidget = nullptr;
#ifdef HAVE_DATAVISUALIZATION
        m_roomLoad3DSurface = nullptr;
#endif
    }
    if (m_studentDistributionChartView) {
        m_studentDistributionChartView->deleteLater();
        m_studentDistributionChartView = nullptr;
    }
}

void AnalyticsWindow::loadAnalyticsData()
{
    if (m_databaseManager) {
        // Load data from database
        auto examSchedule = getExamScheduleFromDatabase();
        
        if (!examSchedule.empty()) {
            // Process real data
            processExamLoadData();
            processDepartmentDistributionData();
            processRoomUtilizationData();
            processConflictDetectionData();
            processExamTimingData();
            processRoomLoadOverDaysData();
            processStudentExamDistributionData();
        } else {
            // Fall back to sample data
            generateSampleData();
        }
    } else {
        // Use sample data for demonstration
        generateSampleData();
    }
}

void AnalyticsWindow::generateSampleData()
{
    m_analyticsData = createSampleAnalyticsData();
}

AnalyticsData AnalyticsWindow::createSampleAnalyticsData()
{
    AnalyticsData data;
    
    // Sample exam load data
    data.examCountPerDay["Monday"] = 5;
    data.examCountPerDay["Tuesday"] = 8;
    data.examCountPerDay["Wednesday"] = 4;
    data.examCountPerDay["Thursday"] = 6;
    data.examCountPerDay["Friday"] = 7;
    data.examCountPerDay["Saturday"] = 3;
    
    // Sample department distribution
    data.examCountPerDepartment["Computer Science"] = 15;
    data.examCountPerDepartment["Information Technology"] = 12;
    data.examCountPerDepartment["Electronics"] = 8;
    data.examCountPerDepartment["Mechanical"] = 6;
    data.examCountPerDepartment["Civil"] = 4;
    
    // Sample room utilization
    data.roomUtilizationPercentage["Room A101"] = 90.0;
    data.roomUtilizationPercentage["Room A102"] = 85.0;
    data.roomUtilizationPercentage["Room B201"] = 75.0;
    data.roomUtilizationPercentage["Room B202"] = 60.0;
    data.roomUtilizationPercentage["Room C301"] = 45.0;
    
    // Sample conflict data
    data.conflictsPerCourse["ML101"] = 2;
    data.conflictsPerCourse["AI401"] = 5;
    data.conflictsPerCourse["DSA201"] = 1;
    data.conflictsPerCourse["DB301"] = 3;
    data.conflictsPerCourse["OS401"] = 4;
    
    // Sample room load over days
    std::vector<std::string> rooms = {"Room A101", "Room A102", "Room B201", "Room B202", "Room C301"};
    std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    
    for (const auto& room : rooms) {
        for (const auto& day : days) {
            int load = QRandomGenerator::global()->bounded(20, 100);
            data.roomLoadPerDay[{room, day}] = load;
        }
    }
    
    // Sample student exam distribution
    data.studentExamCountDistribution[1] = 15;  // 15 students have 1 exam
    data.studentExamCountDistribution[2] = 25;  // 25 students have 2 exams
    data.studentExamCountDistribution[3] = 30;  // 30 students have 3 exams
    data.studentExamCountDistribution[4] = 20;  // 20 students have 4 exams
    data.studentExamCountDistribution[5] = 8;   // 8 students have 5 exams
    data.studentExamCountDistribution[6] = 2;   // 2 students have 6 exams
    
    return data;
}

void AnalyticsWindow::createExamLoadChart()
{
    // Create bar chart for exam load overview
    auto series = new QBarSeries();
    auto barSet = new QBarSet("Exams");
    
    QStringList categories;
    
    for (const auto& pair : m_analyticsData.examCountPerDay) {
        categories << QString::fromStdString(pair.first);
        *barSet << pair.second;
    }
    
    series->append(barSet);
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📊 Exams Scheduled per Day");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto axisY = new QValueAxis();
    axisY->setTitleText("Number of Exams");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    // Style the chart
    chart->legend()->setAlignment(Qt::AlignBottom);
    barSet->setColor(QColor(52, 152, 219));
    
    m_examLoadChartView = new QChartView(chart);
    m_examLoadChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->examLoadChartWidget->setLayout(new QVBoxLayout());
    ui->examLoadChartWidget->layout()->addWidget(m_examLoadChartView);
}

void AnalyticsWindow::createDepartmentDistributionChart()
{
    // Create pie chart for department distribution
    auto series = new QPieSeries();
    
    int total = 0;
    for (const auto& pair : m_analyticsData.examCountPerDepartment) {
        total += pair.second;
    }
    
    QStringList colors = {"#e74c3c", "#3498db", "#2ecc71", "#f39c12", "#9b59b6", "#1abc9c"};
    int colorIndex = 0;
    
    for (const auto& pair : m_analyticsData.examCountPerDepartment) {
        double percentage = (static_cast<double>(pair.second) / total) * 100.0;
        auto slice = series->append(QString::fromStdString(pair.first), pair.second);
        slice->setLabelVisible(true);
        slice->setLabel(QString("%1 (%2%)").arg(QString::fromStdString(pair.first)).arg(QString::number(percentage, 'f', 1)));
        slice->setColor(QColor(colors[colorIndex % colors.size()]));
        colorIndex++;
    }
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("🥧 Exam Distribution by Department");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);
    
    m_departmentChartView = new QChartView(chart);
    m_departmentChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->departmentChartWidget->setLayout(new QVBoxLayout());
    ui->departmentChartWidget->layout()->addWidget(m_departmentChartView);
}

void AnalyticsWindow::createRoomUtilizationChart()
{
    // Create horizontal bar chart for room utilization
    auto series = new QHorizontalBarSeries();
    auto barSet = new QBarSet("Utilization %");
    
    QStringList categories;
    
    for (const auto& pair : m_analyticsData.roomUtilizationPercentage) {
        categories << QString::fromStdString(pair.first);
        *barSet << pair.second;
    }
    
    series->append(barSet);
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📈 Room Capacity Utilization");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisY = new QBarCategoryAxis();
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    auto axisX = new QValueAxis();
    axisX->setTitleText("Utilization Percentage");
    axisX->setRange(0, 100);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    // Color coding based on utilization level
    barSet->setColor(QColor(46, 204, 113)); // Green for good utilization
    
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_roomUtilizationChartView = new QChartView(chart);
    m_roomUtilizationChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->roomUtilizationChartWidget->setLayout(new QVBoxLayout());
    ui->roomUtilizationChartWidget->layout()->addWidget(m_roomUtilizationChartView);
}

void AnalyticsWindow::createConflictDetectionChart()
{
    // Create bar chart for conflict detection
    auto series = new QBarSeries();
    auto barSet = new QBarSet("Conflicts");
    
    QStringList categories;
    
    for (const auto& pair : m_analyticsData.conflictsPerCourse) {
        categories << QString::fromStdString(pair.first);
        *barSet << pair.second;
    }
    
    series->append(barSet);
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("⚡ Conflicts Detected per Course");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto axisY = new QValueAxis();
    axisY->setTitleText("Number of Conflicts");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    // Red color for conflicts (warning)
    barSet->setColor(QColor(231, 76, 60));
    
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_conflictChartView = new QChartView(chart);
    m_conflictChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->conflictChartWidget->setLayout(new QVBoxLayout());
    ui->conflictChartWidget->layout()->addWidget(m_conflictChartView);
}

void AnalyticsWindow::createExamTimingChart()
{
    // Create a Gantt-style chart using scatter plot with different markers for time visualization
    auto series = new QScatterSeries();
    series->setName("Exam Timings");
    series->setMarkerSize(15.0);
    
    // Sample time slots data (would come from actual exam schedule)
    QStringList timeSlots = {"09:00-12:00", "14:00-17:00", "09:00-11:00", "11:30-14:30", "15:00-18:00"};
    QStringList courses = {"ML101", "AI401", "DSA201", "DB301", "OS401"};
    
    for (int i = 0; i < timeSlots.size() && i < courses.size(); ++i) {
        series->append(i, i); // Simple positioning for demo
    }
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📊 Exam Timings Overview");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisX = new QValueAxis();
    axisX->setTitleText("Time Period");
    axisX->setRange(0, 6);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto axisY = new QValueAxis();
    axisY->setTitleText("Courses");
    axisY->setRange(0, 6);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    series->setColor(QColor(155, 89, 182));
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_examTimingChartView = new QChartView(chart);
    m_examTimingChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->examTimingChartWidget->setLayout(new QVBoxLayout());
    ui->examTimingChartWidget->layout()->addWidget(m_examTimingChartView);
}

void AnalyticsWindow::createRoomLoadOverDaysChart()
{
#ifdef HAVE_DATAVISUALIZATION
    // Create 3D surface chart for room load over days
    m_roomLoad3DSurface = new Q3DSurface();
    
    // Create surface series
    auto surfaceSeries = new QSurface3DSeries();
    surfaceSeries->setName("Room Load");
    
    // Create sample data array
    auto dataArray = new QSurfaceDataArray();
    
    std::vector<std::string> rooms = {"Room A101", "Room A102", "Room B201", "Room B202", "Room C301"};
    std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    
    dataArray->reserve(rooms.size());
    for (int room = 0; room < rooms.size(); ++room) {
        auto newRow = new QSurfaceDataRow(days.size());
        for (int day = 0; day < days.size(); ++day) {
            auto it = m_analyticsData.roomLoadPerDay.find({rooms[room], days[day]});
            float load = (it != m_analyticsData.roomLoadPerDay.end()) ? it->second : 0;
            (*newRow)[day].setPosition(QVector3D(day, load, room));
        }
        dataArray->append(newRow);
    }
    
    surfaceSeries->dataProxy()->resetArray(dataArray);
    m_roomLoad3DSurface->addSeries(surfaceSeries);
    
    // Configure axes
    m_roomLoad3DSurface->axisX()->setTitle("Days");
    m_roomLoad3DSurface->axisY()->setTitle("Student Load");
    m_roomLoad3DSurface->axisZ()->setTitle("Rooms");
    
    // Set axis ranges
    m_roomLoad3DSurface->axisX()->setRange(0, days.size() - 1);
    m_roomLoad3DSurface->axisY()->setRange(0, 100);
    m_roomLoad3DSurface->axisZ()->setRange(0, rooms.size() - 1);
    
    // Create widget container for 3D surface
    m_roomLoad3DWidget = QWidget::createWindowContainer(m_roomLoad3DSurface);
    m_roomLoad3DWidget->setMinimumSize(500, 280);
#else
    // Fallback to 2D chart when DataVisualization is not available
    auto series = new QLineSeries();
    series->setName("Average Room Load");
    
    std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    
    for (int i = 0; i < days.size(); ++i) {
        int totalLoad = 0;
        int roomCount = 0;
        
        for (const auto& pair : m_analyticsData.roomLoadPerDay) {
            if (pair.first.second == days[i]) {
                totalLoad += pair.second;
                roomCount++;
            }
        }
        
        double averageLoad = roomCount > 0 ? (double)totalLoad / roomCount : 0;
        series->append(i, averageLoad);
    }
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("📈 Average Room Load Over Days");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisX = new QValueAxis();
    axisX->setTitleText("Days");
    axisX->setRange(0, days.size() - 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto axisY = new QValueAxis();
    axisY->setTitleText("Average Load");
    axisY->setRange(0, 100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    series->setColor(QColor(52, 152, 219));
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    auto chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    m_roomLoad3DWidget = chartView;
#endif
    
    // Add to layout
    ui->roomLoadChartWidget->setLayout(new QVBoxLayout());
    ui->roomLoadChartWidget->layout()->addWidget(m_roomLoad3DWidget);
}

void AnalyticsWindow::createStudentExamDistributionChart()
{
    // Create histogram for student exam count distribution
    auto series = new QBarSeries();
    auto barSet = new QBarSet("Students");
    
    QStringList categories;
    
    for (const auto& pair : m_analyticsData.studentExamCountDistribution) {
        categories << QString("%1 Exams").arg(pair.first);
        *barSet << pair.second;
    }
    
    series->append(barSet);
    
    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("👨‍🎓 Number of Exams per Student");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    auto axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto axisY = new QValueAxis();
    axisY->setTitleText("Number of Students");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    // Gradient color for better visualization
    barSet->setColor(QColor(26, 188, 156));
    
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    m_studentDistributionChartView = new QChartView(chart);
    m_studentDistributionChartView->setRenderHint(QPainter::Antialiasing);
    
    // Add to layout
    ui->studentDistributionChartWidget->setLayout(new QVBoxLayout());
    ui->studentDistributionChartWidget->layout()->addWidget(m_studentDistributionChartView);
}

// Slot implementations
void AnalyticsWindow::onRefreshClicked()
{
    refreshAnalytics();
    QMessageBox::information(this, "Analytics Refreshed", "Analytics data has been refreshed successfully!");
}

void AnalyticsWindow::onExportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Analytics Report", 
                                                   "analytics_report.pdf", "PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        exportChartsAsPDF(fileName);
        QMessageBox::information(this, "Export Successful", 
                                QString("Analytics report exported to: %1").arg(fileName));
    }
}

void AnalyticsWindow::onCloseClicked()
{
    // Close the analytics window and return focus to main window
    hide();
    
    // Ensure parent window is visible and active
    if (parentWidget()) {
        parentWidget()->show();
        parentWidget()->raise();
        parentWidget()->activateWindow();
    }
}

void AnalyticsWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        onCloseClicked();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void AnalyticsWindow::closeEvent(QCloseEvent* event)
{
    // When user clicks window X button, also ensure main window is visible
    if (parentWidget()) {
        parentWidget()->show();
        parentWidget()->raise();
        parentWidget()->activateWindow();
    }
    event->accept();
}

// Data processing methods (stubs for database integration)
void AnalyticsWindow::processExamLoadData()
{
    // Clear existing data
    m_analyticsData.examCountPerDay.clear();
    
    if (m_timetableGenerator && m_timetableGenerator->isGenerated()) {
        // Get actual schedule data from timetable generator
        auto schedule = m_timetableGenerator->getSchedule();
        
        // Process schedule lines to extract exam load per day
        std::map<std::string, int> dayCount;
        
        for (const auto& line : schedule) {
            // Parse schedule line format (assuming it contains day information)
            // This is a simple implementation - adjust based on actual format
            if (line.find("Monday") != std::string::npos) dayCount["Monday"]++;
            else if (line.find("Tuesday") != std::string::npos) dayCount["Tuesday"]++;
            else if (line.find("Wednesday") != std::string::npos) dayCount["Wednesday"]++;
            else if (line.find("Thursday") != std::string::npos) dayCount["Thursday"]++;
            else if (line.find("Friday") != std::string::npos) dayCount["Friday"]++;
            else if (line.find("Saturday") != std::string::npos) dayCount["Saturday"]++;
        }
        
        // Store the processed data
        m_analyticsData.examCountPerDay = dayCount;
        
    } else if (m_databaseManager) {
        // Alternative: Get data directly from database if no generated schedule
        auto courses = m_databaseManager->fetchCourses();
        
        // Simulate exam distribution (in real implementation, you'd have exam date table)
        std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        
        int courseCount = 0;
        auto current = courses.getHead();
        while (current != nullptr) {
            courseCount++;
            current = current->next;
        }
        
        // Distribute courses across days
        for (int i = 0; i < days.size(); i++) {
            m_analyticsData.examCountPerDay[days[i]] = (courseCount + days.size() - 1) / days.size(); // Ceiling division
        }
        
    } else {
        // Fall back to sample data
        generateSampleData();
    }
}

void AnalyticsWindow::processDepartmentDistributionData()
{
    // Clear existing data
    m_analyticsData.examCountPerDepartment.clear();
    
    if (m_databaseManager) {
        // Get actual course data from database
        auto courses = m_databaseManager->fetchCourses();
        
        // Count courses by department
        std::map<std::string, int> deptCount;
        
        auto current = courses.getHead();
        while (current != nullptr) {
            std::string dept = current->data.department;
            if (!dept.empty()) {
                deptCount[dept]++;
            }
            current = current->next;
        }
        
        // Store the processed data
        m_analyticsData.examCountPerDepartment = deptCount;
        
        // If no data found, use sample data
        if (deptCount.empty()) {
            m_analyticsData.examCountPerDepartment["Computer Science"] = 15;
            m_analyticsData.examCountPerDepartment["Information Technology"] = 12;
            m_analyticsData.examCountPerDepartment["Electronics"] = 8;
        }
    } else {
        // Fall back to sample data
        m_analyticsData.examCountPerDepartment["Computer Science"] = 15;
        m_analyticsData.examCountPerDepartment["Information Technology"] = 12;
        m_analyticsData.examCountPerDepartment["Electronics"] = 8;
        m_analyticsData.examCountPerDepartment["Mechanical"] = 6;
        m_analyticsData.examCountPerDepartment["Civil"] = 4;
    }
}

void AnalyticsWindow::processRoomUtilizationData()
{
    // Clear existing data
    m_analyticsData.roomUtilizationPercentage.clear();
    
    if (m_databaseManager) {
        // Get course sizes which indicate room capacity needs
        auto courseSizes = m_databaseManager->getCourseSizes();
        
        // Simulate room assignments and calculate utilization
        std::vector<std::string> rooms = {"Room A101", "Room A102", "Room B201", "Room B202", "Room C301"};
        int totalStudents = 0;
        
        for (const auto& pair : courseSizes) {
            totalStudents += pair.second;
        }
        
        if (totalStudents > 0) {
            // Distribute load across rooms and calculate utilization
            int avgCapacity = 100; // Assume 100 student capacity per room
            
            for (size_t i = 0; i < rooms.size(); i++) {
                int roomLoad = totalStudents / rooms.size();
                if (i < totalStudents % rooms.size()) roomLoad++; // Distribute remainder
                
                double utilization = (static_cast<double>(roomLoad) / avgCapacity) * 100.0;
                utilization = std::min(utilization, 100.0); // Cap at 100%
                
                m_analyticsData.roomUtilizationPercentage[rooms[i]] = utilization;
            }
        } else {
            // Use sample data if no course data
            m_analyticsData.roomUtilizationPercentage["Room A101"] = 90.0;
            m_analyticsData.roomUtilizationPercentage["Room A102"] = 85.0;
            m_analyticsData.roomUtilizationPercentage["Room B201"] = 75.0;
        }
    } else {
        // Fall back to sample data
        m_analyticsData.roomUtilizationPercentage["Room A101"] = 90.0;
        m_analyticsData.roomUtilizationPercentage["Room A102"] = 85.0;
        m_analyticsData.roomUtilizationPercentage["Room B201"] = 75.0;
        m_analyticsData.roomUtilizationPercentage["Room B202"] = 60.0;
        m_analyticsData.roomUtilizationPercentage["Room C301"] = 45.0;
    }
}

void AnalyticsWindow::processConflictDetectionData()
{
    // Clear existing data
    m_analyticsData.conflictsPerCourse.clear();
    
    if (m_databaseManager) {
        // Get student enrollments to detect potential conflicts
        auto enrollments = m_databaseManager->fetchEnrollments();
        auto courses = m_databaseManager->fetchCourses();
        
        // Count students per course to identify potential conflicts
        std::map<std::string, int> courseStudentCount;
        
        auto enrollCurrent = enrollments.getHead();
        while (enrollCurrent != nullptr) {
            courseStudentCount[enrollCurrent->data.courseId]++;
            enrollCurrent = enrollCurrent->next;
        }
        
        // Simulate conflicts based on course load
        auto courseCurrent = courses.getHead();
        while (courseCurrent != nullptr) {
            std::string courseId = courseCurrent->data.courseId;
            int studentCount = courseStudentCount[courseId];
            
            // Simple conflict simulation: more students = higher chance of conflicts
            int conflicts = 0;
            if (studentCount > 100) conflicts = 5;
            else if (studentCount > 75) conflicts = 3;
            else if (studentCount > 50) conflicts = 2;
            else if (studentCount > 25) conflicts = 1;
            
            if (conflicts > 0) {
                m_analyticsData.conflictsPerCourse[courseId] = conflicts;
            }
            
            courseCurrent = courseCurrent->next;
        }
        
        // If no conflicts detected, use sample data
        if (m_analyticsData.conflictsPerCourse.empty()) {
            m_analyticsData.conflictsPerCourse["ML101"] = 2;
            m_analyticsData.conflictsPerCourse["AI401"] = 1;
        }
    } else {
        // Fall back to sample data
        m_analyticsData.conflictsPerCourse["ML101"] = 2;
        m_analyticsData.conflictsPerCourse["AI401"] = 5;
        m_analyticsData.conflictsPerCourse["DSA201"] = 1;
        m_analyticsData.conflictsPerCourse["DB301"] = 3;
        m_analyticsData.conflictsPerCourse["OS401"] = 4;
    }
}

void AnalyticsWindow::processExamTimingData()
{
    // Clear existing data
    m_analyticsData.examTimings.clear();
    
    if (m_timetableGenerator && m_timetableGenerator->isGenerated()) {
        // Process actual schedule data
        auto schedule = m_timetableGenerator->getSchedule();
        
        // Parse schedule to extract timing information
        // This is a simplified implementation
        for (size_t i = 0; i < std::min(schedule.size(), size_t(5)); i++) {
            ExamScheduleData examData;
            examData.courseId = "Course_" + std::to_string(i + 1);
            examData.timeSlot = (i % 2 == 0) ? "09:00-12:00" : "14:00-17:00";
            examData.date = "Day " + std::to_string((i % 5) + 1);
            examData.duration = 180; // 3 hours
            examData.studentsCount = 50 + (i * 10);
            
            m_analyticsData.examTimings.push_back(examData);
        }
    }
    
    // If no timing data, this chart will use scatter plot for demo
}

void AnalyticsWindow::processRoomLoadOverDaysData()
{
    // Clear existing data
    m_analyticsData.roomLoadPerDay.clear();
    
    if (m_databaseManager) {
        // Get course enrollment data
        auto courseSizes = m_databaseManager->getCourseSizes();
        
        std::vector<std::string> rooms = {"Room A101", "Room A102", "Room B201", "Room B202", "Room C301"};
        std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        
        // Distribute course loads across rooms and days
        int courseIndex = 0;
        for (const auto& pair : courseSizes) {
            std::string room = rooms[courseIndex % rooms.size()];
            std::string day = days[courseIndex % days.size()];
            
            m_analyticsData.roomLoadPerDay[{room, day}] += std::min(pair.second, 100);
            courseIndex++;
        }
        
        // Fill empty slots with some load
        for (const auto& room : rooms) {
            for (const auto& day : days) {
                if (m_analyticsData.roomLoadPerDay.find({room, day}) == m_analyticsData.roomLoadPerDay.end()) {
                    m_analyticsData.roomLoadPerDay[{room, day}] = 20 + (rand() % 30);
                }
            }
        }
    } else {
        // Fall back to sample data
        std::vector<std::string> rooms = {"Room A101", "Room A102", "Room B201", "Room B202", "Room C301"};
        std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        
        for (const auto& room : rooms) {
            for (const auto& day : days) {
                int load = 20 + (rand() % 60);
                m_analyticsData.roomLoadPerDay[{room, day}] = load;
            }
        }
    }
}

void AnalyticsWindow::processStudentExamDistributionData()
{
    // Clear existing data
    m_analyticsData.studentExamCountDistribution.clear();
    
    if (m_databaseManager) {
        // Get actual enrollment data from database
        auto enrollments = m_databaseManager->fetchEnrollments();
        
        // Count exams per student
        std::map<std::string, int> studentExamCount;
        
        auto current = enrollments.getHead();
        while (current != nullptr) {
            studentExamCount[current->data.rollNo]++;
            current = current->next;
        }
        
        // Create distribution of exam counts
        std::map<int, int> distribution;
        for (const auto& pair : studentExamCount) {
            distribution[pair.second]++;
        }
        
        // Store the processed data
        m_analyticsData.studentExamCountDistribution = distribution;
        
        // If no data found, use sample data
        if (distribution.empty()) {
            m_analyticsData.studentExamCountDistribution[3] = 30;
            m_analyticsData.studentExamCountDistribution[4] = 25;
            m_analyticsData.studentExamCountDistribution[5] = 20;
        }
    } else {
        // Fall back to sample data
        m_analyticsData.studentExamCountDistribution[1] = 15;
        m_analyticsData.studentExamCountDistribution[2] = 25;
        m_analyticsData.studentExamCountDistribution[3] = 30;
        m_analyticsData.studentExamCountDistribution[4] = 20;
        m_analyticsData.studentExamCountDistribution[5] = 8;
        m_analyticsData.studentExamCountDistribution[6] = 2;
    }
}

std::vector<ExamScheduleData> AnalyticsWindow::getExamScheduleFromDatabase()
{
    std::vector<ExamScheduleData> schedule;
    
    if (m_databaseManager) {
        // Implementation would fetch actual schedule data from database
        // For now, return empty to use sample data
    }
    
    return schedule;
}

void AnalyticsWindow::exportChartsAsPDF(const QString& filename)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    
    QPainter painter;
    if (painter.begin(&printer)) {
        // Capture the entire analytics window
        QPixmap pixmap = this->grab();
        
        QRect rect = painter.viewport();
        QSize size = pixmap.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(pixmap.rect());
        painter.drawPixmap(0, 0, pixmap);
        
        painter.end();
    }
}

QColor AnalyticsWindow::getRandomColor(int index)
{
    QStringList colors = {"#e74c3c", "#3498db", "#2ecc71", "#f39c12", "#9b59b6", "#1abc9c", "#34495e", "#e67e22"};
    return QColor(colors[index % colors.size()]);
}

QString AnalyticsWindow::formatPercentage(double value)
{
    return QString::number(value, 'f', 1) + "%";
}