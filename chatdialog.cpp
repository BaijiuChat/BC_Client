#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QRandomGenerator64>

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
    // ui->messageListView->setSpacing(5); // item 间距

    // 初始化时设置 viewportWidth
    ui->messageListView->setProperty("viewportWidth", ui->messageListView->viewport()->width());

    // 设置 QListView 参数
    ui->messageListView->setWordWrap(true);                      // 启用自动换行
    ui->messageListView->setUniformItemSizes(false);             // 每个 item 可以有不同高度
    ui->messageListView->setResizeMode(QListView::Adjust);       // 自动根据内容大小重新布局

    // 生成测试数据（默认10000条）
    generateTestMessages(10000);

    // 设置图标
    setupNavigation();
    // 搜索的信号与槽
    initSearchSystem();
}

void ChatDialog::resizeEvent(QResizeEvent *event) {
    // 更新 viewportWidth 当窗口大小变化时
    ui->messageListView->setProperty("viewportWidth", ui->messageListView->viewport()->width());
    QDialog::resizeEvent(event); // 调用父类实现
}

// 生成随机文本
QString ChatDialog::generateRandomText(int maxWords)
{
    static const QStringList wordPool = {
        "你好", "Hello", "今天天气不错", "What's up?", "最近怎么样",
        "I'm fine", "谢谢", "Thank you", "这个项目很有趣", "This is a test",
        "我们可以聊聊", "Let's discuss", "明天见", "See you tomorrow",
        "代码写得不错", "Good code", "Qt很好用", "Qt is powerful",
        "随机消息", "Random message", "测试数据", "Test data",
        "中文", "English", "混合", "Mixed", "消息", "Message"
    };

    QString result;
    int wordCount = QRandomGenerator::global()->bounded(1, maxWords);

    for (int i = 0; i < wordCount; ++i) {
        int index = QRandomGenerator::global()->bounded(wordPool.size());
        result += wordPool.at(index);

        // 随机添加空格或标点
        int punctuation = QRandomGenerator::global()->bounded(10);
        if (punctuation < 2) result += "。";
        else if (punctuation < 4) result += "！";
        else if (punctuation < 6) result += "？";
        else result += " ";
    }

    return result.trimmed();
}

// 生成随机时间（覆盖去年、上个月、昨天和今天）
QDateTime ChatDialog::generateRandomDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    int timeType = QRandomGenerator::global()->bounded(100);

    if (timeType < 5) { // 5% 去年
        return now.addYears(-1).addSecs(QRandomGenerator::global()->bounded(31536000));
    } else if (timeType < 20) { // 15% 上个月
        return now.addMonths(-1).addSecs(QRandomGenerator::global()->bounded(2592000));
    } else if (timeType < 40) { // 20% 昨天
        return now.addDays(-1).addSecs(QRandomGenerator::global()->bounded(86400));
    } else { // 60% 今天
        return now.addSecs(QRandomGenerator::global()->bounded(86400));
    }
}

// 生成测试消息
void ChatDialog::generateTestMessages(int count)
{
    // 清空现有消息
    // messageModel->clear();

    // 生成指定数量的测试消息
    for (int i = 0; i < count; ++i) {
        int senderId = QRandomGenerator::global()->bounded(1, 7); // 1-6
        bool isSelf = (senderId == 2); // 假设id为2的是自己

        MessageItemData msg(
            i + 1, // messageId
            senderId, // senderId
            QString("用户%1").arg(senderId), // senderName
            QString(":/LogReg/avatars/avatar%1.png").arg(senderId), // avatarPath
            generateRandomText(), // content
            generateRandomDateTime(), // sendTime
            isSelf, // isSelf
            MessageItemData::Text // type
            );

        messageModel->addMessage(msg);

        // 每生成1000条消息时更新UI（防止界面卡死）
        if (i % 1000 == 0) {
            QCoreApplication::processEvents();
        }
    }
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
