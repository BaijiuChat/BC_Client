#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QString>
#include <QAction>
#include "global.h"
#include "httpmgr.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    void showTip(QString str, bool isOK);
    void clearAll();
    ~ResetDialog();

signals:
    void resetSucceed(const QString& email);
    void cancelReset();
private slots:
    void on_cancelButton_clicked();
    void on_getButton_clicked();
    void on_resetButton_clicked();
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    void initHttpHandlers();

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPwdValid();
    bool checkConfirmValid();
    bool checkCodeValid();

    Ui::ResetDialog *ui;

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QAction *togglePwdAction, *toggleChkAction;
    QString tipTemplate;
    QTimer* backToLoginTimer;
    int countDown = 3;
};

#endif // RESETDIALOG_H
