#ifndef CHATLISTWID_H
#define CHATLISTWID_H

#include <QListWidget>
#include <QTimer>
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

protected:
    void wheelEvent(QWheelEvent *event) override; // 重写滚轮事件
    void enterEvent(QEnterEvent *event) override; // 鼠标进入事件
    void leaveEvent(QEvent *event) override;     // 鼠标离开事件
    bool viewportEvent(QEvent *event) override;

private slots:
    void checkVisibleItems(); // 检查可见项并触发懒加载
    void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    // 存储会话数据
    QVector<ChatItemData> m_chatItems;
    QTimer *m_loadTimer; // 延迟加载定时器
    QSet<int> m_loadedItems; // 跟踪已加载的项索引
    bool m_isFastScrolling; // 标记快速滚动状态
    static const int MAX_LOAD_PER_CHECK = 10; // 每次最多加载 10 项
    static const int MAX_LOAD_PER_CHECK_FAST = 5; // 快速滚动时降低到 5
    static const int ITEM_HEIGHT = 72; // 项高度

    // 初始化UI
    void initUI();
    // 创建测试数据
    QVector<ChatItemData> createTestData();
    // 按时间排序
    void sortChatItems();
    // 查找插入位置
    int findInsertPosition(const ChatItemData &data) const;
    // 创建ChatItemWidget
    void createChatItemWidget(int index);
};

#endif // CHATLISTWID_H
