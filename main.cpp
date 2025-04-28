#include "mainwindow.h"
#include "global.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 读取QSS文件配置
    QFile qss(":/LogReg/style/stylesheet.qss");
    if (qss.open(QFile::ReadOnly)){
        qDebug() << "打开成功";
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug() << "打开qss失败";
    }
    // 读取config.ini文件配置
    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators((app_path + QDir::separator() + fileName));
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host+":"+gate_port;
    MainWindow w;
    w.setWindowTitle("白久飞书");
    w.show();
    return a.exec();
}
