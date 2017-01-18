#include <QtTest/QtTest>
#include <string>
#include <vector>
#include <iostream>
#include <QMessageBox>
#include <stdio.h>
#include "database/CSVReader.h"
#include "database/RecordsManager.h"

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

class TestRecordsManager: public QObject
{

    Q_OBJECT
private slots:
    void test();

};
class TestRecordsManagers: public QObject
{

    Q_OBJECT
private slots:
    void test2();

};
