TARGET = logger_test
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++14

QMAKE_LIBDIR +=
    ../../ConfigManager \
    ../

LIBS = -lconfig_manager -llogger

INCLUDEPATH += \
    ../../ \
    ../

SOURCES += \
    main.cpp \
    log_test.cpp
