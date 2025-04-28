#ifndef CHATITEMDATA_H
#define CHATITEMDATA_H

#include <QDateTime>
#include <QString>
#include <QPixmap>

// 聊天项数据结构
struct ChatItemData {
    int id;                     // 唯一ID
    QString avatarPath;         // 头像路径
    QString name;               // 用户名或群名
    QString lastMessage;        // 最后一条消息预览
    QDateTime lastMessageTime;  // 最后消息时间
    int unreadCount;            // 未读消息数
    bool muted;                 // 是否免打扰
    bool isValid;               // 是否有效(用于过滤)

    // 构造函数
    ChatItemData(int _id = 0,
                 const QString &_avatar = "",
                 const QString &_name = "",
                 const QString &_lastMsg = "",
                 const QDateTime &_time = QDateTime::currentDateTime(),
                 int _unread = 0,
                 bool _muted = false,
                 bool _valid = true)
        : id(_id), avatarPath(_avatar), name(_name),
        lastMessage(_lastMsg), lastMessageTime(_time),
        unreadCount(_unread), muted(_muted), isValid(_valid)
    {}
};


#endif // CHATITEMDATA_H
