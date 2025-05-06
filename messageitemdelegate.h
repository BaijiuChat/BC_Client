#ifndef MESSAGEITEMDELEGATE_H
#define MESSAGEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "MessageItemData.h"

class MessageItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit MessageItemDelegate(QObject *parent = nullptr);

    // 绘制消息item
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 设置item大小
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    // 绘制头像
    void drawAvatar(QPainter *painter, const QRect &rect, const QString &avatarPath) const;

    // 绘制消息气泡
    void drawMessageBubble(QPainter *painter, const QRect &rect,
                           const QString &content, bool isSelf) const;

    // 绘制ID和时间
    void drawHeader(QPainter *painter, const QRect &rect,
                    const QString &senderName, const QDateTime &time) const;

    // 常量
    static constexpr int AVATAR_SIZE = 40; // 头像大小
    static constexpr int MARGIN = 10;      // 边距
    static constexpr int BUBBLE_RADIUS = 10; // 气泡圆角半径
};

#endif // MESSAGEITEMDELEGATE_H
