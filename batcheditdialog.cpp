#include "batcheditdialog.h"


BatchEditDialog::BatchEditDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

	init();
}


void BatchEditDialog::init()
{
	connect(m_ok, SIGNAL(clicked()), SLOT(accept()));
	connect(m_cancel, SIGNAL(clicked()), SLOT(reject()));

//	addField("Artist", Artist);
//	addField("Title", Title);
//    addField("Album", DiscogsAlbumModel::Album);
}


//void BatchChangeDialog::addField(const QString &fieldName, Field field)
//{
//	m_fieldToChange->addItem(fieldName, field);
//}


//Field BatchChangeDialog::selectedField() const
//{
//	const QVariant& data = m_fieldToChange->itemData(m_fieldToChange->currentIndex());
//	return static_cast<Field>(data.toInt());
//}


QVariant BatchEditDialog::enteredValue() const
{
	return m_value->text();
}
