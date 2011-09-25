#include "discogsalbummodel.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDate>
#include <QRegExp>
#include <QStringList>


DiscogsAlbumModel::DiscogsAlbumModel(QObject* parent)
	: QAbstractTableModel(parent)
{

}


void DiscogsAlbumModel::setAlbum(const QString& discogsResponse)
{
	m_tracks.clear();

	QDomDocument doc;
	doc.setContent(discogsResponse);
	QDomElement release = doc.firstChild().firstChild().toElement();


	const QString joinText(" feat. ");

	m_album = release.firstChildElement("title").text();
	m_year  = release.firstChildElement("released").text().left(4).toInt();
	m_genres= genres(release.firstChildElement("genres")).join(", ");


	m_albumArtists = trackArtists(release.firstChildElement("artists")).join(joinText);


	QDomNodeList trackList = release.elementsByTagName("track");
	for(int i = 0; i < trackList.size(); ++i)
	{
		QDomElement track = trackList.at(i).toElement();
		if(track.isNull())
			continue;

		QDomElement artists = track.firstChildElement("artists");
		m_tracks << Track(track.firstChildElement("position").text(),
						  trackArtists(artists).join(joinText),
						  track.firstChildElement("title").text());
	}
	// TODO:
	reset();
}


QList<Track> DiscogsAlbumModel::trackList() const
{
	return m_tracks;
}


const QString& DiscogsAlbumModel::album() const
{
	return m_album;
}


const QString& DiscogsAlbumModel::genre() const
{
	return m_genres;
}


int DiscogsAlbumModel::year() const
{
	return m_year;
}


QVariant DiscogsAlbumModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(index.row() < m_tracks.size());

	const int row = index.row();
	switch(role)
	{
	case Qt::DisplayRole:
		if(index.column() == 0)
			return m_tracks[row].artist;
		else if(index.column() == 1)
			return m_tracks[row].title;
		else if(index.column() == 2)
			return m_album;
		break;
	}

	return QVariant();
}


QVariant DiscogsAlbumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical || role != Qt::DisplayRole)
		return QAbstractTableModel::headerData(section, orientation, role);

	switch(section)
	{
	case 0:
		return tr("Artist");
		break;
	case 1:
		return tr("Title");
		break;
	case 2:
		return tr("Album");
		break;
	}

	return QVariant();
}


int DiscogsAlbumModel::rowCount(const QModelIndex &) const
{
	return m_tracks.size();
}


int DiscogsAlbumModel::columnCount(const QModelIndex &) const
{
	return 3;
}



void DiscogsAlbumModel::moveUp(const QModelIndex& item)
{
	const int row = item.row();
	if(row == 0)
		return;

	beginMoveRows(item.parent(), row, row, item.parent(), row-1);
	m_tracks.swap(row, row-1);
	endMoveRows();
}


void DiscogsAlbumModel::moveDown(const QModelIndex& item)
{
	const int row = item.row();
	if(row == m_tracks.size()-1)
		return;

	// 'row+2' is an index of element, which will be
	// 'parent' (i.e. will appear after) of the element with index 'row'.
	beginMoveRows(item.parent(), row, row, item.parent(), row+2);
	m_tracks.swap(row, row+1);
	endMoveRows();
}


void DiscogsAlbumModel::removeItem(const QModelIndex &index)
{
	if(!index.isValid())
		return;

	const int row = index.row();

	beginRemoveRows(index.parent(), row, row);
	m_tracks.removeAt(row);
	endRemoveRows();
}


void DiscogsAlbumModel::joinItems(const QModelIndexList& list)
{
	Q_ASSERT(list.count() > 2);

	for(int i = 1; i < list.count(); ++i)
		m_tracks[list.first().row()].joinWith(m_tracks.at(list.at(i).row()));

	for(int i = list.count()-1; i >= 1; --i)
		m_tracks.removeAt(list.at(i).row());

	reset();
}

// Fix method name.
QStringList DiscogsAlbumModel::trackArtists(const QDomElement &track) const
{
	QStringList result;
//	QDomElement artists = track.firstChildElement("artists");
	const QDomElement& artists = track;	// FIXIT.
	QDomNodeList artistList = artists.elementsByTagName("artist");

	for(int i = 0; i < artistList.size(); ++i)
	{
		QDomElement artist = artistList.at(i).toElement();

		QDomElement anv = artist.firstChildElement("anv");
		if(!anv.isNull() && !anv.text().isEmpty())
		{
			result << moveTheArticleToStart(anv.text());
			continue;
		}

		QDomElement name = artist.firstChildElement("name");
		if(!name.isNull() && !name.text().isEmpty())
			result << moveTheArticleToStart(name.text());
	}


	if(result.isEmpty())
	{
		if(m_albumArtists.isEmpty())
			result = QStringList(tr("No artist"));
		else
			result = QStringList(m_albumArtists);
	}
	return result;
}


QStringList DiscogsAlbumModel::genres(const QDomElement& genres) const
{
	QStringList result;

	QDomNodeList genreList = genres.elementsByTagName("genre");
	for(int i = 0; i < genreList.size(); ++i)
	{
		const QDomElement& genre = genreList.at(i).toElement();
		if(!genre.isNull())
			result << genre.text();
	}
	if(result.isEmpty())
		result << tr("No genre");

	return result;
}


QString DiscogsAlbumModel::moveTheArticleToStart(const QString& text) const
{
	const QString the = ", The";
	QString result = text.simplified();
	if(result.endsWith(the, Qt::CaseInsensitive))
	{
		result.chop(the.size());
		result.prepend("The ");
	}
	return result;
}
