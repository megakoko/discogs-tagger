#-------------------------------------------------
#
# Project created by QtCreator 2011-06-24T21:43:01
#
#-------------------------------------------------

QT		+= core gui network xml

TARGET	= discogs-tagger
TEMPLATE= app


SOURCES	+= main.cpp\
			mainwindow.cpp \
			trackmodel.cpp \
			discogsviewer.cpp \
			discogsalbumlistmodel.cpp \
    discogsalbummodel.cpp \
    searchedit.cpp

HEADERS	+= mainwindow.h \
			trackmodel.h \
			discogsviewer.h \
			discogsalbumlistmodel.h \
    discogsalbummodel.h \
    structs.h \
    searchedit.h

FORMS	+= mainwindow.ui \
			discogsviewer.ui

LIBS	+= -ltag

RESOURCES += \
    resources.qrc
