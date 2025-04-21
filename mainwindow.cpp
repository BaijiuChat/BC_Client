#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_Dlg = new LoginDialog(this); // 初始化，设置父窗口
    _reg_Dlg = new RegisterDialog(this);
    setCentralWidget(_login_Dlg); // 设置主窗口有谁
    // _login_Dlg->show(); // 显示，注意已经不用了
    // _reg_Dlg->hide(); // 不需要
    _login_Dlg->setWindowFlags(Qt::FramelessWindowHint);
    _reg_Dlg->setWindowFlags(Qt::FramelessWindowHint);

    _stackedWidget = new QStackedWidget(this);
    _stackedWidget->addWidget(_login_Dlg);
    _stackedWidget->addWidget(_reg_Dlg);
    setCentralWidget(_stackedWidget); // 和先后顺序有关

    // 创建注册信号和槽
    connect(_login_Dlg, &LoginDialog::registerRequest, this, &MainWindow::switchToRegister);
    connect(_reg_Dlg, &RegisterDialog::cancelRegister, this, &MainWindow::switchToLogin);
    connect(_reg_Dlg, &RegisterDialog::registerSucceed, [this](const QString& email){
        switchToLogin();
        _login_Dlg->setEmail(email); // 自动填充邮箱
    });

    // customize指完全自定义窗口形式，包括取消默认的标题栏和边框
    // frameless是表示无边框
    // Qt::CustomizeWindowHint|Qt::FramelessWindowHint
    // 注意要用或/，这个表示两种都启用
    //  场景	    CustomizeWindowHint         FramelessWindowHint
    // ​有父窗口	嵌入父窗口，保留可能的内部边框	嵌入父窗口，完全无边框
    // ​无父窗口	可能有系统边框（需手动禁用）  	完全无边框
    // ​典型用途	自定义标题栏但保留边框         完全无边框（如游戏界面、悬浮弹窗）



}

MainWindow::~MainWindow()
{
    delete ui;
    // 由于登录和注册页面都没有父节点
    // 划掉(所以要回收一下)
    // if (_login_Dlg){
    //     delete _login_Dlg;
    //     _login_Dlg = nullptr;
    // }
    // if (_reg_Dlg){
    //     delete _reg_Dlg;
    //     _reg_Dlg = nullptr;
    // }
    // 不可以！
    // 两个窗口来回成为主窗口，Qt会自动管理它们
    // 只需要在上面初始化的时候设置父窗口即可！

}

void MainWindow::switchToRegister()
{
    _stackedWidget->setCurrentWidget(_reg_Dlg);
    // setCentralWidget(_reg_Dlg);
    // _login_Dlg->hide();
    // _reg_Dlg->show();
}

void MainWindow::switchToLogin()
{
    _stackedWidget->setCurrentWidget(_login_Dlg);
    // setCentralWidget(_login_Dlg);
    // _reg_Dlg->hide();
    // _login_Dlg->show();
}
