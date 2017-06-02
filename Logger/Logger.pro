TARGET = Logger
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++14

QMAKE_LIBDIR = ../ConfigManager
LIBS = -lConfigManager -lboost_filesystem

INCLUDEPATH += \
    ../ \
    ../Libraries/

HEADERS += \
    Logger.h

SOURCES += \
    Logger.cpp
