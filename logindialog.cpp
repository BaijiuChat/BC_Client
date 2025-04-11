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
