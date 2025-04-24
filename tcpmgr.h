#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket> // 需要在pro文件中添加QT += network
#include <functional>
#include <QObject> // 发送信号需要包含QObject
#include <QJsonDocument>
#include <QJsonObject>
#include "singleton.h"
#include "global.h"

class TcpMgr: public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    ~TcpMgr();

    // 新增的公共方法
    void connectToHost(const QString &host, quint16 port);
    void disconnect();
    void sendJsonData(ReqId id, const QJsonObject &jsonObj);

private:
    TcpMgr();
    void initHandlers();    // 注册通讯
    void handleMsg(ReqId Id, int len, const QByteArray &data);
    void processBuffer();   // 处理接收缓冲区

    // 信号槽处理方法
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;
    QTcpSocket _socket;     // 通讯用socket
    QString _host;          // socket绑定的IP
    uint16_t _port;         // port
    quint16 _messageId;     // 报文ID
    quint16 _messageLen;    // 报文长度
    QByteArray _buffer;     // 报文内容的字节流
    bool _recvPending;      // 是否有报文截断（报文收全了没有）

public slots:
    void slot_tcp_connect(ServerInfo serverInfo);
    void slot_sent_data(ReqId id, const QByteArray &data);

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, const QByteArray &data);
    void sig_switch_chatdlg();
    void sig_login_failed(int);
    void sig_disconnected();
    void sig_network_error(int errorCode, const QString &errorString);
};
#endif // TCPMGR_H
