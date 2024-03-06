#include "player.h"

//
// Player类的构造函数
//
Player::Player()
{
    // 一些初始化
    number = 0;
    x = y = 0;
    boxOneX = boxOneY = 0;
    boxTwoX = boxTwoY = 0;
    pace = roleLength = 0;
    choosing = linking = freezing = dizzying = useAddSec = false;
    timerFreeze = timerDizzy = 0;
    totalLink = 0;
    timerLink = 0;
    rect1 = rect2 = QRect(0, 0, 0, 0);

    w = nullptr;
    linkPoint = nullptr;

    prop = new int[5];
    for (int i = 0; i < 5; i++)
        prop[i] = 0;
}

//
// Player类的析构函数
//
Player::~Player()
{
    delete w;
    delete prop;
    delete linkPoint;
}

//
// 重载timerEvent函数，以计时器事件event为参数，在消除计时器结束时，将linking转为false，即不处于消除状态
//
void Player::timerEvent(QTimerEvent* event)
{
    if (linking && event->timerId() == timerLink)
        linking = false;
}

//
// move接受方向值direct为参数，无返回值
// 上、下、左、右分别对应direct等于1，2，3，4
//
void Player::move(int direct)
{
    // 给常用值取简称
    int boxLength = w->boxLength, roadWidth = w->roadWidth;
    int width = w->width, length = w->length;
    // 对不同方向移动的处理
    switch(direct) {
        case(1):
            if (y - pace <= 0)
                // 走出一步碰到墙壁（y = 0处），则紧贴墙壁
                y = 0;
            // 否则可以先向上走一步
            else y -= pace;
            break;
        case(2):
            if (y + pace >= (roadWidth * 2 + width) * boxLength)
                // 走出一步碰到墙壁（y = (roadWidth * 2 + width) * boxLength处），则紧贴墙壁
                y = (roadWidth * 2 + width) * boxLength;
            // 否则可以先向下走一步，以下类似
            else y += pace;
            break;
        case(3):
            if (x - pace <= 0)
                x = 0;
            else x -= pace;
            break;
        case(4):
            if (x + pace >= (roadWidth * 2 + length) * boxLength)
                x = (roadWidth * 2 + length) * boxLength;
            else x += pace;
            break;
    }

    // xBox,yBox代表了当前坐标所处区域对应map[xBox][yBox]位置
    int xBox = y / boxLength;
    int yBox = x / boxLength;

    // 首先排除在边缘通路的情况，然后检测map[xBox][yBox]位置是否有方块（值在0至numBox内即为有方块）
    if (xBox >= roadWidth && xBox <= roadWidth + width - 1
            && yBox >= roadWidth && yBox <= roadWidth + length - 1
            && w->map[xBox][yBox] > 0 && w->map[xBox][yBox] <= w->numBox)
        // 有方块，则选中方块
        choose(xBox, yBox, direct);

    if (xBox <= roadWidth * 2 + width - 1 && yBox <= roadWidth * 2 + length - 1
            && w->mapProp[xBox][yBox] != 0)
        // 有道具，则收集道具
        collectProp(xBox, yBox);
}

//
// choose接受xBox,yBox和方向值num为参数
// xBox,yBox代表了当前坐标所处区域对应map[xBox][yBox]位置
// 这个函数用来判断当前激活的是第一个方块还是第二个，并执行相应操作
//
void Player::choose(int xBox, int yBox, int num)
{
    // choosing为0代表当前没有已激活的方块
    if (!choosing) {
        // 选中第一个方块
        choose1or2(1, xBox, yBox, num);
        choosing = 1;
    } else {
        // 选中第二个
        choose1or2(2, xBox, yBox, num);
        choosing = 0;
        // 判断能否连接
        judge();
        // 记录两个激活方块的数据清零
        boxOneX = boxOneY = boxTwoX = boxTwoY = 0;
    }
}

