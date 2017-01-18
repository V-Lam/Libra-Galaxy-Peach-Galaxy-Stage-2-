#include <QDate>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>

#include <QPrintDialog>
#include <QPrinter>
#include <QString>
#include <QSortFilterProxyModel>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QLineSeries>

#include <iostream>
#include <fstream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CustomSort.h"
#include "ErrorEditDialog.h"

#include "database/CSVReader.h"
#include "database/QSortListIO.h"
#include "database/RecordsManager.h"
#include "datamodel/TreeModel.h"
#include "datamodel/GrantFundingTreeModel.h"
#include "datamodel/PresentationTreeModel.h"
#include "datamodel/PublicationTreeModel.h"
#include "datamodel/TeachingTreeModel.h"

#define FUNDORDER_SAVE  "fundsortorder.dat"
#define PRESORDER_SAVE  "pressortorder.dat"
#define PUBORDER_SAVE   "pubsortorder.dat"
#define TEACHORDER_SAVE "teachsortorder.dat"

//<<<<<<< HEAD
//std::vector<std::string> MainWindow::GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
//std::vector<std::string> MainWindow::PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
//std::vector<std::string> MainWindow::PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
//std::vector<std::string> MainWindow::TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};
//=======
#define TEACHFILE  "teachdata.csv"
#define PUBFILE  "pubdata.csv"
#define PRESFILE  "presdata.csv"
#define FUNDFILE  "funddata.csv"

std::vector<std::string> MainWindow::GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date", "Division"};
std::vector<std::string> MainWindow::PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title", "Division"};
std::vector<std::string> MainWindow::PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title", "Division"};
std::vector<std::string> MainWindow::TEACH_MANFIELDS = {"Member Name", "Start Date", "Program", "Division"};
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    fundTree(NULL), presTree(NULL), pubTree(NULL), teachTree(NULL),
    funddb(NULL), presdb(NULL), pubdb(NULL), teachdb(NULL) {
    // draw GUI elements
    ui->setupUi(this);

    this->showMaximized();

    // set up the logo
    QPixmap logo(":/logo.png");
    ui->logo_label->setPixmap(logo);

    // set up application icon
    QIcon icon(":/icon32.ico");
    MainWindow::setWindowIcon(icon);

    // set the date format
    QString dFormat("yyyy");
    ui->FromDate->setDisplayFormat(dFormat);
    ui->ToDate->setDisplayFormat(dFormat);

    // set start year to be 1950
    QDate startDate(1950, 1, 1);

    // set ranges for spinbox
    ui->FromDate->setDateRange(startDate, QDate::currentDate());
    ui->ToDate->setDateRange(startDate, QDate::currentDate());

    // set default dates (1950->current)
    ui->FromDate->setDate(startDate);
    ui->ToDate->setDate(QDate::currentDate());

    // set some member variables to the current date values
    yearStart = 1950;
    yearEnd = QDate::currentDate().year();

    //default print is disabled
    ui->teachPrintButton->setEnabled(false);
    ui->fundPrintButton->setEnabled(false);
    ui->pubPrintButton->setEnabled(false);
    ui->presPrintButton->setEnabled(false);

    ui->teachExportButton->setEnabled(false);
    ui->fundExportButton->setEnabled(false);
    ui->pubExportButton->setEnabled(false);
    ui->presExportButton->setEnabled(false);

    //Setup printer
    printer = new QPrinter();

    dateChanged = {false, false, false, false};

    //userSelectList buttons initial disable
    ui->teachingAddIndex->setEnabled(false);
        ui->teachingUndoIndex->setEnabled(false);
        ui->teachingRemoveIndex->setEnabled(false);
        ui->teachingClearList->setEnabled(false);
        ui->teachingShowList->setEnabled(false);
        ui->teachingHideList->setEnabled(false);

        ui->publicationsAddIndex->setEnabled(false);
        ui->publicationsUndoIndex->setEnabled(false);
        ui->publicationsRemoveIndex->setEnabled(false);
        ui->publicationsClearList->setEnabled(false);
        ui->publicationsShowList->setEnabled(false);
        ui->publicationsHideList->setEnabled(false);

        ui->presentationsAddIndex->setEnabled(false);
        ui->presentationsUndoIndex->setEnabled(false);
        ui->presentationsRemoveIndex->setEnabled(false);
        ui->presentationsClearList->setEnabled(false);
        ui->presentationsShowList->setEnabled(false);
        ui->presentationsHideList->setEnabled(false);

        ui->fundsAddIndex->setEnabled(false);
        ui->fundsUndoIndex->setEnabled(false);
        ui->fundsRemoveIndex->setEnabled(false);
        ui->fundsClearList->setEnabled(false);
        ui->fundsShowList->setEnabled(false);
        ui->fundsHideList->setEnabled(false);

    teachclosable = false;
    presclosable = false;
    pubclosable = false;
    fundclosable =  false;


    //insert dialog box to ask users if they want continue from where they left off last time
    QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Would you like to continue form where you left off?\n"),
                                                               QMessageBox::Yes | QMessageBox::No);
    if (rstBtn == QMessageBox::Yes) {
        std::string filelist[4] = {TEACHFILE, PRESFILE, PUBFILE, FUNDFILE};
        for (int i = 0; i < 4; i++) {
            std::ifstream infile(filelist[i]);
            if (!infile.is_open()) {
                std::ofstream outfile(filelist[i]);
                outfile.close();
            } else {
                if (i == 0) {
                    load_teach(filelist[i].c_str(), true);
                } else if (i == 1) {
                    load_pres(filelist[i].c_str(), true);
                } else if (i == 2) {
                    load_pub(filelist[i].c_str(), true);
                } else {
                    load_fund(filelist[i].c_str(), true);
                }
                infile.close();
            }
        }
    }else{

    }

    std::cout << ui->teach_delete_sort->isEnabled();

}

MainWindow::~MainWindow() {
    delete ui;

    delete fundTree;
    delete presTree;
    delete pubTree;
    delete teachTree;

    delete funddb;
    delete presdb;
    delete pubdb;
    delete teachdb;
    delete printer;
}

void MainWindow::on_actionLoad_file_triggered() {
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          "Select one or more files to load",
                                                          QDir::currentPath(),
                                                          tr("CSV (*.csv);; All files (*.*)"));
    if (filePaths.size() > 0) {
        const int NUM_TABS = 4;
        bool all_loaded[NUM_TABS] = {false, false, false, false};
        int sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
        QStringList::Iterator it = filePaths.begin();
        while (sum != NUM_TABS && it != filePaths.end()) {
            QString path = it[0];
            //note short-circuit eval
            if (!all_loaded[FUNDING] && load_fund(path, true)) {
                all_loaded[FUNDING] = true;
            } else if (!all_loaded[TEACH] && load_teach(path, true)) {
                all_loaded[TEACH] = true;
            } else if (!all_loaded[PUBLICATIONS] && load_pub(path, true)) {
                all_loaded[PUBLICATIONS] = true;
            } else if (!all_loaded[PRESENTATIONS] && load_pres(path, true)) {
                all_loaded[PRESENTATIONS] = true;
            }
            sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
            ++it;
        }
    }
}


/*
 * When the load file button is clicked a file open dialog box
 * is opened.  If a file name is successfully returned, makeTree()
 * is called.
 */
QString MainWindow::load_file() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(),
                                                    tr("CSV (*.csv);; All files (*.*)"));

    if (!filePath.isEmpty()) {
        return filePath;
    } else {
        return "";
    }
}

void MainWindow::refresh(int tabIndex) {
    // if we've loaded in a file, update that data
    switch (tabIndex) {
    case FUNDING:
        if (!fundPath.isEmpty()) {
            makeTree(FUNDING);
            ui->fund_graph_stackedWidget->hide();
            ui->fundGraphTitle->clear();
        }
        break;

    case PRESENTATIONS:
        if (!presPath.isEmpty()) {
            makeTree(PRESENTATIONS);
            ui->pres_graph_stackedWidget->hide();
            ui->presGraphTitle->clear();
        }
        break;
    case PUBLICATIONS:
        if (!pubPath.isEmpty()) {
            makeTree(PUBLICATIONS);
            ui->pub_graph_stackedWidget->hide();
            ui->pubGraphTitle->clear();
        }
        break;

    case TEACH:
        if (!teachPath.isEmpty()) {
            makeTree(TEACH);
            ui->teach_graph_stackedWidget->hide();
            ui->teachGraphTitle->clear();
        }
        break;
    }
}

