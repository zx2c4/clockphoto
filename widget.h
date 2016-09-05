#ifndef WIDGET_H
#define WIDGET_H

#include <QDialog>

class Widget : public QDialog
{
	Q_OBJECT

public:
	Widget(const QStringList &photos, const QString &dir, const QString &clockPhoto, QWidget *parent = 0);
};

#endif // WIDGET_H
