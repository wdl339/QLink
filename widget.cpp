#include "widget.h"
#include "ui_widget.h"
#include "player.h"
#include "menu.h"
using namespace std;
extern Player* player1, * player2;

// 字体格式
QString strStyle = "QLabel{"
                   "font-family: \"Microsoft YaHei\";"
                   "font-size: 24px;"
                   "color: black;"
                   "}";

//
// Widget的构造函数，继承自QWidget类，mod,total,wid,len,num,timeP,ui界面为参数
// 变量分别代表：模式，时间上限，地图宽，长，方块种类数，道具掉落时间间隔
//
Widget::Widget(QWidget* parent, int mod, int total, int wid, int len, int num, int timeP)
    : QWidget(parent)
    , mode(mod), totalTime(total), width(wid), length(len), numBox(num), timeProp(timeP), ui(new Ui::Widget)
{
    ui->setupUi(this);

    newGame();
}

//
// Widget的析构函数
//
Widget::~Widget()
{
    delete ui;

    for (int i = 0; i < width + roadWidth * 2; i++) {
        delete map[i];
        delete mapProp[i];
        delete icons[i];
    }

    delete timer;
    delete timerTime;
    delete timerDrop;
    delete achieve;
    delete play;
}

//
// newGame开启新游戏
//
void Widget::newGame()
{
    // 初始化（各参数含义见.h文件）
    boxLength = 80;
    roadWidth = 2;
    restTime = totalTime;
    numProp = timerHint = timerFlash = layer = typeChallege = typeHelp = 0;
    hinting = changeHint = flashing = pausing = changeFlag = winning = false;
    hint1 = hint2 = QPoint(0, 0);
    player1->pace = boxLength / 2;
    player1->roleLength = boxLength / 2;
    suitForScreen();
    player1->number = 1;
    // 不知道为什么播放器第一次播放时会有延迟，所以在初始化的时候就先播放一次
    play = new QMediaPlayer;
    play->setMedia(QUrl::fromEncoded("qrc:/music/link.mp3"));
    play->setVolume(0);
    play->play();

    if (mode == 2) {
        player2->pace = player1->pace;
        player2->roleLength = player1->roleLength;
        player2->number = 2;
        // 双人模式，玩家1在左，玩家2在右
        player2->x = boxLength * (length + roadWidth * 2);
        player1->y = (rand() % (width + roadWidth * 2) * 2) * (boxLength / 2);
        player2->y = (rand() % (width + roadWidth * 2) * 2) * (boxLength / 2);
    } else {
        // 单人模式，随机玩家位置
        srand(time(NULL));
        player1->x = (rand() % (length + roadWidth * 2) * 2) * (boxLength / 2);
    }

    createTimer();
    createMap();
    createWidget();
    readAchieve();
}

