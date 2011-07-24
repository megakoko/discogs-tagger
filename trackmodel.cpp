#include "trackmodel.h"


#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>

enum {
	colTrack,
	colTitle,
	colArtist,
	colAlbum,
	colGenre,
	colYear,
	colLastElement
};



TrackModel::TrackModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}

// TODO: proper model resetting.
void TrackModel::addTrack(const QString& filename)
{
	using TagLib::FileRef;
	beginInsertRows(QModelIndex(),m_tracks.count(), m_tracks.count());
	m_tracks << QSharedPointer<TagLib::FileRef>(new TagLib::FileRef(filename.toUtf8().data()));
	endInsertRows();
}


void TrackModel::addTracks(const QStringList &filenames)
{
	using TagLib::FileRef;
	beginInsertRows(QModelIndex(),m_tracks.count(), m_tracks.count() + filenames.size() - 1);
	foreach(const QString& filename, filenames)
		m_tracks << QSharedPointer<TagLib::FileRef>(new TagLib::FileRef(filename.toUtf8().data()));
	endInsertRows();
}



int TrackModel::columnCount(const QModelIndex &) const
{
	return colLastElement;
}


int TrackModel::rowCount(const QModelIndex &) const
{
	return m_tracks.count();
}


QVariant TrackModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(m_tracks.count() > index.row());


	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case colTrack:
			return m_tracks[index.row()]->tag()->track();
		case colTitle:
			return TStringToQString(m_tracks[index.row()]->tag()->title());
		case colArtist:
			return TStringToQString(m_tracks[index.row()]->tag()->artist());
		case colAlbum:
			return TStringToQString(m_tracks[index.row()]->tag()->album());
		case colGenre:
			return TStringToQString(m_tracks[index.row()]->tag()->genre());
		case colYear:
			return m_tracks[index.row()]->tag()->year();
		}
		break;
	case Qt::ToolTipRole:
		return QString("%1 %2").arg(index.row()).arg(index.column());
		break;
	}
	return QVariant();

}


QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical || role != Qt::DisplayRole)
		return QAbstractTableModel::headerData(section, orientation, role);

	switch(section)
	{
	case colTrack:
		return "#";
	case colTitle:
		return tr("Title");
	case colArtist:
		return tr("Artist");
	case colAlbum:
		return tr("Album");
	case colGenre:
		return tr("Genre");
	case colYear:
		return tr("Year");
	}

	Q_ASSERT(!"Should never reach it");
	return QVariant();
}
