QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment th0e following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8

SOURCES += \
    ActiveSection.cpp \
    Dataset.cpp \
    FootDetect.cpp \
    IMUPitchDetect.cpp \
    JumpFootDetect.cpp \
    MyThread.cpp \
    MyTool.cpp \
    filterdata.cpp \
    main.cpp \
    mainwindow.cpp \
    mypushbutton.cpp \
    tcpclient.cpp

HEADERS += \
    ActiveSection.h \
    Configure.h \
    Dataset.h \
    FootDetect.h \
    IMUPitchDetect.h \
    JumpFootDetect.h \
    MyDataStruct.h \
    MyThread.h \
    MyTool.h \
    filterdata.h \
    mainwindow.h \
    mypushbutton.h \
    tcpclient.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc



#win32: LIBS += -LD:/anaconda/envs/tfgpu2_1/libs/ -lpython37

#INCLUDEPATH += D:/anaconda/envs/tfgpu2_1/include
#DEPENDPATH += D:/anaconda/envs/tfgpu2_1/include

#win32:!win32-g++: PRE_TARGETDEPS += D:/anaconda/envs/tfgpu2_1/libs/python37.lib
##else:win32-g++: PRE_TARGETDEPS += D:/anaconda/envs/tfgpu2_1/libs/libpython37.a
