#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTreeView>
#include <QStandardItemModel>
#include <QList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Discogs Tagger");
	MainWindow w;
    w.show();


    return a.exec();
}