//
// createTimer创建与设置计时器
//
void Widget::createTimer()
{
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timerTime = new QTimer(this);
    timerTime->setTimerType(Qt::PreciseTimer);
    timerDrop = new QTimer(this);
    timerDrop->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    connect(timerTime, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(timerTime, SIGNAL(timeout()), this, SLOT(judgeDead()));
    connect(timer, SIGNAL(timeout()), this, SLOT(showScore()));
    connect(timerDrop, SIGNAL(timeout()), this, SLOT(dropProp()));
    connect(timer, SIGNAL(timeout()), this, SLOT(hint()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->returnButton, SIGNAL(clicked()), this, SLOT(returnToMenu()));
    connect(ui->restartButton, SIGNAL(clicked()), this, SLOT(restart()));
    timer->start(50);
    timerTime->start(1000);
    timerDrop->start(timeProp);
}

//
// createMap创造地图
//
void Widget::createMap()
{
    // map方块地图，值为0表示没有方块
    map = new int* [width + roadWidth * 2];
    for (int i = 0; i < width + roadWidth * 2; i++) {
        map[i] = new int[length + roadWidth * 2];
        for (int j = 0; j < length + roadWidth * 2; j++)
            map[i][j] = 0;
    }

    // 随机生成地图，先顺序生成每个方块种类，存入preMap，再打乱
    srand(time(NULL));
    int* preMap = new int[width * length];
    int i;
    for (i = 0; i < width * length; i += 2)
        preMap[i] = preMap[i + 1] = rand() % numBox;
    random_shuffle(preMap, preMap + width * length);

    //将preMap中数据存入map
    for (i = 0; i < width * length; i++) {
        int x = i / length + roadWidth;
        int y = i % length + roadWidth;
        map[x][y] = preMap[i] + 1;
    }

    // mapProp方块地图
    mapProp = new int* [width + roadWidth * 2];
    for (int i = 0; i < width + roadWidth * 2; i++) {
        mapProp[i] = new int[length + roadWidth * 2];
        for (int j = 0; j < length + roadWidth * 2; j++)
            mapProp[i][j] = 0;
    }

    // 图片的显示
    icons = new QPixmap* [width + roadWidth * 2];
    for (int i = 0; i < width + roadWidth * 2; i++) {
        icons[i] = new QPixmap[length + roadWidth * 2];
        for (int j = 0; j < length + roadWidth * 2; j++)
            if (map[i][j] > 0 && map[i][j] <= numBox)
                icons[i][j].load(QString::asprintf(":/images/element%d.png", map[i][j]));
    }
}

//
// suitForScreen调整窗口大小
//
void Widget::suitForScreen()
{
    QScreen* screen = QApplication::primaryScreen();
    QRect rect = screen->geometry();
    // 使窗口大小不超出屏幕大小
    if(boxLength * (length + roadWidth * 2 + 6) + player1->roleLength > rect.width()) {
        boxLength = rect.width() / (length + roadWidth * 2 + 7);
        // 因为步伐pace不是1，boxLength最好能整除4，不然可能穿模
        while(boxLength % 4 != 0) {
            boxLength -= 1;
        }
        player1->pace = boxLength / 2;
        player1->roleLength = boxLength / 2;
    }
    if(boxLength * (width + roadWidth * 2 + 2) + player1->roleLength > rect.height()) {
        boxLength = rect.height() / (width + roadWidth * 2 + 3);
        while(boxLength % 4 != 0) {
            boxLength -= 1;
        }
        player1->pace = boxLength / 2;
        player1->roleLength = boxLength / 2;
    }
    setFixedSize(boxLength * (length + roadWidth * 2 + 6) + player1->roleLength, boxLength * (width + roadWidth * 2 + 2) + player1->roleLength);
    this->move((rect.width() - this->geometry().width()) / 2, (rect.height() - this->geometry().height()) / 2);
}

//
// createWidget初始化窗口，包括label，按钮，进度条的设置
//
void Widget::createWidget()
{
    setWindowTitle(tr("Star Rail Link"));
    setFixedSize(boxLength * (length + roadWidth * 2 + 6) + player1->roleLength, boxLength * (width + roadWidth * 2 + 2) + player1->roleLength);
    int x0 = boxLength * (length + roadWidth * 2 + 2) + player1->roleLength;
    ui->labelName->setGeometry(x0, boxLength, boxLength * 3, boxLength / 2);
    ui->labelScore->setGeometry(x0, boxLength * 3 / 2, boxLength, boxLength);
    ui->labelTime->setGeometry(x0, boxLength * 5 / 2, boxLength, boxLength);
    ui->timeBar->setGeometry(boxLength / 2 + x0, boxLength * 7 / 2, boxLength * 2, boxLength / 4);
    ui->numScore->setGeometry(boxLength + x0, boxLength * 3 / 2, boxLength * 2, boxLength);
    ui->numTime->setGeometry(boxLength + x0, boxLength * 5 / 2, boxLength * 2, boxLength);
    ui->pauseButton->setGeometry(boxLength * 2 + x0, boxLength / 4, boxLength / 2, boxLength / 2);
    ui->returnButton->setGeometry(boxLength * 3 / 2 + x0, boxLength / 4, boxLength / 2, boxLength / 2);
    ui->restartButton->setGeometry(boxLength + x0, boxLength / 4, boxLength / 2, boxLength / 2);
    setForTime();
    ui->pauseButton->setIcon(QPixmap(tr(":/images/pause.png")));
    // 取消按钮的焦点
    ui->pauseButton->setFocusPolicy(Qt::NoFocus);
    ui->returnButton->setIcon(QPixmap(tr(":/images/return.png")));
    ui->returnButton->setFocusPolicy(Qt::NoFocus);
    ui->restartButton->setIcon(QPixmap(tr(":/images/restart.png")));
    ui->restartButton->setFocusPolicy(Qt::NoFocus);
    ui->labelLayer->setVisible(false);
    setLabelStyle();
    // 模式不同，label的显示和位置不同
    if (mode != 2) {
        ui->labelName2->setVisible(false);
        ui->labelScore2->setVisible(false);
        ui->numScore2->setVisible(false);
        ui->labelProp5->setVisible(false);
        ui->labelProp4->setText("瞬移");
        ui->labelProp1->setGeometry(boxLength * 3 / 2 + x0, boxLength * 4, boxLength, boxLength);
        ui->labelProp2->setGeometry(boxLength * 3 / 2 + x0, boxLength * 5, boxLength, boxLength);
        ui->labelProp3->setGeometry(boxLength * 3 / 2 + x0, boxLength * 6, boxLength, boxLength);
        ui->labelProp4->setGeometry(boxLength * 3 / 2 + x0, boxLength * 7, boxLength, boxLength);
    } else {
        ui->labelProp1->setGeometry(boxLength + x0, boxLength * 4, boxLength, boxLength);
        ui->labelProp2->setGeometry(boxLength + x0, boxLength * 5, boxLength, boxLength);
        ui->labelProp3->setGeometry(boxLength + x0, boxLength * 6, boxLength, boxLength);
        ui->labelProp4->setGeometry(boxLength + x0, boxLength * 7, boxLength, boxLength);
        ui->labelProp5->setGeometry(boxLength + x0, boxLength * 8, boxLength, boxLength);
        ui->labelName2->setGeometry(x0, boxLength * 9, boxLength * 3, boxLength / 2);
        ui->labelScore2->setGeometry(x0, boxLength * 19 / 2, boxLength, boxLength);
        ui->numScore2->setGeometry(boxLength + x0, boxLength * 19 / 2, boxLength * 2, boxLength);
    }
    background.load(":/images/pure.png");
}

//
// setForTime设置进度条与倒计时
//
void Widget::setForTime()
{
    ui->timeBar->setMaximum(totalTime);
    ui->timeBar->setValue(totalTime);
    ui->timeBar->setStyleSheet("QProgressBar::chunk{background:green}");
    ui->numTime->setText(QString::number(totalTime / 1000));
}

//
// setLabelStyle设置字体格式
//
void Widget::setLabelStyle()
{
    ui->labelName->setStyleSheet(strStyle);
    ui->labelTime->setStyleSheet(strStyle);
    ui->labelScore->setStyleSheet(strStyle);
    ui->numTime->setStyleSheet(strStyle);
    ui->numScore->setStyleSheet(strStyle);
    ui->labelProp1->setStyleSheet(strStyle);
    ui->labelProp2->setStyleSheet(strStyle);
    ui->labelProp3->setStyleSheet(strStyle);
    ui->labelProp4->setStyleSheet(strStyle);
    ui->labelName2->setStyleSheet(strStyle);
    ui->labelScore2->setStyleSheet(strStyle);
    ui->numScore2->setStyleSheet(strStyle);
    ui->labelProp5->setStyleSheet(strStyle);
    ui->labelLayer->setStyleSheet(strStyle);
}

//
// showTime显示时间
//
void Widget::showTime()
{
    // 调整剩余时间，倒计时与进度条
    restTime -= 1000;
    ui->numTime->setText(QString::number(restTime / 1000));
    ui->timeBar->setValue(restTime);
    // 第6项debuff
    if (restTime % 10000 == 0 && typeChallege == 3)
        shuffle();
    // 时间到结束游戏，时间较少时进度条变色
    if (restTime != 0) {
        if (ui->timeBar->value() <= totalTime / 5)
            ui->timeBar->setStyleSheet("QProgressBar::chunk{background:red}");
        else
            ui->timeBar->setStyleSheet("QProgressBar::chunk{background:green}");
    } else endGame();
}

//
// showScore显示分数
//
void Widget::showScore()
{
    ui->numScore->setText(QString::number(player1->totalLink));
    if (mode == 2)
        ui->numScore2->setText(QString::number(player2->totalLink));

    // 第6项成就（因为这个函数调用频率高所以就加在这里了）
    if ((player1->freezing && player2->freezing) || (player1->dizzying && player2->dizzying))
        achieve[5] = true;
}

//
// readAchieve从achieve.txt文件中导入已有成就
//
void Widget::readAchieve()
{
    achieve = new bool[8];
    for (int i = 0; i < 8; i++)
        achieve[i] = false;

    QFile file("achieve.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        for (int i = 0; i < 8; i++)
            in >> achieve[i];
    }
}

//
// endGame结束游戏,并弹出QMessageBox让玩家决定接下来的操作
//
void Widget::endGame()
{
    update();
    showScore();
    allTimerStop();
    QString s;
    if (restTime == 0) {
        if (mode != 2)
            s = QString::asprintf("当前得分：%d\n是否再来一局？", player1->totalLink);
        else {
            if (whoWin())
                s = QString::asprintf("当前得分：\n玩家1：%d\n玩家2：%d\n玩家%d赢了\n\n是否再来一局？",
                                      player1->totalLink, player2->totalLink, whoWin());
            else
                s = QString::asprintf("当前得分：\n玩家1：%d\n玩家2：%d\n打平了\n\n是否再来一局？",
                                      player1->totalLink, player2->totalLink);
        }
    } else {
        // 判断是完全消除还是无解
        winning = true;
        for (int i = 0; i < width + roadWidth * 2; i++)
            for (int j = 0; j < length + roadWidth * 2; j++)
                if (map[i][j] != 0)
                    winning = false;
        if (winning) {
            achieve[0] = true;
            getAchieve();
            if (mode != 2)
                s = QString::asprintf("恭喜你，全部消除！\n\n当前得分：%d\n剩余时间：%ds\n\n是否再来一局？"
                                      , player1->totalLink, restTime / 1000);
            else {
                if (whoWin())
                    s = QString::asprintf("恭喜你们，全部消除！\n\n当前得分：\n玩家1：%d\n玩家2：%d\n玩家%d赢了\n\n是否再来一局？",
                                          player1->totalLink, player2->totalLink, whoWin());
                else {
                    s = QString::asprintf("恭喜你们，全部消除！\n\n当前得分：\n玩家1：%d\n玩家2：%d\n打平了\n\n是否再来一局？",
                                          player1->totalLink, player2->totalLink);
                }
            }
        } else
            s = "无解了!\n是否再来一局？";
    }
    QMessageBox mess(QMessageBox::Question, "游戏结束", s, QMessageBox::Yes | QMessageBox::No);
    mess.setStyleSheet(
                    "QPushButton{font: bold 24px; min-width: 10em; min-height: 5em;}"
                    "QLabel{font: bold 24px;}");
    int result = mess.exec();
    endResult(result);
}

//
// allTimerStop让所有计时器停止
//
void Widget::allTimerStop()
{
    timer->stop();
    timerTime->stop();
    timerDrop->stop();
}

//
// whoWin判断并返回谁赢了（1即玩家1，2即玩家2，0为打平）
//
int Widget::whoWin()
{
    int winner = 0;
    if (player1->totalLink > player2->totalLink)
        winner = 1;
    if (player1->totalLink < player2->totalLink)
        winner = 2;
    return winner;
}

//
// getAchieve在游戏胜利时判断是否取得一些成就（详见achieve.ui）
//
void Widget::getAchieve()
{
    if (totalTime - restTime < 10000)
        achieve[1] = true;
    if ((player1->numProp() > 15 || player2->numProp() > 15) && restTime > totalTime / 2)
        achieve[2] = true;
    if (mode == 2 && restTime > totalTime / 2 && player1->useAddSec == false
            && player2->useAddSec == false)
        achieve[4] = true;
    if (mode == 3 && layer == 5)
        achieve[6] = true;
    if (mode == 3 && layer == 2 && player1->useAddSec == false)
        achieve[7] = true;
}

//
// endResult在游戏结束时根据玩家的选择进行操作，以玩家在QMessageBox选择的按键（yes or no）为参数
//
void Widget::endResult(int result)
{
    saveAchieve();
    // 选no返回主菜单
    if (result == QMessageBox::No) {
        Menu* m = new Menu;
        this->close();
        m->show();
    }
    if (result == QMessageBox::Yes) {
        player1 = new Player;
        player2 = new Player;
        // 单人或双人模式中，layer = 0，重开时总时间不变
        // “深渊”模式中，难度随所在层数layer增加。通关后选择再来一局，会进入下一层，而失败则会重新挑战这一层
        if(mode == 3 && winning) {
            srand(time(NULL));
            int debuff = rand() % 4;
            if(debuff == 0) {
                totalTime -= 30000;
            } else if(debuff == 1) {
                length += 2;
            } else if(debuff == 2) {
                width += 2;
            } else if(debuff == 3) {
                timeProp += 4000;
            }
            layer += 1;
        }
        Widget* w = new Widget(nullptr, mode, totalTime, width, length, numBox, timeProp);
        w->layer = layer;
        player1->w = w;
        player2->w = w;
        this->close();
        w->show();
    }
}

//
// saveAchieve导出已完成成就到achieve.txt
//
void Widget::saveAchieve()
{
    QFile file("achieve.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        for (int i = 0; i < 8; i++)
            out << achieve[i];
    }
}

//
// swap是一个平凡的交换函数，它以a,b的引用为参数
//
void swap(int& a, int& b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

//
// 重载paintEvent函数，接受绘图事件QPaintEvent类为参数，画出游戏窗口，每次update时调用
//
void Widget::paintEvent(QPaintEvent*)
{
    // 谨防神奇操作（例如，在双人模式中暂停，载入单人模式的存档）
    if(changeFlag)
        doIfModeChange();

    QPainter painter(this);
    QPainter* p = &painter;
    painter.setRenderHint(QPainter::Antialiasing);
    // 坐标轴平移，使地图左上角为坐标系原点，便于绘制地图
    painter.translate(boxLength, boxLength);
    p->setPen(QPen(Qt::blue, 4, Qt::SolidLine));

    drawMap(p);
    drawChoose(p);

    // 谨防玩家利用暂停时间“作弊”思考如何连接，暂停时会将地图遮住
    if (pausing)
        p->drawPixmap(-boxLength, -boxLength, this->geometry().width(), this->height(), background);

    drawSide(p);
}

//
// drawMap接受绘图类painter为参数，绘制地图
//
void Widget::drawMap(QPainter* painter)
{
    painter->drawPixmap(0, 0, boxLength * (length + roadWidth * 2) + player1->roleLength,
                        boxLength * (width + roadWidth * 2) + player1->roleLength, background);

    // 假如处于hint中，高亮方块
    if (hinting) {
        if (map[hint1.y() / boxLength][hint1.x() / boxLength] != 0
                && map[hint2.y() / boxLength][hint2.x() / boxLength] != 0) {
            QPainterPath path;
            path.addRoundedRect(hint1.x(), hint1.y(), boxLength, boxLength, BOXRAD, BOXRAD);
            path.addRoundedRect(hint2.x(), hint2.y(), boxLength, boxLength, BOXRAD, BOXRAD);
            painter->fillPath(path, Qt::black);
            painter->drawPath(path);
        } else
            // 如果hint中但是高亮方块已被消除，需要高亮下一对
            changeHint = true;
    }

    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++) {
            // 画出每个icon
            if (map[i][j] > 0 && map[i][j] <= numBox)
                painter->drawRoundedRect(boxLength * j, boxLength * i, boxLength, boxLength, BOXRAD, BOXRAD);
            painter->drawPixmap(boxLength * j, boxLength * i, boxLength, boxLength, icons[i][j]);
        }

    painter->setPen(QPen(Qt::black, 4, Qt::SolidLine));
    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++)
            if (mapProp[i][j] > 0 && mapProp[i][j] <= 5)
                painter->drawRoundedRect(boxLength * j, boxLength * i, boxLength, boxLength, BOXRAD, BOXRAD);
    painter->setPen(QPen(Qt::blue, 4, Qt::SolidLine));

    // 画角色
    player1->drawRole(painter);
    if (mode == 2)
        player2->drawRole(painter);
}

//
// drawChoose接受绘图类painter为参数，绘出玩家的连接折线与激活方块
//
void Widget::drawChoose(QPainter* painter)
{
    player1->drawLink(painter);
    player1->drawChoose(painter);
    if (mode == 2) {
        player2->drawLink(painter);
        player2->drawChoose(painter);
    }
}

//
// drawSide接受绘图类painter为参数，绘制旁边的得分框，倒计时框以及道具框
//
void Widget::drawSide(QPainter* painter)
{
    painter->setPen(QPen(Qt::blue, 4, Qt::SolidLine));
    int x0 = boxLength * (length + roadWidth * 2 + 1) + player1->roleLength;
    // 由于玩家得分框与其他框填充颜色可能不同（详见drawEffect），需要用不同路径画
    QPainterPath path, path1, path2;

    // 三个按钮
    path.addRoundedRect(boxLength * 2 + x0, - boxLength * 3 / 4, boxLength / 2, boxLength / 2, BUTTONRAD, BUTTONRAD);
    path.addRoundedRect(boxLength * 3 / 2 + x0, - boxLength * 3 / 4, boxLength / 2, boxLength / 2, BUTTONRAD, BUTTONRAD);
    path.addRoundedRect(boxLength + x0, - boxLength * 3 / 4, boxLength / 2, boxLength / 2, BUTTONRAD, BUTTONRAD);

    path1.addRoundedRect(x0, 0, boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);
    player1->drawEffect(painter, &path1);
    path.addRoundedRect(x0, boxLength * 3 / 2, boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);

    // “深渊”模式的层数框
    if (layer != 0) {
        ui->labelLayer->setVisible(true);
        ui->labelLayer->setGeometry(boxLength * 2 + x0, boxLength * 8, boxLength, boxLength);
        ui->labelLayer->setText(QString::asprintf("第%d层", layer));
        path.addRoundedRect(x0, boxLength * 7, boxLength * 3, boxLength, UIRAD, UIRAD);
    }

    // 不同模式道具框大小不同
    if (mode != 2) {
        path.addRoundedRect(x0, boxLength * 3, boxLength * 3, boxLength * 4, UIRAD, UIRAD);
        painter->fillPath(path, Qt::yellow);
        painter->drawPath(path);
        player1->drawOwnProp1(painter, x0);
    } else {
        path.addRoundedRect(x0, boxLength * 3, boxLength * 3, boxLength * 5, UIRAD, UIRAD);
        painter->fillPath(path, Qt::yellow);
        painter->drawPath(path);

        path2.addRoundedRect(x0, boxLength * 8, boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);
        player2->drawEffect(painter, &path2);

        player1->drawOwnProp2(painter, x0);
        player2->drawOwnProp2(painter, x0);
    }
}

//
// deleteBox接受x1,x2,y1,y2为参数，删除map[x1][y1]与map[x1][y1]的方块
//
void Widget::deleteBox(int x1, int x2, int y1, int y2)
{
    // 假如有buff和debuff，做处理
    if (typeHelp != 0 || typeChallege != 0)
        faceChallege(x1, y1);
    // 清除两个方块（在存储中和在图片显示中）
    map[x1][y1] = 0;
    map[x2][y2] = 0;
    QPixmap clearPix = QPixmap();
    icons[x1][y1] = clearPix;
    icons[x2][y2] = clearPix;
    // 播放消除音
    play->setMedia(QUrl::fromEncoded("qrc:/music/link.mp3"));
    play->setVolume(50);
    play->play();
}

//
// deleteProp接受x,y为参数，删除mapProp[x][y]处的方块
//
void Widget::deleteProp(int x, int y)
{
    // 对于“任意门”成就的判断
    if (mode != 2 && flashing && mapProp[x][y] == 4)
        achieve[3] = true;
    // 清空该道具（在存储中和在图片显示中），全场道具总数减一
    mapProp[x][y] = 0;
    QPixmap clearPix = QPixmap();
    icons[x][y] = clearPix;
    numProp -= 1;
    // 播放消除音
    play->setMedia(QUrl::fromEncoded("qrc:/music/prop.mp3"));
    play->setVolume(50);
    play->play();
}

//
// 重载keyPressEvent函数，以按键事件event为参数，处理按键
//
void Widget::keyPressEvent(QKeyEvent* event)
{
    if (!pausing) {
        player1->keyMove(event);
        // 单人模式下，按1、2、3、4键，使用+1s,shuffle,hint,flash道具
        // 双人模式下，玩家1按1、2、3、4、5键，使用+1s,shuffle,hint,freeze,dizzy道具。玩家2则是按6、7、8、9、0键

        if (mode == 2) {
            player2->keyMove(event);
            if (event->key() == Qt::Key_4 && player1->prop[3]) {
                freeze(2);
            } else if (event->key() == Qt::Key_5 && player1->prop[4]) {
                dizzy(2);
            } else if (event->key() == Qt::Key_6 && player2->prop[0]) {
                addOneSec(2);
            } else if (event->key() == Qt::Key_7 && player2->prop[1]) {
                shuffle();
                player2->prop[1] -= 1;
            } else if (event->key() == Qt::Key_8 && player2->prop[2]) {
                hint(2);
            } else if (event->key() == Qt::Key_9 && player2->prop[3]) {
                freeze(1);
            } else if (event->key() == Qt::Key_0 && player2->prop[4]) {
                dizzy(1);
            }
        } else if (event->key() == Qt::Key_4 && player1->prop[3]) {
            timerFlash = startTimer(5000);
            flashing = true;
            player1->prop[3] -= 1;
        }

        if (event->key() == Qt::Key_1 && player1->prop[0]) {
            addOneSec(1);
        } else if (event->key() == Qt::Key_2 && player1->prop[1]) {
            shuffle();
            player1->prop[1] -= 1;
        } else if (event->key() == Qt::Key_3 && player1->prop[2]) {
            hint(1);
        }
    }
}

//
// addOneSec接受玩家编号num为参数，实现+1s道具
//
void Widget::addOneSec(int num)
{
    if (restTime + 30000 >= totalTime)
        // 加到满时间上限
        restTime = totalTime;
    else
        restTime += 30000;

    if (num == 1) {
        player1->prop[0] -= 1;
        player1->useAddSec = true;
    } else if(num == 2) {
        player2->prop[0] -= 1;
        player2->useAddSec = true;
    }
}

//
// shuffle实现shuffle道具
//
void Widget::shuffle()
{
    // 特效
    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++) {
            if (map[i][j] > 0 && map[i][j] <= numBox)
                icons[i][j].load(":/images/prop2.png");
        }
    repaint();

    // 统计场上剩余方块，将剩余方块种类输入box
    int* box;
    int numRestBox = width * length - player1->totalLink * 2 - player2->totalLink * 2;
    box = new int [numRestBox];
    int num = 0;
    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++) {
            if (map[i][j] > 0 && map[i][j] <= numBox) {
                box[num] = map[i][j];
                num++;
            }
        }

    // 打乱box
    random_shuffle(box, box + numRestBox);

    // 重新分发方块
    num = 0;
    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++) {
            if (map[i][j] > 0 && map[i][j] <= numBox) {
                map[i][j] = box[num];
                icons[i][j].load(QString::asprintf(":/images/element%d.png", map[i][j]));
                num++;
            }
        }

    // 重排会清空玩家的已激活方块，使提示失效
    if (player1->choosing)
        player1->choosing = 0;
    if (mode == 2 && player2->choosing)
        player2->choosing = 0;
    if (hinting)
        hinting = false;
}

