TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH = include

HEADERS += \
    ExecTime.hpp \
    FormatString.h \
    Logger.h \
	
SOURCES += \
    src/Logger.cpp
