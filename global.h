#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <QDebug>
#include <functional>
#include <QStyle>
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>  // 用于处理文件目录的（反）斜杠
#include <QSettings>

/**
 * @brief repolish 用于刷新QSS
 */
// 头文件定义函数需要用extern
extern std::function<void(QWidget*)> repolish; //预先声明有这个函数，让编译器去cpp文件找这个函数
extern QString gate_url_prefix;
extern std::function<QString(QString)> xorString;

enum ReqId{
    ID_GET_VERIFY_CODE = 1001, //请求验证码
    ID_REG_USER = 1002, // 注册用户
    ID_RESET_USER = 1003, // 重置密码
    ID_LOGIN_USER = 1004, //登录用户
    ID_CHAT_LOGIN = 1005, // 聊天登录
    ID_CHAT_LOGIN_RSP = 1006, // 聊天登录响应
};

enum Modules{
    REGISTERMOD = 1, // 注册模块
    RESETMOD = 2, // 重置密码
    LOGINMOD = 3, // 登录模块
};

// 统一错误码定义（服务器和客户端共用）
enum ErrorCodes {
    // 基础错误码 (0-999)
    SUCCESS = 0,            // 成功
    ERR_JSON = 1,           // JSON解析失败（客户端）
    ERR_NETWORK = 2,        // 网络错误（客户端）

    // 业务逻辑错误码 (1000-1999)
    Error_Json = 1001,      // JSON解析失败（服务器）
    RPCFailed = 1002,       // RPC调用失败（服务器）
    VerifyExpired = 1003,   // 验证码过期
    VerifyCodeErr = 1004,   // 验证码错误
    PasswdErr = 1006,       // 密码错误
    UserMailNotMatch = 1007,// 邮箱不匹配
    PasswdUpFailed = 1008,  // 更新密码失败
    PasswdInvalid = 1009,   // 密码无效
    RPCGetFailed = 1010,    // RPC无法获取聊天服务器

    // 数据相关错误码 (2000-2999)
    UserEmailExists = 2000,     // 用户或邮箱存在
    SQLFailed = 2001,           // SQL异常
    DatabaseConnectionFailed = 2002, // 数据库连接失败
    DatabaseProcedureError = 2003,   // 数据库存储过程错误
    UserEmailNotExists = 2004,     // 用户或邮箱不存在

    // 系统错误码 (3000-3999)
    GeneralException = 2004,    // 一般异常（建议改为3001）
    UnknownException = 2005,    // 未知异常（建议改为3002）
    UnknownError = 2006         // 未定义的错误（建议改为3003）
};

enum ChatUIMode{
    ChatMode,       // 聊天模式
    SearchMode,     // 搜索模式
    ContactMode     // 联系人模式
};

struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};

#endif // GLOBAL_H
