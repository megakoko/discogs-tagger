#include <QtGui/QApplication>
#include "mainwindow.h"

#include "discogsviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Discogs Tagger");
	MainWindow w;
    w.show();

    return a.exec();
}
