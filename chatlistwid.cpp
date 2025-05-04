#include "chatlistwid.h"
#include "chatitemwidget.h"
#include "qevent.h"

#include <QScrollBar>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>

// 构造函数，初始化聊天列表控件
ChatListWid::ChatListWid(QWidget *parent)
    : QListWidget(parent), m_isFastScrolling(false)
{
    initUI();
    m_loadTimer = new QTimer(this);
    m_loadTimer->setSingleShot(true);
    m_loadTimer->setInterval(50); // 延长定时器间隔
    connect(m_loadTimer, &QTimer::timeout, this, &ChatListWid::checkVisibleItems);
    connect(this, &QListWidget::currentItemChanged, this, &ChatListWid::onCurrentItemChanged);
    loadChatItems(createTestData());
}

// 析构函数
ChatListWid::~ChatListWid()
{
}

// 加载聊天项列表
void ChatListWid::loadChatItems(const QVector<ChatItemData> &items)
{
    clear();
    m_chatItems.clear();
    m_loadedItems.clear();
    m_chatItems = items;
    sortChatItems();

    // 只创建QListWidgetItem，不立即创建ChatItemWidget
    for (const ChatItemData &item : m_chatItems) {
        if (item.isValid) {
            QListWidgetItem *listItem = new QListWidgetItem(this);
            listItem->setSizeHint(QSize(240, ITEM_HEIGHT));
            addItem(listItem);
        }
    }

    // 初始加载前15项
    checkVisibleItems();
}

// 创建ChatItemWidget
void ChatListWid::createChatItemWidget(int index)
{
    QListWidgetItem *item = this->item(index);
    if (!item || itemWidget(item)) // 已存在控件
        return;

    ChatItemWidget *widget = new ChatItemWidget(m_chatItems[index], this);
    setItemWidget(item, widget);
    widget->loadFullData();
    m_loadedItems.insert(index);
}

// 检查并加载可见项
void ChatListWid::checkVisibleItems()
{
    QRect viewportRect = viewport()->rect();
    int scrollValue = verticalScrollBar()->value();
    int startIndex = scrollValue / ITEM_HEIGHT;
    int endIndex = (scrollValue + viewportRect.height()) / ITEM_HEIGHT + 1;
    endIndex = qMin(endIndex, count() - 1);
    startIndex = qMax(0, startIndex - 1); // 预加载上一项

    int loadedCount = 0;
    int maxLoad = m_isFastScrolling ? MAX_LOAD_PER_CHECK_FAST : MAX_LOAD_PER_CHECK;
    QSet<int> currentLoadedItems;

    // 遍历可见范围
    for (int i = startIndex; i <= endIndex && i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item)
            continue;

        // 创建控件（如果尚未创建）
        if (!itemWidget(item)) {
            if (loadedCount < maxLoad) {
                createChatItemWidget(i);
                currentLoadedItems.insert(i);
                ++loadedCount;
            }
            continue;
        }

        ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
        if (!widget)
            continue;

        // 加载或卸载数据
        QRect itemRect = visualItemRect(item);
        bool isVisible = viewportRect.intersects(itemRect.adjusted(0, -ITEM_HEIGHT, 0, ITEM_HEIGHT));
        if (isVisible) {
            if (!widget->isFullyLoaded() && loadedCount < maxLoad) {
                widget->loadFullData();
                currentLoadedItems.insert(i);
                ++loadedCount;
            } else if (widget->isFullyLoaded()) {
                currentLoadedItems.insert(i);
            }
        } else {
            if (widget->isFullyLoaded()) {
                widget->unloadData();
                // 可选：销毁不可见控件以进一步释放内存
                // setItemWidget(item, nullptr);
                // delete widget;
            }
        }
    }

    // 卸载其他已加载但不可见的项
    for (int i : m_loadedItems) {
        if (!currentLoadedItems.contains(i) && i >= 0 && i < count()) {
            QListWidgetItem *item = this->item(i);
            if (item) {
                ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
                if (widget && widget->isFullyLoaded()) {
                    widget->unloadData();
                    // 可选：销毁控件
                    // setItemWidget(item, nullptr);
                    // delete widget;
                }
            }
        }
    }

    m_loadedItems = currentLoadedItems;
    m_isFastScrolling = false;
}

// 视口事件处理
bool ChatListWid::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::Paint || event->type() == QEvent::Resize) {
        if (!m_loadTimer->isActive()) {
            m_loadTimer->start();
        }
    }
    return QListWidget::viewportEvent(event);
}

