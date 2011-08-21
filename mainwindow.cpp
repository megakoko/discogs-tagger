#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QSettings>
#include <QLineEdit>
#include <QToolButton>

#include "trackmodel.h"
#include "discogsalbummodel.h"
#include "discogsviewer.h"
#include "searchedit.h"


namespace OptionsNames {
static const QString startDir = "Start directory";
static const QString headerState = "Header state";
static const QString windowGeometry = "Window geometry";
static const QString discogsState = "Discogs view state";
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
	m_settings.setValue(OptionsNames::startDir, m_startDir);
	m_settings.setValue(OptionsNames::headerState,
						m_mainTable->horizontalHeader()->saveState());
	m_settings.setValue(OptionsNames::windowGeometry, saveGeometry());
	m_settings.setValue(OptionsNames::discogsState,
						m_discogsViewer->saveState());
}


void MainWindow::init()
{	
	m_model = new TrackModel(this);
	m_discogsViewer = new DiscogsViewer(this);

	m_stackedWidget->addWidget(m_discogsViewer);
	QList<QTabBar*> tabList = findChildren<QTabBar*>();
	if(!tabList.isEmpty())
		tabList[0]->setCurrentIndex(0);

	m_mainTable->setModel(m_model);
	m_mainTable->horizontalHeader()->setMovable(true);
	updateTable();

	m_startDir = m_settings.value(OptionsNames::startDir).toString();
//	m_mainTable->horizontalHeader()->restoreState(
//				m_settings.value(OptionsNames::headerState).toByteArray());
	restoreGeometry(m_settings.value(OptionsNames::windowGeometry).toByteArray());
	m_discogsViewer->restoreState(
				m_settings.value(OptionsNames::discogsState).toByteArray());
}


void MainWindow::initConnections()
{
	QAction* action;


	// Files View actions.
	action = m_toolBar->addAction(tr("Go to &&Discogs View"), this, SLOT(goToDiscogsPage()));
	action->setShortcut(Qt::CTRL | Qt::Key_Tab);
	m_filesActions << action;

	action = m_toolBar->addAction(tr("&&Add files"), this, SLOT(addFiles()));
	action->setStatusTip(tr("Open a dialog to find some audio tracks to add."));
	action->setShortcut(Qt::CTRL | Qt::Key_A);
	m_filesActions << action;

	action = m_toolBar->addAction(tr("&&Clear"), m_model, SLOT(clear()));
	m_filesActions << action;

	action = m_toolBar->addAction(tr("&&Save"), this, SLOT(save()));
	m_filesActions << action;


	// Discogs View actions.
	action = m_toolBar->addAction(tr("Go to &&Files View"), this, SLOT(goToFilesPage()));
	action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab);
	m_discogsActions << action;

	action = m_toolBar->addAction(tr("&&Import tags"), this, SLOT(importDiscogsTagsToModel()));
	m_discogsActions << action;

	action = m_toolBar->addAction(tr("Move &&up"), m_discogsViewer, SLOT(moveUp()));
	m_discogsActions << action;

	action = m_toolBar->addAction(tr("Move &&down"), m_discogsViewer, SLOT(moveDown()));
	m_discogsActions << action;

	action = m_toolBar->addAction(tr("&&Remove"), m_discogsViewer, SLOT(remove()));
	m_discogsActions << action;

	action = m_toolBar->addAction(tr("&&Join"), m_discogsViewer, SLOT(join()));
	m_discogsActions << action;


	// Hack to add a spacer to QToolBar.
	QWidget* searchWidget = new QWidget(m_toolBar);
	searchWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	m_searchLine = new SearchEdit(m_toolBar);
	// TODO: fix it.
	m_searchLine->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	m_searchLine->setMaximumWidth(300);
	m_searchLine->setMaximumHeight(23);

	QHBoxLayout* layout = new QHBoxLayout(searchWidget);
	layout->addStretch();
	layout->addWidget(m_searchLine);
	layout->setMargin(0);
	searchWidget->setLayout(layout);

	action = m_toolBar->addWidget(searchWidget);
	m_discogsActions << action;


	connect(m_discogsViewer, SIGNAL(statusChanged(QString)),
			m_statusBar, SLOT(showMessage(QString)));
	connect(m_stackedWidget, SIGNAL(currentChanged(int)), SLOT(currentPageChanged(int)));
	connect(m_searchLine, SIGNAL(returnPressed()), SLOT(search()));

	currentPageChanged(0);
}


void MainWindow::search()
{
	m_discogsViewer->search(m_searchLine->text());
}


void MainWindow::updateTable()
{
	QHeaderView* h = m_mainTable->horizontalHeader();

	h->setResizeMode(QHeaderView::Stretch);
	h->setResizeMode(TrackModel::colTrack, QHeaderView::ResizeToContents);
	h->setResizeMode(TrackModel::colYear, QHeaderView::ResizeToContents);
	m_mainTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
}


void MainWindow::addFiles()
{
    const QString dirname =
		QFileDialog::getExistingDirectory(this, tr("Choose search directory"), m_startDir);
	if(dirname.isNull())
		return;

	m_startDir = dirname;

	QStringList files;
	QDir d(dirname);
	findFiles(d, files);

	m_model->addTracks(files);
	updateTable();
}


void MainWindow::importDiscogsTagsToModel()
{
	const QString& album = m_discogsViewer->albumModel()->album();
	const QList<Track>& tracks = m_discogsViewer->albumModel()->trackList();
	const QString& genre = m_discogsViewer->albumModel()->genre();
	const int year = m_discogsViewer->albumModel()->year();

	m_model->importTags(tracks, album, genre, year);
}


void MainWindow::findFiles(const QDir &dir, QStringList &files)
{
	QStringList nameFilter;
	nameFilter << "*.mp3"; // TODO


	const QStringList tmpFiles = dir.entryList(nameFilter, QDir::Files);
	foreach(const QString& file, tmpFiles)
		files << dir.absoluteFilePath(file);


	QStringList directories =
		dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	foreach(const QString& d, directories)
		findFiles(QDir(dir.path() + QDir::separator() +  d), files);
}


void MainWindow::currentPageChanged(const int index)
{
	foreach(QAction* action, m_filesActions)
		action->setVisible(index == 0);

	foreach(QAction* action, m_discogsActions)
		action->setVisible(index == 1);
}


void MainWindow::goToFilesPage()
{
	m_stackedWidget->setCurrentIndex(0);
}


void MainWindow::goToDiscogsPage()
{
	if(m_searchLine->text().isEmpty() && m_mainTable->currentIndex().isValid())
	{
		const int currentRow = m_mainTable->currentIndex().row();
		const QString& text =
			m_model->data(m_model->index(currentRow, TrackModel::colAlbum)).toString() +
			' ' +
			m_model->data(m_model->index(currentRow, TrackModel::colArtist)).toString();
		m_searchLine->setText(text);
	}

	m_stackedWidget->setCurrentIndex(1);
}


void MainWindow::save()
{
	if(!m_model->saveTracks())
		m_statusBar->showMessage(tr("Some of the tracks were not saved"), 5000);
}
