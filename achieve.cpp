#include "achieve.h"
#include "ui_achieve.h"
#include "widget.h"
#include "menu.h"
#define UIRAD 20
extern QString strStyle;

//
// 成就页面的构造函数，继承自QWidget类，ui界面为参数
//
Achieve::Achieve(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Achieve)
{
    ui->setupUi(this);

    // 一些初始化，boxLength相当于一个“刻度”
    boxLength = 100;
    numAchieve = 0;

    // 设置定时器
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
    timer->start(100);

    // 按钮点击信号与返回主菜单信号相连
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(returnToMenu()));
    // 窗口设置
    setWindowTitle(tr("Star Rail Link - achievement"));
    setFixedSize(boxLength * 20, boxLength * 11.25);

    // 对ui中label和按钮的设置
    setLabel();

    // 从achieve,txt文件中，导入achieve数据（成就是否已点亮）
    achieve = new bool[8];
    for (int i = 0; i < 8; i++)
        achieve[i] = false;
    QFile file("achieve.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        for (int i = 0; i < 8; i++)
            in >> achieve[i];
    }

    // 统计已点亮成就的个数，并显示
    for (int i = 0; i < 8; i++)
        if (achieve[i]) numAchieve++;
    ui->label10->setText(QString::asprintf("已点亮成就数：%d", numAchieve));
}

//
// 成就页面的析构函数
//
Achieve::~Achieve()
{
    delete ui;
    delete timer;
    delete achieve;
}

//
//重载paintEvent函数，接受绘图事件QPaintEvent类为参数，画出成就系统的窗口，每次update时调用
//
void Achieve::paintEvent(QPaintEvent*)
{
    // 设置每个成就框的边缘线红色，内部填充显示的两种颜色。大背景为图片
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 4, Qt::SolidLine));
    QColor color1(Qt::yellow);
    color1.setAlpha(10);
    QColor color2(Qt::yellow);
    color2.setAlpha(200);
    QPixmap background;
    background.load(":/images/heta.png");
    painter.drawPixmap(0, 0, boxLength * 20, boxLength * 11.25, background);
    // 用path画左侧成就的四个圆角方框
    for (int i = 0; i < 4; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength, boxLength * (2 * i + 1), boxLength * 7, boxLength, UIRAD, UIRAD);
        // 已点亮的成就为深黄，否则为浅黄
        if (achieve[i])
            painter.fillPath(path, color2);
        else
            painter.fillPath(path, color1);
        painter.drawPath(path);
        // 加上可爱图片
        QPixmap picture;
        picture.load(QString::asprintf(":/images/achieve%d.png", i + 1));
        painter.drawPixmap(boxLength, boxLength * (2 * i + 1), boxLength, boxLength, picture);
    }
    // 画右侧成就的圆角方框，类似左侧
    for (int i = 0; i < 4; i++) {
        QPainterPath path;
        path.addRoundedRect(boxLength * 12, boxLength * (2 * i + 1), boxLength * 7, boxLength, UIRAD, UIRAD);
        if (achieve[i + 4])
            painter.fillPath(path, color2);
        else
            painter.fillPath(path, color1);
        painter.drawPath(path);
        QPixmap picture;
        picture.load(QString::asprintf(":/images/achieve%d.png", i + 5));
        painter.drawPixmap(boxLength * 18, boxLength * (2 * i + 1), boxLength, boxLength, picture);
    }
    // 画返回键的按钮框，填充深黄色
    QPainterPath path;
    path.addRoundedRect(boxLength, boxLength * 9, boxLength * 2, boxLength, UIRAD, UIRAD);
    path.addRoundedRect(boxLength * 17, boxLength * 9, boxLength * 2, boxLength, UIRAD, UIRAD);
    painter.fillPath(path, color2);
    painter.drawPath(path);
}

//
// 返回主菜单
//
void Achieve::returnToMenu()
{
    // 关闭当前界面，构造并显示主菜单
    timer->stop();
    Menu* m = new Menu;
    this->close();
    m->show();
}

//
// 对ui中label和按钮的位置摆放，左右各一列,以及设置字体格式
//
void Achieve::setLabel()
{
    ui->label->setGeometry(boxLength * 2, boxLength, boxLength * 7, boxLength);
    ui->label2->setGeometry(boxLength * 2, boxLength * 3, boxLength * 7, boxLength);
    ui->label3->setGeometry(boxLength * 2, boxLength * 5, boxLength * 7, boxLength);
    ui->label4->setGeometry(boxLength * 2, boxLength * 7, boxLength * 7, boxLength);
    ui->label5->setGeometry(boxLength * 17, boxLength * 9, boxLength * 2, boxLength);
    ui->label6->setGeometry(boxLength * 11, boxLength, boxLength * 7, boxLength);
    ui->label7->setGeometry(boxLength * 11, boxLength * 3, boxLength * 7, boxLength);
    ui->label8->setGeometry(boxLength * 11, boxLength * 5, boxLength * 7, boxLength);
    ui->label9->setGeometry(boxLength * 11, boxLength * 7, boxLength * 7, boxLength);
    ui->label10->setGeometry(boxLength, boxLength * 9, boxLength * 2, boxLength);
    ui->pushButton->setGeometry(boxLength * 17, boxLength * 9, boxLength * 2, boxLength);

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

    ui->label8->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->label6->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->label7->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->label9->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}
