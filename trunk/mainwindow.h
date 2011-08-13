#pragma once


#include <QMainWindow>
#include <QStringList>
#include <QSettings>

#include "ui_mainwindow.h"

class TrackModel;
class QDir;


class MainWindow : public QMainWindow, Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
    void addFiles();

private:
	void init();
	void initConnections();

	void findFiles(const QDir& dir, QStringList& files);
	void updateTable();

	// Start directory for 'Open Directory' or 'Open File' dialog.
	QString startDir;

	TrackModel* m_model;

	QSettings m_settings;
	QStringList m_files;
};
