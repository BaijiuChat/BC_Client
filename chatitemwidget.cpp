#include "chatitemwidget.h"
#include "ui_chatitemwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QDate>
#include <QFontMetrics>
#include <QCache>

// 定义静态头像缓存
QCache<QString, QPixmap> ChatItemWidget::avatarCache(100); // 缓存 100 个头像

ChatItemWidget::ChatItemWidget(const ChatItemData &data, QWidget *parent)
    : QWidget(parent), ui(new Ui::ChatItemWidget), m_data(data), m_isSelected(false), m_isFullyLoaded(false)
{
    ui->setupUi(this);
    initUI();
    QFontMetrics nameMetrics(ui->m_nameLabel->font());
    QString nameElided = nameMetrics.elidedText(m_data.name, Qt::ElideRight, ui->m_nameLabel->width());
    ui->m_nameLabel->setText(nameElided);
}

ChatItemWidget::~ChatItemWidget()
{
    delete ui;
}

void ChatItemWidget::updateData(const ChatItemData &data)
{
    m_data = data;
    m_isFullyLoaded = false;
    QFontMetrics nameMetrics(ui->m_nameLabel->font());
    QString nameElided = nameMetrics.elidedText(m_data.name, Qt::ElideRight, ui->m_nameLabel->width());
    ui->m_nameLabel->setText(nameElided);
    if (isVisible()) {
        loadFullData();
    } else {
        unloadData();
    }
}

void ChatItemWidget::loadFullData()
{
    if (m_isFullyLoaded)
        return;

    // 加载头像（优先从缓存获取）
    QPixmap avatar;
    if (avatarCache.contains(m_data.avatarPath)) {
        avatar = *avatarCache[m_data.avatarPath];
    } else {
        avatar = QPixmap(m_data.avatarPath);
        if (avatar.isNull()) {
            avatar = QPixmap(":/LogReg/avatars/default_avatar.png");
            if (avatar.isNull()) {
                avatar = QPixmap(40, 40);
                avatar.fill(Qt::lightGray);
            }
        }
        QPixmap *cachedAvatar = new QPixmap(createCircularPixmap(avatar, 40));
        avatarCache.insert(m_data.avatarPath, cachedAvatar);
        avatar = *cachedAvatar;
    }
    ui->m_avatarLabel->setPixmap(avatar);

    // 加载消息
    QFontMetrics msgMetrics(ui->m_messageLabel->font());
    QString msgElided = msgMetrics.elidedText(m_data.lastMessage, Qt::ElideRight, ui->m_messageLabel->width());
    ui->m_messageLabel->setText(msgElided);

    // 加载时间
    ui->m_timeLabel->setText(formatTime(m_data.lastMessageTime));

    // 更新通知状态
    updateNotificationStatus();

    m_isFullyLoaded = true;
}

void ChatItemWidget::unloadData()
{
    if (!m_isFullyLoaded)
        return;

    ui->m_avatarLabel->setPixmap(QPixmap()); // 清空头像
    ui->m_messageLabel->setText("");
    ui->m_timeLabel->setText("");
    ui->m_unreadLabel->hide();
    ui->m_mutedLabel->hide();

    m_isFullyLoaded = false;
}

void ChatItemWidget::setSelected(bool selected)
{
    m_isSelected = selected;
    if (selected) {
        setStyleSheet(
            "QWidget {"
            "   background-color: #A2A2FE;"
            "   border-radius: 4px;"
            "   color: white;"
            "}"
            "QLabel { color: white; }"
        );
        ui->m_messageLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 9pt;");
        ui->m_timeLabel->setStyleSheet("color: rgba(255, 255, 255, 0.8); font-size: 8pt;");
    } else {
        setStyleSheet(
            "QWidget {"
            "   background-color: transparent;"
            "   border-radius: 4px;"
            "}"
        );
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

void ChatItemWidget::initUI()
{
    ui->m_mutedLabel->hide();
    ui->m_unreadLabel->setStyleSheet(
        "background-color: #C7C7C7;"
        "color: white;"
        "border-radius: 8px;"
        "padding: 0 4px;"
        "font-size: 8pt;"
    );
    ui->m_unreadLabel->hide();
    ui->m_avatarLabel->setPixmap(QPixmap());
    ui->m_messageLabel->setText("");
    ui->m_timeLabel->setText("");
}

void ChatItemWidget::updateNotificationStatus()
{
    if (m_data.muted) {
        ui->m_unreadLabel->hide();
        ui->m_mutedLabel->show();
    } else {
        ui->m_mutedLabel->hide();
        if (m_data.unreadCount > 0) {
            QString unreadText = m_data.unreadCount > 99 ? "99+" : QString::number(m_data.unreadCount);
            ui->m_unreadLabel->setText(unreadText);
            ui->m_unreadLabel->show();
        } else {
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
        return time.toString("HH:mm");
    } else if (messageDate.addDays(1) == today) {
        return "昨天";
    } else if (messageDate.addDays(7) >= today) {
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
        return time.toString("MM-dd");
    } else {
        return time.toString("yyyy-MM-dd");
    }
}
