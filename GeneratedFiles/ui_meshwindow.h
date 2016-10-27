/********************************************************************************
** Form generated from reading UI file 'meshwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHWINDOW_H
#define UI_MESHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "basemeshviewer.h"

QT_BEGIN_NAMESPACE

class Ui_MeshWindow
{
public:
    QAction *actionOpen;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    BaseMeshViewer *widget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetRightContents;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QComboBox *cbThickness;
    QPushButton *pbInverseCylinder;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetLeftContents;

    void setupUi(QMainWindow *MeshWindow)
    {
        if (MeshWindow->objectName().isEmpty())
            MeshWindow->setObjectName(QStringLiteral("MeshWindow"));
        MeshWindow->resize(800, 600);
        actionOpen = new QAction(MeshWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralwidget = new QWidget(MeshWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget = new BaseMeshViewer(centralwidget);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout->addWidget(widget, 0, 0, 1, 1);

        MeshWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MeshWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MeshWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MeshWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MeshWindow->setStatusBar(statusbar);
        dockWidgetRight = new QDockWidget(MeshWindow);
        dockWidgetRight->setObjectName(QStringLiteral("dockWidgetRight"));
        dockWidgetRight->setMinimumSize(QSize(300, 200));
        dockWidgetRight->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetRightContents = new QWidget();
        dockWidgetRightContents->setObjectName(QStringLiteral("dockWidgetRightContents"));
        gridLayout_3 = new QGridLayout(dockWidgetRightContents);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox = new QGroupBox(dockWidgetRightContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 0));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        cbThickness = new QComboBox(groupBox);
        cbThickness->setObjectName(QStringLiteral("cbThickness"));
        cbThickness->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(cbThickness, 0, 1, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        pbInverseCylinder = new QPushButton(dockWidgetRightContents);
        pbInverseCylinder->setObjectName(QStringLiteral("pbInverseCylinder"));

        gridLayout_3->addWidget(pbInverseCylinder, 1, 0, 1, 1);

        dockWidgetRight->setWidget(dockWidgetRightContents);
        MeshWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidgetRight);
        dockWidgetLeft = new QDockWidget(MeshWindow);
        dockWidgetLeft->setObjectName(QStringLiteral("dockWidgetLeft"));
        dockWidgetLeft->setMinimumSize(QSize(100, 38));
        dockWidgetLeft->setMaximumSize(QSize(100, 524287));
        dockWidgetLeft->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetLeftContents = new QWidget();
        dockWidgetLeftContents->setObjectName(QStringLiteral("dockWidgetLeftContents"));
        dockWidgetLeft->setWidget(dockWidgetLeftContents);
        MeshWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidgetLeft);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(MeshWindow);

        cbThickness->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MeshWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MeshWindow)
    {
        MeshWindow->setWindowTitle(QApplication::translate("MeshWindow", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("MeshWindow", "open", 0));
        menuFile->setTitle(QApplication::translate("MeshWindow", "file", 0));
        groupBox->setTitle(QApplication::translate("MeshWindow", "cloth param", 0));
        label->setText(QApplication::translate("MeshWindow", "Thickness", 0));
        cbThickness->clear();
        cbThickness->insertItems(0, QStringList()
         << QApplication::translate("MeshWindow", "thick", 0)
         << QApplication::translate("MeshWindow", "medium", 0)
         << QApplication::translate("MeshWindow", "thin", 0)
         << QApplication::translate("MeshWindow", "very thin", 0)
        );
        cbThickness->setCurrentText(QApplication::translate("MeshWindow", "thin", 0));
        pbInverseCylinder->setText(QApplication::translate("MeshWindow", "inverse cylinder", 0));
    } // retranslateUi

};

namespace Ui {
    class MeshWindow: public Ui_MeshWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHWINDOW_H
