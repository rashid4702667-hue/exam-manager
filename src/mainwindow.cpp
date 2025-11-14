#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "generatetimetabledialog.h"
#include "analyticswindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)  // Initialize to nullptr first
    , generator(dbManager)
    , isConnected(false)
    , scheduleGenerated(false)
    , scheduleModel(nullptr)  // Initialize to nullptr first
    , analyticsWindow(nullptr)
{
    try {
        qDebug() << "Creating UI...";
        ui = new Ui::MainWindow;
        ui->setupUi(this);
        qDebug() << "UI created";
        
        qDebug() << "Creating schedule model...";
        scheduleModel = new ScheduleTableModel(this);
        qDebug() << "Schedule model created";
        
        qDebug() << "Setting up connections...";
        setupConnections();
        qDebug() << "Connections set up";
        
        enableScheduleActions(false);
        
        // Set up schedule table view
        ui->scheduleTableView->setModel(scheduleModel);
        ui->scheduleTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        qDebug() << "Table view set up";
        
        setWindowTitle("Timetable Planner");
        qDebug() << "Window title set";
        
    } catch (const std::exception& e) {
        qDebug() << "Exception in MainWindow constructor:" << e.what();
        throw;  // Re-throw to be caught by main()
    } catch (...) {
        qDebug() << "Unknown exception in MainWindow constructor";
        throw;  // Re-throw to be caught by main()
    }
}

MainWindow::~MainWindow()
{
    try {
        if (analyticsWindow) {
            delete analyticsWindow;
            analyticsWindow = nullptr;
        }
        if (ui) {
            delete ui;
            ui = nullptr;
        }
        qDebug() << "MainWindow destroyed";
    } catch (...) {
        // Don't throw from destructor
        qDebug() << "Exception during MainWindow destruction";
    }
}

void MainWindow::setupConnections()
{
    // Connect menu actions
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnectDatabase);
    connect(ui->actionGenerate, &QAction::triggered, this, &MainWindow::onGenerateTimetable);
    connect(ui->actionViewSchedule, &QAction::triggered, this, &MainWindow::onViewSchedule);
    connect(ui->actionExportCSV, &QAction::triggered, this, &MainWindow::onExportCSV);
    connect(ui->actionExportText, &QAction::triggered, this, &MainWindow::onExportText);
    connect(ui->actionExportPDF, &QAction::triggered, this, &MainWindow::onExportPDF);
    connect(ui->actionShiftCourse, &QAction::triggered, this, &MainWindow::onShiftCourse);
    connect(ui->actionAnalytics, &QAction::triggered, this, &MainWindow::onAnalytics);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::onConnectDatabase()
{
    ConnectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString dsn = dialog.getDSN();
        QString username = dialog.getUsername();
        QString password = dialog.getPassword();
        
        if (dbManager.connect(dsn.toStdString(), username.toStdString(), password.toStdString())) {
            isConnected = true;
            QMessageBox::information(this, "Connection Success", "Successfully connected to the database!");
            enableScheduleActions(true);
        } else {
            QMessageBox::critical(this, "Connection Error", "Failed to connect to the database!");
        }
    }
}

void MainWindow::onGenerateTimetable()
{
    if (!isConnected) {
        QMessageBox::warning(this, "Not Connected", "Please connect to the database first!");
        return;
    }
    
    GenerateTimetableDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString startDate = dialog.getStartDate();
        generator.setStartDate(startDate.toStdString());
        
        if (generator.generate()) {
            scheduleGenerated = true;
            updateScheduleView();
            // Enable view/export actions now that a schedule exists
            enableScheduleActions(true);
            QMessageBox::information(this, "Success", "Timetable generated successfully!");
            
            // Auto-export to CSV
            if (generator.exportToCSV("exam_schedule.csv")) {
                QMessageBox::information(this, "Export", "Schedule automatically saved to 'exam_schedule.csv'");
            }
        } else {
            QMessageBox::critical(this, "Error", "Failed to generate timetable!");
        }
    }
}

void MainWindow::onViewSchedule()
{
    if (!scheduleGenerated) {
        QMessageBox::warning(this, "No Schedule", "Please generate a timetable first!");
        return;
    }
    updateScheduleView();
}

void MainWindow::onExportCSV()
{
    if (!scheduleGenerated) {
        QMessageBox::warning(this, "No Schedule", "Please generate a timetable first!");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this, "Export CSV", 
        QString(), "CSV Files (*.csv)");
    
    if (!filename.isEmpty()) {
        if (generator.exportToCSV(filename.toStdString())) {
            QMessageBox::information(this, "Success", "Schedule exported to CSV successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to export schedule!");
        }
    }
}

void MainWindow::onExportText()
{
    if (!scheduleGenerated) {
        QMessageBox::warning(this, "No Schedule", "Please generate a timetable first!");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this, "Export Text", 
        QString(), "Text Files (*.txt)");
    
    if (!filename.isEmpty()) {
        if (generator.exportToText(filename.toStdString())) {
            QMessageBox::information(this, "Success", "Schedule exported to text file successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to export schedule!");
        }
    }
}

void MainWindow::onExportPDF()
{
    if (!scheduleGenerated) {
        QMessageBox::warning(this, "No Schedule", "Please generate a timetable first!");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this, "Export PDF", 
        QString(), "PDF Files (*.pdf)");
    
    if (!filename.isEmpty()) {
        if (generator.exportToPDF(filename.toStdString())) {
            QMessageBox::information(this, "Success", "Schedule exported to PDF successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to export schedule!");
        }
    }
}

void MainWindow::onShiftCourse()
{
    if (!scheduleGenerated) {
        QMessageBox::warning(this, "No Schedule", "Please generate a timetable first!");
        return;
    }
    
    // TODO: Implement course shifting dialog
    QMessageBox::information(this, "Not Implemented", "Course shifting feature coming soon!");
}

void MainWindow::onAnalytics()
{
    if (!analyticsWindow) {
        analyticsWindow = new AnalyticsWindow(&dbManager, this);
        analyticsWindow->setAttribute(Qt::WA_DeleteOnClose, false);
    }
    
    analyticsWindow->refreshAnalytics();
    
    // Show analytics as modal dialog instead of hiding main window
    if (analyticsWindow->exec() == QDialog::Accepted) {
        // Dialog closed normally, main window will be visible again
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Timetable Planner",
        "Timetable Planner\n\n"
        "A tool for generating and managing exam schedules.\n"
        "Features:\n"
        "- Automatic timetable generation\n"
        "- Conflict detection and resolution\n"
        "- Multiple export formats (CSV, Text, PDF)\n"
        "- Course rescheduling\n\n"
        "Version 1.0");
}

void MainWindow::updateScheduleView()
{
    // TODO: Update schedule model with current timetable data
    scheduleModel->setSchedule(generator);
    ui->scheduleTableView->resizeColumnsToContents();
    ui->scheduleTableView->resizeRowsToContents();
}

void MainWindow::enableScheduleActions(bool enable)
{
    ui->actionGenerate->setEnabled(enable);
    ui->actionViewSchedule->setEnabled(enable && scheduleGenerated);
    ui->actionExportCSV->setEnabled(enable && scheduleGenerated);
    ui->actionExportText->setEnabled(enable && scheduleGenerated);
    ui->actionExportPDF->setEnabled(enable && scheduleGenerated);
    ui->actionShiftCourse->setEnabled(enable && scheduleGenerated);
    ui->actionAnalytics->setEnabled(enable); // Analytics can work with or without generated schedule
}