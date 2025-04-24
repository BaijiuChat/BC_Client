#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 初始化HttpHandler
    initHttpHandlers();

    ui->tip->setProperty("state","normal"); // 设置默认属性，但不会刷新
    repolish(ui->tip); // 刷新属性

    ui->forgetButton->setStyleSheet(R"(
    QPushButton {
        border: none;
        background: transparent;
    }
    QPushButton:hover {
        background-color: rgba(0, 0, 0, 0.05); /* 可选的悬浮反馈 */
    })");

    togglePwdAction = new QAction(this);
    togglePwdAction->setIcon(QIcon(":/LogReg/res/eye_close.png"));
    ui->pwdLineEdit->addAction(togglePwdAction, QLineEdit::TrailingPosition);
    connect(togglePwdAction, &QAction::triggered, [=]() {
        bool isPasswordHidden = (ui->pwdLineEdit->echoMode() == QLineEdit::Password);
        ui->pwdLineEdit->setEchoMode(isPasswordHidden ? QLineEdit::Normal : QLineEdit::Password);
        togglePwdAction->setIcon(isPasswordHidden ? QIcon(":/LogReg/res/eye_open.png") : QIcon(":/LogReg/res/eye_close.png"));
    });

    // 连接注册按钮
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::registerRequest);
    // 输入合法检测
    connect(ui->emailLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pwdLineEdit, &QLineEdit::editingFinished, this, [this](){
        checkPwdValid();
    });
    // 登录模块的信号与槽
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,
            &LoginDialog::slot_login_mod_finish);
    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    //连接tcp管理者发出的连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &LoginDialog::slot_tcp_con_finish);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setEmail(const QString &email)
{
    ui->emailLineEdit->setText(email); // 自动填写邮箱
    ui->pwdLineEdit->setFocus();    // 焦点移到密码输入框
}

bool LoginDialog::checkEmailValid() {
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


bool LoginDialog::checkPwdValid()
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
        ui->pwdTip->setText("需包含大小写字母和数字⚠️");
        return false;
    }

    ui->pwdTip->setText("");
    return true;
}


void LoginDialog::on_forgetButton_clicked()
{
    resetRequest();
}

void LoginDialog::showTip(QString str, bool isOK)
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

void LoginDialog::on_loginButton_clicked()
{
    if(!checkEmailValid() || !checkPwdValid())
        return;
    auto email = ui->emailLineEdit->text();
    auto pwd = ui->pwdLineEdit->text();
    // 发送json通讯
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = pwd;
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_login"), json_obj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络错误⚠️"), false);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull() || !jsonDoc.isObject()){
        showTip(tr("回包json解析失败⚠️"), false);
        return;
    }
    // 根据ID回调函数
    _handlers[id](jsonDoc.object());
    return;
}

void LoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        showTip(tr("连接成功，正在登录"), true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);

        //发送tcp请求给ChatServer
        TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    }else{
        showTip(tr("连接失败⚠️"), false);
    }
}

void LoginDialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        ServerInfo serverInfo;
        serverInfo.Uid = jsonObj["uid"].toInt();
        serverInfo.Host = jsonObj["host"].toString();
        serverInfo.Port = jsonObj["port"].toString();
        serverInfo.Token = jsonObj["token"].toString();

        _uid = serverInfo.Uid;
        _token = serverInfo.Token;
        qDebug()<< "email is " << email << " uid is " << serverInfo.Uid <<" host is "
                 << serverInfo.Host << " Port is " << serverInfo.Port << " Token is " << serverInfo.Token;
        emit sig_connect_tcp(serverInfo);
    });
}
