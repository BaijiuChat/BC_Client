#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    // 设置图标
    setupNavigation();

    connect(ui->searchEdit, &QLineEdit::textChanged, [=](const QString &text){
        // clearButton->setVisible(!text.isEmpty());
        // 当清空文本时自动切换回聊天列表
        if(text.isEmpty()) {
            ui->searchListWid->hide();
            ui->chatListWid->show();
        }
    });
    // 点击清除按钮（弃用）
    // connect(clearButton, &QAction::triggered, [=](){
    //     ui->searchEdit->clear();
    //     ui->searchListWid->hide();
    //     ui->chatListWid->show();
    //     ui->searchEdit->clearFocus();
    // });
}

void ChatDialog::setupNavigation()
{
    ui->contactListWid->hide();
    ui->searchListWid->hide();
    // 1. 设置按钮图标
    // 侧边栏
    ui->logoLabel->setPixmap(QPixmap(":/LogReg/res/logo.png"));
    ui->chatSectionBtn->setIcon(QIcon(":/LogReg/res/chat.png"));
    ui->contactSectionBtn->setIcon(QIcon(":/LogReg/res/contact.png"));
    // 搜索栏
    ui->searchEdit->setMaxLength(15);
    ui->searchEdit->setPlaceholderText(QStringLiteral("搜索"));
    QAction* searchIcon = new QAction(QIcon(":/LogReg/res/search.png"), "搜索", ui->searchEdit);
    ui->searchEdit->addAction(searchIcon, QLineEdit::LeadingPosition);
    // 右侧清除按钮（初始隐藏）
    ui->searchEdit->setClearButtonEnabled(true);
    // clearButton = new QAction(QIcon(":/LogReg/res/x.png"), "清除", ui->searchEdit);
    // ui->searchEdit->addAction(clearButton, QLineEdit::TrailingPosition);
    // clearButton->setVisible(false);  //弃用
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
