#ifndef ANALYTICSWINDOW_H
#define ANALYTICSWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QKeyEvent>
#include <QCloseEvent>

// Qt Charts includes
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLegend>

// For 3D visualization (optional)
#ifdef HAVE_DATAVISUALIZATION
#include <QtDataVisualization>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/QCategory3DAxis>
#endif

#include <vector>
#include <map>
#include <string>
#include <memory>

// Forward declarations
class DatabaseManager;

QT_BEGIN_NAMESPACE
namespace Ui { class AnalyticsWindow; }
QT_END_NAMESPACE

struct ExamScheduleData {
    std::string courseId;
    std::string courseName;
    std::string department;
    std::string room;
    std::string date;
    std::string timeSlot;
    int duration;
    int studentsCount;
    std::vector<std::string> enrolledStudents;
};

struct AnalyticsData {
    // Exam Load Overview Data
    std::map<std::string, int> examCountPerDay;
    
    // Department Distribution Data
    std::map<std::string, int> examCountPerDepartment;
    
    // Room Utilization Data
    std::map<std::string, double> roomUtilizationPercentage;
    
    // Conflict Detection Data
    std::map<std::string, int> conflictsPerCourse;
    
    // Exam Timing Data
    std::vector<ExamScheduleData> examTimings;
    
    // Room Load Over Days Data
    std::map<std::pair<std::string, std::string>, int> roomLoadPerDay; // (room, day) -> load
    
    // Student Exam Distribution Data
    std::map<int, int> studentExamCountDistribution; // examCount -> studentCount
};

class AnalyticsWindow : public QDialog
{
    Q_OBJECT

public:
    AnalyticsWindow(DatabaseManager* dbManager = nullptr, QWidget *parent = nullptr);
    ~AnalyticsWindow();

    void setDatabaseManager(DatabaseManager* dbManager);
    void refreshAnalytics();

private slots:
    void onRefreshClicked();
    void onExportClicked();
    void onCloseClicked();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::AnalyticsWindow *ui;
    DatabaseManager* m_databaseManager;
    AnalyticsData m_analyticsData;
    
    // Chart views for each analytics section
    QChartView* m_examLoadChartView;
    QChartView* m_departmentChartView;
    QChartView* m_roomUtilizationChartView;
    QChartView* m_conflictChartView;
    QChartView* m_examTimingChartView;
#ifdef HAVE_DATAVISUALIZATION
    QtDataVisualization::Q3DSurface* m_roomLoad3DSurface;
#endif
    QWidget* m_roomLoad3DWidget;
    QChartView* m_studentDistributionChartView;

    // Analytics data processing methods
    void loadAnalyticsData();
    void processExamLoadData();
    void processDepartmentDistributionData();
    void processRoomUtilizationData();
    void processConflictDetectionData();
    void processExamTimingData();
    void processRoomLoadOverDaysData();
    void processStudentExamDistributionData();

    // Chart creation methods
    void createExamLoadChart();
    void createDepartmentDistributionChart();
    void createRoomUtilizationChart();
    void createConflictDetectionChart();
    void createExamTimingChart();
    void createRoomLoadOverDaysChart();
    void createStudentExamDistributionChart();

    // Utility methods
    void setupChartViews();
    void clearCharts();
    QColor getRandomColor(int index);
    QString formatPercentage(double value);
    std::vector<ExamScheduleData> getExamScheduleFromDatabase();
    void exportChartsAsPDF(const QString& filename);
    void clearWidgetLayout(QWidget* widget);
    
    // Sample data generation (for testing without database)
    void generateSampleData();
    AnalyticsData createSampleAnalyticsData();
};

#endif // ANALYTICSWINDOW_H