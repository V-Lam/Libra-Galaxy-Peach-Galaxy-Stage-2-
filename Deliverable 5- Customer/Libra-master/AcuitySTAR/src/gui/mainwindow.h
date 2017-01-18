#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <QMainWindow>
#include <vector>
#include "UserSelectedList.h"

class RecordsManager;
class CSVReader;
class TreeModel;
class CustomSort;
class PieChartWidget;
class QListWidget;
class QTreeView;
class QCustomPlot;
class QPrinter;
class QPainter;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:
    QString load_file();

    void on_pres_load_file_clicked();
    void on_pub_load_file_clicked();
    void on_fund_load_file_clicked();
    void on_teach_load_file_clicked();

    void on_teach_new_sort_clicked();
    void on_pub_new_sort_clicked();
    void on_pres_new_sort_clicked();
    void on_fund_new_sort_clicked();

    void on_actionLoad_file_triggered();

    void on_FromDate_dateChanged(const QDate &date);
    void on_ToDate_dateChanged(const QDate &date);

    void refresh(int tabIndex);

    void on_teach_sort_currentIndexChanged(int index);
    void on_pub_sort_currentIndexChanged(int index);
    void on_pres_sort_currentIndexChanged(int index);
    void on_fund_sort_currentIndexChanged(int index);

    void on_teach_delete_sort_clicked();
    void on_pub_delete_sort_clicked();
    void on_pres_delete_sort_clicked();
    void on_fund_delete_sort_clicked();

    void on_teach_pie_button_toggled();
    void on_teach_bar_button_toggled();
    void on_pub_pie_button_toggled();
    void on_pub_bar_button_toggled();
    void on_pres_pie_button_toggled();
    void on_pres_bar_button_toggled();
    void on_fund_pie_button_toggled();
    void on_fund_bar_button_toggled();

    void on_teachTreeView_clicked(const QModelIndex &index);
    void on_pubTreeView_clicked(const QModelIndex &index);
    void on_presTreeView_clicked(const QModelIndex &index);
    void on_fundTreeView_clicked(const QModelIndex &index);

    void on_categoryTab_currentChanged();

    void on_teach_filter_from_textChanged();
    void on_teach_filter_to_textChanged();
    void on_pub_filter_from_textChanged();
    void on_pub_filter_to_textChanged();
    void on_pres_filter_from_textChanged();
    void on_pres_filter_to_textChanged();
    void on_fund_filter_from_textChanged();
    void on_fund_filter_to_textChanged();

    void on_teachPrintButton_clicked();

    void on_fundPrintButton_clicked();

    void on_presPrintButton_clicked();

    void on_pubPrintButton_clicked();

    void on_teachExportButton_clicked();

    void on_fundExportButton_clicked();

    void on_presExportButton_clicked();

    void on_pubExportButton_clicked();

    void on_teach_stack_button_toggled(bool checked);

    void on_teach_line_button_toggled(bool checked);

    void on_pub_stacked_button_toggled(bool checked);

    void on_pub_line_button_toggled(bool checked);

    void on_pres_stacked_button_toggled(bool checked);

    void on_pres_line_button_toggled(bool checked);

    void on_fund_stacked_button_toggled(bool checked);

    void on_fund_line_button_toggled(bool checked);

    void saveteachdb(QString);
    void savepresdb(QString);
    void savepubdb(QString);
    void savefunddb(QString);

    void disableteachview();
    void disablepubview();
    void disablepresview();
    void disablefundview();

    //FUTURE VIV: these are the function prototypes for the user selected list
        void on_teachingAddIndex_clicked();
        void on_teachingRemoveIndex_clicked();
        void on_teachingUndoIndex_clicked();
        void on_teachingClearList_clicked();
        void on_teachingShowList_clicked();
        void on_teachingHideList_clicked();
        void on_publicationsAddIndex_clicked();
        void on_publicationsRemoveIndex_clicked();
        void on_publicationsUndoIndex_clicked();
        void on_publicationsClearList_clicked();
        void on_publicationsShowList_clicked();
        void on_publicationsHideList_clicked();
        void on_presentationsAddIndex_clicked();
        void on_presentationsRemoveIndex_clicked();
        void on_presentationsUndoIndex_clicked();
        void on_presentationsClearList_clicked();
        void on_presentationsShowList_clicked();
        void on_presentationsHideList_clicked();
        void on_fundsAddIndex_clicked();
        void on_fundsRemoveIndex_clicked();
        void on_fundsUndoIndex_clicked();
        void on_fundsClearList_clicked();
        void on_fundsShowList_clicked();
        void on_fundsHideList_clicked();

    void on_categoryTab_tabCloseRequested(int index);

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    static std::vector<std::string> GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;
    std::vector<std::vector<std::string>*> f_errs_teach, f_errs_pub, f_errs_pres, f_errs_fund, f_errs_teach_fixed, f_errs_pub_fixed, f_errs_pres_fixed, f_errs_fund_fixed;

    enum TABS {
        TEACH, PUBLICATIONS, PRESENTATIONS, FUNDING
    };

    struct field_error;


    Ui::MainWindow* ui;
    QPrinter* printer;

    QList<QStringList> allTeachOrders, allPubOrders, allPresOrders, allFundOrders;
    QString teachPath, pubPath, presPath, fundPath;
    TreeModel *fundTree, *presTree, *pubTree, *teachTree;
    RecordsManager *funddb, *presdb, *pubdb, *teachdb;
    std::vector<std::vector<std::string>> fundData, presData, pubData, teachData;