//
// choose1or2接受which（代表当前激活的是第一个方块还是第二个）,xBox,yBox和方向值num为参数
//
void Player::choose1or2(int which, int xBox, int yBox, int num)
{
    // 如果当前激活的是第一个方块，那么xBox,yBox将分别被赋值给boxOneX,boxOneY
    // 否则被赋值给boxTwoX,boxTwoY
    int* px = (which == 1) ? &boxOneX : &boxTwoX;
    int* py = (which == 1) ? &boxOneY : &boxTwoY;
    *px = xBox;
    *py = yBox;
    switch(num) {
        case(1):
            // 已经向上走碰到方块，于是向下后退一步，避免与方块重合，以下均类似
            y += pace;
            break;
        case(2):
            y -= pace;
            break;
        case(3):
            x += pace;
            break;
        case(4):
            x -= pace;
            break;
    }
}

//
// collectProp接受x,y为参数，收集mapProp[x][y]位置的道具
//
void Player::collectProp(int x, int y)
{
    // 相应道具+1
    prop[w->mapProp[x][y] - 1] += 1;
    w->deleteProp(x, y);
}

//
// judge判断是否能消除，如果能消除，做出消除准备
//
void Player::judge()
{
    // 假如能消除，返回的点数组不含(0,0)
    QPoint* p = w->judgeLink(boxOneX, boxTwoX, boxOneY, boxTwoY);
    if (p[0] != QPoint(0, 0)) {
        // 传递点数组，为后续画连接的折线做准备
        linkPoint = p;
        w->deleteBox(boxOneX, boxTwoX, boxOneY, boxTwoY);
        totalLink += 1;
        linking = true;
        timerLink = startTimer(300);
        // 定位消除动画中矩形的位置
        rect1 = QRect(linkPoint[0].x(), linkPoint[0].y(), 0, 0);
        rect2 = QRect(linkPoint[3].x(), linkPoint[3].y(), 0, 0);
    }
}

//
// numProp统计并返回当前玩家拥有的道具总数
//
int Player::numProp()
{
    int num = 0;
    for (int i = 0; i < 5; i++)
        num += prop[i];
    return num;
}

//
// drawLink接受绘图类painter为参数，假如已经能消除方块，在painter上画出连接在一起的折线与消除动画
//
void Player::drawLink(QPainter* painter)
{
    if (linking) {
        int boxLength = w->boxLength;
        painter->setPen(QPen(Qt::black, 6, Qt::SolidLine));
        // 根据点数组画出折线，顺次连接四个点即可
        // 对各种连接情况均适用（若连接的两点相同，实际上就是画了个点）
        painter->drawLine(linkPoint[0], linkPoint[1]);
        painter->drawLine(linkPoint[1], linkPoint[2]);
        painter->drawLine(linkPoint[2], linkPoint[3]);
        // 画出消除动画
        QPainterPath path;
        path.addRect(linkPoint[0].x() - boxLength / 8, linkPoint[0].y() - boxLength / 8, boxLength / 4, boxLength / 4);
        path.addRect(linkPoint[3].x() - boxLength / 8, linkPoint[3].y() - boxLength / 8, boxLength / 4, boxLength / 4);
        painter->fillPath(path, Qt::black);
        painter->drawPath(path);
        painter->drawRect(rect1);
        painter->drawRect(rect2);
        // 矩形不断扩大
        rect1 = QRect(rect1.x() - boxLength / 10, rect1.y() - boxLength / 10, rect1.width() + boxLength / 5, rect1.height() + boxLength / 5);
        rect2 = QRect(rect2.x() - boxLength / 10, rect2.y() - boxLength / 10, rect2.width() + boxLength / 5, rect2.height() + boxLength / 5);
    }
}

//
// drawChoose接受绘图类painter为参数，假如已经激活一个方块，在painter上表示出方块被激活
//
void Player::drawChoose(QPainter* painter)
{
    if (choosing && w->map[boxOneX][boxOneY] != 0) {
        int boxLength = w->boxLength;
        // 假如是玩家1，激活方块方框为红色，玩家2为绿色
        if (number == 1)
            painter->setPen(QPen(Qt::red, 8, Qt::SolidLine));
        else painter->setPen(QPen(Qt::green, 8, Qt::SolidLine));
        painter->drawRoundedRect(boxOneY * boxLength, boxOneX * boxLength, boxLength, boxLength, BOXRAD, BOXRAD);
    }
}

//
// drawRole接受绘图类painter为参数，在painter上画出角色
//
void Player::drawRole(QPainter* painter)
{
    QPixmap role;
    role.load(QString::asprintf(":/images/role%d.png", number));
    painter->drawPixmap(x, y, roleLength, roleLength, role);
}

