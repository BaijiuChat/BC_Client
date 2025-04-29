#include "chatlistwid.h"
#include "chatitemwidget.h"
#include "qevent.h"

#include <QScrollBar>
#include <QDir>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>

// 构造函数，初始化聊天列表控件
ChatListWid::ChatListWid(QWidget *parent)
    : QListWidget(parent), m_isFastScrolling(false)  // 初始化快速滚动标志为false
{
    initUI();  // 初始化UI
    // 设置加载定时器（单次触发）
    m_loadTimer = new QTimer(this);
    m_loadTimer->setSingleShot(true);
    // 连接定时器超时信号到检查可见项槽函数
    connect(m_loadTimer, &QTimer::timeout, this, &ChatListWid::checkVisibleItems);
    // 加载测试数据
    loadChatItems(createTestData());
}

// 析构函数
ChatListWid::~ChatListWid()
{
}

// 加载聊天项列表
void ChatListWid::loadChatItems(const QVector<ChatItemData> &items)
{
    clear();  // 清空列表
    m_chatItems.clear();  // 清空聊天项数据
    m_loadedItems.clear();  // 清空已加载项索引
    m_chatItems = items;  // 设置新的聊天项数据
    sortChatItems();  // 按时间排序

    // 为每个有效聊天项创建列表项和控件
    for (const ChatItemData &item : m_chatItems) {
        if (item.isValid) {
            QListWidgetItem *listItem = new QListWidgetItem(this);
            listItem->setSizeHint(QSize(240, 72));  // 设置固定大小
            addItem(listItem);
            ChatItemWidget *widget = new ChatItemWidget(item, this);
            setItemWidget(listItem, widget);
        }
    }

    // 完全加载前15项（视口可见区域）
    for (int i = 0; i < qMin(15, count()); ++i) {
        QListWidgetItem *item = this->item(i);
        if (item) {
            ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
            if (widget && !widget->isFullyLoaded()) {
                widget->loadFullData();  // 加载完整数据
                m_loadedItems.insert(i);  // 记录已加载项索引
            }
        }
    }

    checkVisibleItems();  // 检查并加载可见项
}

// 检查并加载可见项
void ChatListWid::checkVisibleItems()
{
    QRect viewportRect = viewport()->rect();  // 获取视口矩形
    // 设置每次检查最多加载的项数（根据是否快速滚动）
    int loadedCount = 0;
    int maxLoad = m_isFastScrolling ? MAX_LOAD_PER_CHECK_FAST : MAX_LOAD_PER_CHECK;

    // 扩展视口范围（上下各72像素）以预加载
    viewportRect.adjust(0, -72, 0, 72);

    QSet<int> currentLoadedItems;  // 当前已加载项索引集合

    // 遍历所有项
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item)
            continue;

        QRect itemRect = visualItemRect(item);  // 获取项的可视矩形
        ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
        if (!widget)
            continue;

        // 检查项是否在扩展后的视口内
        bool isVisible = viewportRect.intersects(itemRect);
        if (isVisible) {
            // 如果可见且未加载，且未超过最大加载数，则加载
            if (!widget->isFullyLoaded() && loadedCount < maxLoad) {
                widget->loadFullData();
                currentLoadedItems.insert(i);
                ++loadedCount;
            } else if (widget->isFullyLoaded()) {
                currentLoadedItems.insert(i);
            }
        } else {
            // 如果不可见且已加载，则卸载
            if (widget->isFullyLoaded()) {
                widget->unloadData();
            }
        }
    }

    m_loadedItems = currentLoadedItems;  // 更新已加载项集合
    m_isFastScrolling = false; // 重置快速滚动状态
}

// 视口事件处理
bool ChatListWid::viewportEvent(QEvent *event)
{
    // 在绘制或调整大小时启动加载定时器
    if (event->type() == QEvent::Paint || event->type() == QEvent::Resize) {
        m_loadTimer->start(5);
    }
    return QListWidget::viewportEvent(event);
}

// 滚轮事件处理
void ChatListWid::wheelEvent(QWheelEvent *event)
{
    QScrollBar *scrollBar = verticalScrollBar();  // 获取垂直滚动条
    if (!scrollBar) {
        event->ignore();
        return;
    }

    // 如果滚动动画正在运行则停止
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        m_scrollAnimation->stop();
    }

    // 获取滚动的像素或角度增量
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();

    // 计算滚动增量
    int delta = 0;
    if (!numPixels.isNull()) {
        delta = numPixels.y();
    } else if (!numDegrees.isNull()) {
        delta = numDegrees.y();
    }

    // 设置滚动动画参数
    m_targetScrollValue = scrollBar->value() - delta;
    m_scrollAnimation->setStartValue(scrollBar->value());
    m_scrollAnimation->setEndValue(m_targetScrollValue);
    m_scrollAnimation->start();

    // 标记为快速滚动并启动加载定时器
    m_isFastScrolling = true;
    m_loadTimer->start(5);

    event->accept();
}

