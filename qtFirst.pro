TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp chunk.h

HEADERS += \
    chunk.h

LIBS+= -lGL -lGLU -lglut

DISTFILES += \
    TODO
