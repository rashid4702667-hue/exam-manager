#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DatabaseManager.h"
#include "TimetableGenerator.h"
#include "scheduletablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Forward declaration
class AnalyticsWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectDatabase();
    void onImportCSV();
    void onGenerateTimetable();
    void onViewSchedule();
    void onExportCSV();
    void onExportText();
    void onExportPDF();
    void onShiftCourse();
    void onAnalytics();
    void onAbout();

private:
    void setupConnections();
    void updateScheduleView();
    void enableScheduleActions(bool enable);

    Ui::MainWindow *ui;
    DatabaseManager dbManager;
    TimetableGenerator generator;
    bool isConnected;
    bool scheduleGenerated;
    ScheduleTableModel *scheduleModel;
    AnalyticsWindow *analyticsWindow;
};

#endif // MAINWINDOW_H