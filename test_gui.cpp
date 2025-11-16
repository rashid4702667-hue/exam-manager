#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Test GUI starting...";
    
    QMainWindow window;
    window.setWindowTitle("Test Window");
    window.resize(400, 300);
    
    QLabel *label = new QLabel("Hello World! If you can see this, Qt is working.", &window);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);
    
    qDebug() << "Showing window...";
    window.show();
    
    qDebug() << "Starting event loop...";
    return app.exec();
}