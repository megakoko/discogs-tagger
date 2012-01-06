#pragma once
#include <QString>

struct Track
{
	Track(const QString& position, const QString& artist, const QString& title,
		  const QString& album, const QString& genre, const int year)
		: position(position), artist(artist), title(title)
		, album(album), genre(genre), year(year)
	{								}

	QString position;
	QString artist;
	QString title;
	QString album;
	QString genre;
	int year;

	void joinWith(const Track& t2);
};
