#-------------------------------------------------
#
# Project created by QtCreator 2011-06-24T21:43:01
#
#-------------------------------------------------

QT		+= core gui

TARGET	= discogs-tagger
TEMPLATE= app


SOURCES	+= main.cpp\
			mainwindow.cpp \
			trackmodel.cpp

HEADERS	+= mainwindow.h \
			trackmodel.h

FORMS	+= mainwindow.ui

LIBS	+= -ltag
