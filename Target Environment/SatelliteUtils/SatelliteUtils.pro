TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11 -O2

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
    catalog_tle.cpp \
    norad_base.cpp \
    noradCircl.cpp \
    noradSDP4.cpp \
    noradSGDP4.cpp \
    noradSGP4.cpp \
    utils.cpp \
    vector3.cpp
