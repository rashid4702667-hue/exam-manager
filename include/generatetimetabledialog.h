#ifndef GENERATETIMETABLEDIALOG_H
#define GENERATETIMETABLEDIALOG_H

#include <QDialog>
#include <QDateTimeEdit>

namespace Ui {
class GenerateTimetableDialog;
}

class GenerateTimetableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateTimetableDialog(QWidget *parent = nullptr);
    ~GenerateTimetableDialog();

    QString getStartDate() const;

private slots:
    void onAccept();

private:
    Ui::GenerateTimetableDialog *ui;
    bool validateDate(const QString &date) const;
};

#endif // GENERATETIMETABLEDIALOG_H