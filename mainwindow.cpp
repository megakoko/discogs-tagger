#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QSettings>
#include <QLineEdit>
#include <QToolButton>
#include <QDesktopServices>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>

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

	setAcceptDrops(true);
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

	m_mainTable->setModel(m_model);
	m_mainTable->horizontalHeader()->setMovable(true);
	updateTable();

	QAction* action = new QAction(this);
	action->setShortcut(Qt::Key_F1);
	connect(action, SIGNAL(triggered()), SLOT(help()));
	addAction(action);

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

	m_toolBar->setMaximumHeight(30);

	// Files View actions.
	action = m_toolBar->addAction(QIcon(":/icons/right"), tr("Go to Discogs View"), this, SLOT(goToDiscogsPage()));
	action->setShortcut(Qt::CTRL | Qt::Key_Tab);
	action->setStatusTip(tr("Move to the Discogs View") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/add"), tr("Add files"), this, SLOT(addFiles()));
	action->setShortcut(Qt::CTRL | Qt::Key_A);
	action->setStatusTip(tr("Open a dialog to find some audio tracks to add") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/remove"), tr("Clear"), m_model, SLOT(clear()));
	action->setShortcut(Qt::CTRL | Qt::Key_C);
	action->setStatusTip(tr("Clear track list") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/save"), tr("Save"), this, SLOT(save()));
	action->setShortcut(Qt::CTRL | Qt::Key_S);
	action->setStatusTip(tr("Save changes") + actionShortcutToString(action));
	m_filesActions << action;

//	action = m_toolBar->addAction(QIcon(), "Batch change", m_discogsViewer, SLOT(batchChange()));
//	action->setStatusTip(tr("Batch change track tags"));
//	m_filesAction << action;


	// Discogs View actions.
	action = m_toolBar->addAction(QIcon(":/icons/left"), tr("Go to Files View"), this, SLOT(goToFilesPage()));
	action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab);
	action->setStatusTip(tr("Move to the Files View") + actionShortcutToString(action));
	m_discogsActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/import"), tr("Import tags"), this, SLOT(importDiscogsTagsToModel()));
	action->setShortcut(Qt::CTRL | Qt::Key_I);
	action->setStatusTip(tr("Import discogs tags to opened files") + actionShortcutToString(action));
	m_discogsActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/up"), QString::null, m_discogsViewer, SLOT(moveUp()));
	action->setShortcut(Qt::CTRL | Qt::Key_U);
	action->setStatusTip(tr("Move track up") + actionShortcutToString(action));
	m_discogsActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/down"), QString::null, m_discogsViewer, SLOT(moveDown()));
	action->setShortcut(Qt::CTRL | Qt::Key_D);
	action->setStatusTip(tr("Move track down") + actionShortcutToString(action));
	m_discogsActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/remove"), QString::null, m_discogsViewer, SLOT(remove()));
	action->setShortcut(Qt::Key_Delete);
	action->setStatusTip(tr("Remove track") + actionShortcutToString(action));
	m_discogsActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/add"), QString::null, m_discogsViewer, SLOT(join()));
	action->setStatusTip(tr("Join tracks") + actionShortcutToString(action));
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
	m_model->importTags(m_discogsViewer->albumModel()->trackList());
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



QString MainWindow::actionShortcutToString(const QAction* action)
{
	if(action->shortcut().isEmpty())
		return QString::null;

	return " (" + action->shortcut().toString(QKeySequence::NativeText) + ")";
}


void MainWindow::help()
{
	QDesktopServices::openUrl(QUrl("http://code.google.com/p/discogs-tagger/"));
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls())
		event->acceptProposedAction();
}


void MainWindow::dropEvent(QDropEvent *event)
{
	QStringList files;
	foreach(const QUrl& url, event->mimeData()->urls())
	{
		QFileInfo info(url.toString().remove("file://"));

		if(info.isDir())
			findFiles(QDir(info.filePath()), files);
		else if(info.suffix().toLower() == "mp3")
			files << info.filePath();
	}

	m_model->addTracks(files);
	updateTable();
	goToFilesPage();
}