//
// hint(int)接受玩家编号num为参数，实现hint道具
//
void Widget::hint(int num)
{
    timerHint = startTimer(10000);
    hinting = changeHint = true;
    if (num == 1)
        player1->prop[2] -= 1;
    else if (num == 2)
        player2->prop[2] -= 1;
}

//
// hint在需要的时候，找到一对可连接方块
//
void Widget::hint()
{
    if (hinting && changeHint) {
        bool flag = false;
        for (int i = roadWidth; i < roadWidth + width; i++) {
            for (int j = roadWidth; j < roadWidth + length; j++) {
                if (map[i][j] > 0 && map[i][j] <= numBox && canMoveTo(i, j)) {
                    for (int m = i; m < roadWidth + width; m++) {
                        for (int n = roadWidth; n < roadWidth + length; n++) {
                            // 要求：能连接且能移动到附近
                            if (map[m][n] > 0 && map[m][n] <= numBox
                                    && judgeLink(i, m, j, n)[0] != QPoint(0, 0) && canMoveTo(m, n)) {
                                // 赋值hint的两个方块的坐标
                                hint1 = QPoint(boxLength * j, boxLength * i);
                                hint2 = QPoint(boxLength * n, boxLength * m);
                                flag = true;
                                break;
                            }
                        }
                        if (flag) break;
                    }
                    if (flag) break;
                }
                if (flag) break;
            }
            if (flag) break;
        }
        changeHint = false;
    }
}

