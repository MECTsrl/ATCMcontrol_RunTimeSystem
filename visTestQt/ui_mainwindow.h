/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Nov 10 18:44:01 2011
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_7;
    QLabel *labelR7;
    QLCDNumber *lcdNumberR7;
    QHBoxLayout *horizontalLayout_6;
    QLabel *labelR6;
    QLCDNumber *lcdNumberR6;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelR5;
    QLCDNumber *lcdNumberR5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelR4;
    QLCDNumber *lcdNumberR4;
    QHBoxLayout *horizontalLayout_0;
    QLabel *labelR0;
    QLCDNumber *lcdNumberR0;
    QHBoxLayout *horizontalLayout_1;
    QLabel *labelR1;
    QLCDNumber *lcdNumberR1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelR2;
    QLCDNumber *lcdNumberR2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelR3;
    QLCDNumber *lcdNumberR3;
    QHBoxLayout *horizontalLayout_D;
    QLabel *labelRD;
    QLCDNumber *lcdNumberRD;
    QHBoxLayout *horizontalLayout_8;
    QLabel *labelR8;
    QLCDNumber *lcdNumberR8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *labelR9;
    QLCDNumber *lcdNumberR9;
    QHBoxLayout *horizontalLayout_A;
    QLabel *labelRA;
    QLCDNumber *lcdNumberRA;
    QHBoxLayout *horizontalLayout_B;
    QLabel *labelRB;
    QLCDNumber *lcdNumberRB;
    QHBoxLayout *horizontalLayout_C;
    QLabel *labelRC;
    QLCDNumber *lcdNumberRC;
    QHBoxLayout *horizontalLayout_E;
    QLabel *labelRE;
    QLCDNumber *lcdNumberRE;
    QHBoxLayout *horizontalLayout_F;
    QLabel *labelRF;
    QLCDNumber *lcdNumberRF;
    QHBoxLayout *horizontalLayout_G;
    QLabel *labelRG;
    QLCDNumber *lcdNumberRG;
    QLabel *labelMBRTU;
    QLineEdit *lineEditMBRTU;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *labelIP_12;
    QLabel *labelIP_11;
    QLabel *labelIP_5;
    QLabel *labelIP_4;
    QLabel *labelIP_3;
    QLabel *labelIP_2;
    QLabel *labelIP_1;
    QLabel *labelIP_6;
    QLabel *labelIP_7;
    QLabel *labelIP_8;
    QLabel *labelIP_9;
    QLabel *labelIP_10;
    QLabel *labelIP_13;
    QLabel *ledIP_2;
    QLabel *ledIP_13;
    QLabel *ledIP_12;
    QLabel *ledIP_11;
    QLabel *ledIP_7;
    QLabel *ledIP_10;
    QLabel *ledIP_6;
    QLabel *ledIP_5;
    QLabel *ledIP_4;
    QLabel *ledIP_3;
    QLabel *ledIP_9;
    QLabel *ledIP_8;
    QLabel *ledIP_1;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(480, 272);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(220, 280, 46, 51));
        gridLayoutWidget_2 = new QWidget(centralWidget);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(10, 100, 461, 141));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        labelR7 = new QLabel(gridLayoutWidget_2);
        labelR7->setObjectName(QString::fromUtf8("labelR7"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelR7->sizePolicy().hasHeightForWidth());
        labelR7->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        labelR7->setFont(font);

        horizontalLayout_7->addWidget(labelR7);

        lcdNumberR7 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR7->setObjectName(QString::fromUtf8("lcdNumberR7"));
        lcdNumberR7->setFont(font);
        lcdNumberR7->setAutoFillBackground(false);
        lcdNumberR7->setSmallDecimalPoint(false);
        lcdNumberR7->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR7->setProperty("value", QVariant(0));

        horizontalLayout_7->addWidget(lcdNumberR7);


        gridLayout_2->addLayout(horizontalLayout_7, 1, 3, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        labelR6 = new QLabel(gridLayoutWidget_2);
        labelR6->setObjectName(QString::fromUtf8("labelR6"));
        sizePolicy.setHeightForWidth(labelR6->sizePolicy().hasHeightForWidth());
        labelR6->setSizePolicy(sizePolicy);
        labelR6->setFont(font);

        horizontalLayout_6->addWidget(labelR6);

        lcdNumberR6 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR6->setObjectName(QString::fromUtf8("lcdNumberR6"));
        lcdNumberR6->setFont(font);
        lcdNumberR6->setAutoFillBackground(false);
        lcdNumberR6->setSmallDecimalPoint(false);
        lcdNumberR6->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR6->setProperty("value", QVariant(0));

        horizontalLayout_6->addWidget(lcdNumberR6);


        gridLayout_2->addLayout(horizontalLayout_6, 1, 2, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        labelR5 = new QLabel(gridLayoutWidget_2);
        labelR5->setObjectName(QString::fromUtf8("labelR5"));
        sizePolicy.setHeightForWidth(labelR5->sizePolicy().hasHeightForWidth());
        labelR5->setSizePolicy(sizePolicy);
        labelR5->setFont(font);

        horizontalLayout_5->addWidget(labelR5);

        lcdNumberR5 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR5->setObjectName(QString::fromUtf8("lcdNumberR5"));
        lcdNumberR5->setFont(font);
        lcdNumberR5->setAutoFillBackground(false);
        lcdNumberR5->setSmallDecimalPoint(false);
        lcdNumberR5->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR5->setProperty("value", QVariant(0));

        horizontalLayout_5->addWidget(lcdNumberR5);


        gridLayout_2->addLayout(horizontalLayout_5, 1, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelR4 = new QLabel(gridLayoutWidget_2);
        labelR4->setObjectName(QString::fromUtf8("labelR4"));
        sizePolicy.setHeightForWidth(labelR4->sizePolicy().hasHeightForWidth());
        labelR4->setSizePolicy(sizePolicy);
        labelR4->setFont(font);

        horizontalLayout_4->addWidget(labelR4);

        lcdNumberR4 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR4->setObjectName(QString::fromUtf8("lcdNumberR4"));
        lcdNumberR4->setFont(font);
        lcdNumberR4->setAutoFillBackground(false);
        lcdNumberR4->setSmallDecimalPoint(false);
        lcdNumberR4->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR4->setProperty("value", QVariant(0));

        horizontalLayout_4->addWidget(lcdNumberR4);


        gridLayout_2->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        horizontalLayout_0 = new QHBoxLayout();
        horizontalLayout_0->setSpacing(6);
        horizontalLayout_0->setObjectName(QString::fromUtf8("horizontalLayout_0"));
        labelR0 = new QLabel(gridLayoutWidget_2);
        labelR0->setObjectName(QString::fromUtf8("labelR0"));
        sizePolicy.setHeightForWidth(labelR0->sizePolicy().hasHeightForWidth());
        labelR0->setSizePolicy(sizePolicy);
        labelR0->setFont(font);

        horizontalLayout_0->addWidget(labelR0);

        lcdNumberR0 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR0->setObjectName(QString::fromUtf8("lcdNumberR0"));
        lcdNumberR0->setFont(font);
        lcdNumberR0->setAutoFillBackground(false);
        lcdNumberR0->setSmallDecimalPoint(false);
        lcdNumberR0->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR0->setProperty("value", QVariant(0));

        horizontalLayout_0->addWidget(lcdNumberR0);


        gridLayout_2->addLayout(horizontalLayout_0, 0, 0, 1, 1);

        horizontalLayout_1 = new QHBoxLayout();
        horizontalLayout_1->setSpacing(6);
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        labelR1 = new QLabel(gridLayoutWidget_2);
        labelR1->setObjectName(QString::fromUtf8("labelR1"));
        sizePolicy.setHeightForWidth(labelR1->sizePolicy().hasHeightForWidth());
        labelR1->setSizePolicy(sizePolicy);
        labelR1->setFont(font);

        horizontalLayout_1->addWidget(labelR1);

        lcdNumberR1 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR1->setObjectName(QString::fromUtf8("lcdNumberR1"));
        lcdNumberR1->setFont(font);
        lcdNumberR1->setAutoFillBackground(false);
        lcdNumberR1->setSmallDecimalPoint(false);
        lcdNumberR1->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR1->setProperty("value", QVariant(0));

        horizontalLayout_1->addWidget(lcdNumberR1);


        gridLayout_2->addLayout(horizontalLayout_1, 0, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelR2 = new QLabel(gridLayoutWidget_2);
        labelR2->setObjectName(QString::fromUtf8("labelR2"));
        sizePolicy.setHeightForWidth(labelR2->sizePolicy().hasHeightForWidth());
        labelR2->setSizePolicy(sizePolicy);
        labelR2->setFont(font);

        horizontalLayout_2->addWidget(labelR2);

        lcdNumberR2 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR2->setObjectName(QString::fromUtf8("lcdNumberR2"));
        lcdNumberR2->setFont(font);
        lcdNumberR2->setAutoFillBackground(false);
        lcdNumberR2->setSmallDecimalPoint(false);
        lcdNumberR2->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR2->setProperty("value", QVariant(0));

        horizontalLayout_2->addWidget(lcdNumberR2);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 2, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelR3 = new QLabel(gridLayoutWidget_2);
        labelR3->setObjectName(QString::fromUtf8("labelR3"));
        sizePolicy.setHeightForWidth(labelR3->sizePolicy().hasHeightForWidth());
        labelR3->setSizePolicy(sizePolicy);
        labelR3->setFont(font);

        horizontalLayout_3->addWidget(labelR3);

        lcdNumberR3 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR3->setObjectName(QString::fromUtf8("lcdNumberR3"));
        lcdNumberR3->setFont(font);
        lcdNumberR3->setAutoFillBackground(false);
        lcdNumberR3->setSmallDecimalPoint(false);
        lcdNumberR3->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR3->setProperty("value", QVariant(0));

        horizontalLayout_3->addWidget(lcdNumberR3);


        gridLayout_2->addLayout(horizontalLayout_3, 0, 3, 1, 1);

        horizontalLayout_D = new QHBoxLayout();
        horizontalLayout_D->setSpacing(6);
        horizontalLayout_D->setObjectName(QString::fromUtf8("horizontalLayout_D"));
        labelRD = new QLabel(gridLayoutWidget_2);
        labelRD->setObjectName(QString::fromUtf8("labelRD"));
        sizePolicy.setHeightForWidth(labelRD->sizePolicy().hasHeightForWidth());
        labelRD->setSizePolicy(sizePolicy);
        labelRD->setFont(font);

        horizontalLayout_D->addWidget(labelRD);

        lcdNumberRD = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRD->setObjectName(QString::fromUtf8("lcdNumberRD"));
        lcdNumberRD->setFont(font);
        lcdNumberRD->setAutoFillBackground(false);
        lcdNumberRD->setSmallDecimalPoint(false);
        lcdNumberRD->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRD->setProperty("value", QVariant(0));

        horizontalLayout_D->addWidget(lcdNumberRD);


        gridLayout_2->addLayout(horizontalLayout_D, 2, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        labelR8 = new QLabel(gridLayoutWidget_2);
        labelR8->setObjectName(QString::fromUtf8("labelR8"));
        sizePolicy.setHeightForWidth(labelR8->sizePolicy().hasHeightForWidth());
        labelR8->setSizePolicy(sizePolicy);
        labelR8->setFont(font);

        horizontalLayout_8->addWidget(labelR8);

        lcdNumberR8 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR8->setObjectName(QString::fromUtf8("lcdNumberR8"));
        lcdNumberR8->setFont(font);
        lcdNumberR8->setAutoFillBackground(false);
        lcdNumberR8->setSmallDecimalPoint(false);
        lcdNumberR8->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR8->setProperty("value", QVariant(0));

        horizontalLayout_8->addWidget(lcdNumberR8);


        gridLayout_2->addLayout(horizontalLayout_8, 2, 1, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        labelR9 = new QLabel(gridLayoutWidget_2);
        labelR9->setObjectName(QString::fromUtf8("labelR9"));
        sizePolicy.setHeightForWidth(labelR9->sizePolicy().hasHeightForWidth());
        labelR9->setSizePolicy(sizePolicy);
        labelR9->setFont(font);

        horizontalLayout_9->addWidget(labelR9);

        lcdNumberR9 = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberR9->setObjectName(QString::fromUtf8("lcdNumberR9"));
        lcdNumberR9->setFont(font);
        lcdNumberR9->setAutoFillBackground(false);
        lcdNumberR9->setSmallDecimalPoint(false);
        lcdNumberR9->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberR9->setProperty("value", QVariant(0));

        horizontalLayout_9->addWidget(lcdNumberR9);


        gridLayout_2->addLayout(horizontalLayout_9, 2, 2, 1, 1);

        horizontalLayout_A = new QHBoxLayout();
        horizontalLayout_A->setSpacing(6);
        horizontalLayout_A->setObjectName(QString::fromUtf8("horizontalLayout_A"));
        labelRA = new QLabel(gridLayoutWidget_2);
        labelRA->setObjectName(QString::fromUtf8("labelRA"));
        sizePolicy.setHeightForWidth(labelRA->sizePolicy().hasHeightForWidth());
        labelRA->setSizePolicy(sizePolicy);
        labelRA->setFont(font);

        horizontalLayout_A->addWidget(labelRA);

        lcdNumberRA = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRA->setObjectName(QString::fromUtf8("lcdNumberRA"));
        lcdNumberRA->setFont(font);
        lcdNumberRA->setAutoFillBackground(false);
        lcdNumberRA->setSmallDecimalPoint(false);
        lcdNumberRA->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRA->setProperty("value", QVariant(0));

        horizontalLayout_A->addWidget(lcdNumberRA);


        gridLayout_2->addLayout(horizontalLayout_A, 2, 3, 1, 1);

        horizontalLayout_B = new QHBoxLayout();
        horizontalLayout_B->setSpacing(6);
        horizontalLayout_B->setObjectName(QString::fromUtf8("horizontalLayout_B"));
        labelRB = new QLabel(gridLayoutWidget_2);
        labelRB->setObjectName(QString::fromUtf8("labelRB"));
        sizePolicy.setHeightForWidth(labelRB->sizePolicy().hasHeightForWidth());
        labelRB->setSizePolicy(sizePolicy);
        labelRB->setFont(font);

        horizontalLayout_B->addWidget(labelRB);

        lcdNumberRB = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRB->setObjectName(QString::fromUtf8("lcdNumberRB"));
        lcdNumberRB->setFont(font);
        lcdNumberRB->setAutoFillBackground(false);
        lcdNumberRB->setSmallDecimalPoint(false);
        lcdNumberRB->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRB->setProperty("value", QVariant(0));

        horizontalLayout_B->addWidget(lcdNumberRB);


        gridLayout_2->addLayout(horizontalLayout_B, 3, 0, 1, 1);

        horizontalLayout_C = new QHBoxLayout();
        horizontalLayout_C->setSpacing(6);
        horizontalLayout_C->setObjectName(QString::fromUtf8("horizontalLayout_C"));
        labelRC = new QLabel(gridLayoutWidget_2);
        labelRC->setObjectName(QString::fromUtf8("labelRC"));
        sizePolicy.setHeightForWidth(labelRC->sizePolicy().hasHeightForWidth());
        labelRC->setSizePolicy(sizePolicy);
        labelRC->setFont(font);

        horizontalLayout_C->addWidget(labelRC);

        lcdNumberRC = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRC->setObjectName(QString::fromUtf8("lcdNumberRC"));
        lcdNumberRC->setFont(font);
        lcdNumberRC->setAutoFillBackground(false);
        lcdNumberRC->setSmallDecimalPoint(false);
        lcdNumberRC->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRC->setProperty("value", QVariant(0));

        horizontalLayout_C->addWidget(lcdNumberRC);


        gridLayout_2->addLayout(horizontalLayout_C, 3, 1, 1, 1);

        horizontalLayout_E = new QHBoxLayout();
        horizontalLayout_E->setSpacing(6);
        horizontalLayout_E->setObjectName(QString::fromUtf8("horizontalLayout_E"));
        labelRE = new QLabel(gridLayoutWidget_2);
        labelRE->setObjectName(QString::fromUtf8("labelRE"));
        sizePolicy.setHeightForWidth(labelRE->sizePolicy().hasHeightForWidth());
        labelRE->setSizePolicy(sizePolicy);
        labelRE->setFont(font);

        horizontalLayout_E->addWidget(labelRE);

        lcdNumberRE = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRE->setObjectName(QString::fromUtf8("lcdNumberRE"));
        lcdNumberRE->setFont(font);
        lcdNumberRE->setAutoFillBackground(false);
        lcdNumberRE->setSmallDecimalPoint(false);
        lcdNumberRE->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRE->setProperty("value", QVariant(0));

        horizontalLayout_E->addWidget(lcdNumberRE);


        gridLayout_2->addLayout(horizontalLayout_E, 3, 2, 1, 1);

        horizontalLayout_F = new QHBoxLayout();
        horizontalLayout_F->setSpacing(6);
        horizontalLayout_F->setObjectName(QString::fromUtf8("horizontalLayout_F"));
        labelRF = new QLabel(gridLayoutWidget_2);
        labelRF->setObjectName(QString::fromUtf8("labelRF"));
        sizePolicy.setHeightForWidth(labelRF->sizePolicy().hasHeightForWidth());
        labelRF->setSizePolicy(sizePolicy);
        labelRF->setFont(font);

        horizontalLayout_F->addWidget(labelRF);

        lcdNumberRF = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRF->setObjectName(QString::fromUtf8("lcdNumberRF"));
        lcdNumberRF->setFont(font);
        lcdNumberRF->setAutoFillBackground(false);
        lcdNumberRF->setSmallDecimalPoint(false);
        lcdNumberRF->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRF->setProperty("value", QVariant(0));

        horizontalLayout_F->addWidget(lcdNumberRF);


        gridLayout_2->addLayout(horizontalLayout_F, 3, 3, 1, 1);

        horizontalLayout_G = new QHBoxLayout();
        horizontalLayout_G->setSpacing(6);
        horizontalLayout_G->setObjectName(QString::fromUtf8("horizontalLayout_G"));
        labelRG = new QLabel(gridLayoutWidget_2);
        labelRG->setObjectName(QString::fromUtf8("labelRG"));
        sizePolicy.setHeightForWidth(labelRG->sizePolicy().hasHeightForWidth());
        labelRG->setSizePolicy(sizePolicy);
        labelRG->setFont(font);

        horizontalLayout_G->addWidget(labelRG);

        lcdNumberRG = new QLCDNumber(gridLayoutWidget_2);
        lcdNumberRG->setObjectName(QString::fromUtf8("lcdNumberRG"));
        lcdNumberRG->setFont(font);
        lcdNumberRG->setAutoFillBackground(false);
        lcdNumberRG->setSmallDecimalPoint(false);
        lcdNumberRG->setSegmentStyle(QLCDNumber::Flat);
        lcdNumberRG->setProperty("value", QVariant(0));

        horizontalLayout_G->addWidget(lcdNumberRG);


        gridLayout_2->addLayout(horizontalLayout_G, 4, 0, 1, 1);

        labelMBRTU = new QLabel(gridLayoutWidget_2);
        labelMBRTU->setObjectName(QString::fromUtf8("labelMBRTU"));
        sizePolicy.setHeightForWidth(labelMBRTU->sizePolicy().hasHeightForWidth());
        labelMBRTU->setSizePolicy(sizePolicy);
        labelMBRTU->setFont(font);

        gridLayout_2->addWidget(labelMBRTU, 4, 2, 1, 1);

        lineEditMBRTU = new QLineEdit(gridLayoutWidget_2);
        lineEditMBRTU->setObjectName(QString::fromUtf8("lineEditMBRTU"));

        gridLayout_2->addWidget(lineEditMBRTU, 4, 3, 1, 1);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 461, 91));
        gridLayoutWidget = new QWidget(groupBox);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 20, 441, 61));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        labelIP_12 = new QLabel(gridLayoutWidget);
        labelIP_12->setObjectName(QString::fromUtf8("labelIP_12"));

        gridLayout->addWidget(labelIP_12, 0, 1, 1, 1);

        labelIP_11 = new QLabel(gridLayoutWidget);
        labelIP_11->setObjectName(QString::fromUtf8("labelIP_11"));

        gridLayout->addWidget(labelIP_11, 0, 2, 1, 1);

        labelIP_5 = new QLabel(gridLayoutWidget);
        labelIP_5->setObjectName(QString::fromUtf8("labelIP_5"));

        gridLayout->addWidget(labelIP_5, 0, 8, 1, 1);

        labelIP_4 = new QLabel(gridLayoutWidget);
        labelIP_4->setObjectName(QString::fromUtf8("labelIP_4"));

        gridLayout->addWidget(labelIP_4, 0, 9, 1, 1);

        labelIP_3 = new QLabel(gridLayoutWidget);
        labelIP_3->setObjectName(QString::fromUtf8("labelIP_3"));

        gridLayout->addWidget(labelIP_3, 0, 10, 1, 1);

        labelIP_2 = new QLabel(gridLayoutWidget);
        labelIP_2->setObjectName(QString::fromUtf8("labelIP_2"));

        gridLayout->addWidget(labelIP_2, 0, 11, 1, 1);

        labelIP_1 = new QLabel(gridLayoutWidget);
        labelIP_1->setObjectName(QString::fromUtf8("labelIP_1"));

        gridLayout->addWidget(labelIP_1, 0, 12, 1, 1);

        labelIP_6 = new QLabel(gridLayoutWidget);
        labelIP_6->setObjectName(QString::fromUtf8("labelIP_6"));

        gridLayout->addWidget(labelIP_6, 0, 7, 1, 1);

        labelIP_7 = new QLabel(gridLayoutWidget);
        labelIP_7->setObjectName(QString::fromUtf8("labelIP_7"));

        gridLayout->addWidget(labelIP_7, 0, 6, 1, 1);

        labelIP_8 = new QLabel(gridLayoutWidget);
        labelIP_8->setObjectName(QString::fromUtf8("labelIP_8"));

        gridLayout->addWidget(labelIP_8, 0, 5, 1, 1);

        labelIP_9 = new QLabel(gridLayoutWidget);
        labelIP_9->setObjectName(QString::fromUtf8("labelIP_9"));

        gridLayout->addWidget(labelIP_9, 0, 4, 1, 1);

        labelIP_10 = new QLabel(gridLayoutWidget);
        labelIP_10->setObjectName(QString::fromUtf8("labelIP_10"));

        gridLayout->addWidget(labelIP_10, 0, 3, 1, 1);

        labelIP_13 = new QLabel(gridLayoutWidget);
        labelIP_13->setObjectName(QString::fromUtf8("labelIP_13"));

        gridLayout->addWidget(labelIP_13, 0, 0, 1, 1);

        ledIP_2 = new QLabel(gridLayoutWidget);
        ledIP_2->setObjectName(QString::fromUtf8("ledIP_2"));
        ledIP_2->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_2->setScaledContents(true);
        ledIP_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_2, 1, 11, 1, 1);

        ledIP_13 = new QLabel(gridLayoutWidget);
        ledIP_13->setObjectName(QString::fromUtf8("ledIP_13"));
        ledIP_13->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_13->setScaledContents(true);
        ledIP_13->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_13, 1, 0, 1, 1);

        ledIP_12 = new QLabel(gridLayoutWidget);
        ledIP_12->setObjectName(QString::fromUtf8("ledIP_12"));
        ledIP_12->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_12->setScaledContents(true);
        ledIP_12->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_12, 1, 1, 1, 1);

        ledIP_11 = new QLabel(gridLayoutWidget);
        ledIP_11->setObjectName(QString::fromUtf8("ledIP_11"));
        ledIP_11->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_11->setScaledContents(true);
        ledIP_11->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_11, 1, 2, 1, 1);

        ledIP_7 = new QLabel(gridLayoutWidget);
        ledIP_7->setObjectName(QString::fromUtf8("ledIP_7"));
        ledIP_7->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_7->setScaledContents(true);
        ledIP_7->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_7, 1, 6, 1, 1);

        ledIP_10 = new QLabel(gridLayoutWidget);
        ledIP_10->setObjectName(QString::fromUtf8("ledIP_10"));
        ledIP_10->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_10->setScaledContents(true);
        ledIP_10->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_10, 1, 3, 1, 1);

        ledIP_6 = new QLabel(gridLayoutWidget);
        ledIP_6->setObjectName(QString::fromUtf8("ledIP_6"));
        ledIP_6->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_6->setScaledContents(true);
        ledIP_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_6, 1, 7, 1, 1);

        ledIP_5 = new QLabel(gridLayoutWidget);
        ledIP_5->setObjectName(QString::fromUtf8("ledIP_5"));
        ledIP_5->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_5->setScaledContents(true);
        ledIP_5->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_5, 1, 8, 1, 1);

        ledIP_4 = new QLabel(gridLayoutWidget);
        ledIP_4->setObjectName(QString::fromUtf8("ledIP_4"));
        ledIP_4->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_4->setScaledContents(true);
        ledIP_4->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_4, 1, 9, 1, 1);

        ledIP_3 = new QLabel(gridLayoutWidget);
        ledIP_3->setObjectName(QString::fromUtf8("ledIP_3"));
        ledIP_3->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_3->setScaledContents(true);
        ledIP_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_3, 1, 10, 1, 1);

        ledIP_9 = new QLabel(gridLayoutWidget);
        ledIP_9->setObjectName(QString::fromUtf8("ledIP_9"));
        ledIP_9->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_9->setScaledContents(true);
        ledIP_9->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_9, 1, 4, 1, 1);

        ledIP_8 = new QLabel(gridLayoutWidget);
        ledIP_8->setObjectName(QString::fromUtf8("ledIP_8"));
        ledIP_8->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_8->setScaledContents(true);
        ledIP_8->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_8, 1, 5, 1, 1);

        ledIP_1 = new QLabel(gridLayoutWidget);
        ledIP_1->setObjectName(QString::fromUtf8("ledIP_1"));
        ledIP_1->setPixmap(QPixmap(QString::fromUtf8(":/led/img/led-red-off.png")));
        ledIP_1->setScaledContents(true);
        ledIP_1->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ledIP_1, 1, 12, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 480, 21));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        labelR7->setText(QApplication::translate("MainWindow", "R7 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR6->setText(QApplication::translate("MainWindow", "R6 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR5->setText(QApplication::translate("MainWindow", "R5 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR4->setText(QApplication::translate("MainWindow", "R4 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR0->setText(QApplication::translate("MainWindow", "R0 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR1->setText(QApplication::translate("MainWindow", "R1 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR2->setText(QApplication::translate("MainWindow", "R2 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelR3->setText(QApplication::translate("MainWindow", "R3 [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelRD->setText(QApplication::translate("MainWindow", "R8 [V]:", 0, QApplication::UnicodeUTF8));
        labelR8->setText(QApplication::translate("MainWindow", "R9 [A]:", 0, QApplication::UnicodeUTF8));
        labelR9->setText(QApplication::translate("MainWindow", "RA [A]:", 0, QApplication::UnicodeUTF8));
        labelRA->setText(QApplication::translate("MainWindow", "RB [mA]:", 0, QApplication::UnicodeUTF8));
        labelRB->setText(QApplication::translate("MainWindow", "RC [mA]:", 0, QApplication::UnicodeUTF8));
        labelRC->setText(QApplication::translate("MainWindow", "RD [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelRE->setText(QApplication::translate("MainWindow", "RE [\302\260C]:", 0, QApplication::UnicodeUTF8));
        labelRF->setText(QApplication::translate("MainWindow", "RF [mA]:", 0, QApplication::UnicodeUTF8));
        labelRG->setText(QApplication::translate("MainWindow", "RG [mA]:", 0, QApplication::UnicodeUTF8));
        labelMBRTU->setText(QApplication::translate("MainWindow", "Modbus:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Input", 0, QApplication::UnicodeUTF8));
        labelIP_12->setText(QApplication::translate("MainWindow", "IP12", 0, QApplication::UnicodeUTF8));
        labelIP_11->setText(QApplication::translate("MainWindow", "IP11", 0, QApplication::UnicodeUTF8));
        labelIP_5->setText(QApplication::translate("MainWindow", "IP5", 0, QApplication::UnicodeUTF8));
        labelIP_4->setText(QApplication::translate("MainWindow", "IP4", 0, QApplication::UnicodeUTF8));
        labelIP_3->setText(QApplication::translate("MainWindow", "IP3", 0, QApplication::UnicodeUTF8));
        labelIP_2->setText(QApplication::translate("MainWindow", "IP2", 0, QApplication::UnicodeUTF8));
        labelIP_1->setText(QApplication::translate("MainWindow", "IP1", 0, QApplication::UnicodeUTF8));
        labelIP_6->setText(QApplication::translate("MainWindow", "IP6", 0, QApplication::UnicodeUTF8));
        labelIP_7->setText(QApplication::translate("MainWindow", "IP7", 0, QApplication::UnicodeUTF8));
        labelIP_8->setText(QApplication::translate("MainWindow", "IP8", 0, QApplication::UnicodeUTF8));
        labelIP_9->setText(QApplication::translate("MainWindow", "IP9", 0, QApplication::UnicodeUTF8));
        labelIP_10->setText(QApplication::translate("MainWindow", "IP10", 0, QApplication::UnicodeUTF8));
        labelIP_13->setText(QApplication::translate("MainWindow", "IP13", 0, QApplication::UnicodeUTF8));
        ledIP_2->setText(QString());
        ledIP_13->setText(QString());
        ledIP_12->setText(QString());
        ledIP_11->setText(QString());
        ledIP_7->setText(QString());
        ledIP_10->setText(QString());
        ledIP_6->setText(QString());
        ledIP_5->setText(QString());
        ledIP_4->setText(QString());
        ledIP_3->setText(QString());
        ledIP_9->setText(QString());
        ledIP_8->setText(QString());
        ledIP_1->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
