TEMPLATE = lib
CONFIG = staticlib
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH = include

HEADERS += \
    GetValueWrapper.hpp \
    ExecTime.hpp \
    FormatString.h \
    Logger.h \
    EigenHeaders.h
	
SOURCES += \
    Logger.cpp
