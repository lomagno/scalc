#-------------------------------------------------
#
# Project created by QtCreator 2015-12-01T15:41:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scalc
TEMPLATE = app
LIBS += -lglpk

SOURCES += main.cpp\
        mainwindow.cpp \
    datamodel.cpp \
    datatableview.cpp \
    costmatrixmodel.cpp \
    costmatrixtableview.cpp \
    newdialog.cpp \
    exportcostmatrixdialog.cpp \
    importcostmatrixdialog.cpp \
    exportdatadialog.cpp \
    aboutdialog.cpp \
    datadelegate.cpp \
    costmatrixdelegate.cpp \
    outputtablewidget.cpp

HEADERS  += mainwindow.h \
    datamodel.h \
    datatableview.h \
    costmatrixmodel.h \
    costmatrixtableview.h \
    newdialog.h \
    common.h \
    exportcostmatrixdialog.h \
    importcostmatrixdialog.h \
    exportdatadialog.h \
    aboutdialog.h \
    datadelegate.h \
    costmatrixdelegate.h \
    outputtablewidget.h

FORMS    += mainwindow.ui \
    newdialog.ui \
    exportcostmatrixdialog.ui \
    importcostmatrixdialog.ui \
    exportdatadialog.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc

win32:RC_FILE = application_icon.rc

OTHER_FILES += \
    application_icon.rc