// 滚动条值改变处理
void ChatListWid::onScrollBarValueChanged(int value)
{
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        return;
    }
    m_targetScrollValue = value;
    m_isFastScrolling = true;  // 标记为快速滚动
    m_loadTimer->start(5);  // 启动加载定时器
    viewport()->update();  // 更新视口
}

// 按最后消息时间排序聊天项
void ChatListWid::sortChatItems()
{
    std::sort(m_chatItems.begin(), m_chatItems.end(),
              [](const ChatItemData &a, const ChatItemData &b) {
                  return a.lastMessageTime > b.lastMessageTime;  // 降序排序
              });
}

// 查找新项的插入位置（按时间排序）
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
    if (!data.isValid)  // 如果数据无效则返回
        return;

    // 查找插入位置并插入数据
    int insertIndex = findInsertPosition(data);
    m_chatItems.insert(insertIndex, data);

    // 创建列表项和控件
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(240, 72));
    insertItem(insertIndex, item);
    ChatItemWidget *widget = new ChatItemWidget(data, this);
    setItemWidget(item, widget);

    // 如果新项在视口内则加载完整数据
    QRect viewportRect = viewport()->rect();
    QRect itemRect = visualItemRect(item);
    if (viewportRect.intersects(itemRect)) {
        widget->loadFullData();
        m_loadedItems.insert(insertIndex);
    }
}

// 更新聊天项
void ChatListWid::updateChatItem(int index, const ChatItemData &data)
{
    if (index < 0 || index >= count())  // 检查索引有效性
        return;

    // 如果时间未改变则直接更新数据
    if (index < m_chatItems.size() && m_chatItems[index].lastMessageTime == data.lastMessageTime) {
        m_chatItems[index] = data;
        QListWidgetItem *item = this->item(index);
        if (item) {
            ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
            if (widget) {
                widget->updateData(data);  // 更新控件数据
                widget->setSelected(currentRow() == index);  // 更新选中状态
                // 如果已加载但未完全加载则加载完整数据
                if (m_loadedItems.contains(index) && !widget->isFullyLoaded()) {
                    widget->loadFullData();
                }
            }
        }
        return;
    }

    // 如果时间改变则移除并重新添加
    removeChatItem(index);
    addChatItem(data);
}

// 移除聊天项
void ChatListWid::removeChatItem(int index)
{
    if (index < 0 || index >= count())  // 检查索引有效性
        return;
    if (index < m_chatItems.size())
        m_chatItems.removeAt(index);  // 从数据中移除
    m_loadedItems.remove(index);  // 从已加载集合中移除
    QListWidgetItem *item = takeItem(index);  // 从列表中移除
    delete item;  // 删除列表项
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
    update();  // 更新控件
}

// 鼠标离开事件
void ChatListWid::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();  // 更新控件
}

