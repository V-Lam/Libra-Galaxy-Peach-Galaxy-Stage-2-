#include "tests/testMainWindow.h"
#include "gui/mainwindow.h"
#include <QtTest/QTest>

// this is test 10.1
void testMainWindow::TEST_01()
{
    MainWindow mW;
    QString path = "../Project Information/Sample Data/Teaching_sample.csv";
    QCOMPARE(mW.checkFile(0,path),0);
}
// this is test 10.2
void testMainWindow::TEST_02(){
    MainWindow mW;
    QString path = "../Project Information/Sample Data/Publications_sample.csv";
    QCOMPARE(mW.checkFile(0,path),1);
}
// this is test 10.3
void testMainWindow::TEST_03(){
    MainWindow mW;
    QString path = "../Project Information/Sample Data/Classeur1.csv";
    QCOMPARE(mW.checkFile(0,path),1);
}

// this is test 10.4
void testMainWindow::TEST_04(){
    MainWindow mW;
    QString path = "";
    QCOMPARE(mW.checkFile(0,path),1);
}
