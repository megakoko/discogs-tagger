#pragma once

#include <QAbstractListModel>

class DiscogsAlbumListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DiscogsAlbumListModel(QObject* parent = 0);

	QVariant data(const QModelIndex& index, int role) const;
	void setAlbums(const QString& discogsResponse);
	int rowCount(const QModelIndex& parent) const;

	static int releaseNumberFromUrl(const QString& url);
	static const int NO_RELEASE_NUMBER;
signals:

public slots:

private:
	struct Album {
		Album(const QString& title, const QString& url)
			: title(title), url(url)
		{						}

		QString title;
		QString url;
	};

	QList<Album> m_albums;
};
