#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
    setWindowTitle("Connect to Database");
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

QString ConnectDialog::getDSN() const
{
    return ui->dsnLineEdit->text();
}

QString ConnectDialog::getUsername() const
{
    return ui->usernameLineEdit->text();
}

QString ConnectDialog::getPassword() const
{
    return ui->passwordLineEdit->text();
}