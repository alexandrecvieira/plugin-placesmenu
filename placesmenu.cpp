/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Alexandre C Vieira <acamargo.vieira@gmail.com>
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
#include <libnotify/notify.h>

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
    volumeMonitor = g_volume_monitor_get();
    g_signal_connect(volumeMonitor, "volume-added", G_CALLBACK(onVolumeAdded), nullptr);
    g_signal_connect(volumeMonitor, "volume-removed", G_CALLBACK(onVolumeRemoved), nullptr);
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

void PlacesMenu::createSubmenu(QMenu* menu, GMount* mount)
{
    char* mountName = g_mount_get_name(mount);
    GIcon* gicon = g_mount_get_icon(mount);
    GFile* mountFile = g_mount_get_default_location(mount);
    QString mountPath = QString::fromUtf8(g_file_get_path(mountFile));
    QIcon icon = Fm::IconInfo::fromGIcon(gicon)->qicon();
    QMenu* subMenu = menu->addMenu(icon, mountName);
    createMenuItem(subMenu, tr("Open"), "folder", mountPath);
    createMenuItem(subMenu, tr("Eject removable media"), "media-eject");
}

void PlacesMenu::createMenuItem(QMenu* menu, const QString& name, const QString& iconName,  const QString& location)
{
    QAction* action = menu->addAction(XdgIcon::fromTheme(iconName), name);
    connect(action, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    mOpenDirectorySignalMapper->setMapping(action, location);    
}

void PlacesMenu::createMenuItem(QMenu* menu,  const QString& name, const QString& iconName)
{
    QAction* action = menu->addAction(XdgIcon::fromTheme(iconName), name);
    // connect(action, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
    // mOpenDirectorySignalMapper->setMapping(action, location);    
}

void PlacesMenu::addActions(QMenu* menu)
{
    QString homeName = tr("Home");
    QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, homeName, "user-home", homeLocation);
       
    QString documentsName = tr("Documents");
    QString documentsLocation = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, documentsName, "folder-documents", documentsLocation);
     
    QString downloadName = tr("Downloads");
    QString downloadLocation = QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, downloadName, "folder-download", downloadLocation);
    
    QString musicName = tr("Music");
    QString musicLocation = QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, musicName, "folder-music", musicLocation);
    
    QString pictureName = tr("Pictures");
    QString pictureLocation = QStandardPaths::locate(QStandardPaths::PicturesLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, pictureName, "folder-picture", pictureLocation);
    
    QString videosName = tr("Videos");
    QString videosLocation = QStandardPaths::locate(QStandardPaths::MoviesLocation, QString(), QStandardPaths::LocateDirectory);
    createMenuItem(menu, videosName, "folder-videos", videosLocation);
    
    menu->addSeparator();

    // load bookmarks
    for(auto& item: bookmarks_->items()) {
     	QString bookmarkName = item->name();
	auto bookmarkPath = item->path().localPath();
	QString bookmarkLocation = QString::fromUtf8(bookmarkPath.get());
	bookmarkLocation += "/";
	if((bookmarkLocation != documentsLocation) && (bookmarkLocation != downloadLocation)
	   && (bookmarkLocation != musicLocation) && (bookmarkLocation != pictureLocation)
	   && (bookmarkLocation != videosLocation)){
	    createMenuItem(menu, bookmarkName, "folder", bookmarkLocation);
	}
    }

    // Mounted drives: populate mounted drives and connect
    int count = 0;
    GList* mountsList = g_volume_monitor_get_mounts(volumeMonitor);
    for(GList* l = mountsList; l; l = l->next) {
	GMount* mount = G_MOUNT(l->data);
	if (g_mount_can_unmount(mount))	{
	    count++;
	    if(count == 1)
		menu->addSeparator();
	    createSubmenu(menu, mount);
	}   
    }
}

void PlacesMenu::onVolumeAdded(GVolumeMonitor* /*monitor*/, GVolume* volume, PlacesMenu* /*pThis*/)
{
    char* volumeName = g_volume_get_name(volume);
    QString text = tr("The device <b><nobr>\"%1\"</nobr></b> is connected.").arg(volumeName);
    showMessage(text);
   
    g_free(volumeName);
}

void PlacesMenu::onVolumeRemoved(GVolumeMonitor* /*monitor*/, GVolume* volume, PlacesMenu* /*pThis*/)
{
    char* volumeName = g_volume_get_name(volume);
    QString text = tr("The device <b><nobr>\"%1\"</nobr></b> is removed.").arg(volumeName);
    showMessage(text);
    
    g_free(volumeName);
}

void PlacesMenu::showMessage(const QString& text)
{
    QString title = tr("Places Menu");
    notify_init(title.toUtf8().data());
    NotifyNotification* notification = notify_notification_new(text.toUtf8().data(), nullptr, nullptr);
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
    notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
    notify_notification_show(notification, NULL);

    g_object_unref(G_OBJECT(notification));

    notify_uninit();
}

