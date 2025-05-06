#ifndef MESSAGEITEMDATA_H
#define MESSAGEITEMDATA_H

#include <QDateTime>
#include <QString>

struct MessageItemData {
    int messageId;               // 消息唯一ID
    int senderId;               // 发送者ID（关联ChatItemData的id）
    QString senderName;          // 发送者用户名
    QString avatarPath;          // 发送者头像路径
    QString content;             // 消息内容（当前为文字，未来可扩展为其他类型）
    QDateTime sendTime;          // 发送时间
    bool isSelf;                 // 是否为自己发送的消息
    enum MessageType { Text, Image, File }; // 消息类型（为未来扩展准备）
    MessageType type;            // 当前消息类型

    // 构造函数
    MessageItemData(int _messageId = 0,
                    int _senderId = 0,
                    const QString &_senderName = "",
                    const QString &_avatarPath = "",
                    const QString &_content = "",
                    const QDateTime &_time = QDateTime::currentDateTime(),
                    bool _isSelf = false,
                    MessageType _type = Text)
        : messageId(_messageId), senderId(_senderId), senderName(_senderName),
        avatarPath(_avatarPath), content(_content), sendTime(_time),
        isSelf(_isSelf), type(_type)
    {}
};

#endif // MESSAGEITEMDATA_H
