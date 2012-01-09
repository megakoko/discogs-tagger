#pragma once

#include "ui_discogsviewer.h"
#include "track.h"

class QDomElement;

class QNetworkReply;
class QNetworkAccessManager;

class QModelIndex;
class DiscogsAlbumListModel;
class DiscogsAlbumModel;



class DiscogsViewer : public QWidget, Ui::DiscogsViewer
{
    Q_OBJECT

public:
    explicit DiscogsViewer(QWidget *parent = 0);

	void search(const QString& input);

	void restoreState(const QByteArray& state);
	QByteArray saveState() const;

	QList<Track> trackList() const;

public slots:
	void moveUp();
	void moveDown();
	void join();
	void remove();

private slots:
    void replyFinished(QNetworkReply*);
	void albumDoubleClicked(const QModelIndex& index);

private:
	void loadAlbumList(const QString& text);
	void loadAlbum(int releaseNumber);

    void handleXML(QString &str);

	QString handleTrack(const QDomElement& track) const;

	QModelIndexList selectedIndexes() const;

    QNetworkAccessManager* manager;

	DiscogsAlbumListModel* m_listModel;
	DiscogsAlbumModel* m_tableModel;

	static const QString albumListRequest;
	static const QString albumRequest;

signals:
	void statusChanged(const QString& status, int timeout = 5000);

};

