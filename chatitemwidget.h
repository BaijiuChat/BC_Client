#ifndef CHATITEMWIDGET_H
#define CHATITEMWIDGET_H

#include <QWidget>
#include "chatitemdata.h"

namespace Ui {
class ChatItemWidget;
}

class ChatItemWidget : public QWidget
{
    Q_OBJECT

public:
    // 显式构造，禁止编译器自动创建对象
    explicit ChatItemWidget(const ChatItemData &data, QWidget *parent = nullptr);
    ~ChatItemWidget();
    // 更新数据
    void updateData(const ChatItemData &data);
    // 获取聊天数据，禁止外部修改内部数据
    ChatItemData getData() const;
    // 选中效果
    void setSelected(bool selected);

    // 懒加载完整内容
    void loadFullData();
    // 检查是否已完整加载
    bool isFullyLoaded() const { return m_isFullyLoaded; }


private:
    Ui::ChatItemWidget *ui;
    ChatItemData m_data; // 聊天项数据
    bool m_isSelected; // 是否选中
    bool m_isFullyLoaded; // 是否已完整加载

    // 初始化UI
    void initUI();
    // 加载数据
    void loadData();
    // 更新消息提示状态
    void updateNotificationStatus();
    // 创建圆形头像
    QPixmap createCircularPixmap(const QPixmap &srcPixmap, int diameter);
    // 格式化时间
    QString formatTime(const QDateTime &time);
};

#endif // CHATITEMWIDGET_H
