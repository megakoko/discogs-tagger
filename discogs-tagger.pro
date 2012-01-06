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
			structs.cpp \
			batchchangedialog.cpp

HEADERS	+=	mainwindow.h \
			trackmodel.h \
			discogsviewer.h \
			discogsalbumlistmodel.h \
			discogsalbummodel.h \
			structs.h \
			searchedit.h \
			batchchangedialog.h

FORMS	+=	mainwindow.ui \
			discogsviewer.ui \
			batchchangedialog.ui

LIBS	+= -ltag

RESOURCES += resources.qrc
