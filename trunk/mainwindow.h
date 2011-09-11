#pragma once


#include <QMainWindow>
#include <QStringList>
#include <QSettings>

#include "ui_mainwindow.h"

class TrackModel;
class QDir;
class DiscogsViewer;


class MainWindow : public QMainWindow, Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
    void addFiles();
	void importDiscogsTagsToModel();
	void currentPageChanged(const int index);

	void goToFilesPage();
	void goToDiscogsPage();

	void search();

	void save();


private:
	void init();
	void initConnections();

	void findFiles(const QDir& dir, QStringList& files);
	void updateTable();

	static QString actionShortcutToString(const QAction* action);

	// Start directory for 'Open Directory' or 'Open File' dialog.
	QString m_startDir;

	TrackModel* m_model;

	QSettings m_settings;

	QLineEdit* m_searchLine;

	DiscogsViewer* m_discogsViewer;

	QList<QAction*> m_filesActions;
	QList<QAction*> m_discogsActions;
};
