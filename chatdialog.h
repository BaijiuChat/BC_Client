#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include <QTimer>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private:
    Ui::ChatDialog *ui;
    QAction *searchIcon/*, clearButton*/;
    ChatUIMode _mode;
    ChatUIMode _state;
    QTimer* searchTimer;  // 防抖定时器

    void setupNavigation();
    void initSearchSystem();
};

#endif // CHATDIALOG_H
