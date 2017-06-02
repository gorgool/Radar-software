QMAKE_CXXFLAGS += -std=c++11
TARGET = te_client

QMAKE_LIBDIR = ../Shared ../../Utils ../../ConfigManager ../../Logger

LIBS = -lConfigManager -lLogger -lboost_system -lboost_filesystem -lboost_thread -lboost_regex \
        -lShared -lUtils 

INCLUDEPATH += \
    ../ \
    ../..

HEADERS += \
    Client.h

SOURCES += \
    example_client.cpp \
    Client.cpp
