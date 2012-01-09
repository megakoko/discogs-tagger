#include "discogsalbummodel.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QRegExp>
#include <QStringList>


DiscogsAlbumModel::DiscogsAlbumModel(QObject* parent)
	: QAbstractTableModel(parent)
{

}


void DiscogsAlbumModel::setAlbum(const QString& discogsResponse)
{
	beginResetModel();

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

	endResetModel();
}


QList<Track> DiscogsAlbumModel::trackList() const
{
	return m_tracks;
}


QVariant DiscogsAlbumModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(index.row() < m_tracks.size());

	QVariant result;

	const int row = index.row();
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case Position:
			result = m_tracks[row].position;
			break;
		case Artist:
			result = m_tracks[row].artist;
			break;
		case Title:
			result = m_tracks[row].title;
			break;
		case Album:
			result = m_tracks[row].album;
			break;
		case Year:
			result = m_tracks[row].year;
			break;
		case Genre:
			result = m_tracks[row].genre;
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}

	return result;
}


QVariant DiscogsAlbumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant result;

	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
		case Position:
			result = "#";
			break;
		case Artist:
			result = tr("Artist");
			break;
		case Title:
			result = tr("Title");
			break;
		case Album:
			result = tr("Album");
			break;
		case Year:
			result = tr("Year");
			break;
		case Genre:
			result = tr("Genre");
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}
	else
		result = QAbstractTableModel::headerData(section, orientation, role);

	return result;
}


int DiscogsAlbumModel::rowCount(const QModelIndex &) const
{
	return m_tracks.size();
}


int DiscogsAlbumModel::columnCount(const QModelIndex &) const
{
	return LastItem;
}


void DiscogsAlbumModel::moveUp(const QModelIndexList &items)
{
	// Starting with the highest index in list.
	for(int i = 0; i < items.count(); ++i)
	{
		const QModelIndex& item = items.at(i);
		const int row = item.row();

		if(row == 0)
			break;
		else
		{
			beginMoveRows(item.parent(), row, row, item.parent(), row-1);
			m_tracks.swap(row, row-1);
			endMoveRows();
		}
	}
}


void DiscogsAlbumModel::moveDown(const QModelIndexList &items)
{
	// Starting with the lowest index in list.
	for(int i = items.count()-1; i >= 0; --i)
	{
		const QModelIndex& item = items.at(i);
		const int row = item.row();

		if(row == m_tracks.size()-1)
			break;
		else
		{
			// 'row+2' is an index of element, which will be
			// 'parent' (i.e. will appear after) of the element with index 'row'.
			beginMoveRows(item.parent(), row, row, item.parent(), row+2);
			m_tracks.swap(row, row+1);
			endMoveRows();
		}
	}
}


void DiscogsAlbumModel::removeItem(const QModelIndexList &items)
{
	for(int i = items.count()-1; i >= 0; --i)
	{
		const QModelIndex& item = items.at(i);

		if(item.isValid())
		{
			const int row = item.row();

			beginRemoveRows(item.parent(), row, row);
			m_tracks.removeAt(row);
			endRemoveRows();
		}
	}
}


void DiscogsAlbumModel::joinItems(const QModelIndexList& items)
{
	Q_ASSERT(items.count() > 2);

	beginResetModel();

	for(int i = 1; i < items.count(); ++i)
		m_tracks[items.first().row()].joinWith(m_tracks.at(items.at(i).row()));

	for(int i = items.count()-1; i >= 1; --i)
		m_tracks.removeAt(items.at(i).row());

	endResetModel();
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
