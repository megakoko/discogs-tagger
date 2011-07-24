#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTreeView>
#include <QStandardItemModel>
#include <QList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    return a.exec();
}
