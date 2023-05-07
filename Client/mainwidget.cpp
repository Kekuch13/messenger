#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->passwordLine->setEchoMode(QLineEdit::Password);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_showPassword_stateChanged(int arg1)
{
    if(arg1 == 2) {
        ui->passwordLine->setEchoMode(QLineEdit::Normal);
    }
    else {
        ui->passwordLine->setEchoMode(QLineEdit::Password);
    }
}


void MainWidget::on_pushButton_clicked()
{
    // написать авторизацию
    ui->label->setText("efggn");
}
