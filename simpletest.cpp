//#include "simpletest.h"
//#include <iostream>
//#include <fstream>
//#define BOXLENGTH 80
//using namespace std;

//// 测试流程：取消本文件和.h文件的注释，注释main函数，重新构建项目，debug运行

////
//// 测试类的构造函数
////
//SimpleTest::SimpleTest()
//{
//    // 从test.txt导入用于测试的地图数据
//    ifstream file("test.txt");
//    int num;
//    if (file.is_open()) {
//        for (int i = 0; i < wi.width + wi.roadWidth * 2; i++)
//            for (int j = 0; j < wi.length + wi.roadWidth * 2; j++) {
//                // 输入地图上每个方块的种类参数，没有道具
//                file >> num;
//                wi.map[i][j] = num;
//                wi.mapProp[i][j] = 0;
//            }
//        file.close();
//    }
//}

////
//// 检测“一条线能连接”的消除情况（横&竖）
////
//void SimpleTest::caseTurnZero()
//{
//    // 对map[2][2]与map[2][5]连接的判断，返回数组的四个值中
//    // 前两个值对应map[2][2]方块在地图上的中心点，后两个值对应map[2][5]方块在地图上的中心点
//    QVERIFY(wi.judgeLink(2, 2, 2, 5)[0] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 5)[1] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 5)[2] == QPoint(5.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 5)[3] == QPoint(5.5 * BOXLENGTH, 2.5 * BOXLENGTH));

//    // 对map[2][2]与map[5][2]连接的判断，与上面类似
//    QVERIFY(wi.judgeLink(2, 5, 2, 2)[0] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 5, 2, 2)[1] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 5, 2, 2)[2] == QPoint(2.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 5, 2, 2)[3] == QPoint(2.5 * BOXLENGTH, 5.5 * BOXLENGTH));

//}

////
//// 检测“折一次能连接”的消除情况（向上&向下）
////
//void SimpleTest::caseTurnOnce()
//{
//    // 对map[3][3]与map[4][5]连接的判断，返回数组的四个值中
//    // 前两个值对应map[3][3]方块在地图上的中心点，
//    // 第三个值对应转折点，第四个值对应map[4][5]方块在地图上的中心点
//    QVERIFY(wi.judgeLink(3, 4, 3, 5)[0] == QPoint(3.5 * BOXLENGTH, 3.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(3, 4, 3, 5)[1] == QPoint(3.5 * BOXLENGTH, 3.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(3, 4, 3, 5)[2] == QPoint(5.5 * BOXLENGTH, 3.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(3, 4, 3, 5)[3] == QPoint(5.5 * BOXLENGTH, 4.5 * BOXLENGTH));

//    // 对map[4][7]与map[5][9]连接的判断
//    QVERIFY(wi.judgeLink(4, 5, 7, 9)[0] == QPoint(7.5 * BOXLENGTH, 4.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 5, 7, 9)[1] == QPoint(7.5 * BOXLENGTH, 4.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 5, 7, 9)[2] == QPoint(7.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 5, 7, 9)[3] == QPoint(9.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//}

////
//// 检测“折两次能连接”的消除情况
//// 依次对应折线形状为：向上凸，向左凸，向下凸，向右凸，（折线中间段在两点之间时的）折线横平与折线竖直
////
//void SimpleTest::caseTurnTwice()
//{
//    // 对map[2][2]与map[2][7]连接的判断，返回数组的四个值中
//    // 第一个值对应map[2][7]方块在地图上的中心点，第二个值对应第一个转折点，
//    // 第三个值对应第二个转折点，第四个值对应map[2][7]方块在地图上的中心点
//    QVERIFY(wi.judgeLink(2, 2, 2, 7)[0] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 7)[1] == QPoint(2.5 * BOXLENGTH, 1.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 7)[2] == QPoint(7.5 * BOXLENGTH, 1.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 2, 2, 7)[3] == QPoint(7.5 * BOXLENGTH, 2.5 * BOXLENGTH));

//    // 对map[2][2]与map[7][3]连接的判断，以下都类似
//    QVERIFY(wi.judgeLink(2, 7, 2, 3)[0] == QPoint(2.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 2, 3)[1] == QPoint(1.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 2, 3)[2] == QPoint(1.5 * BOXLENGTH, 7.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 2, 3)[3] == QPoint(3.5 * BOXLENGTH, 7.5 * BOXLENGTH));

//    QVERIFY(wi.judgeLink(5, 7, 2, 4)[0] == QPoint(2.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(5, 7, 2, 4)[1] == QPoint(2.5 * BOXLENGTH, 8.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(5, 7, 2, 4)[2] == QPoint(4.5 * BOXLENGTH, 8.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(5, 7, 2, 4)[3] == QPoint(4.5 * BOXLENGTH, 7.5 * BOXLENGTH));

//    QVERIFY(wi.judgeLink(2, 7, 7, 4)[0] == QPoint(7.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 7, 4)[1] == QPoint(14.5 * BOXLENGTH, 2.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 7, 4)[2] == QPoint(14.5 * BOXLENGTH, 7.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(2, 7, 7, 4)[3] == QPoint(4.5 * BOXLENGTH, 7.5 * BOXLENGTH));

//    QVERIFY(wi.judgeLink(4, 6, 10, 12)[0] == QPoint(10.5 * BOXLENGTH, 4.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 10, 12)[1] == QPoint(10.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 10, 12)[2] == QPoint(12.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 10, 12)[3] == QPoint(12.5 * BOXLENGTH, 6.5 * BOXLENGTH));

//    QVERIFY(wi.judgeLink(4, 6, 12, 10)[0] == QPoint(12.5 * BOXLENGTH, 4.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 12, 10)[1] == QPoint(12.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 12, 10)[2] == QPoint(10.5 * BOXLENGTH, 5.5 * BOXLENGTH));
//    QVERIFY(wi.judgeLink(4, 6, 12, 10)[3] == QPoint(10.5 * BOXLENGTH, 6.5 * BOXLENGTH));
//}

////
//// 检测无法消除的情况（应当返回点为(0,0)）
////
//void SimpleTest::caseNull()
//{
//    // 方块种类不同
//    QVERIFY(wi.judgeLink(2, 2, 2, 6)[0] == QPoint(0, 0));
//    // 同一个方块，不能自己消除自己
//    QVERIFY(wi.judgeLink(2, 2, 2, 2)[0] == QPoint(0, 0));
//    // 方块种类相同，但是有阻挡，不能通过两次以内折线连接(x,y均不同）
//    QVERIFY(wi.judgeLink(2, 4, 6, 3)[0] == QPoint(0, 0));
//    // 方块种类相同，但是有阻挡，不能通过两次以内折线连接(x相同）
//    QVERIFY(wi.judgeLink(4, 4, 8, 4)[0] == QPoint(0, 0));
//    // 方块种类相同，但是有阻挡，不能通过两次以内折线连接(y相同）
//    QVERIFY(wi.judgeLink(2, 4, 6, 6)[0] == QPoint(0, 0));

//}

//QTEST_MAIN(SimpleTest)
