/********************************************************************************
** Form generated from reading UI file 'sewingeditor.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEWINGEDITOR_H
#define UI_SEWINGEDITOR_H

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
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "squarewidget.h"
#include "svgviewer.h"

QT_BEGIN_NAMESPACE

class Ui_SewingEditorClass
{
public:
    QAction *actionLoad_svg;
    QAction *actionSave_svg;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    SquareWidget *squareWidget;
    SvgViewer *widget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetContents_3;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetContents;

    void setupUi(QMainWindow *SewingEditorClass)
    {
        if (SewingEditorClass->objectName().isEmpty())
            SewingEditorClass->setObjectName(QStringLiteral("SewingEditorClass"));
        SewingEditorClass->resize(1080, 869);
        actionLoad_svg = new QAction(SewingEditorClass);
        actionLoad_svg->setObjectName(QStringLiteral("actionLoad_svg"));
        actionSave_svg = new QAction(SewingEditorClass);
        actionSave_svg->setObjectName(QStringLiteral("actionSave_svg"));
        centralWidget = new QWidget(SewingEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        squareWidget = new SquareWidget(centralWidget);
        squareWidget->setObjectName(QStringLiteral("squareWidget"));
        widget = new SvgViewer(squareWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(130, 60, 621, 511));

        gridLayout->addWidget(squareWidget, 0, 0, 1, 1);

        SewingEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SewingEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1080, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        SewingEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SewingEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        SewingEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SewingEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        SewingEditorClass->setStatusBar(statusBar);
        dockWidgetRight = new QDockWidget(SewingEditorClass);
        dockWidgetRight->setObjectName(QStringLiteral("dockWidgetRight"));
        dockWidgetRight->setMinimumSize(QSize(200, 38));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QStringLiteral("dockWidgetContents_3"));
        dockWidgetRight->setWidget(dockWidgetContents_3);
        SewingEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidgetRight);
        dockWidgetLeft = new QDockWidget(SewingEditorClass);
        dockWidgetLeft->setObjectName(QStringLiteral("dockWidgetLeft"));
        dockWidgetLeft->setMinimumSize(QSize(50, 38));
        dockWidgetLeft->setMaximumSize(QSize(50, 99999));
        dockWidgetLeft->setFloating(false);
        dockWidgetLeft->setAllowedAreas(Qt::LeftDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        dockWidgetLeft->setWidget(dockWidgetContents);
        SewingEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidgetLeft);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_svg);
        menuFile->addAction(actionSave_svg);

        retranslateUi(SewingEditorClass);

        QMetaObject::connectSlotsByName(SewingEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *SewingEditorClass)
    {
        SewingEditorClass->setWindowTitle(QApplication::translate("SewingEditorClass", "SewingEditor", 0));
        actionLoad_svg->setText(QApplication::translate("SewingEditorClass", "open svg", 0));
        actionLoad_svg->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+O", 0));
        actionSave_svg->setText(QApplication::translate("SewingEditorClass", "save svg", 0));
        actionSave_svg->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+S", 0));
        menuFile->setTitle(QApplication::translate("SewingEditorClass", "file", 0));
    } // retranslateUi

};

namespace Ui {
    class SewingEditorClass: public Ui_SewingEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEWINGEDITOR_H
