#include "batcheditdialog.h"


using namespace TrackModelFields;


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

	addField("Artist", Artist);
	addField("Title", Title);
	addField("Album", Album);
	addField("Genre", Genre);
	addField("Year", Year);
}


void BatchEditDialog::addField(const QString &fieldName, Field field)
{
	m_fieldToChange->addItem(fieldName, field);
}


Field BatchEditDialog::selectedField() const
{
	const QVariant& data = m_fieldToChange->itemData(m_fieldToChange->currentIndex());
	return static_cast<Field>(data.toInt());
}


QVariant BatchEditDialog::enteredValue() const
{
	return m_value->text();
}
