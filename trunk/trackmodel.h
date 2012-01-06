#pragma once

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QStringList>

#include <taglib/fileref.h>

#include "structs.h"

//typedef QSharedPointer<TagLib::FileRef> Track;


class TrackModel : public QAbstractTableModel
{
    Q_OBJECT
public:
	enum {
		colTrack,
		colTitle,
		colArtist,
		colAlbum,
		colGenre,
		colYear,
		colLastElement
	};

    explicit TrackModel(QObject *parent = 0);

	// Overloaded methods.
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	// Specific methods.
	void addTrack(const QString& filename);
	void addTracks(const QStringList& filenames);
	void importTags(const QList<Track>& tracks);

public slots:
	bool saveTracks();

	void clear();

private:
	QList<QSharedPointer<TagLib::FileRef> > m_tracks;

	QList<QSharedPointer<TagLib::FileRef> > m_tracksOriginal;

};
