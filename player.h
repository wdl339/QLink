#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "widget.h"
using namespace std;

// 玩家类
class Player: public QObject
{

    Q_OBJECT
public:
    int number; // 玩家编号,1代表玩家1，2为玩家2
    int x, y; // 玩家当前的坐标(x,y)
    int boxOneX, boxOneY; // 激活方块在map中的xy坐标,对应map[boxOneX][boxOneY]
    int boxTwoX, boxTwoY; // 配对的方块在map中的xy坐标
    int pace; // 玩家的步伐（走一步前进多少）
    int roleLength; // 玩家的尺寸
    bool useAddSec; // 玩家是否使用过续时道具
    bool choosing, linking; // 玩家是否已激活一个方块，是否已经可以连接
    bool freezing, dizzying; // 玩家是否被freeze和dizzy道具作用
    int timerFreeze, timerDizzy; // freeze和dizzy道具的计时器
    int totalLink; // 玩家总共已消除多少对方块（即得分）
    Widget* w; // 当前地图
    QPoint* linkPoint; // 绘制连接折线的点组
    int* prop; // 玩家当前拥有的每种道具的数量
    int timerLink; // 消除动画效果的计时器
    QRect rect1, rect2; // 消除动画的矩形

    Player();
    ~Player();

    void choose(int xBox, int yBox, int num); // 激活方块
    void choose1or2(int which, int xBox, int yBox, int num); // 按此时是否已激活方块，分类讨论，对boxOneX,boxOneY等赋值
    void collectProp(int x, int y); // 收集道具
    void judge(); // 判断是否能消除
    int numProp(); // 统计当前拥有道具总数

    void drawLink(QPainter* painter); // 画出连接折线
    void drawChoose(QPainter* painter); // 画出激活效果
    void drawRole(QPainter* painter); // 画出角色
    void drawEffect(QPainter* painter, const QPainterPath* path); // 根据当前角色状态，填充角色得分框
    void drawOwnProp1(QPainter* painter, int x0); // 画出单人模式下玩家的道具收集情况
    void drawOwnProp2(QPainter* painter, int x0); // 画出双人模式下玩家的道具收集情况

    void move(int num); // 玩家移动
    void keyMove(const QKeyEvent* event); // 玩家如何移动

    void timerEvent(QTimerEvent* event)override; // 重载函数
};


#endif //  PLAYER_H
