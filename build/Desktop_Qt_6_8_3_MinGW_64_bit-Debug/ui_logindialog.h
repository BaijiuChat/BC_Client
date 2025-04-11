/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *userLabel;
    QLineEdit *userLineEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *pwdLabel;
    QLineEdit *pwdLineEdit;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QLabel *forgetLabel;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *registerButton;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *loginButton;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(328, 494);
        verticalLayout_2 = new QVBoxLayout(LoginDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        widget = new QWidget(LoginDialog);
        widget->setObjectName("widget");
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(widget);
        label->setObjectName("label");
        label->setPixmap(QPixmap(QString::fromUtf8(":/LoginDialog/res/logo.png")));
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);


        verticalLayout->addWidget(widget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(10, 10, 10, 10);
        userLabel = new QLabel(LoginDialog);
        userLabel->setObjectName("userLabel");
        userLabel->setMinimumSize(QSize(0, 25));
        userLabel->setMaximumSize(QSize(16777215, 25));
        userLabel->setSizeIncrement(QSize(0, 0));
        userLabel->setBaseSize(QSize(0, 0));
        QFont font;
        font.setPointSize(12);
        userLabel->setFont(font);

        horizontalLayout->addWidget(userLabel);

        userLineEdit = new QLineEdit(LoginDialog);
        userLineEdit->setObjectName("userLineEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(userLineEdit->sizePolicy().hasHeightForWidth());
        userLineEdit->setSizePolicy(sizePolicy);
        userLineEdit->setMinimumSize(QSize(0, 25));
        userLineEdit->setMaximumSize(QSize(16777215, 25));

        horizontalLayout->addWidget(userLineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(10, 10, 10, 10);
        pwdLabel = new QLabel(LoginDialog);
        pwdLabel->setObjectName("pwdLabel");
        pwdLabel->setMinimumSize(QSize(0, 25));
        pwdLabel->setMaximumSize(QSize(16777215, 25));
        pwdLabel->setFont(font);

        horizontalLayout_4->addWidget(pwdLabel);

        pwdLineEdit = new QLineEdit(LoginDialog);
        pwdLineEdit->setObjectName("pwdLineEdit");
        pwdLineEdit->setMinimumSize(QSize(0, 25));
        pwdLineEdit->setMaximumSize(QSize(16777215, 25));

        horizontalLayout_4->addWidget(pwdLineEdit);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(10, 10, 10, 10);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        forgetLabel = new QLabel(LoginDialog);
        forgetLabel->setObjectName("forgetLabel");
        forgetLabel->setMinimumSize(QSize(0, 25));
        forgetLabel->setMaximumSize(QSize(16777215, 25));
        forgetLabel->setFont(font);

        horizontalLayout_5->addWidget(forgetLabel);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(10);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(10, 10, 10, 10);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        registerButton = new QPushButton(LoginDialog);
        registerButton->setObjectName("registerButton");
        registerButton->setMinimumSize(QSize(100, 35));
        registerButton->setMaximumSize(QSize(100, 35));
        registerButton->setFont(font);

        horizontalLayout_6->addWidget(registerButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        loginButton = new QPushButton(LoginDialog);
        loginButton->setObjectName("loginButton");
        loginButton->setMinimumSize(QSize(100, 35));
        loginButton->setMaximumSize(QSize(100, 35));
        loginButton->setFont(font);

        horizontalLayout_6->addWidget(loginButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_6);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        label->setText(QString());
        userLabel->setText(QCoreApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\">\350\264\246\346\210\267</p></body></html>", nullptr));
        pwdLabel->setText(QCoreApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\">\345\257\206\347\240\201</p></body></html>", nullptr));
        forgetLabel->setText(QCoreApplication::translate("LoginDialog", "<html><head/><body><p align=\"center\">\345\277\230\350\256\260\345\257\206\347\240\201</span></p></body></html>", nullptr));
        registerButton->setText(QCoreApplication::translate("LoginDialog", "\346\263\250\345\206\214", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
