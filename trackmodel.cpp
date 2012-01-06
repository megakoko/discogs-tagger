#include "trackmodel.h"

#include <QDebug>
#include <QSize>
#include <QBrush>
#include <QApplication>
#include <QFontMetrics>

//#include <taglib/fileref.h>
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


bool TrackModel::saveTracks()
{
	bool result = true;

	beginResetModel();
	for(int i = 0; i < m_tracks.size(); ++i)
	{
		bool success = m_tracks[i]->save();
		if(success)
			TagLib::Tag::duplicate(m_tracks[i]->tag(), m_tracksOriginal[i]->tag());
		else
			result = false;
	}
	endResetModel();
	return result;
}


void TrackModel::importTags(const QList<Track>& tracks)
{
	const int maxIndex = qMin(tracks.count(), m_tracks.count());
	for(int i = 0; i < maxIndex; ++i)
	{
		TagLib::Tag* tag = m_tracks[i]->tag();
		tag->setAlbum(tracks[i].album.toStdString());
		tag->setTitle(tracks[i].title.toStdString());
		tag->setArtist(tracks[i].artist.toStdString());
		tag->setGenre(tracks[i].genre.toStdString());
		tag->setYear(tracks[i].year);
		tag->setTrack(i+1);
//		TagLib::Tag::duplicate(tag, m_tracksOriginal[i]->tag());
	}
}


void TrackModel::clear()
{
	beginResetModel();
	m_tracks.clear();
	m_tracksOriginal.clear();
	endResetModel();
}


int TrackModel::columnCount(const QModelIndex &) const
{
	return TrackModelFields::LastElement;
}


int TrackModel::rowCount(const QModelIndex &) const
{
	return m_tracks.count();
}


QVariant TrackModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(m_tracks.count() > index.row());

	const int row = index.row();
	const int col = index.column();


	QColor diffColor(Qt::red);

	using namespace TrackModelFields;
	switch(role)
	{
	case Qt::SizeHintRole:
		if(col == Position || col == Year)
		{
			const int spinWidth = 40;
			const int textWidth = qApp->fontMetrics().width(data(index).toString());
			return QSize(textWidth + spinWidth, 0);
		}
		break;
	case Qt::ForegroundRole:
		switch(col)
		{
		case Position:
			if(m_tracks[row]->tag()->track() == m_tracksOriginal[row]->tag()->track())
				return QVariant();
			return QBrush(diffColor);
		case Title:
			if(m_tracks[row]->tag()->title() == m_tracksOriginal[row]->tag()->title())
				return QVariant();
			return QBrush(diffColor);
		case Artist:
			if(m_tracks[row]->tag()->artist() == m_tracksOriginal[row]->tag()->artist())
				return QVariant();
			return QBrush(diffColor);
		case Album:
			if(m_tracks[row]->tag()->album() == m_tracksOriginal[row]->tag()->album())
				return QVariant();
			return QBrush(diffColor);
		case Genre:
			if(m_tracks[row]->tag()->genre() == m_tracksOriginal[row]->tag()->genre())
				return QVariant();
			return QBrush(diffColor);
		case Year:
			if(m_tracks[row]->tag()->year() == m_tracksOriginal[row]->tag()->year())
				return QVariant();
			return QBrush(diffColor);
		}


		break;
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(col)
		{
		case Position:
			return m_tracks[row]->tag()->track();
		case Title:
			return TStringToQString(m_tracks[row]->tag()->title());
		case Artist:
			return TStringToQString(m_tracks[row]->tag()->artist());
		case Album:
			return TStringToQString(m_tracks[row]->tag()->album());
		case Genre:
			return TStringToQString(m_tracks[row]->tag()->genre());
		case Year:
			return m_tracks[row]->tag()->year();
		}
		break;

	case Qt::ToolTipRole:
	{
		QString toolTip = tr("Original value:") + "\n%1";
//		TagLib::FileName filename = m_tracks[row]->file()->name();

		// TODO: we don't need no file name here.
		switch(col)
		{
		case Position:
			if(m_tracks[row]->tag()->track() == m_tracksOriginal[row]->tag()->track())
				return QVariant();
			return toolTip.arg(m_tracksOriginal[row]->tag()->track());
		case Title:
			if(m_tracks[row]->tag()->title() == m_tracksOriginal[row]->tag()->title())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->title()));
		case Artist:
			if(m_tracks[row]->tag()->artist() == m_tracksOriginal[row]->tag()->artist())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->artist()));
		case Album:
			if(m_tracks[row]->tag()->album() == m_tracksOriginal[row]->tag()->album())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->album()));
		case Genre:
			if(m_tracks[row]->tag()->genre() == m_tracksOriginal[row]->tag()->genre())
				return QVariant();
			return toolTip.arg(TStringToQString(m_tracksOriginal[row]->tag()->genre()));
		case Year:
			if(m_tracks[row]->tag()->year() == m_tracksOriginal[row]->tag()->year())
				return QVariant();
			return toolTip.arg(m_tracksOriginal[row]->tag()->year());
		}
		break;
	}	// case Qt::ToolTipRole END

	}	// switch END
	return QVariant();
}


QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical || role != Qt::DisplayRole)
		return QAbstractTableModel::headerData(section, orientation, role);

	using namespace TrackModelFields;
	switch(section)
	{
	case Position:
		return "#";
	case Title:
		return tr("Title");
	case Artist:
		return tr("Artist");
	case Album:
		return tr("Album");
	case Genre:
		return tr("Genre");
	case Year:
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
		using namespace TrackModelFields;
		switch(index.column())
		{
		case Position:
			m_tracks[row]->tag()->setTrack(value.toInt());
			return true;
		case Title:
			m_tracks[row]->tag()->setTitle(value.toString().toStdString());
			return true;
		case Artist:
			m_tracks[row]->tag()->setArtist(value.toString().toStdString());
			return true;
		case Album:
			m_tracks[row]->tag()->setAlbum(value.toString().toStdString());
			return true;
		case Genre:
			m_tracks[row]->tag()->setGenre(value.toString().toStdString());
			return true;
		case Year:
			m_tracks[row]->tag()->setYear(value.toInt());
			return true;
		}
	}
	return false;
}


void TrackModel::batchEdit(const QModelIndexList& indexes, TrackModelFields::Field field,
						   const QVariant& value)
{
	for(int i = 0; i < indexes.count(); ++i)
	{
		TagLib::Tag* tag = m_tracks[indexes.at(i).row()]->tag();
		using namespace TrackModelFields;
		switch(field)
		{
		case Position:
			tag->setTrack(value.toInt());
			break;
		case Title:
			tag->setTitle(value.toString().toStdString());
			break;
		case Artist:
			tag->setArtist(value.toString().toStdString());
			break;
		case Album:
			tag->setAlbum(value.toString().toStdString());
			break;
		case Genre:
			tag->setGenre(value.toString().toStdString());
			break;
		case Year:
			tag->setYear(value.toInt());
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}
}
