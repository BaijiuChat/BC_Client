#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h" // 定义刷新函数
#include <QRegularExpression>
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmLineEdit->setEchoMode(QLineEdit::Password);

    ui->tip->setProperty("state","normal"); // 设置默认属性，但不会刷新
    repolish(ui->tip); // 刷新属性
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish); // .get()用于获得HttpMgr指针

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::showTip(QString str, bool isOK)
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

void RegisterDialog::on_getButton_clicked()
{
    auto email = ui->emailLineEdit->text();
    static QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"); // AI生成正则表达式
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption); // 设置大小写不敏感
    bool match = regex.match(email.trimmed()).hasMatch();
    if(match){
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                            json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);
        showTip(tr("发送成功！请注意查收"),true);
    }else{
        showTip(tr("邮箱地址格式不正确"),false);  // tr用于多语言支持
    }
}

void RegisterDialog::initHttpHandlers()
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
        qDebug() << "email是" << email;
    });

    // 可以继续注册其他请求类型的处理逻辑
    // _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj) {...});
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
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

