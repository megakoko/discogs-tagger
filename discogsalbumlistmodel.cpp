#include "discogsalbumlistmodel.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QRegExp>
#include <QStringList>


const int DiscogsAlbumListModel::NO_RELEASE_NUMBER = -1;


DiscogsAlbumListModel::DiscogsAlbumListModel(QObject* parent)
	: QAbstractListModel(parent)
{

}


QVariant DiscogsAlbumListModel::data(const QModelIndex& index, int role) const
{
	Q_ASSERT(index.row() < m_albums.count());

	QVariant result;

	const int& row = index.row();
	switch(role)
	{
	case Qt::DisplayRole:
		result = m_albums[row].title;
		break;
	case Qt::UserRole:
		result = m_albums[row].url;
		break;
	}

	return result;
}


int DiscogsAlbumListModel::rowCount(const QModelIndex&) const
{
    return m_albums.count();
}


void DiscogsAlbumListModel::setAlbums(const QString& discogsResponse)
{
	beginResetModel();
	m_albums.clear();

    QDomDocument doc;
    doc.setContent(discogsResponse);

    QDomNodeList resultList = doc.elementsByTagName("result");
    for(int i = 0; i < resultList.size(); ++i)
    {
        QDomElement result = resultList.at(i).toElement();
        if(result.isNull())
            continue;

		m_albums << Album(result.firstChildElement("title").text().remove('\n'),
						  result.firstChildElement("uri").text());
	}
	endResetModel();
}


// getting release number
int DiscogsAlbumListModel::releaseNumberFromUrl(const QString& url)
{
	// Line start, then anything, then 'www.discogs.com', then anything,
	// then any number, then anything but number, then line end.
    QRegExp rx("^.*www\\.discogs\\.com.*/(\\d+)[^\\d]*$");
	int releaseNumber = NO_RELEASE_NUMBER;

	if(rx.indexIn(url) >= 0)
		releaseNumber = rx.capturedTexts().last().toInt();

	return releaseNumber;
}
