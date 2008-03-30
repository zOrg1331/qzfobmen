RESOURCES += src.qrc
SOURCES += qfobmen.cpp \
           main.cpp \
 user.cpp \
 commentdialog.cpp \
 chatwindow.cpp \
 filethread.cpp \
 fileserver.cpp \
 chatthread.cpp \
 chatserver.cpp
HEADERS += qfobmen.h \
 user.h \
 filethread.h \
 fileserver.h \
 chatthread.h \
 chatserver.h \
 chatwindow.h \
 commentdialog.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/qfobmen
QT += network
