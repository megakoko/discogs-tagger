/*
	Based on a LineEdit from:
	http://labs.qt.nokia.com/2007/06/06/lineedit-with-a-clear-button
*/


#include "searchedit.h"
#include <QToolButton>
#include <QStyle>

SearchEdit::SearchEdit(QWidget *parent)
    : QLineEdit(parent)
	, clearButton(new QToolButton(this))
{
	clearButton->setIcon(QIcon(":/icons/search"));
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("border: none; padding: 0px;");
    clearButton->show();

    const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    const int paddingRight = clearButton->sizeHint().width() + frameWidth + 1;
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(paddingRight));

	connect(clearButton, SIGNAL(clicked()), this, SIGNAL(returnPressed()));
}

void SearchEdit::resizeEvent(QResizeEvent *)
{
    const QSize sz = clearButton->sizeHint();
    const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}
