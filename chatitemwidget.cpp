#include "chatitemwidget.h"
#include "ui_chatitemwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QDate>
#include <QFontMetrics>
#include <QCache>

// 定义静态头像缓存，最大缓存100个头像
QCache<QString, QPixmap> ChatItemWidget::avatarCache(100); // 缓存 100 个头像

// 构造函数，初始化聊天项控件
ChatItemWidget::ChatItemWidget(const ChatItemData &data, QWidget *parent)
    : QWidget(parent), ui(new Ui::ChatItemWidget), m_data(data), m_isSelected(false), m_isFullyLoaded(false)
{
    ui->setupUi(this);  // 设置UI界面
    initUI();  // 初始化UI组件
    // 设置名称标签的省略显示文本
    QFontMetrics nameMetrics(ui->m_nameLabel->font());
    QString nameElided = nameMetrics.elidedText(m_data.name, Qt::ElideRight, ui->m_nameLabel->width());
    ui->m_nameLabel->setText(nameElided);
}

// 析构函数
ChatItemWidget::~ChatItemWidget()
{
    delete ui;  // 删除UI对象
}

// 更新聊天项数据
void ChatItemWidget::updateData(const ChatItemData &data)
{
    m_data = data;  // 更新数据
    m_isFullyLoaded = false;  // 标记为未完全加载
    // 更新名称标签的省略显示文本
    QFontMetrics nameMetrics(ui->m_nameLabel->font());
    QString nameElided = nameMetrics.elidedText(m_data.name, Qt::ElideRight, ui->m_nameLabel->width());
    ui->m_nameLabel->setText(nameElided);
    // 根据可见性决定加载或卸载数据
    if (isVisible()) {
        loadFullData();
    } else {
        unloadData();
    }
}

// 加载完整数据（头像、消息、时间等）
void ChatItemWidget::loadFullData()
{
    if (m_isFullyLoaded)  // 如果已经加载则直接返回
        return;

    // 加载头像（优先从缓存获取）
    QPixmap avatar;
    if (avatarCache.contains(m_data.avatarPath)) {  // 检查缓存中是否有该头像
        avatar = *avatarCache[m_data.avatarPath];  // 从缓存获取头像
    } else {
        // 从文件加载头像
        avatar = QPixmap(m_data.avatarPath);
        if (avatar.isNull()) {  // 如果加载失败
            // 尝试加载默认头像
            avatar = QPixmap(":/LogReg/avatars/default_avatar.png");
            if (avatar.isNull()) {  // 如果默认头像也加载失败
                // 创建一个灰色占位图
                avatar = QPixmap(40, 40);
                avatar.fill(Qt::lightGray);
            }
        }
        // 将头像处理为圆形并加入缓存
        QPixmap *cachedAvatar = new QPixmap(createCircularPixmap(avatar, 40));
        avatarCache.insert(m_data.avatarPath, cachedAvatar);
        avatar = *cachedAvatar;
    }
    ui->m_avatarLabel->setPixmap(avatar);  // 设置头像

    // 加载消息并设置省略显示
    QFontMetrics msgMetrics(ui->m_messageLabel->font());
    QString msgElided = msgMetrics.elidedText(m_data.lastMessage, Qt::ElideRight, ui->m_messageLabel->width());
    ui->m_messageLabel->setText(msgElided);

    // 加载并格式化时间
    ui->m_timeLabel->setText(formatTime(m_data.lastMessageTime));

    // 更新通知状态（未读消息、静音等）
    updateNotificationStatus();

    m_isFullyLoaded = true;  // 标记为已完全加载
}

// 卸载数据（释放资源）
void ChatItemWidget::unloadData()
{
    if (!m_isFullyLoaded)  // 如果未加载则直接返回
        return;

    ui->m_avatarLabel->setPixmap(QPixmap()); // 清空头像
    ui->m_messageLabel->setText("");  // 清空消息
    ui->m_timeLabel->setText("");  // 清空时间
    ui->m_unreadLabel->hide();  // 隐藏未读计数
    ui->m_mutedLabel->hide();  // 隐藏静音图标

    m_isFullyLoaded = false;  // 标记为未加载
}

