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
{
	setupUi(this);

	splitter->setCollapsible(1, false);

	// Setting up models.
	m_listModel = new DiscogsAlbumListModel(this);
	m_albumList->setModel(m_listModel);

	m_tableModel = new DiscogsAlbumModel(this);
	m_albumTable->setModel(m_tableModel);
	m_albumTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);

	QHeaderView* hor = m_albumTable->horizontalHeader();
	hor->setResizeMode(QHeaderView::Stretch);
	hor->setResizeMode(DiscogsAlbumModel::Year, QHeaderView::ResizeToContents);
	hor->setResizeMode(DiscogsAlbumModel::Position, QHeaderView::ResizeToContents);

	manager = new QNetworkAccessManager(this);

	// Manually connecting signals.
	connect(manager, SIGNAL(finished(QNetworkReply*)),
			SLOT(replyFinished(QNetworkReply*)));
	connect(m_albumList, SIGNAL(activated(QModelIndex)),
			SLOT(albumDoubleClicked(QModelIndex)));
}


const DiscogsAlbumModel* DiscogsViewer::albumModel() const
{
	return m_tableModel;
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
//	qDebug() << text;
	if(text.isEmpty())
		return;

	const int releaseNumber = DiscogsAlbumListModel::releaseNumberFromUrl(text);
//	qDebug() << releaseNumber;
	if(releaseNumber != DiscogsAlbumListModel::NO_RELEASE_NUMBER)
		loadAlbum(releaseNumber);
	else
		loadAlbumList(text.replace(' ', '+'));
}


void DiscogsViewer::loadAlbumList(const QString &text)
{
	QUrl url = QUrl(albumListRequest.arg(text));
	manager->get(QNetworkRequest(url));
	qDebug() << url;
	emit statusChanged(tr("Loading album list"));
}


void DiscogsViewer::loadAlbum(int releaseNumber)
{
	QUrl u = QUrl(albumRequest.arg(releaseNumber));
	manager->get(QNetworkRequest(u));
	qDebug() << u;
	emit statusChanged(tr("Loading album"));
}



void DiscogsViewer::replyFinished(QNetworkReply *reply)
{
	emit statusChanged();
	if(reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		QString string(QString::fromUtf8(bytes));


		QDomDocument doc;
		doc.setContent(string);
		QDomNode firstNode = doc.documentElement().firstChild();

		if(!firstNode.isNull())
		{
			QDomElement elem = firstNode.toElement();
			const QString responseType = elem.tagName().toLower();

			if(responseType == "searchresults")
				m_listModel->setAlbums(string);
			else if(responseType == "release")
				m_tableModel->setAlbum(string);
			else
				qWarning() << "UNKNOWN RESPONSE";
//			qDebug() << string;
		}
	}
	else
	{
		// TODO: handle errors here
		qWarning() << "error";
	}
}


void DiscogsViewer::albumDoubleClicked(const QModelIndex &index)
{
	const QString url = m_listModel->data(index, Qt::UserRole).toString();
	const int releaseNo = DiscogsAlbumListModel::releaseNumberFromUrl(url);

	loadAlbum(releaseNo);
}


void DiscogsViewer::moveUp()
{
	if(m_albumTable->selectionModel()->selectedRows(0).size() != 1)
		return;
	else
		m_tableModel->moveUp(m_albumTable->selectionModel()->currentIndex());
}


void DiscogsViewer::moveDown()
{
	if(m_albumTable->selectionModel()->selectedRows(0).size() != 1)
		return;
	else
		m_tableModel->moveDown(m_albumTable->selectionModel()->currentIndex());
}


void DiscogsViewer::join()
{
	// TODO.
	if(m_albumTable->selectionModel()->selectedRows(0).size() < 2)
		return;
	else
		m_tableModel->joinItems(m_albumTable->selectionModel()->selectedRows(0));
}


void DiscogsViewer::remove()
{
	if(m_albumTable->selectionModel()->selectedRows(0).size() != 1)
		return;
	else
		m_tableModel->removeItem(m_albumTable->selectionModel()->currentIndex());
}