//
// canMoveTo接受“坐标”x,y为参数，判断玩家能否移动到map[x][y]处，返回能or不能
//
bool Widget::canMoveTo(int x, int y)
{
    if (map[x - 1][y] == 0 || map[x + 1][y] == 0 || map[x][y - 1] == 0 || map[x][y + 1] == 0)
        return true;
    return false;
}

//
// moveToNear接受“坐标”x,y为参数，使玩家移动到map[x][y]处的方块附近
// 返回“玩家以何种姿态移动到该处”，1-4分别代表玩家向上、下、左、右靠近
//
int Widget::moveToNear(int x, int y)
{
    if (map[x - 1][y] == 0) {
        // map[x][y]处方块的下方没有方块，玩家移动到方块下方，并向上靠近方块。以下类似
        player1->x = y * boxLength;
        player1->y = x * boxLength;
        return 2;
    } else if (map[x + 1][y] == 0) {
        player1->x = y * boxLength;
        player1->y = (x + 1) * boxLength - player1->pace;
        return 1;
    } else if (map[x][y - 1] == 0) {
        player1->x = y * boxLength;
        player1->y = x * boxLength;
        return 4;
    } else if (map[x][y + 1] == 0) {
        player1->x = (y + 1) * boxLength - player1->pace;
        player1->y = x * boxLength;
    }
    return 3;
}

