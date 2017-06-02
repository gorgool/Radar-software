TARGET = ConfigManager
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11

LIBS = -lrapidjson -lboost_system -lboost_filesystem

INCLUDEPATH += \
    ../

HEADERS += \
    ConfigManager.h

SOURCES += \
    ConfigManager.cpp
