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

    // 计算文本区域
    int bubbleMaxWidth = width - AVATAR_SIZE - 4 * MARGIN;
    QFontMetrics fm(painter->font());
    QRect textRect = fm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                     Qt::AlignLeft | Qt::TextWordWrap,
                                     data.content);

    // 消息气泡区域 - 根据文本内容适当调整大小
    QRect bubbleRect = textRect;
    bubbleRect.adjust(-MARGIN, -MARGIN, MARGIN, MARGIN);

    // 绘制头像
    drawAvatar(painter, avatarRect, data.avatarPath);

    // 绘制用户名 - 调整位置使其紧贴气泡
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QRect nameRect;
    if (data.isSelf) {
        nameRect = QRect(rect.right() - AVATAR_SIZE - MARGIN - bubbleRect.width() - MARGIN,
                         y, bubbleRect.width(), fm.height());
        painter->drawText(nameRect, Qt::AlignRight, data.senderName);
    } else {
        nameRect = QRect(x + AVATAR_SIZE + MARGIN, y,
                         bubbleRect.width(), fm.height());
        painter->drawText(nameRect, Qt::AlignLeft, data.senderName);
    }

    // 恢复普通字体
    font.setBold(false);
    painter->setFont(font);

    // 时间戳
    QString timeStr = data.sendTime.toString("hh:mm");
    int timeWidth = fm.horizontalAdvance(timeStr);
    QRect timeRect;

    // 调整气泡和时间戳位置
    if (data.isSelf) {
        // 右对齐布局
        bubbleRect.moveTopRight(QPoint(rect.right() - AVATAR_SIZE - 2 * MARGIN,
                                       nameRect.bottom() + MARGIN / 2));

        // 时间戳在气泡左侧
        timeRect = QRect(bubbleRect.left() - timeWidth - MARGIN,
                         bubbleRect.bottom() - fm.height(),
                         timeWidth, fm.height());
    } else {
        // 左对齐布局
        bubbleRect.moveTopLeft(QPoint(x + AVATAR_SIZE + MARGIN,
                                      nameRect.bottom() + MARGIN / 2));

        // 时间戳在气泡右侧
        timeRect = QRect(bubbleRect.right() + MARGIN,
                         bubbleRect.bottom() - fm.height(),
                         timeWidth, fm.height());
    }

    // 绘制消息气泡
    drawMessageBubble(painter, bubbleRect, data.content, data.isSelf);

    // 绘制时间戳
    painter->drawText(timeRect, data.isSelf ? Qt::AlignRight : Qt::AlignLeft, timeStr);

    painter->restore();
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    // 获取消息数据
    const MessageItemData data = index.data(Qt::UserRole).value<MessageItemData>();

    // 计算高度
    QFontMetrics fm(option.font);
    int bubbleMaxWidth = option.rect.width() - 2 * MARGIN - AVATAR_SIZE - 4 * MARGIN;

    // 用户名高度
    int nameHeight = fm.height();

    // 消息文本高度
    QRect textRect = fm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                     Qt::AlignLeft | Qt::TextWordWrap,
                                     data.content);

    // 总高度 = 头像高度 或 (用户名高度 + 气泡高度 + 间距)，取较大值
    int contentHeight = nameHeight + MARGIN/2 + textRect.height() + MARGIN * 2;
    int height = qMax(AVATAR_SIZE, contentHeight) + 2 * MARGIN;

    return QSize(option.rect.width(), height);
}

void MessageItemDelegate::drawAvatar(QPainter *painter, const QRect &rect,
                                     const QString &avatarPath) const {
    QPixmap avatar(avatarPath);
    if (!avatar.isNull()) {
        avatar = avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(rect, avatar);
    } else {
        painter->setBrush(Qt::gray);
        painter->drawEllipse(rect);
    }
}

void MessageItemDelegate::drawMessageBubble(QPainter *painter, const QRect &rect,
                                            const QString &content, bool isSelf) const {
    // 设置气泡颜色
    painter->setBrush(isSelf ? QColor("#99FFCA") : QColor("#F1E9FA"));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);

    // 绘制文本
    painter->setPen(Qt::black);
    painter->drawText(rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN),
                      Qt::AlignLeft | Qt::TextWordWrap, content);
}
