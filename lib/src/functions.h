#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QMap>
#include <QStringList>
#include <QDateTime>
#include <QFont>
#include <QSettings>
#include <QDomElement>
#include <algorithm>
#include <QString>
#include <QNetworkReply>
#include <QSslError>
#include "models/favorite.h"
#include "logger.h"


// Filesize units
#if defined(Q_OS_WIN)
	// 1 KB = 1024 B
	#define FILESIZE_UNITS QStringList() << QObject::tr("B") << QObject::tr("KB") << QObject::tr("MB") << QObject::tr("GB");
	#define FILESIZE_MULTIPLIER 1024
#elif defined(Q_OS_MACOS)
	// 1 KB = 1000 B
	#define FILESIZE_UNITS QStringList() << QObject::tr("B") << QObject::tr("KB") << QObject::tr("MB") << QObject::tr("GB");
	#define FILESIZE_MULTIPLIER 1000
#else
	// 1 KiB = 1024 B
	#define FILESIZE_UNITS QStringList() << QObject::tr("B") << QObject::tr("KiB") << QObject::tr("MiB") << QObject::tr("GiB");
	#define FILESIZE_MULTIPLIER 1024
#endif



void		sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors);
QDateTime	qDateTimeFromString(QString s);
QString		savePath(QString s = "", bool exists = false);
int			levenshtein(QString, QString);
QString		stripTags(QString);
QString		getUnit(float *size);
QString		formatFilesize(float size);
QString		getExtension(QUrl url);
QString		getExtension(QString url);
QString		setExtension(QString url, QString extension);

bool		setFileCreationDate(QString path, QDateTime time);
void		shutDown(int timeout = 0);
void		openTray();

QString fixFilename(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);
QString fixFilenameWindows(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);
QString fixFilenameLinux(QString filename, QString path = "", int maxlength = 0, bool invalidChars = true);

QMap<QString,QString>		domToMap(QDomElement);

QMap<QString,QStringList>	getCustoms(QSettings *settings);
QMap<QString,QPair<QString,QString>>	getFilenames(QSettings *settings);

QStringList removeWildards(QStringList elements, QStringList remove);



template <typename T>
QList<T> reversed(const QList<T> & in)
{
	QList<T> result;
	std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
	return result;
}
inline bool sortByNote(const Favorite &s1, const Favorite &s2)
{ return s1.getNote() < s2.getNote(); }
inline bool sortByName(const Favorite &s1, const Favorite &s2)
{ return s1.getName().toLower() < s2.getName().toLower(); }
inline bool sortByLastviewed(const Favorite &s1, const Favorite &s2)
{ return s1.getLastViewed() < s2.getLastViewed(); }

#endif // FUNCTIONS_H
