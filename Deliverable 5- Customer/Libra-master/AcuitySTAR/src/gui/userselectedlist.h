#ifndef USERSELECTEDLIST_H
#define USERSELECTEDLIST_H

#include <QModelIndex>
#include <QVector>
#include <string>
#include <QAbstractItemModel>

class UserSelectedList
{
public:
    UserSelectedList();
    void addIndex(QModelIndex index);
    void deleteIndex(QModelIndex index);
    int searchSelectedList(const QModelIndex index);
    void clearSelectedList();
    void undoLastAddedIndex();
    QVector<QModelIndex> getUserSelectedList();
    void setUserSelectedList(QVector<QModelIndex> newList);
private:
    /*
     * vector field attribute that stores QModelIndex
     * The QModelIndex class is used to locate data in a data model
     */
    QVector<QModelIndex> STORED_INDEXES; //this is the user selected list
    bool checkEmpty(QVector<QModelIndex> inputList);
};

#endif // USERSELECTEDLIST_H
