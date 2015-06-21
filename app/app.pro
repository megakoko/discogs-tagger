TEMPLATE=	app

TARGET	=	discogs-tagger

QT		+=	core gui network xml

DESTDIR 	=	bin
OBJECTS_DIR	=	bin/objects
UI_DIR		=	bin/ui
MOC_DIR		=	bin/moc
RCC_DIR		=	bin/rcc

SOURCES	+=	main.cpp\
			mainwindow.cpp \
			trackmodel.cpp \
			discogsviewer.cpp \
			discogsalbumlistmodel.cpp \
			discogsalbummodel.cpp \
			searchedit.cpp \
			track.cpp \
			batcheditdialog.cpp \
			filesviewer.cpp

HEADERS	+=	mainwindow.h \
			trackmodel.h \
			discogsviewer.h \
			discogsalbumlistmodel.h \
			discogsalbummodel.h \
			track.h \
			searchedit.h \
			batcheditdialog.h \
			trackmodelfields.h \
			filesviewer.h

FORMS	+=	mainwindow.ui \
			discogsviewer.ui \
			batcheditdialog.ui \
			filesviewer.ui

LIBS	+= -lTagLib -L../taglib

DEFINES += TAGLIB_NO_CONFIG
INCLUDEPATH += ..
INCLUDEPATH += ../taglib
INCLUDEPATH += ../taglib/toolkit

RESOURCES += resources.qrc
