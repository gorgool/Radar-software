TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += \
    ../ \

HEADERS += \
    CSPoints.h \
    CSTransform.h \
    CSUtils.h

SOURCES += \
    CSTransform.cpp
