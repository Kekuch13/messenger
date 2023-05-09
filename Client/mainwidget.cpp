#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    conn = new Connection("127.0.0.1", 13);

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->passwordLine->setEchoMode(QLineEdit::Password);

    connect(conn, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

MainWidget::~MainWidget()
{
    delete conn;
    delete ui;
}

const std::string& MainWidget::getUsername() const {
    return username;
}

void MainWidget::receiveMessage() {
    try {
        boost::property_tree::ptree root = conn->receiveFromServer();
        std::string responseName = root.get<std::string>("responseName");

        ui->label_4->setText((responseName + std::to_string(c)).c_str());
        c++;
        if (responseName == "authorization") {
            if (root.get<std::string>("status") == "success") {
                username = ui->loginLine->text().toStdString();
                std::unordered_map<std::string, std::string> data;
                data["requestName"] = "getDialogs";
                data["username"] = username;
                conn->sendToServer(data);
                ui->stackedWidget->setCurrentIndex(1);
            } else {
                ui->label_3->setText("Неверный логин/пароль");
            }
        } else if (responseName == "dialogs") {
            auto dialogsNode = root.get_child("dialogs");
            for(auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                std::string user = it->second.get<std::string>("username");
                dialogs[user] = it->second.get<int>("id");
                ui->dialogsList->addItem(user.c_str());
            }
        } else if (responseName == "addNewMessage") {
            if (openDialogs.contains(root.get<int>("dialog_id"))) {
                openDialogs[root.get<int>("dialog_id")]->addMessage(root.get<std::string>("author") + ": " + root.get<std::string>("text"));
            }
            emit conn->readyRead();
        } else if (responseName == "success") {
            //
        }
    } catch (std::exception& e) {
        ui->label_4->setText(e.what());
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

    conn->sendToServer(data);
}

void MainWidget::on_dialogButton_clicked()
{
    QListWidgetItem* item = ui->dialogsList->currentItem();
    if (!item) {
        return;
    }

    int id = dialogs[item->text().toStdString()];
    Dialog* dialog = new Dialog(this, id, username, conn);

    ui->tabWidget->insertTab(ui->tabWidget->count(), dialog, item->text());
    openDialogs[id] = dialog;
    ui->dialogsList->takeItem(ui->dialogsList->row(item));
}

