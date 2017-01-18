#ifndef TESTS_H
#define TESTS_H

#include <QtTest/QTest>

class testMainWindow: public QObject
{
    Q_OBJECT
private slots:
    void TEST_01();
    void TEST_02();
    void TEST_03();
    void TEST_04();

};

#endif // TESTS_H