// 设置选中状态
void ChatItemWidget::setSelected(bool selected)
{
    m_isSelected = selected;
    if (selected) {
        // 选中状态的样式表
        setStyleSheet(
            "QWidget {"
            "   background-color: #A2A2FE;"
            "   border-radius: 4px;"
            "   color: white;"
            "}"
            "QLabel { color: white; }"
            );
        // 设置消息和时间标签的特殊样式
        ui->m_messageLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 9pt;");
        ui->m_timeLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 8pt;");
    } else {
        // 未选中状态的样式表
        setStyleSheet(
            "QWidget {"
            "   background-color: transparent;"
            "   border-radius: 4px;"
            "}"
            );
        // 重置各标签的样式
        ui->m_nameLabel->setStyleSheet("");
        ui->m_messageLabel->setStyleSheet("color: #666666; font-size: 9pt;");
        ui->m_timeLabel->setStyleSheet("color: #888888; font-size: 8pt;");
        ui->m_unreadLabel->setStyleSheet(
            "background-color: #C7C7C7;"
            "color: white;"
            "border-radius: 8px;"
            "padding: 0 4px;"
            "font-size: 8pt;"
            );
    }
}

// 初始化UI组件
void ChatItemWidget::initUI()
{
    ui->m_mutedLabel->hide();  // 隐藏静音图标
    // 设置未读计数标签的样式
    ui->m_unreadLabel->setStyleSheet(
        "background-color: #C7C7C7;"
        "color: white;"
        "border-radius: 8px;"
        "padding: 0 4px;"
        "font-size: 8pt;"
        );
    ui->m_unreadLabel->hide();  // 隐藏未读计数
    ui->m_avatarLabel->setPixmap(QPixmap());  // 清空头像
    ui->m_messageLabel->setText("");  // 清空消息
    ui->m_timeLabel->setText("");  // 清空时间
}

// 更新通知状态（未读消息、静音等）
void ChatItemWidget::updateNotificationStatus()
{
    if (m_data.muted) {  // 如果是静音状态
        ui->m_unreadLabel->hide();  // 隐藏未读计数
        ui->m_mutedLabel->show();  // 显示静音图标
    } else {
        ui->m_mutedLabel->hide();  // 隐藏静音图标
        if (m_data.unreadCount > 0) {  // 如果有未读消息
            // 设置未读计数文本（超过99显示99+）
            QString unreadText = m_data.unreadCount > 99 ? "99+" : QString::number(m_data.unreadCount);
            ui->m_unreadLabel->setText(unreadText);
            ui->m_unreadLabel->show();  // 显示未读计数
        } else {
            ui->m_unreadLabel->hide();  // 隐藏未读计数
        }
    }
}

// 创建圆形头像
QPixmap ChatItemWidget::createCircularPixmap(const QPixmap &srcPixmap, int diameter)
{
    if (srcPixmap.isNull())  // 如果源图像为空则返回空图像
        return QPixmap();

    // 缩放图像以适应指定直径
    QPixmap scaled = srcPixmap.scaled(diameter, diameter, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 创建透明背景的结果图像
    QPixmap result(diameter, diameter);
    result.fill(Qt::transparent);

    // 使用抗锯齿的绘图器
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 创建圆形裁剪路径
    QPainterPath path;
    path.addEllipse(0, 0, diameter, diameter);
    painter.setClipPath(path);

    // 居中绘制缩放后的图像
    int x = (diameter - scaled.width()) / 2;
    int y = (diameter - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    return result;
}

// 格式化时间显示
QString ChatItemWidget::formatTime(const QDateTime &time)
{
    QDate today = QDate::currentDate();  // 获取当前日期
    QDate messageDate = time.date();  // 获取消息日期

    if (messageDate == today) {  // 如果是今天
        return time.toString("HH:mm");  // 只显示时间
    } else if (messageDate.addDays(1) == today) {  // 如果是昨天
        return "昨天";
    } else if (messageDate.addDays(7) >= today) {  // 如果是一周内
        // 返回星期几
        QString dayOfWeek;
        switch (messageDate.dayOfWeek()) {
        case 1: dayOfWeek = "周一"; break;
        case 2: dayOfWeek = "周二"; break;
        case 3: dayOfWeek = "周三"; break;
        case 4: dayOfWeek = "周四"; break;
        case 5: dayOfWeek = "周五"; break;
        case 6: dayOfWeek = "周六"; break;
        case 7: dayOfWeek = "周日"; break;
        }
        return dayOfWeek;
    } else if (messageDate.year() == today.year()) {  // 如果是今年
        return time.toString("MM-dd");  // 显示月-日
    } else {  // 其他情况
        return time.toString("yyyy-MM-dd");  // 显示完整日期
    }
}
