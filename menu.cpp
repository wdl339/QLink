#include "menu.h"
#include "ui_menu.h"
#include "widget.h"
#include "player.h"
#include "achieve.h"
#include "submenu.h"
#define UIRAD 20
extern Player* player1, * player2;

// 字体格式
QString strStyle2 = "QLabel{"
                    "font-family: \"Microsoft YaHei\";"
                    "font-size: 36px;"
                    "color: black;"
                    "}";

QString strStyle3 = "QLabel{"
                    "font-family: \"Microsoft YaHei\";"
                    "font-size: 22px;"
                    "color: black;"
                    "}";

//
// 主菜单的构造函数，继承自QWidget类，ui界面为参数
//
Menu::Menu(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 一些初始化，boxLength相当于一个“刻度”
    boxLength = 100;
    x = this->width();
    y = this->height();

    // 设置定时器
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    timer->start(100);

    // 绑定按钮与相应函数
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(mode1()));
    connect(ui->pushButton2, SIGNAL(clicked()), this, SLOT(mode2()));
    connect(ui->pushButton3, SIGNAL(clicked()), this, SLOT(open()));
    connect(ui->pushButton4, SIGNAL(clicked()), this, SLOT(mode3()));
    connect(ui->pushButton5, SIGNAL(clicked()), this, SLOT(achieve()));
    connect(ui->pushButton6, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->pushButton7, SIGNAL(clicked()), this, SLOT(rule()));

    // 窗口设置
    resize(boxLength * 20, boxLength * 11.25);
    setWindowTitle(tr("Star Rail Link"));
    setLabel();
}

//
// 析构函数
//
Menu::~Menu()
{
    delete ui;
    delete timer;
}

//
// 重载paintEvent函数，接受绘图事件QPaintEvent类为参数，画出成就系统的界面，每次update时调用
//
void Menu::paintEvent(QPaintEvent*)
{
    // 这里主要是写一个窗口大小可以调整（放大or缩小），大小改变后自动适应当前大小
    boxLength = this->width() / 20;
    resize(boxLength * 20, boxLength * 11.25);
    ifSizeChange();

    // 设置每个按钮框的边缘线红色，大背景
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 4, Qt::SolidLine));
    QPixmap background;
    background.load(":/images/menu.png");
    painter.drawPixmap(0, 0, this->width(), this->height(), background);

    // 用path画出7个按钮的圆角方框，内部填充黄色
    QPainterPath path;
    for (int i = 0; i < 3; i++) {
        path.addRoundedRect(boxLength, boxLength * (2 + 2 * i), boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);
        path.addRoundedRect(boxLength * 9 / 2, boxLength * (2 + 2 * i), boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);
    }
    path.addRoundedRect(boxLength, boxLength * 8, boxLength * 3, boxLength * 3 / 2, UIRAD, UIRAD);
    painter.fillPath(path, Qt::yellow);
    painter.drawPath(path);

}

//
// 单人模式的槽函数
//
void Menu::mode1()
{
    // 关闭当前窗口，打开副菜单
    this->close();
    SubMenu* s = new SubMenu;
    s->show();
}

//
// 双人模式的槽函数
//
void Menu::mode2()
{
    // 关闭当前窗口，构造新的玩家，打开游戏界面
    this->close();
    player1 = new Player;
    player2 = new Player;
    Widget* w = new Widget(nullptr, 2);
    player1->w = w;
    player2->w = w;
    w->show();
}

//
// 读取并返回存档文件中mode值（1代表单人，2代表双人，3代表“深渊”模式）
//
int Menu::readMode()
{
    QFile file("data.txt");
    int num;
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> num;
        return num;
    }
    // 如果当前没有存档，则返回1（单人模式）
    return 1;
}

//
// 读取存档的函数
//
void Menu::open()
{
    this->close();
    player1 = new Player;
    player2 = new Player;
    // 根据存档中的mode值构造相应的游戏界面
    Widget* w = new Widget(nullptr, readMode());
    // 读取存档参数
    w->readFile();
    player1->w = w;
    player2->w = w;
    w->show();
}

