#include "customsorttest.h"

//test case 15.1
void CustomSortTest::CustomSortTest1()
{
    CustomSort* sort1 = new CustomSort();
    CustomSort* sort2 = new CustomSort();
    CustomSort* sort3 = new CustomSort();

    std::vector<std::string> MYFIELDLIST1 = {"Member Name", "Date", "Type", "Role", "Title"};
    std::vector<std::string> MYFIELDLIST2 = {"Member Name", "Date", "Type", "Role", "Title", "Extra", "Member Name", "Date", "Type", "Role", "Title"};
    std::vector<std::string> MYFIELDLIST3 = {"Member Name"};
    sort1->setFields(MYFIELDLIST1);
    sort2->setFields(MYFIELDLIST2);
    sort3->setFields(MYFIELDLIST3);
}
