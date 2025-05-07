#include "MessageItemDelegate.h"
#include "qpainterpath.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPixmap>
#include <QTextDocument>

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
    int rightMargin = MARGIN / 2; // 减小右侧边距
    int viewWidth = index.model()->parent()->property("viewportWidth").toInt();
    if (viewWidth <= 0) viewWidth = option.rect.width(); // 兜底 fallback
    int width = viewWidth - x - rightMargin; // 调整可用宽度

    // 头像位置
    QRect avatarRect;
    if (data.isSelf) {
        avatarRect = QRect(rect.right() - rightMargin - AVATAR_SIZE, rect.y(), AVATAR_SIZE, AVATAR_SIZE);
    } else {
        avatarRect = QRect(x, rect.y(), AVATAR_SIZE, AVATAR_SIZE);
    }

    // 设置发送者名字和时间戳的字体（8号字体）
    QFont nameFont = painter->font();
    nameFont.setPointSize(8); // 固定为 8 号字体
    painter->setFont(nameFont);
    QFontMetrics nameFm(nameFont);

    // 计算文本区域 - 设置最大宽度限制
    int maxBubbleWidth = viewWidth * 0.8;
    int bubbleMaxWidth = qMin(width - AVATAR_SIZE - 3 * MARGIN, maxBubbleWidth);

    // 设置气泡文字的字体（11号字体，细体）
    QFont bubbleFont = painter->font();
    bubbleFont.setPointSize(11);
    bubbleFont.setWeight(QFont::Light);

    // 使用 QTextDocument 计算文本区域，动态调整宽度
    QTextDocument textDoc;
    textDoc.setDefaultFont(bubbleFont);
    // 先不设置宽度，让文档自由计算实际宽度
    textDoc.setTextWidth(-1); // 不限制宽度，计算自然宽度
    textDoc.setPlainText(data.content);
    QSizeF textSize = textDoc.size();

    // 如果自然宽度超过最大宽度，则限制宽度并重新计算
    if (textSize.width() > bubbleMaxWidth) {
        textDoc.setTextWidth(bubbleMaxWidth);
        textSize = textDoc.size(); // 重新获取尺寸
    }

    // 修改这里：微调文本高度以匹配实际绘制
    QRect textRect(0, 0, textSize.width(), qMax(textSize.height() - MARGIN/2, 0.0));

    // 消息气泡区域 - 修改内边距
    QRect bubbleRect = textRect;
    bubbleRect.adjust(-MARGIN, -MARGIN, MARGIN, MARGIN/2); // 底部减小内边距

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

    // 使用8号字体计算名字和时间戳高度
    QFont nameFont = option.font;
    nameFont.setPointSize(8);
    QFontMetrics nameFm(nameFont);
    int nameHeight = nameFm.height();

    // 获取实际 viewport 宽度
    int viewWidth = index.model()->parent()->property("viewportWidth").toInt();
    if (viewWidth <= 0) viewWidth = option.rect.width(); // 兜底 fallback
    int width = viewWidth;
    int maxBubbleWidth = width * 0.8;
    int bubbleMaxWidth = qMin(width - AVATAR_SIZE - 3 * MARGIN, maxBubbleWidth);

    // 使用 QTextDocument 计算文本区域，动态调整宽度
    QTextDocument textDoc;
    textDoc.setDefaultFont(bubbleFont);
    // 先不设置宽度，让文档自由计算实际宽度
    textDoc.setTextWidth(-1); // 不限制宽度，计算自然宽度
    textDoc.setPlainText(data.content);
    QSizeF textSize = textDoc.size();

    // 如果自然宽度超过最大宽度，则限制宽度并重新计算
    if (textSize.width() > bubbleMaxWidth) {
        textDoc.setTextWidth(bubbleMaxWidth);
        textSize = textDoc.size(); // 重新获取尺寸
    }

    // 修改这里：微调文本高度以匹配实际绘制
    QRect textRect(0, 0, textSize.width(), qMax(textSize.height() - MARGIN/2, 0.0));

    // 考虑气泡内边距 - 修改这里：调整内边距，底部减小
    int bubbleHeight = textRect.height() + MARGIN + MARGIN/2; // 顶部保留完整MARGIN，底部使用MARGIN/2

    // 计算总高度: 确保包含用户名、气泡和底部间距
    int contentHeight = nameHeight + bubbleHeight;
    int height = qMax(contentHeight, AVATAR_SIZE) + MARGIN;

    // 微调整体高度，可能不需要额外间距了
    height += 3; // 减少额外间距从5到3

    return QSize(option.rect.width(), height);
}

void MessageItemDelegate::drawAvatar(QPainter *painter, const QRect &rect,
                                     const QString &avatarPath) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPixmap avatar(avatarPath);
    if (!avatar.isNull()) {
        avatar = avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainterPath path;
        path.addEllipse(rect);
        painter->setClipPath(path);

        painter->drawPixmap(rect, avatar);
    } else {
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect);
    }

    painter->restore();
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

    // 绘制文本 - 减小底部内边距
    painter->setPen(Qt::black);
    painter->drawText(rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN),
                      Qt::AlignLeft | Qt::TextWordWrap, content);
}
