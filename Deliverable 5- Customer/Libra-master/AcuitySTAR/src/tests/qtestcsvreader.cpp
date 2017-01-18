#include "qtestcsvreader.h"
#include "database/CSVReader.h"
#include <iostream>
#include <vector>

using namespace std;
//please see qtestcsvreader.xml for the testing matrix.
//test qtestcsvreader 1.1
    void CSVTest::testloadcsv1()
    {
        CSVReader reader("../Project Information/Sample Data/Grants_expanded.csv");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()== 37);
    }
//test qtestcsvreader 1.2
    void CSVTest::testloadcsv2()
    {
        CSVReader reader("../Project Information/Sample Data/GrantsClinicalFunding_sample.csv");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()==38);
    }
    //test qtestcsvreader 1.3
    void CSVTest::testloadcsv3()
    {
        CSVReader reader("../Project Information/Sample Data/Presentations_expanded.csv");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()==25);
    }
    //test qtestcsvreader 1.4
    void CSVTest::testloadcsv4()
    {
        CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()==30);
    }
    //test qtestcsvreader 1.5
    void CSVTest::testloadcsv5()
    {
        CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()== 27);
    }
    //test qtestcsvreader 1.6
    void CSVTest::testloadcsv6()
    {
        CSVReader reader("blankblank");
        vector<string> header = reader.getHeaders();
        QVERIFY(header.size()== 0);
    }
//test qtestcsvreader 1.7
    void CSVTest::testloadcsv7()
    {
        CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
        vector< vector<string> > all_data = reader.getData();
        QVERIFY(all_data.size()== 140);
        //QCOMPARE((all_data.size()),140);
    }
 //test qtestcsvreader 1.8
    void CSVTest::testloadcsv8()
    {
        CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
        vector< vector<string> > all_data = reader.getData();
        QVERIFY(all_data.size()!= 13574);
    }
    //test qtestcsvreader 1.9
    void CSVTest::testloadcsv9()
    {
        CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
        string qtestdatestring= "1997/10";
        QCOMPARE(reader.parseDateString(qtestdatestring.c_str()), 1997);
    }
//test qtestcsvreader 1.10
    void CSVTest::testloadcsv10()
    {
        CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
        string qtestdatestring= "10/1997";
        QVERIFY(reader.parseDateString(qtestdatestring.c_str())!= 1997);
    }




