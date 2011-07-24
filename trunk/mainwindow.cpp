#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include "trackmodel.h"


#include <taglib/fileref.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);


	init();
	initConnections();
}


void MainWindow::init()
{
	m_model = new TrackModel(this);

	m_mainTable->setModel(m_model);
	m_mainTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_mainTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);

}


void MainWindow::initConnections()
{
	connect(m_addFiles, SIGNAL(clicked()), SLOT(addFiles()));
}


void MainWindow::addFiles()
{
    const QString dirname =
			QFileDialog::getExistingDirectory(this,
											  tr("Choose search directory"),
											  "/media/crazy/music/");
	if(dirname.isNull())
		return;

	QTime t;

	t.start();
	QStringList files;
	QDir d(dirname);
	findFiles(d, files);
	qDebug() << t.elapsed();

	t.start();
//	foreach(const QString& file, files){
//		m_model->addTrack(file);
//		qApp->processEvents();
//	}
	m_model->addTracks(files);
	qDebug() << t.elapsed();
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
