#-------------------------------------------------
#
# Project created by QtCreator 2015-10-21T14:49:31
#
#-------------------------------------------------
QT              += testlib charts
QT              += core gui
CONFIG          += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = TeamLibra
TEMPLATE = app

DISTFILES += \
    TeamLibra.pro.user

HEADERS += \
    gui/CustomSort.h \
    gui/ErrorEditDialog.h \
    gui/piechartwidget.h \
    gui/mainwindow.h \
    database/CSVReader.h \
    database/RecordsManager.h \
    database/QSortListIO.h\
    datamodel/TreeItem.h \
    datamodel/TreeModel.h \
    datamodel/PresentationTreeModel.h \
    datamodel/PublicationTreeModel.h \
    datamodel/GrantFundingTreeModel.h \
    datamodel/TeachingTreeModel.h \
    database/TestCSVReader.h \
    gui/qcustomplot.h \
    test/t2.h \
    tests/customsorttest.h \
    tests/qtestcsvreader.h \
    tests/qtestqsortlistio.h \
    gui/stackedbarchartwidget.h \
    gui/userselectedlist.h



SOURCES += \
    gui/CustomSort.cpp \
    gui/ErrorEditDialog.cpp \
    gui/piechartwidget.cpp \
    gui/main.cpp \
    gui/mainwindow.cpp \
    database/CSVReader.cpp \
    database/RecordsManager.cpp \
    database/QSortListIO.cpp \
    datamodel/TreeItem.cpp \
    datamodel/TreeModel.cpp \
    datamodel/PresentationTreeModel.cpp \
    datamodel/PublicationTreeModel.cpp \
    datamodel/GrantFundingTreeModel.cpp \
    datamodel/TeachingTreeModel.cpp \
    database/TestCSVReader.cpp \
    gui/qcustomplot.cpp \
    test/t2.cpp \
    tests/customsorttest.cpp \
    tests/qtestcsvreader.cpp \
    tests/qtestqsortlistio.cpp \
    gui/stackedbarchartwidget.cpp \
    gui/userselectedlist.cpp

FORMS += \
    gui/customsort.ui \
    gui/mainwindow.ui \
    gui/ErrorEditDialog.ui

RESOURCES += \
    images.qrc
