#pragma once
#include <QString>

struct Track
{
	Track(const QString& position, const QString& artist, const QString& title)
		: position(position), artist(artist), title(title)
	{								}

	QString position;
	QString artist;
	QString title;

	void joinWith(const Track& t2);
};

