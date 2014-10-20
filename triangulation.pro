TEMPLATE = app
TARGET = triangulation

CONFIG += QtGui
QT += opengl

OBJECTS_DIR = bin

QMAKE_CXXFLAGS = -std=c++11 -Wall

macx {
    QMAKE_CXXFLAGS += -stdlib=libc++  
    QMAKE_LFLAGS += -lc++
}

CONFIG += precompile_header
PRECOMPILED_HEADER = src/stdafx.h

DEPENDPATH += src \
              visualization/headers \
              visualization/headers/common \
              visualization/headers/io \
              visualization/headers/visualization \

INCLUDEPATH += src \
               visualization/headers \

HEADERS += src/stdafx.h \
           src/viewer.h \
           src/contour_builder.h \
           src/edge_list.h \
           src/monotonic_triangulation.h \
           src/polygon.h \
           src/triangulation.h \
           src/util.h \

SOURCES += src/main.cpp \
           src/contour_builder.cpp \
           src/edge_list.cpp \
           src/monotonic_triangulation.cpp \
           src/polygon.cpp \
           src/triangulation.cpp \
           src/util.cpp \

LIBS += -Lvisualization -lvisualization
