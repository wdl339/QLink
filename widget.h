#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QPixmap>
#include <QScreen>
#include <QMessageBox>
#include <QMediaPlayer>
#include <iostream>
#include <fstream>
#define UIRAD 20
#define BOXRAD 15
#define BUTTONRAD 10

QT_BEGIN_NAMESPACE
namespace Ui
{
class Widget;
}
QT_END_NAMESPACE

//游戏界面
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr, int mode = 1, int totalTime = 180000,
           int width = 9, int length = 12, int numBox = 14, int timeProp = 10000);
    ~Widget();
    int mode; // 模式（1代表单人，2代表双人，3代表“深渊”模式）
    int restTime, totalTime; // 剩余时间和时间上限
    int boxLength; // box（正方形）的边长
    int width, length; // 地图的宽度，长度(按box个数计)
    int numBox; // box种类数
    int roadWidth; // 边缘通道宽度(按box个数计)
    int numProp; // 地图上现有的道具数
    int timeProp; // 掉落新道具的时间间隔
    int timerHint, timerFlash; // hint和flash道具的计时器
    bool hinting, changeHint; // hint是否生效中，是否需要改变提示的方块
    bool flashing; // flash是否生效中
    bool pausing; // 是否在暂停中
    bool changeFlag; // 模式是否发生改变（例如，在双人模式中暂停，载入单人模式的存档）
    bool winning; // 是否胜利
    int layer; // “深渊”模式的层数
    int typeChallege, typeHelp; // debuff和buff的种类
    QPoint hint1, hint2; // hint的两个方块的坐标
    QPixmap background; // 背景图片

    int** map; // box地图
    int** mapProp; // 道具地图
    QPixmap** icons; // 地图图片显示
    bool* achieve; // 成就
    QMediaPlayer* play; // 音效播放器

    void newGame(); // 开启新游戏
    void suitForScreen(); // 调整窗口大小
    void createTimer(); // 创造计时器
    void createMap(); // 创造地图
    void createWidget(); // 初始化窗口

    void setForTime(); // 设置倒计时
    void setLabelStyle(); // 设置字体格式
    void drawMap(QPainter* painter); // 绘制地图
    void drawChoose(QPainter* painter); // 绘制玩家的连接折线与激活方块
    void drawSide(QPainter* painter); // 绘制旁边的得分框等

    void endGame(); // 结束游戏
    void allTimerStop(); // 所有计时器停止
    int whoWin(); // 判断谁赢了
    void endResult(int result); // 游戏结束时根据玩家选择进行操作

    QPoint* judgeLink(int x1, int x2, int y1, int y2); // 判断map[x1][y1]和map[x2][y2]能否消除
    QPoint* turnZero(int x1, int x2, int y1, int y2); // 一条线能连接的情况
    QPoint* turnOnce(int x1, int x2, int y1, int y2); // 折一次能连接的情况
    QPoint* turnTwice(int x1, int x2, int y1, int y2); // 折两次能连接的情况
    bool noBoxOnXLine(int x1, int x2, int y); // 判断从map[x1][y]到map[x2][y]间是否没有box
    bool noBoxOnYLine(int y1, int y2, int x); // 判断从map[x][y1]到map[x][y2]间是否没有box
    bool canMoveTo(int x, int y); // 判断玩家能否移动到map[x][y]处

    void deleteBox(int x1, int x2, int y1, int y2); // 删除map[x1][y1]与map[x1][y1]的方块
    void deleteProp(int x, int y); // 删除mapProp[x][y]处的方块

    void addOneSec(int num); // +1s道具
    void shuffle(); // shuffle道具
    void hint(int num); // hint道具
    int moveToNear(int x, int y); // flash移动到map[x][y]处的方块附近
    void freeze(int num); // freeze道具
    void dizzy(int num); // dizzy道具
    void faceChallege(int x, int y); // 对debuff和buff的处理

    void doIfModeChange(); // 应对可能出现的模式改变
    void readFile(); // 载入游戏
    void saveFile(); // 保存游戏
    void readAchieve(); // 导入已有成就
    void getAchieve(); // 游戏胜利时判断是否取得成就
    void saveAchieve(); // 导出已完成成就

private:
    Ui::Widget* ui; // ui
    QTimer* timer, * timerTime, * timerDrop; // 总计时器，倒计时计时器，掉落道具的计时器

protected:
    void paintEvent(QPaintEvent* event) override; // 重载函数
    void keyPressEvent(QKeyEvent* event) override;
    void timerEvent(QTimerEvent* event)override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void showTime(); // 展示时间
    void showScore(); // 展示分数
    void dropProp(); // 掉落道具
    void hint(); // 提示
    void pause(); // 暂停
    void judgeDead(); // 判断是否无解
    void returnToMenu(); // 返回主菜单
    void restart(); // 重新开始

};
#endif //  WIDGET_H