//
// “深渊”模式的槽函数，与前面类似的
//
void Menu::mode3()
{
    this->close();
    player1 = new Player;
    player2 = new Player;
    Widget* w = new Widget(nullptr, 3);
    player1->w = w;
    player2->w = w;
    // 初始时位于“深渊”的第一层
    w->layer = 1;
    w->show();
}

//
// 打开成就界面的槽函数
//
void Menu::achieve()
{
    this->close();
    Achieve* a = new Achieve();
    a->show();
}

//
// 打开规则介绍界面的槽函数
//
void Menu::rule()
{
    // 一些中二的规则介绍
    QString s = "我，黑塔，在此对模拟宇宙的规则进行说明\n\n"
                "角色出现在地图上的随机位置，控制角色在地图的空地上移动\n"
                "单人模式下，WSAD键分别对应上下左右移动\n"
                "双人模式下，WSAD键对应玩家1的上下左右移动，↑↓←→键对应玩家2的上下左右移动\n"
                "角色位于方块旁且再次向方块方向移动，会激活该方块\n"
                "若激活的两个方块相同，且能用弯折两次以内的折线连接，即可消除方块，获得分数\n\n"
                "道具定时随机出现在地图的空地上，地图上最多同时有两个道具\n"
                "接近道具即可收集道具，收集之后才能使用\n"
                "单人模式下，按1、2、3、4键，使用续时、重组、提示、瞬移道具\n"
                "双人模式下，玩家1按1、2、3、4、5键，使用续时、重组、提示、冻结、眩晕道具\n"
                "玩家2则是按6、7、8、9、0键\n\n"
                "道具介绍：\n"
                "续时：延长剩余时间30s\n"
                "重排：所有方块位置重排\n"
                "提示：10s内会高亮一对可能链接的方块，被消除后会高亮下一对\n"
                "瞬移：5s内允许通过鼠标单击来移动角色位置，如果点击到空地则角色可以移动到该位置。\n"
                "如果点击到方块且角色可以移动到该方块旁，则角色移动到该方块旁，同时方块被激活\n"
                "冻结：对手3s内无法移动\n"
                "眩晕：对手10s内移动方向颠倒（上下左右颠倒）\n\n"
                "在单人模式，如果感觉难度太大，可以接受来自我的援助\n"
                "一个叫银狼的黑客骇入了我的模拟宇宙，植入了能修改模拟宇宙参数的病毒\n"
                "你可以选择接受她的挑战，这能帮助我可以尽快定位和消灭病毒\n"
                "援助和挑战都最多只能选择一个，都不选也会以正常模式进入游戏\n\n"
                "在游戏中暂停时，可以选择保存，读取之前保存的存档，以及继续游戏\n"
                "最多只能同时存在一个存档，请谨慎存档\n"
                "使用“历战余响”，可以读取你之前保存的关卡\n\n"
                "“忘却之庭”是具有挑战性的模式，初始进入时位于“深渊”的第一层\n"
                "通关后选择再来一局，会进入下一层\n"
                "失败后选择再来一局，会重新挑战这一层\n"
                "难度会逐层提升，进入新的一层时，会随机增加四类debuff之一，\n"
                "分别为时间上限增加，地图长度增加，宽度增加，道具掉落减少\n"
                "试试看你能到达第几层？\n\n"
                "（你说得对，但规则就是用来打破的！）";
    // 绘制messageBox，标题 按钮 长宽 字体。默认在OK按钮
    QMessageBox mess(QMessageBox::NoIcon, tr("星际和平指南"), s);
    QPushButton* button1 = (mess.addButton(tr("OK"), QMessageBox::ActionRole));
    mess.setDefaultButton(button1);
    mess.setStyleSheet("QLabel{min-width: 1000px; min-height: 800px; font-size: 24px;}");
    mess.exec();
    // 点OK返回
    if (mess.clickedButton() == button1)
        return;
}

