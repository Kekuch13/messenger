#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->passwordLine->setEchoMode(QLineEdit::Password);

    connect(&conn, SIGNAL(readyRead()),this,SLOT(receiveMessage()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::receiveMessage() {
    try {
        boost::property_tree::ptree root = conn.receiveFromServer();

        if (root.get<std::string>("responseName") == "authorization") {
            //                    Dialog* d = new Dialog();
            //                    ui->tabWidget->insertTab(ui->tabWidget->count(), d, "34567");
            if (root.get<std::string>("status") == "success") {
                username = ui->loginLine->text().toStdString();
                std::unordered_map<std::string, std::string> data;
                data["requestName"] = "getDialogs";
                data["username"] = username;
                conn.sendToServer(data);
                ui->stackedWidget->setCurrentIndex(1);
            } else {
                ui->label_3->setText("Неверный логин/пароль");
            }
        } else if (root.get<std::string>("responseName") == "dialogs") {
            auto dialogs = root.get_child("dialogs");
            for(auto it = dialogs.begin(); it != dialogs.end(); ++it) {
                ui->dialogsList->addItem(it->second.data().c_str());
            }
        }
    } catch (std::exception& e) {
        ui->label_3->setText(e.what());
    }

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
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "authorization";
    data["login"] = ui->loginLine->text().toStdString();
    data["password"] = ui->passwordLine->text().toStdString();

    conn.sendToServer(data);
}
