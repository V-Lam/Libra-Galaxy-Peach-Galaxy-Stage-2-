#include "ui_ErrorEditDialog.h"
#include "ErrorEditDialog.h"

#include <QTableWidgetItem>
#include <QStringList>
#include <QDebug>
#include <QBrush>
#include <QColor>
#include <QAbstractButton>
#include <QMessageBox>

/*
 * Load data contained in the errors vector into a QWidgetTable
 * Fields will be marked red and editable if they are a mandatory field
 * and editable.  Otherwise all other fields will not be editable.
 * Clicking Save in the dialog will return the corrected entries to the main
 * program through the errors parameter.  If not all marked fields are edited
 * then a warning message will be displayed.  If cancel is clicked all errors
 * are discarded.
 */
ErrorEditDialog::ErrorEditDialog(QWidget *parent,
                                 std::vector<std::vector<std::string>*>& errors,
                                 std::vector<std::vector<std::string>*>& f_errors,
                                 std::vector<std::string>& headers,
                                 std::vector<std::string>& mandatory) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    errorList(errors),
    fixedErrorsList(f_errors),
    headerList(headers),
    mandatoryList(mandatory),
    ui(new Ui::ErrorEditDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount((int) errors.size());
    ui->tableWidget->setColumnCount((int) headers.size());

    QStringList listHeaders;
    for (int i = 0; i < (int) headers.size(); i++) {
        listHeaders << headers[i].c_str();
    }

    ui->tableWidget->setHorizontalHeaderLabels(listHeaders);
    QTableWidgetItem* item;
    QBrush brush(QColor(255, 0, 0, 100));
    std::vector<std::vector<std::string>*>::iterator it;
    int row = 0;
    for (it = errors.begin(); it != errors.end(); it++) {
        for (int col = 0; col < (int) headers.size() && col < (int) (*it)->size(); col++) {
            item = new QTableWidgetItem();
            Qt::ItemFlags flag = item->flags();
            item->setFlags(Qt::ItemIsSelectable);
            item->setText((*it)->at(col).c_str());
            for (int i = 0; i < (int) mandatory.size(); i++) {
                if (mandatory[i].compare(headers.at(col)) == 0
                        && (*it)->at(col).compare("") == 0) {
                    item->setBackground(brush);
                    item->setFlags(flag);
                    std::vector<int> location = {row,col};
                    errorLocation.push_back(location);

                }
            }
            ui->tableWidget->setItem(row, col, item);
        }
        row++;
        currentLocationIndex = 0;
        std::vector<int> location = errorLocation[currentLocationIndex];
        ui->tableWidget->setCurrentCell(location[0],location[1]);
    }
}

//Clean up allocated memory for the table items
ErrorEditDialog::~ErrorEditDialog()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            delete ui->tableWidget->item(i,j);
        }
    }
    delete ui;
}

//Save the new data entered by the user via the error reference var
void ErrorEditDialog::saveData(){
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidget->columnCount() && col < (int) errorList[row]->size(); col++) {
            std::vector<std::string>::iterator it = errorList[row]->begin()+col;
            if (errorList[row]->at(col).compare("") == 0 && ui->tableWidget->item(row, col)->text().toStdString().compare("") == 0) {
                it = errorList[row]->erase(it);
                errorList[row]->insert(it, ui->tableWidget->item(row, col)->text().toStdString());
            }
        }
    }
    accept();
}

void ErrorEditDialog::on_save_clicked(){   

    std::vector<std::vector<std::string>*> tempFixedErrorsList, tempErrorsList;
    std::vector<std::string> tempErrorRow;
    int emptyMandFields = 0;
    int rowIndex = 0;
    //check if mandatory fields have been filled
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        bool rowcheck = true;
        for (int col = 0; col < ui->tableWidget->columnCount() && col < (int) errorList[row]->size(); col++) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item->text().compare("") == 0) {
                emptyMandFields++;
                rowcheck = false;
                //break
            }
        }
        if(rowcheck){
            //TODO: I do not get what I am supposed to be doing here
            //But I want to
            for (int col = 0; col < ui->tableWidget->columnCount() && col < (int) errorList[row]->size(); col++){
                std::vector<std::string>::iterator it = errorList[row]->begin()+col;
                tempFixedErrorsList[rowIndex]->insert(it, ui->tableWidget->item(row, col)->text().toStdString());
            }
            rowIndex++;
        }else{

        }
    }

    if (emptyMandFields > 0) {
        QMessageBox::StandardButton rstBtn = QMessageBox::question(this, "Error",
                                                                   tr("Some mandatory fields are still empty.\nWould you like to save the fixed records anyway?"),
                                                                   QMessageBox::Yes | QMessageBox::No);
        if (rstBtn == QMessageBox::Yes) {
            errorList = tempErrorsList;
            fixedErrorsList = tempFixedErrorsList;
            accept();
        }
    }
}


void ErrorEditDialog::on_cancel_clicked(){
    reject();
}

void ErrorEditDialog::on_Previous_Error_clicked(){
    if(currentLocationIndex>0){
           currentLocationIndex--;
           std::vector<int> location = errorLocation[currentLocationIndex];
           ui->tableWidget->setCurrentCell(location[0],location[1]);
    }else if(currentLocationIndex==0){
        currentLocationIndex = errorLocation.size()-1;
        std::vector<int> location = errorLocation[currentLocationIndex];
        ui->tableWidget->setCurrentCell(location[0],location[1]);
    }
}

void ErrorEditDialog::on_Next_Error_clicked(){
    if(currentLocationIndex<errorLocation.size()-1){
       currentLocationIndex++;
       std::vector<int> location = errorLocation[currentLocationIndex];
       ui->tableWidget->setCurrentCell(location[0],location[1]);
   }else{
       currentLocationIndex = 0;
       std::vector<int> location = errorLocation[currentLocationIndex];
       ui->tableWidget->setCurrentCell(location[0],location[1]);
   }
}
