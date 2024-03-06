#pragma once
#ifndef ACHIEVE_H
#define ACHIEVE_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QPixmap>

namespace Ui
{
class Achieve;
}

// 这是成就界面
class Achieve : public QWidget
{
    Q_OBJECT

public:
    explicit Achieve(QWidget* parent = nullptr);
    ~Achieve();

private:
    Ui::Achieve* ui; // ui
    QTimer* timer; // 计时器
    int boxLength; // 相当于一个刻度是多长
    bool* achieve; // 当前玩家各项成就的点亮情况
    int numAchieve; // 当前已点亮的成就总数

    void setLabel(); // 设置label

protected:
    void paintEvent(QPaintEvent* event) override;// 重载paintEvent函数

private slots:
    void returnToMenu(); // 返回主菜单

};

#endif //  ACHIEVE_H
