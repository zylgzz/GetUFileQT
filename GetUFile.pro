#-------------------------------------------------
#
# Project created by QtCreator 2019-05-05T22:37:17
#
#-------------------------------------------------

QT       += core gui
CONFIG  += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = GetUFile
TEMPLATE = app


SOURCES += main.cpp\
        getufile.cpp

HEADERS  += getufile.h


LIBS += -L$$PWD/zlib1.2.8/zlib128-dll/lib/ -lzdll
INCLUDEPATH += $$PWD/zlib1.2.8/zlib128-dll/include
DEPENDPATH  += $$PWD/zlib1.2.8/zlib128-dll/include

LIBS += -L$$PWD/myQuazup/lib/ -lquazip
INCLUDEPATH += $$PWD/myQuazup/include
DEPENDPATH  += $$PWD/myQuazup/include
