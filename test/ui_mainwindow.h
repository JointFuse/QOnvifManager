/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionStopMovement;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *cmbDevicesComboBox;
    QPushButton *btnRefresh;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_8;
    QLineEdit *txtUserName;
    QLabel *label_10;
    QLineEdit *txtPassword;
    QPushButton *setUsernamePass;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnRefreshData;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_12;
    QLineEdit *txtName;
    QLabel *label_11;
    QLineEdit *txtLocation;
    QLabel *label_13;
    QLabel *txtHardware;
    QPushButton *btnSetScopes;
    QLabel *label_9;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QDateTimeEdit *dateTimeEditLocal;
    QLabel *label_5;
    QDateTimeEdit *dateTimeEditUtc;
    QPushButton *btnGetDataAndTime;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_7;
    QDateTimeEdit *dateTimeEditSet;
    QPushButton *btsSetDateAndTime;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout;
    QPushButton *btnsetHome;
    QPushButton *btngoHome;
    QSpacerItem *horizontalSpacer_3;
    QGridLayout *gridLayout_2;
    QPushButton *btnLeft;
    QPushButton *btnDown;
    QPushButton *btnzoomIn;
    QPushButton *btnUp;
    QPushButton *btnzoomOut;
    QPushButton *btnRight;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QComboBox *cbxPresents;
    QPushButton *btnrefreshPresents;
    QPushButton *btnsetPresent;
    QPushButton *btngoPresent;
    QLabel *label_6;
    QLabel *label_14;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(741, 642);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionStopMovement = new QAction(MainWindow);
        actionStopMovement->setObjectName(QString::fromUtf8("actionStopMovement"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        cmbDevicesComboBox = new QComboBox(centralWidget);
        cmbDevicesComboBox->setObjectName(QString::fromUtf8("cmbDevicesComboBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cmbDevicesComboBox->sizePolicy().hasHeightForWidth());
        cmbDevicesComboBox->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(cmbDevicesComboBox);

        btnRefresh = new QPushButton(centralWidget);
        btnRefresh->setObjectName(QString::fromUtf8("btnRefresh"));

        horizontalLayout->addWidget(btnRefresh);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 7, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_5->addWidget(label_8);

        txtUserName = new QLineEdit(centralWidget);
        txtUserName->setObjectName(QString::fromUtf8("txtUserName"));

        horizontalLayout_5->addWidget(txtUserName);

        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_5->addWidget(label_10);

        txtPassword = new QLineEdit(centralWidget);
        txtPassword->setObjectName(QString::fromUtf8("txtPassword"));

        horizontalLayout_5->addWidget(txtPassword);

        setUsernamePass = new QPushButton(centralWidget);
        setUsernamePass->setObjectName(QString::fromUtf8("setUsernamePass"));

        horizontalLayout_5->addWidget(setUsernamePass);


        gridLayout->addLayout(horizontalLayout_5, 6, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        btnRefreshData = new QPushButton(centralWidget);
        btnRefreshData->setObjectName(QString::fromUtf8("btnRefreshData"));

        horizontalLayout_3->addWidget(btnRefreshData);


        gridLayout->addLayout(horizontalLayout_3, 15, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_6->addWidget(label_12);

        txtName = new QLineEdit(centralWidget);
        txtName->setObjectName(QString::fromUtf8("txtName"));

        horizontalLayout_6->addWidget(txtName);

        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_6->addWidget(label_11);

        txtLocation = new QLineEdit(centralWidget);
        txtLocation->setObjectName(QString::fromUtf8("txtLocation"));

        horizontalLayout_6->addWidget(txtLocation);

        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_6->addWidget(label_13);

        txtHardware = new QLabel(centralWidget);
        txtHardware->setObjectName(QString::fromUtf8("txtHardware"));

        horizontalLayout_6->addWidget(txtHardware);

        btnSetScopes = new QPushButton(centralWidget);
        btnSetScopes->setObjectName(QString::fromUtf8("btnSetScopes"));

        horizontalLayout_6->addWidget(btnSetScopes);


        gridLayout->addLayout(horizontalLayout_6, 10, 0, 1, 1);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 5, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_2->addWidget(label_4);

        dateTimeEditLocal = new QDateTimeEdit(centralWidget);
        dateTimeEditLocal->setObjectName(QString::fromUtf8("dateTimeEditLocal"));
        dateTimeEditLocal->setEnabled(true);
        sizePolicy.setHeightForWidth(dateTimeEditLocal->sizePolicy().hasHeightForWidth());
        dateTimeEditLocal->setSizePolicy(sizePolicy);
        dateTimeEditLocal->setReadOnly(true);

        horizontalLayout_2->addWidget(dateTimeEditLocal);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_2->addWidget(label_5);

        dateTimeEditUtc = new QDateTimeEdit(centralWidget);
        dateTimeEditUtc->setObjectName(QString::fromUtf8("dateTimeEditUtc"));
        dateTimeEditUtc->setEnabled(true);
        sizePolicy.setHeightForWidth(dateTimeEditUtc->sizePolicy().hasHeightForWidth());
        dateTimeEditUtc->setSizePolicy(sizePolicy);
        dateTimeEditUtc->setReadOnly(true);
        dateTimeEditUtc->setCalendarPopup(false);
        dateTimeEditUtc->setTimeSpec(Qt::UTC);

        horizontalLayout_2->addWidget(dateTimeEditUtc);

        btnGetDataAndTime = new QPushButton(centralWidget);
        btnGetDataAndTime->setObjectName(QString::fromUtf8("btnGetDataAndTime"));

        horizontalLayout_2->addWidget(btnGetDataAndTime);


        gridLayout->addLayout(horizontalLayout_2, 8, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_4->addWidget(label_7);

        dateTimeEditSet = new QDateTimeEdit(centralWidget);
        dateTimeEditSet->setObjectName(QString::fromUtf8("dateTimeEditSet"));
        sizePolicy.setHeightForWidth(dateTimeEditSet->sizePolicy().hasHeightForWidth());
        dateTimeEditSet->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(dateTimeEditSet);

        btsSetDateAndTime = new QPushButton(centralWidget);
        btsSetDateAndTime->setObjectName(QString::fromUtf8("btsSetDateAndTime"));

        horizontalLayout_4->addWidget(btsSetDateAndTime);


        gridLayout->addLayout(horizontalLayout_4, 9, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        btnsetHome = new QPushButton(centralWidget);
        btnsetHome->setObjectName(QString::fromUtf8("btnsetHome"));

        verticalLayout->addWidget(btnsetHome);

        btngoHome = new QPushButton(centralWidget);
        btngoHome->setObjectName(QString::fromUtf8("btngoHome"));

        verticalLayout->addWidget(btngoHome);


        horizontalLayout_7->addLayout(verticalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        btnLeft = new QPushButton(centralWidget);
        btnLeft->setObjectName(QString::fromUtf8("btnLeft"));

        gridLayout_2->addWidget(btnLeft, 1, 1, 1, 1);

        btnDown = new QPushButton(centralWidget);
        btnDown->setObjectName(QString::fromUtf8("btnDown"));

        gridLayout_2->addWidget(btnDown, 2, 2, 1, 1);

        btnzoomIn = new QPushButton(centralWidget);
        btnzoomIn->setObjectName(QString::fromUtf8("btnzoomIn"));

        gridLayout_2->addWidget(btnzoomIn, 2, 1, 1, 1);

        btnUp = new QPushButton(centralWidget);
        btnUp->setObjectName(QString::fromUtf8("btnUp"));

        gridLayout_2->addWidget(btnUp, 0, 2, 1, 1);

        btnzoomOut = new QPushButton(centralWidget);
        btnzoomOut->setObjectName(QString::fromUtf8("btnzoomOut"));

        gridLayout_2->addWidget(btnzoomOut, 2, 3, 1, 1);

        btnRight = new QPushButton(centralWidget);
        btnRight->setObjectName(QString::fromUtf8("btnRight"));

        gridLayout_2->addWidget(btnRight, 1, 3, 1, 1);


        horizontalLayout_7->addLayout(gridLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        cbxPresents = new QComboBox(centralWidget);
        cbxPresents->setObjectName(QString::fromUtf8("cbxPresents"));

        verticalLayout_2->addWidget(cbxPresents);

        btnrefreshPresents = new QPushButton(centralWidget);
        btnrefreshPresents->setObjectName(QString::fromUtf8("btnrefreshPresents"));

        verticalLayout_2->addWidget(btnrefreshPresents);

        btnsetPresent = new QPushButton(centralWidget);
        btnsetPresent->setObjectName(QString::fromUtf8("btnsetPresent"));

        verticalLayout_2->addWidget(btnsetPresent);

        btngoPresent = new QPushButton(centralWidget);
        btngoPresent->setObjectName(QString::fromUtf8("btngoPresent"));

        verticalLayout_2->addWidget(btngoPresent);


        horizontalLayout_7->addLayout(verticalLayout_2);


        gridLayout->addLayout(horizontalLayout_7, 13, 0, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 14, 0, 1, 1);

        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout->addWidget(label_14, 11, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 741, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(btnDown, SIGNAL(released()), actionStopMovement, SLOT(trigger()));
        QObject::connect(btnLeft, SIGNAL(released()), actionStopMovement, SLOT(trigger()));
        QObject::connect(btnRight, SIGNAL(released()), actionStopMovement, SLOT(trigger()));
        QObject::connect(btnUp, SIGNAL(released()), actionStopMovement, SLOT(trigger()));
        QObject::connect(btnzoomIn, SIGNAL(released()), actionStopMovement, SLOT(trigger()));
        QObject::connect(btnzoomOut, SIGNAL(released()), actionStopMovement, SLOT(trigger()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionStopMovement->setText(QCoreApplication::translate("MainWindow", "StopMovement", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Devices List:", nullptr));
        btnRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Date and Time:", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Username:", nullptr));
        txtUserName->setText(QCoreApplication::translate("MainWindow", "admin", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Password:", nullptr));
        txtPassword->setText(QString());
        setUsernamePass->setText(QCoreApplication::translate("MainWindow", "Set", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Put a break point in pointed place in btn  \"on refresh data\" slot to see device data.", nullptr));
        btnRefreshData->setText(QCoreApplication::translate("MainWindow", "Refresh Data", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Name:", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Location:", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "hardware:", nullptr));
        txtHardware->setText(QCoreApplication::translate("MainWindow", "Unknown", nullptr));
        btnSetScopes->setText(QCoreApplication::translate("MainWindow", "setScopes", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Login:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Local:", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Utc:", nullptr));
        btnGetDataAndTime->setText(QCoreApplication::translate("MainWindow", "Get data and time", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Local:", nullptr));
        btsSetDateAndTime->setText(QCoreApplication::translate("MainWindow", "Set date and time", nullptr));
        btnsetHome->setText(QCoreApplication::translate("MainWindow", "setHome", nullptr));
        btngoHome->setText(QCoreApplication::translate("MainWindow", "goHome", nullptr));
        btnLeft->setText(QCoreApplication::translate("MainWindow", "left", nullptr));
        btnDown->setText(QCoreApplication::translate("MainWindow", "down", nullptr));
        btnzoomIn->setText(QCoreApplication::translate("MainWindow", "zoomIn", nullptr));
        btnUp->setText(QCoreApplication::translate("MainWindow", "up", nullptr));
        btnzoomOut->setText(QCoreApplication::translate("MainWindow", "zoomOut", nullptr));
        btnRight->setText(QCoreApplication::translate("MainWindow", "right", nullptr));
        btnrefreshPresents->setText(QCoreApplication::translate("MainWindow", "refres presents", nullptr));
        btnsetPresent->setText(QCoreApplication::translate("MainWindow", "set present", nullptr));
        btngoPresent->setText(QCoreApplication::translate("MainWindow", "go present", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Control:", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "ptz:", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