//
// drawEffect接受绘图类painter和路径类path为参数，根据当前角色状态，对玩家得分框进行颜色填充
//
void Player::drawEffect(QPainter* painter, const QPainterPath* path)
{
    // 正常情况下得分框填充黄色
    // 当玩家被冻结，冻结期间得分框显示为蓝色。dizzy期间为紫色，flash期间为灰色
    if (freezing)
        painter->fillPath(*path, Qt::blue);
    else if (dizzying)
        painter->fillPath(*path, QColor("#EE82EE"));
    else if (w->flashing)
        painter->fillPath(*path, QColor("#C0C0C0"));
    else painter->fillPath(*path, Qt::yellow);
    painter->drawPath(*path);
}

//
// drawOwnProp1接受绘图类painter和x坐标x0为参数，在单人模式下，画出玩家的道具收集情况
// x0相当于一个基准，参考该基准定位道具图片的位置
//
void Player::drawOwnProp1(QPainter* painter, int x0)
{
    int boxLength = w->boxLength;
    for (int i = 1; i <= 4; i++) {
        // 当玩家收集有该种类道具时，道具图片被方框围住，并显示为绿色
        if (prop[i - 1]) {
            QPainterPath pathProp;
            pathProp.addRoundedRect(x0 + boxLength / 2, boxLength * (2 + i), boxLength, boxLength, UIRAD, UIRAD);
            painter->fillPath(pathProp, Qt::green);
            painter->drawPath(pathProp);
        }
        // 画出道具图片
        QPixmap pixProp;
        pixProp.load(QString::asprintf(":/images/prop%d.png", i));
        painter->drawPixmap(x0 + boxLength / 2, boxLength * (2 + i), boxLength, boxLength, pixProp);
    }
}

//
// drawOwnProp2接受绘图类painter和x坐标x0为参数，在双人模式下，画出玩家的道具收集情况
// 与上面drawOwnProp2函数类似，但是道具数量变化为5，道具图片位置发生改动
//
void Player::drawOwnProp2(QPainter* painter, int x0)
{
    int boxLength = w->boxLength;
    for (int i = 1; i <= 5; i++) {
        if (prop[i - 1]) {
            QPainterPath pathProp;
            if (number == 1)
                pathProp.addRoundedRect(x0, boxLength * (2 + i), boxLength, boxLength, UIRAD, UIRAD);
            else
                pathProp.addRoundedRect(x0 + boxLength * 2, boxLength * (2 + i), boxLength, boxLength, UIRAD, UIRAD);
            painter->fillPath(pathProp, Qt::green);
            painter->drawPath(pathProp);
        }
        QPixmap pixProp;
        pixProp.load(QString::asprintf(":/images/mode2prop%d.png", i));
        if (number == 1)
            painter->drawPixmap(x0, boxLength * (2 + i), boxLength, boxLength, pixProp);
        else
            painter->drawPixmap(x0 + boxLength * 2, boxLength * (2 + i), boxLength, boxLength, pixProp);
    }
}

//
// keyMove接受鼠标点击事件event为参数，使角色做出相应的移动动作
//
void Player::keyMove(const QKeyEvent* event)
{
    if (!freezing) {
        int* p = new int [4];
        if (!dizzying) {
            for(int i = 0; i < 4; i++)
                p[i] = i + 1;
        } else {
            p[0] = 2;
            p[1] = 1;
            p[2] = 4;
            p[3] = 3;
        }
        // 玩家1使用wsad键为上下左右，玩家2使用↑↓←→键
        if (number == 1) {
            switch(event->key()) {
                case Qt::Key_W:
                    move(p[0]);
                    break;
                case Qt::Key_S:
                    move(p[1]);
                    break;
                case Qt::Key_A:
                    move(p[2]);
                    break;
                case Qt::Key_D:
                    move(p[3]);
                    break;
            }
        } else {
            switch(event->key()) {
                case Qt::Key_Up:
                    move(p[0]);
                    break;
                case Qt::Key_Down:
                    move(p[1]);
                    break;
                case Qt::Key_Left:
                    move(p[2]);
                    break;
                case Qt::Key_Right:
                    move(p[3]);
                    break;
            }
        }
    }
}
