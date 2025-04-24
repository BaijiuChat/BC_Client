#include "tcpmgr.h"
#include <QDebug>

TcpMgr::TcpMgr() : _host(""), _port(0), _messageId(0), _messageLen(0), _recvPending(false)
{
    // 连接socket
    connect(&_socket, &QTcpSocket::connected, this, &TcpMgr::onConnected);
    connect(&_socket, &QTcpSocket::readyRead, this, &TcpMgr::onReadyRead);
    connect(&_socket, &QTcpSocket::disconnected, this, &TcpMgr::onDisconnected);

    // 错误处理
    connect(&_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &TcpMgr::onError);

    // 连接发送信号
    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_sent_data);

    // 初始化消息处理函数
    initHandlers();
}

TcpMgr::~TcpMgr()
{
    _socket.close();
    qDebug() << "TcpMgr析构";
}

void TcpMgr::connectToHost(const QString &host, quint16 port)
{
    _host = host;
    _port = port;
    _socket.connectToHost(host, port);
}

void TcpMgr::disconnect()
{
    _socket.close();
}

void TcpMgr::onConnected()
{
    qDebug() << "socket已连接！";
    emit sig_con_success(true);
}

void TcpMgr::onReadyRead()
{
    // 将接收到的数据追加到缓冲区
    _buffer.append(_socket.readAll());

    // 处理数据
    processBuffer();
}

void TcpMgr::processBuffer()
{
    QDataStream stream(&_buffer, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_6_9);

    while (!_buffer.isEmpty()) {
        // 1. 如果不在接收中且缓冲区数据足够读取头部
        if (!_recvPending && _buffer.size() >= static_cast<int>(sizeof(quint16) * 2)) {
            stream.device()->seek(0); // 重置流位置
            stream >> _messageId >> _messageLen;

            // 标记正在接收
            _recvPending = true;
        }

        // 2. 如果在接收中但数据还不够完整消息
        if (_recvPending && _buffer.size() < (sizeof(quint16) * 2 + _messageLen)) {
            // 等待更多数据
            break;
        }

        // 3. 如果在接收中且数据足够，处理完整消息
        if (_recvPending) {
            // 提取消息体 (跳过头部的8字节)
            QByteArray msgBody = _buffer.mid(sizeof(quint16) * 2, _messageLen);

            // 从缓冲区移除已处理的消息(头部+消息体)
            _buffer.remove(0, sizeof(quint16) * 2 + _messageLen);

            // 重置流
            stream.device()->seek(0);

            // 处理消息
            qDebug() << "收到消息，ID:" << _messageId << "长度:" << _messageLen;
            handleMsg(static_cast<ReqId>(_messageId), _messageLen, msgBody);

            // 重置接收状态
            _recvPending = false;
        }
    }
}

void TcpMgr::onDisconnected()
{
    qDebug() << "socket已断开";
    emit sig_disconnected();
}

void TcpMgr::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    qDebug() << "网络错误:" << _socket.errorString();
    emit sig_network_error(_socket.error(), _socket.errorString());
}

// 连接到服务器
void TcpMgr::slot_tcp_connect(ServerInfo serverInfo)
{
    _host = serverInfo.Host;
    _port = static_cast<uint16_t>(serverInfo.Port.toUInt());
    _socket.connectToHost(_host, _port);
}

// 注册消息处理函数
void TcpMgr::initHandlers()
{
    // 注册登录处理函数
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(id)
        Q_UNUSED(len)

        // 解析JSON响应
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            qDebug() << "JSON解析失败";
            emit sig_login_failed(ErrorCodes::ERR_JSON);
            return;
        }

        QJsonObject jsonObj = doc.object();
        if (!jsonObj.contains("error")) {
            qDebug() << "JSON格式错误：缺少error字段";
            emit sig_login_failed(ErrorCodes::ERR_JSON);
            return;
        }

        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            qDebug() << "登录失败，错误码：" << error;
            emit sig_login_failed(error);
            return;
        }

        // 登录成功
        qDebug() << "登录成功";
        emit sig_switch_chatdlg();
    });

    // 可以在这里添加更多消息处理函数...
}

void TcpMgr::handleMsg(ReqId id, int len, const QByteArray &data)
{
    // 查找并调用对应的消息处理函数
    auto it = _handlers.find(id);
    if (it != _handlers.end()) {
        it.value()(id, len, data);
    } else {
        qDebug() << "未注册的消息ID：" << id;
    }
}

// 发送数据槽函数
void TcpMgr::slot_sent_data(ReqId id, const QByteArray &data)
{
    if (_socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "发送失败：socket未连接";
        return;
    }

    // 准备发送缓冲区
    QByteArray sendBuffer;
    QDataStream stream(&sendBuffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_9);

    // 写入消息头 (ID和长度)
    stream << static_cast<quint16>(id) << static_cast<quint16>(data.size());

    // 添加消息体
    sendBuffer.append(data);

    // 发送数据
    qint64 written = _socket.write(sendBuffer);
    if (written != sendBuffer.size()) {
        qDebug() << "发送数据不完整:" << written << "/" << sendBuffer.size();
    }
}

// 快捷发送JSON数据的方法
void TcpMgr::sendJsonData(ReqId id, const QJsonObject &jsonObj)
{
    QJsonDocument doc(jsonObj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    emit sig_send_data(id, data);
}
