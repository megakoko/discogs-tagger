#pragma once

#include <QLineEdit>


class QToolButton;

class SearchEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchEdit(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);

private:
    QToolButton *clearButton;

};
