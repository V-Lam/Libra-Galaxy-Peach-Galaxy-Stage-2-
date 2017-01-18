#include <iostream>
#include <stdio.h>
#include <QApplication>
#include <QTreeView>
#include <QSplashScreen>
#include <QTimer>

#include "gui/mainwindow.h"
#include "database/TestCSVReader.h"
#include "test/t2.h"
#include "tests/qtestcsvreader.h"
#include "tests/qtestqsortlistio.h"
//#include "tests/testmainwindow.h"

using namespace std;

class I : public QThread
{
public:
    static void sleep(unsigned long secs) { QThread::sleep(secs); }
};

int main(int argc, char *argv[]) {
    int test = 0;   /*0= test off */

    QApplication app(argc, argv);
    QSplashScreen *splash=new QSplashScreen;
    splash->setPixmap(QPixmap(":/splash.png"));
    splash->show();

    QThread::msleep(2000);

    if (test) {

        //testing records manager
        TestQString *test_obj = new TestQString();
        QTest::qExec(test_obj);

        TestRecordsManager *test_obj2 = new TestRecordsManager();
        QTest::qExec(test_obj2);

        TestRecordsManagers *test_obj3 = new TestRecordsManagers();
        QTest::qExec(test_obj3);
        
        //for testing the csvreader class
         CSVTest *test_obj4 = new CSVTest();
         QTest::qExec(test_obj4);

        //for testing the qsortlistio class
        QSLIO *test_obj1 = new QSLIO();
        QTest::qExec(test_obj1);

    }

    MainWindow w;

//    QTimer::singleShot(2500, splash, SLOT(close()));
//    QTimer::singleShot(2500, &w, SLOT(show()));       //start mainwindow after
//    QTimer::singleShot(2500, w, SLOT(show()));
    w.show();

   // w.show();

//    w.show();

    return app.exec();
}
