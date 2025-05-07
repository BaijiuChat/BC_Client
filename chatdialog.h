#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include <QTimer>
#include "MessageListModel.h"
#include "MessageItemDelegate.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

protected:
    void resizeEvent(QResizeEvent *event) override; // 重写 resizeEvent

private:
    Ui::ChatDialog *ui;
    QAction *searchIcon;
    ChatUIMode _mode;
    ChatUIMode _state;
    QTimer* searchTimer;         // 防抖定时器
    MessageListModel *messageModel; // 消息模型
    MessageItemDelegate *messageDelegate; // 消息绘制代理

    void setupNavigation();
    void initSearchSystem();

    // 测试数据生成相关函数
    QString generateRandomText(int maxWords = 30);
    QDateTime generateRandomDateTime();
    void generateTestMessages(int count = 10000);
};

#endif // CHATDIALOG_H
