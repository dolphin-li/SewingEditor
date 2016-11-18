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
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
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
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionSelect_connected;
    QAction *actionRemove_selected_corner;
    QAction *actionSplit_selected_edge;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    SvgViewer *widget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSelection;
    QMenu *menuGroup;
    QMenu *menuPath;
    QMenu *menuOp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetRightContents;
    QGridLayout *gridLayout_4;
    QListWidget *listHistory;
    QGroupBox *gbLayers;
    QGridLayout *gridLayout_2;
    QListWidget *listLayers;
    QPushButton *pbNewLayer;
    QPushButton *pbMergeLayers;
    QPushButton *pbRemoveLayers;
    QGroupBox *gbParam;
    QGridLayout *gridLayout_3;
    QPushButton *pbSelectClosed;
    QPushButton *pbClosePolygon;
    QPushButton *pbRemovePairs;
    QPushButton *pbMakePair;
    QPushButton *pbToConnectedGroups;
    QDoubleSpinBox *sbPixelToMeter;
    QPushButton *pbSymmetricCopy;
    QPushButton *pbPixelToMeter;
    QPushButton *pbPathSplitByIntersect;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QPushButton *pbGenerateMesh;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_6;
    QPushButton *pbSmoothPoly;
    QDoubleSpinBox *dbSmoothPolyThre;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetLeftContents;

    void setupUi(QMainWindow *SewingEditorClass)
    {
        if (SewingEditorClass->objectName().isEmpty())
            SewingEditorClass->setObjectName(QStringLiteral("SewingEditorClass"));
        SewingEditorClass->resize(1176, 864);
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
        actionUndo = new QAction(SewingEditorClass);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionRedo = new QAction(SewingEditorClass);
        actionRedo->setObjectName(QStringLiteral("actionRedo"));
        actionSelect_connected = new QAction(SewingEditorClass);
        actionSelect_connected->setObjectName(QStringLiteral("actionSelect_connected"));
        actionRemove_selected_corner = new QAction(SewingEditorClass);
        actionRemove_selected_corner->setObjectName(QStringLiteral("actionRemove_selected_corner"));
        actionSplit_selected_edge = new QAction(SewingEditorClass);
        actionSplit_selected_edge->setObjectName(QStringLiteral("actionSplit_selected_edge"));
        centralWidget = new QWidget(SewingEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new SvgViewer(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout->addWidget(widget, 0, 0, 1, 1);

        SewingEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SewingEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 1176, 21));
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
        palette2.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        menuFile->setPalette(palette2);
        menuFile->setAutoFillBackground(false);
        menuFile->setStyleSheet(QStringLiteral(""));
        menuSelection = new QMenu(menuBar);
        menuSelection->setObjectName(QStringLiteral("menuSelection"));
        menuSelection->setAutoFillBackground(false);
        menuSelection->setStyleSheet(QStringLiteral(""));
        menuGroup = new QMenu(menuBar);
        menuGroup->setObjectName(QStringLiteral("menuGroup"));
        menuGroup->setStyleSheet(QStringLiteral(""));
        menuPath = new QMenu(menuBar);
        menuPath->setObjectName(QStringLiteral("menuPath"));
        menuPath->setStyleSheet(QStringLiteral(""));
        menuOp = new QMenu(menuBar);
        menuOp->setObjectName(QStringLiteral("menuOp"));
        menuOp->setStyleSheet(QStringLiteral(""));
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
        dockWidgetRight->setMinimumSize(QSize(275, 575));
        dockWidgetRight->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetRight->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetRightContents = new QWidget();
        dockWidgetRightContents->setObjectName(QStringLiteral("dockWidgetRightContents"));
        gridLayout_4 = new QGridLayout(dockWidgetRightContents);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        listHistory = new QListWidget(dockWidgetRightContents);
        listHistory->setObjectName(QStringLiteral("listHistory"));
        listHistory->setStyleSheet(QStringLiteral(""));
        listHistory->setTabKeyNavigation(false);

        gridLayout_4->addWidget(listHistory, 0, 0, 1, 2);

        gbLayers = new QGroupBox(dockWidgetRightContents);
        gbLayers->setObjectName(QStringLiteral("gbLayers"));
        gridLayout_2 = new QGridLayout(gbLayers);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        listLayers = new QListWidget(gbLayers);
        listLayers->setObjectName(QStringLiteral("listLayers"));
        listLayers->setStyleSheet(QStringLiteral(""));
        listLayers->setTabKeyNavigation(true);
        listLayers->setDragEnabled(true);
        listLayers->setDragDropMode(QAbstractItemView::InternalMove);
        listLayers->setSelectionMode(QAbstractItemView::ExtendedSelection);
        listLayers->setResizeMode(QListView::Adjust);
        listLayers->setLayoutMode(QListView::Batched);
        listLayers->setGridSize(QSize(50, 20));
        listLayers->setViewMode(QListView::ListMode);

        gridLayout_2->addWidget(listLayers, 0, 0, 1, 3);

        pbNewLayer = new QPushButton(gbLayers);
        pbNewLayer->setObjectName(QStringLiteral("pbNewLayer"));

        gridLayout_2->addWidget(pbNewLayer, 1, 0, 1, 1);

        pbMergeLayers = new QPushButton(gbLayers);
        pbMergeLayers->setObjectName(QStringLiteral("pbMergeLayers"));

        gridLayout_2->addWidget(pbMergeLayers, 1, 1, 1, 1);

        pbRemoveLayers = new QPushButton(gbLayers);
        pbRemoveLayers->setObjectName(QStringLiteral("pbRemoveLayers"));

        gridLayout_2->addWidget(pbRemoveLayers, 1, 2, 1, 1);


        gridLayout_4->addWidget(gbLayers, 1, 0, 1, 2);

        gbParam = new QGroupBox(dockWidgetRightContents);
        gbParam->setObjectName(QStringLiteral("gbParam"));
        gbParam->setMinimumSize(QSize(0, 200));
        gridLayout_3 = new QGridLayout(gbParam);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        pbSelectClosed = new QPushButton(gbParam);
        pbSelectClosed->setObjectName(QStringLiteral("pbSelectClosed"));

        gridLayout_3->addWidget(pbSelectClosed, 2, 1, 1, 1);

        pbClosePolygon = new QPushButton(gbParam);
        pbClosePolygon->setObjectName(QStringLiteral("pbClosePolygon"));

        gridLayout_3->addWidget(pbClosePolygon, 2, 0, 1, 1);

        pbRemovePairs = new QPushButton(gbParam);
        pbRemovePairs->setObjectName(QStringLiteral("pbRemovePairs"));

        gridLayout_3->addWidget(pbRemovePairs, 1, 1, 1, 1);

        pbMakePair = new QPushButton(gbParam);
        pbMakePair->setObjectName(QStringLiteral("pbMakePair"));

        gridLayout_3->addWidget(pbMakePair, 1, 0, 1, 1);

        pbToConnectedGroups = new QPushButton(gbParam);
        pbToConnectedGroups->setObjectName(QStringLiteral("pbToConnectedGroups"));

        gridLayout_3->addWidget(pbToConnectedGroups, 0, 1, 1, 1);

        sbPixelToMeter = new QDoubleSpinBox(gbParam);
        sbPixelToMeter->setObjectName(QStringLiteral("sbPixelToMeter"));
        sbPixelToMeter->setDecimals(4);
        sbPixelToMeter->setMaximum(999999);

        gridLayout_3->addWidget(sbPixelToMeter, 3, 1, 1, 1);

        pbSymmetricCopy = new QPushButton(gbParam);
        pbSymmetricCopy->setObjectName(QStringLiteral("pbSymmetricCopy"));

        gridLayout_3->addWidget(pbSymmetricCopy, 0, 0, 1, 1);

        pbPixelToMeter = new QPushButton(gbParam);
        pbPixelToMeter->setObjectName(QStringLiteral("pbPixelToMeter"));

        gridLayout_3->addWidget(pbPixelToMeter, 3, 0, 1, 1);

        pbPathSplitByIntersect = new QPushButton(gbParam);
        pbPathSplitByIntersect->setObjectName(QStringLiteral("pbPathSplitByIntersect"));

        gridLayout_3->addWidget(pbPathSplitByIntersect, 4, 0, 1, 1);


        gridLayout_4->addWidget(gbParam, 2, 0, 1, 2);

        groupBox = new QGroupBox(dockWidgetRightContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 100));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        pbGenerateMesh = new QPushButton(groupBox);
        pbGenerateMesh->setObjectName(QStringLiteral("pbGenerateMesh"));

        gridLayout_5->addWidget(pbGenerateMesh, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox, 3, 0, 1, 1);

        groupBox_2 = new QGroupBox(dockWidgetRightContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_6 = new QGridLayout(groupBox_2);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        pbSmoothPoly = new QPushButton(groupBox_2);
        pbSmoothPoly->setObjectName(QStringLiteral("pbSmoothPoly"));

        gridLayout_6->addWidget(pbSmoothPoly, 0, 0, 1, 1);

        dbSmoothPolyThre = new QDoubleSpinBox(groupBox_2);
        dbSmoothPolyThre->setObjectName(QStringLiteral("dbSmoothPolyThre"));
        dbSmoothPolyThre->setDecimals(4);
        dbSmoothPolyThre->setMaximum(9.9999e+08);

        gridLayout_6->addWidget(dbSmoothPolyThre, 1, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 3, 1, 1, 1);

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
        menuBar->addAction(menuOp->menuAction());
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
        menuPath->addAction(actionRemove_selected_corner);
        menuPath->addAction(actionSplit_selected_edge);
        menuOp->addAction(actionUndo);
        menuOp->addAction(actionRedo);

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
        actionUndo->setText(QApplication::translate("SewingEditorClass", "undo", 0));
        actionUndo->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+Z", 0));
        actionRedo->setText(QApplication::translate("SewingEditorClass", "redo", 0));
        actionRedo->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+Shift+Z", 0));
        actionSelect_connected->setText(QApplication::translate("SewingEditorClass", "select connected", 0));
        actionRemove_selected_corner->setText(QApplication::translate("SewingEditorClass", "remove selected Edge corner", 0));
        actionRemove_selected_corner->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+E", 0));
        actionSplit_selected_edge->setText(QApplication::translate("SewingEditorClass", "Split selected edge", 0));
        actionSplit_selected_edge->setShortcut(QApplication::translate("SewingEditorClass", "S", 0));
        menuFile->setTitle(QApplication::translate("SewingEditorClass", "file", 0));
        menuSelection->setTitle(QApplication::translate("SewingEditorClass", "selection", 0));
        menuGroup->setTitle(QApplication::translate("SewingEditorClass", "group", 0));
        menuPath->setTitle(QApplication::translate("SewingEditorClass", "path", 0));
        menuOp->setTitle(QApplication::translate("SewingEditorClass", "op", 0));
        gbLayers->setTitle(QApplication::translate("SewingEditorClass", "Layers", 0));
#ifndef QT_NO_TOOLTIP
        pbNewLayer->setToolTip(QApplication::translate("SewingEditorClass", "copy selected objs to new layer (CTRL+N)", 0));
#endif // QT_NO_TOOLTIP
        pbNewLayer->setText(QApplication::translate("SewingEditorClass", "New", 0));
        pbNewLayer->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+N", 0));
#ifndef QT_NO_TOOLTIP
        pbMergeLayers->setToolTip(QApplication::translate("SewingEditorClass", "merge selected layers (CTRL+M)", 0));
#endif // QT_NO_TOOLTIP
        pbMergeLayers->setText(QApplication::translate("SewingEditorClass", "Merge", 0));
        pbMergeLayers->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+M", 0));
#ifndef QT_NO_TOOLTIP
        pbRemoveLayers->setToolTip(QApplication::translate("SewingEditorClass", "remove selected layers (CTRL+R)", 0));
#endif // QT_NO_TOOLTIP
        pbRemoveLayers->setText(QApplication::translate("SewingEditorClass", "Remove", 0));
        pbRemoveLayers->setShortcut(QApplication::translate("SewingEditorClass", "Ctrl+R", 0));
        gbParam->setTitle(QApplication::translate("SewingEditorClass", "param", 0));
#ifndef QT_NO_TOOLTIP
        pbSelectClosed->setToolTip(QApplication::translate("SewingEditorClass", "select closed polygons", 0));
#endif // QT_NO_TOOLTIP
        pbSelectClosed->setText(QApplication::translate("SewingEditorClass", "select closed", 0));
#ifndef QT_NO_TOOLTIP
        pbClosePolygon->setToolTip(QApplication::translate("SewingEditorClass", "make selected polygon closed (ALT+L)", 0));
#endif // QT_NO_TOOLTIP
        pbClosePolygon->setText(QApplication::translate("SewingEditorClass", "cLose polygon", 0));
        pbClosePolygon->setShortcut(QApplication::translate("SewingEditorClass", "Alt+L", 0));
#ifndef QT_NO_TOOLTIP
        pbRemovePairs->setToolTip(QApplication::translate("SewingEditorClass", "remove selected pairs", 0));
#endif // QT_NO_TOOLTIP
        pbRemovePairs->setText(QApplication::translate("SewingEditorClass", "Remove pairs", 0));
        pbRemovePairs->setShortcut(QApplication::translate("SewingEditorClass", "Alt+R", 0));
#ifndef QT_NO_TOOLTIP
        pbMakePair->setToolTip(QApplication::translate("SewingEditorClass", "make selected poly edges to pairs (ALT+M)", 0));
#endif // QT_NO_TOOLTIP
        pbMakePair->setText(QApplication::translate("SewingEditorClass", "Make pair", 0));
        pbMakePair->setShortcut(QApplication::translate("SewingEditorClass", "Alt+M", 0));
#ifndef QT_NO_TOOLTIP
        pbToConnectedGroups->setToolTip(QApplication::translate("SewingEditorClass", "convert selected path to polygons (ALT+C)", 0));
#endif // QT_NO_TOOLTIP
        pbToConnectedGroups->setText(QApplication::translate("SewingEditorClass", "Connected groups", 0));
        pbToConnectedGroups->setShortcut(QApplication::translate("SewingEditorClass", "Alt+C", 0));
#ifndef QT_NO_TOOLTIP
        pbSymmetricCopy->setToolTip(QApplication::translate("SewingEditorClass", "symmetrically copy selected polygons (ALT+S)", 0));
#endif // QT_NO_TOOLTIP
        pbSymmetricCopy->setText(QApplication::translate("SewingEditorClass", "Symmetric copy", 0));
        pbSymmetricCopy->setShortcut(QApplication::translate("SewingEditorClass", "Alt+S", 0));
#ifndef QT_NO_TOOLTIP
        pbPixelToMeter->setToolTip(QApplication::translate("SewingEditorClass", "1 meter  == ? pixels", 0));
#endif // QT_NO_TOOLTIP
        pbPixelToMeter->setText(QApplication::translate("SewingEditorClass", "pixel to meter", 0));
        pbPathSplitByIntersect->setText(QApplication::translate("SewingEditorClass", "path split by intersect", 0));
        groupBox->setTitle(QApplication::translate("SewingEditorClass", "mesh", 0));
        pbGenerateMesh->setText(QApplication::translate("SewingEditorClass", "Generate Mesh", 0));
        groupBox_2->setTitle(QApplication::translate("SewingEditorClass", "Smooth Poly", 0));
        pbSmoothPoly->setText(QApplication::translate("SewingEditorClass", "smooth poly", 0));
        dockWidgetLeft->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class SewingEditorClass: public Ui_SewingEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEWINGEDITOR_H
