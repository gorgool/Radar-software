TARGET = logger
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++14

QMAKE_LIBDIR = ../ConfigManager
LIBS = -lconfig_manager

INCLUDEPATH += \
    ../ \
    ../Libraries/

HEADERS += \
    Logger.h

SOURCES += \
    Logger.cpp
