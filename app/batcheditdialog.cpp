#include "batcheditdialog.h"


BatchEditDialog::BatchEditDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

	init();
	resize(width(), 0);
}


void BatchEditDialog::init()
{
	connect(m_ok, SIGNAL(clicked()), SLOT(accept()));
	connect(m_cancel, SIGNAL(clicked()), SLOT(reject()));

	using namespace TrackModelFields;
	addField("Artist", Artist);
	addField("Title", Title);
	addField("Album", Album);
	addField("Genre", Genre);
	addField("Year", Year);
}


void BatchEditDialog::addField(const QString &fieldName, TrackModelFields::Field field)
{
	m_fieldToChange->addItem(fieldName, field);
}


TrackModelFields::Field BatchEditDialog::selectedField() const
{
	const QVariant& data = m_fieldToChange->itemData(m_fieldToChange->currentIndex());
	return static_cast<TrackModelFields::Field>(data.toInt());
}


QVariant BatchEditDialog::enteredValue() const
{
	return m_value->text();
}