// 初始化UI
void ChatListWid::initUI()
{
    setFrameShape(QFrame::NoFrame);  // 无边框
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 禁用水平滚动条
    setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁止编辑
    setSelectionMode(QAbstractItemView::SingleSelection);  // 单选模式
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);  // 像素级滚动

    // 设置垂直滚动条
    QScrollBar *scrollBar = verticalScrollBar();
    if (scrollBar) {
        scrollBar->setSingleStep(10);  // 单步滚动量
        scrollBar->setPageStep(50);  // 页步长
        // 连接滚动条值改变信号
        connect(scrollBar, &QScrollBar::valueChanged, this, &ChatListWid::onScrollBarValueChanged);
    }

    // 设置滚动动画
    m_scrollAnimation = new QPropertyAnimation(scrollBar, "value", this);
    m_scrollAnimation->setDuration(300);  // 动画持续时间
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);  // 缓动曲线

    // 设置样式表
    setStyleSheet(
        "QListWidget {"
        "   background-color: white;"  // 白色背景
        "   border: none;"  // 无边框
        "   outline: none;"  // 无轮廓
        "}"
        "QListWidget::item {"  // 列表项样式
        "   background-color: white;"  // 白色背景
        "   border-radius: 4px;"  // 圆角
        "   margin: 2px 5px;"  // 外边距
        "}"
        "QListWidget::item:selected {"  // 选中项样式
        "   background-color: #A2A2FE;"  // 浅紫色背景
        "}"
        "QListWidget::item:hover:!selected {"  // 悬停项样式
        "   background-color: #F0F0F0;"  // 浅灰色背景
        "}"
        "QScrollBar:vertical {"  // 垂直滚动条样式
        "   background: transparent;"  // 透明背景
        "   width: 4px;"  // 宽度
        "   margin: 0px 0px 0px 0px;"  // 边距
        "   border-radius: 4px;"  // 圆角
        "}"
        "QScrollBar:vertical:hover, QListWidget:hover QScrollBar:vertical {"  // 悬停时滚动条样式
        "   background: #F5F5F5;"  // 浅灰色背景
        "}"
        "QScrollBar::handle:vertical {"  // 滚动条滑块样式
        "   background: #C0C0C0;"  // 灰色
        "   min-height: 20px;"  // 最小高度
        "   border-radius: 4px;"  // 圆角
        "}"
        "QScrollBar::handle:vertical:hover {"  // 悬停时滑块样式
        "   background: #A0A0A0;"  // 深灰色
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"  // 上下箭头样式
        "   height: 0px;"  // 高度为0（隐藏）
        "   background: none;"  // 无背景
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"  // 滑块外区域样式
        "   background: none;"  // 无背景
        "}"
        );

    // 连接当前项改变信号
    connect(this, &QListWidget::currentItemChanged, [this](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous) {  // 前一个选中项
            QWidget *prevWidget = itemWidget(previous);
            if (prevWidget) {
                ChatItemWidget *prevChatWidget = qobject_cast<ChatItemWidget*>(prevWidget);
                if (prevChatWidget) {
                    prevChatWidget->setSelected(false);  // 取消选中状态
                }
            }
        }

        if (current) {  // 当前选中项
            QWidget *currWidget = itemWidget(current);
            if (currWidget) {
                ChatItemWidget *currChatWidget = qobject_cast<ChatItemWidget*>(currWidget);
                if (currChatWidget) {
                    currChatWidget->setSelected(true);  // 设置选中状态
                    // 如果未完全加载则加载完整数据
                    if (!currChatWidget->isFullyLoaded()) {
                        currChatWidget->loadFullData();
                        m_loadedItems.insert(this->row(current));  // 记录已加载
                    }
                }
            }
        }
    });
}

// 创建测试数据
QVector<ChatItemData> ChatListWid::createTestData()
{
    QVector<ChatItemData> testData;
    testData.reserve(100);

    QStringList avatarPaths = {
        ":/LogReg/avatars/avatar1.png",
        ":/LogReg/avatars/avatar2.png",
        ":/LogReg/avatars/avatar3.png",
        ":/LogReg/avatars/avatar4.png",
        ":/LogReg/avatars/avatar5.png",
        ":/LogReg/avatars/avatar6.png",
    };

    QStringList names;
    QStringList surnames = {"赵","钱","孙","李","周","吴","郑","王","冯","陈","褚","卫","蒋","沈","韩","杨"};
    QStringList givenNames = {"伟","芳","娜","秀英","敏","静","丽","强","磊","军","洋","勇","艳","杰","娟","涛"};
    QStringList groupSuffixes = {"交流群","讨论组","粉丝群","亲友团","同学会","工作群","项目组","游戏群"};

    for (int i = 0; i < 60; ++i) {
        names.append(surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                     givenNames[QRandomGenerator::global()->bounded(givenNames.size())]);
    }

    for (int i = 0; i < 40; ++i) {
        QString name = surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                       givenNames[QRandomGenerator::global()->bounded(givenNames.size())] +
                       "的" + groupSuffixes[QRandomGenerator::global()->bounded(groupSuffixes.size())];
        names.append(name);
    }

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

    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 100; ++i) {
        ChatItemData item;
        item.id = i + 1;
        item.avatarPath = avatarPaths[i % avatarPaths.size()];
        int randomMinutes = QRandomGenerator::global()->bounded(43200);
        item.lastMessageTime = now.addSecs(-randomMinutes * 60);
        item.name = names[i];

        if (QRandomGenerator::global()->bounded(100) < 50 && !item.name.contains("群")) {
            QString sender = names[QRandomGenerator::global()->bounded(60)] + ": ";
            item.lastMessage = sender + messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        } else {
            item.lastMessage = messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        }

        bool isGroup = item.name.contains("群");
        if (QRandomGenerator::global()->bounded(100) < (isGroup ? 60 : 30)) {
            item.unreadCount = QRandomGenerator::global()->bounded(1, isGroup ? 150 : 20);
        }

        item.muted = QRandomGenerator::global()->bounded(100) < (isGroup ? 30 : 10);
        item.isValid = true;
        testData.append(item);
    }

    std::shuffle(testData.begin(), testData.end(), *QRandomGenerator::global());
    return testData;
}
