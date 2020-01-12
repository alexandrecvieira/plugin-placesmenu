/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Daniel Drzisga <sersmicro@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is diinstributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "placesmenu.h"
#include <QStandardPaths>
#include <QStyle>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QIcon>
#include <XdgIcon>

PlacesMenu::PlacesMenu(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mMenu(0),
    mBaseDirectory(QDir::homePath()),
    mDefaultIcon(XdgIcon::fromTheme("folder")),
    bookmarks_{Fm::Bookmarks::globalInstance()}
{
    // ensure that glib integration of Qt is not turned off
    // This fixes #168: https://github.com/lxqt/pcmanfm-qt/issues/168
    qunsetenv("QT_NO_GLIB");

    mOpenDirectorySignalMapper = new QSignalMapper(this);
    mMenuSignalMapper = new QSignalMapper(this);

    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mButton.setText(QString(tr("Places")));
    
    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    connect(mOpenDirectorySignalMapper, SIGNAL(mapped(QString)), this, SLOT(openDirectory(QString)));
}

PlacesMenu::~PlacesMenu()
{
    if(mMenu)
    {
        delete mMenu;
        mMenu = 0;
    }
}

void PlacesMenu::showMenu()
{
    buildMenu();
    
    willShowWindow(mMenu);
    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // Solution is to execute menu 1ms later using timer
    mMenu->popup(calculatePopupWindowPos(mMenu->sizeHint()).topLeft());
}

void PlacesMenu::buildMenu()
{
    if(mMenu)
    {
        delete mMenu;
        mMenu = 0;
    }

    mMenu = new QMenu();

    addActions(mMenu);
}

void PlacesMenu::openDirectory(const QString& path)
{
    QDesktopServices::openUrl(QUrl("file://" + QDir::toNativeSeparators(path)));
}

void PlacesMenu::addActions(QMenu* menu)
{
    QString homeName = tr("Home");
    QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionHome = menu->addAction(XdgIcon::fromTheme("user-home"), homeName);
    connect(openDirectoryActionHome, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionHome, homeLocation);
    
    QString documentsName = tr("Documents");
    QString documentsLocation = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionDocument = menu->addAction(XdgIcon::fromTheme("folder-documents"), documentsName);
    connect(openDirectoryActionDocument, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionDocument, documentsLocation);

    QString downloadName = tr("Downloads");
    QString downloadLocation = QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionDownload = menu->addAction(XdgIcon::fromTheme("folder-download"), downloadName);
    connect(openDirectoryActionDownload, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionDownload, downloadLocation);
    
    QString musicName = tr("Music");
    QString musicLocation = QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionMusic = menu->addAction(XdgIcon::fromTheme("folder-music"), musicName);
    connect(openDirectoryActionMusic, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionMusic, musicLocation);
    
    QString pictureName = tr("Pictures");
    QString pictureLocation = QStandardPaths::locate(QStandardPaths::PicturesLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionPicture = menu->addAction(XdgIcon::fromTheme("folder-picture"), pictureName);
    connect(openDirectoryActionPicture, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionPicture, pictureLocation);
    
    QString videosName = tr("Videos");
    QString videosLocation = QStandardPaths::locate(QStandardPaths::MoviesLocation, QString(), QStandardPaths::LocateDirectory);
    QAction* openDirectoryActionVideos = menu->addAction(XdgIcon::fromTheme("folder-videos"), videosName);
    connect(openDirectoryActionVideos, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(openDirectoryActionVideos, videosLocation);

    menu->addSeparator();

    for(auto& item: bookmarks_->items()) {
     	QString bookmarkName = item->name();
	auto bookmarkPath = item->path().localPath();
	QString bookmarkLocation = QString::fromUtf8(bookmarkPath.get());
	bookmarkLocation += "/";
	qDebug() << "1. " + bookmarkLocation;
	qDebug() << "2. " + documentsLocation;
       	if((bookmarkLocation != documentsLocation) && (bookmarkLocation != downloadLocation)
	   && (bookmarkLocation != musicLocation) && (bookmarkLocation != pictureLocation)
	   && (bookmarkLocation != videosLocation)){
	    QAction* bookmarkAction = menu->addAction(XdgIcon::fromTheme("folder"), bookmarkName);
	    connect(bookmarkAction, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
	    mOpenDirectorySignalMapper->setMapping(bookmarkAction, bookmarkLocation);
	}
    }
}

