#pragma once


#include <QMainWindow>
#include <QStringList>
#include <QSettings>

#include "ui_mainwindow.h"

class TrackModel;
class QDir;
class FilesViewer;
class DiscogsViewer;


class MainWindow : public QMainWindow, Ui_MainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void importDiscogsTagsToModel();
	void currentPageChanged(const int index);

	void goToFilesPage();
	void goToDiscogsPage();

	void search();
	void help();

private:
	void init();
	void initToolBar();
	void initConnections();

	static QString actionShortcutToString(const QAction* action);

	QSettings m_settings;

	QLineEdit* m_searchLine;

	FilesViewer* m_filesViewer;
	DiscogsViewer* m_discogsViewer;

	QList<QAction*> m_filesActions;
	QList<QAction*> m_discogsActions;

};
