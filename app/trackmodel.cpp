#include "trackmodel.h"

#include <QDebug>
#include <QSize>
#include <QBrush>
#include <QApplication>
#include <QFontMetrics>

#include <taglib/tag.h>
#include <taglib/toolkit/tstring.h>

using TagLib::FileRef;


TrackModel::TrackModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}


void TrackModel::addTracks(const QStringList &filenames)
{
	using TagLib::FileRef;

	int countOfAddedTracks = 0;
	foreach(const QString& filename, filenames)
	{
		QSharedPointer<FileRef> track (new FileRef(filename.toUtf8().data()));
		if(!track->isNull())
		{
			++countOfAddedTracks;
			m_tracks << track;
			m_tracksOriginal << QSharedPointer<FileRef>(new FileRef(filename.toUtf8().data()));
		}
	}
	beginInsertRows(QModelIndex(), m_tracks.count(), m_tracks.count() + countOfAddedTracks - 1);
	endInsertRows();
}


bool TrackModel::saveTracks()
{
	bool savedAllTracks = true;

	beginResetModel();
	for(int i = 0; i < m_tracks.size(); ++i)
	{
		bool saved = m_tracks[i]->save();
		if(saved)
			TagLib::Tag::duplicate(m_tracks[i]->tag(), m_tracksOriginal[i]->tag());
		else
			savedAllTracks = false;
	}
	endResetModel();

	return savedAllTracks;
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



// It seems TagLib developers forgot about this operator.
inline bool operator !=(const TagLib::String& left, const TagLib::String& right) {
	return !(left == right);
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(m_tracks.count() > index.row());

	const int col = index.column();
	static const QBrush diffBrush = QBrush(QColor(Qt::red));

	const TagLib::Tag* const tag = m_tracks[index.row()]->tag();
	const TagLib::Tag* const tagOriginal = m_tracksOriginal[index.row()]->tag();

	QVariant result;
	using namespace TrackModelFields;
	switch(role)
	{
	case Qt::SizeHintRole:
		if(col == Position || col == Year)
		{
			const int spinWidth = 40;
			const int textWidth = qApp->fontMetrics().width(data(index).toString());
			result = QSize(textWidth + spinWidth, 0);
		}
		break;
	case Qt::ForegroundRole:
		switch(col)
		{
		case Position:
			if(tag->track() != tagOriginal->track())
				result = diffBrush;
			break;
		case Title:
			if(tag->title() != tagOriginal->title())
				result = diffBrush;
			break;
		case Artist:
			if(tag->artist() != tagOriginal->artist())
				result = diffBrush;
			break;
		case Album:
			if(tag->album() != tagOriginal->album())
				result = diffBrush;
			break;
		case Genre:
			if(tag->genre() != tagOriginal->genre())
				result = diffBrush;
			break;
		case Year:
			if(tag->year() != tagOriginal->year())
				result = diffBrush;
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
		break;

	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(col)
		{
		case Position:
			result = tag->track();
			break;
		case Title:
			result = TStringToQString(tag->title());
			break;
		case Artist:
			result = TStringToQString(tag->artist());
			break;
		case Album:
			result = TStringToQString(tag->album());
			break;
		case Genre:
			result = TStringToQString(tag->genre());
			break;
		case Year:
			result = tag->year();
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
		break;

	case Qt::ToolTipRole:
	{
		QString toolTip = tr("New value:\t%1\nOriginal value:\t%2");
		switch(col)
		{
		case Position:
			if(tag->track() != tagOriginal->track())
				result = toolTip.arg(tag->track()).arg(tagOriginal->track());
			break;
		case Title:
			if(tag->title() != tagOriginal->title())
			{
				result = toolTip.arg(TStringToQString(tag->title())).
						 arg(TStringToQString(tagOriginal->title()));
			}
			break;
		case Artist:
			if(tag->artist() != tagOriginal->artist())
			{
				result = toolTip.arg(TStringToQString(tag->artist())).
						 arg(TStringToQString(tagOriginal->artist()));
			}
			break;
		case Album:
			if(tag->album() != tagOriginal->album())
			{
				result = toolTip.arg(TStringToQString(tag->album())).
						 arg(TStringToQString(tagOriginal->album()));
			}
			break;
		case Genre:
			if(tag->genre() != tagOriginal->genre())
			{
				result = toolTip.arg(TStringToQString(tag->genre())).
						 arg(TStringToQString(tagOriginal->genre()));
			}
			break;
		case Year:
			if(tag->year() != tagOriginal->year())
				result = toolTip.arg(tag->year()).arg(tagOriginal->year());
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
		break;
	}	// End of "case Qt::ToolTipRole"

	}	// End of switch.


	return result;
}


QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant result;

	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		using namespace TrackModelFields;
		switch(section)
		{
		case Position:
			result = "#";
			break;
		case Title:
			result = tr("Title");
			break;
		case Artist:
			result = tr("Artist");
			break;
		case Album:
			result = tr("Album");
			break;
		case Genre:
			result = tr("Genre");
			break;
		case Year:
			result = tr("Year");
			break;
		default:
			qCritical() << "Unhandled option in" << __FILE__ << __FUNCTION__;
		}
	}
	else
		result = QAbstractTableModel::headerData(section, orientation, role);

	return result;
}



Qt::ItemFlags TrackModel::flags(const QModelIndex &) const
{
	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool dataWasUpdated = false;

	if(role == Qt::EditRole && index.isValid())
	{
		TagLib::Tag* tag = m_tracks[index.row()]->tag();
		using namespace TrackModelFields;
		switch(index.column())
		{
		case Position:
			tag->setTrack(value.toInt());
			dataWasUpdated = true;
			break;
		case Title:
			tag->setTitle(value.toString().toStdString());
			dataWasUpdated = true;
			break;
		case Artist:
			tag->setArtist(value.toString().toStdString());
			dataWasUpdated = true;
			break;
		case Album:
			tag->setAlbum(value.toString().toStdString());
			dataWasUpdated = true;
			break;
		case Genre:
			tag->setGenre(value.toString().toStdString());
			dataWasUpdated = true;
			break;
		case Year:
			tag->setYear(value.toInt());
			dataWasUpdated = true;
			break;
		}
	}

	return dataWasUpdated;
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