//
// freeze接受玩家编号num为参数，实现freeze道具
//
void Widget::freeze(int num)
{
    if(num == 1) {
        player1->freezing = true;
        player1->timerFreeze = startTimer(3000);
        player2->prop[3] -= 1;
    } else if(num == 2) {
        player2->freezing = true;
        player2->timerFreeze = startTimer(3000);
        player1->prop[3] -= 1;
    }
}

//
// dizzy接受玩家编号num为参数，实现dizzy道具
//
void Widget::dizzy(int num)
{
    if(num == 1) {
        player1->dizzying = true;
        player1->timerDizzy = startTimer(10000);
        player2->prop[4] -= 1;
    } else if(num == 2) {
        player2->dizzying = true;
        player2->timerDizzy = startTimer(10000);
        player1->prop[4] -= 1;
    }
}

//
// 重载mousePressEvent函数，以鼠标点击事件event为参数，处理flash作用下鼠标的点击效果
//
void Widget::mousePressEvent(QMouseEvent* event)
{
    if (flashing && event->button() == Qt::LeftButton) {
        // 鼠标点击处坐标对应map[x][y]处
        int y = event->x() / boxLength - 1;
        int x = event->y() / boxLength - 1;
        if (x >= 0 && x < roadWidth * 2 + width && y >= 0 && y < roadWidth * 2 + length) {
            if (map[x][y] == 0) {
                // 无方块，移动至空地
                player1->x = (event->x() - boxLength) / player1->pace * player1->pace;
                player1->y = (event->y() - boxLength) / player1->pace * player1->pace;
            }
            if (map[x][y] > 0 && map[x][y] <= numBox && canMoveTo(x, y)) {
                // 有方块，移动并选中方块
                int num = moveToNear(x, y);
                player1->choose(x, y, num);
            }
            if (mapProp[x][y] != 0)
                // 有道具，收集道具
                player1->collectProp(x, y);
        }
    }
}

