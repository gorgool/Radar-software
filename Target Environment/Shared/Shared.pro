TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11 -O2

INCLUDEPATH = include

HEADERS += \
    ReferencePointDesc.h \
    TargetEnvironmentErrors.h \
    TargetEnvironmentMessages.h \
    TargetTable.h

SOURCES += \
    src/TargetTable.cpp