// 滚轮事件处理
void ChatListWid::wheelEvent(QWheelEvent *event)
{
    QScrollBar *scrollBar = verticalScrollBar();
    if (!scrollBar) {
        QListWidget::wheelEvent(event);
        return;
    }

    int delta = 0;
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();
    if (!numPixels.isNull()) {
        delta = numPixels.y();
    } else if (!numDegrees.isNull()) {
        delta = numDegrees.y() / 2; // 标准化角度增量
    }

    scrollBar->setValue(scrollBar->value() - delta);
    m_isFastScrolling = true;
    if (!m_loadTimer->isActive()) {
        m_loadTimer->start();
    }
    event->accept();
}

// 当前项改变处理
void ChatListWid::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (previous) {
        ChatItemWidget *prevWidget = qobject_cast<ChatItemWidget*>(itemWidget(previous));
        if (prevWidget) {
            prevWidget->setSelected(false);
        }
    }

    if (current) {
        if (!itemWidget(current)) {
            createChatItemWidget(row(current));
        }
        ChatItemWidget *currWidget = qobject_cast<ChatItemWidget*>(itemWidget(current));
        if (currWidget) {
            currWidget->setSelected(true);
            if (!currWidget->isFullyLoaded()) {
                currWidget->loadFullData();
                m_loadedItems.insert(row(current));
            }
        }
    }
}

// 按最后消息时间排序聊天项
void ChatListWid::sortChatItems()
{
    std::sort(m_chatItems.begin(), m_chatItems.end(),
              [](const ChatItemData &a, const ChatItemData &b) {
                  return a.lastMessageTime > b.lastMessageTime;
              });
}

// 查找新项的插入位置
int ChatListWid::findInsertPosition(const ChatItemData &data) const
{
    auto it = std::lower_bound(m_chatItems.begin(), m_chatItems.end(), data,
                               [](const ChatItemData &a, const ChatItemData &b) {
                                   return a.lastMessageTime > b.lastMessageTime;
                               });
    return std::distance(m_chatItems.begin(), it);
}

// 添加聊天项
void ChatListWid::addChatItem(const ChatItemData &data)
{
    if (!data.isValid)
        return;

    int insertIndex = findInsertPosition(data);
    m_chatItems.insert(insertIndex, data);

    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(240, ITEM_HEIGHT));
    insertItem(insertIndex, item);

    QRect viewportRect = viewport()->rect();
    QRect itemRect = visualItemRect(item);
    if (viewportRect.intersects(itemRect)) {
        createChatItemWidget(insertIndex);
    }
}

// 更新聊天项
void ChatListWid::updateChatItem(int index, const ChatItemData &data)
{
    if (index < 0 || index >= count())
        return;

    if (index < m_chatItems.size() && m_chatItems[index].lastMessageTime == data.lastMessageTime) {
        m_chatItems[index] = data;
        QListWidgetItem *item = this->item(index);
        if (item) {
            ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
            if (widget) {
                widget->updateData(data);
                widget->setSelected(currentRow() == index);
                if (m_loadedItems.contains(index) && !widget->isFullyLoaded()) {
                    widget->loadFullData();
                }
            } else if (m_loadedItems.contains(index)) {
                createChatItemWidget(index);
            }
        }
        return;
    }

    removeChatItem(index);
    addChatItem(data);
}

// 移除聊天项
void ChatListWid::removeChatItem(int index)
{
    if (index < 0 || index >= count())
        return;
    if (index < m_chatItems.size())
        m_chatItems.removeAt(index);
    m_loadedItems.remove(index);
    QListWidgetItem *item = takeItem(index);
    delete item;
}

// 获取所有聊天项数据
QVector<ChatItemData> ChatListWid::getChatItems() const
{
    return m_chatItems;
}

// 获取指定索引的聊天项数据
ChatItemData ChatListWid::getChatItemData(int index) const
{
    if (index < 0 || index >= m_chatItems.size())
        return ChatItemData();
    return m_chatItems[index];
}

// 获取当前选中项的索引
int ChatListWid::currentChatIndex() const
{
    return currentRow();
}

// 鼠标进入事件
void ChatListWid::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
}

// 鼠标离开事件
void ChatListWid::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
}

// 初始化UI
void ChatListWid::initUI()
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QScrollBar *scrollBar = verticalScrollBar();
    if (scrollBar) {
        scrollBar->setSingleStep(10);
        scrollBar->setPageStep(50);
    }

    setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: none;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   background-color: white;"
        "   border-radius: 4px;"
        "   margin: 2px 5px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #A2A2FE;"
        "}"
        "QListWidget::item:hover:!selected {"
        "   background-color: #F0F0F0;"
        "}"
        "QScrollBar:vertical {"
        "   background: transparent;"
        "   width: 4px;"
        "   margin: 0px 0px 0px 0px;"
        "   border-radius: 4px;"
        "}"
        "QScrollBar:vertical:hover, QListWidget:hover QScrollBar:vertical {"
        "   background: #F5F5F5;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #C0C0C0;"
        "   min-height: 20px;"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #A0A0A0;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "   background: none;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background: none;"
        "}"
        );
}

