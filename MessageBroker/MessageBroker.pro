TARGET - message_broker
TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11

QMAKE_LIBDIR = ../ConfigManager
LIBS = -lconfig_manager -lzmq

INCLUDEPATH += \
    ../

HEADERS += \
    Server.h \
    Client.h \
    Message.h \
    HeartbeatMessage.h

SOURCES += \
    Server.cpp \
    Client.cpp
