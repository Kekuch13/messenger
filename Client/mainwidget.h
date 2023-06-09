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
    MainWidget(QWidget *parent, std::shared_ptr<Connection> conn);
    ~MainWidget();

private:
    void loadMessages(int dialog_id);

signals:
    void setWidget(int idx);
    void newDialog(int id, std::string username);

private slots:
    void on_showPassword_stateChanged(int arg1);
    void on_enterButton_clicked();
    void receiveMessage();
    void on_dialogButton_clicked();
    void on_registrationButton_clicked();
    void on_createAccountButton_clicked();
    void on_homeButton_clicked();
    void on_newDialogButton_clicked();
    void on_tabWidget_tabCloseRequested(int index);
    void changeWindow(int idx);
    void createDialog(int id, std::string username);

private:
    std::shared_ptr<Connection> conn;
    std::string username;
    Ui::MainWidget *ui;
    std::unordered_map<std::string, int> dialogs;
    std::unordered_map<int, std::shared_ptr<Dialog>> openDialogs;
};
#endif // MAINWIDGET_H
