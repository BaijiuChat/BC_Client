#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this();
    // 目的​​：确保 Lambda 异步回调执行时，HttpMgr 对象仍存活（避免回调中访问已析构的 this）。
    // ​​要求​​：HttpMgr 必须继承 std::enable_shared_from_this<HttpMgr>。
    // 原理​​：增加当前对象的引用计数，防止异步回调期间对象被销毁。
    QNetworkReply *reply = _manager.post(request, data); // reply是自己定义的指针，需要自己释放
    // 返回值​​：QNetworkReply* 用于处理响应和错误。
    connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){
        // 捕获错误情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            // 发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater(); // 稍后回收reply，防止reply还在被占用中
            return;
        }
        // 无错误
        QString res = reply->readAll();
        // 发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        // 发送信号通知指定模块http的响应结束了
        emit sig_reg_mod_finish(id, res, err);
    }
}
