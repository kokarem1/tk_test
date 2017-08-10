TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    passivesock.c \
    errexit.c

HEADERS += \
    passivesock.h \
    errexit.h

