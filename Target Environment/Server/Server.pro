TARGET = te_server

QMAKE_CXXFLAGS += -std=c++11

QMAKE_LIBDIR = ../Shared ../Satellite_Utils ../../Utils ../../CoordinateSystems

LIBS = -lboost_system -lboost_thread -lboost_regex \
       -lShared -lUtils -lCoordinateSystems -lSatelliteUtils

HEADERS += \
    include/Connection.h \
    include/TargetEnvironmentServer.h

SOURCES += \
    src/Connection.cpp \
    src/example_server.cpp \
    src/SectorCheck.cpp \
    src/TargetEnvironmentServer.cpp
