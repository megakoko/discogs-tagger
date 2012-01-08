#include "track.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>


void Track::joinWith(const Track &t2)
{
	static const QString joinText = " + ";

	QRegExp regexp("^\\d+");
	if(regexp.indexIn(this->position) >= 0)
	{
		const QString& possibleNumber = regexp.capturedTexts().last();
		if(t2.position.startsWith(possibleNumber))
			this->position = possibleNumber;
	}
	if(t2.artist != this->artist)
		this->artist += joinText + t2.artist;
	if(t2.title != this->title)
		this->title  += joinText + t2.title;
}
