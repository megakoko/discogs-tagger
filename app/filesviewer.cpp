#include "filesviewer.h"


#include <QDebug>
#include <QFileDialog>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>


#include "trackmodel.h"
#include "batcheditdialog.h"


FilesViewer::FilesViewer(QWidget* parent)
	: QWidget(parent)
	, m_model(new TrackModel(this))
{
	setupUi(this);

	setAcceptDrops(true);
	init();
}


void FilesViewer::init()
{
	m_filesTable->setModel(m_model);
	m_filesTable->horizontalHeader()->setSectionsMovable(true);

	QHeaderView* h = m_filesTable->horizontalHeader();
	h->setSectionResizeMode(QHeaderView::Stretch);
	h->setSectionResizeMode(TrackModelFields::Position, QHeaderView::ResizeToContents);
	h->setSectionResizeMode(TrackModelFields::Year, QHeaderView::ResizeToContents);

	m_filesTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}


QString FilesViewer::startDir() const
{
	return m_startDir;
}


void FilesViewer::setStartDir(const QString &dir)
{
	m_startDir = dir;
}


QByteArray FilesViewer::horizontalHeaderState() const
{
	return m_filesTable->horizontalHeader()->saveState();
}


void FilesViewer::setHorizontalHeaderState(const QByteArray &state)
{
	m_filesTable->horizontalHeader()->restoreState(state);
}


void FilesViewer::addFiles()
{
	const QString dirname =
		QFileDialog::getExistingDirectory(this, tr("Choose search directory"), m_startDir);

	if(!dirname.isNull())
	{
		QStringList files;
		QDir d(dirname);

		findFiles(d, files);
		m_model->addTracks(files);

		// If user choosed /media/music/ArtistName/Album_1, start directory for the next
		// dialog will be  /media/music/ArtistName.
		if(d.cdUp())
			m_startDir = d.absolutePath();
		else
			m_startDir = dirname;
	}
}


void FilesViewer::clear()
{
	m_model->clear();
}


void FilesViewer::findFiles(const QDir &dir, QStringList &files)
{
	const QStringList tmpFiles = dir.entryList(QStringList(), QDir::Files);
	foreach(const QString& file, tmpFiles)
		files << dir.absoluteFilePath(file);


	QStringList directories =
		dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	foreach(const QString& d, directories)
		findFiles(QDir(dir.path() + QDir::separator() +  d), files);
}


void FilesViewer::save()
{
	if(!m_model->saveTracks())
		emit sendMessage(tr("Some of the tracks were not saved"));
}


void FilesViewer::batchEdit()
{
	const QModelIndexList& selectedIndexes = m_filesTable->selectionModel()->selectedRows(0);
	if(selectedIndexes.count() > 0)
	{
		BatchEditDialog d(this);
		if(d.exec() == QDialog::Accepted)
			m_model->batchEdit(selectedIndexes, d.selectedField(), d.enteredValue());
	}
}


void FilesViewer::importTags(const QList<Track> &tracks)
{
	m_model->importTags(tracks);
}


void FilesViewer::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls())
		event->acceptProposedAction();
}


void FilesViewer::dropEvent(QDropEvent *event)
{
	QStringList files;
	foreach(const QUrl& url, event->mimeData()->urls())
	{
		const QFileInfo info(url.toLocalFile());

		if(info.isDir())
			findFiles(QDir(info.filePath()), files);
		else if(info.isFile())
			files << info.filePath();
	}

	m_model->addTracks(files);
}
