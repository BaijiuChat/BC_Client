#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    , _mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode)
{
    ui->setupUi(this);

    // 初始化消息模型和代理
    messageModel = new MessageListModel(this);
    messageDelegate = new MessageItemDelegate(this);
    ui->messageListView->setModel(messageModel);
    ui->messageListView->setItemDelegate(messageDelegate);
    ui->messageListView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁用编辑
    ui->messageListView->setSpacing(5); // item 间距

    // 添加测试消息
    MessageItemData msg1(1, 1, "User1", ":/LogReg/avatars/avatar1.png", "Hello!", QDateTime::currentDateTime(), false, MessageItemData::Text);
    MessageItemData msg2(2, 2, "Me", ":/LogReg/avatars/avatar2.png", "你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好你好!", QDateTime::currentDateTime(), true, MessageItemData::Text);
    messageModel->addMessage(msg1);
    messageModel->addMessage(msg2);

    // 设置图标
    setupNavigation();
    // 搜索的信号与槽
    initSearchSystem();
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

void ChatDialog::initSearchSystem() {
    // 初始化定时器
    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(500); // 500ms防抖间隔

    // 连接信号槽
    connect(ui->searchEdit, &QLineEdit::textChanged, [=](const QString &text){
        // 即时UI切换逻辑
        _mode = text.isEmpty() ? ChatUIMode::ChatMode : ChatUIMode::SearchMode;
        ui->searchListWid->setVisible(_mode == ChatUIMode::SearchMode);
        ui->chatListWid->setVisible(_mode == ChatUIMode::ChatMode);

        // 触发防抖搜索
        searchTimer->start();
    });
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
