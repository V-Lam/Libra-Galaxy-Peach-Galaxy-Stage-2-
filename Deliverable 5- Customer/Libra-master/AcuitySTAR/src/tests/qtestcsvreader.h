#include <QtTest/QtTest>

#ifndef TESTINGRM_H
#define TESTINGRM_H


//for testing the CSVReader class.



//please see qtestcsvreader.xml for the testing matrix.


class CSVTest: public QObject
{
    Q_OBJECT
private slots:

    void testloadcsv1();

    void testloadcsv2();

    void testloadcsv3();

    void testloadcsv4();

    void testloadcsv5();

    void testloadcsv6();

    void testloadcsv7();

    void testloadcsv8();

    void testloadcsv9();

    void testloadcsv10();


};

#endif // TESTINGRM_H
