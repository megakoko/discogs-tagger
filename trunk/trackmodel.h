#pragma once

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QStringList>

namespace TagLib{
class FileRef;
}

typedef QSharedPointer<TagLib::FileRef> Track;


class TrackModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrackModel(QObject *parent = 0);

	// Overloaded methods.
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	// Specific methods.
	void addTrack(const QString& filename);
	void addTracks(const QStringList& filenames);
signals:

public slots:

private:
	QList<QSharedPointer<TagLib::FileRef> > m_tracks;

};
