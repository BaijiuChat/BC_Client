#include "chatdialog.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    // 设置图标
    setupNavigation();
}

void ChatDialog::setupNavigation()
{
    // 1. 设置按钮图标
    // 侧边栏
    ui->logoLabel->setPixmap(QPixmap(":/LogReg/res/logo.png"));
    ui->chatSectionBtn->setIcon(QIcon(":/LogReg/res/chat.png"));
    ui->contactSectionBtn->setIcon(QIcon(":/LogReg/res/contact.png"));
    // 顶部
    ui->addButton->setIcon(QIcon(":/LogReg/res/plus.png"));
    ui->callButton->setIcon(QIcon(":/LogReg/res/call.png"));
    ui->videoButton->setIcon(QIcon(":/LogReg/res/video.png"));
    // 输入框
    ui->emojiButton->setIcon(QIcon(":/LogReg/res/emoji.png"));
    ui->upToGroupBtn->setIcon(QIcon(":/LogReg/res/upToGroup.png"));

    // 2. 设置按钮属性
    QList<QToolButton*> navButtons = {
        ui->chatSectionBtn,
        ui->contactSectionBtn
    };

    foreach (QToolButton *btn, navButtons) {
        btn->setCheckable(true);
        btn->setAutoExclusive(true); // 替代QButtonGroup的简便方案
    }

    // 3. 默认选中
    ui->chatSectionBtn->setChecked(true);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
