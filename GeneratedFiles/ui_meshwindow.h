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
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "basemeshviewer.h"

QT_BEGIN_NAMESPACE

class Ui_MeshWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    BaseMeshViewer *widget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QDockWidget *dockWidgetRight;
    QWidget *dockWidgetRightContents;
    QGridLayout *gridLayout_7;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QComboBox *cbThickness;
    QLabel *label_2;
    QDoubleSpinBox *dbStiffBend;
    QLabel *label_3;
    QDoubleSpinBox *dbStiffMyu;
    QLabel *label_4;
    QDoubleSpinBox *dbStiffLambda;
    QLabel *label_5;
    QDoubleSpinBox *dbRho;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *label_6;
    QDoubleSpinBox *dbMyuK;
    QLabel *label_7;
    QDoubleSpinBox *dbMyuS;
    QLabel *label_8;
    QDoubleSpinBox *dbStiffN;
    QLabel *label_9;
    QDoubleSpinBox *dbStiffF;
    QLabel *label_14;
    QDoubleSpinBox *dbOffset;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_6;
    QPushButton *pbInverseCylinder;
    QPushButton *pbReInitMesh;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_5;
    QLabel *label_17;
    QSpinBox *sbCoarseMesh;
    QLabel *label_18;
    QSpinBox *sbDetailMesh;
    QDockWidget *dockWidgetLeft;
    QWidget *dockWidgetLeftContents;

    void setupUi(QMainWindow *MeshWindow)
    {
        if (MeshWindow->objectName().isEmpty())
            MeshWindow->setObjectName(QStringLiteral("MeshWindow"));
        MeshWindow->resize(800, 681);
        actionOpen = new QAction(MeshWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(MeshWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
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
        dockWidgetRight->setMinimumSize(QSize(300, 598));
        dockWidgetRight->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dockWidgetRightContents = new QWidget();
        dockWidgetRightContents->setObjectName(QStringLiteral("dockWidgetRightContents"));
        gridLayout_7 = new QGridLayout(dockWidgetRightContents);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
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

        gridLayout_2->addWidget(cbThickness, 0, 1, 1, 2);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 2);

        dbStiffBend = new QDoubleSpinBox(groupBox);
        dbStiffBend->setObjectName(QStringLiteral("dbStiffBend"));
        dbStiffBend->setDecimals(10);

        gridLayout_2->addWidget(dbStiffBend, 1, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        dbStiffMyu = new QDoubleSpinBox(groupBox);
        dbStiffMyu->setObjectName(QStringLiteral("dbStiffMyu"));
        dbStiffMyu->setDecimals(3);

        gridLayout_2->addWidget(dbStiffMyu, 2, 2, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 3, 0, 1, 2);

        dbStiffLambda = new QDoubleSpinBox(groupBox);
        dbStiffLambda->setObjectName(QStringLiteral("dbStiffLambda"));
        dbStiffLambda->setDecimals(3);

        gridLayout_2->addWidget(dbStiffLambda, 3, 2, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 4, 0, 1, 2);

        dbRho = new QDoubleSpinBox(groupBox);
        dbRho->setObjectName(QStringLiteral("dbRho"));
        dbRho->setDecimals(3);

        gridLayout_2->addWidget(dbRho, 4, 2, 1, 1);


        gridLayout_7->addWidget(groupBox, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer, 3, 0, 1, 1);

        groupBox_2 = new QGroupBox(dockWidgetRightContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 0, 0, 1, 1);

        dbMyuK = new QDoubleSpinBox(groupBox_2);
        dbMyuK->setObjectName(QStringLiteral("dbMyuK"));
        dbMyuK->setDecimals(3);

        gridLayout_3->addWidget(dbMyuK, 0, 1, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_3->addWidget(label_7, 1, 0, 1, 1);

        dbMyuS = new QDoubleSpinBox(groupBox_2);
        dbMyuS->setObjectName(QStringLiteral("dbMyuS"));
        dbMyuS->setDecimals(3);

        gridLayout_3->addWidget(dbMyuS, 1, 1, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_3->addWidget(label_8, 2, 0, 1, 1);

        dbStiffN = new QDoubleSpinBox(groupBox_2);
        dbStiffN->setObjectName(QStringLiteral("dbStiffN"));
        dbStiffN->setDecimals(3);

        gridLayout_3->addWidget(dbStiffN, 2, 1, 1, 1);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_3->addWidget(label_9, 3, 0, 1, 1);

        dbStiffF = new QDoubleSpinBox(groupBox_2);
        dbStiffF->setObjectName(QStringLiteral("dbStiffF"));
        dbStiffF->setDecimals(3);

        gridLayout_3->addWidget(dbStiffF, 3, 1, 1, 1);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_3->addWidget(label_14, 4, 0, 1, 1);

        dbOffset = new QDoubleSpinBox(groupBox_2);
        dbOffset->setObjectName(QStringLiteral("dbOffset"));
        dbOffset->setDecimals(3);

        gridLayout_3->addWidget(dbOffset, 4, 1, 1, 1);


        gridLayout_7->addWidget(groupBox_2, 1, 0, 1, 1);

        groupBox_5 = new QGroupBox(dockWidgetRightContents);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_6 = new QGridLayout(groupBox_5);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        pbInverseCylinder = new QPushButton(groupBox_5);
        pbInverseCylinder->setObjectName(QStringLiteral("pbInverseCylinder"));

        gridLayout_6->addWidget(pbInverseCylinder, 0, 0, 1, 1);

        pbReInitMesh = new QPushButton(groupBox_5);
        pbReInitMesh->setObjectName(QStringLiteral("pbReInitMesh"));

        gridLayout_6->addWidget(pbReInitMesh, 0, 1, 1, 1);


        gridLayout_7->addWidget(groupBox_5, 4, 0, 1, 1);

        groupBox_4 = new QGroupBox(dockWidgetRightContents);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_5 = new QGridLayout(groupBox_4);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        label_17 = new QLabel(groupBox_4);
        label_17->setObjectName(QStringLiteral("label_17"));

        gridLayout_5->addWidget(label_17, 0, 0, 1, 1);

        sbCoarseMesh = new QSpinBox(groupBox_4);
        sbCoarseMesh->setObjectName(QStringLiteral("sbCoarseMesh"));
        sbCoarseMesh->setMaximum(999999);

        gridLayout_5->addWidget(sbCoarseMesh, 0, 1, 2, 1);

        label_18 = new QLabel(groupBox_4);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout_5->addWidget(label_18, 1, 0, 2, 1);

        sbDetailMesh = new QSpinBox(groupBox_4);
        sbDetailMesh->setObjectName(QStringLiteral("sbDetailMesh"));
        sbDetailMesh->setMaximum(999999);

        gridLayout_5->addWidget(sbDetailMesh, 2, 1, 1, 1);


        gridLayout_7->addWidget(groupBox_4, 2, 0, 1, 1);

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
        menuFile->addAction(actionSave);

        retranslateUi(MeshWindow);

        cbThickness->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MeshWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MeshWindow)
    {
        MeshWindow->setWindowTitle(QApplication::translate("MeshWindow", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("MeshWindow", "open", 0));
        actionSave->setText(QApplication::translate("MeshWindow", "save", 0));
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
        label_2->setText(QApplication::translate("MeshWindow", "stiff_bend", 0));
        label_3->setText(QApplication::translate("MeshWindow", "stiff_myu", 0));
        label_4->setText(QApplication::translate("MeshWindow", "stiff_lambda", 0));
        label_5->setText(QApplication::translate("MeshWindow", "rho", 0));
        groupBox_2->setTitle(QApplication::translate("MeshWindow", "contact param", 0));
        label_6->setText(QApplication::translate("MeshWindow", "myu_k", 0));
        label_7->setText(QApplication::translate("MeshWindow", "myu_s", 0));
        label_8->setText(QApplication::translate("MeshWindow", "stiff_n", 0));
        label_9->setText(QApplication::translate("MeshWindow", "stiff_f", 0));
        label_14->setText(QApplication::translate("MeshWindow", "offset", 0));
        groupBox_5->setTitle(QApplication::translate("MeshWindow", "GroupBox", 0));
        pbInverseCylinder->setText(QApplication::translate("MeshWindow", "inverse cylinder", 0));
        pbReInitMesh->setText(QApplication::translate("MeshWindow", "re init mesh", 0));
        groupBox_4->setTitle(QApplication::translate("MeshWindow", "mesh triangles", 0));
        label_17->setText(QApplication::translate("MeshWindow", "coarse mesh", 0));
        label_18->setText(QApplication::translate("MeshWindow", "detail mesh", 0));
    } // retranslateUi

};

namespace Ui {
    class MeshWindow: public Ui_MeshWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHWINDOW_H
