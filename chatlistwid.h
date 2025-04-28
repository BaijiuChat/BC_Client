#ifndef CHATLISTWID_H
#define CHATLISTWID_H

#include <QListWidget>
#include "chatitemdata.h"

class ChatListWid : public QListWidget
{
    Q_OBJECT
public:
    explicit ChatListWid(QWidget *parent = nullptr);
    ~ChatListWid();

    // 加载会话列表数据
    void loadChatItems(const QVector<ChatItemData> &items);
    // 更新单个item
    void updateChatItem(int index, const ChatItemData &data);
    // 添加会话项
    void addChatItem(const ChatItemData &data);
    // 移除会话项
    void removeChatItem(int index);
    // 获取会话项数据
    QVector<ChatItemData> getChatItems() const;
    // 获取单个会话项数据
    ChatItemData getChatItemData(int index) const;
    // 返回当前选中的会话项索引
    int currentChatIndex() const;

private:
    // 存储会话数据
    QVector<ChatItemData> m_chatItems;

    // 初始化UI
    void initUI();
    // 创建测试数据
    QVector<ChatItemData> createTestData();
};

#endif // CHATLISTWID_H
