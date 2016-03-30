TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11 -O2

HEADERS += \
    SearchArea.h \
    Target.h \
    Errors.h \
    Messages.h \
    TargetTable.h

SOURCES += \
    TargetTable.cpp
