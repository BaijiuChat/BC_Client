#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"
/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      主界面
 *
 * @author     白久不是酒
 * @date       2025/04/03
 * @history
 *****************************************************************************/

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void switchToRegister();
    void switchToLogin();
    void switchToReset();
    void switchToChat();

private:
    Ui::MainWindow *ui;
    QStackedWidget *_stackedWidget;
    LoginDialog *_login_Dlg;
    RegisterDialog *_reg_Dlg;
    ResetDialog *_reset_Dlg;
    ChatDialog *_chat_Dlg;
};
#endif // MAINWINDOW_H
