TARGET = te_server

QMAKE_CXXFLAGS += -std=c++11 -fopenmp

QMAKE_LIBDIR = ../Shared ../SatelliteUtils ../../Utils ../../CoordinateSystems ../../ConfigManager ../../Logger

LIBS = -lboost_system -lboost_thread -lboost_regex \
       -lShared -lUtils -lCoordinateSystems -lSatelliteUtils -fopenmp -lConfigManager -lLogger

INCLUDEPATH +=\
    ../ \
    ../..

HEADERS += \
    Connection.h \
    Server.h \
    ComputeParams.h \
    SectorCheck.h

SOURCES += \
    Connection.cpp \
    example_server.cpp \
    SectorCheck.cpp \
    Server.cpp \
    ComputeParams.cpp
