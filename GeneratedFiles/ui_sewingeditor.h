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
    QAction *actionSelect_all;
    QAction *actionSelect_none;
    QAction *actionSelect_inverse;
    QAction *actionGroup;
    QAction *actionUngroup;
    QAction *actionDelete_selected;
    QAction *actionSplit_selected_path;
    QAction *actionMerge_selected_path;
    QAction *actionFix_grouping;
    QAction *actionSelect_similar_width;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    SquareWidget *squareWidget;
    SvgViewer *widget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSelection;
    QMenu *menuGroup;
    QMenu *menuPath;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetRightContents;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetLeftContents;

    void setupUi(QMainWindow *SewingEditorClass)
    {
        if (SewingEditorClass->objectName().isEmpty())
            SewingEditorClass->setObjectName(QStringLiteral("SewingEditorClass"));
        SewingEditorClass->resize(1080, 869);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(73, 73, 73, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(109, 109, 109, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(91, 91, 91, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(36, 36, 36, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(48, 48, 48, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::NoRole, brush1);
        QBrush brush7(QColor(255, 255, 220, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::NoRole, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::NoRole, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush6);
        SewingEditorClass->setPalette(palette);
        SewingEditorClass->setAutoFillBackground(true);
        SewingEditorClass->setDocumentMode(false);
        actionLoad_svg = new QAction(SewingEditorClass);
        actionLoad_svg->setObjectName(QStringLiteral("actionLoad_svg"));
        actionSave_svg = new QAction(SewingEditorClass);
        actionSave_svg->setObjectName(QStringLiteral("actionSave_svg"));
        actionSelect_all = new QAction(SewingEditorClass);
        actionSelect_all->setObjectName(QStringLiteral("actionSelect_all"));
        actionSelect_none = new QAction(SewingEditorClass);
        actionSelect_none->setObjectName(QStringLiteral("actionSelect_none"));
        actionSelect_inverse = new QAction(SewingEditorClass);
        actionSelect_inverse->setObjectName(QStringLiteral("actionSelect_inverse"));
        actionGroup = new QAction(SewingEditorClass);
        actionGroup->setObjectName(QStringLiteral("actionGroup"));
        actionUngroup = new QAction(SewingEditorClass);
        actionUngroup->setObjectName(QStringLiteral("actionUngroup"));
        actionDelete_selected = new QAction(SewingEditorClass);
        actionDelete_selected->setObjectName(QStringLiteral("actionDelete_selected"));
        actionSplit_selected_path = new QAction(SewingEditorClass);
        actionSplit_selected_path->setObjectName(QStringLiteral("actionSplit_selected_path"));
        actionMerge_selected_path = new QAction(SewingEditorClass);
        actionMerge_selected_path->setObjectName(QStringLiteral("actionMerge_selected_path"));
        actionFix_grouping = new QAction(SewingEditorClass);
        actionFix_grouping->setObjectName(QStringLiteral("actionFix_grouping"));
        actionSelect_similar_width = new QAction(SewingEditorClass);
        actionSelect_similar_width->setObjectName(QStringLiteral("actionSelect_similar_width"));
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
        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 1080, 21));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        menuBar->setPalette(palette1);
        menuBar->setAutoFillBackground(false);
        menuBar->setStyleSheet(QStringLiteral("background-color: rgb(73, 73, 73);"));
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(false);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        QPalette palette2;
        QBrush brush8(QColor(150, 150, 150, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush8);
        menuFile->setPalette(palette2);
        menuFile->setAutoFillBackground(false);
        menuFile->setStyleSheet(QStringLiteral("background-color: rgb(150,150, 150);"));
        menuSelection = new QMenu(menuBar);
        menuSelection->setObjectName(QStringLiteral("menuSelection"));
        menuSelection->setAutoFillBackground(false);
        menuSelection->setStyleSheet(QStringLiteral("background-color: rgb(150, 150, 150);"));
        menuGroup = new QMenu(menuBar);
        menuGroup->setObjectName(QStringLiteral("menuGroup"));
        menuGroup->setStyleSheet(QStringLiteral("background-color: rgb(150, 150, 150);"));
        menuPath = new QMenu(menuBar);
        menuPath->setObjectName(QStringLiteral("menuPath"));
        menuPath->setStyleSheet(QStringLiteral("background-color: rgb(150, 150, 150);"));
        SewingEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SewingEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        mainToolBar->setPalette(palette3);
        mainToolBar->setAutoFillBackground(true);
        SewingEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SewingEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        SewingEditorClass->setStatusBar(statusBar);
        dockWidgetRight = new QDockWidget(SewingEditorClass);
        dockWidgetRight->setObjectName(QStringLiteral("dockWidgetRight"));
        dockWidgetRight->setMinimumSize(QSize(200, 38));
        dockWidgetRight->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetRight->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetRightContents = new QWidget();
        dockWidgetRightContents->setObjectName(QStringLiteral("dockWidgetRightContents"));
        dockWidgetRight->setWidget(dockWidgetRightContents);
        SewingEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidgetRight);
        dockWidgetLeft = new QDockWidget(SewingEditorClass);
        dockWidgetLeft->setObjectName(QStringLiteral("dockWidgetLeft"));
        dockWidgetLeft->setMinimumSize(QSize(40, 80));
        dockWidgetLeft->setMaximumSize(QSize(50, 99999));
        dockWidgetLeft->setFloating(false);
        dockWidgetLeft->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetLeft->setAllowedAreas(Qt::LeftDockWidgetArea);
        dockWidgetLeftContents = new QWidget();
        dockWidgetLeftContents->setObjectName(QStringLiteral("dockWidgetLeftContents"));
        dockWidgetLeft->setWidget(dockWidgetLeftContents);
        SewingEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidgetLeft);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSelection->menuAction());
        menuBar->addAction(menuGroup->menuAction());
        menuBar->addAction(menuPath->menuAction());
        menuFile->addAction(actionLoad_svg);
        menuFile->addAction(actionSave_svg);
        menuSelection->addAction(actionSelect_all);
        menuSelection->addAction(actionSelect_none);
        menuSelection->addAction(actionSelect_inverse);
        menuSelection->addAction(actionDelete_selected);
        menuSelection->addAction(actionSelect_similar_width);
        menuGroup->addAction(actionGroup);
        menuGroup->addAction(actionUngroup);
        menuGroup->addAction(actionFix_grouping);
        menuPath->addAction(actionSplit_selected_path);
        menuPath->addAction(actionMerge_selected_path);

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
        actionSelect_all->setText(QApplication::translate("SewingEditorClass", "select all", 0));
        actionSelect_all->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+A", 0));
        actionSelect_none->setText(QApplication::translate("SewingEditorClass", "select none", 0));
        actionSelect_none->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+D", 0));
        actionSelect_inverse->setText(QApplication::translate("SewingEditorClass", "select inverse", 0));
        actionSelect_inverse->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+Shift+I", 0));
        actionGroup->setText(QApplication::translate("SewingEditorClass", "group", 0));
        actionGroup->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+G", 0));
        actionUngroup->setText(QApplication::translate("SewingEditorClass", "ungroup", 0));
        actionUngroup->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+Shift+G", 0));
        actionDelete_selected->setText(QApplication::translate("SewingEditorClass", "delete selected", 0));
        actionDelete_selected->setShortcut(QApplication::translate("SewingEditorClass", "Del", 0));
        actionSplit_selected_path->setText(QApplication::translate("SewingEditorClass", "split selected path", 0));
        actionSplit_selected_path->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+Shift+P", 0));
        actionMerge_selected_path->setText(QApplication::translate("SewingEditorClass", "merge selected path", 0));
        actionMerge_selected_path->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+P", 0));
        actionFix_grouping->setText(QApplication::translate("SewingEditorClass", "fix grouping", 0));
        actionSelect_similar_width->setText(QApplication::translate("SewingEditorClass", "select similar width", 0));
        actionSelect_similar_width->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+W", 0));
        menuFile->setTitle(QApplication::translate("SewingEditorClass", "file", 0));
        menuSelection->setTitle(QApplication::translate("SewingEditorClass", "selection", 0));
        menuGroup->setTitle(QApplication::translate("SewingEditorClass", "group", 0));
        menuPath->setTitle(QApplication::translate("SewingEditorClass", "path", 0));
        dockWidgetLeft->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class SewingEditorClass: public Ui_SewingEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEWINGEDITOR_H
