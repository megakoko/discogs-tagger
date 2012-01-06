#pragma once

#include "ui_batcheditdialog.h"

class BatchEditDialog : public QDialog, private Ui::BatchEditDialog
{
	Q_OBJECT

public:
	explicit BatchEditDialog(QWidget *parent = 0);

//	DiscogsAlbumModelFields::Field selectedField() const;
	QVariant enteredValue() const;


private:
	void init();

//	void addField(const QString& fieldName, DiscogsAlbumModelFields::Field field);

};
