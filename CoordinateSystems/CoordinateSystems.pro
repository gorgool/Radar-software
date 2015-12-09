TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH = include

HEADERS += \
    CSPoints.h \
    CSTransform.h \
    CSUtils.h

SOURCES += \
    src/CSTransform.cpp
