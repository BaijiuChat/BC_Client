/********************************************************************************
** Form generated from reading UI file 'registerdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QLabel *userLabel;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *userLineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_4;
    QLineEdit *emailLineEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_5;
    QLineEdit *pwdLineEdit;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_6;
    QLineEdit *confirmLineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer;
    QLineEdit *codeLineEdit;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *getButton;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *registerButton;
    QSpacerItem *horizontalSpacer_9;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName("RegisterDialog");
        RegisterDialog->resize(382, 621);
        verticalLayout_3 = new QVBoxLayout(RegisterDialog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        label = new QLabel(RegisterDialog);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(10, 10, 10, 10);
        userLabel = new QLabel(RegisterDialog);
        userLabel->setObjectName("userLabel");

        horizontalLayout_2->addWidget(userLabel);

        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        userLineEdit = new QLineEdit(RegisterDialog);
        userLineEdit->setObjectName("userLineEdit");
        userLineEdit->setMinimumSize(QSize(0, 30));

        horizontalLayout_2->addWidget(userLineEdit);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(10, 10, 10, 10);
        label_2 = new QLabel(RegisterDialog);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        horizontalSpacer_4 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        emailLineEdit = new QLineEdit(RegisterDialog);
        emailLineEdit->setObjectName("emailLineEdit");
        emailLineEdit->setMinimumSize(QSize(0, 30));

        horizontalLayout_3->addWidget(emailLineEdit);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(10, 10, 10, 10);
        label_4 = new QLabel(RegisterDialog);
        label_4->setObjectName("label_4");

        horizontalLayout_4->addWidget(label_4);

        horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        pwdLineEdit = new QLineEdit(RegisterDialog);
        pwdLineEdit->setObjectName("pwdLineEdit");
        pwdLineEdit->setMinimumSize(QSize(0, 30));

        horizontalLayout_4->addWidget(pwdLineEdit);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(7);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(10, 10, 10, 10);
        label_3 = new QLabel(RegisterDialog);
        label_3->setObjectName("label_3");

        horizontalLayout->addWidget(label_3);

        horizontalSpacer_6 = new QSpacerItem(10, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        confirmLineEdit = new QLineEdit(RegisterDialog);
        confirmLineEdit->setObjectName("confirmLineEdit");
        confirmLineEdit->setMinimumSize(QSize(0, 30));

        horizontalLayout->addWidget(confirmLineEdit);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_5 = new QLabel(RegisterDialog);
        label_5->setObjectName("label_5");

        horizontalLayout_5->addWidget(label_5);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        codeLineEdit = new QLineEdit(RegisterDialog);
        codeLineEdit->setObjectName("codeLineEdit");
        codeLineEdit->setMinimumSize(QSize(0, 30));

        horizontalLayout_5->addWidget(codeLineEdit);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        getButton = new QPushButton(RegisterDialog);
        getButton->setObjectName("getButton");
        getButton->setMinimumSize(QSize(75, 40));
        getButton->setMaximumSize(QSize(75, 40));
        QFont font1;
        font1.setPointSize(12);
        getButton->setFont(font1);

        horizontalLayout_5->addWidget(getButton);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_8);

        cancelButton = new QPushButton(RegisterDialog);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setMinimumSize(QSize(80, 40));
        QFont font2;
        font2.setPointSize(14);
        cancelButton->setFont(font2);

        horizontalLayout_6->addWidget(cancelButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        registerButton = new QPushButton(RegisterDialog);
        registerButton->setObjectName("registerButton");
        registerButton->setMinimumSize(QSize(80, 40));
        registerButton->setFont(font2);

        horizontalLayout_6->addWidget(registerButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_9);


        verticalLayout_2->addLayout(horizontalLayout_6);


        verticalLayout_3->addLayout(verticalLayout_2);


        retranslateUi(RegisterDialog);

        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p align=\"center\">\346\263\250\345\206\214/Sign Up</p></body></html>", nullptr));
        userLabel->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\350\264\246\346\210\267</span></p></body></html>", nullptr));
        label_2->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">\351\202\256\347\256\261</span></p></body></html>", nullptr));
        label_4->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p><span style=\" font-size:12pt;\">\345\257\206\347\240\201</span></p></body></html>", nullptr));
        label_3->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p><span style=\" font-size:12pt;\">\347\241\256\350\256\244</span></p></body></html>", nullptr));
        label_5->setText(QCoreApplication::translate("RegisterDialog", "<html><head/><body><p><span style=\" font-size:12pt;\">\351\252\214\350\257\201\347\240\201</span></p></body></html>", nullptr));
        getButton->setText(QCoreApplication::translate("RegisterDialog", "\350\216\267\345\217\226", nullptr));
        cancelButton->setText(QCoreApplication::translate("RegisterDialog", "\350\277\224\345\233\236", nullptr));
        registerButton->setText(QCoreApplication::translate("RegisterDialog", "\346\263\250\345\206\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
