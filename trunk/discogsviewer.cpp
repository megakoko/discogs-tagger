#include "discogsviewer.h"
#include <QDebug>
#include <QRegExp>
#include <QtXml>
#include <QtNetwork>
#include <QFile>

#include "discogsalbumlistmodel.h"
#include "discogsalbummodel.h"


const QString DiscogsViewer::albumListRequest =
		"http://api.discogs.com/search?type=releases&f=xml&q=%1";
const QString DiscogsViewer::albumRequest =
		"http://api.discogs.com/release/%1?f=xml";


DiscogsViewer::DiscogsViewer(QWidget *parent)
	: QWidget(parent)
	, manager(new QNetworkAccessManager(this))
	, m_listModel(new DiscogsAlbumListModel(this))
	, m_tableModel(new DiscogsAlbumModel(this))

{
	setupUi(this);

	// Prevent collapsing of the table section.
	splitter->setCollapsible(1, false);

	// Setting up models.
	m_albumList->setModel(m_listModel);

	m_albumTable->setModel(m_tableModel);
	m_albumTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);

	QHeaderView* hor = m_albumTable->horizontalHeader();
	hor->setResizeMode(QHeaderView::Stretch);
	hor->setResizeMode(DiscogsAlbumModel::Year, QHeaderView::ResizeToContents);
	hor->setResizeMode(DiscogsAlbumModel::Position, QHeaderView::ResizeToContents);

	// Manually connecting signals.
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			SLOT(replyFinished(QNetworkReply*)));
	connect(m_albumList, SIGNAL(activated(QModelIndex)),
			SLOT(albumDoubleClicked(QModelIndex)));
}


void DiscogsViewer::restoreState(const QByteArray& state)
{
	splitter->restoreState(state);
}


QByteArray DiscogsViewer::saveState() const
{
	return splitter->saveState();
}


void DiscogsViewer::search(const QString& input)
{
	QString text = input.simplified();
	if(!text.isEmpty())
	{
		const int releaseNumber = DiscogsAlbumListModel::releaseNumberFromUrl(text);
		if(releaseNumber != DiscogsAlbumListModel::NO_RELEASE_NUMBER)
			loadAlbum(releaseNumber);
		else
			loadAlbumList(text.replace(' ', '+'));
	}
}


QList<Track> DiscogsViewer::trackList() const
{
	return m_tableModel->trackList();
}


void DiscogsViewer::loadAlbumList(const QString &text)
{
	const QUrl& url = QUrl(albumListRequest.arg(text));
	manager->get(QNetworkRequest(url));
	emit statusChanged(tr("Loading album list"));
}


void DiscogsViewer::loadAlbum(int releaseNumber)
{
	const QUrl& url = QUrl(albumRequest.arg(releaseNumber));
	manager->get(QNetworkRequest(url));
	emit statusChanged(tr("Loading album"));
}



void DiscogsViewer::replyFinished(QNetworkReply *reply)
{
	bool successfullyLoadedReply = false;

	if(reply->error() == QNetworkReply::NoError)
	{
		emit statusChanged(tr("Finished loading"));

		const QByteArray& bytes = reply->readAll();
		const QString& string(QString::fromUtf8(bytes));


		QDomDocument doc;
		doc.setContent(string);
		const QDomNode& firstNode = doc.documentElement().firstChild();

		if(!firstNode.isNull())
		{
			const QDomElement& elem = firstNode.toElement();
			const QString responseType = elem.tagName().toLower();

			if(responseType == "searchresults")
			{
				m_listModel->setAlbums(string);
				successfullyLoadedReply = true;
			}
			else if(responseType == "release")
			{
				m_tableModel->setAlbum(string);
				successfullyLoadedReply = true;
			}
			else
				qWarning() << "UNKNOWN RESPONSE";
		}
	}


	if(!successfullyLoadedReply)
	{
		emit statusChanged(tr("Error has occured during loading."));
		qWarning() << "Error occured!";
	}
}


void DiscogsViewer::albumDoubleClicked(const QModelIndex &index)
{
	const QString& url = m_listModel->data(index, Qt::UserRole).toString();
	const int releaseNo = DiscogsAlbumListModel::releaseNumberFromUrl(url);

	loadAlbum(releaseNo);
}


void DiscogsViewer::moveUp()
{
	const QModelIndexList& indexes = m_albumTable->selectionModel()->selectedRows(0);
	if(indexes.size() == 1)
		m_tableModel->moveUp(indexes.first());
}


void DiscogsViewer::moveDown()
{
	const QModelIndexList& indexes = m_albumTable->selectionModel()->selectedRows(0);
	if(indexes.size() == 1)
		m_tableModel->moveDown(indexes.first());
}


void DiscogsViewer::join()
{
	const QModelIndexList& indexes = m_albumTable->selectionModel()->selectedRows(0);
	if(indexes.size() >= 2)
		m_tableModel->joinItems(indexes);
}


void DiscogsViewer::remove()
{
	const QModelIndexList& indexes = m_albumTable->selectionModel()->selectedRows(0);
	if(indexes.size() == 1)
		m_tableModel->removeItem(indexes.first());
}
