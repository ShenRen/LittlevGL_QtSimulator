QT += core gui widgets
TEMPLATE = app
CONFIG += c++11
CONFIG += no_keywords

SOURCES += \
    mouse_cursor_icon.c \
    pcfiles.cpp \
    qtsimulator.cpp \
    simmain.cpp

HEADERS += \
    pcfiles.h \
    qtsimulator.h \
    qtsimulator.hpp

#添加PC文件支持
DEFINES += PC_FILES=1

#界面模拟器
DEFINES += __GUI_Simulater__

#屏幕尺寸 28 35 40 43
DEFINES += SCREEN_SIZE=35

include($$PWD/../LittlevGL/LittlevGL.pri)
include($$PWD/../LittlevGL_CPPPort/LittlevGL_CPPPort.pri)
include($$PWD/../MarlinUI/MarlinUI.pri)
include($$PWD/../MarlinSimulator/MarlinSimulator.pri)

MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj

DESTDIR             = $$PWD/../bin
#DLLDESTDIR          = $$PWD/bin

