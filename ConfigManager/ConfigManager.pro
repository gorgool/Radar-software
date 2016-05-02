TARGET = config_manager
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11

LIBS = -lboost_filesystem -lboost_system -lrapidjson

INCLUDEPATH += \
    ../

HEADERS += \
    ConfigManager.h

SOURCES += \
    ConfigManager.cpp
