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
    QPoint numDegrees = event->angleDelta() /* /8 */;

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
    testData.reserve(100); // 预分配空间提高性能

    // 头像路径保持不变
    QStringList avatarPaths = {
        ":/LogReg/avatars/avatar1.png",
        ":/LogReg/avatars/avatar2.png",
        ":/LogReg/avatars/avatar3.png",
        ":/LogReg/avatars/avatar4.png",
        ":/LogReg/avatars/avatar5.png",
        ":/LogReg/avatars/avatar6.png",
    };

    // 扩展为100个随机中文名（个人+群组）
    QStringList names;
    QStringList surnames = {"赵","钱","孙","李","周","吴","郑","王","冯","陈","褚","卫","蒋","沈","韩","杨"};
    QStringList givenNames = {"伟","芳","娜","秀英","敏","静","丽","强","磊","军","洋","勇","艳","杰","娟","涛"};
    QStringList groupSuffixes = {"交流群","讨论组","粉丝群","亲友团","同学会","工作群","项目组","游戏群"};

    // 生成60个个人联系人
    for(int i=0; i<60; ++i) {
        names.append(surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                     givenNames[QRandomGenerator::global()->bounded(givenNames.size())]);
    }

    // 生成40个群组
    for(int i=0; i<40; ++i) {
        QString name = surnames[QRandomGenerator::global()->bounded(surnames.size())] +
                       givenNames[QRandomGenerator::global()->bounded(givenNames.size())] +
                       "的" + groupSuffixes[QRandomGenerator::global()->bounded(groupSuffixes.size())];
        names.append(name);
    }

    // 更丰富的消息模板
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

    // 生成100条测试数据
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < 100; ++i) {
        ChatItemData item;
        item.id = i + 1;
        item.avatarPath = avatarPaths[i % avatarPaths.size()];

        // 随机时间（最近30天内）
        int randomMinutes = QRandomGenerator::global()->bounded(43200); // 30天*24小时*60分钟
        item.lastMessageTime = now.addSecs(-randomMinutes * 60);

        // 随机名称和消息
        item.name = names[i];

        // 50%概率显示发送者
        if (QRandomGenerator::global()->bounded(100) < 50 && !item.name.contains("群")) {
            QString sender = names[QRandomGenerator::global()->bounded(60)] + ": ";
            item.lastMessage = sender + messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        } else {
            item.lastMessage = messageTemplates[QRandomGenerator::global()->bounded(messageTemplates.size())];
        }

        // 随机未读消息（个人30%概率，群组60%概率）
        bool isGroup = item.name.contains("群");
        if (QRandomGenerator::global()->bounded(100) < (isGroup ? 60 : 30)) {
            item.unreadCount = QRandomGenerator::global()->bounded(1, isGroup ? 150 : 20);
        }

        // 免打扰状态（个人10%概率，群组30%概率）
        item.muted = QRandomGenerator::global()->bounded(100) < (isGroup ? 30 : 10);

        item.isValid = true;
        testData.append(item);
    }

    // 打乱顺序使数据更随机
    std::shuffle(testData.begin(), testData.end(), *QRandomGenerator::global());
    return testData;
}
