TARGET = examplewidget
QT += core gui widgets

INCLUDEPATH += include

HEADERS += include/abstractdividerslider.h
HEADERS += include/piechartslider.h
HEADERS += example/examplewidget.h

SOURCES += src/abstractdividerslider.cpp
SOURCES += src/piechartslider.cpp
SOURCES += example/examplewidget.cpp
SOURCES += example/main.cpp
