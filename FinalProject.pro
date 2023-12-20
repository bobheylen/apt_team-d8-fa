QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    XEnemy.cpp \
    controller/graphicviewcontroller.cpp \
    controller/textviewcontroller.cpp \
    controller/viewcontroller.cpp \
    controller/worldcontroller.cpp \
    controller/windowcontroller.cpp\
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    XEnemy.h \
    controller/graphicviewcontroller.h \
    controller/textviewcontroller.h \
    controller/viewcontroller.h \
    controller/worldcontroller.h \
    controller/windowcontroller.h\
    mainwindow.h \
    pathNode.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    images.qrc

QMAKE_CXXFLAGS += -fconcepts-diagnostics-depth=200

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../worldsource/release/ -lworld

INCLUDEPATH += $$PWD/../worldsource
DEPENDPATH += $$PWD/../worldsource
