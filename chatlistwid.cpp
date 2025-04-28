#include "chatlistwid.h"
#include "chatitemwidget.h"

#include <QDir>
#include <QRandomGenerator>

ChatListWid::ChatListWid(QWidget *parent)
    : QListWidget(parent)
{
    initUI();
    // 加载测试数据
    loadChatItems(createTestData());
}

ChatListWid::~ChatListWid()
{

}

void ChatListWid::loadChatItems(const QVector<ChatItemData> &items)
{
    // 清空列表
    clear();
    m_chatItems.clear();
    // 添加项目
    for(const ChatItemData &item : items){
        if (item.isValid){
            addChatItem(item);
        }
    }

    // if(count() > 0){
    //     setCurrentRow(0);
    // }
}

void ChatListWid::updateChatItem(int index, const ChatItemData &data)
{
    // 检测合法
    if (index < 0 || index >= count())
        return;
    // 更新数据缓存
    if (index < m_chatItems.size())
        m_chatItems[index] = data;
    // 更新UI
    QListWidgetItem *item = this->item(index);
    if (item){
        // 提升widget
        ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
        if (widget){
            widget->updateData(data);
            // 设置选中，注意不要用true
            widget->setSelected(currentRow() == index);
        }
    }

}

void ChatListWid::addChatItem(const ChatItemData &data)
{
    // 判断是否有效
    if(!data.isValid)
        return;
    // 将数据添加到缓存
    m_chatItems.append(data);
    // 创建QListWidget对象
    QListWidgetItem *item = new QListWidgetItem(this);
    item->setSizeHint(QSize(width(), 75)); // 设置项高度
    // 添加对象
    addItem(item);
    // 创建自定义widget绑定QListWidgetItem
    ChatItemWidget *widget = new ChatItemWidget(data, this);
    setItemWidget(item, widget);
}

void ChatListWid::removeChatItem(int index)
{
    if(index < 0 || index >= count())
        return;
    if(index < m_chatItems.size())
        m_chatItems.removeAt(index);
    // 移除UI项
    QListWidgetItem *item = takeItem(index);
    delete item;
}

QVector<ChatItemData> ChatListWid::getChatItems() const
{
    return m_chatItems;
}

ChatItemData ChatListWid::getChatItemData(int index) const
{
    if(index < 0 || index >= m_chatItems.size())
        return ChatItemData();
    return m_chatItems[index];
}

int ChatListWid::currentChatIndex() const
{
    return currentRow();
}

void ChatListWid::initUI()
{
    // 设置列表基本属性
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // 应用样式表
    setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: none;"
        "}"
        "QListWidget::item {"
        "   background-color: white;"
        "   border-radius: 4px;"
        "   margin: 2px 5px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #A2A2FE;" // 紫色背景
        "}"
        "QListWidget::item:hover:!selected {"
        "   background-color: #F0F0F0;" // 悬停时淡灰色背景
        "}"
        );

    // 连接选择变化信号（换聊天）
    connect(this, &QListWidget::currentItemChanged, [this](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous) {
            // 恢复前一个选中项的样式
            QWidget *prevWidget = itemWidget(previous);
            if (prevWidget) {
                // 安全提升widget为chatitemwidget
                ChatItemWidget *prevChatWidget = qobject_cast<ChatItemWidget*>(prevWidget);
                if (prevChatWidget) {
                    prevChatWidget->setSelected(false);
                }
            }
        }

        if (current) {
            // 设置当前选中项的样式
            QWidget *currWidget = itemWidget(current);
            if (currWidget) {
                // 安全提升widget为chatitemwidget
                ChatItemWidget *currChatWidget = qobject_cast<ChatItemWidget*>(currWidget);
                if (currChatWidget) {
                    currChatWidget->setSelected(true);
                }
            }
        }
    });
}

QVector<ChatItemData> ChatListWid::createTestData()
{
    QVector<ChatItemData> testData;

    // 预定义的头像路径(实际应用中应指向真实文件)
    QStringList avatarPaths = {
        ":/LogReg/avatars/avatar1.png",
        ":/LogReg/avatars/avatar2.png",
        ":/LogReg/avatars/avatar3.png",
        ":/LogReg/avatars/avatar4.png",
        ":/LogReg/avatars/avatar5.png",
        ":/LogReg/avatars/avatar6.png",
    };

    // 预定义的用户/群组名
    QStringList names = {
        "缘宝的宝库",
        "shao的AI绘画小组",
        "steam联机交流群",
        "醉浆草",
        "产品设计交流群",
        "前端开发实战",
        "技术分享",
        "张三",
        "李四",
        "王五"
    };

    // 预定义的最后一条消息
    QStringList lastMessages = {
        "2分14有啥意思吗",
        "plato: 好白的西瓜",
        "白又不是酒: 我也",
        "别走啊你",
        "新版UI设计稿已经上传",
        "React Hooks真香",
        "下周技术分享会议安排",
        "周末有空一起吃饭吗？",
        "项目文档已更新，请查收",
        "明天开会记得带上材料"
    };

    // 生成10个测试项
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 10; ++i) {
        ChatItemData item;
        item.id = i + 1;

        // 轮流使用预定义的头像(简单处理，实际应根据名称hash等方式选择)
        item.avatarPath = avatarPaths[i % avatarPaths.size()];

        // 随机时间(最近7天内)
        int randomMinutes = QRandomGenerator::global()->bounded(10080); // 7天*24小时*60分钟
        item.lastMessageTime = now.addSecs(-randomMinutes * 60);

        // 使用预定义的名称和消息
        item.name = names[i];
        item.lastMessage = lastMessages[i];

        // 随机设置未读消息数(30%概率有未读消息)
        if (QRandomGenerator::global()->bounded(100) < 30) {
            item.unreadCount = QRandomGenerator::global()->bounded(1, 120);
        }

        // 随机设置免打扰状态(20%概率开启)
        item.muted = QRandomGenerator::global()->bounded(100) < 20;

        // 所有测试项均有效
        item.isValid = true;

        testData.append(item);
    }

    return testData;
}
