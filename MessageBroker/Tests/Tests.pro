TARGET = mb_test
TEMPALTE = app

QMAKE_CXXFLAGS += -std=c++11

QMAKE_LIBDIR += \
 	../ \
	../../ConfigManager 

LIBS = -lmessage_broker -lzmq -lboost_system -lconfig_manager -lboost_filesystem

INCLUDEPATH +=\
    ../ \
    ../../

SOURCES += \
    main.cpp