int MainWindow::checkFile(int index, QString filePath) {

    CSVReader reader;
    std::vector<std::string> header;
    std::string searchstring;

    int sortHeaderIndex = 2;

    switch (index) {

    case TEACH:
        // read if first or if a new file is loaded
        if (teachPath.isEmpty() || (!teachPath.isEmpty() && filePath.compare(teachPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete teachdb;
            teachdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Program";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = teachdb->getHeaderIndex("Start Date");
                teachData = reader.getData();
                unsigned int j;
                for (int i = 0; i < (int) teachData.size(); i++) {
                    for (j = 0; j < TEACH_MANFIELDS.size(); j++) {
                        int index = teachdb->getHeaderIndex(TEACH_MANFIELDS[j]);
                        if (index >= teachData[i].size() || teachData[i][index].compare("") == 0) {
                            f_errs_teach.push_back(&teachData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == TEACH_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = teachdb->getHeaderIndex("Start Date");
                        int year;
                        sscanf(teachData[i][yrIndex].c_str(), "%4d", &year);
                        teachData[i][yrIndex] = std::to_string(year);

                        teachdb->addRecord(reader.parseDateString(teachData[i][sortHeaderIndex]), &teachData[i]);
                    }
                }

                if (f_errs_teach.size() > 0) {
                    if(handle_field_errors(f_errs_teach, f_errs_teach_fixed, header, TEACH_MANFIELDS)) {
                        for (unsigned int i = 0; i < f_errs_teach_fixed.size(); i++) {
                            teachdb->addRecord(reader.parseDateString((*(f_errs_teach_fixed[i]))[sortHeaderIndex]), f_errs_teach_fixed[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->teachPrintButton->setEnabled(true);
        ui->teachExportButton->setEnabled(true);
        break;

    case PUBLICATIONS:
        // read if first or if a new file is loaded
        if (presPath.isEmpty() || (!presPath.isEmpty() && filePath.compare(presPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete pubdb;
            pubdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Publication Status";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = pubdb->getHeaderIndex("Status Date");
                pubData = reader.getData();
                unsigned int j;
                for (int i = 0; i < (int) pubData.size(); i++) {
                    for (j = 0; j < PUBS_MANFIELDS.size(); j++) {
                        int index = pubdb->getHeaderIndex(PUBS_MANFIELDS[j]);
                        if (index >= pubData[i].size() || pubData[i][index].compare("") == 0) {
                            f_errs_pub.push_back(&pubData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == PUBS_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = pubdb->getHeaderIndex("Status Date");
                        int year;
                        sscanf(pubData[i][yrIndex].c_str(), "%4d", &year);
                        pubData[i][yrIndex] = std::to_string(year);

                        pubdb->addRecord(reader.parseDateString(pubData[i][sortHeaderIndex]), &pubData[i]);
                    }
                }

                if (f_errs_pub.size() > 0) {
                    if(handle_field_errors(f_errs_pub, f_errs_pub_fixed, header, PUBS_MANFIELDS)) {
                        for (unsigned int i = 0; i < f_errs_pub_fixed.size(); i++) {
                            pubdb->addRecord(reader.parseDateString((*(f_errs_pub_fixed[i]))[sortHeaderIndex]), f_errs_pub_fixed[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->pubPrintButton->setEnabled(true);
        ui->pubExportButton->setEnabled(true);
        break;

    case PRESENTATIONS:
        // read if first or if a new file is loaded
        if (presPath.isEmpty() || (!presPath.isEmpty() && filePath.compare(presPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete presdb;
            presdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Activity Type";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = presdb->getHeaderIndex("Date");
                presData = reader.getData();
                unsigned int j = 0;
                for (int i = 0; i < (int) presData.size(); i++) {
                    //check through mandatory fields for empty fields
                    for (j = 0; j < PRES_MANFIELDS.size(); j++) {
                        int index = presdb->getHeaderIndex(PRES_MANFIELDS[j]);
                        if (index >= presData[i].size() || presData[i][index].compare("") == 0) {
                            f_errs_pres.push_back(&presData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == PRES_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = presdb->getHeaderIndex("Date");
                        int year;
                        sscanf(presData[i][yrIndex].c_str(), "%4d", &year);
                        presData[i][yrIndex] = std::to_string(year);

                        presdb->addRecord(reader.parseDateString(presData[i][sortHeaderIndex]), &presData[i]);
                    }
                }

                if (f_errs_pres.size() > 0) {
                    if(handle_field_errors(f_errs_pres, f_errs_pres_fixed, header, PRES_MANFIELDS)) {
                        for (unsigned int i = 0; i < f_errs_pres_fixed.size(); i++) {
                            presdb->addRecord(reader.parseDateString((*(f_errs_pres_fixed[i]))[sortHeaderIndex]), f_errs_pres_fixed[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->presPrintButton->setEnabled(true);
        ui->presExportButton->setEnabled(true);
        break;

    case FUNDING:
        // read if first or if a new file is loaded
        if (fundPath.isEmpty() || (!fundPath.isEmpty() && filePath.compare(fundPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete funddb;
            funddb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Funding Type";

            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = funddb->getHeaderIndex("Start Date");
                fundData = reader.getData();
                unsigned int j;
                for (int i = 0; i < (int) fundData.size(); i++) {
                    for (j = 0; j < GRANTS_MANFIELDS.size(); j++) {
                        int index = funddb->getHeaderIndex(GRANTS_MANFIELDS[j]);
                        if (index >= fundData[i].size() || fundData[i][index].compare("") == 0) {
                            f_errs_fund.push_back(&fundData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == GRANTS_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = funddb->getHeaderIndex("Start Date");
                        int year;
                        sscanf(fundData[i][yrIndex].c_str(), "%4d", &year);
                        fundData[i][yrIndex] = std::to_string(year);

                        // boolean interpretation
                        int prIndex = funddb->getHeaderIndex("Peer Reviewed?");
                        if (fundData[i][prIndex].compare("True") == 0) {
                            fundData[i][prIndex] = "Peer Reviewed";
                        } else {
                            fundData[i][prIndex] = "Not Peer Reviewed";
                        }
                        funddb->addRecord(reader.parseDateString(fundData[i][sortHeaderIndex]), &fundData[i]);
                    }
                }
                if (f_errs_fund.size() > 0) {
                    if(handle_field_errors(f_errs_fund, f_errs_fund_fixed, header, GRANTS_MANFIELDS)) {
                        for (unsigned int i = 0; i < f_errs_fund_fixed.size(); i++) {
                            funddb->addRecord(reader.parseDateString((*(f_errs_fund_fixed[i]))[sortHeaderIndex]), f_errs_fund_fixed[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->fundPrintButton->setEnabled(true);
        ui->fundExportButton->setEnabled(true);
        break;
    }
    return EXIT_SUCCESS;
}

/**
 * State saving
 * Saving all the teaching data in a specific tab to a file
 * @param filenme the name of the file that the data will be written to
 * If filenme is "default" write teh data out to the temp state file.
 */
void MainWindow::saveteachdb(QString filenme){

    std::ofstream outfile;
    if (filenme == "default"){
        outfile.open(TEACHFILE);
    }else{
        outfile.open(filenme.toStdString());
    }
    Manager tempmanager;

    BasicRecord headers = teachdb->getHeaders();
    for(BasicRecord::iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it == headers.end() - 1) {
            outfile << '"' << *it << '"' << "\n";
        } else {
            outfile << '"' << *it << '"' << ",";
        }
    }
    tempmanager = teachdb->getManager();
    for (Manager::iterator it = tempmanager.begin(); it != tempmanager.end();++it){
        for(BasicRecord::iterator it2 = it->second->begin(); it2 != it->second->end(); ++it2) {
            if (it2 == it->second->end() - 1) {
                outfile << '"' << *it2 << '"' << "\n";
            } else {
                outfile << '"' << *it2 << '"' << ",";
            }
        }
    }
    outfile.close();
    disableteachview();

}
/**
 * Disbaling the buttons on the teaching tab when closing a file
 */
void MainWindow::disableteachview(){

    ui->teachTreeView->setVisible(false);
    ui->teach_file_label->setText("No file chosen.");
    ui->teach_sort->setEnabled(false);
    ui->teach_delete_sort->setEnabled(false);
    ui->teach_new_sort->setEnabled(false);
    ui->teach_filter_from->setEnabled(false);
    ui->teach_filter_to->setEnabled(false);
    ui->teach_pie_button->setEnabled(false);
    ui->teach_bar_button->setEnabled(false);
    ui->teach_to_label->setEnabled(false);
    ui->teach_sort_label->setEnabled(false);
    ui->teach_filter->setEnabled(false);
    ui->teach_filter_label->setEnabled(false);


}

/**
 * State saving
 * Saving all the publishing data in a specific tab to a file
 * @param filenme the name of the file that the data will be written to
 * If filenme is "default" write teh data out to the temp state file.
 */
void MainWindow::savepubdb(QString filenme){
    std::ofstream outfile;
    if (!filenme.isEmpty()) {
        if (filenme == "default") {
            outfile.open(PUBFILE);
        } else {
            outfile.open(filenme.toStdString());
        }

        Manager tempmanager;

        BasicRecord headers = pubdb->getHeaders();
        for (BasicRecord::iterator it = headers.begin(); it != headers.end(); ++it) {
            if (it == headers.end() - 1) {
                outfile << '"' << *it << '"' << "\n";
            } else {
                outfile << '"' << *it << '"' << ",";
            }
        }
        tempmanager = pubdb->getManager();
        for (Manager::iterator it = tempmanager.begin(); it != tempmanager.end(); ++it) {
            for (BasicRecord::iterator it2 = it->second->begin(); it2 != it->second->end(); ++it2) {
                if (it2 == it->second->end() - 1) {
                    outfile << '"' << *it2 << '"' << "\n";
                } else {
                    outfile << '"' << *it2 << '"' << ",";
                }
            }
        }
        outfile.close();
        disablepubview();
    }
}

/**
 * Disbaling the buttons on the teaching tab when closing a file
 */
void MainWindow::disablepubview(){

    ui->pubTreeView->setVisible(false);
    ui->pub_file_label->setText("No file chosen.");
    ui->pub_sort->setEnabled(false);
    ui->pub_delete_sort->setEnabled(false);
    ui->pub_new_sort->setEnabled(false);
    ui->pub_filter_from->setEnabled(false);
    ui->pub_filter_to->setEnabled(false);
    ui->pub_pie_button->setEnabled(false);
    ui->pub_bar_button->setEnabled(false);
    ui->pub_to_label->setEnabled(false);
    ui->pub_sort_label->setEnabled(false);
    ui->pub_filter->setEnabled(false);
    ui->pub_filter_label->setEnabled(false);

}

/**
* State saving
* Saving all the presentation data in a specific tab to a file
* @param filenme the name of the file that the data will be written to
* If filenme is "default" write teh data out to the temp state file.
*/
void MainWindow::savepresdb(QString filenme){
    std::ofstream outfile;
    if (filenme == "default"){
        outfile.open(PRESFILE);
    }else{
        outfile.open(filenme.toStdString());
    }
    Manager tempmanager;

    BasicRecord headers = presdb->getHeaders();
    for(BasicRecord::iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it == headers.end() - 1) {
            outfile << '"' << *it << '"' << "\n";
        } else {
            outfile << '"' << *it << '"' << ",";
        }
    }
    tempmanager = presdb->getManager();
    for (Manager::iterator it = tempmanager.begin(); it != tempmanager.end();++it){
        for(BasicRecord::iterator it2 = it->second->begin(); it2 != it->second->end(); ++it2) {
            if (it2 == it->second->end() - 1) {
                outfile << '"' << *it2 << '"' << "\n";
            } else {
                outfile << '"' << *it2 << '"' << ",";
            }
        }
    }
    outfile.close();
    disablepresview();
}

/**
 * Disbaling the buttons on the teaching tab when closing a file
 */
void MainWindow::disablepresview(){

    ui->presTreeView->setVisible(false);
    ui->pres_file_label->setText("No file chosen.");
    ui->pres_sort->setEnabled(false);
    ui->pres_delete_sort->setEnabled(false);
    ui->pres_new_sort->setEnabled(false);
    ui->pres_filter_from->setEnabled(false);
    ui->pres_filter_to->setEnabled(false);
    ui->pres_pie_button->setEnabled(false);
    ui->pres_bar_button->setEnabled(false);
    ui->pres_to_label->setEnabled(false);
    ui->pres_sort_label->setEnabled(false);
    ui->pres_filter->setEnabled(false);
    ui->pres_filter_label->setEnabled(false);

}

/**
 * State saving
 * Saving all the Grants and funding data in a specific tab to a file
 * @param filenme the name of the file that the data will be written to
 * If filenme is "default" write teh data out to the temp state file.
 */
void MainWindow::savefunddb(QString filenme){
    std::ofstream outfile;
    if (filenme == "default"){
        outfile.open(FUNDFILE);
    }else{
        outfile.open(filenme.toStdString());
    }
    Manager tempmanager;

    BasicRecord headers = funddb->getHeaders();
    for(BasicRecord::iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it == headers.end() - 1) {
            outfile << '"' << *it << '"' << "\n";
        } else {
            outfile << '"' << *it << '"' << ",";
        }
    }
    tempmanager = funddb->getManager();
    for (Manager::iterator it = tempmanager.begin(); it != tempmanager.end();++it){
        for(BasicRecord::iterator it2 = it->second->begin(); it2 != it->second->end(); ++it2) {
            if (it2 == it->second->end() - 1) {
                outfile << '"' << *it2 << '"' << "\n";
            } else {
                outfile << '"' << *it2 << '"' << ",";
            }
        }
    }
    outfile.close();
    disablefundview();

}

/**
 * Disbaling the buttons on the grants and funding tab when closing a file
 */
void MainWindow::disablefundview(){

    ui->fundTreeView->setVisible(false);
    ui->fund_file_label->setText("No file chosen.");
    ui->fund_sort->setEnabled(false);
    ui->fund_delete_sort->setEnabled(false);
    ui->fund_new_sort->setEnabled(false);
    ui->fund_filter_from->setEnabled(false);
    ui->fund_filter_to->setEnabled(false);
    ui->fund_pie_button->setEnabled(false);
    ui->fund_bar_button->setEnabled(false);
    ui->fund_to_label->setEnabled(false);
    ui->fund_sort_label->setEnabled(false);
    ui->fund_filter->setEnabled(false);
    ui->fund_filter_label->setEnabled(false);

}

void MainWindow::createDefaultSortOrder(int tabIndex) {
    QStringList defaultOrder;
    defaultOrder << "Default";

    switch (tabIndex) {
    case FUNDING:
        // specify default sort order
        defaultOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";

        // add default list to member variable
        allFundOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveFundSort(FUNDORDER_SAVE);
        saveFundSort.saveList(allFundOrders);
    }

        break;
    case PRESENTATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPresOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePresSort(PRESORDER_SAVE);
        savePresSort.saveList(allPresOrders);
    }

        break;
    case PUBLICATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPubOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePubSort(PUBORDER_SAVE);
        savePubSort.saveList(allPubOrders);
    }

        break;
    case TEACH:
        // specify default sort order
        defaultOrder << "Member Name" << "Program";

        // add default list to member variable
        allTeachOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveTeachSort(FUNDORDER_SAVE);
        saveTeachSort.saveList(allFundOrders);
    }

        break;
    }
}

/*
 * err: vector of pointers to record entries.
 * headers: vector of strings containing the db headers
 * mandatory: vector of strings containing the mandatory fields in the db
 *
 * Returns true when the passed err vector contains edited fields.
 * Returns false when no fields have been edited.
 */
bool MainWindow::handle_field_errors(std::vector<std::vector<std::string>*>& err,
                                     std::vector<std::vector<std::string>*>& fixed_err,
                                     std::vector<std::string>& headers,
                                     std::vector<std::string>& mandatory) {
    //Since CSVReader alldata contains completely empty records
    //remove them first.
    std::vector<std::vector<std::string>*>::iterator it;
    for (it = err.begin(); it != err.end(); it++) {
        bool allEmpty = true;
        for (int col = 0; col < (int) (*it)->size(); col++) {
            if ((*it)->at(col).compare("") != 0) {
                allEmpty = false;
            }
        }
        if (allEmpty) {
            it = err.erase(it);
            it--;
        }
    }
    //Return false; there are no errors to correct
    if (err.size() == 0) {
        return false;
    }
    QMessageBox prompt;
    QString mainText = "File contains ";
    mainText.append(QString::number(err.size()));
    mainText.append(" records with missing mandatory fields.");
    prompt.setText(mainText);
    prompt.setInformativeText("Do you want to edit these entries or discard?");
    prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    prompt.setDefaultButton(QMessageBox::Yes);
    prompt.setButtonText(QMessageBox::Yes, "Edit");
    prompt.setButtonText(QMessageBox::No, "Discard");
    prompt.setWindowIcon(QIcon(":/icon32.ico"));
    int ret = prompt.exec();

    switch (ret) {
    case QMessageBox::Yes: {
        ErrorEditDialog diag(this, err, fixed_err, headers, mandatory);
        if(diag.exec()) {
            return true;
        }
        return false;
    }

    case QMessageBox::No:
    default:
        return false;
    }
}

/*
 * Builds a TreeModel for the ui's embedded treeview.
 */
void MainWindow::makeTree(int tabIndex) {
    switch (tabIndex) {
    case TEACH:
        // set up some member variables to point to the current data in use
        currentdb = teachdb;
        delete teachTree;
        teachTree = new TeachingTreeModel(currentdb);
        currentTree = teachTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->teachTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, teachSortOrder, getFilterStartChar(TEACH), getFilterEndChar(TEACH));

        ui->teach_pie_button->toggle();

        break;

    case PUBLICATIONS:
        // set up some member variables to point to the current data in use
        currentdb = pubdb;
        delete pubTree;
        pubTree = new PublicationTreeModel(currentdb);
        currentTree = pubTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->pubTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, pubSortOrder, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

        ui->pub_pie_button->toggle();

        break;
    case PRESENTATIONS:
        // set up some member variables to point to the current data in use
        currentdb = presdb;
        delete presTree;

        presTree = new PresentationTreeModel(currentdb);
        currentTree = presTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->presTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, presSortOrder, getFilterStartChar(PRESENTATIONS), getFilterEndChar(PRESENTATIONS));

        ui->pres_pie_button->toggle();

        break;

    case FUNDING:
        // set up some member variables to point to the current data in use
        currentdb = funddb;
        delete fundTree;
        fundTree = new GrantFundingTreeModel(currentdb);
        currentTree = fundTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->fundTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, fundSortOrder, getFilterStartChar(FUNDING), getFilterEndChar(FUNDING));

        ui->fund_pie_button->toggle();

        break;
    }


    // set up the tree in our UI
    //includes the ability to sort by a user selected list. (ApplePicker Mp4)
    QSortFilterProxyModel *sortWrapTreeView = new QSortFilterProxyModel(currentView);
    sortWrapTreeView->setSourceModel(currentTree);
    sortWrapTreeView->setSortCaseSensitivity(Qt::CaseInsensitive);      //ignoring case sensitivity: makes Strings sorted based on letter as opposed to specific ASCII code of an Upper/Lower case letter
    currentView->setModel(sortWrapTreeView);
    currentView->header()->setSortIndicator(0, Qt::AscendingOrder);     //makes default header sort to Ascending Order in the Names Column
    currentView->setSortingEnabled(true);                               //makes headers clickable. on click it changes whether it's ascending/descending sorting.

            //currentView->setModel(currentTree);       //this line is the old peach galaxy code. ignore it


    // set resize property to stretch
    currentView->header()->resizeSections(QHeaderView::Stretch);
}

void MainWindow::setupPieChart(PieChartWidget* pieChart, QListWidget *pieListWidget, std::vector<std::pair <std::string, double>> pieChartList) {
    // draws the pie graph by sending piechartwidget a vector of name, presentation count
    int pieSize = (int) pieChartList.size();
    QVector<QColor> colorList(pieSize);
    int r, g, b = 0; //rgb values

    QVector<QColor> colorSelection(pieSize); //Creates a QVector titles colorSelection

    pieListWidget->clear();
    for (int i = 0; i < pieSize; i++) { //For each element, up to the number of elements in the pie graph
        for (int i = 0; i < pieSize; i++)
        {
            //The rgb values are determined in a weird, basically random way. However they are based on i, so that the pie charts will always draw from the same colour pool
            if((i % 2) == 1)
            {
                //r = 35 * ((2 * i) / 2);
                r = 40 * (i) + i;
                g = 140 - (20 * i);
                b = 25 + (15 * i);

                //In case of RGB values exceeding being OOB
                if(b < 1 || b > 255)
                {
                    b = 25;
                }
                if(g < 0)
                {
                    g = 25 + (3 * i);
                }
                if(r > 255)
                {
                    r = 25 + (20 * (i) / 3) + i;
                }
            }

            if((i % 2) == 0)
            {
                r = 50 * (i) + i;
                g = 25 + (15 * i);
                b = 140 - (20 * i);

                //In case of RGB values exceeding OOB
                if(b < 1 || b > 255)
                {
                    b = 25;
                }
                if(g > 110) // will happen after 6th roll
                {
                    g = 255 - (12 * i);
                }
                if(r > 255)
                {
                    r = 25 + (20 * (i) / 3) + i;
                }
            }

            colorSelection[i] = (QColor(r, g, b));
        }

        colorList[i] = (QColor(colorSelection[i])); //Instead of assigning random colours each time, assign fixed colours from the second vector
        pieListWidget->addItem(QString::fromStdString(pieChartList[i].first));

        // set legend colors
        QPixmap pixmap(100, 100);
        pixmap.fill(QColor(colorList[i]));
        QIcon tempIcon(pixmap);
        pieListWidget->item(i)->setIcon(tempIcon);
    }

    pieChart->setData(pieChartList, colorList); //passes vector list to piechartwidget
}

void MainWindow::setupLineChart(QWidget *lineChartWidget, std::vector<std::pair<QString, std::vector<double>>> lineChartList) {
    QtCharts::QChart *chart = new QtCharts::QChart();

    int lineSize = (int)lineChartList.size();

    for (int i = 0; i < lineSize; i++) {
        std::pair<QString, std::vector<double>> tmpPair = lineChartList[i];
        QtCharts::QLineSeries *tmpSeries = new QtCharts::QLineSeries();

        int pairSize = (int)tmpPair.second.size();

        for (int j = 0; j < pairSize; j++) {
            int tmpValue = (int)(tmpPair.second[j]);

            tmpSeries->append(j, tmpValue);
        }

        chart->addSeries(tmpSeries);
    }

    chart->legend()->hide();
    chart->createDefaultAxes();
//    chart->setTitle("Simple line chart example");

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart, lineChartWidget);
    chartView->setRenderHint(QPainter::Antialiasing);

    QSize widgetSize = lineChartWidget->size();

    chartView->resize(widgetSize.width() - 100, widgetSize.height() - 100);

    qInfo() << "After resizing window";
}

void MainWindow::setupStackedBarChart(QWidget *stackedBarChartWidget, std::vector<QString> barCategories, std::vector<std::pair<QString, std::vector<double>>> barSetList) {

    QtCharts::QHorizontalStackedBarSeries *series = new QtCharts::QHorizontalStackedBarSeries();

    qInfo() << "Beginning of setup stacked bar chart";


    int barSize = (int)barSetList.size();

    for (int i = 0; i < barSize; i++) {
        std::pair<QString, std::vector<double>> tmpPair = barSetList[i];
        QtCharts::QBarSet *tmpSet = new QtCharts::QBarSet(tmpPair.first);

        int pairSize = (int)tmpPair.second.size();

        for (int j = 0; j < pairSize; j++) {
            double tmpValue = tmpPair.second[j];

            *tmpSet << tmpValue;
        }

        series->append(tmpSet);
    }

    qInfo() << "After added sets to series";

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    qInfo() << "After setting series";
//    chart->setTitle("Test title");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    qInfo() << "Before creating categories";

    QStringList categories;

    int barCategorySize = (int)barCategories.size();

    for (int i = 0; i < barCategorySize; i++) {
        categories << barCategories[i];
    }

    qInfo() << "After adding categories";

    QtCharts::QBarCategoryAxis *axis = new QtCharts::QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisY(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    qInfo() << "After setting axis and legend";

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart, stackedBarChartWidget);
    chartView->setRenderHint(QPainter::Antialiasing);

    qInfo() << "After creating chart view";

//    chartView->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    QSize widgetSize = stackedBarChartWidget->size();

    chartView->resize(widgetSize.width() - 100, widgetSize.height() - 100);

    qInfo() << "After resizing window";


//    QtCharts::QBarSet *set0 = new QtCharts::QBarSet("Jane");
//    QtCharts::QBarSet *set1 = new QtCharts::QBarSet("John");
//    QtCharts::QBarSet *set2 = new QtCharts::QBarSet("Axel");
//    QtCharts::QBarSet *set3 = new QtCharts::QBarSet("Mary");
//    QtCharts::QBarSet *set4 = new QtCharts::QBarSet("Samantha");

//    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
//    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
//    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
//    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
//    *set4 << 9 << 7 << 5 << 3 << 1 << 2;

//    QtCharts::QHorizontalStackedBarSeries *series = new QtCharts::QHorizontalStackedBarSeries();
//    series->append(set0);
//    series->append(set1);
//    series->append(set2);
//    series->append(set3);
//    series->append(set4);

//    QtCharts::QChart *chart = new QtCharts::QChart();
//    chart->addSeries(series);
//    chart->setTitle("Simple horizontal stacked barchart example");
//    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

//    QStringList categories;
//    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
//    QtCharts::QBarCategoryAxis *axis = new QtCharts::QBarCategoryAxis();
//    axis->append(categories);
//    chart->createDefaultAxes();
//    chart->setAxisY(axis, series);

//    chart->legend()->setVisible(true);
//    chart->legend()->setAlignment(Qt::AlignBottom);

//    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart, stackedBarChartWidget);
//    chartView->setRenderHint(QPainter::Antialiasing);

//    chartView->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
//    QSize widgetSize = stackedBarChartWidget->size();
//    chartView->resize(800, 600);
}

void MainWindow::setupBarChart(QCustomPlot *barChart, std::vector<std::pair <std::string, double>> barChartList) {
    // create empty bar chart objects:
    QCPBars *yLabels = new QCPBars(barChart->yAxis, barChart->xAxis);
    barChart->addPlottable(yLabels);

    // set names and colors:
    QPen pen;
    pen.setWidthF(1.2);
    yLabels->setName("Type");
    pen.setColor(QColor(255, 131, 0));
    yLabels->setPen(pen);
    yLabels->setBrush(QColor(255, 131, 0, 50));

    //get label list
    int barSize = (int) barChartList.size();
    double maxCount = 0;
    double scaledCount;
    QVector<double> ticks;
    QVector<QString> ylabels;
    QVector<double> count;

    //add label list to y axis labels
    for (int i = 0; i < barSize; i++){
        ticks << (i+1);
        ylabels << QString::fromStdString(barChartList[i].first);
        if (barChartList[i].second>1000000){
            scaledCount = barChartList[i].second/1000000;
        } else if (barChartList[i].second>1000){
            scaledCount = barChartList[i].second/1000;
        } else{
            scaledCount = barChartList[i].second;
        }
        count <<scaledCount;

        if (maxCount < barChartList[i].second)
            maxCount = barChartList[i].second;
    }

    //setup Y Axis
    barChart->yAxis->setAutoTicks(false);
    barChart->yAxis->setAutoTickLabels(false);
    barChart->yAxis->setTickVector(ticks);
    barChart->yAxis->setTickVectorLabels(ylabels);
    barChart->yAxis->setTickLabelPadding(1);
    barChart->yAxis->setSubTickCount(0);
    barChart->yAxis->setTickLength(0, 1);
    barChart->yAxis->grid()->setVisible(true);
    barChart->yAxis->setRange(0, barSize+1);

    if(maxCount>1000000){
        maxCount = maxCount/1000000;
        barChart->xAxis->setLabel("Total (in Millions)");
    }else if (maxCount>1000){
        maxCount = maxCount/1000;
        barChart->xAxis->setLabel("Total (in Thousands)");
    }else{
        barChart->xAxis->setLabel("Total");
    }

    // setup X Axis
    barChart->xAxis->setAutoTicks(true);
    barChart->xAxis->setRange(0,maxCount+(maxCount*.05));
    barChart->xAxis->setAutoTickLabels(true);
    barChart->xAxis->setAutoTickStep(true);
    barChart->xAxis->grid()->setSubGridVisible(true);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    barChart->xAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    barChart->xAxis->grid()->setSubGridPen(gridPen);

    yLabels->setData(ticks, count);
}


void MainWindow::on_teach_new_sort_clicked() {
    if (teachdb != NULL) {
//<<<<<<< HEAD
//=======
        std::vector<std::string> sortFields = TEACH_MANFIELDS;
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(TEACH_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allTeachOrders << newSortOrder;
            ui->teach_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_pub_new_sort_clicked() {
    if (pubdb != NULL) {
//<<<<<<< HEAD
//=======
        std::vector<std::string> sortFields = PUBS_MANFIELDS;
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PUBS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPubOrders << newSortOrder;
            ui->pub_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_pres_new_sort_clicked() {
    if (presdb != NULL) {
//<<<<<<< HEAD
//=======
        std::vector<std::string> sortFields = PRES_MANFIELDS;
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PRES_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPresOrders << newSortOrder;
            ui->pres_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_fund_new_sort_clicked() {
    if (funddb != NULL) {
//<<<<<<< HEAD
//=======
        std::vector<std::string> sortFields = GRANTS_MANFIELDS;
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(GRANTS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allFundOrders << newSortOrder;
            ui->fund_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_teach_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allTeachOrders[index];
        teachSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            teachSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->teach_filter->setText(QString::fromStdString(teachSortOrder[0]));
        refresh(TEACH);
    }
}

void MainWindow::on_pub_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPubOrders[index];
        pubSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            pubSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pub_filter->setText(QString::fromStdString(pubSortOrder[0]));
        refresh(PUBLICATIONS);
    }
}

void MainWindow::on_pres_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPresOrders[index];
        presSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            presSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pres_filter->setText(QString::fromStdString(presSortOrder[0]));
        refresh(PRESENTATIONS);
    }
}

void MainWindow::on_fund_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allFundOrders[index];
        fundSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            fundSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->fund_filter->setText(QString::fromStdString(fundSortOrder[0]));
        refresh(FUNDING);
    }
}

void MainWindow::on_teach_delete_sort_clicked() {
    if (ui->teach_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->teach_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allTeachOrders.removeAt(ui->teach_sort->currentIndex());
            ui->teach_sort->removeItem(ui->teach_sort->currentIndex());

            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_pub_delete_sort_clicked() {
    if (ui->pub_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pub_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPubOrders.removeAt(ui->pub_sort->currentIndex());
            ui->pub_sort->removeItem(ui->pub_sort->currentIndex());

            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_pres_delete_sort_clicked() {
    if (ui->pres_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pres_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPresOrders.removeAt(ui->pres_sort->currentIndex());
            ui->pres_sort->removeItem(ui->pres_sort->currentIndex());

            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_fund_delete_sort_clicked() {
    if (ui->fund_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->fund_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allFundOrders.removeAt(ui->fund_sort->currentIndex());
            ui->fund_sort->removeItem(ui->fund_sort->currentIndex());

            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_teach_bar_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(3);}
void MainWindow::on_teach_pie_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_pub_bar_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(3);}
void MainWindow::on_pub_pie_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_pres_bar_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(3);}
void MainWindow::on_pres_pie_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_fund_bar_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(3);}
void MainWindow::on_fund_pie_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_teach_stack_button_toggled(bool checked)
{
    ui->teach_graph_stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_teach_line_button_toggled(bool checked)
{
    ui->teach_graph_stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pub_stacked_button_toggled(bool checked)
{
    ui->pub_graph_stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pub_line_button_toggled(bool checked)
{
    ui->pub_graph_stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pres_stacked_button_toggled(bool checked)
{
    ui->pres_graph_stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pres_line_button_toggled(bool checked)
{
    ui->pres_graph_stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_fund_stacked_button_toggled(bool checked)
{
    ui->fund_graph_stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_fund_line_button_toggled(bool checked)
{
    ui->fund_graph_stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_teach_load_file_clicked() {
    QString path = load_file();
    if (!path.isEmpty()) {
        load_teach(path);
    }
}

bool MainWindow::load_teach(QString path, bool multi_file) {
    if (!checkFile(TEACH, path)) {
        // enable gui elements
        ui->teach_sort->setEnabled(true);
        ui->teach_delete_sort->setEnabled(true);
        ui->teach_new_sort->setEnabled(true);
        ui->teach_filter_from->setEnabled(true);
        ui->teach_filter_to->setEnabled(true);
        ui->teach_pie_button->setEnabled(true);
        ui->teach_bar_button->setEnabled(true);
        ui->teach_to_label->setEnabled(true);
        ui->teach_sort_label->setEnabled(true);
        ui->teach_filter->setEnabled(true);
        ui->teach_filter_label->setEnabled(true);

        // load save order
        QSortListIO teachSaveOrder(TEACHORDER_SAVE);
        allTeachOrders = teachSaveOrder.readList();
        ui->teach_sort->clear();
        for (int i = 0; i < allTeachOrders.size(); i++) {
            ui->teach_sort->addItem(allTeachOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->teach_sort->currentIndex() < 0) {
            createDefaultSortOrder(TEACH);
            ui->teach_sort->addItem(allTeachOrders[0][0]);
        }

        // create the tree
        teachPath = path;
        makeTree(TEACH);
        ui->teach_file_label->setText(teachPath);
        teachclosable = true;

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid teaching file.");
            on_teach_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_pub_load_file_clicked() {
    QString path = load_file();
    if (!path.isEmpty()) {
        load_pub(path);
    }
}

bool MainWindow::load_pub(QString path, bool multi_file) {
    if (!checkFile(PUBLICATIONS, path)) {
        // enable gui elements
        ui->pub_sort->setEnabled(true);
        ui->pub_delete_sort->setEnabled(true);
        ui->pub_new_sort->setEnabled(true);
        ui->pub_filter_from->setEnabled(true);
        ui->pub_filter_to->setEnabled(true);
        ui->pub_pie_button->setEnabled(true);
        ui->pub_bar_button->setEnabled(true);
        ui->pub_to_label->setEnabled(true);
        ui->pub_sort_label->setEnabled(true);
        ui->pub_filter->setEnabled(true);
        ui->pub_filter_label->setEnabled(true);

        // load save order
        QSortListIO pubSaveOrder(PUBORDER_SAVE);
        allPubOrders = pubSaveOrder.readList();
        ui->pub_sort->clear();
        for (int i = 0; i < allPubOrders.size(); i++) {
            ui->pub_sort->addItem(allPubOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->pub_sort->currentIndex() < 0) {
            createDefaultSortOrder(PUBLICATIONS);
            ui->pub_sort->addItem(allPubOrders[0][0]);
        }

        // create the tree
        pubPath = path;
        makeTree(PUBLICATIONS);
        ui->pub_file_label->setText(pubPath);
        pubclosable = true;

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid publications file.");
            on_pub_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_pres_load_file_clicked() {
    QString path = load_file();
    if (!path.isEmpty()) {
        load_pres(path);
    }
}

bool MainWindow::load_pres(QString path, bool multi_file) {
    if (!checkFile(PRESENTATIONS, path)) {
        // enable gui elements
        ui->pres_sort->setEnabled(true);
        ui->pres_delete_sort->setEnabled(true);
        ui->pres_new_sort->setEnabled(true);
        ui->pres_filter_from->setEnabled(true);
        ui->pres_filter_to->setEnabled(true);
        ui->pres_pie_button->setEnabled(true);
        ui->pres_bar_button->setEnabled(true);
        ui->pres_to_label->setEnabled(true);
        ui->pres_sort_label->setEnabled(true);
        ui->pres_filter->setEnabled(true);
        ui->pres_filter_label->setEnabled(true);

        // load save order
        QSortListIO presSaveOrder(PRESORDER_SAVE);
        allPresOrders = presSaveOrder.readList();
        ui->pres_sort->clear();
        for (int i = 0; i < allPresOrders.size(); i++) {
            ui->pres_sort->addItem(allPresOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->pres_sort->currentIndex() < 0) {
            createDefaultSortOrder(PRESENTATIONS);
            ui->pres_sort->addItem(allPresOrders[0][0]);
        }

        // create the tree
        presPath = path;
        makeTree(PRESENTATIONS);
        ui->pres_file_label->setText(presPath);
        presclosable = true;

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid presentations file.");
            on_pres_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_fund_load_file_clicked() {
    QString path = load_file();
    if (!path.isEmpty()) {
        load_fund(path);
    }
}

bool MainWindow::load_fund(QString path, bool multi_file) {
    if (!checkFile(FUNDING, path)) {
        // enable gui elements
        ui->fund_sort->setEnabled(true);
        ui->fund_delete_sort->setEnabled(true);
        ui->fund_new_sort->setEnabled(true);
        ui->fund_filter_from->setEnabled(true);
        ui->fund_filter_to->setEnabled(true);
        ui->fund_pie_button->setEnabled(true);
        ui->fund_bar_button->setEnabled(true);
        ui->fund_to_label->setEnabled(true);
        ui->fund_sort_label->setEnabled(true);
        ui->fund_filter->setEnabled(true);
        ui->fund_filter_label->setEnabled(true);

        // load save order
        QSortListIO fundSaveOrder(FUNDORDER_SAVE);
        allFundOrders = fundSaveOrder.readList();
        ui->fund_sort->clear();
        for (int i = 0; i < allFundOrders.size(); i++) {
            ui->fund_sort->addItem(allFundOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->fund_sort->currentIndex() < 0) {
            createDefaultSortOrder(FUNDING);
            ui->fund_sort->addItem(allFundOrders[0][0]);
        }

        // create the tree
        fundPath = path;
        makeTree(FUNDING);
        ui->fund_file_label->setText(fundPath);
        fundclosable = true;

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid grants and funding file.");
            on_fund_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_FromDate_dateChanged(const QDate &date) {
    // set the member variable to the new date
    yearStart = date.year();

    // update end date spinbox to not fall below that year
    ui->ToDate->setMinimumDate(date);

    dateChanged = {true, true, true, true};

    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}

void MainWindow::on_ToDate_dateChanged(const QDate &date) {
    // set the member variable to the new date
    yearEnd = date.year();

    // update end date spinbox to not fall below that year
    ui->ToDate->setMinimumDate(ui->FromDate->date());

    dateChanged = {true, true, true, true};

    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}

void MainWindow::on_categoryTab_currentChanged() {
    if (dateChanged[ui->categoryTab->currentIndex()] == true) {
        refresh(ui->categoryTab->currentIndex());
        dateChanged[ui->categoryTab->currentIndex()] = false;
    }
}

void MainWindow::on_teachTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==teachClickedName || index.column()!=0) { return;}

    qInfo() << "Clicked name: " << clickedName;

    //future viv LOOK HERE: this enables the button when we are on that current tab. you will see later there are similar for each subject tab.
    ui->teachingAddIndex->setEnabled(true); //enables the add index button for teaching when the teaching tab is open
    ui->teachingShowList->setEnabled(true);
    ui->teachingHideList->setEnabled(true);

    std::vector<std::string> parentsList;
    QModelIndex current = index;

    //future viv LOOK HERE: this is to get the current index for teaching. you will see later that there are similar variables for each subject tab
    currentTeachIndex = index;    //get the current index for the user selected list

    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    std::vector<QString> categories;
    std::vector<std::pair<QString, std::vector<double>>> allValues;
    std::vector<std::pair<QString, std::vector<double>>> allLineValues;

    if (parentsList.size()!=teachSortOrder.size()) {
        QTextStream out(stdout);
        teachClickedName = clickedName;
        std::vector<std::string> sortOrder(teachSortOrder.begin(), teachSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                teachdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(TEACH), getFilterEndChar(TEACH));
        std::vector<std::pair <std::string, double>> chartList;
        qInfo() << "Outputting content of teach list";
        categories.push_back(clickedName);
        for (int i = 0; i < (int) list.size(); i++) {
            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;
            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
//            QList<QWidget *> childWidgets = ui->teachStackedBarChart->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
//            foreach(QWidget *tmpWidget, childWidgets) {
//                delete tmpWidget;
//            }

            ui->teachStackedBarChart->resize(1100, 600);
            ui->teachLineChart->resize(1100, 650);
            ui->teach_graph_stackedWidget->hide();
            setupStackedBarChart(ui->teachStackedBarChart, categories, allValues);
            setupLineChart(ui->teachLineChart, allLineValues);
    //        ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->teachStackedBarChart->show();
        }

        if (!chartList.empty()) {
            ui->teachBarChart->clearPlottables();
            setupBarChart(ui->teachBarChart, chartList);
            ui->teachBarChart->replot();

            setupPieChart(ui->teachPieChart, ui->teachPieList, chartList);

            if (parentsList.size()>1) {
                ui->teachGraphTitle->setText("Total " + clickedName + " Teaching by " +
                                             QString::fromStdString(teachSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->teachGraphTitle->setText("Total Teaching by " + QString::fromStdString(parentsList[0]));
            }
            ui->teach_graph_stackedWidget->show();
        }
    } else {
        ui->teach_graph_stackedWidget->hide();
        ui->teachGraphTitle->clear();
        teachClickedName.clear();
    }
}

void MainWindow::on_pubTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==pubClickedName || index.column()!=0) { return;}

    //future viv LOOK HERE: this is same as what i said for the teaching tab
    ui->publicationsAddIndex->setEnabled(true);      //for the user selected list: enables the add index button for publications when the publications tab is open
    ui->publicationsShowList->setEnabled(true);
    ui->publicationsHideList->setEnabled(true);

    std::vector<std::string> parentsList;
    QModelIndex current = index;

    //future viv LOOK HERE: again, same situation explained before
    currentPubIndex = index; //for the user selected list, gets current index

    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    std::vector<QString> categories;
    std::vector<std::pair<QString, std::vector<double>>> allValues;
    std::vector<std::pair<QString, std::vector<double>>> allLineValues;

    if (parentsList.size()!=pubSortOrder.size()) {
        pubClickedName = clickedName;

        categories.push_back(clickedName);

        std::vector<std::string> sortOrder(pubSortOrder.begin(), pubSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));
        std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {


            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
//            QList<QWidget *> childWidgets = ui->teachStackedBarChart->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
//            foreach(QWidget *tmpWidget, childWidgets) {
//                delete tmpWidget;
//            }
            ui->pubStackedBarChart->resize(1100, 600);
            ui->pubLineChart->resize(1100, 650);
            ui->pub_graph_stackedWidget->hide();
            setupStackedBarChart(ui->pubStackedBarChart, categories, allValues);
            setupLineChart(ui->pubLineChart, allLineValues);
    //        ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->pubStackedBarChart->show();
        }

        if (!chartList.empty()) {
            ui->pubBarChart->clearPlottables();
            setupBarChart(ui->pubBarChart, chartList);
            ui->pubBarChart->replot();

            setupPieChart(ui->pubPieChart, ui->pubPieList, chartList);

            if (parentsList.size()>1) {
                ui->pubGraphTitle->setText("Total " + clickedName + " Publications by " +
                                           QString::fromStdString(pubSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->pubGraphTitle->setText("Total Publications by " + QString::fromStdString(parentsList[0]));
            }
            ui->pub_graph_stackedWidget->show();
        }
    } else {
        ui->pub_graph_stackedWidget->hide();
        ui->pubGraphTitle->clear();
        pubClickedName.clear();
    }
}

void MainWindow::on_presTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==presClickedName || index.column()!=0) { return;}
    //future viv: same explanation
    ui->presentationsAddIndex->setEnabled(true);//for the user selected list: when presentation tab is open this button is enabled
    ui->presentationsShowList->setEnabled(true);
    ui->presentationsHideList->setEnabled(true);

    std::vector<std::string> parentsList;
    QModelIndex current = index;

    //future viv: same explanation
    currentPresIndex = index;   //for the user selected list: get current index
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    std::vector<QString> categories;
    std::vector<std::pair<QString, std::vector<double>>> allValues;
    std::vector<std::pair<QString, std::vector<double>>> allLineValues;

    if (parentsList.size()!=presSortOrder.size()) {
        presClickedName = clickedName;

        categories.push_back(clickedName);

        std::vector<std::string> sortOrder(presSortOrder.begin(), presSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                presdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PRESENTATIONS), getFilterEndChar(PRESENTATIONS));
        std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {


            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
//            QList<QWidget *> childWidgets = ui->teachStackedBarChart->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
//            foreach(QWidget *tmpWidget, childWidgets) {
//                delete tmpWidget;
//            }
            ui->presStackedBarChart->resize(1100, 600);
            ui->presLineChart->resize(1100, 650);
            ui->pres_graph_stackedWidget->hide();
            setupStackedBarChart(ui->presStackedBarChart, categories, allValues);
            setupLineChart(ui->presLineChart, allLineValues);
    //        ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->presStackedBarChart->show();
        }

        if (!chartList.empty()) {
            ui->presBarChart->clearPlottables();
            setupBarChart(ui->presBarChart, chartList);
            ui->presBarChart->replot();

            setupPieChart(ui->presPieChart, ui->presPieList, chartList);

            if (parentsList.size()>1) {
                ui->presGraphTitle->setText("Total " + clickedName + " Presentations by " +
                                            QString::fromStdString(presSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->presGraphTitle->setText("Total Presentations by " + QString::fromStdString(parentsList[0]));
            }
            ui->pres_graph_stackedWidget->show();
        }
    } else {
        ui->pres_graph_stackedWidget->hide();
        ui->presGraphTitle->clear();
        presClickedName.clear();
    }
}

void MainWindow::on_fundTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==fundClickedName || index.column()!=0) { return;}

    ui->fund_stacked_button->setDisabled(true);
    ui->fund_line_button->setDisabled(true);


    //future viv: again same thing
    ui->fundsAddIndex->setEnabled(true);//for the user selected list: enables the button
    ui->fundsShowList->setEnabled(true);
    ui->fundsHideList->setEnabled(true);


    std::vector<std::string> parentsList;
    QModelIndex current = index;

    //future viv: again same thing
    currentFundIndex = index; //for the user selected list: get current index


    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=fundSortOrder.size()) {
        if (clickedName != fundClickedName) {
            fundClickedName = clickedName;
            std::vector<std::string> sortOrder(fundSortOrder.begin(), fundSortOrder.begin()+parentsList.size()+1);
            std::vector<std::pair <std::string, double>> chartList =
                    funddb->getTotalsTuple(yearStart, yearEnd, sortOrder, parentsList, "Total Amount", getFilterStartChar(FUNDING), getFilterEndChar(FUNDING));

//            if (!allValues.empty()) {
//    //            QList<QWidget *> childWidgets = ui->teachStackedBarChart->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
//    //            foreach(QWidget *tmpWidget, childWidgets) {
//    //                delete tmpWidget;
//    //            }
//                ui->fund_graph_stackedWidget->hide();
//                setupStackedBarChart(ui->fundStackedBarChart, categories, allValues);
//                setupLineChart(ui->fundLineChart, allLineValues);
//        //        ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
//                ui->fundStackedBarChart->show();
//            }

            if (!chartList.empty()) {
                ui->fundBarChart->clearPlottables();
                setupBarChart(ui->fundBarChart, chartList);
                ui->fundBarChart->replot();

                setupPieChart(ui->fundPieChart, ui->fundPieList, chartList);

                if (parentsList.size()>1) {
                    ui->fundGraphTitle->setText("Total " + clickedName + " Grants & Funding by " +
                                                QString::fromStdString(fundSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
                } else {
                    ui->fundGraphTitle->setText("Total Grants & Funding by " + QString::fromStdString(parentsList[0]));
                }
                ui->fund_graph_stackedWidget->show();
            }
        } else {
            ui->fund_graph_stackedWidget->hide();
            ui->fundGraphTitle->clear();
            fundClickedName.clear();
        }
    }
}

void MainWindow::on_teachPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->teachChartFrame->render(&painter);
    }
}

void MainWindow::on_fundPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->fundChartFrame->render(&painter);
    }
}

void MainWindow::on_presPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->presChartFrame->render(&painter);
    }
}

void MainWindow::on_pubPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->pubChartFrame->render(&painter);
    }
}

void MainWindow::on_teachExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        QPainter painter;
        painter.begin(&writer);
        painter.scale(10.0, 10.0);
        ui->teachChartFrame->render(&painter);
        /* Another option for bar chart since it is QCustom plot
        if (ui->teach_bar_button->isChecked()) {
            ui->teachBarChart->savePdf(fileName);
        } */
    }

}

void MainWindow::on_fundExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        QPainter painter;
        painter.begin(&writer);
        painter.scale(10.0, 10.0);
        ui->fundChartFrame->render(&painter);
    }
}

void MainWindow::on_presExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        QPainter painter;
        painter.begin(&writer);
        painter.scale(10.0, 10.0);
        ui->presChartFrame->render(&painter);
    }
}

void MainWindow::on_pubExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        QPainter painter;
        painter.begin(&writer);
        painter.scale(10.0, 10.0);
        ui->pubChartFrame->render(&painter);
    }
}


char MainWindow::getFilterStartChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_from->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_from->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_from->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_from->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else {
        // otherwise, default is '*'
        return '*';
    }
}

char MainWindow::getFilterEndChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_to->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_to->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_to->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_to->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else if (charInField == '*') {
        // otherwise, is it '*'?
        return charInField;
    } else {
        // otherwise, default is 'Z'
        return 'Z';
    }
}

void MainWindow::on_teach_filter_from_textChanged() { refresh(TEACH);}
void MainWindow::on_teach_filter_to_textChanged() { refresh(TEACH);}
void MainWindow::on_pub_filter_from_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pub_filter_to_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pres_filter_from_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_pres_filter_to_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_fund_filter_from_textChanged() { refresh(FUNDING);}
void MainWindow::on_fund_filter_to_textChanged() { refresh(FUNDING);}
//<<<<<<< HEAD
//=======

/**
 * Method to close a tab when
 * @param index teh index of the tab ie. 0 is teaching tab QWidget
 */
void MainWindow::on_categoryTab_tabCloseRequested(int index){

    QString fileName;

    switch (index) {
        case TEACH:
            //TODO: create a check to check if there is currently anything open in the tab before it is closed for all tabs
            if (teachclosable == true){

                QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Would you like to save your session?\n"),
                                                                           QMessageBox::Yes | QMessageBox::No);
                if (rstBtn == QMessageBox::Yes) {
                     //code to save the session state based on the tab

                    fileName = QFileDialog::getSaveFileName(ui->categoryTab->widget(0),
                                                            QObject::tr("Save database file"), QDir::currentPath(),
                                                            QObject::tr("CSV (*.csv);;All Files (*)"));
                    if (!fileName.isNull() && !fileName.isEmpty()){
                        saveteachdb(fileName);
                        teachclosable = false;
                    }
                }else{

                    QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Are you sure you would like to continue without saving?\n"),
                                                                               QMessageBox::Yes | QMessageBox::No);
                    if (rstBtn == QMessageBox::Yes) {
                        remove(TEACHFILE); // delete the temp teach file used in state saving
                        disableteachview();
                        teachclosable = false;
                    }
                }
            }
        break;

        case PUBLICATIONS:
            if (pubclosable == true){

                QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Would you like to save your session?\n"),
                                                                           QMessageBox::Yes | QMessageBox::No);
                if (rstBtn == QMessageBox::Yes) {
                     //code to save the session state based on the tab

                    fileName = QFileDialog::getSaveFileName(ui->categoryTab->widget(1),
                                                            QObject::tr("Save database file"), QDir::currentPath(),
                                                            QObject::tr("CSV (*.csv);;All Files (*)"));
                    if (!fileName.isNull() && !fileName.isEmpty()){
                        savepubdb(fileName);
                        pubclosable = false;
                    }
                }else{

                    QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Are you sure you would like to continue without saving?\n"),
                                                                               QMessageBox::Yes | QMessageBox::No);
                    if (rstBtn == QMessageBox::Yes) {
                        remove(PUBFILE); // delete the temp Publication file used in state saving
                        disablepubview();
                        pubclosable = false;
                    }
                }
            }
        break;

        case PRESENTATIONS:
            if (presclosable == true){

                QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Would you like to save your session?\n"),
                                                                           QMessageBox::Yes | QMessageBox::No);
                if (rstBtn == QMessageBox::Yes) {
                     //code to save the session state based on the tab

                    fileName = QFileDialog::getSaveFileName(ui->categoryTab->widget(2),
                                                            QObject::tr("Save database file"), QDir::currentPath(),
                                                            QObject::tr("CSV (*.csv);;All Files (*)"));
                    if (!fileName.isNull() && !fileName.isEmpty()){
                        savepresdb(fileName);
                        presclosable = false;
                    }
                }else{

                    QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Are you sure you would like to continue without saving?\n"),
                                                                               QMessageBox::Yes | QMessageBox::No);
                    if (rstBtn == QMessageBox::Yes) {
                        remove(PRESFILE); // delete the temp presentation file used in state saving
                        disablepresview();
                        presclosable = false;
                    }
                }
            }
        break;

        case FUNDING:
        if (fundclosable == true){

            QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Would you like to save your session?\n"),
                                                                       QMessageBox::Yes | QMessageBox::No);
            if (rstBtn == QMessageBox::Yes) {
                 //code to save the session state based on the tab

                fileName = QFileDialog::getSaveFileName(ui->categoryTab->widget(3),
                                                        QObject::tr("Save database file"), QDir::currentPath(),
                                                        QObject::tr("CSV (*.csv);;All Files (*)"));
                if (!fileName.isNull() && !fileName.isEmpty()){
                    savefunddb(fileName);
                    fundclosable = false;
                }
            }else{
                QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Libra" ,tr("Are you sure you would like to continue without saving?\n"),
                                                                           QMessageBox::Yes | QMessageBox::No);
                if (rstBtn == QMessageBox::Yes) {
                    remove(PUBFILE); // delete the temp Grants and Funding file used in state saving
                    disablefundview();
                    fundclosable = false;
                }
            }
        }
        break;
    }
 }

/*
 * The following buttons are used:
 *      addIndex, removeIndex, UndoIndex, ClearList, hidelist, show list
 * These  buttons are for each tab (teaching, publications, grants, presentation)
*/
//---------------------------FOR THE USER SELECTED LIST------------------------

/**
 * Stores the currently selected index into the userSelectedList for the Teaching Subject Area.
 */
void MainWindow::on_teachingAddIndex_clicked()
{
    QString clickedName = this->currentTeachIndex.data(Qt::DisplayRole).toString();
    int checkDuplicateStorage = userTeachList.searchSelectedList(this->currentTeachIndex);
    if(checkDuplicateStorage < 0){
        userTeachList.addIndex(this->currentTeachIndex);
    }

   if(this->userTeachList.getUserSelectedList().size() > 0){
       ui->teachingUndoIndex->setEnabled(true); //enables the button on the UI
       ui->teachingRemoveIndex->setEnabled(true);
       ui->teachingClearList->setEnabled(true);
   }
}

 /**
  * removes the last added index from the userSelectedList for the Teaching subject area.
 */
void MainWindow::on_teachingUndoIndex_clicked(){
    this->userTeachList.undoLastAddedIndex();

    if(this->userTeachList.getUserSelectedList().size() == 0){
        ui->teachingUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
        ui->teachingRemoveIndex->setEnabled(false);
        ui->teachingClearList->setEnabled(false);
    }
}

/**
 * removes the currently selected index from the userSelectedList for the Teaching subject area.
*/
void MainWindow::on_teachingRemoveIndex_clicked()
{
   //checking if exists in usl
    QString clickedName = this->currentTeachIndex.data(Qt::DisplayRole).toString();
    int checkDuplicateStorage = userTeachList.searchSelectedList(this->currentTeachIndex);
    if(checkDuplicateStorage > 0){//if it exists
        userTeachList.deleteIndex(this->currentTeachIndex);//remove
    }

    if(this->userTeachList.getUserSelectedList().size() == 0){
        ui->teachingUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
        ui->teachingRemoveIndex->setEnabled(false);
        ui->teachingClearList->setEnabled(false);
    }
}


/**
 * clears the userSelectedList for the Teaching Subject Area
 */
void MainWindow::on_teachingClearList_clicked()
{
    this->userTeachList.clearSelectedList();

    ui->teachingUndoIndex->setEnabled(false);//when list is cleared it's empty so we disable these buttons
    ui->teachingClearList->setEnabled(false);
    ui->teachingRemoveIndex->setEnabled(false);
}

/**
 * displays the user selectedlist
 *
 * when it is clicked hides whatever is not in the user selected list
 * we are modifying the VIEW, that is what is displayed from the variable: QTreeView* currentView
*/
void MainWindow::on_teachingShowList_clicked()
{ /*
    //need the row number and the parent for the qmodel index
    QModelIndex indexParent ;//= currentTeachIndex.parent();
            //<subject area>Tree->currentIndex()//to get the currently selected index

    //go through the rows in the tree and hide it all
    int totalRow = ui->teachTreeView->model()->rowCount();
    for (int i=0; i<= totalRow; i++){
        //currentView->setRowHidden(i, indexParent, true);
        ui->teachTreeView->setRowHidden(i, indexParent, true);  //set it to be hidden using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }

    //NOTE: i believe the way i implemented iterating through the list (below) is why the program crashes. the program seems to crash sometimes when i click Show List

    //now we will iterate through the user selected list and unhide rows from the current view
    std::vector<QModelIndex>::iterator it;  // declare an iterator to a vector of QModelIndex objects
    for(auto iter=userTeachList.getUserSelectedList().begin() ; iter != userTeachList.getUserSelectedList().end(); iter++) {
        QModelIndex currInd = *iter;
        int currentRow = currInd.row();
        ui->teachTreeView->setRowHidden(currentRow, currInd.parent(), false);//show it
    }*/


    QModelIndex indexParent;//need the row number and the parent for the qmodel index
    //go through the rows in the tree and hide it all
    int rows = ui->teachTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        ui->teachTreeView->setRowHidden(rowPosition, indexParent, true);
    }

     //now we will iterate through the user selected list and unhide rows from the current view
    QVector<QModelIndex> referenceList = userTeachList.getUserSelectedList();
    for(int listIndex = 0; listIndex < referenceList.size(); listIndex++) {
        QModelIndex storedIndex = referenceList[listIndex];
        int storedRow = storedIndex.row();
        ui->teachTreeView->setRowHidden(storedRow, indexParent, false);
        
        
    }
    /*
    below is the coded for displaying the graph for the userselectedlist. This feature is not functional and has been commented out.
    parameters need to be changed to their respective subject tab and other paramenters need to be changed so that they reflect the list. 
    */
    
    /*
std::vector<QString> categories;
std::vector<std::pair<QString, std::vector<double>>> allValues;
std::vector<std::pair<QString, std::vector<double>>> allLineValues;


std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {

            //here we need to write some code to check if the current item from the list is in the userselected list. If it is we proceed else we go to next item. 

            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
            ui->pubStackedBarChart->resize(1100, 600);
            ui->pubLineChart->resize(1100, 650);
            ui->pub_graph_stackedWidget->hide();
            setupStackedBarChart(ui->pubStackedBarChart, categories, allValues);
            setupLineChart(ui->pubLineChart, allLineValues);
            ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->pubStackedBarChart->show();
        }
*/
    
}

/**
 * show everything again
*/
void MainWindow::on_teachingHideList_clicked()
{
    QModelIndex indexParent;

    //go through the rows in the tree and show it all
    int rows = ui->teachTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        //currentView->setRowHidden(i, indexParent, false);
        ui->teachTreeView->setRowHidden(rowPosition, indexParent, false); //set it to be visible using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }
}
//-----------------------------------------------------------------------------

/**
 * Stores the currently selected index into the userSelectedList for the Publications Subject Area.
 */
void MainWindow::on_publicationsAddIndex_clicked()
{
    QString clickedName = this->currentPubIndex.data(Qt::DisplayRole).toString();
    int checkDuplicateStorage = userPubList.searchSelectedList(this->currentPubIndex);
    if(checkDuplicateStorage < 0){
        userPubList.addIndex(this->currentPubIndex);
    }

   if(this->userPubList.getUserSelectedList().size() > 0){
       ui->publicationsUndoIndex->setEnabled(true); //enables the button on the UI
       ui->publicationsRemoveIndex->setEnabled(true);
       ui->publicationsClearList->setEnabled(true);
   }
}

 /**
  * removes the last added index from the userSelectedList for the publications subject area.
 */
void MainWindow::on_publicationsUndoIndex_clicked()
{
   this->userPubList.undoLastAddedIndex();

    if(this->userPubList.getUserSelectedList().size() == 0){
        ui->publicationsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
        ui->publicationsRemoveIndex->setEnabled(false);
        ui->publicationsClearList->setEnabled(false);
    }
}

/**
* removes the currently selected index from the userSelectedList for the Publications subject area.
*/
void MainWindow::on_publicationsRemoveIndex_clicked()
{
  //checking if exists in usl
   QString clickedName = this->currentPubIndex.data(Qt::DisplayRole).toString();
   int checkDuplicateStorage = userPubList.searchSelectedList(this->currentPubIndex);
   if(checkDuplicateStorage > 0){//if it exists
       userPubList.deleteIndex(this->currentPubIndex);//remove
   }

   if(this->userPubList.getUserSelectedList().size() == 0){
       ui->publicationsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
       ui->publicationsRemoveIndex->setEnabled(false);
       ui->publicationsClearList->setEnabled(false);
   }
}

/**
 * clears the userSelectedList for the publications Subject Area
 */
void MainWindow::on_publicationsClearList_clicked()
{
    this->userPubList.clearSelectedList();

    ui->publicationsUndoIndex->setEnabled(false);//when list is cleared it's empty so we disable these buttons
    ui->publicationsRemoveIndex->setEnabled(false);
    ui->publicationsClearList->setEnabled(false);
}

/**
 * displays the user selectedlist
 *
 * when it is clicked hides whatever is not in the user selected list
 * we are modifying the VIEW, that is what is displayed from the variable: QTreeView* currentView
*/
void MainWindow::on_publicationsShowList_clicked()
{/*
    //need the row number and the parent for the qmodel index
    QModelIndex indexParent ;

    //go through the rows in the tree and hide it all
    int totalRow = ui->pubTreeView->model()->rowCount();
    for (int i=0; i<= totalRow; i++){
        ui->pubTreeView->setRowHidden(i, indexParent, true);  //set it to be hidden using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }

    //NOTE: i believe the way i implemented iterating through the list (below) is why the program crashes. the program seems to crash sometimes when i click Show List

    //now we will iterate through the user selected list and unhide rows from the current view
    std::vector<QModelIndex>::iterator it;  // declare an iterator to a vector of QModelIndex objects
    for(auto iter=userPubList.getUserSelectedList().begin() ; iter != userPubList.getUserSelectedList().end(); iter++) {
        QModelIndex currInd = *iter;
        int currentRow = currInd.row();
        ui->pubTreeView->setRowHidden(currentRow, currInd.parent(), false);//show it
    }*/

    QModelIndex indexParent;//need the row number and the parent for the qmodel index
    //go through the rows in the tree and hide it all
    int rows = ui->pubTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        ui->pubTreeView->setRowHidden(rowPosition, indexParent, true);
    }

     //now we will iterate through the user selected list and unhide rows from the current view
    QVector<QModelIndex> referenceList = userPubList.getUserSelectedList();
    for(int listIndex = 0; listIndex < referenceList.size(); listIndex++) {
        QModelIndex storedIndex = referenceList[listIndex];
        int storedRow = storedIndex.row();
        ui->pubTreeView->setRowHidden(storedRow, indexParent, false);
    }
    /*
    below is the coded for displaying the graph for the userselectedlist. This feature is not functional and has been commented out.
    parameters need to be changed to their respective subject tab and other paramenters need to be changed so that they reflect the list. 
    */
    
    /*
std::vector<QString> categories;
std::vector<std::pair<QString, std::vector<double>>> allValues;
std::vector<std::pair<QString, std::vector<double>>> allLineValues;


std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {

            //here we need to write some code to check if the current item from the list is in the userselected list. If it is we proceed else we go to next item. 

            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
            ui->pubStackedBarChart->resize(1100, 600);
            ui->pubLineChart->resize(1100, 650);
            ui->pub_graph_stackedWidget->hide();
            setupStackedBarChart(ui->pubStackedBarChart, categories, allValues);
            setupLineChart(ui->pubLineChart, allLineValues);
            ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->pubStackedBarChart->show();
        }
*/
}

/**
 * show everything again
*/
void MainWindow::on_publicationsHideList_clicked()
{
    QModelIndex indexParent;

    //go through the rows in the tree and show it all
    int rows = ui->pubTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        //currentView->setRowHidden(i, indexParent, false);
        ui->pubTreeView->setRowHidden(rowPosition, indexParent, false); //set it to be visible using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }
}
//-----------------------------------------------------------------------------

/**
 * Stores the currently selected index into the userSelectedList for the presentations Subject Area.
 */
void MainWindow::on_presentationsAddIndex_clicked()
{
    QString clickedName = this->currentPresIndex.data(Qt::DisplayRole).toString();
    int checkDuplicateStorage = userPresList.searchSelectedList(this->currentPresIndex);
    if(checkDuplicateStorage < 0){
        userPresList.addIndex(this->currentPresIndex);
    }

   if(this->userPresList.getUserSelectedList().size() > 0){
       ui->presentationsUndoIndex->setEnabled(true); //enables the button on the UI
       ui->presentationsClearList->setEnabled(true);
       ui->presentationsRemoveIndex->setEnabled(true);
   }
}

 /**
  * removes the last added index from the userSelectedList for the presentations subject area.
 */
void MainWindow::on_presentationsUndoIndex_clicked()
{
   this->userPresList.undoLastAddedIndex();

    if(this->userPresList.getUserSelectedList().size() == 0){
        ui->presentationsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
        ui->presentationsClearList->setEnabled(false);
        ui->presentationsRemoveIndex->setEnabled(false);
    }
}

/**
* removes the currently selected index from the userSelectedList for the presentations subject area.
*/
void MainWindow::on_presentationsRemoveIndex_clicked()
{
  //checking if exists in usl
   QString clickedName = this->currentPresIndex.data(Qt::DisplayRole).toString();
   int checkDuplicateStorage = userPresList.searchSelectedList(this->currentPresIndex);
   if(checkDuplicateStorage > 0){//if it exists
       userPresList.deleteIndex(this->currentPresIndex);//remove
   }

   if(this->userPresList.getUserSelectedList().size() == 0){
       ui->presentationsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
       ui->presentationsClearList->setEnabled(false);
       ui->presentationsRemoveIndex->setEnabled(false);
   }
}


/**
 * clears the userSelectedList for the presentations Subject Area
 */
void MainWindow::on_presentationsClearList_clicked()
{
    this->userPresList.clearSelectedList();

    ui->presentationsUndoIndex->setEnabled(false);//when list is cleared it's empty so we disable these buttons
    ui->presentationsClearList->setEnabled(false);
    ui->presentationsRemoveIndex->setEnabled(false);
}

/**
 * displays the user selectedlist
 *
 * when it is clicked hides whatever is not in the user selected list
 * we are modifying the VIEW, that is what is displayed from the variable: QTreeView* currentView
*/
void MainWindow::on_presentationsShowList_clicked()
{/*
    //need the row number and the parent for the qmodel index
    QModelIndex indexParent ;

    //go through the rows in the tree and hide it all
    int totalRow = ui->presTreeView->model()->rowCount();
    for (int i=0; i<= totalRow; i++){
        ui->presTreeView->setRowHidden(i, indexParent, true);  //set it to be hidden using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }

    //NOTE: i believe the way i implemented iterating through the list (below) is why the program crashes. the program seems to crash sometimes when i click Show List

    //now we will iterate through the user selected list and unhide rows from the current view
    std::vector<QModelIndex>::iterator it;  // declare an iterator to a vector of QModelIndex objects
    for(auto iter=userPresList.getUserSelectedList().begin() ; iter != userPresList.getUserSelectedList().end(); iter++) {
        QModelIndex currInd = *iter;
        int currentRow = currInd.row();
        ui->presTreeView->setRowHidden(currentRow, currInd.parent(), false);//show it
    }*/
    QModelIndex indexParent;//need the row number and the parent for the qmodel index
    //go through the rows in the tree and hide it all
    int rows = ui->presTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        ui->presTreeView->setRowHidden(rowPosition, indexParent, true);
    }

     //now we will iterate through the user selected list and unhide rows from the current view
    QVector<QModelIndex> referenceList = userPresList.getUserSelectedList();
    for(int listIndex = 0; listIndex < referenceList.size(); listIndex++) {
        QModelIndex storedIndex = referenceList[listIndex];
        int storedRow = storedIndex.row();
        ui->presTreeView->setRowHidden(storedRow, indexParent, false);
    }
    /*
    below is the coded for displaying the graph for the userselectedlist. This feature is not functional and has been commented out.
    parameters need to be changed to their respective subject tab and other paramenters need to be changed so that they reflect the list. 
    */
    
    /*
std::vector<QString> categories;
std::vector<std::pair<QString, std::vector<double>>> allValues;
std::vector<std::pair<QString, std::vector<double>>> allLineValues;


std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {

            //here we need to write some code to check if the current item from the list is in the userselected list. If it is we proceed else we go to next item. 

            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
            ui->pubStackedBarChart->resize(1100, 600);
            ui->pubLineChart->resize(1100, 650);
            ui->pub_graph_stackedWidget->hide();
            setupStackedBarChart(ui->pubStackedBarChart, categories, allValues);
            setupLineChart(ui->pubLineChart, allLineValues);
            ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->pubStackedBarChart->show();
        }
*/
}

/**
 * show everything again
*/
void MainWindow::on_presentationsHideList_clicked()
{
    QModelIndex indexParent;

    //go through the rows in the tree and show it all
    int rows = ui->presTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        //currentView->setRowHidden(i, indexParent, false);
        ui->presTreeView->setRowHidden(rowPosition, indexParent, false); //set it to be visible using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }

}
//-----------------------------------------------------------------------------

/**
 * Stores the currently selected index into the userSelectedList for the Grants Subject Area.
 */
void MainWindow::on_fundsAddIndex_clicked()
{
    QString clickedName = this->currentFundIndex.data(Qt::DisplayRole).toString();
    int checkDuplicateStorage = userFundList.searchSelectedList(this->currentFundIndex);
    if(checkDuplicateStorage < 0){
        userFundList.addIndex(this->currentFundIndex);
    }

   if(this->userFundList.getUserSelectedList().size() > 0){
       ui->fundsUndoIndex->setEnabled(true); //enables the button on the UI
       ui->fundsClearList->setEnabled(true);
       ui->fundsRemoveIndex->setEnabled(true);
   }
}

 /**
  * removes the last added index from the userSelectedList for the Grants subject area.
 */
void MainWindow::on_fundsUndoIndex_clicked()
{
   this->userFundList.undoLastAddedIndex();

    if(this->userFundList.getUserSelectedList().size() == 0){
        ui->fundsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
        ui->fundsClearList->setEnabled(false);
        ui->fundsRemoveIndex->setEnabled(false);
    }
}

/**
* removes the currently selected index from the userSelectedList for the presentations subject area.
*/
void MainWindow::on_fundsRemoveIndex_clicked()
{
  //checking if exists in usl
   QString clickedName = this->currentFundIndex.data(Qt::DisplayRole).toString();
   int checkDuplicateStorage = userFundList.searchSelectedList(this->currentFundIndex);
   if(checkDuplicateStorage > 0){//if it exists
       userFundList.deleteIndex(this->currentFundIndex);//remove
   }

   if(this->userFundList.getUserSelectedList().size() == 0){
       ui->fundsUndoIndex->setEnabled(false);//list is empty, we disabled these buttons
       ui->fundsClearList->setEnabled(false);
       ui->fundsRemoveIndex->setEnabled(false);
   }
}

/**
 * clears the userSelectedList for the Grants Subject Area
 */
void MainWindow::on_fundsClearList_clicked(){

    this->userFundList.clearSelectedList();

    ui->fundsUndoIndex->setEnabled(false);//when list is cleared it's empty so we disable these buttons
    ui->fundsClearList->setEnabled(false);
    ui->fundsRemoveIndex->setEnabled(false);
}

/**
 * displays the user selectedlist
 *
 * when it is clicked hides whatever is not in the user selected list
 * we are modifying the VIEW, that is what is displayed from the variable: QTreeView* currentView
*/
void MainWindow::on_fundsShowList_clicked(){
/*
    //need the row number and the parent for the qmodel index
    QModelIndex indexParent ;

    //go through the rows in the tree and hide it all
    int totalRow = ui->fundTreeView->model()->rowCount();
    for (int i=0; i<= totalRow; i++){
        ui->fundTreeView->setRowHidden(i, indexParent, true);  //set it to be hidden using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }

    //NOTE: i believe the way i implemented iterating through the list (below) is why the program crashes. the program seems to crash sometimes when i click Show List

    //now we will iterate through the user selected list and unhide rows from the current view
    std::vector<QModelIndex>::iterator it;  // declare an iterator to a vector of QModelIndex objects
    for(auto iter=userFundList.getUserSelectedList().begin() ; iter != userFundList.getUserSelectedList().end(); iter++) {
        QModelIndex currInd = *iter;
        int currentRow = currInd.row();
        ui->fundTreeView->setRowHidden(currentRow, currInd.parent(), false);//show it
    }*/

    QModelIndex indexParent;//need the row number and the parent for the qmodel index
    //go through the rows in the tree and hide it all
    int rows = ui->fundTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        ui->fundTreeView->setRowHidden(rowPosition, indexParent, true);
    }

     //now we will iterate through the user selected list and unhide rows from the current view
    QVector<QModelIndex> referenceList = userFundList.getUserSelectedList();
    for(int listIndex = 0; listIndex < referenceList.size(); listIndex++) {
        QModelIndex storedIndex = referenceList[listIndex];
        int storedRow = storedIndex.row();
        ui->fundTreeView->setRowHidden(storedRow, indexParent, false);
    }
    /*
    below is the coded for displaying the graph for the userselectedlist. This feature is not functional and has been commented out.
    parameters need to be changed to their respective subject tab and other paramenters need to be changed so that they reflect the list. 
    */
    
    /*
std::vector<QString> categories;
std::vector<std::pair<QString, std::vector<double>>> allValues;
std::vector<std::pair<QString, std::vector<double>>> allLineValues;


std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {

            //here we need to write some code to check if the current item from the list is in the userselected list. If it is we proceed else we go to next item. 

            std::vector<double> tmp = {static_cast<double>(list[i].second)};
            std::vector<double> lineTmp = {0, static_cast<double>(list[i].second)};
            std::pair<QString, std::vector<double>> tmpPair;
            tmpPair.first = QString::fromStdString(list[i].first);
            tmpPair.second = tmp;

            std::pair<QString, std::vector<double>> tmpLinePair;
            tmpLinePair.first = QString::fromStdString(list[i].first);
            tmpLinePair.second = lineTmp;

            allValues.push_back(tmpPair);
            allLineValues.push_back(tmpLinePair);
            std::cout << list[i].first << " " << list[i].second << std::endl;



            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!allValues.empty()) {
            ui->pubStackedBarChart->resize(1100, 600);
            ui->pubLineChart->resize(1100, 650);
            ui->pub_graph_stackedWidget->hide();
            setupStackedBarChart(ui->pubStackedBarChart, categories, allValues);
            setupLineChart(ui->pubLineChart, allLineValues);
            ui->teachStackedBarChart->setSizePolicy(new QSizePolicy(Policy.Fixed, Policy.Fixed));
            ui->pubStackedBarChart->show();
        }
*/
}

/**
 * show everything again
*/
void MainWindow::on_fundsHideList_clicked()
{
    QModelIndex indexParent;

    //go through the rows in the tree and show it all
    int rows = ui->fundTreeView->model()->rowCount();
    for(int rowPosition = 0; rowPosition < rows; rowPosition++){
        //currentView->setRowHidden(i, indexParent, false);
        ui->fundTreeView->setRowHidden(rowPosition, indexParent, false); //set it to be visible using the QTreeView::setRowHidden(int row, const QModelIndex &parent, bool hide)
    }
}
//----------------------------END OF METHODS FOR USER SELECTED LIST-------------------------------------------------
//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad

int paletteCount = 0;
void MainWindow::on_pushButton_4_clicked()
{
    if(paletteCount % 2 == 0)
    {
    ui->pushButton_4->setText("Day");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(80, 80, 120, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(80, 80, 80);");
     paletteCount++;
    }
    else
    {
    ui->pushButton_4->setText("Night");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(255, 255, 255, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(255, 255, 255);");
     paletteCount++;
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(paletteCount % 2 == 0)
    {
    ui->pushButton_3->setText("Day");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(80, 80, 120, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(80, 80, 80);");
     paletteCount++;
    }
    else
    {
    ui->pushButton_3->setText("Night");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(255, 255, 255, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(255, 255, 255);");
     paletteCount++;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(paletteCount % 2 == 0)
    {
    ui->pushButton_2->setText("Day");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(80, 80, 120, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(80, 80, 80);");
     paletteCount++;
    }
    else
    {
    ui->pushButton_2->setText("Night");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(255, 255, 255, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(255, 255, 255);");
     paletteCount++;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(paletteCount % 2 == 0)
    {
    ui->pushButton->setText("Day");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(80, 80, 120, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(80, 80, 80);");
     paletteCount++;
    }
    else
    {
    ui->pushButton->setText("Night");
    ui->frame->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0.972, x2:0.847, y2:0.199, stop:0 rgba(162, 162, 162, 255), stop:1 rgba(255, 255, 255, 255));;");
    ui->categoryTab->setStyleSheet("background-color: rgb(255, 255, 255);");
     paletteCount++;
    }
}
