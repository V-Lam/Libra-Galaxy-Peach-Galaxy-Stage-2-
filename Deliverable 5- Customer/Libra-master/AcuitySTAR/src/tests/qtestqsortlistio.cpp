#include "qtestqsortlistio.h"
#include "database/QSortListIO.h"
#include <iostream>
#include <QStringList>

using namespace std;
//please see qtestqsortlistio.xml for the testing matrix.
//test qtestqsortlistio 1
    void QSLIO::testqslio1()
    {
        QList<QStringList> list1;
        QStringList slist1,slist2,slist3;
        slist1.append("kedar");
        slist2.append("alex");
        slist3.append("rohan");
        list1.append(slist1);
        list1.append(slist2);
        list1.append(slist3);
        QSortListIO temp("temp.txt");
        temp.saveList(list1);
        QVERIFY(temp.readList()==list1);
    }