//
// 重载timerEvent函数，以计时器事件event为参数，处理在各类道具计时器结束时，清除道具效果
//
void Widget::timerEvent(QTimerEvent* event)
{
    if (hinting && event->timerId() == timerHint)
        hinting = false;
    if (flashing && event->timerId() == timerFlash)
        flashing = false;
    if (player1->freezing && event->timerId() == player1->timerFreeze)
        player1->freezing = false;
    if (player2->freezing && event->timerId() == player2->timerFreeze)
        player2->freezing = false;
    if (player1->dizzying && event->timerId() == player1->timerDizzy)
        player1->dizzying = false;
    if (player2->dizzying && event->timerId() == player2->timerDizzy)
        player2->dizzying = false;
}

//
// judgeLink接受坐标x1,x2,y1,y2为参数，判断map[x1][y1]和map[x2][y2]能否消除
// 返回消除的连接折线上的四个点
//
QPoint* Widget::judgeLink(int x1, int x2, int y1, int y2)
{
    QPoint* linkPoint;
    if (map[x1][y1] != map[x2][y2]) {
        linkPoint = new QPoint[4];
        // 当不能消除时，返回的四个点都是(0,0)
        for (int i = 0; i < 4; i++)
            linkPoint[i] = QPoint(0, 0);
        return linkPoint;
    }
    // 三种情况都考虑一下
    linkPoint = turnZero(x1, x2, y1, y2);
    if (linkPoint != nullptr)
        return linkPoint;
    linkPoint = turnOnce(x1, x2, y1, y2);
    if (linkPoint != nullptr)
        return linkPoint;
    linkPoint = turnTwice(x1, x2, y1, y2);
    if (linkPoint != nullptr)
        return linkPoint;
    linkPoint = new QPoint[4];
    for (int i = 0; i < 4; i++)
        linkPoint[i] = QPoint(0, 0);
    return linkPoint;
}

//
// turnZero接受坐标x1,x2,y1,y2为参数，判断map[x1][y1]和map[x2][y2]用一条线能否连接
// 返回消除的连接折线上的四个点（会画出来一条线）
//
QPoint* Widget::turnZero(int x1, int x2, int y1, int y2)
{
    if ((x1 != x2 && y1 != y2) || (x1 == x2 && y1 == y2))
        // 不能则返回nullptr
        return nullptr;
    // 横线或竖线
    if (x1 == x2) {
        if (!noBoxOnYLine(y1, y2, x1))
            return nullptr;
    } else if (!noBoxOnXLine(x1, x2, y1))
        return nullptr;
    QPoint* linkPoint = new QPoint [4];
    // 返回的前两个点是前一个方块的中心点，后两个点是后一个方块的中心点
    QPoint p1(boxLength * y1 + boxLength / 2, boxLength * x1 + boxLength / 2);
    QPoint p2(boxLength * y2 + boxLength / 2, boxLength * x2 + boxLength / 2);
    linkPoint[0] = linkPoint[1] = p1;
    linkPoint[2] = linkPoint[3] = p2;
    return linkPoint;
}

//
// turnOnce接受坐标x1,x2,y1,y2为参数，判断map[x1][y1]和map[x2][y2]用只折一次的直线能否连接
// 返回消除的连接折线上的四个点（会画出来折一次的线）
//
QPoint* Widget::turnOnce(int x1, int x2, int y1, int y2)
{
    QPoint* linkPoint = new QPoint [4];
    // 折点只有两种情况
    if (map[x1][y2] == 0 && noBoxOnXLine(x1, x2, y2) && noBoxOnYLine(y1, y2, x1)) {
        // 返回的前两个点是前一个方块的中心点，第三个点是折点，第四个点是后一个方块的中心点
        QPoint p1(boxLength * y1 + boxLength / 2, boxLength * x1 + boxLength / 2);
        QPoint p2(boxLength * y2 + boxLength / 2, boxLength * x1 + boxLength / 2);
        QPoint p3(boxLength * y2 + boxLength / 2, boxLength * x2 + boxLength / 2);
        linkPoint[0] = linkPoint[1] = p1;
        linkPoint[2] = p2;
        linkPoint[3] = p3;
        return linkPoint;
    }
    if (map[x2][y1] == 0 && noBoxOnXLine(x1, x2, y1) && noBoxOnYLine(y1, y2, x2)) {
        QPoint p1(boxLength * y1 + boxLength / 2, boxLength * x1 + boxLength / 2);
        QPoint p2(boxLength * y1 + boxLength / 2, boxLength * x2 + boxLength / 2);
        QPoint p3(boxLength * y2 + boxLength / 2, boxLength * x2 + boxLength / 2);
        linkPoint[0] = linkPoint[1] = p1;
        linkPoint[2] = p2;
        linkPoint[3] = p3;
        return linkPoint;
    }
    // 不能则返回nullptr
    return nullptr;
}

