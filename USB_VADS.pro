QT       += core gui
QT       += network
QT       += charts
QT       += widgets
QT       += core5compat
QT       += sql
QT       += axcontainer
QT       += serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ExchangeSlots.cpp \
    ExchangeThread.cpp \
    FlexLogic.cpp \
    HWSettings.cpp \
    IZM.cpp \
    OSC.cpp \
    TestTHR.cpp \
    VectDiagram.cpp \
    crc16.cpp \
    main.cpp \
    mainwindow.cpp \
    settings_class.cpp \
    tflexconnection.cpp \
    tflexport.cpp \
    tlogiccell.cpp \
    tmodbusdfz.cpp \
    vadc_class.cpp

HEADERS += \
    ExchangeParams.h \
    ExchangeThread.h \
    FlexLogic.h \
    HWSettings.h \
    OSC.h \
    crc16.h \
    mainwindow.h \
    settings_class.h \
    tflexconnection.h \
    tflexport.h \
    tlogiccell.h \
    tmodbusdfz.h \
    vadc_class.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:{
    RC_ICONS = $$PWD/L011.ico
    VERSION = 1.0.0
    DEFINES += VERSION=\\\"$$VERSION\\\"
    QMAKE_TARGET_COMPANY = PGS
    QMAKE_TARGET_PRODUCT = V4Z
    QMAKE_TARGET_DESCRIPTION = V4Z_PC_control
    QMAKE_TARGET_COPYRIGHT = PGS
}

RESOURCES += \
    res.qrc
