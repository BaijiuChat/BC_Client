#ifndef MESSAGELISTVIEW_H
#define MESSAGELISTVIEW_H

#include <QListView>
#include <QPropertyAnimation>

class MessageListView : public QListView {
    Q_OBJECT
public:
    explicit MessageListView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onScrollBarValueChanged(int value);

private:
    QPropertyAnimation *scrollAnimation; // 滚动动画
    int targetScrollValue;               // 目标滚动值
};

#endif // MESSAGELISTVIEW_H
