#include "submenu.h"
#include "ui_submenu.h"
#include "player.h"
extern QString strStyle;
extern Player* player1, * player2;

//
// 副菜单的构造函数，继承自QWidget类，ui界面为参数
//
SubMenu::SubMenu(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SubMenu)
{
    ui->setupUi(this);

    // 一些初始化
    boxLength = 100;
    gap = 30;
    totalTime = 180000;
    width = 9;
    length = 12;
    numBox = 14;
    timeProp = 10000;
    curHelp = curChallenge = 0;

    // 分别有6种buff和debuff（援助和挑战）可以选择
    challenge = new bool[6];
    for (int i = 0; i < 6; i++)
        challenge[i] = false;

    help = new bool[6];
    for (int i = 0; i < 6; i++)
        help[i] = false;

    // 设置定时器与窗口
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    timer->start(100);
    setWindowTitle(tr("Star Rail Link"));
    setFixedSize(boxLength * 20, boxLength * 11.25);
    setLabel();
    connectButton();
}

//
// 副菜单的析构函数
//
SubMenu::~SubMenu()
{
    delete ui;
    delete timer;
    delete challenge;
    delete help;
}

//
// 重载paintEvent函数，接受绘图事件QPaintEvent类为参数，画出副菜单的窗口，每次update时调用
//
void SubMenu::paintEvent(QPaintEvent*)
{
    // 设置每个选项框的边缘线红色，设置背景图片
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 4, Qt::SolidLine));
    QPixmap background;
    background.load(":/images/pure.png");
    painter.drawPixmap(0, 0, boxLength * 20, boxLength * 11.25, background);
    background.load(":/images/prop4.png");
    painter.drawPixmap(0, 0, boxLength * 10, boxLength * 11.25, background);
    background.load(":/images/prop3.png");
    painter.drawPixmap(boxLength * 10, 0, boxLength * 10, boxLength * 11.25, background);

    // 先用path画出标题框和开始按钮框，深黄色填充
    QColor color2(Qt::yellow);
    color2.setAlpha(200);
    QPainterPath path;
    path.addRoundedRect(boxLength * 3, boxLength / 4, boxLength * 4, boxLength / 2, UIRAD, UIRAD);
    path.addRoundedRect(boxLength * 13, boxLength / 4, boxLength * 4, boxLength / 2, UIRAD, UIRAD);
    path.addRoundedRect(boxLength * 9, boxLength * 19 / 2, boxLength * 2, boxLength, UIRAD, UIRAD);
    painter.fillPath(path, color2);
    painter.drawPath(path);

    // 画选项框
    drawLabel(&painter);
}

//
// 开始游戏的函数
//
void SubMenu::begin()
{
    // 调整参数后，关闭当前窗口，打开游戏界面
    setData();
    timer->stop();
    this->close();
    player1 = new Player;
    player2 = new Player;
    Widget* w = new Widget(nullptr, 1, totalTime, width, length, numBox, timeProp);
    player1->w = w;
    player2->w = w;
    setTypeHelp(w);
    w->show();
}

