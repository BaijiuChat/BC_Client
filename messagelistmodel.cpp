#include "MessageListModel.h"

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent) {
}

int MessageListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return messages.size();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= messages.size())
        return QVariant();

    if (role == Qt::UserRole) {
        return QVariant::fromValue(messages[index.row()]);
    }

    return QVariant();
}

void MessageListModel::addMessage(const MessageItemData &message) {
    beginInsertRows(QModelIndex(), messages.size(), messages.size());
    messages.append(message);
    endInsertRows();
}

void MessageListModel::clearMessages() {
    beginResetModel();
    messages.clear();
    endResetModel();
}
