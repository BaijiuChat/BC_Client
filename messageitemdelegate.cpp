#include "MessageItemDelegate.h"
#include "qpainterpath.h"
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

    // 减小右侧边距，使消息更靠近右侧
    int x = rect.x() + MARGIN; // 从左侧增加偏移
    int rightMargin = MARGIN / 2; // 减小右侧边距，原来是MARGIN
    int width = rect.width() - x - rightMargin; // 调整可用宽度

    // 头像位置
    QRect avatarRect;
    if (data.isSelf) {
        // 让自己的头像更靠近右边
        avatarRect = QRect(rect.right() - rightMargin - AVATAR_SIZE, rect.y(), AVATAR_SIZE, AVATAR_SIZE);
    } else {
        avatarRect = QRect(x, rect.y(), AVATAR_SIZE, AVATAR_SIZE);
    }

    // 设置发送者名字和时间戳的字体（8号字体）
    QFont nameFont = painter->font();
    nameFont.setPointSize(8); // 固定为 8 号字体
    painter->setFont(nameFont);
    QFontMetrics nameFm(nameFont);

    // 计算文本区域 - 设置最大宽度限制并增加宽度
    int maxBubbleWidth = option.rect.width() * 0.8; // 增加最大宽度比例，原来是0.8
    int bubbleMaxWidth = qMin(width - AVATAR_SIZE - 3 * MARGIN, maxBubbleWidth); // 减少边距计算，原来是4*MARGIN

    // 设置气泡文字的字体（11号字体，细体）
    QFont bubbleFont = painter->font();
    bubbleFont.setPointSize(11);
    bubbleFont.setWeight(QFont::Light);
    QFontMetrics bubbleFm(bubbleFont);
    QRect textRect = bubbleFm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                           Qt::AlignLeft | Qt::TextWordWrap,
                                           data.content);

    // 消息气泡区域 - 增大内边距
    QRect bubbleRect = textRect;
    bubbleRect.adjust(-MARGIN, -MARGIN, MARGIN, MARGIN); // 增大内边距

    // 绘制头像
    drawAvatar(painter, avatarRect, data.avatarPath);

    // 绘制用户名（使用8号字体，不加粗）
    painter->setFont(nameFont);

    QRect nameRect;
    if (data.isSelf) {
        nameRect = QRect(rect.right() - rightMargin - AVATAR_SIZE - bubbleRect.width() - MARGIN,
                         rect.y(), bubbleRect.width(), nameFm.height());
        painter->drawText(nameRect, Qt::AlignRight, data.senderName);
    } else {
        nameRect = QRect(x + AVATAR_SIZE + MARGIN, rect.y(),
                         bubbleRect.width(), nameFm.height());
        painter->drawText(nameRect, Qt::AlignLeft, data.senderName);
    }

    // 时间戳（使用8号字体）
    nameFont.setBold(false);
    painter->setFont(nameFont);
    QString timeStr = data.sendTime.toString("hh:mm");
    int timeWidth = nameFm.horizontalAdvance(timeStr);
    QRect timeRect;

    // 调整气泡和时间戳位置，减小右侧边距
    if (data.isSelf) {
        bubbleRect.moveTopRight(QPoint(rect.right() - rightMargin - AVATAR_SIZE - MARGIN,
                                       nameRect.bottom()));
        timeRect = QRect(bubbleRect.left() - timeWidth - MARGIN / 2,
                         bubbleRect.bottom() - nameFm.height(),
                         timeWidth, nameFm.height());
    } else {
        bubbleRect.moveTopLeft(QPoint(x + AVATAR_SIZE + MARGIN,
                                      nameRect.bottom()));
        timeRect = QRect(bubbleRect.right() + MARGIN / 2,
                         bubbleRect.bottom() - nameFm.height(),
                         timeWidth, nameFm.height());
    }

    // 绘制消息气泡
    drawMessageBubble(painter, bubbleRect, data.content, data.isSelf);

    // 绘制时间戳
    painter->setFont(nameFont);
    painter->drawText(timeRect, data.isSelf ? Qt::AlignRight : Qt::AlignLeft, timeStr);

    painter->restore();
}

QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    const MessageItemData data = index.data(Qt::UserRole).value<MessageItemData>();

    // 使用11号字体计算气泡高度
    QFont bubbleFont = option.font;
    bubbleFont.setPointSize(11);
    bubbleFont.setWeight(QFont::Light);
    QFontMetrics bubbleFm(bubbleFont);

    // 使用相同的宽度限制计算，增加宽度比例
    int maxBubbleWidth = option.rect.width() * 0.8; // 增加宽度比例，与paint方法保持一致
    int width = option.rect.width();
    int bubbleMaxWidth = qMin(width - AVATAR_SIZE - 3 * MARGIN, maxBubbleWidth); // 减少边距计算

    QRect textRect = bubbleFm.boundingRect(QRect(0, 0, bubbleMaxWidth, 0),
                                           Qt::AlignLeft | Qt::TextWordWrap,
                                           data.content);

    // 使用8号字体计算名字高度
    QFont nameFont = option.font;
    nameFont.setPointSize(8);
    // QFontMetrics nameFm(nameFont);
    // int nameHeight = nameFm.height();

    // 计算总高度
    int height = AVATAR_SIZE + textRect.height();
    return QSize(option.rect.width(), height);
}

void MessageItemDelegate::drawAvatar(QPainter *painter, const QRect &rect,
                                     const QString &avatarPath) const {
    painter->save(); // 保存当前painter状态
    painter->setRenderHint(QPainter::Antialiasing, true); // 确保抗锯齿

    QPixmap avatar(avatarPath);
    if (!avatar.isNull()) {
        avatar = avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 创建圆形裁剪路径
        QPainterPath path;
        path.addEllipse(rect);
        painter->setClipPath(path);

        // 绘制头像（会被裁剪为圆形）
        painter->drawPixmap(rect, avatar);
    } else {
        // 绘制默认圆形头像
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect);
    }

    painter->restore(); // 恢复painter状态
}

void MessageItemDelegate::drawMessageBubble(QPainter *painter, const QRect &rect,
                                            const QString &content, bool isSelf) const {
    // 设置气泡颜色
    painter->setBrush(isSelf ? QColor("#99FFCA") : QColor("#F1E9FA"));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, BUBBLE_RADIUS, BUBBLE_RADIUS);

    // 设置字体（11号，细体）
    QFont font = painter->font();
    font.setPointSize(11);
    font.setWeight(QFont::Light);
    painter->setFont(font);

    // 绘制文本 - 增大内边距
    painter->setPen(Qt::black);
    painter->drawText(rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN),
                      Qt::AlignLeft | Qt::TextWordWrap, content);
}
