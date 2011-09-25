#pragma once


struct Track
{
	Track(const QString& position, const QString& artist, const QString& title)
		: position(position), artist(artist), title(title)
	{								}

	QString position;
	QString artist;
	QString title;

	void joinWith(const Track& t2)
	{
		static const QString joinText = " + ";

		if(t2.artist != this->artist)
			this->artist += joinText + t2.artist;
		if(t2.title != this->title)
			this->title  += joinText + t2.title;
	}
};

