#pragma once


#include <QMainWindow>
#include <QStringList>

#include "ui_mainwindow.h"

class TrackModel;
class QDir;


class MainWindow : public QMainWindow, Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void addFiles();

private:
	void init();
	void initConnections();

	void findFiles(const QDir& dir, QStringList& files);

	TrackModel* m_model;
};
