#include "mainwidget.h"
#include "./ui_mainwidget.h"
#include <QDebug>

#include <thread>
#include <QMessageBox>

MainWidget::MainWidget(QWidget *parent, std::shared_ptr<Connection> conn)
    : QWidget(parent), ui(new Ui::MainWidget), conn(conn)
    {
    try {
        ui->setupUi(this);
        ui->stackedWidget->setCurrentIndex(0);
        ui->passwordLine->setEchoMode(QLineEdit::Password);

        connect(this, &MainWidget::setWidget, this, &MainWidget::changeWindow);
        connect(this, &MainWidget::newDialog, this, &MainWidget::createDialog);

        std::thread([this] {return this->receiveMessage();}).detach();
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Ошибка", "Упс... Возникли проблемы с подключением к серверу");
    }
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::changeWindow(int idx) {
    ui->stackedWidget->setCurrentIndex(idx);
}

void MainWidget::createDialog(int id, std::string username) {
    QListWidgetItem *item = ui->newDialogsList->currentItem();
    auto dialog = std::make_shared<Dialog>(this, id, username, conn);

    ui->tabWidget->insertTab(ui->tabWidget->count(), dialog.get(), item->text());
    openDialogs[id] = dialog;
    ui->newDialogsList->takeItem(ui->newDialogsList->row(item));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

void MainWidget::receiveMessage() // обработчик поступивших запросов от сервера
{
    while (true) {
        try {
            boost::property_tree::ptree root = conn->receiveFromServer();
            std::string responseName = root.get<std::string>("responseName");

            if (responseName == "authorization") {
                if (root.get<std::string>("status") == "success") {
                    username = ui->loginLine->text().toStdString();
                    std::unordered_map<std::string, std::string> data;
                    data["requestName"] = "getDialogs";
                    data["username"] = username;
                    conn->sendToServer(data);

                    data.clear();
                    data["requestName"] = "getNewDialogs";
                    data["username"] = username;
                    conn->sendToServer(data);

                    emit setWidget(1);
                } else {
                    ui->label_3->setText("Неверный логин/пароль");
                }
            } else if (responseName == "registration") {
                if (root.get<std::string>("status") == "success") {
                    emit setWidget(0);
                    ui->label_3->setText("Аккаунт успешно создан");
                } else {
                    ui->label_8->setText("Логин занят");
                }
            } else if (responseName == "dialogs") {
                auto dialogsNode = root.get_child("dialogs");
                for (auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                    std::string user = it->second.get<std::string>("username");
                    dialogs[user] = it->second.get<int>("id");
                    ui->dialogsList->addItem(user.c_str());
                }
            } else if (responseName == "addNewMessage") {
                if (openDialogs.contains(root.get<int>("dialog_id"))) {
                    openDialogs[root.get<int>("dialog_id")]->addMessage(root.get<std::string>("author") + ": " + root.get<std::string>("text"));
                }
            } else if (responseName == "NewDialogs") {
                auto dialogsNode = root.get_child("dialogs");
                for (auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                    std::string user = it->second.data();
                    ui->newDialogsList->addItem(user.c_str());
                }
            } else if (responseName == "createdDialog") {
                int id = root.get<int>("dialog_id");
                emit newDialog(id, username);
            } else if (responseName == "dialogMessages") {
                int id = root.get<int>("dialog_id");
                boost::property_tree::ptree dialogsNode = root.get_child("messages");
                for (auto it = dialogsNode.begin(); it != dialogsNode.end(); ++it) {
                    std::string author = it->second.get<std::string>("author");
                    std::string text = it->second.get<std::string>("text");
                    qDebug() << author + ": " + text;
                    openDialogs[id]->addMessage(author + ": " + text);
                }
                openDialogs[id]->scrollToBottom();
            }
        } catch (std::exception &e) {
            qDebug() << e.what();
            ui->label_3->setText(e.what());
            ui->label_4->setText(e.what());
        }
    }
}

void MainWidget::loadMessages(int dialog_id) // запрос всех сообщений для конкретного диалога
{
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "dialogMessages";
    data["username"] = username;
    data["dialog_id"] = std::to_string(dialog_id);

    conn->sendToServer(data);
}

void MainWidget::on_showPassword_stateChanged(int arg1) // обработчик кнопки "Показать пароль"
{
    if (arg1 == 2) {
        ui->passwordLine->setEchoMode(QLineEdit::Normal);
    } else {
        ui->passwordLine->setEchoMode(QLineEdit::Password);
    }
}

void MainWidget::on_enterButton_clicked() // обработчик кнопки "Войти"
{
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "authorization";
    data["login"] = ui->loginLine->text().toStdString();
    data["password"] = ui->passwordLine->text().toStdString();

    conn->sendToServer(data);
}

void MainWidget::on_dialogButton_clicked() // обработчик кнопки "Перейти к диалогу"
{
    QListWidgetItem *item = ui->dialogsList->currentItem();
    if (!item) {
        return;
    }

    int id = dialogs[item->text().toStdString()];
    auto dialog = std::make_shared<Dialog>(this, id, username, conn);

    ui->tabWidget->insertTab(ui->tabWidget->count(), dialog.get(), item->text());
    openDialogs[id] = dialog;
    ui->dialogsList->takeItem(ui->dialogsList->row(item));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    loadMessages(id);
}

void MainWidget::on_registrationButton_clicked() // обработчик кнопки "Регистрация"
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWidget::on_createAccountButton_clicked() // обработчик кнопки "Создать аккаунт"
{
    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "registration";
    data["login"] = ui->loginLineReg->text().toStdString();
    data["password"] = ui->passwordLineReg->text().toStdString();

    conn->sendToServer(data);
}

void MainWidget::on_homeButton_clicked() // обработчик кнопки "Назад" из окна регистрации
{
    ui->loginLineReg->clear();
    ui->passwordLineReg->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::on_newDialogButton_clicked() // обработчик кнопки "Новый диалог"
{
    QListWidgetItem *item = ui->newDialogsList->currentItem();
    if (!item) {
        return;
    }

    std::unordered_map<std::string, std::string> data;
    data["requestName"] = "createDialog";
    data["username"] = username;
    data["buddy"] = item->text().toStdString();

    conn->sendToServer(data);
}

void MainWidget::on_tabWidget_tabCloseRequested(int index) // обработчик закрытия вкладок с диалогами
{
    auto curr = dynamic_cast<Dialog*>(ui->tabWidget->widget(index));
    if(curr) {
        std::string name = ui->tabWidget->tabText(index).toStdString();
        dialogs[name] = curr->getId();
        ui->dialogsList->addItem(name.c_str());
        openDialogs.erase(curr->getId());
    }
}