//
// 退出游戏的槽函数
//
void Menu::quit()
{
    exit(0);
}

//
// 判断当前窗口大小是否变化，如果变化，更新窗口参数，并重新设置Label位置与大小
//
void Menu::ifSizeChange()
{
    if (x != this->width() || y != this->height()) {
        x = this->width();
        y = this->height();
        setLabel();
    }
}

//
// 设置所有Label与按钮的位置、大小与字体
// Label按左四右三排列，每个Label下都有一个按钮与之重合
//
void Menu::setLabel()
{
    ui->labelOne->setGeometry(boxLength, boxLength * 2, boxLength * 3, boxLength * 3 / 4);
    ui->labelTwo->setGeometry(boxLength, boxLength * 4, boxLength * 3, boxLength * 3 / 4);
    ui->labelThree->setGeometry(boxLength, boxLength * 6, boxLength * 3, boxLength * 3 / 4);
    ui->labelFour->setGeometry(boxLength * 9 / 2, boxLength * 2, boxLength * 3, boxLength * 3 / 4);
    ui->labelFive->setGeometry(boxLength * 9 / 2, boxLength * 4, boxLength * 3, boxLength * 3 / 4);
    ui->labelSix->setGeometry(boxLength * 9 / 2, boxLength * 6, boxLength * 3, boxLength * 3 / 4);
    ui->labelSeven->setGeometry(boxLength, boxLength * 8, boxLength * 3, boxLength * 3 / 4);

    ui->label->setGeometry(boxLength, boxLength * 3, boxLength * 3, boxLength * 3 / 4);
    ui->label2->setGeometry(boxLength, boxLength * 5, boxLength * 3, boxLength * 3 / 4);
    ui->label3->setGeometry(boxLength, boxLength * 7, boxLength * 3, boxLength * 3 / 4);
    ui->label4->setGeometry(boxLength * 9 / 2, boxLength * 3, boxLength * 3, boxLength * 3 / 4);
    ui->label5->setGeometry(boxLength * 9 / 2, boxLength * 5, boxLength * 3, boxLength * 3 / 4);
    ui->label6->setGeometry(boxLength * 9 / 2, boxLength * 7, boxLength * 3, boxLength * 3 / 4);
    ui->label7->setGeometry(boxLength, boxLength * 9, boxLength * 3, boxLength * 3 / 4);

    ui->pushButton->setGeometry(boxLength, boxLength * 2, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton2->setGeometry(boxLength, boxLength * 4, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton3->setGeometry(boxLength, boxLength * 6, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton4->setGeometry(boxLength * 9 / 2, boxLength * 2, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton5->setGeometry(boxLength * 9 / 2, boxLength * 4, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton6->setGeometry(boxLength * 9 / 2, boxLength * 6, boxLength * 3, boxLength * 3 / 2);
    ui->pushButton7->setGeometry(boxLength, boxLength * 8, boxLength * 3, boxLength * 3 / 2);

    ui->labelOne->setStyleSheet(strStyle2);
    ui->labelTwo->setStyleSheet(strStyle2);
    ui->labelThree->setStyleSheet(strStyle2);
    ui->labelFour->setStyleSheet(strStyle2);
    ui->labelFive->setStyleSheet(strStyle2);
    ui->labelSix->setStyleSheet(strStyle2);
    ui->labelSeven->setStyleSheet(strStyle2);
    ui->label->setStyleSheet(strStyle3);
    ui->label2->setStyleSheet(strStyle3);
    ui->label3->setStyleSheet(strStyle3);
    ui->label4->setStyleSheet(strStyle3);
    ui->label5->setStyleSheet(strStyle3);
    ui->label6->setStyleSheet(strStyle3);
    ui->label7->setStyleSheet(strStyle3);
}
