TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11 -O2
INCLUDEPATH = include

HEADERS += \
    catalog_tle.h \
    norad_base.h \
    noradCircl.h \
    noradSDP4.h \
    noradSGDP4.h \
    noradSGP4.h \
    utils.h \
    vector3.h

SOURCES += \
    src/catalog_tle.cpp \
    src/norad_base.cpp \
    src/noradCircl.cpp \
    src/noradSDP4.cpp \
    src/noradSGDP4.cpp \
    src/noradSGP4.cpp \
    src/utils.cpp \
    src/vector3.cpp
