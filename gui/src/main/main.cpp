/**
 * @file
 * @author  Bionus <bio.nus@hotmail.fr>
 *
 * @section LICENSE
 *
 * Copyright 2011 - 2012 Bionus
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * Batch downloader and viewer for boorus/imageboards.
 */



#include <QApplication>
#include <QtGlobal>
#include <QSettings>
#include "downloader/downloader.h"
#include "models/site.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	#include <QCommandLineParser>
#else
	#include <vendor/qcommandlineparser.h>
#endif
#if !USE_CLI
	#include <QtGui>
	#include "ui/QAffiche.h"
	#include "ui/QBouton.h"
	#include "viewer/zoomwindow.h"
	#include "settings/optionswindow.h"
	#if USE_BREAKPAD
		#include "crashhandler/crashhandler.h"
	#endif
#endif



void noMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	Q_UNUSED(type);
	Q_UNUSED(context);
	Q_UNUSED(message);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("Grabber");
	app.setApplicationDisplayName("Grabber");
	app.setApplicationVersion(VERSION);
	app.setOrganizationName("Bionus");
	app.setOrganizationDomain("bionus.fr.cr");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	#if !USE_CLI
		QCommandLineOption cliOption(QStringList() << "c" << "cli", "Disable the GUI.");
		parser.addOption(cliOption);
	#endif
	QCommandLineOption tagsOption(QStringList() << "t" << "tags", "Tags to search for.", "tags");
	QCommandLineOption sourceOption(QStringList() << "s" << "sources", "Source websites.", "sources");
	QCommandLineOption pageOption(QStringList() << "p" << "page", "Starting page.", "page", "1");
	QCommandLineOption limitOption(QStringList() << "m" << "max", "Maximum of returned images.", "count");
	QCommandLineOption perpageOption(QStringList() << "i" << "perpage", "Number of images per page.", "count", "20");
	QCommandLineOption pathOption(QStringList() << "l" << "location", "Location to save the results.", "path");
	QCommandLineOption filenameOption(QStringList() << "f" << "filename", "Filename to save the results.", "filename");
	QCommandLineOption userOption(QStringList() << "u" << "user", "Username to connect to the source.", "user");
	QCommandLineOption passwordOption(QStringList() << "w" << "password", "Password to connect to the source.", "password");
	QCommandLineOption blacklistOption(QStringList() << "b" << "blacklist", "Download blacklisted images.");
	QCommandLineOption postfilteringOption(QStringList() << "r" << "postfilter", "Filter results.", "filter");
	QCommandLineOption noDuplicatesOption(QStringList() << "n" << "no-duplicates", "Remove duplicates from results.");
	QCommandLineOption verboseOption(QStringList() << "d" << "debug", "Show debug messages.");
	QCommandLineOption tagsMinOption(QStringList() << "tm" << "tags-min", "Minimum count for tags to be returned.", "count", "0");
	QCommandLineOption tagsFormatOption(QStringList() << "tf" << "tags-format", "Format for returning tags.", "format", "%tag\t%count\t%type");
	parser.addOption(tagsOption);
	parser.addOption(sourceOption);
	parser.addOption(pageOption);
	parser.addOption(limitOption);
	parser.addOption(perpageOption);
	parser.addOption(pathOption);
	parser.addOption(filenameOption);
	parser.addOption(userOption);
	parser.addOption(passwordOption);
	parser.addOption(blacklistOption);
	parser.addOption(postfilteringOption);
	parser.addOption(tagsMinOption);
	parser.addOption(tagsFormatOption);
	parser.addOption(noDuplicatesOption);
	parser.addOption(verboseOption);
	QCommandLineOption returnCountOption(QStringList() << "rc" << "return-count", "Return total image count.");
	QCommandLineOption returnTagsOption(QStringList() << "rt" << "return-tags", "Return tags for a search.");
	QCommandLineOption returnPureTagsOption(QStringList() << "rp" << "return-pure-tags", "Return tags.");
	QCommandLineOption returnImagesOption(QStringList() << "ri" << "return-images", "Return images url.");
	QCommandLineOption downloadOption(QStringList() << "download", "Download found images.");
	parser.addOption(returnCountOption);
	parser.addOption(returnTagsOption);
	parser.addOption(returnPureTagsOption);
	parser.addOption(returnImagesOption);
	parser.addOption(downloadOption);

	parser.process(app);

	bool gui = false;
	#if !USE_CLI
		gui = !parser.isSet(cliOption);
	#endif

	if (!gui && !parser.isSet(verboseOption))
		qInstallMessageHandler(noMessageOutput);

	#if USE_BREAKPAD && !USE_CLI
		if (gui)
		{
			QDir dir = QFileInfo(argv[0]).dir();
			QString crashes = savePath("crashes");
			if (!dir.exists(crashes))
			{ dir.mkpath(crashes); }
			CrashHandler::instance()->Init(crashes);
		}
	#endif

	Profile *profile = new Profile(savePath());

	if (!gui)
	{
		Downloader *dwnldr = new Downloader(profile,
											parser.value(tagsOption).split(" ", QString::SkipEmptyParts),
											parser.value(postfilteringOption).split(" ", QString::SkipEmptyParts),
											Site::getSites(profile, parser.value(sourceOption).split(" ", QString::SkipEmptyParts)),
											parser.value(pageOption).toInt(),
											parser.value(limitOption).toInt(),
											parser.value(perpageOption).toInt(),
											parser.value(pathOption),
											parser.value(filenameOption),
											parser.value(userOption),
											parser.value(passwordOption),
											parser.isSet(blacklistOption),
											profile->getSettings()->value("blacklistedtags").toString().split(' '),
											parser.isSet(noDuplicatesOption),
											parser.value(tagsMinOption).toInt(),
											parser.value(tagsFormatOption));

		if (parser.isSet(returnCountOption))
			dwnldr->getPageCount();
		if (parser.isSet(returnTagsOption))
			dwnldr->getPageTags();
		if (parser.isSet(returnPureTagsOption))
			dwnldr->getTags();
		if (parser.isSet(returnImagesOption))
			dwnldr->getUrls();
		if (parser.isSet(downloadOption))
			dwnldr->getImages();

		dwnldr->setQuit(true);
		QObject::connect(dwnldr, SIGNAL(quit()), qApp, SLOT(quit()));
	}
	#if !USE_CLI
		else
		{
			QMap<QString, QString> params;
			params.insert("booru", parser.value(sourceOption));
			params.insert("limit", parser.value(limitOption));
			params.insert("page", parser.value(pageOption));
			params.insert("path", parser.value(pathOption));
			params.insert("filename", parser.value(filenameOption));
			params.insert("user", parser.value(userOption));
			params.insert("password", parser.value(passwordOption));
			params.insert("ignore", parser.isSet(blacklistOption) ? "true" : "false");
			params.insert("tags", parser.value(tagsOption));

			mainWindow *mainwindow = new mainWindow(profile);
			mainwindow->init(parser.positionalArguments(), params);
			mainwindow->show();
		}
	#endif

	return app.exec();
}