//<<<<<<< HEAD
//=======
    //Boolena values that determine whether or not the closable feature works
    bool teachclosable, presclosable, pubclosable, fundclosable;

    //creates user selected list objects, one for each subject field
    UserSelectedList userTeachList, userPresList, userPubList, userFundList;

//>>>>>>> a74bde066a12e7470f08e455d311ff6a8e8405ad
    std::vector<std::string> teachSortOrder, pubSortOrder, presSortOrder, fundSortOrder;
    TreeModel* currentTree;
    RecordsManager* currentdb;
    QTreeView* currentView;
    QString teachClickedName, pubClickedName, presClickedName, fundClickedName;

    QModelIndex currentTeachIndex, currentPubIndex, currentPresIndex, currentFundIndex;

    QList<bool> dateChanged;

    std::vector<std::string> getParentsList(QModelIndex &index);
    char getFilterStartChar(int type);
    char getFilterEndChar(int type);

    int yearStart, yearEnd;

    int checkFile(int index, QString filePath);
    //std::vector<std::string> getSelectedSortOrder(int tabIndex);
    void createDefaultSortOrder(int tabIndex);
    void makeTree(int tabIndex);

    void setupPieChart(PieChartWidget *pieChart, QListWidget *pieListWidget, std::vector<std::pair<std::string, double> > pieChartList);
    void setupStackedBarChart(QWidget *stackedBarChartWidget, std::vector<QString> categories, std::vector<std::pair<QString, std::vector<double>>> barSetList);
    void setupLineChart(QWidget *lineChartWidget, std::vector<std::pair<QString, std::vector<double>>> barSetList);
    void setupBarChart(QCustomPlot *barChart, std::vector<std::pair<std::string, double> > barChartList);

    bool handle_field_errors(std::vector<std::vector<std::string>*>& errors,
                             std::vector<std::vector<std::string>*>& f_errors,
                             std::vector<std::string>& headers,
                             std::vector<std::string>& mandatory);
    bool load_fund(QString path, bool multi_file = false);
    bool load_pub(QString path, bool multi_file = false);
    bool load_teach(QString path, bool multi_file = false);
    bool load_pres(QString path, bool multi_file = false);
};

#endif // MAINWINDOW_H
