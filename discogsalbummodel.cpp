#include "discogsalbummodel.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDate>
#include <QRegExp>
#include <QStringList>


using DiscogsAlbumModelFields::Field;


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

	const QString& album = release.firstChildElement("title").text();
	const int year  = release.firstChildElement("released").text().left(4).toInt();
	const QString& genres = getGenres(release.firstChildElement("genres")).join(", ");


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
						  track.firstChildElement("title").text(),
						  album,
						  genres,
						  year);
	}
	// TODO:
	reset();
}


QList<Track> DiscogsAlbumModel::trackList() const
{
	return m_tracks;
}


QVariant DiscogsAlbumModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(index.row() < m_tracks.size());

	const int row = index.row();
	switch(role)
	{
	using namespace DiscogsAlbumModelFields;
	case Qt::DisplayRole:
		switch(index.column())
		{
		case Position:
			return m_tracks[row].position;
		case Artist:
			return m_tracks[row].artist;
		case Title:
			return m_tracks[row].title;
		case Album:
			return m_tracks[row].album;
		case Year:
			return m_tracks[row].year;
		case Genre:
			return m_tracks[row].genre;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}

	return QVariant();
}


QVariant DiscogsAlbumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		using namespace DiscogsAlbumModelFields;
		switch(section)
		{
		case Position:
			return "#";
		case Artist:
			return tr("Artist");
		case Title:
			return tr("Title");
		case Album:
			return tr("Album");
		case Year:
			return tr("Year");
		case Genre:
			return tr("Genre");
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}

	return QAbstractTableModel::headerData(section, orientation, role);
}


int DiscogsAlbumModel::rowCount(const QModelIndex &) const
{
	return m_tracks.size();
}


int DiscogsAlbumModel::columnCount(const QModelIndex &) const
{
	return DiscogsAlbumModelFields::LastItem;
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


void DiscogsAlbumModel::changeItems(const QModelIndexList& list, Field column,
									const QVariant& value)
{
	for(int i = 0; i < list.count(); ++i)
	{
		using namespace DiscogsAlbumModelFields;
		Track& track = m_tracks[list[i].row()];
		switch(column)
		{
		case Artist:
			track.artist = value.toString();
			break;
		case Album:
			track.album = value.toString();
			break;
		case Title:
			track.title = value.toString();
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}
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


QStringList DiscogsAlbumModel::getGenres(const QDomElement& genres) const
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
