/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QTextEdit *listWidget_outPath;
    QTextEdit *listWidget_ver;
    QPushButton *pushButton_chooseFile;
    QPushButton *pushButton_save;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(699, 114);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(11, 10, 674, 43));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget_outPath = new QTextEdit(layoutWidget);
        listWidget_outPath->setObjectName(QString::fromUtf8("listWidget_outPath"));
        listWidget_outPath->setMinimumSize(QSize(221, 41));
        listWidget_outPath->setMaximumSize(QSize(411, 41));

        horizontalLayout->addWidget(listWidget_outPath);

        listWidget_ver = new QTextEdit(layoutWidget);
        listWidget_ver->setObjectName(QString::fromUtf8("listWidget_ver"));
        listWidget_ver->setMinimumSize(QSize(221, 41));
        listWidget_ver->setMaximumSize(QSize(411, 41));

        horizontalLayout->addWidget(listWidget_ver);

        pushButton_chooseFile = new QPushButton(layoutWidget);
        pushButton_chooseFile->setObjectName(QString::fromUtf8("pushButton_chooseFile"));
        pushButton_chooseFile->setMinimumSize(QSize(71, 41));
        pushButton_chooseFile->setMaximumSize(QSize(71, 41));

        horizontalLayout->addWidget(pushButton_chooseFile);

        pushButton_save = new QPushButton(layoutWidget);
        pushButton_save->setObjectName(QString::fromUtf8("pushButton_save"));
        pushButton_save->setEnabled(false);
        pushButton_save->setMinimumSize(QSize(71, 41));
        pushButton_save->setMaximumSize(QSize(61, 41));

        horizontalLayout->addWidget(pushButton_save);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 699, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton_chooseFile->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        pushButton_save->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
