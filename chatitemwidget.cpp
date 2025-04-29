#include "chatitemwidget.h"
#include "ui_chatitemwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QDate>
#include <QFontMetrics>

ChatItemWidget::ChatItemWidget(const ChatItemData &data, QWidget *parent)
    : QWidget(parent), ui(new Ui::ChatItemWidget), m_data(data), m_isSelected(false)
{
    ui->setupUi(this);
    initUI();
    loadData();
}

ChatItemWidget::~ChatItemWidget()
{
    delete ui;
}

void ChatItemWidget::updateData(const ChatItemData &data)
{
    m_data = data;
    loadData();
}

ChatItemData ChatItemWidget::getData() const
{
    return m_data;
}

void ChatItemWidget::setSelected(bool selected)
{
    m_isSelected = selected;

    // 根据选中状态更新背景色
    if (selected) {
        setStyleSheet(
            "QWidget {"
            "   background-color: #A2A2FE;" // 紫色背景
            "   border-radius: 4px;"
            "   color: white;"              // 白色文本
            "}"
            "QLabel { color: white; }"     // 所有标签文本变白
            );
        // 消息预览文本保持灰白色
        ui->m_messageLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 9pt;");
        // 时间文本保持灰白色
        ui->m_timeLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 8pt;");
    } else {
        setStyleSheet(
            "QWidget {"
            "   background-color: transparent;"  // 白色背景
            "   border-radius: 4px;"
            "}"
            );
        // 恢复默认文本颜色
        ui->m_nameLabel->setStyleSheet("");
        ui->m_messageLabel->setStyleSheet("color: #666666; font-size: 9pt;");
        ui->m_timeLabel->setStyleSheet("color: #888888; font-size: 8pt;");

        // 恢复未读消息样式
        ui->m_unreadLabel->setStyleSheet(
            "background-color: #C7C7C7;"  // 红色背景
            "color: white;"               // 白色文本
            "border-radius: 8px;"         // 圆角
            "padding: 0 4px;"             // 水平内边距
            "font-size: 8pt;"             // 字体大小
            );
    }
}

void ChatItemWidget::initUI()
{
    ui->m_mutedLabel->hide();   // 隐藏免打扰图标
    // 未读消息气泡
    ui->m_unreadLabel->setStyleSheet(
        "background-color: #C7C7C7;"  // 红色背景
        "color: white;"               // 白色文本
        "border-radius: 8px;"         // 圆角
        "padding: 0 4px;"             // 水平内边距
        "font-size: 8pt;"             // 字体大小
        );
    ui->m_unreadLabel->hide(); // 默认隐藏
}

void ChatItemWidget::loadData()
{
    // 设置头像
    QPixmap avatar(m_data.avatarPath);
    if(avatar.isNull()){
        avatar = QPixmap(":/LogReg/avatars/default_avatar.png");
        if(avatar.isNull()){
            avatar = QPixmap(40, 40);
            avatar.fill(Qt::lightGray);
        }
    }
    ui->m_avatarLabel->setPixmap(createCircularPixmap(avatar, 40));

    // 设置标题（带省略）
    QFontMetrics nameMetrics(ui->m_nameLabel->font());
    QString nameElided = nameMetrics.elidedText(m_data.name, Qt::ElideRight, ui->m_nameLabel->width());
    ui->m_nameLabel->setText(nameElided);

    // 设置消息（带省略）
    QFontMetrics msgMetrics(ui->m_messageLabel->font());
    QString msgElided = msgMetrics.elidedText(m_data.lastMessage, Qt::ElideRight, ui->m_messageLabel->width());
    ui->m_messageLabel->setText(msgElided);

    // // 设置标题
    // ui->m_nameLabel->setText(m_data.name);
    // // 设置最后一条消息
    // ui->m_messageLabel->setText(m_data.lastMessage);

    // 设置时间
    ui->m_timeLabel->setText(formatTime(m_data.lastMessageTime));
    // 更新通知状态
    updateNotificationStatus();
}

void ChatItemWidget::updateNotificationStatus()
{
    // 处理免打扰
    if(m_data.muted){
        ui->m_unreadLabel->hide();
        ui->m_mutedLabel->show();
    }
    else {
        ui->m_mutedLabel->hide();
        // 处理99+
        if (m_data.unreadCount > 0){
            QString unreadText = m_data.unreadCount > 99 ? "99+" : QString::number(m_data.unreadCount);
            ui->m_unreadLabel->setText(unreadText);
            ui->m_unreadLabel->show();
        }
        else {
            ui->m_unreadLabel->hide();
        }
    }
}

QPixmap ChatItemWidget::createCircularPixmap(const QPixmap &srcPixmap, int diameter)
{
    if (srcPixmap.isNull())
        return QPixmap();

    QPixmap scaled = srcPixmap.scaled(diameter, diameter, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPixmap result(diameter, diameter);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    path.addEllipse(0, 0, diameter, diameter);
    painter.setClipPath(path);

    // 确保图像居中
    int x = (diameter - scaled.width()) / 2;
    int y = (diameter - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    return result;
}



QString ChatItemWidget::formatTime(const QDateTime &time)
{
    QDate today = QDate::currentDate();
    QDate messageDate = time.date();

    if (messageDate == today) {
        // 今天的消息只显示时间
        return time.toString("HH:mm");
    } else if (messageDate.addDays(1) == today) {
        // 昨天的消息
        return "昨天";
    } else if (messageDate.addDays(7) >= today) {
        // 一周内的消息显示星期几
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
    } else if (messageDate.year() == today.year()) {
        // 今年内的其他日期
        return time.toString("MM-dd");
    } else {
        // 往年的消息
        return time.toString("yyyy-MM-dd");
    }
}
