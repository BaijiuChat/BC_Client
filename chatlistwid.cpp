#include "chatlistwid.h"
#include "chatitemwidget.h"
#include "qevent.h"

#include <QScrollBar>
#include <QDir>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>

ChatListWid::ChatListWid(QWidget *parent)
    : QListWidget(parent), m_isFastScrolling(false)
{
    initUI();
    m_loadTimer = new QTimer(this);
    m_loadTimer->setSingleShot(true);
    connect(m_loadTimer, &QTimer::timeout, this, &ChatListWid::checkVisibleItems);
    loadChatItems(createTestData());
}

ChatListWid::~ChatListWid()
{
}

void ChatListWid::loadChatItems(const QVector<ChatItemData> &items)
{
    clear();
    m_chatItems.clear();
    m_loadedItems.clear();
    m_chatItems = items;
    sortChatItems();

    for (const ChatItemData &item : m_chatItems) {
        if (item.isValid) {
            QListWidgetItem *listItem = new QListWidgetItem(this);
            listItem->setSizeHint(QSize(240, 72));
            addItem(listItem);
            ChatItemWidget *widget = new ChatItemWidget(item, this);
            setItemWidget(listItem, widget);
        }
    }

    // 完全加载前 15 项
    for (int i = 0; i < qMin(15, count()); ++i) {
        QListWidgetItem *item = this->item(i);
        if (item) {
            ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
            if (widget && !widget->isFullyLoaded()) {
                widget->loadFullData();
                m_loadedItems.insert(i);
            }
        }
    }

    checkVisibleItems();
}

void ChatListWid::checkVisibleItems()
{
    QRect viewportRect = viewport()->rect();
    // 修复：移除错误的 QRegion 调用，改用 QRect 检测可见性
    int loadedCount = 0;
    int maxLoad = m_isFastScrolling ? MAX_LOAD_PER_CHECK_FAST : MAX_LOAD_PER_CHECK;

    // 扩展视口范围（上下各 72 像素）
    viewportRect.adjust(0, -72, 0, 72);

    QSet<int> currentLoadedItems;

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);
        if (!item)
            continue;

        QRect itemRect = visualItemRect(item);
        ChatItemWidget *widget = qobject_cast<ChatItemWidget*>(itemWidget(item));
        if (!widget)
            continue;

        // 优化后的可见性检测（仅使用 viewportRect）
        bool isVisible = viewportRect.intersects(itemRect);
        if (isVisible) {
            // 可见项：加载完整内容
            if (!widget->isFullyLoaded() && loadedCount < maxLoad) {
                widget->loadFullData();
                currentLoadedItems.insert(i);
                ++loadedCount;
            } else if (widget->isFullyLoaded()) {
                currentLoadedItems.insert(i);
            }
        } else {
            // 不可见项：卸载动态内容
            if (widget->isFullyLoaded()) {
                widget->unloadData();
            }
        }
    }

    m_loadedItems = currentLoadedItems;
    m_isFastScrolling = false; // 重置快速滚动状态
}

bool ChatListWid::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::Paint || event->type() == QEvent::Resize) {
        m_loadTimer->start(5);
    }
    return QListWidget::viewportEvent(event);
}

void ChatListWid::wheelEvent(QWheelEvent *event)
{
    QScrollBar *scrollBar = verticalScrollBar();
    if (!scrollBar) {
        event->ignore();
        return;
    }

    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        m_scrollAnimation->stop();
    }

    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();

    int delta = 0;
    if (!numPixels.isNull()) {
        delta = numPixels.y();
    } else if (!numDegrees.isNull()) {
        delta = numDegrees.y();
    }

    m_targetScrollValue = scrollBar->value() - delta;
    m_scrollAnimation->setStartValue(scrollBar->value());
    m_scrollAnimation->setEndValue(m_targetScrollValue);
    m_scrollAnimation->start();

    // 统一触发逻辑，仅启动定时器
    m_isFastScrolling = true; // 标记为快速滚动
    m_loadTimer->start(5);

    event->accept();
}

void ChatListWid::onScrollBarValueChanged(int value)
{
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        return;
    }
    m_targetScrollValue = value;
    m_isFastScrolling = true;
    m_loadTimer->start(5);
    viewport()->update();
}

void ChatListWid::sortChatItems()
{
    std::sort(m_chatItems.begin(), m_chatItems.end(),
              [](const ChatItemData &a, const ChatItemData &b) {
                  return a.lastMessageTime > b.lastMessageTime;
              });
}

int ChatListWid::findInsertPosition(const ChatItemData &data) const
{
    auto it = std::lower_bound(m_chatItems.begin(), m_chatItems.end(), data,
                               [](const ChatItemData &a, const ChatItemData &b) {
                                   return a.lastMessageTime > b.lastMessageTime;
                               });
    return std::distance(m_chatItems.begin(), it);
}

void ChatListWid::addChatItem(const ChatItemData &data)
{
    if (!data.isValid)
        return;

    int insertIndex = findInsertPosition(data);
    m_chatItems.insert(insertIndex, data);

    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(240, 72));
    insertItem(insertIndex, item);
    ChatItemWidget *widget = new ChatItemWidget(data, this);
    setItemWidget(item, widget);

    QRect viewportRect = viewport()->rect();
    QRect itemRect = visualItemRect(item);
    if (viewportRect.intersects(itemRect)) {
        widget->loadFullData();
        m_loadedItems.insert(insertIndex);
    }
}

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
            }
        }
        return;
    }

    removeChatItem(index);
    addChatItem(data);
}

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

QVector<ChatItemData> ChatListWid::getChatItems() const
{
    return m_chatItems;
}

ChatItemData ChatListWid::getChatItemData(int index) const
{
    if (index < 0 || index >= m_chatItems.size())
        return ChatItemData();
    return m_chatItems[index];
}

int ChatListWid::currentChatIndex() const
{
    return currentRow();
}

void ChatListWid::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    update();
}

void ChatListWid::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    update();
}

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
        connect(scrollBar, &QScrollBar::valueChanged, this, &ChatListWid::onScrollBarValueChanged);
    }

    m_scrollAnimation = new QPropertyAnimation(scrollBar, "value", this);
    m_scrollAnimation->setDuration(300);
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);

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

    connect(this, &QListWidget::currentItemChanged, [this](QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous) {
            QWidget *prevWidget = itemWidget(previous);
            if (prevWidget) {
                ChatItemWidget *prevChatWidget = qobject_cast<ChatItemWidget*>(prevWidget);
                if (prevChatWidget) {
                    prevChatWidget->setSelected(false);
                }
            }
        }

        if (current) {
            QWidget *currWidget = itemWidget(current);
            if (currWidget) {
                ChatItemWidget *currChatWidget = qobject_cast<ChatItemWidget*>(currWidget);
                if (currChatWidget) {
                    currChatWidget->setSelected(true);
                    if (!currChatWidget->isFullyLoaded()) {
                        currChatWidget->loadFullData();
                        m_loadedItems.insert(this->row(current));
                    }
                }
            }
        }
    });
}

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
