#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <unordered_map>
#include "connection.h"

namespace Ui {
class Dialog;
}

class Dialog : public QWidget, std::enable_shared_from_this<Dialog>
{
    Q_OBJECT

public:
    Dialog(QWidget *parent, int id, std::string username, std::shared_ptr<Connection> conn);
    ~Dialog();

    void addMessage(std::string msg);
    void scrollToBottom();

    const int& getId() const;

private slots:
    void on_sendButton_clicked();

private:
    Ui::Dialog *ui;
    int id;
    std::string username;
    std::shared_ptr<Connection> conn;
};

#endif // DIALOG_H
