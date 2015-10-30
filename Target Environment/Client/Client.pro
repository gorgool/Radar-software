QMAKE_CXXFLAGS += -std=c++11
TARGET = te_client

QMAKE_LIBDIR = ../Shared ../../Utils

LIBS = -lboost_system -lboost_thread -lboost_regex \
        -lShared -lUtils

HEADERS += \
    include/TargetEnvironmentClient.h

SOURCES += \
    src/example_client.cpp \
    src/TargetEnvironmentClient.cpp
