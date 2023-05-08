#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
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


void MainWidget::on_enterButton_clicked()
{
    // написать авторизацию
    Dialog* d = new Dialog();
    ui->tabWidget->insertTab(ui->tabWidget->count(), d, "34567");
    ui->stackedWidget->setCurrentIndex(1);
}
