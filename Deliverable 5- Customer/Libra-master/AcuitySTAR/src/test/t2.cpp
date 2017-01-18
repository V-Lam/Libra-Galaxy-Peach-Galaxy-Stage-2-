#include "t2.h"

void TestQString::toUpper()
{
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}

/**
 * @brief TestRecordsManager::test
 * Testing if records mamanger recognizes recorgnizes the corrent headers
 */
void TestRecordsManager::test()
{
    std::vector<std::string> headers[4];    //A list of headers to be read in from the file
    CSVReader *readers[4];

    //Array of files to be read in
    std::string files[] = {"../Project Information/Sample Data/Teaching_sample.csv",
                           "../Project Information/Sample Data/Presentations_sample.csv",
                           "../Project Information/Sample Data/Publications_sample.csv",
                           "../Project Information/Sample Data/GrantsClinicalFunding_sample.csv"};

    //Array of headers expected for each file type
    std::string headercheck[4][7] = {
        {"Member Name", "Start Date", "Program"},
        {"Member Name", "Date", "Type", "Role", "Title"},
        {"Member Name", "Type", "Status Date", "Role", "Title"},
        {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"}

    };
    int len[] = {3,5,5,7}; //number of headers expected smple type

    //char s[10];
    for(int i = 0; i < 4; i++){
        readers[i] = new CSVReader(files[i]);   //reading in the files
        headers[i] = readers[i]->getHeaders();  //getting the headers from the files
        for(int x = 0; x < len[i]; x++){
            //sprintf(s, "%d %d", i, x);
            //Checking if all thet required headers are found after being read in


            QVERIFY(std::find(headers[i].begin(), headers[i].end(), headercheck[i][x]) != headers[i].end());
        }
    }
}

/**
 * @brief TestRecordsManager::test
 * Testing if records mamanger recognizes recorgnizes the corrent headers
 */
void TestRecordsManagers::test2()
{
    std::vector<std::string> headers[4];    //A list of headers to be read in from the file
    CSVReader *readers[4];

    //Array of files to be read in
    std::string files[] = {
                           "../Project Information/Sample Data/Presentations_sample.csv",
                           "../Project Information/Sample Data/Teaching_sample.csv",
                           "../Project Information/Sample Data/GrantsClinicalFunding_sample.csv",
                           "../Project Information/Sample Data/Publications_sample.csv",};

    //Array of headers expected for each file type
    std::string headercheck[4] = {"Program", "Date", "Status Date", "Funding Type"};
    char s[10];
    for(int i = 0; i < 4; i++){
        readers[i] = new CSVReader(files[i]);   //reading in the files
        headers[i] = readers[i]->getHeaders();  //getting the headers from the files

        //sprintf(s, "%d %d", i, x);
        //Checking if all thet required headers are found after being read in
        QVERIFY2(std::find(headers[i].begin(), headers[i].end(), headercheck[i]) == headers[i].end(), s);

    }
}

