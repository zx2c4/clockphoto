#include "widget.h"
#include <QApplication>
#include <QFile>
#include <QDir>

static void usage(const char *progname)
{
	fprintf(stderr, "Usage: %s CLOCK_PHOTO [PHOTO_DIRECTORY]\n\nThis program adds or subtracts a time offset from the EXIF metadata of\nphotos in PHOTO_DIRECTORY based on a photo of a clock in CLOCK_PHOTO. If\nPHOTO_DIRECTORY is unspecified, it is assumed to be the enclosing\ndirectory of CLOCK_PHOTO. It then sets the file mtime to the timestamp.\n\nCopyright (C) Jason A. Donenfeld <jason@zx2c4.com>. All Rights Reserved.\nSee www.zx2c4.com for more information.\n", progname);
	exit(1);
}

int main(int argc, char *argv[])
{
	QDir dir;
	if (argc != 2 && argc != 3)
		usage(argv[0]);
	if (!QFile::exists(argv[1]))
		usage(argv[0]);
	if (argc == 3)
		dir = QDir(argv[2]);
	else
		dir = QFileInfo(argv[1]).dir();
	if (!dir.exists())
		usage(argv[0]);
	const QStringList photos = dir.entryList(QStringList() << "*.jpg", QDir::Files | QDir::Readable | QDir::Writable, QDir::Name);
	const QString clockPhoto = argv[1];
	if (photos.isEmpty())
		usage(argv[0]);

	QApplication a(argc, argv);
	Widget w(photos, dir.absolutePath(), clockPhoto);
	w.show();
	w.setFixedSize(w.size());
	return a.exec();
}
