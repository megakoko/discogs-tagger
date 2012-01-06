#pragma once

#include "ui_batchchangedialog.h"

class BatchChangeDialog : public QDialog, private Ui::BatchChangeDialog
{
	Q_OBJECT

public:
	explicit BatchChangeDialog(QWidget *parent = 0);

//	DiscogsAlbumModelFields::Field selectedField() const;
	QVariant enteredValue() const;


private:
	void init();

//	void addField(const QString& fieldName, DiscogsAlbumModelFields::Field field);

};
