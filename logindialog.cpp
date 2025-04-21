#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 连接注册按钮
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::registerRequest);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setEmail(const QString &email)
{
    ui->userLineEdit->setText(email); // 自动填写邮箱
    ui->pwdLineEdit->setFocus();    // 焦点移到密码输入框
}
