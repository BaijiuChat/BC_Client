#include "MessageListView.h"
#include <QWheelEvent>
#include <QScrollBar>

MessageListView::MessageListView(QWidget *parent)
    : QListView(parent), targetScrollValue(0)
{
    // 初始化滚动动画
    scrollAnimation = new QPropertyAnimation(verticalScrollBar(), "value", this);
    scrollAnimation->setDuration(300); // 动画持续时间 300ms
    scrollAnimation->setEasingCurve(QEasingCurve::OutCubic); // 使用 OutCubic 曲线，平滑减速效果

    // 连接滚动条值变化信号
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &MessageListView::onScrollBarValueChanged);
}

void MessageListView::wheelEvent(QWheelEvent *event)
{
    QScrollBar *scrollBar = verticalScrollBar();
    if (!scrollBar) {
        QListView::wheelEvent(event);
        return;
    }

    // 如果动画正在运行，先停止
    if (scrollAnimation->state() == QPropertyAnimation::Running) {
        scrollAnimation->stop();
    }

    // 计算滚动增量
    int delta = 0;
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();
    if (!numPixels.isNull()) {
        delta = numPixels.y();
    } else if (!numDegrees.isNull()) {
        delta = numDegrees.y() / 1.8; // 标准化角度增量（通常 120 度对应 15 像素）
    }

    // 计算目标滚动值
    targetScrollValue = scrollBar->value() - delta;
    targetScrollValue = qBound(scrollBar->minimum(), targetScrollValue, scrollBar->maximum());

    // 设置动画起始值和结束值
    scrollAnimation->setStartValue(scrollBar->value());
    scrollAnimation->setEndValue(targetScrollValue);
    scrollAnimation->start();

    event->accept();
}

void MessageListView::onScrollBarValueChanged(int value)
{
    // 如果动画正在运行，不处理手动滚动
    if (scrollAnimation->state() == QPropertyAnimation::Running) {
        return;
    }

    targetScrollValue = value;
    viewport()->update();
}
