#include "chatlistwid.h"
#include "chatitemwidget.h"

#include "qevent.h"
#include <QScrollBar>  // 添加 QScrollBar 头文件
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
    item->setSizeHint(QSize(240, 72)); // 设置项高度
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

void ChatListWid::wheelEvent(QWheelEvent *event)
{
    QScrollBar *scrollBar = verticalScrollBar();
    if (!scrollBar) {
        event->ignore();
        return;
    }

    // 停止当前动画（如果有）
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        m_scrollAnimation->stop();
    }

    // 计算目标滚动值
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta()/* /8 */;

    int delta = 0;
    if (!numPixels.isNull()) {
        delta = numPixels.y()/* /2 */; // 降低灵敏度，越小越快
    } else if (!numDegrees.isNull()) {
        delta = numDegrees.y()/* /2 */;
    }

    m_targetScrollValue = scrollBar->value() - delta;

    // 设置动画的起始值和结束值
    m_scrollAnimation->setStartValue(scrollBar->value());
    m_scrollAnimation->setEndValue(m_targetScrollValue);
    m_scrollAnimation->start();

    event->accept();
}

void ChatListWid::onScrollBarValueChanged(int value)
{
    // 如果是用户手动拖动滚动条，停止动画并更新目标值
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        return; // 动画正在运行，不干扰
    }

    m_targetScrollValue = value;
    viewport()->update(); // 刷新视图
}

void ChatListWid::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    // 鼠标进入时显示滚动条（通过样式表控制）
    update();
}

void ChatListWid::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    // 鼠标离开时隐藏滚动条（通过样式表控制）
    update();
}

void ChatListWid::initUI()
{
    // 设置列表基本属性
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置滚动模式
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 获取垂直滚动条并设置步长
    QScrollBar *scrollBar = verticalScrollBar();
    if (scrollBar) {
        scrollBar->setSingleStep(10); // 最小步长为 x 像素
        scrollBar->setPageStep(50);   // 页面步长为 x 像素

        // 连接滚动条值变化信号
        connect(scrollBar, &QScrollBar::valueChanged, this, &ChatListWid::onScrollBarValueChanged);
    }

    // 初始化滚动动画
    m_scrollAnimation = new QPropertyAnimation(scrollBar, "value", this);
    m_scrollAnimation->setDuration(300); // 动画持续时间 300ms
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic); // 平滑过渡

    // 应用样式表
    setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: none;"
        "   outline: none;" // 取消虚线边框
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
        // 垂直滚动条
        "QScrollBar:vertical {"
        "   background: transparent;" // 设置隐形
        "   width: 4px;"          // 滑动条宽度
        "   margin: 0px 0px 0px 0px;"
        "   border-radius: 4px;"  // 圆角
        "}"
        "QScrollBar:vertical:hover, QListWidget:hover QScrollBar:vertical {"
        "   background: #F5F5F5;" // 轨道背景色（显现）
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #C0C0C0;" // 滑块背景色
        "   min-height: 20px;"    // 滑块最小高度
        "   border-radius: 4px;"  // 圆角
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #A0A0A0;" // 悬浮时滑块变亮
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"         // 隐藏上下箭头
        "   background: none;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "   background: none;"     // 轨道其他部分透明
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
        "王五",
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
        "明天开会记得带上材料",
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
    for (int i = 0; i < 20; ++i) {
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
