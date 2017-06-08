#include "widget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTimeEdit>
#include <QProcess>
#include <QPushButton>
#include <QApplication>
#include <QDialogButtonBox>

Widget::Widget(const QStringList &photos, const QString &dir, const QString &clockPhoto, QWidget *parent) : QDialog(parent)
{
	QProcess p;
	p.start("exiv2", QStringList() << clockPhoto << "-K" << "Exif.Image.DateTime" << "-Pv", QProcess::ReadOnly);
	p.waitForFinished(-1);
	const QDateTime clockDate = QDateTime::fromString(p.readAllStandardOutput(), "yyyy:MM:dd HH:mm:ss\n");
	setWindowTitle(tr("Clock Photo Time Adjuster"));
	QVBoxLayout *layout = new QVBoxLayout;
	if (photos.count() == 1)
		layout->addWidget(new QLabel(tr("Adjusting dates for: <pre>%1</pre>").arg(photos.at(0))));
	else
		layout->addWidget(new QLabel(tr("Adjusting dates for %1 photos in: <pre>%2</pre>").arg(photos.count()).arg(dir)));
	QLabel *image = new QLabel;
	const QPixmap photo(clockPhoto);
	if (photo.height() > photo.width())
		image->setPixmap(photo.scaledToHeight(800, Qt::SmoothTransformation));
	else
		image->setPixmap(photo.scaledToWidth(800, Qt::SmoothTransformation));
	layout->addWidget(image);
	QDateTimeEdit *date = new QDateTimeEdit;
	date->setDisplayFormat("MMMM d, yyyy HH:mm:ss");
	date->setDateTime(clockDate);
	layout->addWidget(date);
	const QString defaultText = tr("Choose the date and time of the clock photo above.");
	QLabel *adjustment = new QLabel(defaultText);
	layout->addWidget(adjustment);
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
	buttons->button(QDialogButtonBox::Save)->setText(tr("&Adjust dates"));
	buttons->button(QDialogButtonBox::Save)->setEnabled(false);
	layout->addWidget(buttons);
	setLayout(layout);

	connect(date, &QDateTimeEdit::dateTimeChanged, this, [=]() {
		qint64 diff = date->dateTime().secsTo(clockDate);
		qint64 days = qAbs(diff) / 86400;
		QTime difference = QTime(0, 0).addSecs(qAbs(diff) % 86400);
		if (diff != 0) {
			adjustment->setText(QString("%1 %2 days, %3 hours, %4 minutes, %5 seconds").arg(diff < 0 ? tr("Add") : tr("Subtract")).arg(days).arg(difference.hour()).arg(difference.minute()).arg(difference.second()));
			buttons->button(QDialogButtonBox::Save)->setEnabled(true);
		} else {
			adjustment->setText(defaultText);
			buttons->button(QDialogButtonBox::Save)->setEnabled(false);
		}
	});

	connect(buttons, &QDialogButtonBox::accepted, this, [=]() {
		qint64 diff = clockDate.secsTo(date->dateTime());
		QString what;
		if (diff < 0)
			what += "-";
		diff = qAbs(diff);
		qint64 hours = diff / (60 * 60);
		diff = diff % (60 * 60);
		qint64 minutes = diff / 60;
		qint64 seconds = diff % 60;
		what += QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
		QProcess p;
		p.setWorkingDirectory(dir);
		p.start("exiv2", QStringList() << "-a" << what << photos);
		p.waitForFinished(-1);
		p.start("exiv2", QStringList() << "-T" << photos);
		p.waitForFinished(-1);
		close();
	});

	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);
}
