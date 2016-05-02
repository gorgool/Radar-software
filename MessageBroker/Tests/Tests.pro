TARGET = message_broker

QMAKE_CXXFLAGS += -std=c++11 -fopenmp

QMAKE_LIBDIR = ../

LIBS = -lmessage_broker

INCLUDEPATH +=\
    ../

SOURCES += \
    server_test.cpp
