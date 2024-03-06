#pragma once
#ifndef SUBMENU_H
#define SUBMENU_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include "widget.h"

namespace Ui
{
class SubMenu;
}

// 这是单人模式下会调用的副菜单，用来进行buff和debuff的选择
class SubMenu : public QWidget
{
    Q_OBJECT

public:
    explicit SubMenu(QWidget* parent = nullptr);
    ~SubMenu();

private:
    Ui::SubMenu* ui; // ui
    QTimer* timer; // 计时器
    int boxLength; // “刻度”
    int gap; // buff选项框之间的间隔
    bool* challenge; // debuff（挑战）的选择
    bool* help; // buff（援助）的选择
    int curHelp, curChallenge; // 记录当前已选的buff和debuff

    // 以下是用来初始化游戏界面的参数
    int totalTime; // 时间上限
    int width; // 方块群的宽度
    int length; // 方块群的长度
    int numBox; // 方块的种类数
    int timeProp; // 掉落道具的时间间隔

    void setLabel(); // 设置label
    void connectButton(); // 将按钮点击信号与槽函数绑定
    void setData(); // 对初始参数的调整
    void setTypeHelp(Widget* w); // 向游戏界面传达buff和debuff种类
    void drawLabel(QPainter* p); // 画出选项框

protected:
    void paintEvent(QPaintEvent* event) override; // 重载paintEvent

private slots:
    void begin(); // 开始游戏
    void setChallege(int num); // debuff的选择
    void setHelp(int num); // buff的选择

};

#endif //  SUBMENU_H
