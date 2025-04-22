#include "resetdialog.h"
#include "ui_resetdialog.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    initHttpHandlers();

    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmLineEdit->setEchoMode(QLineEdit::Password);

    ui->tip->setProperty("state","normal"); // 设置默认属性，但不会刷新
    repolish(ui->tip); // 刷新属性

    togglePwdAction = new QAction(this);
    togglePwdAction->setIcon(QIcon(":/LogReg/res/eye_close.png"));
    ui->pwdLineEdit->addAction(togglePwdAction, QLineEdit::TrailingPosition);
    connect(togglePwdAction, &QAction::triggered, [=]() {
        bool isPasswordHidden = (ui->pwdLineEdit->echoMode() == QLineEdit::Password);
        ui->pwdLineEdit->setEchoMode(isPasswordHidden ? QLineEdit::Normal : QLineEdit::Password);
        togglePwdAction->setIcon(isPasswordHidden ? QIcon(":/LogReg/res/eye_open.png") : QIcon(":/LogReg/res/eye_close.png"));
    });
    toggleChkAction = new QAction(this);
    toggleChkAction->setIcon(QIcon(":/LogReg/res/eye_close.png"));
    ui->confirmLineEdit->addAction(toggleChkAction, QLineEdit::TrailingPosition);
    connect(toggleChkAction, &QAction::triggered, [=]() {
        bool isPasswordHidden = (ui->confirmLineEdit->echoMode() == QLineEdit::Password);
        ui->confirmLineEdit->setEchoMode(isPasswordHidden ? QLineEdit::Normal : QLineEdit::Password);
        toggleChkAction->setIcon(isPasswordHidden ? QIcon(":/LogReg/res/eye_open.png") : QIcon(":/LogReg/res/eye_close.png"));
    });
    ui->pwdLineEdit->setStyleSheet("QLineEdit { padding-right: 25px; }");
    ui->confirmLineEdit->setStyleSheet("QLineEdit { padding-right: 25px; }");

    connect(ui->userLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkUserValid();
    });
    connect(ui->emailLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pwdLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkPwdValid();
    });
    connect(ui->confirmLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->codeLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkCodeValid();
    });

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this, &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::showTip(QString str, bool isOK)
{
    if(isOK){
        ui->tip->setProperty("state","normal");
        ui->tip->setText(str);
        repolish(ui->tip);
    }else{
        ui->tip->setText(str);
        ui->tip->setProperty("state","error"); // 更改状态
        repolish(ui->tip);
    }
}

void ResetDialog::clearAll()
{
    ui->userLineEdit->clear();
    ui->emailLineEdit->clear();
    ui->pwdLineEdit->clear();
    ui->confirmLineEdit->clear();
    ui->codeLineEdit->clear();
}

bool ResetDialog::checkUserValid() {
    const QString text = ui->userLineEdit->text();

    if (text.isEmpty()) {
        ui->userTip->setText("该项不能为空⚠️");
        return false;
    }

    const QRegularExpression regex("^\\w{4,15}$");
    if (!regex.match(text).hasMatch()) {
        ui->userTip->setText("需为4-15位字母/数字/下划线⚠️");
        return false;
    }

    ui->userTip->setText("");
    return true;
}

bool ResetDialog::checkEmailValid() {
    const QString text = ui->emailLineEdit->text();

    if (text.isEmpty()) {
        ui->emailTip->setText("该项不能为空⚠️");
        return false;
    }

    const QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    if (!regex.match(text).hasMatch()) {
        ui->emailTip->setText("邮箱格式不正确⚠️");
        return false;
    }

    ui->emailTip->setText("");
    return true;
}


bool ResetDialog::checkPwdValid()
{
    const QString pwd = ui->pwdLineEdit->text();

    if(pwd.isEmpty()) {
        ui->pwdTip->setText("该项不能为空⚠️");
        return false;
    }

    // 规则：5-20位，至少1大写+1小写+1数字
    if(pwd.length() < 5 || pwd.length() > 20) {
        ui->pwdTip->setText("密码至少为5-20位数字⚠️");
        return false;
    }

    bool hasUpper = false, hasLower = false, hasDigit = false;
    for(const QChar& c : pwd) {
        if(c.isUpper()) hasUpper = true;
        else if(c.isLower()) hasLower = true;
        else if(c.isDigit()) hasDigit = true;
    }

    if(!(hasUpper && hasLower && hasDigit)) {
        ui->pwdTip->setText("密码需要包含大小写字母和数字⚠️");
        return false;
    }

    ui->pwdTip->setText("");
    return true;
}

