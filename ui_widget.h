/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QProgressBar *timeBar;
    QLabel *labelTime;
    QLabel *numTime;
    QLabel *labelScore;
    QLabel *numScore;
    QLabel *labelName;
    QLabel *labelProp1;
    QLabel *labelProp2;
    QLabel *labelProp3;
    QLabel *labelProp4;
    QLabel *labelName2;
    QLabel *labelScore2;
    QLabel *numScore2;
    QLabel *labelProp5;
    QPushButton *pauseButton;
    QPushButton *returnButton;
    QLabel *labelLayer;
    QPushButton *restartButton;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(658, 491);
        timeBar = new QProgressBar(Widget);
        timeBar->setObjectName(QString::fromUtf8("timeBar"));
        timeBar->setGeometry(QRect(510, 180, 107, 20));
        timeBar->setValue(24);
        timeBar->setTextVisible(false);
        labelTime = new QLabel(Widget);
        labelTime->setObjectName(QString::fromUtf8("labelTime"));
        labelTime->setGeometry(QRect(480, 120, 26, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setBold(true);
        font.setWeight(75);
        labelTime->setFont(font);
        labelTime->setAlignment(Qt::AlignCenter);
        numTime = new QLabel(Widget);
        numTime->setObjectName(QString::fromUtf8("numTime"));
        numTime->setGeometry(QRect(520, 120, 107, 32));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Microsoft YaHei"));
        font1.setBold(true);
        font1.setWeight(75);
        numTime->setFont(font1);
        numTime->setAlignment(Qt::AlignCenter);
        labelScore = new QLabel(Widget);
        labelScore->setObjectName(QString::fromUtf8("labelScore"));
        labelScore->setGeometry(QRect(480, 80, 26, 31));
        labelScore->setFont(font);
        labelScore->setAlignment(Qt::AlignCenter);
        numScore = new QLabel(Widget);
        numScore->setObjectName(QString::fromUtf8("numScore"));
        numScore->setGeometry(QRect(520, 80, 101, 32));
        numScore->setFont(font1);
        numScore->setAlignment(Qt::AlignCenter);
        labelName = new QLabel(Widget);
        labelName->setObjectName(QString::fromUtf8("labelName"));
        labelName->setGeometry(QRect(480, 60, 54, 12));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        labelName->setFont(font2);
        labelName->setAlignment(Qt::AlignCenter);
        labelProp1 = new QLabel(Widget);
        labelProp1->setObjectName(QString::fromUtf8("labelProp1"));
        labelProp1->setGeometry(QRect(520, 230, 54, 12));
        labelProp1->setFont(font2);
        labelProp1->setAlignment(Qt::AlignCenter);
        labelProp2 = new QLabel(Widget);
        labelProp2->setObjectName(QString::fromUtf8("labelProp2"));
        labelProp2->setGeometry(QRect(520, 250, 54, 12));
        labelProp2->setFont(font2);
        labelProp2->setAlignment(Qt::AlignCenter);
        labelProp3 = new QLabel(Widget);
        labelProp3->setObjectName(QString::fromUtf8("labelProp3"));
        labelProp3->setGeometry(QRect(520, 270, 54, 12));
        labelProp3->setFont(font2);
        labelProp3->setAlignment(Qt::AlignCenter);
        labelProp4 = new QLabel(Widget);
        labelProp4->setObjectName(QString::fromUtf8("labelProp4"));
        labelProp4->setGeometry(QRect(520, 290, 54, 12));
        labelProp4->setFont(font2);
        labelProp4->setAlignment(Qt::AlignCenter);
        labelName2 = new QLabel(Widget);
        labelName2->setObjectName(QString::fromUtf8("labelName2"));
        labelName2->setGeometry(QRect(500, 350, 54, 12));
        labelName2->setFont(font2);
        labelName2->setAlignment(Qt::AlignCenter);
        labelScore2 = new QLabel(Widget);
        labelScore2->setObjectName(QString::fromUtf8("labelScore2"));
        labelScore2->setGeometry(QRect(500, 380, 26, 31));
        labelScore2->setFont(font);
        labelScore2->setAlignment(Qt::AlignCenter);
        numScore2 = new QLabel(Widget);
        numScore2->setObjectName(QString::fromUtf8("numScore2"));
        numScore2->setGeometry(QRect(520, 380, 101, 32));
        numScore2->setFont(font1);
        numScore2->setAlignment(Qt::AlignCenter);
        labelProp5 = new QLabel(Widget);
        labelProp5->setObjectName(QString::fromUtf8("labelProp5"));
        labelProp5->setGeometry(QRect(520, 310, 54, 12));
        labelProp5->setFont(font2);
        labelProp5->setAlignment(Qt::AlignCenter);
        pauseButton = new QPushButton(Widget);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        pauseButton->setGeometry(QRect(569, 20, 21, 20));
        pauseButton->setFlat(true);
        returnButton = new QPushButton(Widget);
        returnButton->setObjectName(QString::fromUtf8("returnButton"));
        returnButton->setGeometry(QRect(540, 20, 21, 20));
        returnButton->setFlat(true);
        labelLayer = new QLabel(Widget);
        labelLayer->setObjectName(QString::fromUtf8("labelLayer"));
        labelLayer->setEnabled(true);
        labelLayer->setGeometry(QRect(470, 450, 54, 12));
        labelLayer->setFont(font2);
        labelLayer->setAlignment(Qt::AlignCenter);
        restartButton = new QPushButton(Widget);
        restartButton->setObjectName(QString::fromUtf8("restartButton"));
        restartButton->setGeometry(QRect(500, 20, 21, 20));
        restartButton->setFlat(true);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        timeBar->setFormat(QString());
        labelTime->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\346\227\266\351\227\264</p></body></html>", nullptr));
        numTime->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">.</p></body></html>", nullptr));
        labelScore->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\345\276\227\345\210\206</p></body></html>", nullptr));
        numScore->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">0</p></body></html>", nullptr));
        labelName->setText(QCoreApplication::translate("Widget", "<html><head/><body><p>\347\216\251\345\256\2661</p></body></html>", nullptr));
        labelProp1->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\347\273\255\346\227\266</p></body></html>", nullptr));
        labelProp2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\351\207\215\347\273\204</p></body></html>", nullptr));
        labelProp3->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\346\217\220\347\244\272</p></body></html>", nullptr));
        labelProp4->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\345\206\273\347\273\223</p></body></html>", nullptr));
        labelName2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p>\347\216\251\345\256\2662</p></body></html>", nullptr));
        labelScore2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\345\276\227\345\210\206</p></body></html>", nullptr));
        numScore2->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">.</p></body></html>", nullptr));
        labelProp5->setText(QCoreApplication::translate("Widget", "<html><head/><body><p align=\"center\">\347\234\251\346\231\225</p></body></html>", nullptr));
        pauseButton->setText(QString());
        returnButton->setText(QString());
        labelLayer->setText(QCoreApplication::translate("Widget", "<html><head/><body><p>\347\254\2541\345\261\202</p></body></html>", nullptr));
        restartButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
