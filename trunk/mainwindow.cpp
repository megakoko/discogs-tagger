#include "mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QLineEdit>
#include <QDesktopServices>
#include <QUrl>

#include "filesviewer.h"
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
	, m_filesViewer(new FilesViewer(this))
	, m_discogsViewer(new DiscogsViewer(this))
{
	setupUi(this);

	init();
	initToolBar();
	initConnections();
}


MainWindow::~MainWindow()
{
	// Saving settings.
	m_settings.setValue(OptionsNames::startDir, m_filesViewer->startDir());
	m_settings.setValue(OptionsNames::headerState,
						m_filesViewer->horizontalHeaderState());
	m_settings.setValue(OptionsNames::windowGeometry, saveGeometry());
	m_settings.setValue(OptionsNames::discogsState,
						m_discogsViewer->saveState());
}


void MainWindow::init()
{
	m_stackedWidget->addWidget(m_filesViewer);
	m_stackedWidget->addWidget(m_discogsViewer);


	// Pressing F1 will open Project's web page.
	QAction* action = new QAction(this);
	action->setShortcut(Qt::Key_F1);
	connect(action, SIGNAL(triggered()), SLOT(help()));
	addAction(action);


	// Restoring settings.
	m_filesViewer->setStartDir(m_settings.value(OptionsNames::startDir).toString());
	m_filesViewer->setHorizontalHeaderState(
				m_settings.value(OptionsNames::headerState).toByteArray());
	restoreGeometry(m_settings.value(OptionsNames::windowGeometry).toByteArray());
	m_discogsViewer->restoreState(
				m_settings.value(OptionsNames::discogsState).toByteArray());
}


void MainWindow::initToolBar()
{
	QAction* action;

	m_toolBar->setMaximumHeight(30);


	// Files View actions.
	action = m_toolBar->addAction(QIcon(":/icons/right"), tr("Go to Discogs View"), this, SLOT(goToDiscogsPage()));
	action->setShortcut(Qt::CTRL | Qt::Key_Tab);
	action->setStatusTip(tr("Move to the Discogs View") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/add"), tr("Add files"), m_filesViewer, SLOT(addFiles()));
	action->setShortcut(Qt::CTRL | Qt::Key_A);
	action->setStatusTip(tr("Open a dialog to find some audio tracks to add") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/remove"), tr("Clear"), m_filesViewer, SLOT(clear()));
	action->setShortcut(Qt::CTRL | Qt::Key_C);
	action->setStatusTip(tr("Clear track list") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/save"), tr("Save"), m_filesViewer, SLOT(save()));
	action->setShortcut(Qt::CTRL | Qt::Key_S);
	action->setStatusTip(tr("Save changes") + actionShortcutToString(action));
	m_filesActions << action;

	action = m_toolBar->addAction(QIcon(":/icons/batch-edit"), "Batch edit", m_filesViewer, SLOT(batchEdit()));
	action->setStatusTip(tr("Batch edit track tags"));
	m_filesActions << action;


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
}


void MainWindow::initConnections()
{
	connect(m_discogsViewer, SIGNAL(statusChanged(QString)),
			m_statusBar, SLOT(showMessage(QString)));
	connect(m_stackedWidget, SIGNAL(currentChanged(int)), SLOT(currentPageChanged(int)));
	connect(m_searchLine, SIGNAL(returnPressed()), SLOT(search()));
	connect(m_filesViewer, SIGNAL(sendMessage(QString,int)),
			m_statusBar, SLOT(showMessage(QString,int)));

	currentPageChanged(0);
}


void MainWindow::search()
{
	m_discogsViewer->search(m_searchLine->text());
}


void MainWindow::importDiscogsTagsToModel()
{
	m_filesViewer->importTags(m_discogsViewer->trackList());
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
	m_stackedWidget->setCurrentIndex(1);
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