bool ResetDialog::checkConfirmValid()
{
    const QString pwd = ui->pwdLineEdit->text();
    const QString confirm = ui->confirmLineEdit->text();

    if(confirm.isEmpty()) {
        ui->confirmTip->setText("该项不能为空⚠️");
        return false;
    }

    if(pwd != confirm) {
        ui->confirmTip->setText("两次密码不一致⚠️");
        return false;
    }

    ui->confirmTip->setText("");
    return true;
}

bool ResetDialog::checkCodeValid()
{
    const QString code = ui->codeLineEdit->text();

    if(code.isEmpty()) {
        ui->codeTip->setText("该项不能为空⚠️");
        return false;
    }

    // 规则：6位纯数字
    if(code.length() != 6 || !code.toInt()) {
        ui->codeTip->setText("需为六位数字⚠️");
        return false;
    }

    ui->codeTip->setText("");
    return true;
}

void ResetDialog::on_cancelButton_clicked()
{
    cancelReset();
}


void ResetDialog::on_getButton_clicked()
{
    auto email = ui->emailLineEdit->text();
    if(checkEmailValid()){
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                            json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::RESETMOD);
        showTip(tr("正在发送邮件中..."),false);
    }else{
        showTip(tr("邮箱地址格式不正确"),false);  // tr用于多语言支持
    }
}

void ResetDialog::initHttpHandlers()
{
    // 注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送至邮箱"), true);
        // qDebug() << "email是" << email;
    });

    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_RESET_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            if (error == ErrorCodes::VerifyExpired || error == ErrorCodes::VerifyCodeErr) {
                showTip(tr("验证码错误"), false);
            } else if (error == ErrorCodes::Error_Json) {  // 1004
                showTip(tr("请确认您的信息"), false);
            } else if (error == ErrorCodes::PasswdErr) {  // 1004
                showTip(tr("确认密码错误"), false);
            } else {
                showTip(tr("出现了一些错误..."), false);
            }
            return;
        }
        auto email = jsonObj["email"].toString();

        tipTemplate = tr("重置密码成功！\n将在%1秒后跳回登录");
        showTip(tipTemplate.arg(countDown), true);// 显示初始提示
        backToLoginTimer = new QTimer(this);
        connect(backToLoginTimer, &QTimer::timeout, this, [this, email]() {
            --countDown;
            if (countDown <= 0) {
                backToLoginTimer->stop();
                backToLoginTimer->deleteLater();
                showTip("", false); // 隐藏提示
                clearAll();
                emit resetSucceed(email);
            } else {
                showTip(tipTemplate.arg(countDown), true); // 更新倒计时文本
            }
        });
        backToLoginTimer->start(1000); // 每秒触发一次
    });
}

void ResetDialog::on_resetButton_clicked()
{
    if(ui->userLineEdit->text() == "" ||
        ui->emailLineEdit->text() == "" ||
        ui->pwdLineEdit->text() == "" ||
        ui->confirmLineEdit->text() == "" ||
        ui->codeLineEdit->text() == ""){
        showTip(tr("请补全信息"), false);
        return;
    }

    if(!checkUserValid() || !checkEmailValid() || !checkPwdValid() || !checkConfirmValid() || ! checkCodeValid()){
        showTip(tr("请修正错误"), false);
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->userLineEdit->text();
    json_obj["email"] = ui->emailLineEdit->text();
    json_obj["passwd"] = xorString(ui->pwdLineEdit->text());
    json_obj["confirm"] = xorString(ui->confirmLineEdit->text());
    json_obj["verifycode"] = ui->codeLineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/reset_pwd"),
                                        json_obj, ReqId::ID_REG_USER,Modules::RESETMOD);
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        return;
    }

    // 解析json字符串,res转成二进制
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"), false);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(tr("json转换失败"), false);
        return;
    }

    _handlers[id](jsonDoc.object()); // 执行处理函数
    return;
}

