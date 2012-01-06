#pragma once

#include <QAbstractTableModel>
#include "structs.h"
#include "discogsalbummodelfields.h"

class QDomElement;


class DiscogsAlbumModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit DiscogsAlbumModel(QObject* parent = 0);

	void setAlbum(const QString& discogsResponse);
	QList<Track> trackList() const;
	const QString& album() const;
	int year() const;
	const QString& genre() const;

	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;

public slots:
	void moveUp(const QModelIndex& item);
	void moveDown(const QModelIndex& item);
	void removeItem(const QModelIndex& item);
	void joinItems(const QModelIndexList& list);
	void changeItems(const QModelIndexList& list, DiscogsAlbumModelFields::Field column,
					 const QVariant& value);

private:
	QStringList trackArtists(const QDomElement& track) const;
	QStringList genres(const QDomElement& genre) const;

	QString moveTheArticleToStart(const QString& text) const;

	QString m_album;
	QString m_albumArtists;
	int m_year;
	QString m_genres;

	QList<Track> m_tracks;

};
