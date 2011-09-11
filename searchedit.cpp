/*
	Based on a LineEdit from:
	http://labs.qt.nokia.com/2007/06/06/lineedit-with-a-clear-button
*/


#include "searchedit.h"
#include <QToolButton>
#include <QStyle>

SearchEdit::SearchEdit(QWidget *parent)
    : QLineEdit(parent)
{
    clearButton = new QToolButton(this);

	clearButton->setIcon(QIcon(":/icons/search"));
//	clearButton->setIcon(QIcon("faenza-icons/zoom-fit.png"));

    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->show();
    connect(clearButton, SIGNAL(clicked()), this, SIGNAL(returnPressed()));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").
    		arg(clearButton->sizeHint().width() + frameWidth + 1));
//    QSize msz = minimumSizeHint();
//    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
//                   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void SearchEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}
