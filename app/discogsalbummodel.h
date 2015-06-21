#pragma once

#include <QAbstractTableModel>
#include "track.h"

class QDomElement;


class DiscogsAlbumModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Field {
		Position,
		Title,
		Artist,
		Album,
		Genre,
		Year,
		LastItem
	};

	explicit DiscogsAlbumModel(QObject* parent = 0);

	void setAlbum(const QString& discogsResponse);
	QList<Track> trackList() const;

	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;

public slots:
	void moveUp(const QModelIndexList& items);
	void moveDown(const QModelIndexList& items);
	void removeItem(const QModelIndexList& items);
	void joinItems(const QModelIndexList& items);

private:
	QStringList trackArtists(const QDomElement& track) const;
	QStringList getGenres(const QDomElement& genre) const;

	QString moveTheArticleToStart(const QString& text) const;

	QString m_albumArtists;

	QList<Track> m_tracks;

};
