QT       += core gui graphs quick graphswidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Source/geodesicfunctions.cpp \
    Source/main.cpp \
    Source/mainwindow.cpp \
    Source/secondwindow.cpp \
    Source/plottingfunctions.cpp

HEADERS += \
    Headers/geodesicfunctions.h \
    Headers/mainwindow.h \
    Headers/secondwindow.h \
    Headers/plottingfunctions.h

FORMS += \
    Forms/mainwindow.ui \
    Forms/secondwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
