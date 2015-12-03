TARGET = te_server

QMAKE_CXXFLAGS += -std=c++11 -fopenmp

QMAKE_LIBDIR = ../Shared ../Satellite_Utils ../../Utils ../../CoordinateSystems -fopenmp

LIBS = -lboost_system -lboost_thread -lboost_regex \
       -lShared -lUtils -lCoordinateSystems -lSatelliteUtils

HEADERS += \
    include/Connection.h \
    include/TargetEnvironmentServer.h \
    include/ComputeParams.h

SOURCES += \
    src/Connection.cpp \
    src/example_server.cpp \
    src/SectorCheck.cpp \
    src/TargetEnvironmentServer.cpp \
    src/ComputeParams.cpp
