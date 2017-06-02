TARGET = logger_test
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++14

QMAKE_LIBDIR += \
    ../../ConfigManager \
    ../

LIBS = -lConfigManager -lLogger -lboost_system -lboost_filesystem

INCLUDEPATH += \
    ../../ConfigManager \
    ../../ \
    ../

SOURCES += \
    main.cpp \
    log_test.cpp
