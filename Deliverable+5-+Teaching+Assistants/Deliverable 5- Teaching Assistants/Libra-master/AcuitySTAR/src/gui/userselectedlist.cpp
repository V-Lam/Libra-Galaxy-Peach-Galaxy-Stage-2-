/*
 * Author: Vivian L
 *
 * class that modifies QModelIndex items for the user selected list
 */

#include "UserSelectedList.h"
#include <exception>

/////////////////////////////////////

/*
 * HOW USER SELECTED LIST WORKS:
 *
 * This class is for adding, removing, and clearing QModelIndex from the vector
 * (alternatively, a link list can work instead of a vector)
 *
 * there is also a search function to make sure we don't store duplicates
 *
 * push buttons on main window for adding vector, undoing storage of vector and clearing vector
 *
 * there are 4 private variables that reference the currently selected QModelIndex for each of the subject areas
 *
 * add to index list button stores the variables into the list
*/

//////////////////////////////////FIELDS//////////////////////////////////////
//in the header

//////////////////////////////////FUNCTION PROTOTYPES/////////////////////////
//in the header

//////////////////////////////CONSTRUCTOR////////////////////////////////////
//empty. we dont need anything
UserSelectedList::UserSelectedList()
{
}

///////////////////////////////////METHODS///////////////////////////////////////
//getters
QVector<QModelIndex> UserSelectedList::getUserSelectedList(){
    return this->STORED_INDEXES;
}

//setters
void UserSelectedList::setUserSelectedList(QVector<QModelIndex> newList){
    this->STORED_INDEXES = newList;
}
/////////////

/*
 * check if user selected list is empty
 * false = empty
 * true = not empty
 */
bool UserSelectedList::checkEmpty(QVector<QModelIndex> testList){
    if(testList.size() != 0){
        return true;
    }
    else{
        return false;
    }
}




/*
 * method to add QModelIndex to the end of the list (stores the index of a data enty from QTreeView)
 * using vector's pushback method
*/
void UserSelectedList::addIndex(QModelIndex index){
    this->STORED_INDEXES.push_back(index);
}



/*
 * method to remove the last QModelIndec from the vector
 * using vector's remove()
*/
void UserSelectedList::deleteIndex(QModelIndex index){
    //Check to see if the list has indices to remove from the list.
    if(checkEmpty(this->getUserSelectedList())){
        int position = this->searchSelectedList(index);
        if(position != -1){
            this->STORED_INDEXES.remove(position);
        }
    }
}



/*
 * Removes the last QModelIndex stored
 */
void UserSelectedList::undoLastAddedIndex(){
    //note: the vector containing the indexes must not be empty
    if(checkEmpty(this->getUserSelectedList())){
        this->STORED_INDEXES.pop_back();
    }
}



/*
 * Searches the list for the index and deletes the index in the list if it is found.
 * (so that theres no duplicates)
 */
int UserSelectedList::searchSelectedList(const QModelIndex index){
    //If the userSelectedList is empty
    int position = -1;
    if(!checkEmpty(this->getUserSelectedList())){
        return position;
    }
    else{
        QString clickedName = index.data(Qt::DisplayRole).toString();
        QVector<QModelIndex> tempList = this->getUserSelectedList();
        for(int pos = 0; pos < tempList.size(); pos++){
            if(clickedName == tempList[pos].data(Qt::DisplayRole).toString()){
                position = pos;
                break;
            }
        }
        return position;
    }
}



/*
 * method to clear the list (removes all elements from the vector)
 * using vector's clear method
*/
void UserSelectedList::clearSelectedList(){
    this->STORED_INDEXES.clear();
}
