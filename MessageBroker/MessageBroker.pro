TARGET = message_broker
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++14

QMAKE_LIBDIR = ../ConfigManager
LIBS = -lconfig_manager -lzmq

INCLUDEPATH += \
    ../ \
    ../Libraries/zmq/include/

HEADERS += \
    Server.h \
    Client.h \
    Message.h \
    HeartbeatMessage.h

SOURCES += \
    Server.cpp \
    Client.cpp