//
// 设置所有Label与按钮的位置、大小与字体
// Label按左六右六排列，每个Label下都有一个按钮与之重合
//
void SubMenu::setLabel()
{
    ui->label->setGeometry(boxLength, 0, boxLength * 8, boxLength);
    ui->label2->setGeometry(boxLength, boxLength * 1, boxLength * 8, boxLength);
    ui->label3->setGeometry(boxLength, boxLength * 2 + gap, boxLength * 8, boxLength);
    ui->label4->setGeometry(boxLength, boxLength * 3 + gap * 2, boxLength * 8, boxLength);
    ui->label5->setGeometry(boxLength, boxLength * 5, boxLength * 8, boxLength);
    ui->label6->setGeometry(boxLength, boxLength * 6 + gap, boxLength * 8, boxLength);
    ui->label7->setGeometry(boxLength, boxLength * 7 + gap * 2, boxLength * 8, boxLength);
    ui->pushButton->setGeometry(boxLength, boxLength * 1, boxLength * 8, boxLength);
    ui->pushButton2->setGeometry(boxLength, boxLength * 2 + gap, boxLength * 8, boxLength);
    ui->pushButton3->setGeometry(boxLength, boxLength * 3 + gap * 2, boxLength * 8, boxLength);
    ui->pushButton4->setGeometry(boxLength, boxLength * 5, boxLength * 8, boxLength);
    ui->pushButton5->setGeometry(boxLength, boxLength * 6 + gap, boxLength * 8, boxLength);
    ui->pushButton6->setGeometry(boxLength, boxLength * 7 + gap * 2, boxLength * 8, boxLength);
    ui->startButton->setGeometry(boxLength * 9, boxLength * 19 / 2, boxLength * 2, boxLength);
    ui->labelStart->setGeometry(boxLength * 9, boxLength * 19 / 2, boxLength * 2, boxLength);
    ui->label8->setGeometry(boxLength * 11, 0, boxLength * 8, boxLength);
    ui->label9->setGeometry(boxLength * 11, boxLength * 1, boxLength * 8, boxLength);
    ui->label10->setGeometry(boxLength * 11, boxLength * 2 + gap, boxLength * 8, boxLength);
    ui->label11->setGeometry(boxLength * 11, boxLength * 3 + gap * 2, boxLength * 8, boxLength);
    ui->label12->setGeometry(boxLength * 11, boxLength * 5, boxLength * 8, boxLength);
    ui->label13->setGeometry(boxLength * 11, boxLength * 6 + gap, boxLength * 8, boxLength);
    ui->label14->setGeometry(boxLength * 11, boxLength * 7 + gap * 2, boxLength * 8, boxLength);
    ui->pushButton7->setGeometry(boxLength * 11, boxLength * 1, boxLength * 8, boxLength);
    ui->pushButton8->setGeometry(boxLength * 11, boxLength * 2 + gap, boxLength * 8, boxLength);
    ui->pushButton9->setGeometry(boxLength * 11, boxLength * 3 + gap * 2, boxLength * 8, boxLength);
    ui->pushButton10->setGeometry(boxLength * 11, boxLength * 5, boxLength * 8, boxLength);
    ui->pushButton11->setGeometry(boxLength * 11, boxLength * 6 + gap, boxLength * 8, boxLength);
    ui->pushButton12->setGeometry(boxLength * 11, boxLength * 7 + gap * 2, boxLength * 8, boxLength);
    ui->label->setStyleSheet(strStyle);
    ui->label2->setStyleSheet(strStyle);
    ui->label3->setStyleSheet(strStyle);
    ui->label4->setStyleSheet(strStyle);
    ui->label5->setStyleSheet(strStyle);
    ui->label6->setStyleSheet(strStyle);
    ui->label7->setStyleSheet(strStyle);
    ui->label8->setStyleSheet(strStyle);
    ui->label9->setStyleSheet(strStyle);
    ui->label10->setStyleSheet(strStyle);
    ui->label11->setStyleSheet(strStyle);
    ui->label12->setStyleSheet(strStyle);
    ui->label13->setStyleSheet(strStyle);
    ui->label14->setStyleSheet(strStyle);
    ui->labelStart->setStyleSheet(strStyle);
}

//
// 将按钮点击信号与槽函数绑定
//
void SubMenu::connectButton()
{
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(begin()));
    connect(ui->pushButton, &QPushButton::clicked, this, [this]() {
        setChallege(0);
    });
    connect(ui->pushButton2, &QPushButton::clicked, this, [this]() {
        setChallege(1);
    });
    connect(ui->pushButton3, &QPushButton::clicked, this, [this]() {
        setChallege(2);
    });
    connect(ui->pushButton4, &QPushButton::clicked, this, [this]() {
        setChallege(3);
    });
    connect(ui->pushButton5, &QPushButton::clicked, this, [this]() {
        setChallege(4);
    });
    connect(ui->pushButton6, &QPushButton::clicked, this, [this]() {
        setChallege(5);
    });
    connect(ui->pushButton7, &QPushButton::clicked, this, [this]() {
        setHelp(0);
    });
    connect(ui->pushButton8, &QPushButton::clicked, this, [this]() {
        setHelp(1);
    });
    connect(ui->pushButton9, &QPushButton::clicked, this, [this]() {
        setHelp(2);
    });
    connect(ui->pushButton10, &QPushButton::clicked, this, [this]() {
        setHelp(3);
    });
    connect(ui->pushButton11, &QPushButton::clicked, this, [this]() {
        setHelp(4);
    });
    connect(ui->pushButton12, &QPushButton::clicked, this, [this]() {
        setHelp(5);
    });
}

