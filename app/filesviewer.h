#pragma once

#include "ui_filesviewer.h"
#include "track.h"


class TrackModel;
class QDir;


class FilesViewer : public QWidget, private Ui::FilesViewer
{
	Q_OBJECT
public:
	explicit FilesViewer(QWidget* parent = 0);

	void importTags(const QList<Track>& tracks);

	QString startDir() const;
	void setStartDir(const QString& dir);

	QByteArray horizontalHeaderState() const;
	void setHorizontalHeaderState(const QByteArray& state);

public slots:
	void addFiles();
	void clear();
	void save();
	void batchEdit();

protected:
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);

private:
	void init();

	void findFiles(const QDir& dir, QStringList& files);

	TrackModel* m_model;

	// Start directory for 'Open File/Directory' dialog.
	QString m_startDir;

signals:
	void sendMessage(const QString& message, int timeout = 5000);

};
