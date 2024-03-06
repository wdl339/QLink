#include "widget.h"
#include "menu.h"
#include "player.h"
#include "simpletest.h"
#include <QApplication>

// 构造了两名玩家
Player* player1 = new Player;
Player* player2 = new Player;

//
// 平平无奇的main函数
//
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    // 构造主菜单并显示
    Menu m;
    m.show();
    return a.exec();
}
