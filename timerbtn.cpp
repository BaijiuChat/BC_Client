#include "timerbtn.h"

TimerBtn::TimerBtn(QWidget *parent):QPushButton(parent), _counter(10)
{
    qDebug() << "timerbtn创建";
    _timer = new QTimer(this);

    connect(_timer, &QTimer::timeout, [this](){
        --_counter;
        if(_counter <= 0){
            _timer->stop();
            _counter = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_counter) + "秒");
    });
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        this->setEnabled(false);
        _timer->start(1000);
        this->setText(QString::number(_counter) + "s");
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e); // 调用基类的释放逻辑
}
