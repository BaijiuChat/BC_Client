#include "MessageItemDelegate.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPixmap>

MessageItemDelegate::MessageItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

void MessageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 获取消息数据
    const MessageItemData data = index.data(Qt::UserRole).value<MessageItemData>();

    // 计算布局
    QRect rect = option.rect;
    int x = rect.x() + MARGIN;
    int y = rect.y() + MARGIN;
    int width = rect.width() - 2 * MARGIN;

    // 头像位置
    QRect avatarRect;
    if (data.isSelf) {
        avatarRect = QRect(rect.right() - MARGIN - AVATAR_SIZE, y, AVATAR_SIZE, AVATAR_SIZE);
    } else {
        avatarRect = QRect(x, y, AVATAR_SIZE, AVATAR_SIZE);
    }

    // 消息主体区域
    int bubbleMaxWidth = width - AVATAR_SIZE - 3 * MARGIN;
    QFontMetrics fm(painter->font());
    QRect bubbleRect = fm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                       Qt::AlignLeft | Qt::TextWordWrap,
                                       data.content);
    bubbleRect.adjust(-MARGIN, -MARGIN, MARGIN, MARGIN); // 气泡内边距

    // 消息主体位置
    QRect messageRect;
    if (data.isSelf) {
        messageRect = QRect(rect.right() - MARGIN - AVATAR_SIZE - MARGIN - bubbleRect.width(),
                            y + AVATAR_SIZE + MARGIN,
                            bubbleRect.width(), bubbleRect.height());
    } else {
        messageRect = QRect(x + AVATAR_SIZE + MARGIN,
                            y + AVATAR_SIZE + MARGIN,
                            bubbleRect.width(), bubbleRect.height());
    }

    // 绘制头像
    drawAvatar(painter, avatarRect, data.avatarPath);

    // 绘制ID和时间
    drawHeader(painter, QRect(messageRect.x(), y, messageRect.width(), AVATAR_SIZE),
               data.senderName, data.sendTime);

    // 绘制消息气泡
    drawMessageBubble(painter, messageRect, data.content, data.isSelf);

    painter->restore();
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    // 获取消息数据
    const MessageItemData data = index.data(Qt::UserRole).value<MessageItemData>();

    // 计算高度
    QFontMetrics fm(option.font);
    int bubbleMaxWidth = option.rect.width() - 2 * MARGIN - AVATAR_SIZE - 3 * MARGIN;
    QRect bubbleRect = fm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                       Qt::AlignLeft | Qt::TextWordWrap,
                                       data.content);
    bubbleRect.adjust(-MARGIN, -MARGIN, MARGIN, MARGIN);

    int height = AVATAR_SIZE + bubbleRect.height() + 3 * MARGIN;
    return QSize(option.rect.width(), height);
}

void MessageItemDelegate::drawAvatar(QPainter *painter, const QRect &rect,
                                     const QString &avatarPath) const {
    QPixmap avatar(avatarPath);
    if (!avatar.isNull()) {
        avatar = avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(rect, avatar);
    } else {
        // 绘制默认头像（简单矩形）
        painter->setBrush(Qt::gray);
        painter->drawEllipse(rect);
    }
}

void MessageItemDelegate::drawMessageBubble(QPainter *painter, const QRect &rect,
                                            const QString &content, bool isSelf) const {
    // 设置气泡颜色
    painter->setBrush(isSelf ? QColor(0, 255, 0, 180) : QColor(255, 255, 255, 180));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);

    // 绘制文本
    painter->setPen(Qt::black);
    painter->drawText(rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN),
                      Qt::AlignLeft | Qt::TextWordWrap, content);
}

void MessageItemDelegate::drawHeader(QPainter *painter, const QRect &rect,
                                     const QString &senderName, const QDateTime &time) const {
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignLeft, senderName);

    font.setBold(false);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignRight, time.toString("hh:mm"));
}
