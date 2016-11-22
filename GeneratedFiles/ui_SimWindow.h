/********************************************************************************
** Form generated from reading UI file 'SimWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMWINDOW_H
#define UI_SIMWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "simviewer.h"

QT_BEGIN_NAMESPACE

class Ui_SimWindow
{
public:
    QAction *actionOpen;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    SimViewer *widget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetRightContents;
    QGridLayout *gridLayout_7;
    QSpacerItem *verticalSpacer;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetLeftContents;

    void setupUi(QMainWindow *SimWindow)
    {
        if (SimWindow->objectName().isEmpty())
            SimWindow->setObjectName(QStringLiteral("SimWindow"));
        SimWindow->resize(800, 681);
        actionOpen = new QAction(SimWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralwidget = new QWidget(SimWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new SimViewer(centralwidget);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout->addWidget(widget, 0, 0, 1, 1);

        SimWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SimWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        SimWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(SimWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        SimWindow->setStatusBar(statusbar);
        dockWidgetRight = new QDockWidget(SimWindow);
        dockWidgetRight->setObjectName(QStringLiteral("dockWidgetRight"));
        dockWidgetRight->setMinimumSize(QSize(300, 598));
        dockWidgetRight->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetRightContents = new QWidget();
        dockWidgetRightContents->setObjectName(QStringLiteral("dockWidgetRightContents"));
        gridLayout_7 = new QGridLayout(dockWidgetRightContents);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer, 0, 0, 1, 1);

        dockWidgetRight->setWidget(dockWidgetRightContents);
        SimWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidgetRight);
        dockWidgetLeft = new QDockWidget(SimWindow);
        dockWidgetLeft->setObjectName(QStringLiteral("dockWidgetLeft"));
        dockWidgetLeft->setMinimumSize(QSize(100, 38));
        dockWidgetLeft->setMaximumSize(QSize(100, 524287));
        dockWidgetLeft->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetLeftContents = new QWidget();
        dockWidgetLeftContents->setObjectName(QStringLiteral("dockWidgetLeftContents"));
        dockWidgetLeft->setWidget(dockWidgetLeftContents);
        SimWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidgetLeft);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(SimWindow);

        QMetaObject::connectSlotsByName(SimWindow);
    } // setupUi

    void retranslateUi(QMainWindow *SimWindow)
    {
        SimWindow->setWindowTitle(QApplication::translate("SimWindow", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("SimWindow", "open", 0));
        menuFile->setTitle(QApplication::translate("SimWindow", "file", 0));
    } // retranslateUi

};

namespace Ui {
    class SimWindow: public Ui_SimWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMWINDOW_H