// 创建测试数据
QVector<ChatItemData> ChatListWid::createTestData()
{
    // 配置变量 - 只需修改这些变量即可调整数据模拟量
    const int TOTAL_COUNT = 10000;          // 总数据量
    const int INDIVIDUAL_RATIO = 60;        // 个人聊天占比（百分比）
    const int GROUP_RATIO = 40;             // 群聊占比（百分比）
    const int MAX_UNREAD_INDIVIDUAL = 20;    // 个人聊天最大未读数
    const int MAX_UNREAD_GROUP = 150;        // 群聊最大未读数
    const int MUTE_PROB_INDIVIDUAL = 10;     // 个人聊天静音概率（百分比）
    const int MUTE_PROB_GROUP = 30;          // 群聊静音概率（百分比）
    const int UNREAD_PROB_INDIVIDUAL = 30;   // 个人聊天有未读消息概率（百分比）
    const int UNREAD_PROB_GROUP = 60;        // 群聊有未读消息概率（百分比）
    const int SENDER_PROB = 50;              // 显示发送者概率（百分比）

    QVector<ChatItemData> testData;
    testData.reserve(TOTAL_COUNT);

    // 静态数据配置
    QStringList avatarPaths = {
        ":/LogReg/avatars/avatar1.png",
        ":/LogReg/avatars/avatar2.png",
        ":/LogReg/avatars/avatar3.png",
        ":/LogReg/avatars/avatar4.png",
        ":/LogReg/avatars/avatar5.png",
        ":/LogReg/avatars/avatar6.png",
    };

    QStringList surnames = {"赵","钱","孙","李","周","吴","郑","王","冯","陈","褚","卫","蒋","沈","韩","杨"};
    QStringList givenNames = {"伟","芳","娜","秀英","敏","静","丽","强","磊","军","洋","勇","艳","杰","娟","涛"};
    QStringList groupSuffixes = {"交流群","讨论组","粉丝群","亲友团","同学会","工作群","项目组","游戏群"};

    QStringList messageTemplates = {
        "你吃饭了吗？",
        "在吗？有事找你",
        "[图片]",
        "[语音消息]",
        "明天下午3点开会",
        "这个需求什么时候能完成？",
        "我马上到",
        "周末一起出去玩吧",
        "你看这个链接：https://example.com",
        "😂😂😂",
        "好的，没问题",
        "我再考虑一下",
        "谢谢！",
        "你听说了吗？",
        "最新版本已经发布",
        "帮我带杯咖啡",
        "晚上吃什么？",
        "项目进度怎么样了？",
        "这个bug怎么解决？",
        "记得带身份证"
    };

    // 预生成名称列表
    QStringList names;
    int individualCount = TOTAL_COUNT * INDIVIDUAL_RATIO / 100;
    int groupCount = TOTAL_COUNT * GROUP_RATIO / 100;

    for (int i = 0; i < individualCount; ++i) {
        names.append(surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                     givenNames[QRandomGenerator::global()->bounded(givenNames.size())]);
    }

    for (int i = 0; i < groupCount; ++i) {
        QString name = surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                       givenNames[QRandomGenerator::global()->bounded(givenNames.size())] +
                       "的" + groupSuffixes[QRandomGenerator::global()->bounded(groupSuffixes.size())];
        names.append(name);
    }

    // 生成测试数据
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < TOTAL_COUNT; ++i) {
        ChatItemData item;
        item.id = i + 1;
        item.avatarPath = avatarPaths[i % avatarPaths.size()];
        int randomMinutes = QRandomGenerator::global()->bounded(43200); // 30天内随机时间
        item.lastMessageTime = now.addSecs(-randomMinutes * 60);
        item.name = names[i];

        bool isGroup = item.name.contains("群");

        // 生成最后一条消息
        if (QRandomGenerator::global()->bounded(100) < SENDER_PROB && !isGroup) {
            QString sender = names[QRandomGenerator::global()->bounded(individualCount)] + ": ";
            item.lastMessage = sender + messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        } else {
            item.lastMessage = messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        }

        // 生成未读消息数
        if (QRandomGenerator::global()->bounded(100) < (isGroup ? UNREAD_PROB_GROUP : UNREAD_PROB_INDIVIDUAL)) {
            item.unreadCount = QRandomGenerator::global()->bounded(1, isGroup ? MAX_UNREAD_GROUP : MAX_UNREAD_INDIVIDUAL);
        } else {
            item.unreadCount = 0;
        }

        // 设置静音状态
        item.muted = QRandomGenerator::global()->bounded(100) < (isGroup ? MUTE_PROB_GROUP : MUTE_PROB_INDIVIDUAL);
        item.isValid = true;
        testData.append(item);
    }

    std::shuffle(testData.begin(), testData.end(), *QRandomGenerator::global());
    return testData;
}
