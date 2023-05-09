#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "dialog.h"
#include "connection.h"
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    std::unordered_map<int, Dialog*> openDialogs;

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    const std::string& getUsername() const;

private slots:
    void on_showPassword_stateChanged(int arg1);

    void on_enterButton_clicked();

    void receiveMessage();

    void on_dialogButton_clicked();

    void on_registrationButton_clicked();

    void on_createAccountButton_clicked();

    void on_homeButton_clicked();

private:
    int c = 1;
    Connection* conn;
    std::string username;
    Ui::MainWidget *ui;
    std::unordered_map<std::string, int> dialogs;

};
#endif // MAINWIDGET_H
