#--------------------------------------------------#
#                                                  #
#  Project created by: Johannes de Lange 23689293  #
#                                                  #
#--------------------------------------------------#

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UDP_Dragon
TEMPLATE = app


SOURCES += main.cpp\
        dragon.cpp

HEADERS  += dragon.h

FORMS    += dragon.ui