//
// turnTwice接受坐标x1,x2,y1,y2为参数，判断map[x1][y1]和map[x2][y2]用折两次的直线能否连接
// 返回消除的连接折线上的四个点
//
QPoint* Widget::turnTwice(int x1, int x2, int y1, int y2)
{
    if (x1 == x2 && y1 == y2)
        // 不能则返回nullptr
        return nullptr;
    QPoint* linkPoint = new QPoint [4];
    int closest = -100;
    // 从上往下扫描
    for (int i = 0; i < roadWidth * 2 + width; i++) {
        bool flag = true;
        for (int j = min(y1, y2); j <= max(y1, y2); j++)
            // 首先这一段内应该没有方块
            if (map[i][j] != 0)
                flag = false;
        if (flag && min(abs(i - x1), abs(i - x2)) < min(abs(closest - x1), abs(closest - x2))
                && noBoxOnXLine(i, x2, y2) && noBoxOnXLine(i, x1, y1))
            closest = i; // 找“最短路径”
    }
    if (closest != -100) {
        // 四个点分别为，中心点1，折点1，折点2，中心点2
        linkPoint[0] = QPoint (boxLength * y1 + boxLength / 2, boxLength * x1 + boxLength / 2);
        linkPoint[1] = QPoint (boxLength * y1 + boxLength / 2, boxLength * closest + boxLength / 2);
        linkPoint[2] = QPoint (boxLength * y2 + boxLength / 2, boxLength * closest + boxLength / 2);
        linkPoint[3] = QPoint (boxLength * y2 + boxLength / 2, boxLength * x2 + boxLength / 2);
        return linkPoint;
    }
    // 从左往右扫描
    for (int i = 0; i < roadWidth * 2 + length; i++) {
        bool flag = true;
        for (int j = min(x1, x2); j <= max(x1, x2); j++)
            if (map[j][i] != 0)
                flag = false;
        if (flag && min(abs(i - y1), abs(i - y2)) < min(abs(closest - y1), abs(closest - y2))
                && noBoxOnYLine(y1, i, x1) && noBoxOnYLine(y2, i, x2))
            closest = i;
    }
    if (closest != -100) {
        linkPoint[0] = QPoint (boxLength * y1 + boxLength / 2, boxLength * x1 + boxLength / 2);
        linkPoint[1] = QPoint (boxLength * closest + boxLength / 2, boxLength * x1 + boxLength / 2);
        linkPoint[2] = QPoint (boxLength * closest + boxLength / 2, boxLength * x2 + boxLength / 2);
        linkPoint[3] = QPoint (boxLength * y2 + boxLength / 2, boxLength * x2 + boxLength / 2);
        return linkPoint;
    }
    return nullptr;
}

//
// noBoxOnXLine接受坐标x1,x2,y为参数，判断map[x1][y]和map[x2][y]之间是否没有方块，返回true（无方块）or false（有方块）
//
bool Widget::noBoxOnXLine(int x1, int x2, int y)
{
    for (int i = min(x1, x2) + 1; i < max(x1, x2); i++)
        if (map[i][y] != 0)
            return false;
    return true;
}

//
// noBoxOnYLine接受坐标y1,y2,x为参数，判断map[x][y1]和map[x][y2]之间是否没有方块，返回true（无方块）or false（有方块）
//
bool Widget::noBoxOnYLine(int y1, int y2, int x)
{
    for (int i = min(y1, y2) + 1; i < max(y1, y2); i++)
        if (map[x][i] != 0)
            return false;
    return true;
}

//
// judgeDead判断是否无解，处理无解情况
//
void Widget::judgeDead()
{
    bool flag = false;
    for (int i = roadWidth; i < roadWidth + width; i++) {
        for (int j = roadWidth; j < roadWidth + length; j++) {
            if (map[i][j] > 0 && map[i][j] <= numBox) {
                for (int m = i; m < roadWidth + width; m++) {
                    for (int n = roadWidth; n < roadWidth + length; n++) {
                        // 还有能连接的方块，且能移动到附近
                        // 同时用judgeLink和canMoveTo的原因：有一种情况是两个方块相邻，但是不能移动到其中一个附近
                        if (map[m][n] > 0 && map[m][n] <= numBox && judgeLink(i, m, j, n)[0] != QPoint(0, 0)
                                && canMoveTo(i, j) && canMoveTo(m, n)) {
                            flag = true;
                            break;
                        }
                    }
                    if (flag) break;
                }
                if (flag) break;
            }
            if (flag) break;
        }
        if (flag) break;
    }
    // 无解，结束游戏
    if (!flag)
        endGame();
}

//
// dropProp掉落道具
//
void Widget::dropProp()
{
    srand(time(NULL));
    // 场上道具数小于2时掉落新道具
    if (numProp < 2) {
        int i, j;
        bool flag = true;
        while(flag) {
            // 随机位置，确认无方块
            i = rand() % (roadWidth * 2 + width);
            j = rand() % (roadWidth * 2 + length);
            if (map[i][j] == 0 && mapProp[i][j] == 0)
                flag = false;
        }
        // 决定道具种类，将对应图片load到icon的对应元素
        if (mode != 2) {
            mapProp[i][j] = rand() % 4 + 1;
            icons[i][j].load(QString::asprintf(":/images/prop%d.png", mapProp[i][j]));
        } else {
            mapProp[i][j] = rand() % 5 + 1;
            icons[i][j].load(QString::asprintf(":/images/mode2prop%d.png", mapProp[i][j]));
        }
        numProp += 1;
    }
}

//
// pause在暂停后弹出QMessageBox
//
void Widget::pause()
{
    if (!pausing) {
        // 暂停后要repaint，遮住地图
        repaint();
        allTimerStop();
        pausing = true;
        QMessageBox mess(QMessageBox::Question, tr("暂停中"), tr("你想要..."));
        QPushButton* button1 = (mess.addButton(tr("保存游戏"), QMessageBox::ActionRole));
        QPushButton* button2 = (mess.addButton(tr("载入存档"), QMessageBox::ActionRole));
        QPushButton* button3 = (mess.addButton(tr("继续游戏"), QMessageBox::ActionRole));
        mess.setDefaultButton(button3);
        mess.setStyleSheet(
                        "QPushButton{font: bold 24px; min-width: 10em; min-height: 5em;}"
                        "QLabel{font: bold 24px;}");
        // 运行消息弹窗mess，直至按下按钮
        mess.exec();
        if (mess.clickedButton() == button1)
            saveFile();
        else if (mess.clickedButton() == button2)
            readFile();
        timer->start();
        timerTime->start();
        timerDrop->start();
        pausing = false;
    }
}

