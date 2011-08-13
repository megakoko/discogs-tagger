#include "trackmodel.h"

#include <QDebug>
#include <QSize>
#include <QBrush>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>

using TagLib::FileRef;



TrackModel::TrackModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}

// TODO: proper model resetting.
void TrackModel::addTrack(const QString& filename)
{
	beginInsertRows(QModelIndex(),m_tracks.count(), m_tracks.count());
	m_tracks << QSharedPointer<FileRef>(new FileRef(filename.toUtf8().data()));
	m_tracksOriginal << QSharedPointer<FileRef>(new FileRef(filename.toUtf8().data()));
	endInsertRows();
}


void TrackModel::addTracks(const QStringList &filenames)
{
	using TagLib::FileRef;
	beginInsertRows(QModelIndex(),m_tracks.count(), m_tracks.count() + filenames.size() - 1);
	foreach(const QString& filename, filenames)
	{
		m_tracks << QSharedPointer<FileRef>(new FileRef(filename.toUtf8().data()));
		m_tracksOriginal << QSharedPointer<FileRef>(new FileRef(filename.toUtf8().data()));
	}
	endInsertRows();
}


void TrackModel::saveTracks() const
{
	// TODO: Check if we can save file.
	for(int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->save();
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

	const int row = index.row();
//	const int col = index.column();


	QColor diffColor(Qt::red);

	switch(role)
	{
//	case Qt::SizeHintRole:
//		if(index.column() == colTrack || index.column() == colYear)
//		{
//			const int spinWidth = 70;
//			QSize s = headerData(index.column(), Qt::Horizontal, role).toSize();
//			s.setWidth(s.width() + spinWidth);
//			s.setHeight(1);
//			qDebug() << index.column() << s;
//			return s;
//		}
//		break;
	case Qt::ForegroundRole:
		switch(index.column())
		{
		case colTrack:
			if(m_tracks[row]->tag()->track() == m_tracksOriginal[row]->tag()->track())
				return QVariant();
			return QBrush(diffColor);
		case colTitle:
			if(m_tracks[row]->tag()->title() == m_tracksOriginal[row]->tag()->title())
				return QVariant();
			return QBrush(diffColor);
		case colArtist:
			if(m_tracks[row]->tag()->artist() == m_tracksOriginal[row]->tag()->artist())
				return QVariant();
			return QBrush(diffColor);
		case colAlbum:
			if(m_tracks[row]->tag()->album() == m_tracksOriginal[row]->tag()->album())
				return QVariant();
			return QBrush(diffColor);
		case colGenre:
			if(m_tracks[row]->tag()->genre() == m_tracksOriginal[row]->tag()->genre())
				return QVariant();
			return QBrush(diffColor);
		case colYear:
			if(m_tracks[row]->tag()->year() == m_tracksOriginal[row]->tag()->year())
				return QVariant();
			return QBrush(diffColor);
		}


		break;
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column())
		{
		case colTrack:
			return m_tracks[row]->tag()->track();
		case colTitle:
			return TStringToQString(m_tracks[row]->tag()->title());
		case colArtist:
			return TStringToQString(m_tracks[row]->tag()->artist());
		case colAlbum:
			return TStringToQString(m_tracks[row]->tag()->album());
		case colGenre:
			return TStringToQString(m_tracks[row]->tag()->genre());
		case colYear:
			return m_tracks[row]->tag()->year();
		}
		break;

	case Qt::ToolTipRole:
	{
		QString toolTip = "<b>%1</b><br />%2";
		TagLib::FileName filename = m_tracks[row]->file()->name();

		switch(index.column())
		{
		case colTrack:
			if(m_tracks[row]->tag()->track() == m_tracksOriginal[row]->tag()->track())
				return QVariant();
			return toolTip.arg(m_tracksOriginal[row]->tag()->track()).arg(filename);
		case colTitle:
			if(m_tracks[row]->tag()->title() == m_tracksOriginal[row]->tag()->title())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->title())).arg(filename);
		case colArtist:
			if(m_tracks[row]->tag()->artist() == m_tracksOriginal[row]->tag()->artist())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->artist())).arg(filename);
		case colAlbum:
			if(m_tracks[row]->tag()->album() == m_tracksOriginal[row]->tag()->album())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->album())).arg(filename);
		case colGenre:
			if(m_tracks[row]->tag()->genre() == m_tracksOriginal[row]->tag()->genre())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->genre())).arg(filename);
		case colYear:
			if(m_tracks[row]->tag()->year() == m_tracksOriginal[row]->tag()->year())
				return QVariant();
			return toolTip.arg(m_tracksOriginal[row]->tag()->year()).arg(filename);
		}
		break;
	}	// case Qt::ToolTipRole END

	}	// switch END
	return QVariant();

}


QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role == Qt::SizeHintRole)
		qDebug() << QAbstractTableModel::headerData(section, orientation, role);

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



Qt::ItemFlags TrackModel::flags(const QModelIndex &) const
{
	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid())
		return false;

	if(role == 2)
	{
		// TODO: maybe check QVariant to int or string convertion?
		const int row = index.row();
		switch(index.column())
		{
		case colTrack:
			m_tracks[row]->tag()->setTrack(value.toInt());
			return true;
		case colTitle:
			m_tracks[row]->tag()->setTitle(value.toString().toStdString());
			return true;
		case colArtist:
			m_tracks[row]->tag()->setArtist(value.toString().toStdString());
			return true;
		case colAlbum:
			m_tracks[row]->tag()->setAlbum(value.toString().toStdString());
			return true;
		case colGenre:
			m_tracks[row]->tag()->setGenre(value.toString().toStdString());
			return true;
		case colYear:
			m_tracks[row]->tag()->setYear(value.toInt());
			return true;
		}
	}
	return false;
}
