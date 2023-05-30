#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, int id, std::string username, Connection* conn) :
    QWidget(parent),
    ui(new Ui::Dialog),
    id(id),
    username(username),
    conn(conn)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_sendButton_clicked() // обработка и отправка введенного сообщения
{
    std::string message = ui->lineEdit->text().toStdString();

    if(!message.empty()) {
        addMessage(username + ": " + message);
        scrollToBottom();
        std::unordered_map<std::string, std::string> data;
        data["requestName"] = "sendNewMessage";
        data["username"] = username;
        data["dialog_id"] = std::to_string(id);
        data["text"] = message;

        ui->lineEdit->clear();
        conn->sendToServer(data);
    }
}

void Dialog::addMessage(std::string msg) // добавление сообщения в поле с диалогом
{
    ui->messagesList->addItem(msg.c_str());
}

void Dialog::scrollToBottom()
{
    QListWidgetItem* item = ui->messagesList->item(ui->messagesList->count()-1);
    ui->messagesList->scrollToItem(item);
}
