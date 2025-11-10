#include "generatetimetabledialog.h"
#include "ui_generatetimetabledialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QDateTimeEdit>
#include <QDate>

GenerateTimetableDialog::GenerateTimetableDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GenerateTimetableDialog)
{
    ui->setupUi(this);
    setWindowTitle("Generate Timetable");

    // Set default date to today in DD-MM-YYYY format
    QDate today = QDate::currentDate();
    ui->dateEdit->setDate(today);
    ui->dateEdit->setDisplayFormat("dd-MM-yyyy");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &GenerateTimetableDialog::onAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &GenerateTimetableDialog::reject);
}

GenerateTimetableDialog::~GenerateTimetableDialog()
{
    delete ui;
}

QString GenerateTimetableDialog::getStartDate() const
{
    return ui->dateEdit->date().toString("dd-MM-yyyy");
}

void GenerateTimetableDialog::onAccept()
{
    QString date = getStartDate();
    if (validateDate(date)) {
        accept();
    } else {
        QMessageBox::warning(this, "Invalid Date",
            "Please enter a valid date in DD-MM-YYYY format\n"
            "Example: 15-01-2026");
    }
}

bool GenerateTimetableDialog::validateDate(const QString &date) const
{
    QRegularExpression re("^\\d{2}-\\d{2}-\\d{4}$");
    if (!re.match(date).hasMatch()) {
        return false;
    }

    QDate inputDate = QDate::fromString(date, "dd-MM-yyyy");
    return inputDate.isValid();
}