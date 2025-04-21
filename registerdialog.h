#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QAction>
#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void cancelRegister();

private slots:
    void on_getButton_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void on_registerButton_clicked();

private:
    void initHttpHandlers();

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPwdValid();
    bool checkConfirmValid();
    bool checkCodeValid();

    void showTip(QString str, bool isOK);
    // void AddTipErr(TipErr te, QString tips);
    // void DelTipErr(TipErr te);
    // QMap<TipErr, QString> _tip_errs;

    QAction *togglePwdAction, *toggleChkAction;

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