//
// setChallege接受debuff代号num为参数，进行debuff（挑战）的选择
//
void SubMenu::setChallege(int num)
{
    // 如果已选中，则取消选择
    if (challenge[num])
        challenge[num] = false;
    // debuff的前三项与buff的前三项是冲突的，不能同时选择
    else if ((num < 3 && !help[num]) || num >= 3) {
        // 只能选择一个debuff，前一个选择的debuff会被取消选择
        challenge[curChallenge] = false;
        challenge[num] = true;
        curChallenge = num;
    }
}

//
// setHelp以buff代号num为参数，进行buff（援助）的选择
//
void SubMenu::setHelp(int num)
{
    // 如果已选中，则取消选择
    if (help[num])
        help[num] = false;
    // debuff的前三项与buff的前三项是冲突的，不能同时选择
    else if ((num < 3 && !challenge[num]) || num >= 3) {
        // 只能选择一个buff，前一个选择的buff会被取消选择
        help[curHelp] = false;
        help[num] = true;
        curHelp = num;
    }
}

//
// setData根据已选择的buff和debuff,对初始参数调整
//
void SubMenu::setData()
{
    if (challenge[0])
        totalTime = 150000;
    if (help[0])
        totalTime = 240000;
    if (challenge[1]) {
        width = 10;
        length = 16;
        //        width = 20;
        //        length = 30;
    }
    if (help[1]) {
        //        width = 8;
        //        length = 10;
        width = 2;
        length = 2;
    }
    if (challenge[2])
        timeProp = 15000;
    if (help[2])
        timeProp = 8000;
    if (help[3])
        numBox = 10;
}

//
// setTypeHelp向游戏界面传达buff和debuff种类
// 主要是一些无法通过setData生效的buff和debuff需要这样做
//
void SubMenu::setTypeHelp(Widget* w)
{
    if (challenge[3])
        w->typeChallege = 1;
    if (challenge[4])
        w->typeChallege = 2;
    if (challenge[5])
        w->typeChallege = 3;
    if (help[4])
        w->typeHelp = 1;
    if (help[5])
        w->typeHelp = 2;
}

//
// drawLabel接受绘图类painter为参数，在painter上画出选项框
//
void SubMenu::drawLabel(QPainter* painter)
{
    QColor color1(Qt::yellow);
    color1.setAlpha(10);
    QColor color2(Qt::yellow);
    color2.setAlpha(200);

    // 以下4个for循环，分别画出左上、左下、右上、右下各三个框
    for (int i = 0; i < 3; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength, boxLength * (i + 1) + gap * i, boxLength * 8, boxLength, UIRAD, UIRAD);
        // 已选择的debuff框内填充深黄色，否则为浅黄色
        if (challenge[i])
            painter->fillPath(path, color2);
        else
            painter->fillPath(path, color1);
        painter->drawPath(path);
    }
    for (int i = 0; i < 3; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength, boxLength * (i + 5) + gap * i, boxLength * 8, boxLength, UIRAD, UIRAD);
        if (challenge[i + 3])
            painter->fillPath(path, color2);
        else
            painter->fillPath(path, color1);
        painter->drawPath(path);
    }
    for (int i = 0; i < 3; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength * 11, boxLength * (i + 1) + gap * i, boxLength * 8, boxLength, UIRAD, UIRAD);
        if (help[i])
            painter->fillPath(path, color2);
        else
            painter->fillPath(path, color1);
        painter->drawPath(path);
    }
    for (int i = 0; i < 3; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength * 11, boxLength * (i + 5) + gap * i, boxLength * 8, boxLength, UIRAD, UIRAD);
        if (help[i + 3])
            painter->fillPath(path, color2);
        else
            painter->fillPath(path, color1);
        painter->drawPath(path);
    }
}
