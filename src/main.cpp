#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QDateTime>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile logFile("debug.log");
    static bool firstTime = true;
    if (firstTime) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Text);
        firstTime = false;
    } else {
        logFile.open(QIODevice::Append | QIODevice::Text);
    }

    QTextStream stream(&logFile);
    stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    
    switch (type) {
        case QtDebugMsg:
            stream << "Debug: ";
            break;
        case QtInfoMsg:
            stream << "Info: ";
            break;
        case QtWarningMsg:
            stream << "Warning: ";
            break;
        case QtCriticalMsg:
            stream << "Critical: ";
            break;
        case QtFatalMsg:
            stream << "Fatal: ";
            break;
    }
    
    stream << msg << " (" << context.file << ":" << context.line << ")" << Qt::endl;
    logFile.close();
}

int main(int argc, char *argv[])
{
    try {
        // Install message handler for debug logging
        qInstallMessageHandler(messageHandler);

        // Enable high DPI scaling
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        
        qDebug() << "Starting application...";
        QApplication app(argc, argv);
        qDebug() << "QApplication created";
        
        // Check if plugins are properly loaded
        QStringList paths = app.libraryPaths();
        qDebug() << "Library paths:" << paths;
        
        MainWindow window;
        qDebug() << "MainWindow created";
        window.show();
        qDebug() << "MainWindow shown";
        
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", 
            QString("Unhandled exception: %1").arg(e.what()));
        return 1;
    } catch (...) {
        QMessageBox::critical(nullptr, "Error", 
            "Unknown error occurred during startup");
        return 1;
    }
}