//
// returnToMenu返回主菜单
//
void Widget::returnToMenu()
{
    pausing = true;
    repaint();
    allTimerStop();
    saveAchieve();
    QMessageBox mess(QMessageBox::Question, "即将返回菜单", "是否需要存档？",
                     QMessageBox::Yes | QMessageBox::No);
    mess.setStyleSheet(
                    "QPushButton{font: bold 24px; min-width: 10em; min-height: 5em;}"
                    "QLabel{font: bold 24px;}");
    int result = mess.exec();
    if (result == QMessageBox::Yes)
        saveFile();
    Menu* m = new Menu;
    this->close();
    m->show();
}

//
// restart重新开始本轮游戏
//
void Widget::restart()
{
    pausing = true;
    allTimerStop();
    saveAchieve();
    player1 = new Player;
    player2 = new Player;
    if(mode == 3 && winning) {
        totalTime -= 30000;
        layer += 1;
    }
    Widget* w = new Widget(nullptr, mode, totalTime, width, length, numBox, timeProp);
    w->layer = layer;
    this->close();
    player1->w = w;
    player2->w = w;
    w->typeChallege = typeChallege;
    w->typeHelp = typeHelp;
    w->show();
}

//
// readFile从data.txt中读取存档
//
void Widget::readFile()
{
    QFile file("data.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        in >> mode >> restTime >> totalTime >> boxLength >> width
           >> length >> numBox >> roadWidth >> numProp >> timeProp >> layer >> typeChallege >> typeHelp;

        map = new int* [width + roadWidth * 2];
        mapProp = new int* [width + roadWidth * 2];
        icons = new QPixmap* [width + roadWidth * 2];
        for (int i = 0; i < width + roadWidth * 2; i++) {
            map[i] = new int[length + roadWidth * 2];
            mapProp[i] = new int[length + roadWidth * 2];
            icons[i] = new QPixmap[length + roadWidth * 2];
            for (int j = 0; j < length + roadWidth * 2; j++) {
                in >> map[i][j];
                in >> mapProp[i][j];
                in >> icons[i][j];
            }
        }
        createWidget();
        readAchieve();

        in >> player1->number >> player1->x >> player1->y >> player1->pace >> player1->boxOneX
           >> player1->boxOneY >> player1->roleLength >> player1->useAddSec
           >> player1->choosing >> player1->totalLink;
        if (mode == 2) {
            in >> player2->number >> player2->x >> player2->y >> player2->pace >> player2->boxOneX
               >> player2->boxOneY >> player2->roleLength >> player1->useAddSec
               >> player2->choosing >> player2->totalLink;
            for (int i = 0; i < 5; i++) {
                in >> player1->prop[i];
                in >> player2->prop[i];
            }
        } else {
            for (int i = 0; i < 4; i++)
                in >> player1->prop[i];
        }
        changeFlag = true;
    }
}

//
// saveFile向data.txt中存档
//
void Widget::saveFile()
{
    QFile file("data.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);

        out << mode << restTime << totalTime << boxLength << width << length
            << numBox << roadWidth << numProp << timeProp << layer << typeChallege << typeHelp;

        for (int i = 0; i < width + roadWidth * 2; i++)
            for (int j = 0; j < length + roadWidth * 2; j++) {
                out << map[i][j];
                out << mapProp[i][j];
                out << icons[i][j];
            }

        out << player1->number << player1->x << player1->y << player1->pace << player1->boxOneX
            << player1->boxOneY << player1->roleLength << player1->useAddSec
            << player1->choosing << player1->totalLink;

        if (mode == 2) {
            out << player2->number << player2->x << player2->y << player2->pace << player2->boxOneX
                << player2->boxOneY << player2->roleLength << player2->useAddSec
                << player2->choosing << player2->totalLink;
            for (int i = 0; i < 5; i++) {
                out << player1->prop[i];
                out << player2->prop[i];
            }
        } else {
            for (int i = 0; i < 4; i++)
                out << player1->prop[i];
        }
    }
}

//
// doIfModeChange应对可能出现的模式改变（例如，在双人模式中暂停，载入单人模式的存档）
//
void Widget::doIfModeChange()
{
    if (mode == 2) {
        ui->labelName2->setVisible(true);
        ui->labelScore2->setVisible(true);
        ui->numScore2->setVisible(true);
        ui->labelProp4->setText("冻结");
        ui->labelProp5->setVisible(true);
    }
    for (int i = 0; i < width + roadWidth * 2; i++)
        for (int j = 0; j < length + roadWidth * 2; j++) {
            if (map[i][j] == 0 && mapProp[i][j] == 0) {
                QPixmap clearPix = QPixmap();
                icons[i][j] = clearPix;
            }
        }
    changeFlag = false;
    ui->timeBar->setMaximum(totalTime);
}

//
// faceChallege接受坐标x,y为参数，在消除map[x][y]时，若存在buff或debuff，触发效果（详见submenu.ui)
//
void Widget::faceChallege(int x, int y)
{
    // map[x][y] % 7结果相同的，方块元素种类也相同，详见:/images/element1.png至:/images/element14.png
    if (typeChallege == 1 && map[x][y] % 7 == 1) {
        player1->freezing = true;
        player1->timerFreeze = startTimer(3000);
    }
    if (typeChallege == 2 && map[x][y] % 7 == 2) {
        if (restTime - 3000 < 0) {
            restTime = 1000;
        } else {
            restTime -= 3000;
        }
    }
    if (typeHelp == 1 && map[x][y] % 7 == 4) {
        if (restTime + 3000 >= totalTime) {
            restTime = totalTime;
        } else {
            restTime += 3000;
        }
    }
    if (typeHelp == 2 && map[x][y] % 7 == 5) {
        srand(time(NULL));
        if (rand() % 3 == 0)
            player1->prop[rand() % 4] += 1;
    }
}
