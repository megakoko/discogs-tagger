TEMPLATE=	app

TARGET	=	discogs-tagger

QT		+=	core gui network xml

SOURCES	+=	main.cpp\
			mainwindow.cpp \
			trackmodel.cpp \
			discogsviewer.cpp \
			discogsalbumlistmodel.cpp \
			discogsalbummodel.cpp \
			searchedit.cpp \
			structs.cpp

HEADERS	+=	mainwindow.h \
			trackmodel.h \
			discogsviewer.h \
			discogsalbumlistmodel.h \
			discogsalbummodel.h \
			structs.h \
			searchedit.h

FORMS	+=	mainwindow.ui \
			discogsviewer.ui

LIBS	+= -ltag

RESOURCES += resources.qrc
