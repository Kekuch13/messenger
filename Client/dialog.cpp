#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, int id, std::string username, Connection*& conn) :
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

void Dialog::on_sendButton_clicked()
{
    QString message = ui->lineEdit->text();

    if(!message.isEmpty()) {
        ui->textBrowser->append(username.c_str() + ": " + message);
        std::unordered_map<std::string, std::string> data;
        data["requestName"] = "sendNewMessage";
        data["username"] = username;
        data["dialog_id"] = std::to_string(id);
        data["text"] = message.toStdString();

        conn->sendToServer(data);
    }
}

void Dialog::addMessage(std::string msg) {
    ui->label->setText(msg.c_str());
    ui->textBrowser->append(msg.c_str());
}
