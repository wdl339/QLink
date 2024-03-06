#pragma once
#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QPixmap>

namespace Ui
{
class Menu;
}

// 这是主菜单
class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget* parent = nullptr);
    ~Menu();

private:
    Ui::Menu* ui; // ui
    int boxLength; // 一个刻度有多长
    int x, y; // 当前窗口的长宽数据
    QTimer* timer; // 计时器

    int readMode(); // 从存档中读取存档的模式
    void ifSizeChange(); // 窗口大小变化时的操作
    void setLabel(); // 设置label

protected:
    void paintEvent(QPaintEvent* event) override; // 重载paintEvent函数

private slots: // 槽函数
    void mode1(); // 启动单人模式
    void mode2(); // 双人
    void open(); // 读取存档
    void quit(); // 退出游戏
    void mode3(); // “深渊”模式（自创的，非作业要求）
    void achieve(); // 成就界面（自创）
    void rule(); // 规则介绍（自创）

};

#endif //  MENU_H
