RESOURCES += src.qrc
SOURCES += qfobmen.cpp \
           main.cpp
HEADERS += qfobmen.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/qfobmen
QT += network
