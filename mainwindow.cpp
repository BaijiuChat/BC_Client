#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_Dlg = new LoginDialog(this); // 初始化，设置父窗口
    setCentralWidget(_login_Dlg); // 设置主窗口有谁
    // _login_Dlg->show(); // 显示，注意已经不用了，详见下文

    // 创建注册信号和槽
    connect(_login_Dlg, &LoginDialog::registerRequest, this, &MainWindow::switchToRegister);
    _reg_Dlg = new RegisterDialog(this);

    // customize指完全自定义窗口形式，包括取消默认的标题栏和边框
    // frameless是表示无边框
    // Qt::CustomizeWindowHint|Qt::FramelessWindowHint
    // 注意要用或/，这个表示两种都启用
    //  场景	    CustomizeWindowHint         FramelessWindowHint
    // ​有父窗口	嵌入父窗口，保留可能的内部边框	嵌入父窗口，完全无边框
    // ​无父窗口	可能有系统边框（需手动禁用）  	完全无边框
    // ​典型用途	自定义标题栏但保留边框         完全无边框（如游戏界面、悬浮弹窗）
    _login_Dlg->setWindowFlags(Qt::FramelessWindowHint);
    _reg_Dlg->setWindowFlags(Qt::FramelessWindowHint);
    // _reg_Dlg->hide(); // 不需要
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
    setCentralWidget(_reg_Dlg);
    _login_Dlg->hide();
    _reg_Dlg->show();
}
