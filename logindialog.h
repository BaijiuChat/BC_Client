#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAction>
#include <QJsonObject>
#include <QRegularExpression>
#include "httpmgr.h"
#include "tcpmgr.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    void setEmail(const QString &email);
    bool checkEmailValid();
    bool checkPwdValid();
    void showTip(QString str, bool isOK);
    void initHttpHandlers();
    ~LoginDialog();

signals:
    void registerRequest();
    void resetRequest();
    void sig_connect_tcp(ServerInfo);

private slots:
    void on_forgetButton_clicked();
    void on_loginButton_clicked();
    void slot_login_failed(int err);
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);

private:
    Ui::LoginDialog *ui;
    QAction *togglePwdAction;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

    int _uid;
    QString _token;
};

#endif // LOGINDIALOG_H
