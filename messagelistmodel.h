#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include <QAbstractListModel>
#include "MessageItemData.h"

class MessageListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit MessageListModel(QObject *parent = nullptr);

    // 必须实现的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 添加消息
    void addMessage(const MessageItemData &message);

    // 清空消息
    void clearMessages();

private:
    QList<MessageItemData> messages; // 存储消息列表
};

#endif // MESSAGELISTMODEL_H
