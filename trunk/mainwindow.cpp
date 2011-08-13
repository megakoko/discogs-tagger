#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QSettings>

#include "trackmodel.h"

namespace OptionsNames {
static const QString startDir = "startDir";
static const QString headerState = "headerState";
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
	, m_settings("discogs-tagger")
{
    setupUi(this);

	init();
	initConnections();
}


MainWindow::~MainWindow()
{
	m_settings.setValue(OptionsNames::startDir, startDir);
	m_settings.setValue(OptionsNames::headerState, m_mainTable->horizontalHeader()->saveState());
}


void MainWindow::init()
{
	m_model = new TrackModel(this);

	m_mainTable->setModel(m_model);
	m_mainTable->horizontalHeader()->setMovable(true);
	updateTable();

	startDir = m_settings.value(OptionsNames::startDir).toString();
//	m_mainTable->horizontalHeader()->restoreState(m_settings.value(OptionsNames::headerState).toByteArray());
}


void MainWindow::initConnections()
{
	connect(m_addFiles, SIGNAL(clicked()), SLOT(addFiles()));
	connect(m_save, SIGNAL(clicked()), m_model, SLOT(saveTracks()));
}


void MainWindow::updateTable()
{
	QHeaderView* h = m_mainTable->horizontalHeader();

	h->setResizeMode(TrackModel::colTrack, QHeaderView::ResizeToContents);
	h->setResizeMode(TrackModel::colYear, QHeaderView::ResizeToContents);
}


void MainWindow::addFiles()
{
    const QString dirname =
			QFileDialog::getExistingDirectory(this,
											  tr("Choose search directory"),
											  startDir);
	if(dirname.isNull())
		return;

	startDir = dirname;

	QTime t;

	t.start();
	QStringList files;
	QDir d(dirname);
	findFiles(d, files);
	m_files << files;
	qDebug() << t.elapsed();

	t.start();
//	foreach(const QString& file, files){
//		m_model->addTrack(file);
//		qApp->processEvents();
//	}
	m_model->addTracks(files);
	qDebug() << t.elapsed();
	updateTable();
}


void MainWindow::findFiles(const QDir &dir, QStringList &files)
{
	QStringList nameFilter;
	nameFilter << "*.mp3"; // TODO


	const QStringList tmpFiles = dir.entryList(nameFilter, QDir::Files);
	foreach(const QString& file, tmpFiles)
		files << dir.absoluteFilePath(file);


	QStringList directories = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	foreach(const QString& d, directories)
		findFiles(QDir(dir.path() + QDir::separator() +  d), files);
}
